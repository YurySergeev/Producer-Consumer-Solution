#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>

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

void *produce(void *arg) {
    int item = 0;
    while (1) {
        sleep(1);  // simulate production time
        item++;

        sem_wait(empty);
        sem_wait(mutex);

        // Critical section
        table->buffer[table->count] = item;
        printf("Producer produced: %d\n", item);
        table->count++;

        sem_post(mutex);
        sem_post(full);
    }
    return NULL;
}

int main() {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    ftruncate(shm_fd, sizeof(SharedTable));
    table = mmap(0, sizeof(SharedTable), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (table == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Initialize table
    table->count = 0;
    memset(table->buffer, 0, sizeof(table->buffer));

    // Open or create semaphores
    mutex = sem_open("/mutex", O_CREAT, 0666, 1);
    empty = sem_open("/empty", O_CREAT, 0666, MAX_ITEMS);
    full  = sem_open("/full",  O_CREAT, 0666, 0);

    if (mutex == SEM_FAILED || empty == SEM_FAILED || full == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    // Create producer thread
    pthread_t producer_thread;
    pthread_create(&producer_thread, NULL, produce, NULL);
    pthread_join(producer_thread, NULL);

    // Cleanup (will not be reached in infinite loop)
    munmap(table, sizeof(SharedTable));
    close(shm_fd);

    sem_close(mutex);
    sem_close(empty);
    sem_close(full);

    return 0;
}
