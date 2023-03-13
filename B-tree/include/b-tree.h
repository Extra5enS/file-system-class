#ifndef B_TREE
#define B_TREE
#include "b-tree-node.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    struct node* root;
} btree;

void btree_init(btree* bt, size_t t);
int btree_search(btree* bt, int64_t key, int64_t* value);
void btree_insert(btree* bt, int64_t key, int64_t value);
int btree_erase(btree* bt, int64_t);
btree btree_merge(btree* bt, btree* nextbt);
void btree_foreach(btree* bt, void (*)(struct kv_pair* value));
void btree_free(btree* bt);

typedef struct {
	struct node_iter iter;
	btree* bt;
} btree_iterator;

void btree_iterator_init(btree_iterator* biter, btree* bt);
int btree_iterator_next(btree_iterator* biter, struct kv_pair* value);

#ifdef __cplusplus
}
#endif

#endif
