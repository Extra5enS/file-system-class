#ifndef B_TREE_NODE
#define B_TREE_NODE

#include<stdlib.h>
#include<inttypes.h>

void* xmalloc(size_t count, size_t size);

struct kv_pair {
    int64_t key, value;
    int is_delete;
};

void kv_pair_init(struct kv_pair* kvp, int64_t key, int64_t value);
int kv_pair_cmp(void* lv, void* rv);

struct node {
    struct kv_pair* pairs;
    struct node** nodes;

    int leaf;
    size_t size;
    size_t t; // if tsize == 2 * stated - 1 -> creat new 
};

struct ans {
    struct node* n;
    size_t i;
};

void node_init(struct node* n, size_t t);
struct ans node_search(struct node* n, int64_t key);
void node_erase(struct node* n, int64_t key);
void node_foreach(struct node* n, void (*)(struct kv_pair* value));
struct node* node_merge(struct node* ln, struct node* rn);
void node_split_child(struct node* n, size_t i);
void node_insert_nonfull(struct node* n, int64_t key, int64_t value);
void node_free(struct node* n);

struct node_stack {
	struct node_stack* next;
	struct node* this_node;
	size_t pointer;
};

void node_stack_push(struct node_stack** ns, struct node* next);
void node_stack_pop(struct node_stack** ns);

struct node_iter{
	struct node_stack* stack;
};

void node_iter_init(struct node_iter* ni, struct node* base);
int node_iter_next(struct node_iter* ni, struct kv_pair* value);

#endif
