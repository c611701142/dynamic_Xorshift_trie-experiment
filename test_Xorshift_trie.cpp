#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <random>
#include <unistd.h>

#include "Xorshift_trie.hpp"
#include "hash_trie.hpp"
//#include "dynamic_doublle_array.hpp"

namespace {

class Stopwatch {
  using hrc = std::chrono::high_resolution_clock;
  hrc::time_point start_;
 public:
  Stopwatch() : start_(hrc::now()) {}
  auto time_process() const {
    return hrc::now() - start_;
  }
  double get_sec() const {
    return std::chrono::duration<double>(time_process()).count();
  }
  double get_milli_sec() const {
    return std::chrono::duration<double, std::milli>(time_process()).count();
  }
  double get_micro_sec() const {
    return std::chrono::duration<double, std::micro>(time_process()).count();
  }
};

template <class Process>
double milli_sec_in(Process process) {
  Stopwatch sw;
  process();
  return sw.get_milli_sec();
}

}
std::vector<std::string> time_keysets;
std::vector<std::string> str_list;
void key_set(){
    //std::string input_name = "s2.txt";
    std::string input_name = "wordnet-3.0-word";
    //std::string input_name = "ipadic-word";
    //std::string input_name = "jawiki-20150118.line";
    //std::string input_name = "enwiki-20150205.line";
    std::ifstream ifs(input_name);
    if (!ifs) {
        std::cerr<<"File not found input file: "<< input_name <<std::endl;
        exit(0);
    }
    for (std::string s; std::getline(ifs, s);){
        str_list.push_back(s);
    }
    std::cout << "data : " << input_name << std::endl;
}

// キー検索速度を計測するためのkeysetsの作成
void MakeTimeKeysets(int size) {
    std::random_device rnd;
    time_keysets.clear();
    for(int i=0; i < 100000; i++) {
        int v = rnd() % size;
        time_keysets.push_back(str_list[v]);
    }
}

inline uint64_t get_process_size() {
    FILE* fp = std::fopen("/proc/self/statm", "r");
    uint64_t dummy(0), vm(0);
    std::fscanf(fp, "%ld %ld ", &dummy, &vm);  // get resident (see procfs)
    std::fclose(fp);
    return vm * ::getpagesize();
}

int main(int argc, char* argv[]){
    key_set();
    /*
    uint64_t size_begin = get_process_size();
    kuroda::xorshift_trie xorshift_try;//Xorshiftトライ呼び出し
    std::cout << "------experiment_start---------- " <<std::endl;
    std::cout << "------Xorshift---------- " <<std::endl;
    Stopwatch sw;
    for(const std::string& str : str_list) {//配列の単語数
        xorshift_try.xor_try(str);//1単語ずつ追加(str)
    }
    auto time = sw.get_milli_sec();
    auto memory = get_process_size() - size_begin;
    std::cout << "constract time : " << time << "[ms]" << std::endl;
    std::cout << "node : " << xorshift_try.node_count << std::endl;
    std::cout << "replace_time : " << xorshift_try.re_take << std::endl;
    std::cout << "mask : " << xorshift_try.mask << std::endl;
    std::cout << "memory : " << memory << "[bytes]" << std::endl;
    xorshift_try.display();
    double time_sum = 0.0;
    for(int i=0; i < 10; i++) {
      //std::cout << " keisoku " << i << std::endl;
      MakeTimeKeysets(str_list.size());
      Stopwatch search_time;
      int n = 0;
      for(const std::string& str : time_keysets) {//配列の単語数
          n++;
          //bool check = xorshift_try.contains_all(str_list, n);
          bool check = xorshift_try.contains(str);
          if(check == false){
              std::cout << "failed..." << n << std::endl;
              exit(0);//プログラム異常終了
          }
          else{
              //std::cout << "OK" << std::endl;
          }
          
      }
      time = search_time.get_milli_sec();
      time_sum += time;
    }
    std::cout << "search_time_averave : " << time_sum/10.0 << "[ms]" << std::endl;
    std::cout << "1 key search_time : " << time_sum/1000.0 << "[μs]" <<  std::endl;
    */
    
    std::cout << "------experiment_start---------- " <<std::endl;
    std::cout << "------hashtable---------- " <<std::endl;
    uint64_t size_begin = get_process_size();
    kuroda::hash_trie hash_try;//hashtableトライ呼び出し
    Stopwatch sw;
    for(const std::string& str : str_list) {//配列の単語数
        hash_try.insert(str);//1単語ずつ追加(str)
    }
    auto time = sw.get_milli_sec();
    auto memory = get_process_size() - size_begin;
    std::cout << "constract time : " << time << "[ms]" << std::endl;
    std::cout << "memory : " << memory << "[bytes]" << std::endl;
    double time_sum = 0.0;
    for(int i=0; i < 10; i++) {
      //std::cout << " keisoku " << i << std::endl;
      MakeTimeKeysets(str_list.size());
      Stopwatch search_time;
      for(const std::string& str : time_keysets) {//配列の単語数
          //bool check = xorshift_try.contains_all(str_list, n);
          bool check = hash_try.contains(str);
          if(check == false){
              std::cout << "failed..." << std::endl;
              exit(0);//プログラム異常終了
          }
          else{
              //std::cout << "OK" << std::endl;
          }
          
      }
      time = search_time.get_milli_sec();
      time_sum += time;
    }
    std::cout << "search_time_averave : " << time_sum/10.0 << "[ms]" << std::endl;
    std::cout << "1 key search_time : " << time_sum/1000.0 << "[μs]" <<  std::endl;
    return 0;//プログラム終了
}
