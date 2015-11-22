#include "block_list.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

struct block {
    int min_value;
    char *contained;
    char *values;
    struct block *next;
};

struct block_list make_block_list(int block_size, int item_size) {
	struct block_list list;
	list.block_size = block_size;
	list.item_size = item_size;

	list.head =  malloc(sizeof(struct block));
	list.head ->min_value = 0;
	list.head ->values = malloc(list.item_size * list.block_size);
	list.head ->contained = calloc(list.block_size, 1);
	list.head ->next = 0x0;
	return list;
}

void block_insert(struct block_list list, long int *keys, void *items, int n_items) {
	struct block *current = list.head;

	// This can be thought of as a kind of merge. We iterate through 'keys' and 
	// the block list in tandem, inserting an item when we find its spot. If the
	// space for an item does not exist, we create it.
	int i = 0;
	while (current != 0x0 && i < n_items) {
		if (keys[i] >= current->min_value + list.block_size) {
			if (current->next == 0x0 ||
				current->next->min_value > keys[i]) {
				struct block *farNext = current->next;
				current->next = malloc(sizeof(struct block));
				current->next->min_value = (keys[i] / list.block_size) * list.block_size;
				current->next->values = malloc(list.item_size * list.block_size);
				current->next->contained = calloc(list.block_size, 1);
				current->next->next = farNext;
			}

			current = current->next;
		} else if (keys[i] >= current->min_value) {// keys[i] < min_value + list.block_size
			int block_index = keys[i] - current->min_value;
			memcpy(current->values + (block_index * list.item_size), items + (i * list.item_size), list.item_size);
			current->contained[block_index] = 1;
			i += 1;
		} else {
			// It should not be possible to get to the state where the key is 
            // less than the minimum of the current block if the keys are 
            // sorted. If they're not, we want to skip this key so as little as
            // possible is messed up.
			i += 1;
		}
	}
}

void block_remove(struct block_list list, long int *keys, int n_keys) {
	struct block *current = list.head;

	int i = 0;
	while (current != 0x0 && i < n_keys) {
		if (keys[i] >= current->min_value + list.block_size) {
			current = current->next;
		} else if (keys[i] >= current->min_value) {// keys[i] < min_value + list.block_size
			int block_index = keys[i] - current->min_value;
			current->contained[block_index] = 0;
			i += 1;
		} else {
			// It should not be possible to get to the state where the key is
            // less than the minimum of the current block if the keys are
            // sorted. If they're not, we want to skip this key so as little as
            // possible is messed up.
			i += 1;
		}
	}
}

void block_act(void (*f)(void **), int n_lists, ...) {
    va_list block_args;
    int i;
    int j;
    struct block **blocks = malloc(n_lists * sizeof(struct block*));
    int *sizes = malloc(n_lists * sizeof(int));
    void **args = malloc(n_lists * sizeof(void*));
    int block_size;

    va_start(block_args, n_lists);
    for (i = 0; i < n_lists; ++i) {
        struct block_list list = va_arg(block_args, struct block_list);
        blocks[i] = list.head;
        block_size = list.block_size;
        sizes[i] = list.item_size;
    }
    va_end(block_args);

    while (1) {
        char allEqual = 1;
        // Compare all blocks to one another. If any are behind the others,
        // move them forward. If we hit the end of any list, we can return,
        // because if any list is at the end there can be no more tuples of
        // values that contain one from every list.
        for (i = 0; i < n_lists; ++i) {
            for (j = i + 1; j < n_lists; ++j) {
                if (blocks[i] == 0x0 || blocks[j] == 0x0) return;
                if (blocks[i]->min_value < blocks[j]->min_value) {
                    blocks[i] = blocks[i]->next;
                    allEqual = 0;
                } else if (blocks[i]->min_value > blocks[j]->min_value) {
                    blocks[j] = blocks[j]->next;
                    allEqual = 0;
                }
            }
        }

        if (!allEqual) continue;

        for (i = 0; i < block_size; ++i) {
            char contained = 1;
            for (j = 0; j < n_lists; ++j) {
                contained = contained & blocks[j]->contained[i];
                args[j] = blocks[j]->values + (i * sizes[j]);
            }

            if (contained) {
                f(args);
            }
        }

        for (i = 0; i < n_lists; ++i) {
            blocks[i] = blocks[i]->next;
        }
    }


    free(blocks);
    free(sizes);
    free(args);
}
