#include"../include/b-tree.h"
#include<stdio.h>

void btree_init(btree* bt, size_t t) {
    bt -> root = xmalloc(1, sizeof(struct node));
    node_init(bt -> root, t);
}

int btree_search(btree* bt, int64_t key, int64_t* value) {
    struct ans a = node_search(bt -> root, key);
    if(a.n == NULL || a.n -> pairs[a.i].is_delete) {
        return -1;
    } else {
        *value = a.n -> pairs[a.i].value;
        return 0;
    }
}

void btree_insert(btree* bt, int64_t key, int64_t value) { 
    struct ans a = node_search(bt -> root, key);
    if(a.n != NULL) {
        if(a.n -> pairs[a.i].is_delete) {
            a.n -> pairs[a.i].is_delete = 0;
        }
        a.n -> pairs[a.i].value = value;
        return;
    }

    struct node* root = bt -> root;
    
    if(root -> size == 2 * root -> t - 1) {
        struct node* newn = xmalloc(1, sizeof(struct node));
        node_init(newn, root -> t);

        bt -> root = newn;
        newn -> leaf = 0;
        newn -> size = 0;
        newn -> nodes[0] = root;
        node_split_child(newn, 0);
        node_insert_nonfull(newn, key, value);
    } else {
        node_insert_nonfull(root, key, value);
    }
}

int btree_erase(btree* bt, int64_t key) {
    struct ans a = node_search(bt -> root, key);
    if(a.n != NULL && !a.n -> pairs[a.i].is_delete) {
        a.n -> pairs[a.i].is_delete = 1;
        return 1;
    } else {
        return 0;
    }
}

void btree_foreach(btree* bt, void (*ptr)(struct kv_pair* value)) {
    node_foreach(bt -> root, ptr);
}

void btree_free(btree* bt) {
    node_free(bt -> root);
    free(bt -> root);
}

btree btree_merge(btree* ltree, btree* rtree) {
    btree res_tree;
    res_tree.root = node_merge(ltree -> root, rtree -> root);
    return res_tree;
}

void btree_iterator_init(btree_iterator* biter, btree* bt) {
	biter -> bt = bt;
	node_iter_init(&(biter -> iter), bt -> root);
}

int btree_iterator_next(btree_iterator* biter,struct kv_pair* value) {
	if(biter -> bt -> root -> size == 0) {
		return -1;
	}
	int res;
	do {
		res = node_iter_next(&(biter -> iter), value);
	} while(res == 0 && value -> is_delete);
	return res;
}
