#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#define SHM_NAME "/shm_table"
#define MAX_ITEMS 2

// Shared structure
typedef struct {
    int buffer[MAX_ITEMS];
    int count; // current number of items on the table
} SharedTable;

SharedTable *table;

// Named semaphores
sem_t *mutex, *empty, *full;

void *consume(void *arg) {
    while (1) {
        sem_wait(full);
        sem_wait(mutex);

        // Critical section
        int item = table->buffer[table->count - 1];
        printf("Consumer consumed: %d\n", item);
        table->buffer[table->count - 1] = 0;  // Optional: clear slot
        table->count--;

        sem_post(mutex);
        sem_post(empty);

        sleep(2);  // simulate consumption time
    }
    return NULL;
}

int main() {
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    table = mmap(0, sizeof(SharedTable), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (table == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Open semaphores (they were already created by producer)
    mutex = sem_open("/mutex", 0);
    empty = sem_open("/empty", 0);
    full  = sem_open("/full",  0);

    if (mutex == SEM_FAILED || empty == SEM_FAILED || full == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    // Create consumer thread
    pthread_t consumer_thread;
    pthread_create(&consumer_thread, NULL, consume, NULL);
    pthread_join(consumer_thread, NULL);

    // Cleanup (not reached in infinite loop)
    munmap(table, sizeof(SharedTable));
    close(shm_fd);

    sem_close(mutex);
    sem_close(empty);
    sem_close(full);

    return 0;
}
