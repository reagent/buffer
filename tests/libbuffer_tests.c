#include "minunit.h"
#include <buffer.h>

int
string_compare_full(char *str, char *comparison, int bytes)
{
    int same = 1,
        i    = 0;

    for (i = 0; i < bytes; i++) {
        same = same && (str[i] == comparison[i]);
    }

    return same;
}

int
buffer_compare_full(Buffer *buf, char *comparison, int bytes)
{
    return string_compare_full(buf->contents, comparison, bytes);
}

int
buffer_compare(Buffer *buf, char *comparison)
{
    return buffer_compare_full(buf, comparison, strlen(comparison) + 1);
}

char *
test_grow()
{
    Buffer *buf = buffer_alloc(1);

    mu_assert(buf->total_size == 1, "Allocated size should be 1.");

    buffer_append(buf, "A", 1);
    mu_assert(buf->total_size == 4, "Allocated size should grow to 4.");

    buffer_append(buf, "BC", 2);
    mu_assert(buf->total_size == 4, "Does not grow when there is room.");

    buffer_append(buf, "D", 1);
    mu_assert(buf->total_size == 8, "Grows when out of space.");

    buffer_append(buf, "EFGH", 4);
    mu_assert(buf->total_size == 16, "Allocated size should grow to 16.");

    buffer_append(buf, "IJKLMNOPQRSTUVWXYZ", 18);
    mu_assert(buf->total_size == 38, "Allocated size should grow to 38.");

    buffer_free(buf);

    return NULL;
}

char *
test_buffer_length()
{
    Buffer *buf = buffer_alloc(8);

    mu_assert(buffer_strlen(buf) == 0, "Empty buffer length should be 0.");

    buffer_append(buf, "ABC", 3);
    mu_assert(buffer_strlen(buf) == 3, "Buffer length with 'ABC' should be 3.");

    buffer_free(buf);

    return NULL;
}

char *
test_buffer_append()
{
    int cmp_size = 4;
    char cmp[cmp_size];

    Buffer *buf = buffer_alloc(16);

    memset(&cmp, 0, cmp_size);

    cmp[0] = '\0';
    mu_assert(buffer_compare_full(buf, cmp, 1) == 1, "Buffer should be empty.");

    memset(&cmp, 0, cmp_size);
    buffer_append(buf, "A", 1);

    cmp[0] = 'A';
    cmp[1] = '\0';
    mu_assert(buffer_compare_full(buf, cmp, 2) == 1, "Appending C-style string should work.");

    memset(&cmp, 0, cmp_size);
    cmp[0] = 'A';
    cmp[1] = 'A';
    cmp[2] = '\0';

    buffer_append(buf, "A", 3);
    mu_assert(buffer_compare_full(buf, cmp, 3) == 1, "Appending C-style string stops at NUL.");

    memset(&cmp, 0, cmp_size);
    cmp[0] = 'A';
    cmp[1] = 'A';
    cmp[2] = 'B';
    cmp[3] = '\0';

    char no_nul[1] = {'B'};

    buffer_append(buf, no_nul, 1);
    mu_assert(buffer_compare_full(buf, cmp, 4) == 1, "Appending string without NUL byte stops at specified length.");

    buffer_free(buf);

    return NULL;
}

char *
test_buffer_appendf()
{
    Buffer *buf = buffer_alloc(64);

    buffer_appendf(buf, "%s, #%d", "Hello", 2);
    mu_assert(buffer_compare(buf, "Hello, #2") == 1, "Printing interprets string and decimal characters");

    buffer_appendf(buf, ". %s, #%d?", "Yes", 1);
    mu_assert(buffer_compare(buf, "Hello, #2. Yes, #1?") == 1, "Printing appends to existing buffer");

    buffer_free(buf);

    return NULL;
}

char *
test_buffer_nappendf()
{
    Buffer *buf = buffer_alloc(64);

    char test1[3] = {'H', 'e', '\0'};

    buffer_nappendf(buf, 2, "%s", "Hello");
    mu_assert(buffer_compare_full(buf, test1, 3) == 1, "Invalid characters after append.");

    buffer_free(buf);

    return NULL;
}

char *
test_buffer_to_s()
{
    Buffer *buf = buffer_alloc(64);
    char *result = NULL;

    buffer_append(buf, "Hi there.", 9);
    char compare[10] = {'H', 'i', ' ', 't', 'h', 'e', 'r', 'e', '.', '\0'};

    result = buffer_to_s(buf);
    mu_assert(string_compare_full(result, compare, 10) == 1, "Invalid string value.");

    free(result);
    buffer_free(buf);

    return NULL;
}

char *
all_tests()
{
    mu_suite_start();

    mu_run_test(test_grow);
    mu_run_test(test_buffer_length);
    mu_run_test(test_buffer_append);
    mu_run_test(test_buffer_appendf);
    mu_run_test(test_buffer_nappendf);
    mu_run_test(test_buffer_to_s);

    return NULL;
}

RUN_TESTS(all_tests);