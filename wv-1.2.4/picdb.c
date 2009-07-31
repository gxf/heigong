#include <string.h>
#include "picdb.h"

static void picdb_item_construct(picdb_item * item){
    item -> name = NULL;
    item -> next = NULL;
}

void picdb_construct(pic_db* picDb){
    picDb->inited = 1;
    picDb->size   = 0;
    picDb->head   = NULL;
}

int picdb_lookup(pic_db* picDb, const char * file_name){
    if (picDb->inited == 0){
        return 0;
    }
    picdb_item * p;
    for(p = picDb->head; p != NULL; p = p->next){
        if (!(strcmp(p->name, file_name))){
            return 1;
        }
    }
    return 0;
}

int picdb_insert(pic_db* picDb, const char * file_name){
    if (1 == picdb_lookup(picDb, file_name)){
        /* Insert twice is not allowed */
        return 1;
    }
    if (picDb->inited == 0){
        return 0;
    }
    /* Append new item to list head */
    picdb_item * p = picDb->head;
    picdb_item * q = (picdb_item *)malloc(sizeof(picdb_item));
    picdb_item_construct(q);
    char * name_str = (char *)malloc(strlen(file_name) + 1);
    strcpy(name_str, file_name);
    q->name = name_str;
    /* first node */
    if (p != NULL){
        q->next = p->next;
        p->next = q;
    }
    else{
        q->next = NULL;
        p       = q;
    }
    picDb->size++;
}

int picdb_destroy(pic_db* picDb){
    picdb_item * p = picDb->head; 
    picdb_item * q = NULL;
    while(p != NULL){
        q = p->next; 
        free(p->name);
        free(p);
        p = q;
    }
    picDb->size = 0;
}
