#ifndef BLOCK_LIST_H_INCLUDED
#define BLOCK_LIST_H_INCLUDED

struct block;

struct block_list {
	struct block *head;
	int block_size;
	int item_size;
};

struct block_list_iterator {
	struct block *current;
	int index;
	int block_size;
	int item_size;
};

struct block_list make_block_list(int block_size, int item_size);

// Insert or remove a series of items from the block list. The keys MUST be ordered ascending. This allows
// for the optimization of traversing the list a single time.
void block_insert(struct block_list list, long int *keys, void *items, int n_items);
void block_remove(struct block_list list, long int *keys, int n_keys);

struct block_list_iterator iterate(struct block_list list);
void *current(struct block_list_iterator iterator);
long int current_key(struct block_list_iterator iterator);
struct block_list_iterator next(struct block_list_iterator iterator);
char available(struct block_list_iterator iterator);

#endif // SORTED_BLOCKS_H_INCLUDED
