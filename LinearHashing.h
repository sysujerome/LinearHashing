#include <stdint.h>
#include <iostream>
#include <unistd.h>
#include <memory.h>
#include <vector>
#include <cmath>

#define TABLE_SIZE 4 // adjustable
#define HASH_SIZE  16 // adjustable
#define FILE_SIZE 1024 * 1024 * 16 // 16 MB adjustable

using namespace std;

// typedef struct metadata {
//     size_t size;            // the size of whole hash table array 
//     size_t level;           // level of hash
//     uint64_t next;          // the index of the next split hash table
//     uint64_t overflow_num;  // amount of overflow hash tables 
// } metadata;

// data entry of hash table
typedef struct entry {
    uint64_t key;
    uint64_t value;
} entry;

// hash table
typedef struct lh_slot {
    // entry kv_arr[TABLE_SIZE];   // data entry array of hash table
    vector<entry> kv_arr;
    uint64_t fill_num;          // amount of occupied slots in kv_arr
    uint64_t next_offset;       // the file address of overflow hash table 
} pm_table;

// persistent memory linear hash
class LinearHash {
private:
    // void* start_addr;      // the start address of mapped file
    // void* overflow_addr;   // the start address of overflow table array
    // metadata* meta;        // virtual address of metadata
    // pm_table* table_arr;   // virtual address of hash table array
    vector<lh_slot> table;
    vector<entry> overflow;
    uint64_t level;
    uint64_t next;


    void split();
    uint64_t hashFunc(const uint64_t &key);
    pm_table* newOverflowTable(uint64_t &offset);

public:
    LinearHash() = delete;
    LinearHash(uint64_t table_size);
    ~LinearHash();

    int insert(const uint64_t &key, const uint64_t &value);

    void show();
    uint64_t getLevel();
    uint64_t getNext();
    int search(const uint64_t &key, uint64_t &value);
    int remove(const uint64_t &key);
    int update(const uint64_t &key, const uint64_t &value);
};