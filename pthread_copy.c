#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/mman.h>
#include<pthread.h>
#include<sys/fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>

#define THREAD_NO 10

char* ptr = NULL;
int blocksize;

int create_map(void)
{
	int fd = open("pthread_creat.c",O_RDONLY);
	int Fsize = lseek(fd,0,SEEK_END);
	ptr = mmap(NULL,Fsize,PROT_READ,MAP_PRIVATE,fd,0);
	return Fsize;
}

void * thread_copy(void* arg)
{
	int i =*(int*)arg;
	int POS = i * blocksize;
	char buf[blocksize];
	bzero(buf,sizeof(buf));
	snprintf(buf,blocksize+1,"%s",ptr+POS);
	int dest_fd = open("00000.c",O_RDWR|O_CREAT,0664);
	lseek(dest_fd,POS,SEEK_SET);
	write(dest_fd,buf,strlen(buf));
	close(dest_fd);
	pthread_exit(NULL);
}

int thread_cur(const char* File)
{
	int fd = open(File,O_RDONLY);
	int Fsize = lseek(fd,0,SEEK_END);
	if(Fsize%THREAD_NO==0)
		return Fsize/THREAD_NO;
	else
		return Fsize/THREAD_NO+1;


}

int thread_create(void)
{
	pthread_t tid[THREAD_NO];
	int i=0;
	for(i=0;i<THREAD_NO;i++){
		pthread_create(&tid[i],NULL,thread_copy,(void*)&i);
		usleep(10000);
	}
	while(i--){
		pthread_join(tid[i],NULL);
	}
	return 0;

}



int main(void)
{
	int Fsize;
	Fsize = create_map();
	blocksize = thread_cur("pthread_creat.c");
	thread_create();
	munmap(ptr,Fsize);
	printf("Thread copy is Done..\n");
	return 0;
}
