//Allen Poon
//CS 449 Project 5

//Compile with gcc -o server server.c -pthread -lm

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#define MYPORT	51400

void *process(void *connfd);

typedef struct Arguments //struct allows passing multiple argss to pthread_create()
{
	int *connfd;
	struct sockaddr_in addr;
}args;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main()
{
	int sfd, connfd; //server and client file descriptors
	pthread_t thread;
	sfd = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr;
	args* x; //contains client file descriptor and address to allow passing multiple argss
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port=htons(MYPORT);
	addr.sin_addr.s_addr=INADDR_ANY;

	bind(sfd, (struct sockaddr *)&addr, sizeof(addr));
	if(bind<0)
	{
		perror(NULL);
	}	
	if(sfd<0)
	{
		perror(NULL);
		//printf("Server unable to start\n");
	}
	else
		printf("Server is up and running\n\n");

	while(1)
	{
		x = malloc(sizeof(struct Arguments));

		listen(sfd,10);
		int length=sizeof(addr);
		connfd = accept(sfd, (struct sockaddr *)&addr, &length);
		if(connfd<0)
			//printf("Client could not connect\n");
			perror(NULL);
		else
		{
			printf("Client successfully connected\n");
			x->connfd=&connfd;
			x->addr=addr;
		}
		if(pthread_create(&thread, NULL, process, (void *)x)<0)
			perror(NULL);
		else
			printf("Thread Successfully created\n\n");
		pthread_join(thread, NULL);
	}
	close(sfd);
}

void *process(void *p)
{
	char httpRequest[4096]; //4kb request limit
	char fileRequest[1024];
	FILE *file;
	char *serverMessage;
	int contentSize;
	int digits;
	char *fileContent;
	time_t currTime;
	struct tm *locTime;
	char timeString[40];
	int infoSent;
	int totalInfoSent=0;
	FILE *output;
	int hostPort=MYPORT;
	char host[50];
	char input[4096];
	args *x=p;
	int *connfd=x->connfd;
	struct sockaddr_in addr=x->addr;
	int i;
	recv(*connfd, httpRequest,4096,0);
	
	
	//store the first line of httpRequest into input
	for(i=0; i<4096; i++)
	{
		input[i]=httpRequest[i];
		if((httpRequest[i]=='\n') || (httpRequest[i]=='\r'))
			break;
	}

	//check the input for 'GET '
	if(strncmp(input,"GET ", 4)==0)
	{

		i=5;
		//store the html file in fileRequest
		while(httpRequest[i]!=' ')
		{
			fileRequest[i-5]=httpRequest[i];
			i++;
		}

		fileRequest[i]='\0';
		
		file=fopen(fileRequest, "r");
		if(file==NULL)
		{
			//allocate 30 bytes for the 404 message
			serverMessage=(char *)malloc(30*sizeof(char));
			strcpy(serverMessage,"\nHTTP/1.1 404 Not Found\n");
		}
		else
		{
			//calculate the size of the file in order to calculate required memory
			fseek(file,0,SEEK_END);
			contentSize=ftell(file);
			//calculate the number of digits to represent the file size
			digits=(int)(log10(contentSize));
			//allocate appropriate amount of memory for file
			fileContent=(char *)malloc((contentSize)*sizeof(char));
			//read in file starting from beginning
			fseek(file,0,SEEK_SET);
			fread(fileContent,1,contentSize,file);
			
			serverMessage=(char *)malloc((contentSize*sizeof(char))+(digits*(sizeof(char)))+(200*sizeof(char)));
			strcpy(serverMessage,"\nHTTP/1.1 200 OK\n");
			//set up the time of request
			currTime=time(NULL);
			locTime=localtime(&currTime);
			strftime(timeString,40,"Date: %A, %d %B %Y %X %Z\n", locTime);
			strcat(serverMessage,timeString);
			//file size
			strcat(serverMessage,"\nContent-Length: ");
			char contentSizeString[digits];
			sprintf(contentSizeString,"%d",contentSize);
			strcat(serverMessage,contentSizeString);
			//connection status and content type
			strcat(serverMessage,"\nConnection: close\nContent-Type: text/html\n\n");
			strcat(serverMessage,fileContent);
		}
		//printf("INPUT: %s\n",&httpRequest);
		
		unsigned short clientPort = ntohs(addr.sin_port);
		char *clientIP = inet_ntoa(addr.sin_addr);
		const char localHostIP[]="127.0.0.1"; //reserved ip address for localhost machine
		//lock when doing file access to prevent data race
		pthread_mutex_lock(&mutex);
		output=fopen("stats.txt","a");
		fprintf(output,"\n%s\nHost: %s:%d\nClient: %s:%hu\n",input, localHostIP, hostPort, clientIP, clientPort);
		fclose(output);
		//unlock when finished to allow other threads to write
		pthread_mutex_unlock(&mutex);
	}
	else
	{
		//allocate 30 bytes for the 404 message
		serverMessage=(char *)malloc(30*sizeof(char));
		strcpy(serverMessage,"\nHTTP/1.1 404 Not Found\n");
	}
	
	send(*connfd,serverMessage,strlen(serverMessage),0);
	close(*connfd);
}