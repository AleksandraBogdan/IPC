#include <string.h>
#include <stdio.h>
#include <sys/mman.h>

void* buf;

void conn_create();
int conn_read(int index);
void conn_write(int index, int number);
void conn_delete();
