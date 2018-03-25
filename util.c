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


/*
 * Architectures might want to move the poison pointer offset
 * into some well-recognized area such as 0xdead000000000000,
 * that is also not mappable by user-space exploits:
 */
#ifdef CONFIG_ILLEGAL_POINTER_VALUE
# define POISON_POINTER_DELTA _AC(CONFIG_ILLEGAL_POINTER_VALUE, UL)
#else
# define POISON_POINTER_DELTA 0
#endif
/*
 * These are non-NULL pointers that will result in page faults
 * under normal circumstances, used to verify that nobody uses
 * non-initialized list entries.
 */
#define LIST_POISON1  ((void *) 0x100 + POISON_POINTER_DELTA)
#define LIST_POISON2  ((void *) 0x200 + POISON_POINTER_DELTA)


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

// Insert a new entry between two known consecutive entries.
// This is only for internal list manipulation where we know the prev/next entries already!
static inline void __list_add(struct list_head *new, struct list_head *prev, struct list_head *next){
	if (!__list_add_valid(new, prev, next))
		return;
	next->prev = new;
	new->next = next;
	new->prev = prev;
	WRITE_ONCE(prev->next, new);
}

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 * Insert a new entry after the specified head. This is good for implementing stacks.
 */
static inline void list_add(struct list_head *new, struct list_head *head) {
	__list_add(new, head, head->next);
}

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 * Insert a new entry before the specified head. This is useful for implementing queues.
 */
static inline void list_add_tail(struct list_head *new, struct list_head *head) {
	__list_add(new, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries point to each other.
 * This is only for internal list manipulation where we know the prev/next entries already!
 */
static inline void __list_del(struct list_head * prev, struct list_head * next) {
	next->prev = prev;
	WRITE_ONCE(prev->next, next);
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty() on entry does not return true after this, the entry is in an undefined state.
 */
static inline void __list_del_entry(struct list_head *entry) {
	if (!__list_del_entry_valid(entry))
		return;
	__list_del(entry->prev, entry->next);
}

static inline void list_del(struct list_head *entry) {
	__list_del_entry(entry);
	entry->next = LIST_POISON1;
	entry->prev = LIST_POISON2;
}

#define MINORBITS	20
#define MINORMASK	((1U << MINORBITS) - 1)
#define MAJOR(dev)	((unsigned int) ((dev) >> MINORBITS))
#define MINOR(dev)	((unsigned int) ((dev) & MINORMASK))
#define MKDEV(ma,mi)	(((ma) << MINORBITS) | (mi))

struct file_operations {
	struct module *owner;
	loff_t (*llseek) (struct file*, loff_t, int);
	ssize_t (*read) (struct file*, char __user*, size_t, loff_t*);
	ssize_t (*write) (struct file*, const char __user*, size_t, loff_t*);
	ssize_t (*read_iter) (struct kiocb*, struct iov_iter*);
	ssize_t (*write_iter) (struct kiocb*, struct iov_iter*);
	int (*iterate) (struct file*, struct dir_context*);
	int (*iterate_shared) (struct file*, struct dir_context*);
	__poll_t (*poll) (struct file*, struct poll_table_struct*);
	long (*unlocked_ioctl) (struct file*, unsigned int, unsigned long);
	long (*compat_ioctl) (struct file*, unsigned int, unsigned long);
	int (*mmap) (struct file*, struct vm_area_struct*);
	unsigned long mmap_supported_flags;
	int (*open) (struct inode*, struct file*);
	int (*flush) (struct file*, fl_owner_t id);
	int (*release) (struct inode*, struct file*);
	int (*fsync) (struct file*, loff_t, loff_t, int datasync);
	int (*fasync) (int, struct file*, int);
	int (*lock) (struct file*, int, struct file_lock*);
	ssize_t (*sendpage) (struct file*, struct page*, int, size_t, loff_t*, int);
	unsigned long (*get_unmapped_area)(struct file*, unsigned long, unsigned long, unsigned long, unsigned long);
	int (*check_flags)(int);
	int (*flock) (struct file*, int, struct file_lock*);
	ssize_t (*splice_write)(struct pipe_inode_info*, struct file*, loff_t*, size_t, unsigned int);
	ssize_t (*splice_read)(struct file*, loff_t*, struct pipe_inode_info*, size_t, unsigned int);
	int (*setlease)(struct file*, long, struct file_lock**, void**);
	long (*fallocate)(struct file* file, int mode, loff_t offset, loff_t len);
	void (*show_fdinfo)(struct seq_file* m, struct file* f);
#ifndef CONFIG_MMU
	unsigned (*mmap_capabilities)(struct file*);
#endif
	ssize_t (*copy_file_range)(struct file*, loff_t, struct file*, loff_t, size_t, unsigned int);
	int (*clone_file_range)(struct file *, loff_t, struct file *, loff_t, u64);
	ssize_t (*dedupe_file_range)(struct file*, u64, u64, struct file*, u64);
} __randomize_layout;
