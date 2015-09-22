#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include<unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>


void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno,lngth,len,cid[1];

    struct sockaddr_in serv_addr;
    struct hostent *server;

    pid_t p;

    char buf[2048],name[30],msg[2079],*m;


    if (argc < 3)
    {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    portno = atoi(argv[2]);

    printf("Please enter your name: \n");
    scanf("%[^\n]s",name);

    //Create a client socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; //Internet address domain

    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(portno); //Converting to network byte order

    //Establish connection between client and server
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");


    memset(buf,0,sizeof(buf));
    //printf("%s\n",buf);



    p = fork();

    if(p==0)
    {
    	while(1)
    	{
    		memset(buf,0,sizeof(buf));
    		memset(msg,0,sizeof(msg));

    		//printf("Enter the command you want to execute\n");
    		scanf(" %[^\n]s",buf);
    		printf("\n");

    		strcat(msg,name);
    		strcat(msg," : ");
    		strcat(msg,buf);

    		write(sockfd,msg,sizeof(msg));

    	}

    }

    else
    {
        	while(1)
        	{
        		//m= (char *)malloc(2079);
        		memset(msg,0,sizeof(msg));
        		read(sockfd,msg,sizeof(msg));
        		printf("%s \n\n",msg);
        		//free(m);
        	}

    }

    	/*
    		while(1)
    		{
    		printf("Enter the command you want to execute\n");
    		scanf(" %[^\n]s",buf);
    		write(sockfd,buf,sizeof(buf));

    		//memset(buf,'\0',1024);
    		read(sockfd,buf,sizeof(buf));
    		printf("%s \n",buf);
    		}

    		*/

    	exit(0);






    return 0;
}
