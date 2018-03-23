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
container_of(pointer, container_type, container_field);
*/
