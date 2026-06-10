#include <stddef.h>
#include <stdio.h>
#include <string.h>

void print_string(const char* string, size_t size)
{

    for (size_t i = 0; i < size; i++) {
        printf("Char %lu: %c\n", i, string[i]);
    }

    printf("Last Char: %c\n", string[size] == '\0' ? '/' : '_');
}

int main()
{
    const char* name = "Fabiano";

    printf("Size: %lu\n", strlen(name));
    printf("Size: %lu\n", strlen("Fabiano"));

    return 0;
}
