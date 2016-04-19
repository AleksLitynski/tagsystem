#include <stdio.h>

#define args(...) __VA_ARGS__
#define iter(TYPE, SETUP, ITEM, BODY) \
    TYPE ## _set * ITEM ## _set; \
    TYPE ## _item * ITEM; \
    TYPE ## _init(SETUP, ITEM ## _set, ITEM); \
    while(TYPE ## _next(ITEM ## _set, ITEM)) BODY; \
    TYPE ## _close(ITEM ## _set, ITEM); 


typedef struct {
    int to;
    int current;
} test_iter_set;
typedef int test_iter_item;

int test_iter_init(int low, int high, test_iter_set * set, test_iter_item * item) {
    printf("%i\n", low);
    printf("%i\n", high);
    *item = 0;
    set->to = high;
    set->current = low;
    return 0;
}

int test_iter_next(test_iter_set * set, test_iter_item * item) {
    if(set->current > set->to) {
        *item = set->current;
        set->current++;
        return 0;
    }
    return 1;
}

int test_iter_close(test_iter_set * set, test_iter_item *item) { 
    return 0;
}


int main(int argc, char * argv[]) {

    iter(test_iter, args(1, 10), i, {
        printf("%i", i);
        printf("other shit");
    })

    return 0;
}

