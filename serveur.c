#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>


int main(int argc, char **argv){
    int dss ,dsc;
    int clientlen;
    struct sockaddr_in srvAddr ,cltAddr;
    char * msg=calloc(1024,sizeof(char));
    int dsClt[20];
    int n , cptClt,maxDS,i,j;
    fd_set readfds,readAllfd ;
    dss = socket(AF_INET,SOCK_STREAM,0);
    memset((char *) &srvAddr,'\0',sizeof(srvAddr));
    srvAddr.sin_family = AF_INET; 
    srvAddr.sin_addr.s_addr = htons(INADDR_ANY);
    srvAddr.sin_port = htons(1234);
    bind(dss,(struct sockaddr *)&srvAddr,sizeof(srvAddr));
    listen(dss,5);
    clientlen=sizeof(cltAddr);
    cptClt = 0;
    FD_ZERO(&readAllfd);
    FD_SET(dss,&readAllfd);
    maxDS=dss;
    while(1){
    	FD_ZERO(&readfds);
    	readfds=readAllfd;
    	select(maxDS+1,&readfds,NULL,NULL,NULL);
    	if(FD_ISSET(dss,&readfds)){
    		dsc = accept(dss,(struct sockaddr *) &cltAddr,&clientlen);
    		cptClt++;
    		FD_SET(dsc , &readAllfd);
    		dsClt[cptClt-1]=dsc;
    		if(dsc >maxDS) maxDS=dsc;
    		printf("Nouveau client communiquant via DS :%d \n",dsc);
    	}
    	else {
    		for (i=0;i<cptClt;i++){
    			dsc = dsClt[i];
    			if(FD_ISSET(dsc,&readfds)){
    				memset(msg,'\0',1024);
    				n=recv(dsc,msg,1024,0);
    				if (n==0){
    					FD_CLR(dsc,&readAllfd);
    					for(j=i;j<cptClt-1;j++) dsClt[j]=dsClt[j+1];
    					cptClt--;
    					close(dsc);
    					if((dsc==maxDS)&& cptClt !=0)
    						for(j=1;j<cptClt;i++)
    							if(dsClt[j]>maxDS) maxDS=dsClt[j];
    					memset(msg,'\n',1024);
    					printf("le client communiquant via DS : %d s'est deconnecte\n",dsc );
    					break;
    				}
    				else if (n>0){
						for(int i=0;i<cptClt;i++){
							if(dsClt[i]!=dss && dsClt[i]!=dsc){
								send(dsClt[i],msg,strlen(msg),0);
							}
						}
    					memset(msg,'\0',1024);
    					break;
    				}
    			}
    		}

    	}
    }    
	close(dss);
	exit(0);
}
