#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>

#define TEST_ASSERT(expr)                                                   \
    do {                                                                    \
        if (!(expr)) {                                                       \
            fprintf(stderr, "FAIL: %s\n  file: %s\n  line: %d\n",            \
                    #expr, __FILE__, __LINE__);                             \
            exit(1);                                                        \
        }                                                                   \
    } while (0)

#define RUN_TEST(fn)                                                        \
    do {                                                                    \
        printf("running %s...\n", #fn);                                     \
        fn();                                                               \
        printf("PASS  %s\n", #fn);                                       \
    } while (0)

#endif
