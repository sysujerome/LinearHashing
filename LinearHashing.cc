#include "LinearHashing.h"
/**
 * LinearHash::LinearHash 
 * 
 * @param  {char*} file_path : the file path of data file
 * if the data file exist, open it and recover the hash
 * if the data file does not exist, create it and initial the hash
 */
LinearHash::LinearHash(uint64_t table_size) {
    this->level = 0;
    this->next = 0;
    this->table.resize(table_size);


}
/**
 * LinearHash::~LinearHash 
 * 
 * unmap and close the data file
 */
LinearHash::~LinearHash() {
    // 这个函数用于关闭pmem_map打开的NVM文件
    // pmem_unmap(start_addr, FILE_SIZE);
    table.clear();
    overflow.clear();
}
/**
 * LinearHash 
 * 
 * split the hash table indexed by the meta->next
 * update the metadata
 */
void LinearHash::split() {
    // fill the split table
    // fill the new table
    int size = table.size();
    table.resize(size+1);
    int slot_index = 0;
    for (auto kv : table[next].kv_arr) {
        auto cur_key = kv.key;
        auto index = cur_key % uint64_t(pow(2, level+1) * HASH_SIZE);
        if (index == table.size()-1) table.back().kv_arr.push_back(kv);
        else table[index].kv_arr[slot_index++] = kv; 
    }
    table[next].kv_arr.resize(slot_index);
    ++next;
    if (next == (pow(2, level) * HASH_SIZE)) {  // 重新开始
        next = 0;
        ++level;
    }
    return;
}
/**
 * LinearHash 
 * 
 * @param  {uint64_t} key     : key
 * @param  {size_t} hash_size : the N in hash func: idx = hash % N
 * @return {uint64_t}         : index of hash table array
 * 
 * need to hash the key with proper hash function first
 * then calculate the index by N module
 */
uint64_t LinearHash::hashFunc(const uint64_t &key) {
    uint64_t pos = key % uint64_t(pow(2, level) * HASH_SIZE); // 取模公式
    if (pos < next) {
        pos = key % uint64_t(pow(2, level+1) * HASH_SIZE); // 该节点以及分裂过了
    }
    return pos;
    
}

uint64_t LinearHash::getLevel() {return this->level;}
uint64_t LinearHash::getNext() {return this->next;}

// /**
//  * LinearHash 
//  * 
//  * @param  {uint64_t} offset : the file address offset of the overflow hash table
//  *                             to the start of the whole file
//  * @return {lh_slot*}       : the virtual address of new overflow hash table
//  */
// lh_slot* LinearHash::newOverflowTable(uint64_t &offset) {
//     // 再偏移值后面新建一个和原数据表一样大小的数据表
//     auto addr = this->start_addr+offset
//     void* newBuf = reinterpret_cast<void*>(table_arr+size*sizeof(lh_slot));
//     auto newTable = new(newBuf) lh_slot[this->meta->size];
//     return addr;
// }

/**
 * LinearHash 
 * 
 * @param  {uint64_t} key   : inserted key
 * @param  {uint64_t} value : inserted value
 * @return {int}            : success: 0. fail: -1
 * 
 * insert the new kv pair in the hash
 * 
 * always insert the entry in the first empty slot
 * 
 * if the hash table is full then split is triggered
 */
int LinearHash::insert(const uint64_t &key, const uint64_t &value) {
    entry newEntry = {key, value};
    auto index = hashFunc(key);
    if (table[index].kv_arr.size() == TABLE_SIZE) { // 分裂
        split();
    }
    index = hashFunc(key);
    if (table[index].kv_arr.size() == TABLE_SIZE) {
        overflow.push_back(newEntry);
    }
    else {
        table[index].kv_arr.push_back(newEntry);
    }

    return 0;
}

/**
 * LinearHash 
 * 
 * @param  {uint64_t} key   : the searched key
 * @param  {uint64_t} value : return value if found
 * @return {int}            : 1 found, 0 not found, -1 error value
 * 
 * search the target entry and return the value
 */
int LinearHash::search(const uint64_t &key, uint64_t &value) {
    auto index = hashFunc(key);
    for (auto kv : table[index].kv_arr) {
        if (kv.key == key) {
            value = kv.value;
            return 1;
        }
    }
    // if (table[index].kv_arr.size() != TABLE_SIZE) return 0;
    for (auto kv : overflow) {
        if (kv.key == key) {
            value = kv.value;
            return 1;
        }
    }
    return 0;
}

/**
 * LinearHash 
 * 
 * @param  {uint64_t} key : target key
 * @return {int}          : success: 1. fail: 0
 * 
 * remove the target entry, move entries after forward
 * if the overflow table is empty, remove it from hash
 */
int LinearHash::remove(const uint64_t &key) {
    auto index = hashFunc(key);

    // using content = table[index].kv_arr;
    for (auto iter = table[index].kv_arr.begin(); iter != table[index].kv_arr.end(); ) {
        if (iter->key == key) {
            iter = table[index].kv_arr.erase(iter);
            return 1;
        }
        else ++iter;
    }
    for (auto iter = overflow.begin(); iter != overflow.end(); ++iter) {
        if (iter->key == key) {
            overflow.erase(iter);
            return 1;
        }
    }
    return 0;
}

/**
 * LinearHash 
 * 
 * @param  {uint64_t} key   : target key
 * @param  {uint64_t} value : new value
 * @return {int}            : success: 0. fail: -1
 * 
 * update an existing entry
 */
int LinearHash::update(const uint64_t &key, const uint64_t &value) {
    auto index = hashFunc(key);
    for (auto iter = table[index].kv_arr.begin(); iter != table[index].kv_arr.end(); ) {
        if (iter->key == key) {
            iter->value = value;

            return 1;
        }
        else ++iter;
    }
    for (auto iter = overflow.begin(); iter != overflow.end(); ++iter) {
        if (iter->key == key) {
            iter->value = value;
            return 1;
        }
    }
    insert(key, value);
    return 1;
}


// // vector<entry>& LinearHash::getSlotByIndex(int index) {
// //     return &(table[index]
// // }




void LinearHash::show() {
    std::cout << "The content in table:" << std::endl;
    for (auto slot : table) {
        for (auto kv : slot.kv_arr) {
            std::cout << kv.key << " " << kv.value << "\t";
        }
        std::cout << std::endl;
    }


    std::cout << "The content in overflow: " << std::endl;

    for (auto kv : overflow) {
        cout << kv.key << " " << kv.value << "\t";
    }
    std::cout << std::endl << std::endl;
    return;
}