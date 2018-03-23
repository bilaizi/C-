#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    return 0;
}


/* 
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
	if (argc < 2) {
		fprintf(stderr, "Usage: %s \n", argv[0]);
		exit(EXIT_FAILURE);
	}
	exit (EXIT_SUCCESS);
}
*/

/*
struct person {
    int age;
    char *name;
} p;
#define container_of(ptr, type, member) ({\
   	const typeof(((type *)0)->member) * __mptr = (ptr);\
	(type *)((char *)__mptr - offsetof(type, member)); })
struct person the_person;
char *the_name_ptr = the_person.name;
container_of(the_name_ptr, struct person, name);



#define LIST_HEAD_INIT(name) { &(name), &(name) }
#define LIST_HEAD(name) struct list_head name = LIST_HEAD_INIT(name)
static inline void INIT_LIST_HEAD(struct list_head *list){
    WRITE_ONCE(list->next, list);
    list->prev = list;
}
#ifdef CONFIG_DEBUG_LIST
    extern bool __list_add_valid(struct list_head *new, struct list_head *prev, struct list_head *next);
    extern bool __list_del_entry_valid(struct list_head *entry);
#else
    static inline bool __list_add_valid(struct list_head *new, struct list_head *prev, struct list_head *next) {
	return true;
    }
    static inline bool __list_del_entry_valid(struct list_head *entry) {
	return true;
    }
#endif


*/
