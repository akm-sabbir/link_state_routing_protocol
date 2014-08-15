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
#include<string.h>
#include<stdlib.h>
#include "client.h"
#define ECHOMAX 255     /* Longest string to echo */

#define RCVBUFSIZE 1024
#define SECS 2
int setCheck;
void DieWithError(char errorMessage[]);  /* External error handling function */
void alarmSettings(struct sigaction &myAction){
	if(sigfillset(&myAction.sa_mask) < 0) /* block everything in handler */
		DieWithError("sigfillset() failed");
	myAction.sa_flags = 0;
	if(sigaction(SIGALRM, &myAction, 0) < 0)
		DieWithError("sigaction() failed for SIGALRM");
	alarm(SECS);
}
void catchAlarm(int ignore){
	  printf("catch Alarm\n");
      setCheck = 1;
}

int tecpClient(int serverPort,char *argv,local_machine lc,int update[],int count,int &totalUpdate,int distArr[][120])
{
	FILE *fp = freopen("updateDatasent.txt","a+",stdout);
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort;     /* Echo server port */
    char *servIP;                    /* Server IP address (dotted quad) */
    char *echoString = new char[50];                /* String to send to echo server */
    char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
    unsigned int echoStringLen;      /* Length of string to echo */
    int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv() 
                                            and total bytes read */
	struct sigaction myAction;
	char endString[5];
	strcpy(endString,"end");
    servIP = argv;             /* First arg: server IP address (dotted quad) */
	echoServPort = serverPort; /* Use given port, if any */
	setCheck = 0;
	myAction.sa_handler = catchAlarm;
    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
    echoServAddr.sin_family = AF_INET;             /* Internet address family */
    echoString[0] = lc.name;
	printf("%c\n",lc.name);
	echoString[1]=' ';
	echoString[2] = totalUpdate+48;
	printf("%d\n",totalUpdate);
	//echoString[3]=' ';
    echoStringLen = strlen(echoString);          /* Determine input length */
     for(int j = 65; j < 73; j++){
		    if(update[j-65] == 1){
				char tempString[20];
				char temp2[20];
				strcat(echoString," ");
				sprintf(temp2,"%c",j);
				strcat(echoString,temp2);
				sprintf(tempString, "%d", distArr[lc.name][j]);
				printf("%c distArr[lc.name][j]\n",j,distArr[lc.name][j]);
				strcat(echoString," ");
				strcat(echoString,tempString);
				echoStringLen = strlen(echoString);			
			}
	}
	fclose(fp);
	//totalUpdate = 0;
	strcat(echoString,"\0");
	//printf("Client : %s\n",echoString);
    /* Send the string to the server */
	int checkBeforereset = 0;
 for(int i = 0; i < count ;i++){
    if(setCheck == 1){
		setCheck = 0;
		checkBeforereset = 1;
	}
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");
	echoServAddr.sin_addr.s_addr = inet_addr(lc.link_info[count][2]);
	echoServAddr.sin_port = htons(lc.port_num); /* Server port */
	if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0){
        DieWithError("connect() failed");
		checkBeforereset = 1;
		break;
	}else
		printf("Successfully conntected\n");
	while(1){	
		 //sleep(1);
		 //strcpy(echoString,"");
		printf("sent data %s\n",echoString);
		if (send(sock, echoString, echoStringLen, 0) != echoStringLen)
			DieWithError("send() sent a different number of bytes than expected");
		printf("waiting for response\n");
		int recieved = 0;
		alarmSettings(myAction);
		if ((recieved = recv(sock, echoBuffer, RCVBUFSIZE, 0)) < 0) {
			DieWithError("Failed to receive additional bytes from client");
		}
		alarm(0);
		if(setCheck == 1 && recieved ==0)
			break;
		if(strcmp(echoString,"end") == 0){
			printf("Data sent ");
			break;
		}
		if(recieved != 0)
			printf("%s\n",echoBuffer);
		strcpy(echoString,"end");
		echoStringLen = strlen(echoString);          /* Determine input length */
		 
	}
	shutdown(sock,2);
    close(sock);
 }
	if(checkBeforereset == 0){
		for(int ko = 65; ko < 73; ko++)
			update[ko-65] = 0;
		totalUpdate = 0;
	}
	alarm(0);
    /* Receive the same string back from the server */
    totalBytesRcvd = 0;
     	/* Setup to print the echoed string */
    printf("\n");    /* Print a final linefeed */
	
 //   exit(0);
}
