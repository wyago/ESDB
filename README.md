# ESDB

The interface to the system lies in src/esdb.h:

```c
struct esdb;

struct esdb *make_esdb(int first_id, int block_size);

int register_component(struct esdb *db, int component_size);

void esdb_act(struct esdb *db, void (*f)(struct esdb *, void **),
    int n_components, int *components);

void queue_entity(struct esdb *db, int n_components, int *component_ids,
    void **component_data);

void free_entity(struct esdb *db, long int id);

void flush_queues(struct esdb *db);
```

To initialize a DB, you use `make_esdb` and `register_component`. The first_id 
parameter is the first entity ID to use (so that you can reserve space 
for zero or somesuch), and `block_size` is the number of components to put in each
block for the internal block list (block lists will be explained later). A good
typical value for `block_size` is somewhere around 1024.

`register_component` is for telling the database about a component type you want
to be able to add to entities later on. You can register components at any time
after getting a `struct esdb`, but there isn't yet functionality to unregister
components. The only parameter is the `component_size`, which is just the number
of bytes that each component must be able to store. The value returned is the ID
of the component registered.

`esdb_act` is where it gets more interesting. This function is used to perform
an action on all entities with a particular set of components. The function
pointer f should take the database and an array of pointers, each of which
points to the appropriate component in memory. The components array is the array
of component IDs which are needed for the calculation.

`queue_entity` lets you add an entity to the db. You aren't allowed to add an
entity in one step because that would make changes to the list of components
potentially while iterating over them. `component_ids` is the array of IDs for
the components to be added, while `component_data` is the array of pointers to
data for those components.

`free_entity` similarly is not instantaneous. It queues the action so it doesn't
interfere with ongoing iteration. `id` is the ID of the entity to remove.

`flush_queues` should be run at the end of the frame to enact all the queued 
changes.
