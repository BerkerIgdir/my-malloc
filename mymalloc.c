#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

struct header_t;
typedef struct header_t header_t;

struct header_t {
    uint8_t free;
    header_t *next;
    size_t size;
};

#define HEADER_SIZE sizeof(struct header_t)

header_t *head = (header_t *) 0;
header_t *tail = (header_t *) 0;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

header_t *getExistingSpace(size_t size) {
    header_t *p = head;

    while (p) {
        if (p->size >= size && p->free) {
            p->free = 0;
            return p;
        }
        p = p->next;
    }
    return 0;
}

void free(void *ptr) {

    pthread_mutex_lock(&mut);

    if (!ptr) {
        return;
    }

    header_t *p = (header_t *) ptr - 1;

    p->free = 1;
    pthread_mutex_unlock(&mut);
}

void *malloc(size_t size) {
    if (!size) {
        return NULL;
    }
    pthread_mutex_lock(&mut);
    header_t *p = getExistingSpace(size);

    if (!p) {
        const void *const block = sbrk(HEADER_SIZE + size);

        if ((void *) -1 == block) {
            return NULL;
        }

        p = (header_t *) block;
        p->free = 0;
        p->next = 0;
        p->size = size;

        if (!head) {
            head = p;
            tail = p;
        } else {
            tail->next = p;
            tail = p;
        }
        pthread_mutex_unlock(&mut);
        return (void *) (p + 1);
    } else {
        pthread_mutex_unlock(&mut);
        return (void *) (p + 1);
    }
}

void *realloc(void *base, size_t size) {
    if (!base) {
        return malloc(size);
    }

    header_t *tmp = (header_t *) base - 1;

    const size_t oldSize = tmp->size;

    if (oldSize >= size) {
        return base;
    }

    tmp = malloc(size);

    if (!tmp) {
        return NULL;
    }

    memcpy(tmp, base, oldSize);

    free(base);

    return (void *) (tmp);
}

void *calloc(size_t elmnt, size_t elSize) {
    if (!elSize) {
        return NULL;
    }

    size_t realSize = elmnt * elSize;

    void *temp = malloc(realSize);

    if (!temp) {
        return (void *) 0;
    }

    memset(temp, 0, realSize);

    return (header_t *) temp;
}

//int main(void) {
//    void *r = NULL;
//
//    for (size_t i = 0; i < 4; i++) {
//        // r = malloc(1024);
//
//        // sleep(3);
//
//        r = realloc(r, 2048);
//
//        free(r);
//    }
//
//    return 0;
//}
