#include <iostream>
#include <map>

#include "../include/b-tree.h"
#define MAX_TASK_ARRAY_SIZE (1 << 14) // for first time

#define MAX_KEY_ARRAY_SIZE (1 << 14)
#define	IN_TASK(n) ((n) % 3)
#define IS_INSERT 0
#define IS_ERASE 1
#define IS_SEARCH 2

int comper_stores(std::map<int64_t, int64_t> test_map, btree test_btree) {
	kv_pair kv;
	btree_iterator biter;
	btree_iterator_init(&biter, &test_btree);

	int res = 0;

	for(auto& map_pair : test_map) {
		int loc_res = btree_iterator_next(&biter, &kv);
		if(loc_res != 0) {
			fprintf(stderr, "ERROR: tree_size < map_size\n");
			res = -1;
		} else if(kv.key != map_pair.first || kv.value != map_pair.second) {
			fprintf(stderr, "ERROR: map_pair = (%ld, %ld), kv = (%ld, %ld)\n",
					map_pair.first, map_pair.second, kv.key, kv.value);
			res = -1;
		}
	}
	if(btree_iterator_next(&biter, &kv) != -1) {	
		fprintf(stderr, "ERROR: tree_size > map_size : kv = (%ld, %ld)\n", kv.key, kv.value);
		res = -1;
	}
	while(btree_iterator_next(&biter, &kv) != -1);
	return res;
}

void print_task(uint8_t* task, size_t size) {
	for(size_t i = 0; i < size; ++i) {
		printf("%d", task[i]);
	}
	putchar('\n');
}

int main() {
	srand(time(NULL));
	uint8_t* task_array = (uint8_t*)calloc(MAX_TASK_ARRAY_SIZE, sizeof(uint8_t)); 
	int64_t* key_array = (int64_t*)calloc(MAX_KEY_ARRAY_SIZE, sizeof(int64_t));

	for(;;) {
		size_t task_array_step_size = rand() % (MAX_TASK_ARRAY_SIZE - 10) + 10;
		for(size_t i = 0; i < MAX_KEY_ARRAY_SIZE; ++i) {
			key_array[i] = rand();
		}
		for(size_t i = 0; i < task_array_step_size; ++i) {
			task_array[i] = IN_TASK(rand());
		}

		std::map<int64_t, int64_t> test_map;
		btree test_btree;
		btree_init(&test_btree, 4);

		int64_t counter_of_bad_search = 0;
		for(size_t i = 0; i < task_array_step_size; ++i) {
			switch(task_array[i]) {
				case IS_INSERT: 
					{
						int64_t key = key_array[rand() % MAX_KEY_ARRAY_SIZE];
						test_map[key] = i; // std::map insert
						btree_insert(&test_btree, key, i);
					}
					break;
				case IS_ERASE:
					{
						int64_t key = key_array[rand() % MAX_KEY_ARRAY_SIZE];
						auto p = test_map.find(key);
						if(p != test_map.end()) {
							test_map.erase(p);
						}
						btree_erase(&test_btree, key);
					}
					break;
				case IS_SEARCH:
					{
						size_t find_key = rand() % (MAX_KEY_ARRAY_SIZE + 1);
						int64_t key;
						if(find_key == MAX_KEY_ARRAY_SIZE) { // random key
							key = rand(); 
						} else {
							key = key_array[find_key]; // known key
						}
						auto p = test_map.find(key);
						int64_t value = 0;
						int ans = btree_search(&test_btree, key, &value);
						if(!((p == test_map.end() && ans == -1) ||
									(p != test_map.end() && ans == 0 && p -> second == value))) {
							counter_of_bad_search++;
						}
					}
					break;
			}
		}
		print_task(task_array, task_array_step_size);
		if(comper_stores(test_map, test_btree) == 0 && 
				counter_of_bad_search == 0) {
			printf("OK\n");
		} else {
			printf("FAIL\n");
			printf("count of bad search: %ld\n",
					counter_of_bad_search);
			break;
		}
		printf("----------------------------------\n");
		btree_free(&test_btree);
	}

	free(key_array);
	free(task_array);
	return 0;
}
