
#include <stdio.h>
#include <unistd.h>

struct header_t;
typedef struct header_t header_t;

struct header_t
{
    int free;
    header_t *next;
    size_t size;
};

header_t *head = (header_t *)0;

header_t *getExistingSpace(size_t size)
{
    header_t *p = head;

    while (p)
    {
        if (p->size >= size && p->free)
        {
            p->free = 0;
            return p;
        }
        p = p->next;
    }
    return 0;
}

void free(void *ptr)
{
    header_t *p = (header_t *)(ptr - sizeof(header_t));

    if (!p)
    {
        _exit(0);
    }

    p->free = 1;
}

void *my_malloc(size_t size)
{
    header_t *p = getExistingSpace(size);

    if (!p)
    {
        printf("First time allocation\n");

        const void *const block = sbrk(sizeof(header_t) + size);

        if ((void *)-1 == block)
        {
            return (void *)0;
        }

        p = (header_t *)block;
        p->free = 0;
        p->next = 0;
        p->size = size;
    }

    return p + sizeof(header_t);
}

int main(void)
{

    void *const r = my_malloc(1024);
    free(r);
    printf("%zu ------ %zu\n", (size_t)r, (size_t)sbrk(1024));
    return 0;
}
