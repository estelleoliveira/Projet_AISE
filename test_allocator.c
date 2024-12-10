#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <setjmp.h>
#include <stddef.h>
#include <cmocka.h>

#include "allocator.h"

static void test_allocation_and_free(void **state) {
    (void)state;
    void* ptr = my_malloc(64,1);
    assert(ptr != NULL);
    strcpy((char*)ptr, "Hello Allocator!");
    assert(strcmp((char*)ptr, "Hello Allocator!") == 0);
    my_free(ptr, 0);
}

static void test_reuse_after_free(void **state) {
    (void)state;
    void* ptr1 = my_malloc(64,1);
    my_free(ptr1,1);
    void* ptr2 = my_malloc(64,1);
    assert_ptr_equal(ptr1, ptr2); //vérifie que la mémoire est réutilisée
}

static void test_different_sizes(void **state) {
    (void)state;
    void* ptr1 = my_malloc(32,1);  //classe 32
    void* ptr2 = my_malloc(128,1); //classe 128
    assert_ptr_not_equal(ptr1, ptr2);        //vérifie que les adresses sont différentes
    my_free(ptr1,1);
    my_free(ptr2,1);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_allocation_and_free),
        cmocka_unit_test(test_reuse_after_free),
        cmocka_unit_test(test_different_sizes),
    };
    return cmocka_run_group_tests_name("Allocator Tests", tests, NULL, NULL);
}