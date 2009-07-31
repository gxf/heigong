#ifndef PIC_DB_H
#define PIC_DB_H

typedef struct _picdb_item{
    char *  name;
    struct _picdb_item * next;
}picdb_item;

typedef struct _pic_db{
    int inited;
    int size;
    picdb_item * head;
}pic_db;

void picdb_construct(pic_db* picDb);
int picdb_lookup(pic_db* picDb, const char * file_name);
int picdb_insert(pic_db* picDb, const char * file_name);
int picdb_destroy(pic_db* picDb);

#endif
