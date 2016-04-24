#include <stdio.h> 
#define args(...) __VA_ARGS__
#define iter(TYPE, SETUP, ITEM, BODY) { \
    TYPE ## _ctx ITEM ## _ctx; \
    TYPE ## _item ITEM; \
    TYPE ## _init(SETUP, &ITEM ## _ctx, &ITEM); \
    while(TYPE ## _next(&ITEM ## _ctx, &ITEM)) BODY; \
    TYPE ## _close(&ITEM ## _ctx, &ITEM); \
    }


typedef struct {
    int to;
    int current;
} test_iter_ctx;
typedef int test_iter_item;

int test_iter_init(int low, int high, 
    test_iter_ctx * set, test_iter_item * item) {
    set->current = low;
    set->to = high;
    *item = 0;
    return 0;
}

int test_iter_next(test_iter_ctx * set, test_iter_item * item) {

    if(set->current <= set->to) {
        *item = set->current;
        set->current++;
        return 1;
    }
    return 0;
}

int test_iter_close(test_iter_ctx * set, test_iter_item *item) { 
    return 0;
}

void mk(test_iter_ctx ** j) {
    *j = malloc(sizeof(test_iter_ctx));
}

int main(int argc, char * argv[]) {

    iter(test_iter, args(1, 10), i, {
        printf("it is now: %i oclock\n", i);
        if(i >= 5) break;
    })

    return 0;
}

