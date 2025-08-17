#ifndef F
#define F
#include "functions.h"

int square(int);
int cube(int);
char* prime(int);
char* hello(char *);
char* pingpong(char *);
char *root(void);
int fibonacci(int);

int (*functions1[])(int) = {
    [CUBE]=cube,
    [SQUARE]=square,
    [ASQUARE]=square,
    [AFIBO]=fibonacci,
};

char* (*functions2[])(char *) = {
[PINGPONG]=pingpong,
[HELLO]=hello,
};

char* (*functions3[])(int)={
    [PRIME]=prime,
    [APRIME]=prime,  
};

char *(*parent[])(void)={
    [ROOT]=root,
};

#endif