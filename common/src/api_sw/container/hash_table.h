
#ifndef __HASH_TABLE_H_
#define __HASH_TABLE_H_

#include <stdbool.h>
#include "xil_types.h"
#include <stdlib.h>

#define MAX_TB_SIZE 3000
#define MAX_KEY_SIZE 30
#define MAX_TEST_TB_SIZE 20

typedef struct Node {
	char key[MAX_TB_SIZE];
	int value;
	struct Node* next;
}Node;

typedef struct NodeL {
	char key[MAX_TB_SIZE];
	u64 value;
	struct Node* next;
}NodeL;

void c_str_cpy(char* dest, const char* src);
int c_str_cmp(const char* str1, const char* str2);
int hash_func(const char* str);
void init_hash_table(volatile struct Node** tb);
//void init_hash_table_n(volatile struct Node** tb, int tb_size);
void add_item(volatile struct Node** tb, const char* key, int value);
bool find_item(struct Node** tb, const char* key, int* val);
bool delete_item(struct Node** tb, const char* key);

void init_hash_table_l(volatile struct NodeL** tb, int tb_size);
void add_item_l(volatile struct NodeL** tb, const char* key, u64 value);
bool find_item_l(struct NodeL** tb, const char* key, int* val);
bool delete_item_l(struct NodeL** tb, const char* key);

#endif
