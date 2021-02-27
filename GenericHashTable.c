#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "GenericHashTable.h"
#define INT_TYPE 0
#define STR_TYPE 1

Table *createTable(int size, int dType, int listLength)
{
	struct Table *table = (struct Table *)malloc(sizeof(struct Table));
	if (size <= 0 || (dType != INT_TYPE && dType != STR_TYPE) || listLength <= 0)
	{ //chaeck if the input is valid
		fprintf(stderr, "invalid input \n");
		return NULL;
	}
	table->arr = malloc(sizeof(struct Object *) * (size)); //set array of linked lists for the table
	if (table == NULL || table->arr == NULL)			   //memory allocation check
	{
		perror("MEMORY ALLOCATION FAILED");
		return NULL;
	}
	for (int i = 0; i < size; i++) //initillaize values for the array
	{
		table->arr[i] = NULL;
	}
	table->D = 1;				   //the ratio between the current size of the table to its original size
	table->M = size;			   //ORIGINAL TABLE SIZE
	table->N = size;			   //CURRENT TABLE SIZE
	table->list_size = listLength; //size of linked list
	table->data_type = dType;	   //type of data
	return table;
}

Object *createObject(void *data)
{
	Object *object = malloc(sizeof(struct Object));
	if (object == NULL) //memory allocation check
	{
		perror("memory allocation failed \n");
		return NULL;
	}
	object->data = data; //set data to the new object
	object->next = NULL; //initillaize value for the next object in the list
	return object;
}
int intHashFun(int *key, int origSize)
{
	int to_return = (*key);
	return to_return % origSize;
	;
}
int isEqual(int type, void *data1, void *data2)
{
	if (type == INT_TYPE)
	{
		int a = *(int *)data1;
		int b = *(int *)data2;
		return a - b; //0 if equals
	}
	else
	{
		return strcmp((char *)data1, (char *)data2); //0 if equals
	}
}

int strHashFun(char *key, int origSize)
{
	int sum = 0;
	for (int i = 0; i < strlen(key); i++) //calculate the int value for the string char by char
	{
		sum += (int)key[i];
	}
	int new_key = sum % origSize;
	return new_key;
}
//double the size of the table:

void extand(Table *table)
{
	table->N = table->N * 2;
	table->D = table->N / table->M;
	Object **extended = (Object **)malloc(sizeof(struct Object *) * table->N); //initillaize array for the new table
	if (extended == NULL)													   //memory allocation check
	{
		perror("MEMORY ALLOCATION FAILED");
		return;
	}
	for (int i = 0; i < table->N; i++) //initillaize values for the new table
	{
		extended[i] = NULL;
	}
	for (int i = 0; i < table->N; i += 2) //copy values for the new table
	{
		extended[i] = table->arr[i / 2];
	}
	free(table->arr);	   //free old table memory
	table->arr = extended; //set new table as the hash table.
}
int add(Table *table, void *data)
{

	if (data == NULL)
	{ //input validation check
		fprintf(stderr, "invalid input \n");
		return -1;
	}
	Object *obj = NULL;
	int key;
	/* Initilize values for pointers */
	int *pointer_int = 0;
	char *pointer_str = NULL;
	if (table->data_type == INT_TYPE)
	{
		pointer_int = (int *)malloc(sizeof(int));
		if (pointer_int == NULL)
		{ //memory allocation check
			perror("MEMORY ALLOCATION FAILED");
			return -1;
		}
		*pointer_int = *(int *)data;
		/* Key generating for the data */
		key = (intHashFun((void *)pointer_int, table->M));
		if (key < 0)
		{
			key = key + table->M;
		}
		key = key * (table->D);
		obj = createObject(pointer_int);
	}
	if (table->data_type == STR_TYPE)
	{
		pointer_str = (char *)malloc(sizeof(char) * (1 + strlen((char *)data)));
		if (pointer_str == NULL)
		{ //memory allocation check
			perror("MEMORY ALLOCATION FAILED");
			return -1;
		}
		strcpy(pointer_str, (char *)data);								//copy data to pointer
		key = (table->D) * (strHashFun((void *)pointer_str, table->M)); //set key
		obj = createObject((char *)*&pointer_str);
	}
	if (table->arr[key] == NULL) //first place in the list is available, place object here
	{
		table->arr[key] = obj;
		table->arr[key]->next = NULL;
		return key;
	}
	else
	{
		bool set = false;
		for (int i = key; i < key + table->D; i++) //search for the next availlable place in the currect key range.
		{
			Object *current_object = table->arr[i];
			if (current_object == NULL) //place is availlable, , place object here
			{
				table->arr[i] = obj;
				set = true;
				return key;
			}
			int counter = 1;
			while (counter < table->list_size) //if 1st place is not availlable, search the rest of the list
			{
				counter++;
				if (current_object->next == NULL)
				{
					current_object->next = obj;
					set = true;
					return key;
				}
				else
				{
					current_object = current_object->next; //go to next place in the list
				}
			}
		}
		if (set == false) //no place was found for the new object to be placed. extend the table.
		{
			extand(table); //extand table size X2
			if (table->data_type == INT_TYPE)
			{
				key = (intHashFun((void *)pointer_int, table->M));
				if (key < 0)
				{
					key = key + table->M;
				}
				key = key * (table->D);
			}
			else
			{
				key = (table->D) * (strHashFun(pointer_str, table->M));
			}
			table->arr[key + 1] = obj; //place object in the new list (guarnteed to be availlable)
			return key;
		}
	}
	return key;
}
Object *search(Table *table, void *data)
{
	int key;
	if (data == NULL)//input validation check
	{ 
		fprintf(stderr, "invalid input \n");
		return NULL;
	}
	/*generate the currect key by the type of data*/
	if (table->data_type == INT_TYPE)
	{
		key = (intHashFun((void *)data, table->M));
		if (key < 0)
		{
			key = key + table->M;
		}
		key = key * table->D;
	}
	if (table->data_type == STR_TYPE)
	{
		key = (table->D) * (strHashFun((void *)data, table->M));
	}
	for (int i = key; i < key + table->D; i++) //search for a matching value list by list object by object.
	{
		Object *current_object = table->arr[i];
		int counter = 0;
		while (current_object != NULL)
		{
			if (table->data_type == INT_TYPE && isEqual(INT_TYPE, (int *)current_object->data, (int *)data) == 0)//value match. return object and store location values
			{
				current_object->col = i;
				current_object->row = counter;
				return current_object;
			}
			if (table->data_type == STR_TYPE && isEqual(STR_TYPE, (char *)current_object->data, (char *)data) == 0)//value match. return object and store location values
			{
				current_object->col = i;
				current_object->row = counter;
				return current_object;
			}
			else //keep looking for the value
			{
				current_object = current_object->next;
				counter++;
			}
		}
	}
	return NULL; //if no matching was found.
}
void freeObject(Object *obj, int type) //free memory for a single object.
{
	free(obj->data);
	free(obj);
}
int removeObj(Table *table, void *data)
{
	Object *temp;
	int key;

	if (data == NULL)
	{ //input validation check
		fprintf(stderr, "invalid input \n");
		return -1;
	}
	/* key generating to search the value by type of data*/
	if (table->data_type == INT_TYPE)
	{
		key = (intHashFun((void *)data, table->M));
		if (key < 0)
		{
			key = key + table->M;
		}
		key = key * table->D;
	}
	if (table->data_type == STR_TYPE)
	{
		key = table->D * (strHashFun((void *)data, table->M));
	}

	Object *current_object = table->arr[key]; //first object in the list

	for (int i = key; i < key + table->D; i++)//search for a matching value list by list object by object.
	{
		current_object = table->arr[i];
		if (current_object == NULL)
		{
			continue;
		}
		if (isEqual(table->data_type, current_object->data, data) == 0) //value match. remove object and free memory
		{
			temp = current_object->next;
			table->arr[i] = temp;
			freeObject(current_object, table->data_type);
			return key;
		}
		else
		{
			while (current_object->next != NULL)
			{
				if (isEqual(table->data_type, current_object->next->data, data) == 0)//value match. remove object and free memory
				{
					temp = current_object->next->next;
					freeObject(current_object->next, table->data_type);
					current_object->next = temp;
					return key;
				}
				current_object = current_object->next;
			}
		}
	}
	return -1; //if no matching was found.
}
void print_list(Object *head, int dataType)
{
	Object *current_object = head;
	while (current_object != NULL) //prints all elements in the list tilll the end of it
	{
		if (dataType == INT_TYPE)
		{
			printf("%d 	--> 	", *(int *)current_object->data);
		}
		else //string type
		{
			printf("%s 	--> 	", (char *)current_object->data);
		}
		current_object = current_object->next;
	}
}
void printTable(Table *table)
{
	printf("\n \n");
	for (int i = 0; i < table->N; i++) //print list by list using print_list function
	{
		printf("[%d]	", i);
		print_list(table->arr[i], table->data_type);
		printf("\n");
	}
	printf("\n \n");
}
void freeTable(Table *table)
{
	Object *current_object = NULL;
	Object *current_object2 = NULL;
	for (int i = 0; i < table->N; i++) //free all linked llists in the table one by one object by ocject
	{
		current_object = table->arr[i];
		while (current_object != NULL)
		{
			current_object2 = current_object->next;
			freeObject(current_object, table->data_type);
			current_object = current_object2;
		}
	}
	free(current_object);
	free(current_object2);
	free(table->arr);
	free(table);
	return;
}
