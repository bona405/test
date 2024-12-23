/*
 * vector.h
 *
 *  Created on: 2022. 8. 4.
 *      Author: Wonju
 */

#ifndef SRC_CONTAINER_VECTOR_H_
#define SRC_CONTAINER_VECTOR_H_

#include <stdio.h>
#include <stdlib.h>
#define VECTOR_INIT_CAPACITY 6
#define UNDEFINE  -1
#define SUCCESS 0
#define VECTOR_INIT(vec) vector vec;\
 vector_init(&vec)
//Store and track the stored data
typedef struct aVectorList
{
    void **items;
    int capacity;
    int size;
} aVectorList;
//structure contain the function pointer
typedef struct aVector vector;
struct aVector
{
	aVectorList vectorList;
//function pointersPushBack
    int (*Size)(vector *);
    int (*Resize)(vector *, int);
    int (*PushBack)(vector *, void *);
    int (*Set)(vector *, int, void *);
    void *(*Get)(vector *, int);
    int (*Erase)(vector *, int);
    int (*Free)(vector *);
};
int VectorSize(vector *v);
int VectorResize(vector *v, int capacity);
int vectorPushBack(vector *v, void *item);
int vectorSet(vector *v, int index, void *item);
void *vectorGet(vector *v, int index);
int vectorDelete(vector *v, int index);
int vectorFree(vector *v);
void vector_init(vector *v);

#endif /* SRC_CONTAINER_VECTOR_H_ */
