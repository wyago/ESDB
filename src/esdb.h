#ifndef ESDB_H_INCLUDED
#define ESDB_H_INCLUDED

struct esdb;

struct esdb *make_esdb(int first_id, int block_size);

int register_component(struct esdb *db, int component_size);

void esdb_act(
    struct esdb *db, 
    void (*f)(struct esdb *, long int entity_id, void **),
    int n_components, int *components);

void queue_entity(struct esdb *db, int n_components, int *component_ids,
    void **component_data);

void free_entity(struct esdb *db, long int id);

void flush_queues(struct esdb *db);

#endif
