#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>


sem_t Libre;// indice des cases vides dans le buffer
sem_t Occuper;// indice des cases occupées dans le buffer
sem_t Access_Indice;//assurer l'acces critique par le thread au buffer

volatile MSG_BLOCK buffer[4];
MSG_BLOCK sortie;
int iLibre,iOccupe,jLibre,jOccupe;
int indiceLibre[4];
int indiceOccuppe[4];
pthread_t th1;
pthread_t th2;
pthread_t th3;
pthread_t th4;
pthread_t th5;


void MessageFill(volatile MSG_BLOCK* mBlock, pthread_t threadId){
DWORD i;
mBlock->checksum = 0;
for(i=0;i < DATA_SIZE;i++){
mBlock->mData[i] = rand();
mBlock->checksum ^= mBlock->mData[i];
}
mBlock->threadID = threadId;
return;
}

/**
* Display the message content
* @param mBlock the message pointer
*/
void MessageDisplay(volatile MSG_BLOCK* mBlock){
DWORD i, tcheck=0;
for(i=0;i < DATA_SIZE;i++)
tcheck ^= mBlock->mData[i];
if(tcheck == mBlock->checksum)
printf("[OK      ] Checksum validated\n");
else
printf("[  FAILED] Checksum failed, message corrupted\n");

}
void * producer()
{
     int iL;
	while(1)
	{
		sem_wait(&Libre);
		sem_wait(&Access_Indice);
		iL=iLibre;
		iLibre++;
		if(iLibre==4) iLibre=0;
		produceCount=(produceCount+1)%4;
		sem_post(&Access_Indice);
		MessageFill(&buffer[iL],pthread_self());
		sem_post(&Occuper);
	}
}
void * consumer()
{ 
	DWORD i;
  MSG_BLOCK msg;
	while(1)
	{
      sem_wait(&Occuper);
	msg=buffer[iOccupe];
	if(iOccupe==4) iOccupe=0;
	iOccupe++;   
	sem_post(&Libre);
	consumeCount=(consumeCount+1)%4;
	for(i=0;i<DATA_SIZE;i++)
	{
		sortie.mData[i]+=msg.mData[i];
	}
		printf("diagnostic:\nle nombre de donnees acquises c est: %d \nle nombre de donnees sommees c est: %d \nle nombre de donnees restants c'est : %d\n",produceCount,consumeCount,4-consumeCount);   
}
}


int  main()
{
	sem_init(&Libre,0,6);
	sem_init(&Occuper,0,0);
	sem_init(&Access_Indice,0,1);
	consumeCount=0;
	produceCount=0;
	pthread_create(&th1,NULL,producer,NULL);
	pthread_create(&th2,NULL,producer,NULL);
	pthread_create(&th3,NULL,producer,NULL);
	pthread_create(&th4,NULL,producer,NULL);
	pthread_create(&th5,NULL,consumer,NULL);
	pthread_join(th4,NULL);
	pthread_join(th3,NULL);
	pthread_join(th2,NULL);
	pthread_join(th1,NULL);
	pthread_join(th5,NULL);
	return 0;
}
