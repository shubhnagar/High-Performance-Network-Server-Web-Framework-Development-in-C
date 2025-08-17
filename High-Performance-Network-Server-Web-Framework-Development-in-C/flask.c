#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>
#include "civetweb.h"
#include "functions.h"
#include "functionslist.h"

#define MAX_SIZE 999999

struct map {
    int index;
    char *url;
    struct map *next;
};

struct map *hash_table[MAX_SIZE];

unsigned int hash(const char *url) {
    unsigned long hash = 6912;
    int c;
    while ((c = *url++)) {
        hash = ((hash << 5) + hash) + c;  
    }
    return hash % MAX_SIZE;
}

void insert(const char *url, int function_id) {
    unsigned int index = hash(url);
    struct map *new_mapping = malloc(sizeof(struct map));
    new_mapping->url = strdup(url);
    new_mapping->index = function_id;
    new_mapping->next = hash_table[index];
    hash_table[index] = new_mapping;
}
int get_index(const char *url) {
    unsigned int index = hash(url);
    struct map *current = hash_table[index];

    while (current) {
        if (strcmp(current->url, url) == 0) {
            return current->index;
        }
        current = current->next;
    }

    return -1;  
}

void initialize_mappings() {
    insert("/", ROOT);
    insert("/square", SQUARE);
    insert("/cube", CUBE);
    insert("/hello", HELLO);
    insert("/prime", PRIME);
    insert("/pingpong", PINGPONG);
    insert("/arithmetic/square",ASQUARE);
    insert("/arithmetic/prime",APRIME);
    insert("/arithmetic/fibonacci",AFIBO);
}

static int begin_request_handler(struct mg_connection *conn){
    const struct mg_request_info *request_info = mg_get_request_info(conn);
    char response_buffer[256];
    int param = 0;
    int result;
    char content[100];
    int index = get_index(request_info->request_uri);\
    
    //for part 1 ,2,3  
    
    // if (strcmp(request_info->request_uri, paths[ROOT]) == 0) {
    //     index = ROOT;
    // } else if (strcmp(request_info->local_uri, paths[SQUARE]) == 0) {
    //     index = SQUARE;
    // } else if (strcmp(request_info->local_uri, paths[CUBE]) == 0) {
    //     index = CUBE;
    // } else if (strcmp(request_info->local_uri, paths[HELLO]) == 0) {
    //     index = HELLO;
    // } else if (strcmp(request_info->local_uri, paths[PRIME]) == 0) {
    //     index = PRIME;
    // } else if (strcmp(request_info->local_uri, paths[PINGPONG]) == 0) {
    //     index = PINGPONG;
    // } else if (strcmp(request_info->local_uri, paths[ASQUARE]) == 0) {
    //     index = ASQUARE;
    // }

    if (index == -1) {
        mg_printf(conn, "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n404 Not Found");
        return 404;
    }
    if(index==ROOT){
        if (parent[index] != NULL) {
            char *result = parent[index]();
            snprintf(response_buffer, sizeof(response_buffer), "Response from root : %s",result);
            mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n%s", response_buffer);
        } else {
            mg_printf(conn, "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nInternal Server Error");
        }
    }
    else if (index == SQUARE || index == CUBE || index==ASQUARE||index==AFIBO) {     
        char num_str[32];
        if (mg_get_var(request_info->query_string,sizeof(request_info->query_string),"num", num_str, sizeof(num_str)) > 0) {
            param = atoi(num_str);  
        }

        if (functions1[index] != NULL) {
            result = functions1[index](param);
            snprintf(response_buffer, sizeof(response_buffer), "Result: %d", result);
            mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n%s", response_buffer);
        } else {
            mg_printf(conn, "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nInternal Server Error");
        }
    }
    else if (index == HELLO || index == PINGPONG) {
        char num_str[32];
        mg_get_var(request_info->query_string,1024,"str", num_str, sizeof(num_str));
        if (functions2[index] != NULL) {
            char *result=functions2[index](num_str);
            snprintf(response_buffer, sizeof(response_buffer), "Response from %s", result);
            mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n%s", response_buffer);
        } else {
            mg_printf(conn, "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nInternal Server Error");
        }
    }
    else if(index==APRIME || index==PRIME){
         char num_str[32];
        mg_get_var(request_info->query_string,1024,"num", num_str, sizeof(num_str));
        param=atoi(num_str);
        if (functions3[index] != NULL) {
            char *result=functions3[index](param);
            snprintf(response_buffer, sizeof(response_buffer), "Result %s", result);
            mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n%s", response_buffer);
        } else {
            mg_printf(conn, "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nInternal Server Error");
        }
    }
    return 1;
}

int main(int argc,char *argv[]){

    if(argc!=3){
        printf("Usage <portno> <number of threads> ");
        return -1;
    }

    struct mg_callbacks callbacks;
    struct mg_context *ctx;
    char *c=argv[1];
    char *num_threads=argv[2];
    const char *options[]={
        "listening_ports",c,
        "num_threads",num_threads,
         NULL
    };
    initialize_mappings();
    memset(&callbacks,0,sizeof(callbacks));
    callbacks.begin_request=begin_request_handler;
    ctx = mg_start(&callbacks, NULL, options);
    getchar();
    mg_stop(ctx);
    return 0;
}