#include <sys/stat.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <stdbool.h>
#include <fcntl.h>

#define SERVER_TCP_PORT 3000	/* well-known port */
#define BUFLEN		256	/* buffer length */

struct PDU { //PDU Structure
char type;
int length;
char data[100];
}pdu1, pdu2,buf;

int main(int argc, char **argv){
	struct stat st;
	char buf1[100], buf2[100], buf3[100];
	int read_return1, file, read_return2, fileSize;

	int 	n, i, bytes_to_read, x, j=0;
	int n1;
	bool test = false;
	int 	sd, port;
	struct	hostent		*hp;
	struct	sockaddr_in server;
	char	*host, *bp, rbuf[BUFLEN], sbuf[BUFLEN], user_input[BUFLEN], user_in[BUFLEN];
	FILE * file1;
	FILE * file2;
        FILE *fileul;
	

	switch(argc){
	case 2:
		host = argv[1];
		port = SERVER_TCP_PORT;
		break;
	case 3:
		host = argv[1];
		port = atoi(argv[2]);
		break;
	default:
		fprintf(stderr, "Usage: %s host [port]\n", argv[0]);
		exit(1);
	}

	/* Create a stream socket	*/
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "Can't creat a socket\n");
		exit(1);
	}

	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if (hp = gethostbyname(host))
	  bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);
	else if ( inet_aton(host, (struct in_addr *) &server.sin_addr) ){
	  fprintf(stderr, "Can't get server's address\n");
	  exit(1);
	}

	/* Connecting to the server */
	if (connect(sd, (struct sockaddr *)&server, sizeof(server)) == -1){
	  fprintf(stderr, "Can't connect \n");
	  exit(1);
	}
	while(1){
		//User interface
		printf("COMMANDS: \"dl\" for download, \"ul\" for upload, \"cd\" for change server directory, \"ls\" for list files in server directory \n");
		printf("Please enter a command\n");
		read_return1 = read(0, user_input, 100);
                user_input[read_return1-1]='\0';
                

	//download request
	if(strcmp(user_input,"dl")==0){
		int templength=0;
		printf("please enter file name\n");
		read_return2 = read(0, user_in, 100);
		user_in[read_return2] = '\0';
		pdu1.type = 'D';
		pdu1.length = strlen(user_in);
		strcpy(pdu1.data, user_in);
		//printf("type: %c\n", pdu1.type);
		write(sd, &pdu1, read_return2+7); //send request to the server
		read_return1 = read(sd, &pdu2, 107); //read the response (Good info || bad info)
		if(pdu2.type == 'F'){//Good information
		file = open(user_input, O_CREAT | O_WRONLY, S_IRUSR| S_IWUSR);
			
                   
			templength = pdu2.length;
			while(templength >= 0){ //Read from server
				read_return1 = read(sd, pdu2.data, 100);
				write(file, pdu2.data, read_return1);
				templength -= 100;
                         
			   
		 
			}
//extra        
	         FILE *fp; 
		fp = fopen("test2.txt","w");
		
		while(pdu2.type == 'F' ){
		
	
                        //printf("PPPP\n");
			write(1, pdu2.data, read_return1);
                        //printf("TTTT\n");
			
                        //printf("ggggg\n");
                        printf("a:%s",pdu2.data);
			
		        fprintf(fp , "%s\n" , pdu2.data); 
                        
                        exit(0); 
		}
               
	}
		
		else{
			read_return1 = read(sd, &pdu2, 100);
			if(pdu2.type == 'E'){
				printf("%s\n", pdu2.data);
			}
		}	
		
   
	}

	//file list request
	if(strcmp(user_input,"ls")== 0){
		pdu1.type = 'L';
		write(sd, &pdu1, 107);
		read(sd, &pdu1, 107);
		int templength = pdu1.length;
		if(pdu1.type == 'I'){
			printf("---------------------\n");
			while(templength >= 0){
				read_return1 = read(sd, pdu1.data, 100);
				write(1, pdu1.data, read_return1);
				templength -= 100;
			}
			printf("---------------------\n");
		}
	}
         char filename[256];

	int a;
	//client file upload
	if(strcmp(user_input,"ul")==0){
	printf("Please enter the name of a file that you would like to upload:\n");
	read_return1 = read(0, buf1, 100);//user input for file that client wants to upload 
	buf1[read_return1] = '\0';
	printf("the file you entered is: %s\n", buf1);//buf1
        printf("b:%s",buf1);//buf1
        //filename = buf1;
	strcpy(filename,buf1);
        printf("filename:%s",filename);
	//file = open(buf1, O_RDONLY);
        //file = open(pdu1.data,O_RDWR); //added this
	//file = fopen(pdu1.data,"w"); //added this
	file = open("desktop.txt", O_RDWR); //only working
       // file = fopen(filename, "rb");
	if(file >= 0){
		pdu1.type = 'U';
		strcpy(pdu1.data, buf1);
		pdu1.length = read_return1;
		write(sd, &pdu1, read_return1+7); //sending file name to server
		printf("pdu1.type = %c\n", pdu1.type);
		printf("pdu1.data %s\n", pdu1.data);
		if(stat(pdu1.data, &st) == 0)
		fileSize = st.st_size;
		printf("filesize:%d",fileSize);
	}else{
		printf("The filename that you entered does not exist.\n");
	}
        printf("I am writing to the socket now");
	read_return2 = read(sd, &pdu2, 100);
	printf("pdu2.type = %c\n", pdu2.type);
	if(pdu2.type == 'R'){
		pdu1.type = 'F';
		pdu1.length = fileSize;
		printf("type = %c\n", pdu1.type);
		printf("fileSize = %d\n", fileSize);
		printf("pdu1.data = %s\n", pdu1.data);
		write(sd, &pdu1, 5);
		while((read_return2 = read(file, pdu1.data, 100))!= 0){//writing file data to server
			printf("pdu1.data = %s\n", pdu1.data);
			printf("read_return=%d\n", read_return2);
			write(sd, pdu1.data, read_return2);
		}

             
	}
     // else{printf("error");}
//fclose was added but it still closes the socket
//fclose(file); this is changed
//exit(0);
}

	//changing servers directory
	if(strcmp(user_input,"cd")== 0){
		pdu1.type = 'P';
		printf("Please enter the directory you would like to go to within the server\n");
		read_return1 = read(0, pdu1.data, 100);
		pdu1.data[read_return1] = '\0';
		pdu1.length = read_return1;
		write(sd, &pdu1, read_return1+7);
		int i;
		for(i = 0; i<100; i++){
			pdu1.data[i] = NULL;
		}
	}
}
	close(sd);
	return(0);
}
