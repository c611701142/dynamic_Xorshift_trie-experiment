#ifndef OPEN_ADRESS__HASHING_HPP_
#define OPEN_ADRESS__HASHING_HPP_
#include <iostream>
#include <vector>
#include <unordered_map>
#include <cassert>

namespace kuroda {

class HashTableInterface {
public:
  virtual int64_t get(int64_t key)const = 0;
  virtual void set(int64_t key, int64_t value) = 0;
};

class HashTable : public HashTableInterface{
// TODO: Needs implementation by yourself.

public:
//ハッシュテーブルの要素数の初期値
static constexpr int dell = -2;//データが入っていたがそれが削除されたことを示す値
static constexpr int64_t invalid = -1;
static constexpr int64_t default_size = 1ull << 6;
int64_t k = (std::log(default_size)/std::log(2));//mask値の決定のため、P, C 拡張時にインクリメント
HashTable(){
	hashArray.resize(default_size);
	exists.resize(default_size,false);
    exists[0] = true;//0番目は使わない
}

private://クラスメンバ変数
int64_t hash_use = 0;//ハッシュテーブルの要素の使用数
/* DataItemの定義 */
struct DataItem {
    int key,value;
    DataItem(): key(invalid),value(invalid){}
};
std::vector<DataItem> hashArray;//ハッシュテーブル
std::vector<bool> exists;//空判定配列

/* ハッシュ関数*/
int64_t hashCode(int64_t key)const{
    int64_t maskXos_ = 1ull << k;
    return (std::hash<int64_t>()(key)) % maskXos_;
}

public:
int64_t get(int64_t key)const{
    //get the hash
    
    int64_t hashIndex = hashCode(key);
    while (exists[hashIndex]){
        if (hashArray[hashIndex].key == key)
            return hashArray[hashIndex].value;
        hashIndex++;
        //wrap around the table
        hashIndex = hashCode(hashIndex);
    }
    return invalid;//ここでは初期値が返る
}
//ハッシュテーブルの中身を
/* 挿入のための関数 */
//ハッシュ値の衝突が発生した場合は、再ハッシュを繰り返して、
//「空状態」バケットを調べていき、空いているバケットを発見したらデータを格納します。
void set(int64_t key, int64_t value){
    //std::cout << "before_expand" << load_factor << "%" << std::endl;

    //keyによる探索の期待計算量が、負荷率をqとしてO(1/(1-q))になる
    if(hash_use * 2 >= hashArray.size()){
        //std::cout << "ex_bagin " << std::endl;
        k++;
        expand_resize();
        //std::cout << "ex_end " << std::endl;
    }
    //get the hash
    int64_t hashIndex = hashCode(key);
    //move in array until an empty or deleted cell
    int collision = 0;//衝突回数 のちにXorshiftで使うかも
    while(exists[hashIndex]){
        //go to next cell
        ++hashIndex;
        ++collision;
        //wrap around the table
        hashIndex = hashCode(hashIndex);
        //std::cout << "key = " << key << std::endl;
        //std::cout << "collision = " << collision << std::endl;
    }
    //std::cout << "hashnumber = " << hashIndex << std::endl;
    collision = 0;
    hashArray[hashIndex].value = value;
    hashArray[hashIndex].key = key;
    exists[hashIndex] = true;//使用済みにする
    hash_use++;
    //std::cout << "   " << hashIndex << std::endl;
}

private:
void expand_resize(){
    std::vector<DataItem> hashArray2(2*hashArray.size());//P,C配列
    std::vector<bool> exists2(2*exists.size());//空判定配列
    exists2[0] = true;//0番目は使わない
    for(int64_t i = 1;i < hashArray.size();i++){
        if(exists[i]){//使用要素にアクセス
            int64_t key = hashArray[i].key;
            int64_t value = hashArray[i].value;
            int64_t hashIndex = hashCode(key);
            //std::cout << "h = " << hashIndex << std::endl;
            //std::cout << "k = " << k << std::endl;
            while(exists2[hashIndex]){
                ++hashIndex;
                //wrap around the table
                hashIndex = hashCode(hashIndex);
            }
            hashArray2[hashIndex].key = key;
            hashArray2[hashIndex].value = value;
            exists2[hashIndex] = true;
        }
    }
    hashArray = std::move(hashArray2);
    exists = std::move(exists2);
}
    
};

}
#endif //OPEN_ADRESS__HASHING_HP