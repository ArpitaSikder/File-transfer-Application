/* A simple echo server using TCP */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define SERVER_TCP_PORT 3000	/* well-known port */
#define BUFLEN		256	/* buffer length */

int echod(int);
void reaper(int);


struct PDU { //PDU Structure
char type;
int length;
char data[100];
}pdu1, pdu2;

int main(int argc, char **argv)
{
	int 	sd, new_sd, client_len, port;
	struct	sockaddr_in server, client;

	switch(argc){
	case 1:
		port = SERVER_TCP_PORT;
		break;
	case 2:
		port = atoi(argv[1]);
		break;
	default:
		fprintf(stderr, "Usage: %d [port]\n", argv[0]);
		exit(1);
	}

	/* Create a stream socket	*/
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "Can't creat a socket\n");
		exit(1);
	}

	/* Bind an address to the socket	*/
	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sd, (struct sockaddr *)&server, sizeof(server)) == -1){
		fprintf(stderr, "Can't bind name to socket\n");
		exit(1);
	}

	/* queue up to 5 connect requests  */
	listen(sd, 5);

	(void) signal(SIGCHLD, reaper);

	while(1) {
	  client_len = sizeof(client);
	  new_sd = accept(sd, (struct sockaddr *)&client, &client_len);
	  if(new_sd < 0){
	    fprintf(stderr, "Can't accept client \n");
	    exit(1);
	  }
	  switch (fork()){
	  case 0:		/* child */
		(void) close(sd);
		exit(echod(new_sd));
	  default:		/* parent */
		(void) close(new_sd);
		break;
	  case -1:
		fprintf(stderr, "fork: error\n");
	  }
	}
}

int echod(int sd)
{
	struct stat st;
  int fileSize;
  int file;
  int read_return1;
  int read_return2;
	int link[2];
	char	ls[5000],temp[500];
	pid_t pid;
  while((read_return1 = read(sd, &pdu1, 107))!=0){


		//uploading
    if(pdu1.type=='U'){
//pdu1.data="ighuyg";
      printf("pdu1.data=%s here\n", pdu1.data);
      printf("loook");
     // file = open(pdu1.data, O_CREAT | O_WRONLY, S_IRUSR| S_IWUSR);//changed this
     // file=open("desktop.txt",O_CREAT | O_RDWR);
     //file = open("desktop.txt", O_CREAT | O_WRONLY, S_IRUSR| S_IWUSR);
     file = fopen("desktop.txt","w");
     printf("going in to");
      pdu2.type = 'R';
      write(sd, &pdu2, read_return1+7);//write ready
      read_return1 = read(sd, &pdu1, 107); //read reposnse from client should be 'F'
      printf("going in to");
      printf("type=%c",pdu1.type);
      printf("going in");
      if(pdu1.type=='F'){
	 printf("inside");
      fileSize = pdu1.length;
        printf("filesz:%d",fileSize);  
       while(fileSize==0){  //changed this >0
	printf("I am here");
         printf("filename:%s",pdu1.data);
         read_return1 = read(sd, pdu1.data, 100);
         printf("data:%s",pdu1.data);
         
         //write(file, pdu1.data, read_return1); //this is changed
        
         fileSize-=(read_return1);
       }
     fprintf(file,"%s", pdu1.data);
       if(fileSize < 0)
        close(file);
      }
    }


		//changing directory
		if(pdu1.type == 'P'){
			printf("type = %c,length = %d, dir = %s\n", pdu1.type, pdu1.length, pdu1.data);
			chdir(pdu1.data);
			char *const parmList2[] = {"/bin/ls",NULL , NULL, NULL};
			if((pid = fork()) == 0)
				execv("/bin/ls", parmList2);
		}

		//dowload request from client
		if(pdu1.type == 'D'){ //client wants to download
				printf("download activated\n");
				pdu1.length = st.st_size; //get size of file
				printf("type: %c\n", pdu1.type);
				printf("size: %d\n", pdu1.length);
				printf("file: %s\n", pdu1.data);
				file = open(pdu1.data, O_RDONLY);
				if(file >= 0){//file exists
					pdu1.type = 'F';
					if(stat(pdu1.data, &st) == 0)
						pdu1.length = st.st_size; //get size of file
						printf("type: %c\n", pdu1.type);
						printf("size: %d\n", pdu1.length);
						printf("file: %s\n", pdu1.data);
					write(sd, &pdu1, 107);
					while((read_return1 = read(file, pdu1.data, 100)) != 0){
						printf("data: %s\n", pdu1.data);
						write(sd, pdu1.data, read_return1);
					}
				}else{
					char l[] = "The file does not exist.";
					pdu1.type = 'E';
					strcpy(pdu1.data,l);
					pdu1.length = sizeof(l);
					write(sd, &pdu1, 100);
				}
			}
			//list request from client
			if(pdu1.type =='L'){
				char *const parmList[] = {"/bin/ls", NULL, NULL, NULL};
				pipe(link);
				if((pid = fork()) == 0){
						dup2 (link[1], STDOUT_FILENO);
						close(link[0]);
						close(link[1]);
						execv("/bin/ls", parmList);
					}
					int nbytes = read(link[0], ls, sizeof(ls));
					pdu1.type = 'I';
					pdu1.length = nbytes;
					printf("ls: \n%s\n", ls);
					write(sd, &pdu1, 107);
					write(sd, ls, nbytes);
			}


		int i;
		pdu1.length = 0;
		for(i = 0; i<100; i++){ //this clears the pdu data potions to NULL
			pdu1.data[i] = NULL;
		}
  }
    return(0);

  }


void	reaper(int sig)
{
	int	status;
	while(wait3(&status, WNOHANG, (struct rusage *)0) >= 0);
}
