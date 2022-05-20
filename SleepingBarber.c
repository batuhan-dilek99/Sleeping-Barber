#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include<string.h>
#include<semaphore.h>



//barber sleeps when there is no customer
//If all chairs are occupied, new customer will not get in queue
//If chairs are available but barber is busy, customer sits on a chairs
//If the barber is sleeping, Customer wakes the barber up


//There are chairs and barber chair. barber sleeps on barber chair.

sem_t waitingRoom; //Thread waiting room
sem_t barberChair; //Mutual Exclusion
sem_t barberSleep; //makes barber sleep
sem_t holdCustomer;
int stop = 0;

void *customerFun(void *cid){
    int custId = *(int *) cid;
    sem_wait(&waitingRoom);  //Wait for an opening in the waiting room
    printf("Customer : %d has entered to the waiting room\n", custId);

    sem_wait(&barberChair);   //Customer waits until the barber chair goes free.

    sem_post(&waitingRoom);   //customer releases the waiting room chair

    printf("Customer %d is waking up the barber\n",custId);
    sem_post(&barberSleep);   //Waking the barber from his sleep.

    sem_wait(&holdCustomer);  //Waiting barber to cut customer's hair.

    sem_post(&barberChair);   //after getting a haircut customer releases the barberchair

}

void *barberFun(void *bid){
    int barId = *(int *) bid;

    while(!stop){
        printf("Barber is sleeping\n");
        sem_wait(&barberSleep);  //Waiting for someone to wake him up.
        if(!stop){
            printf("Cutting hair...\n");
            sleep(3);
            printf("Barber finished cutting hair\n");
            sem_post(&holdCustomer); //after giving service to the customer, Customer is free to go
        }
    }
}


int main(void){

    sem_init(&waitingRoom, 0, 5);   //there are 5 waiting chairs.
    sem_init(&barberChair, 0, 1);
    sem_init(&barberSleep, 0, 0);
    sem_init(&holdCustomer,0 ,0);
    pthread_t customer[10], barber;
    int c = 10;
    int b = 1;
    int customers[10] = {1,2,3,4,5,6,7,8,9,10};

    pthread_create(&barber, NULL, (void *)barberFun, (void *)&b);

    for(int i = 0; i < c; i++){
        pthread_create(&customer[i], NULL, (void *)customerFun, (void *)&customers[i]);
    }



    for(int i = 0; i < c; i++){
        pthread_join(customer[i], NULL);
    }

    stop = 1;
    sem_post(&barberSleep);   //after all customers are served. wake the barber up to send him home. He is tired now :/
    pthread_join(barber, NULL);

    return 0;
}
