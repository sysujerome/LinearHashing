#include "LinearHashing.h"
int main() {
    LinearHash hash(HASH_SIZE);
    uint64_t size ;
    std::cin >> size;
    for (uint64_t i = 1; i <= size; i++) {
        hash.insert(i, i);
    }


    for (uint64_t i = 1; i <= size; i++) {
        uint64_t val;
        // if (hash.search(i, val)) cout << "key value\t" << i << " " << val << std::endl;
        // else std::cout << "not found!!!!!!!!!!!!!!\t" << i << std::endl;
        if (!hash.search(i, val)) {
            std::cout << "not found!!!!!!!!!!!!!!\t" << i << std::endl;
            exit(-1);
        }
    }
    hash.show();


    // for (uint64_t i = 1; i <= size; i++) {
    //     if (!hash.remove(i)) {
    //         std::cout << "remove error !!!!!!!!!!!!!!\t" << i << std::endl;
    //         exit(-1);
    //     }
    // }


    for (uint64_t i = 1; i <= size; i++) {
        if (!hash.update(i, 10000+i)) {
            std::cout << "update error !!!!!!!!!!!!!!\t" << i << std::endl;
            exit(-1);
        }
    }
    
    std::cout << "level" << "\t" << "next" << std::endl;
    std::cout << hash.getLevel() << "\t" << hash.getNext() << std::endl;
    hash.show();
    return 0;
}