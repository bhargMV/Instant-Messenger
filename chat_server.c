#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void parse(char *buff, char **comm)
{
     while (*buff != '\0')
     {
          while (*buff == ' ' || *buff == '\t' || *buff == '\n')
               *buff++ = '\0';

          *comm++ = buff;

          while (*buff != '\0' && *buff != ' ' && *buff != '\t' && *buff != '\n')
               buff++;
     }

     *comm = '\0';
}


int main(int argc, char *argv[])
{
    /* master file descriptor list */
    fd_set master;

    /* temp file descriptor list for select() */
    fd_set read_fds;

    /* server address */
    struct sockaddr_in serveraddr;

    /* client address */
    struct sockaddr_in clientaddr;

    /* maximum file descriptor number */
    int fdmax;

    /* listening socket descriptor */
    int listener;

    /* newly accept()ed socket descriptor */
    int newfd;

    /* buffer for client data */
    char buf[2048],temp[30];

    int nbytes,PORT;

    /* for setsockopt() SO_REUSEADDR, below */

    int yes = 1;
    int addrlen;
    int i, j,t_pid;
    char *comm[15];


    if (argc < 2)
    {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    PORT = atoi(argv[1]);

    /* clear the master and temp sets */
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    /* get the listener */
    if((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
    	error("Server-socket() error!");
    	exit(1);
    }

    //printf("Server-socket() is OK...\n");

    /*"address already in use" error message */
    if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
    	error("Server-setsockopt() error!");
    	exit(1);
    }

    //printf("Server-setsockopt() is OK...\n");

    /* bind */
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(PORT);
    memset(&(serveraddr.sin_zero), '\0', 8);


    if(bind(listener, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
    {
        error("Server-bind() error lol!");
        exit(1);
    }

    //printf("Server-bind() is OK...\n");


    /* listen */
    if(listen(listener, 10) == -1)
    {
         error("Server-listen() error lol!");
         exit(1);
    }

    //printf("Server-listen() is OK...\n");


    /* add the listener to the master set */
    FD_SET(listener, &master);

    /* keep track of the biggest file descriptor */
    fdmax = listener; /* so far, it's this one*/


    /* loop */
    for(;;)
    {
    	/* copy it */
    	read_fds = master;

    	if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
    	{
    		error("Server-select() error lol!");
    		exit(1);
    	}

    	//printf("Server-select() is OK...\n");


    	/*run through the existing connections looking for data to be read*/

    	for(i = 0; i <= fdmax; i++)
    	{

    		if(FD_ISSET(i, &read_fds))
    		{
    			/* we got one... */

    			if(i == listener)
    			{
    				/* handle new connections */

    				addrlen = sizeof(clientaddr);

    				if((newfd = accept(listener, (struct sockaddr *)&clientaddr, &addrlen)) == -1)
    				{
    					error("Server-accept() error !");
    				}

    				else
    				{
    					//printf("Server-accept() is OK...\n");

    					FD_SET(newfd, &master); /* add to master set */

    					if(newfd > fdmax)
    					{
    						/* keep track of the maximum */
    						fdmax = newfd;
    					}

    					//printf("%s: New connection from %s on socket %d\n", argv[0], inet_ntoa(clientaddr.sin_addr), newfd);

    				}

    			}

    			else
    			{
    				/* handle data from a client */
    				if((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0)
    				{
    					/* got error or connection closed by client */

    					if(nbytes == 0)
    						/* connection closed */

    						printf("%s: socket %d hung up\n", argv[0], i);

    					else
    						error("recv() error!");

    					/* close it... */
    					close(i);

    					/* remove from master set */
    					FD_CLR(i, &master);

    				}

    				else
    				{
    					/* we got some data from a client*/

    					//sprintf(temp,"   ----  by Stranger %d\n",i);
    					//strcat(buf,temp);
    					printf("%s \n",buf);

    					for(j = 0; j <= fdmax; j++)
    					{
    					/* send to everyone! */

    						if(FD_ISSET(j, &master))
    						{
    					       /* except the listener and ourselves */

    					       if(j != listener && j!=i)
    					       {
    					              write(j, buf, strlen(buf));
    					       }

    						}

    					}
    				}

    			}

    		}

    	}

    }

    return 0;

 }
