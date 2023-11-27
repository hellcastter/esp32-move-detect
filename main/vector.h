//
// Created by Victor Muryn on 19.11.2023.
//

#ifndef CAMERA_VECTOR_H
#define CAMERA_VECTOR_H
#include <esp_log.h>
#include "stddef.h"
#include "stdlib.h"
#include "stdio.h"

// base structure 
typedef struct { 
    size_t size; 
    size_t capacity;
    size_t** array;
} Array; 
  
// function prototypes 
//  array container functions 
void arrayInit(Array** arr_ptr); 
void freeArray(Array* container); 

// Basic Operation functions 
void insertItem(Array* container, size_t* item);
//void updateItem(Array* container, size_t i, size_t* item);
size_t* getItem(Array* container, size_t i);
//void deleteItem(Array* container, size_t* item);
//void printArray(Array* container);

#endif //CAMERA_VECTOR_H
