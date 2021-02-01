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
//std::unordered_map<int64_t,int32_t> ht;
static constexpr uint8_t kLeafChar = 0;
int64_t create_key(int32_t node, uint8_t c) const {
    //assert(node < (1u<<24));
    return (int64_t(node) << 8) + c;
}

//int32_t node_count = 0;

public:
double c_averave  = 0;
int32_t c_zero = 0;
int32_t node_count = 0;
int C_MAX = 0;

bool contains(const std::string& str)const{//文字列strが辞書にあるかどうか検索
    int32_t node = 0; // root
    for (uint8_t c : str) {
        int32_t new_node = ht.get(create_key(node, c));
        //auto next_node = ht.find(create_key(node,c));
        if(new_node == HashTable::invalid){
        //if(next_node !=  ht.end()){
            std::cout << str << "\n";
            return false;
        }
        //node = next_node->second;
        node = new_node;
    }
    //return ht.find(create_key(node,kLeafChar)) !=  ht.end();
    return ht.get(create_key(node,kLeafChar)) !=  HashTable::invalid;
}

void insert(const std::string& str){ // 文字列strを辞書に追加
    int32_t node = 0;
    for (uint8_t c : str) {
        //std::cout << c << "\n";
        int32_t new_node_it = ht.get(create_key(node,c));
        //auto next_node_it = ht.find(create_key(node,c));
        //if(new_node !=  HashTable::invalid){
        //if(next_node_it !=  ht.end()){
        if(new_node_it !=  HashTable::invalid){
            node = new_node_it;
        }
        else{
             //setする(new_nodeの登場)
            node_count++;
            auto new_node = node_count;
            int64_t key = create_key(node,c);
            int32_t value = new_node;
            ht.set(key,value);
            //ht[key] = value;
            //ht.insert({key,value});
            node = new_node;
        }
    }
    node_count++;
    ht.set(create_key(node,kLeafChar),node_count);//終端文字の遷移を格納
    //ht.insert(create_key(node,kLeafChar),node_count);
    //ht[create_key(node,kLeafChar)] = node_count;
    c_averave = (double)ht.collision_sum/(double)ht.hashArray.size();
    c_zero = ht.collision_zero;
    C_MAX = ht.c_max;
   // ht.display();

}
};

}

#endif //OPEN_ADRESS__HASHING_HPP_
