#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include<algorithm>

namespace kuroda {

class XorshiftInterface {
public:
   virtual uint64_t get_nextnode(uint64_t node,uint8_t c)const = 0;
   virtual uint64_t set(uint64_t node,uint8_t c) = 0;
};

class Xorshift : public XorshiftInterface{
public:
static constexpr int64_t invalid = -1;
//static constexpr uint64_t default_size = 1ull << 10;
static constexpr uint64_t default_size = 1ull << 4;
Xorshift(){
	pc_.resize(default_size);
	exists.resize(default_size,false);
    exists[0] = true;//0番目は使わない
}

private:
//std::vector<int> B_ = {13,-7,5};//ビットシフトパターン
//std::vector<int> B_1 = {5,-7,13};//ビットシフトパターン(逆関数用)
std::vector<int> B_ = {5,-9,7};//ビットシフトパターン
std::vector<int> B_1 = {7,-9,5};//ビットシフトパターン(逆関数用)
//先行研究のビットシフトパターン
//{26,-5,6}{7,-30,1}{7,-6}
uint64_t hash_use = 1;//配列P,C の使用数

public:
struct DataItem {
    //int p,c;
    uint8_t p;
    int c;
    DataItem(): p(invalid),c(invalid){}
};
std::vector<bool> exists;//空判定配列
std::vector<DataItem> pc_ ;//P,C配列
int k = (std::log(default_size)/std::log(2)) + 8;//mask値の決定のため、P, C 拡張時にインクリメント
int replace_time = 0;

private:
uint64_t expand(uint64_t node){
    //std::cout << "before table" << pc_.size() << "  " << k << std::endl;
    //display();
	//出力値を遷移先候補とパリティ値から復元する
    std::vector<DataItem> pc_2(2*pc_.size());//P,C配列
    std::vector<bool> exists2(2*pc_.size());//空判定配列
    std::vector<int64_t> place(pc_.size(),invalid);
    
    place[0] = 0;
    exists2[0] = true;//0番目は使わない
    /////////////////////////////////////////////////////ファイルサイズが大きいとここがバグる
    for(uint64_t i = 1;i < pc_.size();i++){
        if (!exists[i] or replace(i,place,pc_2,exists2) != invalid){
            continue;
        }
    }
    k++;//最終的に、マスク＋１に更新する
    pc_ = std::move(pc_2);
    exists = std::move(exists2);
    //display1();
    return place[node];
} 

//格納されている要素にアクセスし、経路をたどって親から再配置する
int64_t replace(uint64_t node,std::vector<int64_t>& place,std::vector<DataItem>& pc_2,std::vector<bool>& exists2){
    if (place[node] != invalid){//再配置済みの時、無視
        return invalid;
    }
    uint64_t seed = get_seed(node);
    int c = seed % 256;
    int parent = seed >> 8;
    if (place[parent] == invalid){//前のトライ上で、再配置が終わっていないとき
        replace(parent,place,pc_2,exists2);
    }  
    seed = (place[parent] << 8 ) + c;
    k++;//mask更新
    uint64_t x1 = xos(seed);
    uint64_t new_node = x1 >> 8;
    int collision = 0;
    while(exists2[new_node]){//使用済みならば再Xos
        x1 = xos(x1);//出力値をxorに再代入s
        new_node = x1 >> 8;
        collision++;
    } 
    if(collision > 50){
        std::cout << "   " << collision <<  std::endl;
    }
    uint8_t parity = x1 % 256;
    pc_2[new_node].p = parity;
    pc_2[new_node].c = collision;
    exists2[new_node] = true;
    place[node] = new_node;
    k--;
    return 0;
}

uint64_t get_seed(uint64_t t)const{//配列番号、パリティ値、衝突回数からシード値を得る
    uint64_t x = 0;
    if(pc_[t].c == 0){
        x = (t << 8) + pc_[t].p;
    }
    else{
        x = (t << 8) + pc_[t].p;
        for(int i = 0;i < pc_[t].c;i++){//衝突回数回ixosに代入することで、元のシード値をえる
            x = ixos(x);//仮の出力値を得る(再xosする前の)
        }
    }
    uint64_t seed = ixos(x);
    return seed;
}//ここから、親と遷移文字が分かる


private:
uint64_t xos(uint64_t x)const{//前&x
    uint64_t maskXos_ = 1ull << k;
	for(int b: B_){
		if(b >= 0){
			x = (x ^ (x << b)) % maskXos_;
		}
		else if(b < 0){
			x = (x ^ (x >> -b)) % maskXos_;
        }
	}
	return x;
}

int bit_len(int x)const{
    assert(x > 0);
    int n = 0;
    while ((1 << n) <= x-1){
        ++n;
    }
    return n;
}

int loop_time(int k,int b)const{
	assert(k > 0 and b > 0);
    return 1 << bit_len((k-1) / b + 1);
}

uint64_t ixos(uint64_t x)const{//前シード値から出力
    uint64_t maskXos_ = 1ull << k;
	//xorの回数は2^ceil(log2(k/b))
	for(int b: B_1){
		int n = loop_time(k, abs(b)) - 1;
		for(int i = 0;i < n;i++){
			if(b >= 0){
				x = (x ^ (x << b)) % maskXos_;
			}
			else if(b < 0){
				x = (x ^ (x >> -b)) % maskXos_;
			}
		}
	}
	return x;
}

uint64_t create_seed(uint64_t node, uint8_t c)const{
    return (node << 8) + c;
}

public:
//配列P,Cに要素を格納、衝突が起これば再配置
uint64_t set(uint64_t node,uint8_t c){//引数 : シード値
    //keyによる探索の期待計算量が、負荷率をqとしてO(1/(1-q))になる
    if(hash_use * 2 >= pc_.size()){
        replace_time++;
        node = expand(node);
    }
    uint64_t seed = create_seed(node,c);
    //std::cout << node <<  "    " << c << std::endl;
    uint64_t x1 = xos(seed);
    uint64_t t = x1 >> 8;//遷移先候補 8桁目以降 
    int collision = 0;
    while(exists[t]){//使用済みならば再Xos
        x1 = xos(x1);//出力値をxorに再代入
		t = x1 >> 8;
        collision++;
    }
    uint8_t parity = x1 % 256;
    pc_[t].p = parity;
    pc_[t].c = collision;
    exists[t] = true;
	hash_use++;
    return node;
}

uint64_t get_nextnode(uint64_t node,uint8_t c)const{//引数シード値
    uint64_t seed = create_seed(node,c);
	uint64_t x1 = xos(seed);
    uint64_t t = x1 >> 8;//遷移先
    uint8_t parity = x1 % 256;
    int collision = 0;
    while(exists[t]){//使用済みならば再Xos
        if(pc_[t].p == parity && pc_[t].c == collision){
            return t;
        }
        x1 = xos(x1);//
        t = x1 >> 8;//遷移先
        parity = x1 % 256;
        collision++;
    }
	return invalid;
}


};

}
