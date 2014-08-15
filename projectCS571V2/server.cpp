#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <strings.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include  <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>      /* for errno and EINTR */
#include <signal.h>     /* for sigaction() */
#include "client.h"
#define ECHOMAX 255     /* Longest string to echo */
#define MAXPENDING 0
#define TIMEOUT_SECS 7
const int SHMKEY  = 10000;
const int SHMHOLDER = 20000;
const int BUFFSIZE = 1024;
int global_warming = 0;
int update[20];
char add[20] ;
unsigned short echoServPort;     /* Server port */
int tries = 0;
//int shm_key = shmget(SHMKEY,sizeof(int),0666|IPC_CREAT);
//int shm_holder = shmget(SHMHOLDER,sizeof(int),0666|IPC_CREAT);
//int* key_mem = (int*)shmat(shm_key,NULL,0);
//int* holder_mem = (int*)shmat(shm_holder,NULL,0);
void DieWithError(char errorMessage[]);  /* Error handling function */
void CatchAlarm(int ignored); 
void printData();
//void HandleTCPClient(int);   /* TCP client handling function */
distance_vector distVec;
local_machine lc;
int distArr[120][120];
int totalUpdate;
int isfloat (const char *s)
{
     char *ep = NULL;
     long i = strtol(s, &ep,10);
     if (!*ep)
         return true;  // it's an int
     /*if (*ep == 'e'  ||
         *ep == 'E'  ||
         *ep == '.')
         return true;*/
     return false;  // it not a float, but there's more stuff after it
}
int HandleTCPClient(int sock) {
    int received = -1;
	int command = 0;
	char stringHold[200];
	static int a = 0;
	local_machine local;
	FILE *fp = freopen("outputA.txt","a+",stdout);
	strcpy(stringHold,"");
    /* Receive message */
	char buffer[BUFFSIZE];
    if ((received = recv(sock, buffer, BUFFSIZE, 0)) < 0) {
       DieWithError("Error while recieve from client\n");
    }
    /* Send bytes and check for more incoming data in loop */
	char data[200];
	buffer[received]='\0';
	strcpy(data,buffer);
	int totalBytercvd = received;
	local.name = buffer[0];
	printf("I am waiting here for data\n");
	int j = 0;
    while (received > 0) {
      /* Send back received data */
		printf("recieved  = %d data=%s\n",received,buffer);
		char echoBuff[5];
		strcpy(echoBuff,"ok");
		send(sock, echoBuff, strlen(echoBuff), 0);
 // respond to command 
      /* Check for more data */
	  strcpy(buffer,"");
      if ((received = recv(sock, buffer, BUFFSIZE, 0)) < 0) {
			DieWithError("Failed to receive additional bytes from client");
      }
	  buffer[received]='\0';
	  if(strcmp(buffer,"end") == 0){
			//printf("Server end of the client service\n");
			break;
	  }
	  /*if(isfloat(buffer))
		local.port_num = atoi(buffer);
	  else
		strcpy(local.link_info[j++][0],buffer);*/
	  totalBytercvd += received;
    }
// close the network socket when the client leaves
	if(totalBytercvd < 0)
		return -1;
	if(strlen(data)!= 0){
		tempHolder tmp = stringTokennizer(data);
		int kIndex = 2;
		for(int i = 0 ; i < atoi(tmp[1]); i++){
			if(distArr[lc.name][tmp[kIndex][0]] >= distArr[lc.name][tmp[0][0]]+atoi(tmp[kIndex+1])){
				distArr[lc.name][tmp[kIndex][0]] = distArr[lc.name][tmp[0][0]]+atoi(tmp[kIndex+1]);
				if(update[tmp[kIndex][0] - 65] != 1){
					update[tmp[kIndex][0] - 65] = 1;
					totalUpdate++;
				}
			}
		    kIndex +=2;
		}
	}
	printf("Table for %c after receiving update %d\n",lc.name,++a);
	for(int  ki = 65; ki<72; ki++)
		printf("%c\t",ki);
	printf("\n");
	for(int  ki = 65; ki<72; ki++)
		printf("%d\t",distArr[lc.name][ki]);
	printf("\n");
	fclose(fp);
	return command;
}
void alarmSetting(struct sigaction &myAction){
	if(sigfillset(&myAction.sa_mask) < 0) /* block everything in handler */
		DieWithError("sigfillset() failed");
	myAction.sa_flags = 0;
	if(sigaction(SIGALRM, &myAction, 0) < 0)
		DieWithError("sigaction() failed for SIGALRM");
	alarm(TIMEOUT_SECS);
}
int main()
{
	printf("Start of the main\n");
    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    struct sockaddr_in echoServAddr; /* Local address */
	struct sigaction myAction;       /* For setting signal handler */
    struct sockaddr_in echoClntAddr; /* Client address */
    int clntLen;            /* Length of client address data structure */
	char str[200];
	FILE *fp = freopen("configuration.txt","r",stdin);
	if(fp == NULL){
	 printf("Done Not finding the file\n");
	}
	gets(str);
	lc.name = str[0];
	gets(str);
	lc.port_num = atoi(str);
	int  j = 0;
	int i = 0;
	int count = 0;
	for(j = 0 ; j < 120;j++)
		for(i =0; i < 120; i++)
			if(i == j)
				distArr[i][j] = 0;
			else
				distArr[i][j] = 10000;
	j = 0;
	while(gets(str)){
		tempHolder temp = stringTokennizer(str);
		strcpy(lc.link_info[j][0],temp[0]);
		strcpy(lc.link_info[j][1],temp[1]);
		strcpy(lc.link_info[j][2],temp[2]);
		update[lc.link_info[j][0][0]-65] = 1;
		distArr[lc.name][lc.link_info[j][0][0]] = atoi(lc.link_info[j][1]);
		j++;
	}
	printf("j = %d",j);
	count = j;
	totalUpdate = j;
	for( j = 0 ; j  < count; j++)
		printf("%s %s %s\n",lc.link_info[j][0],lc.link_info[j][1],lc.link_info[j][2]);
	printf("my port number %d\n",lc.port_num);
	echoServPort = lc.port_num;//atoi(argv[1]);  /* First arg:  local port */
	strcpy(add, "127.0.0.1");
	int pid = fork();
	//initialize_signal(myAction);
	if(pid == 0){
		//tecpClient(echoServPort,add);
	}else{
		myAction.sa_handler = CatchAlarm;
		if (sigfillset(&myAction.sa_mask) < 0) /* block everything in handler */
			DieWithError("sigfillset() failed");
		myAction.sa_flags = 0;
		if (sigaction(SIGALRM, &myAction, 0) < 0)
			DieWithError("sigaction() failed for SIGALRM");
		printf("Setting timer\n");
		//sleep(1);
		
		/* Create socket for incoming connections */
		if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
			DieWithError("socket() failed");
      
		/* Construct local address structure */
		memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
		echoServAddr.sin_family = AF_INET;                /* Internet address family */
	    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
		echoServAddr.sin_port = htons(echoServPort);      /* Local port */

		/* Bind to the local address */
		if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
		    DieWithError("bind() failed");

		/* Mark the socket so it will listen for incoming connections */
	    if (listen(servSock, MAXPENDING) < 0)
	      DieWithError("listen() failed");
		alarm(TIMEOUT_SECS);
		for (;;) /* Run forever */
		{
        /* Set the size of the in-out parameter */
		    clntLen = sizeof(echoClntAddr);
			if(errno == EINTR){
				printf("Alarm went off\n");
				alarmSetting(myAction);
			}else{
				printf("Setting Alarm Again\n");
				alarmSetting(myAction);
			}
			printf("I am here\n");
        /* Wait for a client to connect */
	        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0){
				//DieWithError("accept() failed");
				if(global_warming == 1){
					global_warming = 0;	
					printf("global interruption\n");
					tecpClient(echoServPort,add,lc,update,count,totalUpdate,distArr);
				}else{
					printf("global value %d %d\n",global_warming,tries);
					DieWithError("accept() failed");
				}
			}else{
				printf("Server Address %s\n", inet_ntoa(echoServAddr.sin_addr));
				printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
				alarm(0);
				HandleTCPClient(clntSock);
				shutdown(clntSock,2);
				close(clntSock);
				//alarm(TIMEOUT_SECS);
			}

			/* clntSock is connected to a client! */			
		}
    /* NOT REACHED */
	}
	alarm(0);
	return 1;
}
void CatchAlarm(int ignored)     /* Handler for SIGALRM */{
	 tries += 1;
	 global_warming = 1;
	
}