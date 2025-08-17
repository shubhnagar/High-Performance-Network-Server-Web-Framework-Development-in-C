#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int cube(int a){
    if(a==0) return 1;
    return a*a*a;
}
int square(int a){
    if(a==0) return 1;
    return a*a;
}
char* hello(char *str){
   if(strlen(str)==0){
    return "Hello World";
   }
    char *result=malloc(1024);
    strcpy(result,"Hello ,");
    strcat(result,str);
    return result;
}
char* pingpong(char *str){
    if(strlen(str)==0){
    return "pingpong";
   }
    return str;
}
char* prime(int a){
    if(a==0) return "False";
    for(int i=2;i*i<a;++i){
        if(a%i==0) return "False";
    }
    return "True";
}

char* root(void){

    return "Hello World";
}

int fibonacci(int a){
    if(a==0) return 1;
    int s=0,e=1;
    if(a==1) return 0;
    if(a==2) return 1;
    a=a-2;
    int temp;
    while(a--){
        temp=s+e;
        s=e;
        e=temp;
    }
    return temp;
}
