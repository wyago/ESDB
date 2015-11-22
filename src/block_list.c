#include "block_list.h"
#include <string.h>
#include <stdlib.h>

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

	// This can be thought of as a kind of merge. We iterate through 'keys' and the
	// block list in tandem, inserting an item when we find its spot. If the space
	// for an item does not exist, we create it.
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
			// It should not be possible to get to the state where the key is less than the minimum of the current block
			// if the keys are sorted. If they're not, we want to skip this key so as little as possible is messed up.
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
			// It should not be possible to get to the state where the key is less than the minimum of the current block
			// if the keys are sorted. If they're not, we want to skip this key so as little as possible is messed up.
			i += 1;
		}
	}
}

struct block_list_iterator iterate(struct block_list list) {
	struct block_list_iterator iterator;
	iterator.current = list.head;
	iterator.block_size = list.block_size;
	iterator.item_size = list.item_size;
	iterator.index = 0;

	next(iterator);
	return iterator;
}

void *current(struct block_list_iterator iterator) {
	return iterator.current->values + (iterator.index * iterator.item_size);
}

long int current_key(struct block_list_iterator iterator) {
	return iterator.current->min_value + iterator.index;
}

struct block_list_iterator next(struct block_list_iterator iterator) {
	if (iterator.current == 0x0) return iterator;

	iterator.index += 1;
	while (iterator.index < iterator.block_size &&
		!iterator.current->contained[iterator.index]) {
		iterator.index += 1;
	}

	if (iterator.index >= iterator.block_size) {
		iterator.index = 0;
		iterator.current = iterator.current->next;
		return next(iterator);
	}

	return iterator;
}

char available(struct block_list_iterator iterator) {
	return iterator.current != 0x0;
}
