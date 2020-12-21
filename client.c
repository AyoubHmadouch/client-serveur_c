
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define BUFFERSIZE 2048
//fonctions:
void send_recv(int k, int ds,char *nom)
{
	char msg_send[BUFFERSIZE];
	char msg_recv[BUFFERSIZE];
	int nb_recv;

	if (k == 0){
		fgets(msg_send,BUFFERSIZE,stdin);
		if (strcmp(msg_send,"exit") == 0) {
			exit(0);
		}else{
			char *message=malloc(BUFFERSIZE*sizeof(char));
			sprintf(message,"%s:%s",nom,msg_send);
			send(ds,message,strlen(message),0);
		}
	}else {
		nb_recv=recv(ds,msg_recv,BUFFERSIZE,0);
		msg_recv[nb_recv] = '\0';
		printf("%s",msg_recv);
		fflush(stdout);
	}
}
void connect_socket(int *sockfd, struct sockaddr_in *server_addr)
{
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}
	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(1234);
	server_addr->sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(server_addr->sin_zero, '0', sizeof server_addr->sin_zero);
	
	if(connect(*sockfd, (struct sockaddr *)server_addr, sizeof(struct sockaddr)) == -1) {
		perror("probleme au niveau de connect");
		exit(1);
	}
}
int main(int argc,char **argv){
	int i,ds,fdmax;
	struct sockaddr_in aSrv;
	fd_set readfds,readAllfd;
	connect_socket(&ds,&aSrv);
	FD_ZERO(&readAllfd);
	FD_ZERO(&readfds);
	FD_SET(0,&readAllfd);
    FD_SET(ds,&readAllfd);
    fdmax=ds;
    char nom[10];
	printf("Saisir votre nom :");
	scanf("%s",nom);
	fflush(stdout);
    while(1){
    	readfds=readAllfd;
    	if(select(fdmax+1,&readfds,NULL,NULL,NULL) == -1){
    		perror("probleme au niveau de select");
			exit(4);
    	}
    	for(i=0;i<=fdmax;i++){
    		if(FD_ISSET(i,&readfds)){
    		 	send_recv(i,ds,nom);
			}
    	}	
    }
	
	printf("Vous etes deconnecte");
	close(ds);
	return EXIT_SUCCESS;
}
