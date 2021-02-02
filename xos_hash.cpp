#include <iostream>
#include<vector>
#include<algorithm>
#include <math.h>
#include <string>
#include <fstream>
#include <sstream>
static uint64_t size = 1ull << 5;
static int k = 5;
using namespace std;
std::vector<int> B_ = {5,-9,7};//ビットシフトパターン

uint64_t xos(uint64_t x){//前&x
    uint64_t maskXos_ = 1ull << k;
	for(int b: B_){
		if(b >= 0){
			x = (x ^ (x << b)) % maskXos_;
			//x = (x ^ (x << b));
		}
		else if(b < 0){
			x = (x ^ (x >> -b)) % maskXos_;
			//x = (x ^ (x >> -b));
        }
	}
	return x;
}

uint64_t hash_os(uint64_t x){
    uint64_t maskXos_ = 1ull << k;
    return hash<uint64_t>()(x) % maskXos_;
}



int main(){
	vector<uint64_t> a1(0);
	vector<uint64_t> b1(0);
	for(uint64_t i = 1; i <= size;i++){
		uint64_t a = xos(i);
		uint64_t b = hash_os(i);
		a1.push_back(a);
		b1.push_back(b);
	}
	sort(a1.begin(),a1.end());
	sort(b1.begin(),b1.end());
    ofstream log;
    log.open("xos.csv",ios::trunc);
    for(int i = 0; i < size;i++){
        log << a1[i];
        log << "\n";
    }
    log.close(); // これで保存終了
    log.open("hash.csv",ios::trunc);
    for(int i = 0; i < size;i++){
        log << b1[i];
        log << "\n";
    }
    log.close(); // これで保存終了



	return 0;
}


