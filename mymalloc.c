#include <stdio.h>
#include <unistd.h>

#ifdef _WIN32
#include <synchapi.h>
#elif __linux__
#include <unistd.h>
#endif

struct header_t;
typedef struct header_t header_t;

struct header_t
{
    unsigned int free;
    header_t *next;
    size_t size;
};

header_t *head = (header_t *)0;
header_t *tail = (header_t *)0;

header_t *getExistingSpace(size_t size)
{
    header_t *p = head;

    while (p)
    {
        if (p->size >= size && p->free)
        {
            p->free = 0;
            return p + 1;
        }
        p = p->next;
    }
    return 0;
}

void free(void *ptr)
{
    header_t *p = (header_t *)ptr - 1;

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
        const void *const block = sbrk(sizeof(header_t) + size);

        if ((void *)-1 == block)
        {
            return (void *)0;
        }

        p = (header_t *)block;
        p->free = 0;
        p->next = 0;
        p->size = size;

        if (!head)
        {
            head = p;
            tail = p;
            printf("%zu\n", (size_t)p);
        }
        else
        {
                        tail->next = p;
            tail = p;
        }
        return p + 1;
    }
    else
    {
        return p;
    }
}

int main(void)
{
    void *r;

    for (size_t i = 0; i < 4; i++)
    {
        r = my_malloc(1024);

        // sleep(3);

        free(r);
    }

    return 0;
}
