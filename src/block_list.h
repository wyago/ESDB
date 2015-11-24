#ifndef BLOCK_LIST_H_INCLUDED
#define BLOCK_LIST_H_INCLUDED

struct esdb;
struct block;

struct block_list {
	struct block *head;
	int block_size;
	int item_size;
};

struct block_list make_block_list(int block_size, int item_size);

// Insert or remove a series of items from the block list. The keys MUST be 
// ordered ascending. This allows for the optimization of traversing the list 
// a single time.
void block_insert(struct block_list list, long int *keys, void **items, int n_items);
void block_remove(struct block_list list, long int *keys, int n_keys);

// A function that allows you to act on any number of blocks where the keys of
// the values are the same.
void block_act(struct esdb *db, void (*f)(struct esdb*, void **), int n_lists, struct block_list *lists);

#endif // SORTED_BLOCKS_H_INCLUDED
