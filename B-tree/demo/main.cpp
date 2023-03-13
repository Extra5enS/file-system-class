#include <stdio.h>
#include <sys/time.h>
#include <limits.h>
#include <map>

#include "../include/b-tree.h"

#define TEST_ARRAY_SIZE (1l << 15)//(1l << 22)

void print(struct kv_pair* kv) {
    printf("%ld -> %ld \n", kv -> key, kv -> value);
}

int main() {
    btree btr;
    btree_init(&btr, 2);
    printf("insert key's from 1 to 20 with value = 10 * key\n"); 
    for(int64_t i = 0; i <= 20; ++i) {
        btree_insert(&btr, i, 10*i); 
    }
    printf("----------\n");   
    btree_foreach(&btr, print);
    printf("----------\n");   
    printf("change value's with key from 5 to 10 with value = key\n");
    for(int64_t i = 5; i <= 10; ++i) {
        btree_insert(&btr, i, i);
    }
    printf("----------\n");   
    btree_foreach(&btr, print);
    printf("----------\n");   
    printf("delete elements with keys from 0 to 10\n");
    for(int64_t i = 0; i <= 10; ++i) {
        btree_erase(&btr, i);
    }
    printf("----------\n");   
    btree_foreach(&btr, print);
    printf("----------\n");   
    printf("merge test\n");
    int64_t mmt1[5] = {1, 3, 5, 7, 9};/*,
                         11, 13, 15, 17, 19, 
                         21, 23, 25, 27, 29, 
                         31, 33, 35, 37, 39};*/
    int64_t mmt2[5] = {0, 2, 4, 6, 8};/*, 
                         10, 12, 14, 16, 18, 
                         20, 22, 24, 26, 28, 
                         30, 32, 34, 36, 38};*/
    btree mt1, mt2;
    btree_init(&mt1, 2);
    btree_init(&mt2, 2);
    for(int i = 0; i < 5; ++i) {
       btree_insert(&mt1, mmt1[i], i); 
       btree_insert(&mt2, mmt2[i], i); 
    }
    btree merged = btree_merge(&mt1, &mt2);
    printf("----------\n");   
    printf("\n1 three\n");
    btree_foreach(&mt1, print);
    printf("\n2 three\n");
    btree_foreach(&mt2, print);
    printf("\nmerged\n");
    btree_foreach(&merged, print);
    printf("----------\n");   
	
	btree_free(&btr);
    btree_free(&mt1);
    btree_free(&mt2);
    btree_free(&merged);
    
    return 0;
}

