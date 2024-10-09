
#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

int total_errors = 0;

// Helper function to increment error count and print an error message
void report_error(const char *message) {
    printf(stderr, "\x1b[31mError: %s\x1b[0m\n", message);
    total_errors++;
}

// Test Case 1: malloc and free 1-byte objects 120 times
void test_case1() {
    for (int i = 0; i < 120; i++) {
        void *ptr = malloc(1);  // Allocate 1 byte
        if (!ptr) {
            report_error("Failed to allocate 1 byte in test_case1");
        }
        free(ptr);              // Immediately free it
    }
}

// Test Case 2: Allocate 120 1-byte objects, then free them
void test_case2() {
    void *pointers[120];
    for (int i = 0; i < 120; i++) {
        pointers[i] = malloc(1);  // Allocate 1 byte for each pointer
        if (!pointers[i]) {
            report_error("Failed to allocate 1 byte in test_case2");
        }
    }
    for (int i = 0; i < 120; i++) {
        free(pointers[i]);        // Free each pointer
    }
}

// Test Case 3: Random allocation and free
void test_case3() {
    void *pointers[120] = {NULL};  // Array to hold 120 pointers
    int allocated = 0;
    for (int i = 0; i < 240; i++) {
        if (allocated < 120 && (rand() % 2 == 0)) {  // Allocate half the time
            pointers[allocated++] = malloc(1);       // Allocate 1 byte
            if (!pointers[allocated - 1]) {
                report_error("Failed to allocate 1 byte in test_case3");
            }
        } else if (allocated > 0) {                  // Free randomly
            int index = rand() % allocated;
            free(pointers[index]);
            pointers[index] = pointers[--allocated]; // Move last element to index
        }
    }
    // Free any remaining allocated pointers
    for (int i = 0; i < allocated; i++) {
        free(pointers[i]);
    }
}

// Test Case 4: Allocate large objects and check for memory leaks
void test_case4() {
    void *pointers[50];
    for (int i = 0; i < 50; i++) {
        pointers[i] = malloc(64);  // Allocate 64-byte objects
        if (pointers[i] == NULL) {
            report_error("Failed to allocate 64 bytes in test_case4");
            exit(1);
        }
    }
    // No freeing, this should be detected by the leak detector
}

// Test Case 5: Allocate objects, free half, and allocate again
void test_case5() {
    void *pointers[60];
    
    // Allocate 60 1-byte objects
    for (int i = 0; i < 60; i++) {
        pointers[i] = malloc(1);
        if (!pointers[i]) {
            report_error("Failed to allocate 1 byte in test_case5 (first pass)");
        }
    }

    // Free half of the objects
    for (int i = 0; i < 30; i++) {
        free(pointers[i]);
    }

    // Allocate 30 more objects
    for (int i = 0; i < 30; i++) {
        pointers[i] = malloc(1);
        if (!pointers[i]) {
            report_error("Failed to allocate 1 byte in test_case5 (second pass)");
        }
    }

    // Free remaining objects
    for (int i = 30; i < 60; i++) {
        free(pointers[i]);
    }
}

// Function to run a test case and measure its performance
void run_test_case(void (*test_func)(), const char *test_name) {
    struct timeval start, end;
    gettimeofday(&start, NULL);  // Record start time
    
    test_func();  // Run the test case

    gettimeofday(&end, NULL);  // Record end time
    
    // Calculate the elapsed time in microseconds
    long seconds = (end.tv_sec - start.tv_sec);
    long microseconds = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
    printf("%s took %ld microseconds\n", test_name, microseconds);
}

// Summarize the results of all tests and print the total error count
void summarize_results() {
    if (total_errors == 0) {
        printf("All tests passed successfully.\n");
    } else {
        printf("\x1b[31mTest completed with %d errors.\n\x1b[0m", total_errors);  // Print in red
    }
}


// Run all test cases multiple times and measure average performance
void run_all_tests() {
    const int iterations = 1;
    for (int i = 0; i < iterations; i++) {
        run_test_case(test_case1, "Test Case 1");
        reset_heap();
        run_test_case(test_case2, "Test Case 2");
        reset_heap();
        run_test_case(test_case3, "Test Case 3");
        reset_heap();
        run_test_case(test_case4, "Test Case 4");
        reset_heap();
        run_test_case(test_case5, "Test Case 5");
        reset_heap();
    }
}

int main() {
    run_all_tests();  // Run all test cases
    summarize_results();  // Summarize the results of all test cases
    return 0;
}