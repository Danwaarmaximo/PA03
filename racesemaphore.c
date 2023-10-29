#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>

struct Bank {
    int balance[2];
    sem_t semaphore;
};

void MakeTransactions(struct Bank *Bank) {
    int i, tmp1, tmp2, rint;
    double dummy;
    for (i = 0; i < 100; i++) {
        rint = (rand() % 30) - 15;
        sem_wait(&Bank->semaphore); // Wait for semaphore
        if (((tmp1 = Bank->balance[0]) + rint) >= 0 && ((tmp2 = Bank->balance[1]) - rint) >= 0) {
            Bank->balance[0] = tmp1 + rint;
            for (int j = 0; j < rint * 1000; j++) {
                dummy = 2.345 * 8.765 / 1.234;
            } // spend time on purpose
            Bank->balance[1] = tmp2 - rint;
        }
        sem_post(&Bank->semaphore); // Release semaphore
    }
}

int main(int argc, char **argv) {
    int shmid;
    void *shmaddr;
    struct Bank *Bank;

    // Create shared memory segment
    shmid = shmget(IPC_PRIVATE, sizeof(struct Bank), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Error in creating shared memory segment\n");
        return 1;
    }

    // Attach the shared memory segment
    shmaddr = shmat(shmid, NULL, 0);
    if (shmaddr == (void *)-1) {
        perror("Error in attaching shared memory segment\n");
        return 1;
    }

    // Initialize the Bank structure and semaphore in shared memory
    Bank = (struct Bank *)shmaddr;
    Bank->balance[0] = 100;
    Bank->balance[1] = 100;
    sem_init(&Bank->semaphore, 1, 1); // Initialize semaphore

    printf("Init balances A:%d + B:%d ==> %d!\n", Bank->balance[0], Bank->balance[1], Bank->balance[0] + Bank->balance[1]);

    pid_t pid = fork();

    if (pid == -1) {
        perror("Error in fork()\n");
        return 1;
    }

    if (pid == 0) {
        // Child process
        MakeTransactions(Bank);
        shmdt(Bank); // Detach the shared memory segment
        exit(0); // Exit child process after completion
    } else {
        // Parent process
        MakeTransactions(Bank);
        wait(NULL); // Wait for the child process to complete
        printf("Child process completed.\n");

        // Print the balances after transactions
        printf("Let's check the balances A:%d + B:%d ==> %d ?= 200\n", Bank->balance[0], Bank->balance[1], Bank->balance[0] + Bank->balance[1]);

        // Destroy semaphore and detach and remove the shared memory segment
        sem_destroy(&Bank->semaphore);
        shmdt(Bank);
        shmctl(shmid, IPC_RMID, NULL);
    }

    return 0;
}

