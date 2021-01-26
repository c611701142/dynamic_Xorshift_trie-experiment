#ifndef OPEN_ADRESS__HASH_TRIE_HPP_
#define OPEN_ADRESS__HASH_TRIE_HPP_
#include <iostream>
#include <vector>
#include <unordered_map>
#include <cassert>

#include "hashing.hpp"

namespace kuroda {

class hash_trie{
private:
kuroda::HashTable ht;
static constexpr uint8_t kLeafChar = 0;

int64_t create_key(int64_t node, uint8_t c) const {
    //assert(node < (1u<<24));
    return (node << 8) + c;
}

int64_t node_count = 0;

public:
double c_averave  = 0;
int64_t c_zero = 0;

bool contains(const std::string& str)const{//文字列strが辞書にあるかどうか検索
    int64_t node = 0; // root
    for (uint8_t c : str) {
        int64_t new_node = ht.get(create_key(node, c));
        if(new_node == HashTable::invalid){
            return false;
        }
        node = new_node;
    }
    return ht.get(create_key(node,kLeafChar)) !=  HashTable::invalid;
}
int64_t word = 0;
void insert(const std::string& str){ // 文字列strを辞書に追加
    int64_t node = 0;
    word++;
    if(word % 100000 == 0){
        //std::cout << str << word << "\n";
    }
    for (uint8_t c : str) {
        //std::cout << c << "\n";
        int64_t new_node = ht.get(create_key(node,c));
        if(new_node !=  HashTable::invalid){
            node = new_node;
        }
        else{
             //setする(new_nodeの登場)
            node_count++;
            new_node = node_count;
            int64_t key = create_key(node,c);
            int64_t value = new_node;
            ht.set(key,value);
            node = new_node;
        }

    }
    node_count++;
    ht.set(create_key(node,kLeafChar),node_count);//終端文字の遷移を格納
    c_averave = (double)ht.collision_sum/(double)ht.hashArray.size();
    c_zero = ht.collision_zero;
   // ht.display();

}
};

}

#endif //OPEN_ADRESS__HASHING_HPP_
