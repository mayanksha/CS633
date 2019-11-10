#include <stdio.h>
#include <stdlib.h>

#include "vector.h"

int main(void)
{
    int i;

    vector v;
    vector_init(&v);

    vector_add(&v, 1);
    vector_add(&v, 99);
    vector_add(&v, 10000);
    vector_add(&v, 200);

    for (i = 0; i < vector_total(&v); i++)
        printf("%d ", vector_get(&v, i));
    printf("\n");

    vector_delete(&v, 3);
    vector_delete(&v, 2);
    vector_delete(&v, 1);

    vector_set(&v, 0, 2000);
    vector_add(&v, 33);

    for (i = 0; i < vector_total(&v); i++)
        printf("%d ", vector_get(&v, i));
    printf("\n");

    vector_free(&v);
}
