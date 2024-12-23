#include "hash_table.h"
#include "stdio.h"
void c_str_cpy(char* dest, const char* src)
{
	while (*src != '\0') 
	{
		*dest = *src;
		dest++; src++;
	}
	*dest = '\0';
}

int c_str_cmp(const char* str1, const char* str2)
{
	while (*str1 != '\0' && (*str1 == *str2))
	{
		str1++;
		str2++;
	}
	return *str1 - *str2;
}

int hash_func(const char* str)
{
	unsigned long hash = 5381;
	int c;

	while ((c = *str++))
		hash = (((hash << 5) + hash) + c) % MAX_TB_SIZE; /* hash * 33 + c */

	return hash % MAX_TB_SIZE;
}

int hash_func_for_test(const char* str)
{
	unsigned long hash = 5381;
	int c;

	while ((c = *str++))
		hash = (((hash << 5) + hash) + c) % MAX_TEST_TB_SIZE; /* hash * 33 + c */

	return hash % MAX_TB_SIZE;
}

void init_hash_table(volatile struct Node** tb)
{
	for (int i = 0; i < MAX_TB_SIZE; ++i)
	{
		struct Node* cur = tb[i];
		struct Node* tmp;
		while (cur != NULL)
		{
			tmp = cur;
			cur = cur->next;
			free(tmp);
		}
		tb[i] = NULL;
	}
}

void add_item(volatile struct Node** tb, const char* key, int value)
{

	struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
	c_str_cpy(new_node->key, key);
	new_node->value = value;
	new_node->next = NULL;

	int index = hash_func(key);

	if (tb[index] == NULL)
	{
		tb[index] = new_node;
	}

	else
	{
		struct Node* cur = tb[index];
		while (cur != NULL)
		{
			if (c_str_cmp(cur->key, key) == 0)
			{
				cur->value = value;
				return;
			}
			cur = cur->next;
		}

		new_node->next = tb[index];
		tb[index] = new_node;
	}
}

bool find_item(struct Node** tb, const char* key, int* val)
{
	int index = hash_func(key);
	struct Node* cur = tb[index];

	while (cur != NULL)
	{
		if (c_str_cmp(cur->key, key) == 0)
		{
			*val = cur->value;
			return true;
		}
		cur = cur->next;
	}
	return false;
}

bool delete_item(struct Node** tb, const char* key) {

	int index = hash_func(key);

	if (tb[index] == NULL)
		return false;

	if (c_str_cmp(tb[index]->key, key) == 0)
	{
		struct Node* selected_node = tb[index];
		tb[index] = tb[index]->next;
		free(selected_node);
		return true;
	}
	else
	{

		struct Node* cur = tb[index]->next;
		struct Node* prev = tb[index];

		while (cur != NULL && c_str_cmp(cur->key, key) != 0)
		{
			prev = cur;
			cur = cur->next;
		}

		if (cur == NULL) return false;

		prev->next = cur->next;
		free(cur);
		return true;
	}
}


void init_hash_table_l(volatile struct NodeL** tb, int tb_size)
{
	for (int i = 0; i < tb_size; ++i)
	{
		struct NodeL* cur = tb[i];
		struct NodeL* tmp;
		while (cur != NULL)
		{
			tmp = cur;
			cur = cur->next;
			free(tmp);
		}
		tb[i] = NULL;
	}
}


void add_item_l(volatile struct NodeL** tb, const char* key, u64 value)
{

	struct NodeL* new_node = (struct NodeL*)malloc(sizeof(struct NodeL));
	c_str_cpy(new_node->key, key);
	new_node->value = value;
	new_node->next = NULL;

	int index = hash_func(key);

	if (tb[index] == NULL) 
	{
		tb[index] = new_node;
	}

	else
	{
		struct NodeL* cur = tb[index];
		while (cur != NULL) 
		{
			if (c_str_cmp(cur->key, key) == 0) 
			{
				cur->value = value;
				return;
			}
			cur = cur->next;
		}

		new_node->next = tb[index];
		tb[index] = new_node;
	}
}

bool find_item_l(struct NodeL** tb, const char* key, int* val)
{
	int index = hash_func(key);
	struct NodeL* cur = tb[index];

	while (cur != NULL) 
	{
		if (c_str_cmp(cur->key, key) == 0) 
		{
			*val = cur->value;
			return true;
		}
		cur = cur->next;
	}
	return false;
}

bool delete_item_l(struct NodeL** tb, const char* key)
{

	int index = hash_func(key);

	if (tb[index] == NULL) 
		return false;

	if (c_str_cmp(tb[index]->key, key) == 0) 
	{
		struct Node* selected_node = tb[index];
		tb[index] = tb[index]->next;
		free(selected_node);
		return true;
	}
	else 
	{

		struct Node* cur = tb[index]->next;
		struct Node* prev = tb[index];

		while (cur != NULL && c_str_cmp(cur->key, key) != 0) 
		{
			prev = cur;
			cur = cur->next;
		}

		if (cur == NULL) return false;

		prev->next = cur->next;
		free(cur);
		return true;
	}
}

