#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>

int main(){


    sem_unlink("/mutex");
    sem_unlink("/full");
    sem_unlink("/empty");
    sem_unlink("/shm_table");

    printf("Semaphores and memory cleared");
    
    return 0;

}