#include "esdb.h"

#include <stdlib.h>
#include <limits.h>
#include "block_list.h"

struct queued_entity {
    long int id;
    int n_components;
    int *component_ids;
    void **component_data;
};

struct esdb {
    int *component_ids;
    struct block_list *lists;
    int n_lists;

    long int *freed_ids;
    int n_freed;
    int cap_freed;

    struct queued_entity *queued;
    int n_queued;
    int cap_queued;

    long int next_id;
    int block_size;
};

struct esdb *make_esdb(int first_id, int block_size) {
    struct esdb *db = malloc(sizeof(struct esdb));
    db->component_ids = 0x0;
    db->lists = 0x0;
    db->n_lists = 0;
    
    db->n_freed = 0;
    db->cap_freed = 128;
    db->freed_ids = malloc(db->cap_freed * sizeof(long int));
    
    db->n_queued = 0;
    db->cap_queued = 128;
    db->queued = malloc(db->cap_queued * sizeof(struct queued_entity));

    db->next_id = first_id;
    db->block_size = block_size;

    return db;
}

int register_component(struct esdb *db, int component_size) {
    db->n_lists += 1;
    db->lists = realloc(db->lists, db->n_lists * sizeof(struct block_list));
    db->lists[db->n_lists - 1] = make_block_list(db->block_size, component_size);
    return db->n_lists - 1;
}

void esdb_act(struct esdb *db,
    void (*f)(struct esdb *, long int entity_id, void **),
    int n_components, int *components) {
    int i;
    struct block_list *lists = malloc(n_components * sizeof(struct block_list));
    for (i = 0; i < n_components; ++i) {
        lists[i] = db->lists[components[i]];
    }

    block_act(db, f, n_components, lists);
    free(lists);
}

void queue_entity(struct esdb *db, int n_components, int *component_ids,
    void **component_data) {
    db->n_queued += 1;
    if (db->n_queued >= db->cap_queued) {
        db->cap_queued *= 2;
        db->queued = realloc(db->queued, db->cap_queued * sizeof(struct queued_entity));
    }

    struct queued_entity *queued = db->queued + db->n_queued - 1;
    queued->id = db->next_id;
    db->next_id += 1;
    queued->n_components = n_components;
    queued->component_ids = component_ids;
    queued->component_data = component_data;
}

void free_entity(struct esdb *db, long int id) {
    db->n_freed += 1;
    if (db->n_freed >= db->cap_freed) {
        db->cap_freed *= 2;
        db->freed_ids = realloc(db->freed_ids, db->cap_freed * sizeof(long int));
    }

    db->freed_ids[db->n_freed - 1] = id;
}

void sort(int n, long int *keys, void **data);
void swap_buffers(struct esdb *db) {
    int i;
    int j;
    int k;

    for (i = 0; i < db->n_lists; ++i) {
        block_remove(db->lists[i], db->freed_ids, db->n_freed);
    }
    db->n_freed = 0;

    long int *keys = malloc(db->n_queued * sizeof(long int));
    void **data = malloc(db->n_queued * sizeof(void*));
    for (i = 0; i < db->n_lists; ++i) {
        int change_count = 0;
        for (j = 0; j < db->n_queued; ++j) {
            for (k = 0; k < db->queued[j].n_components; ++k) {
                if (db->queued[j].component_ids[k] == i) {
                    change_count += 1;
                    keys[change_count - 1] = db->queued[j].id;
                    data[change_count - 1] = db->queued[j].component_data[k];
                }
            }
        }
        sort(change_count, keys, data);
        block_insert(db->lists[i], keys, data, change_count);
    }
    
    for (i = 0; i < db->n_queued; ++i) {
        free(db->queued[i].component_ids);
        for (j = 0; j < db->queued[i].n_components; ++j) {
            free(db->queued[i].component_data[j]);
        }
        free(db->queued[i].component_data);
    }
    
    db->n_queued = 0;
    free(keys);
    free(data);

    for (i = 0; i < db->n_lists; ++i) {
        block_swap(db->lists[i]);
    }
}

void sort(int n, long int *keys, void **data) {
    int i;
    int j;
    int sorted = 1;
    for (i = 0; i < n-1; ++i) {
        if (keys[i] < keys[i+1]) {
            sorted = 0;
            break;
        }
    }
    if (!sorted) return;
    // Currently selection sort for simplicity.
    for (i = 0; i < n; ++i) {
        long int min = INT_MAX;
        int pos = 0;
        for (j = i; j < n; ++j) {
            if (keys[j] <= min) {
                pos = j;
                min = keys[j];
            }
        }
        int temp_key;
        void *temp_data;
        temp_key = keys[i];
        temp_data = data[i];

        keys[i] = keys[pos];
        data[i] = data[pos];

        keys[pos] = temp_key;
        data[pos] = temp_data;
    }
}
