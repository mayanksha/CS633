#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "vector.h"

void vector_init(vector *v)
{
    v->capacity = VECTOR_INIT_CAPACITY;
    v->total = 0;
    v->items = malloc(sizeof(int) * v->capacity);
}

int vector_total(vector *v)
{
    return v->total;
}

static void vector_resize(vector *v, int capacity)
{
    #ifdef DEBUG_ON
    printf("vector_resize: %d to %d\n", v->capacity, capacity);
    #endif

    int *items = realloc(v->items, sizeof(int) * capacity);
    if (items) {
        v->items = items;
        v->capacity = capacity;
    }
}

void vector_add(vector *v, int item)
{
    if (v->capacity == v->total)
        vector_resize(v, v->capacity * 2);
    v->items[v->total++] = item;
}

void vector_set(vector *v, int index, int item)
{
    if (index >= 0 && index < v->total)
        v->items[index] = item;
}

int vector_get(vector *v, int index)
{
    if (index >= 0 && index < v->total)
        return v->items[index];
    return -1;
}

void vector_delete(vector *v, int index)
{
    if (index < 0 || index >= v->total)
        return;

    v->items[index] = -1;

    for (int i = index; i < v->total - 1; i++) {
        v->items[i] = v->items[i + 1];
        v->items[i + 1] = -1;
    }

    v->total--;

    if (v->total > 0 && v->total == v->capacity / 4)
        vector_resize(v, v->capacity / 2);
}

void vector_free(vector *v)
{
    if (v->capacity != 0)
        free(v->items);
}
