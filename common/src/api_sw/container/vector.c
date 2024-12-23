/*
 * vector.c
 *
 *  Created on: 2022. 8. 4.
 *      Author: Wonju
 */

#include "vector.h"

int VectorSize(vector *v)
{
    int totalCount = UNDEFINE;
    if(v)
    {
        totalCount = v->vectorList.size;
    }
    return totalCount;
}
int VectorResize(vector *v, int capacity)
{
    int  status = UNDEFINE;
    if(v)
    {
        void **items = realloc(v->vectorList.items, sizeof(void *) * capacity);
        if (items)
        {
            v->vectorList.items = items;
            v->vectorList.capacity = capacity;
            status = SUCCESS;
        }
    }
    return status;
}
int vectorPushBack(vector *v, void *item)
{
    int  status = UNDEFINE;
    if(v)
    {
        if (v->vectorList.capacity == v->vectorList.size)
        {
            status = VectorResize(v, v->vectorList.capacity * 2);
            if(status != UNDEFINE)
            {
                v->vectorList.items[v->vectorList.size++] = item;
            }
        }
        else
        {
            v->vectorList.items[v->vectorList.size++] = item;
            status = SUCCESS;
        }
    }
    return status;
}
int vectorSet(vector *v, int index, void *item)
{
    int  status = UNDEFINE;
    if(v)
    {
        if ((index >= 0) && (index < v->vectorList.size))
        {
            v->vectorList.items[index] = item;
            status = SUCCESS;
        }
    }
    return status;
}
void *vectorGet(vector *v, int index)
{
    void *readData = NULL;
    if(v)
    {
        if ((index >= 0) && (index < v->vectorList.size))
        {
            readData = v->vectorList.items[index];
        }
    }
    return readData;
}
int vectorDelete(vector *v, int index)
{
    int  status = UNDEFINE;
    int i = 0;
    if(v)
    {
        if ((index < 0) || (index >= v->vectorList.size))
            return status;
        v->vectorList.items[index] = NULL;
        for (i = index; (i < v->vectorList.size - 1); ++i)
        {
            v->vectorList.items[i] = v->vectorList.items[i + 1];
            v->vectorList.items[i + 1] = NULL;
        }
        v->vectorList.size--;
        if ((v->vectorList.size > 0) && ((v->vectorList.size) == (v->vectorList.capacity / 4)))
        {
        	VectorResize(v, v->vectorList.capacity / 2);
        }
        status = SUCCESS;
    }
    return status;
}
int vectorFree(vector *v)
{
    int  status = UNDEFINE;
    if(v)
    {
        free(v->vectorList.items);
        v->vectorList.items = NULL;
        status = SUCCESS;
    }
    return status;
}
void vector_init(vector *v)
{
    //init function pointers
    v->Size = VectorSize;
    v->Resize = VectorResize;
    v->PushBack = vectorPushBack;
    v->Set = vectorSet;
    v->Get = vectorGet;
    v->Free = vectorFree;
    v->Erase = vectorDelete;
    //initialize the capacity and allocate the memory
    v->vectorList.capacity = VECTOR_INIT_CAPACITY;
    v->vectorList.size = 0;
    v->vectorList.items = malloc(sizeof(void *) * v->vectorList.capacity);
}
//int main(int argc, char *argv[])
//{
//    int i =0;
//    //init vector
//    VECTOR_INIT(v);
//    //Add data in vector
//    v.pfVectorAdd(&v,"aticleworld.com\n");
//    v.pfVectorAdd(&v,"amlendra\n");
//    v.pfVectorAdd(&v,"Pooja\n");
//    //print the data and type cast it
//    for (i = 0; i < v.pfVectorTotal(&v); i++)
//    {
//        printf("%s", (char*)v.pfVectorGet(&v, i));
//    }
//    //Set the data at index 0
//    v.pfVectorSet(&v,0,"Apoorv\n");
//    printf("\n\n\nVector list after changes\n\n\n");
//    //print the data and type cast it
//    for (i = 0; i < v.pfVectorTotal(&v); i++)
//    {
//        printf("%s", (char*)v.pfVectorGet(&v, i));
//    }
//    return 0;
//}
