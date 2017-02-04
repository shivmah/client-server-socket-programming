#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <ctype.h>
#include <signal.h>
#include <stdlib.h>
#define len_of_unique_name 11
struct connection                        
{
	long int unique_id;
	char unique_name[len_of_unique_name];
		
}my;

int pid,sockfd;


void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void store_my_conn_info(int sockfd)
{
	char buffer[256];
	memset(buffer,'\0',256);
	int n1 = read(sockfd,buffer,256);
	if (n1 < 0) error("ERROR reading from socket\n");
	printf("%s\n",buffer);
	
	if(strcmp(buffer,"Connection Limit Exceeded !!")==0)
	{
		exit(0);
	}
	
	char *token;
	
	token=strtok(buffer,"_");
		
	my.unique_id=atoi(token);
	strcpy(my.unique_name,strtok(NULL,"_"));
	printf("%s\n",strtok(NULL,"_"));

}

void kill_me()
{
    
   // printf("\nCtrl+C not responding... try Ctrl+\\ \n");
      	char buffer[256];
        bzero(buffer,256);
        buffer[0]='\0';
    	sprintf(buffer,"kill_%ld",my.unique_id);
    	printf("%s\n",buffer);	
	int n1=write(sockfd,buffer,256);
	if (n1 < 0) error("ERROR writing to socket\n");
	
   	
   	 printf("acha chalta hu duaoo me yaad rakhna n1 :%d\n",n1);
   	 fflush(stdout);
   	 close(sockfd);
   	 kill(getppid(),SIGKILL);
   	 exit(0);
    
}

void kill_me_signal()
{
    
   // printf("\nCtrl+C not responding... try Ctrl+\\ \n");
	char buffer[256];
	bzero(buffer,256);
	buffer[0]='\0';
	sprintf(buffer,"kill_%ld",my.unique_id);
	printf("%s\n",buffer);	
	int n1=write(sockfd,buffer,256);
	if (n1 < 0) 
		error("ERROR writing to socket\n");


	printf("acha chalta hu duaoo me yaad rakhna!!\n");
	fflush(stdout);
	kill(getppid(),SIGKILL);
	exit(0);
    
}

int main(int argc, char *argv[])
{
	int portno, n1,n2;
	int count;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char sc[2]=";";
	char sn[2]="\n";
	char buffer[256];
	char keyword[256];
	char buffer_copy[256];
    
    
    
	if (argc < 3) 
	{
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(0);
	}
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sockfd < 0) 
		error("ERROR opening socket");
	server = gethostbyname(argv[1]);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, 
	 (char *)&serv_addr.sin_addr.s_addr,
	 server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting\n");
       
  
        // stroring my id and name
        store_my_conn_info(sockfd);
     
	
		
		if(pid=fork()==0)
		{
			//read process to read msgs from server and display them to console
			char msg[256];
			signal(SIGINT, kill_me);
			while(1)
			{
				msg[0]='\0';
				bzero(msg,256);
				n1=read(sockfd,msg,256);
				if (n1 < 0) error("ERROR reading to socket 1\n");
	    			printf("%s\n",msg);
	
			}
		}
		else
		{
			    // write process
			    while(1)
			    {
			    	
				    	int c,k=0;
				    	printf("\n\n\n1: who are online?\n2: send msg\n3:disconnect\n\n\n\n" );
				    	printf(">>>Enter choice\n\n\n");
				    	scanf("%d",&c);
				    	if(isdigit(c+'0')==0)
				    	{
				    		printf("Enter a valid digit\n");
				    		continue;
				    	}
				    	
				    	switch(c)
				    	{
				    		bzero(buffer,256);
				    		case 1:
				    			
				    			n1=write(sockfd,"q_1",256);
				    			if (n1 < 0) error("ERROR writing to socket\n");
				    			break;
				    		
				    		case 2:	
				    				
				    				printf("\n\nwrite id or name to send message\n@");
					    			long int id;
					    			scanf("%ld",&id);
					    			char msg[256];
					    			printf("\n@%ld:",id);
					    			
					    			sprintf(buffer,"msg_%ld-%ld-",my.unique_id,id);
					    			
					    			fgets(msg,256,stdin);
					    			fgets(msg,256,stdin);
					    			
					    			
					    			strcat(buffer,msg);
					    			n1=write(sockfd,buffer,256);
					    			
					    			if (n1 < 0) error("ERROR writing to socket\n");
				    				break;
				    			
				    		case 3:

				    				kill_me();
				    				break;
				    		
				    		default :
					    			printf("Invalid \n");
					    			break;
				    			
				    			
				    			
				    	}
				    
				    
			    }
		}	    

   
    return 0;
}

