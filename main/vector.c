#include "vector.h"

void arrayInit(Array** arr_ptr) 
{
    Array *container; 
    container = (Array*)malloc(sizeof(Array)); 
    if(!container) { 
        printf("Memory Allocation Failed\n"); 
        exit(0); 
    } 
  
    container->size = 0; 
    container->capacity = 8; 
    container->array = (size_t **)malloc(8 * sizeof(size_t)); 
    if (!container->array){ 
        printf("Memory Allocation Failed\n"); 
        exit(0); 
    } 
  
    *arr_ptr = container; 
} 
  
//  Insertion Operation 
void insertItem(Array* container, size_t* item)
{ 
    if (container->size == container->capacity) { 
        size_t **temp = container->array;
        container->capacity <<= 1; 
        container->array = realloc(container->array, container->capacity * sizeof(size_t)); 
        if(!container->array) { 
            printf("Out of Memory\n"); 
            container->array = temp; 
            return; 
        } 
    }

    container->array[container->size++] = item; 
} 
  
// Retrieve Item at Particular Index 
size_t* getItem(Array* container, size_t index)
{ 
    if(index >= container->size) { 
        printf("Index Out of Bounds\n"); 
        return 0;
    } 
    return container->array[index]; 
} 
  
// Update Operation 
//void updateItem(Array* container, size_t index, size_t* item)
//{
//    if (index >= container->size) {
//        printf("Index Out of Bounds\n");
//        return;
//    }
//    container->array[index] = item;
//}
  
// Delete Item from Particular Index 
//void deleteItem(Array* container, size_t index)
//{
//    if(index >= container->size) {
//        printf("Index Out of Bounds\n");
//        return;
//    }
//
//    for (size_t i = index; i < container->size; i++) {
//        container->array[i] = container->array[i + 1];
//    }
//    container->size--;
//}
  
// Array Traversal 
//void printArray(Array* container)
//{
//    printf("Array elements: ");
//    for (size_t i = 0; i < container->size; i++) {
//        printf("%zu ", *container->array[i]);
//    }
//    printf("\nSize: ");
//    printf("%zu", container->size);
//    printf("\nCapacity: ");
//    printf("%zu\n", container->capacity);
//}
//
// Freeing the memory allocated to the array 
void freeArray(Array* container) 
{ 
    free(container->array); 
    free(container); 
}