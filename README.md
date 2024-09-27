Sparse Matrix Implementation

## Problem Area Description

This project focuses on efficiently handling sparse matrices—matrices in which a significant number of elements are zero. Instead of storing every element, including zeros, we optimize memory usage by using a linked list-based data structure.

## Data Structure

The sparse matrix is represented using the following structure:

```cpp
typedef struct node {
   int row;
   int column;
   int value;
   node* next;
   node* down;
} NODE;
```


