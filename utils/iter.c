#include <stdio.h> 
#define args(...) __VA_ARGS__
#define iter(TYPE, SETUP, ITEM, BODY) { \
    TYPE ## _set ITEM ## _set; \
    TYPE ## _item ITEM; \
    TYPE ## _init(SETUP, &ITEM ## _set, &ITEM); \
    while(TYPE ## _next(&ITEM ## _set, &ITEM)) BODY; \
    TYPE ## _close(&ITEM ## _set, &ITEM); \
    }


typedef struct {
    int to;
    int current;
} test_iter_set;
typedef int test_iter_item;

int test_iter_init(int low, int high, 
    test_iter_set * set, test_iter_item * item) {
    set->current = low;
    set->to = high;
    *item = (test_iter_item) 0;
    return 0;
}

int test_iter_next(test_iter_set * set, test_iter_item * item) {

    printf("test_iter_next\n");
    printf("%i\n", set->current);
    if(set->current > set->to) {
            printf("first\n");
        *item = set->current;
        set->current++;
        return 0;
    }
            printf("second\n");
    return 1;
}

int test_iter_close(test_iter_set * set, test_iter_item *item) { 
    return 0;
}

void mk(test_iter_set ** j) {
    *j = malloc(sizeof(test_iter_set));
}

int main(int argc, char * argv[]) {

    iter(test_iter, args(1, 10), i, {
        printf("%i", i);
        printf("other shit");
    })

    return 0;
}

