#include <stdio.h>
#include <stdlib.h>
#include <time.h>

unsigned long fact_r(int n) {
    unsigned long fact = 1;
    if (n > 1) 
        fact = fact_r(n - 1) * n;
    return fact;
}

unsigned long fact_i(int n) {
    unsigned long fact = 1;
    while (n >= 2) 
        fact *= n--;
    return fact;
}

double how_long(int n, int n_meas, unsigned long (*f)(int)) {
    unsigned long fact;
    clock_t start = clock();
    for (int k = 0; k < n_meas; k++) 
        fact = (*f)(n);
    clock_t end = clock();
    double td = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("%s: %lu\n", (f == &fact_r) ? "Recursion" : "Iteration", fact);
    return td;
}

int main(void) {
    double t_r = how_long(20, 10000000, &fact_r);
    double t_i = how_long(20, 10000000, &fact_i);
    printf("r=%f i=%f r/i=%f\n", t_r, t_i, t_r / t_i);
    return 0;
}

/*
Recursion: 2432902008176640000
Iteration: 2432902008176640000
r=1.413497 i=0.657159 r/i=2.150921
*/
