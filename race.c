#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct {
    int balance[2];
} Bank = {{100, 100}}; // global variable defined

void *MakeTransactions() { // routine for thread execution
    int i, j, tmp1, tmp2, rint;
    double dummy;
    for (i = 0; i < 100; i++) {
        rint = (rand() % 30) - 15;
        if (((tmp1 = Bank.balance[0]) + rint) >= 0 && ((tmp2 = Bank.balance[1]) - rint) >= 0) {
            Bank.balance[0] = tmp1 + rint;
            for (j = 0; j < rint * 1000; j++) {
                dummy = 2.345 * 8.765 / 1.234;
            } // spend time on purpose
            Bank.balance[1] = tmp2 - rint;
        }
    }
    return NULL;
}

int main(int argc, char **argv) {
    int i;
    void *voidptr = NULL;
    pthread_t tid[2];

    srand(getpid());

    printf("Init balances A:%d + B:%d ==> %d!\n",
           Bank.balance[0], Bank.balance[1], Bank.balance[0] + Bank.balance[1]);

    for (i = 0; i < 2; i++) {
        if (pthread_create(&tid[i], NULL, MakeTransactions, NULL)) {
            perror("Error in thread creating\n");
            return (1);
        }
    }

    for (i = 0; i < 2; i++) {
        if (pthread_join(tid[i], (void *)&voidptr)) {
            perror("Error in thread joining\n");
            return (1);
        }
    }

    printf("Let's check the balances A:%d + B:%d ==> %d ?= 200\n",
           Bank.balance[0], Bank.balance[1], Bank.balance[0] + Bank.balance[1]);

    return 0;
}

