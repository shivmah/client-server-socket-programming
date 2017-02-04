/* server process*/
//references //http://stackoverflow.com/questions/3597975/how-to-limit-connection-in-linux-socket-programming   
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include<sys/time.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <signal.h>
#include <stdlib.h>

#define max_no_clients 5
#define SHMSZ  5000000
#define len_of_unique_name 10
#define length_of_msg 256
#define size_of_msg_queue 100

int shmid;
key_t key1=9425;
struct message
{
	char msg[length_of_msg];
	long int sender_id;
	long int reciever_id;
	int r_i,s_i;
	
};

struct connection                        
{
	long int unique_id;
	char unique_name[len_of_unique_name];
	struct timeval tm;
	int newsockfd;
	int i;
	struct message msg_queue[size_of_msg_queue]; 
	int front,rear;
	sem_t mutex;
		
};



struct all
{
	struct connection c_info[max_no_clients];
	int no_of_clients;
	
	
	
}*all;

 FILE *log_file;

void display_msg_queue(int i)
{
	int j;
	printf("\n\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
	struct message *m;
	printf(" front %d  rear %d \n",(all->c_info[i].front),(all->c_info[i].rear));
	for(j=0;j<=(all->c_info[i].rear);j++)
	{	
		printf("%ld %ld %s\n",all->c_info[i].msg_queue[j].sender_id,all->c_info[i].msg_queue[j].reciever_id,all->c_info[i].msg_queue[j].msg);
	}
	printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\n\n");
	
	


}

int is_empty(int i)
{
	//whether queue is empty?
         if(all->c_info[i].front==all->c_info[i].rear)
	{
		return 1;
	}
	return 0;
}

void enqueue( struct message messag)
{

	int i=messag.r_i;
	all->c_info[i].msg_queue[all->c_info[i].rear]=messag;
	all->c_info[i].rear=all->c_info[i].rear+1;
}

struct message dequeue(int i)
{
	         struct message m;
		m=all->c_info[i].msg_queue[all->c_info[i].front];
		(all->c_info[i].front)++;
		return m;
}


void create_shared_memory()
{
	
	 if ((shmid = shmget(key1,SHMSZ, IPC_CREAT | 0666)) < 0)
	  {
       		 perror("main shmget");
        	exit(1);
   	  }
	
	if ((all = shmat(shmid, NULL, 0)) == (struct all*)-1)
	 {
        	perror("main shmat\n");
        	exit(1);
  	  } 
    
	int i;
	//initialising shared  memory
	for(i=0;i<max_no_clients;i++)
	{
		all->c_info[i].newsockfd=-1;
		printf("ID:%ld\nName:%s\nConnecting Time:%ld\nnewsockfd:%d\n",all->c_info[i].unique_id,all->c_info[i].unique_name,all->c_info[i].tm.tv_sec,all->c_info[i].newsockfd);	
		all->c_info[i].front=all->c_info[i].rear=0;
		// initialising semaphores
		sem_init(&(all->c_info[i].mutex),1,1);
	}

	all->no_of_clients=0;
      
}





void display_sm()
{
	printf("#########################################################################################################\n");
	int i;
	for(i=0;i<max_no_clients;i++)
	{
		printf("ID:%ld\nName:%s\nConnecting Time:%ld\nnewsockfd:%d\n",all->c_info[i].unique_id,all->c_info[i].unique_name,all->c_info[i].tm.tv_sec,all->c_info[i].newsockfd);	
	}
	printf("#########################################################################################################\n");
}
void error(const char *msg)
{
    perror(msg);
    //exit(1);
}

long int gen_unique_id()
{
	return rand()% 99999 + 10000;
}

char* gen_unique_name()
{
	char * unique_id;
	const char *charset="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	int charset_len=strlen(charset),i,random_no;
	unique_id=(char*)malloc(len_of_unique_name*sizeof(char));
	for(i=0;i<len_of_unique_name;i++)
	{
		random_no=rand() % charset_len;	
		unique_id[i]=charset[random_no];
	}
	
	return unique_id;
}



void get_time(struct timeval *tv)
{
	gettimeofday(tv, NULL);
}




int register_client(int newsockfd,FILE * log_file)
{
	
	int i=0,n1;
	char *msg=(char*)malloc(sizeof(char)*length_of_msg);
	// looking for an empty index in shared memory -> c_info
	for(i=0;i<max_no_clients;i++)
	{
		if(all->c_info[i].newsockfd==-1)
		{
			
			break;
		}
			
	}
	
	bzero(msg,strlen(msg));
	all->c_info[i].unique_id=gen_unique_id();
	strcpy(all->c_info[i].unique_name,gen_unique_name());
	get_time(&(all->c_info[i].tm));
	all->c_info[i].newsockfd=newsockfd;
	all->c_info[i].i=i;
	
	//creating and sending welcome msg
	
	bzero(msg,strlen(msg)+1);
	sprintf(msg,"%ld_%s_welcome !!\nID:%ld\nName:%s\nConnecting Time:%ld\n",all->c_info[i].unique_id,all->c_info[i].unique_name,all->c_info[i].unique_id,all->c_info[i].unique_name,all->c_info[i].tm.tv_sec);	
	n1=write(newsockfd,msg,strlen(msg)+1);
	if (n1 < 0) error("ERROR writing to socket\n");
	
	// printing to log_file
	fprintf(log_file,"%d,%d,%ld\n",n1,0,all->c_info[i].unique_id);
	fflush(log_file);
	free(msg);
	// return index of this connection in c_info array
	return i;
	
}

void send_rejection_msg(int newsockfd)
{
	char  msg_for_rejection[256];
	msg_for_rejection[0]='\0';
	strcpy(msg_for_rejection,"Connection Limit Exceeded !!");
	int n1 = write(newsockfd,msg_for_rejection,strlen(msg_for_rejection)+1);
		if (n1 < 0) error("ERROR writing to socket\n");
		
	
}
// to give client details about online clients
void online_clients(int newsockfd,FILE * log_file)
{
	
	int i,n1;
	char  msg[length_of_msg];
	msg[0]='\0';
	strcat(msg,"Id:	Name:\n");
	
	for(i=0;i<max_no_clients;i++)
	{
		
		if(all->c_info[i].newsockfd==newsockfd)
			continue;
		// concatenation of id and name of all online clients 	
		if(all->c_info[i].newsockfd!=-1)
		{
			char temp[length_of_msg];
			sprintf(temp,"%ld %s\n",all->c_info[i].unique_id,all->c_info[i].unique_name);
			strcat(msg,temp);
			
		}
	}
	
	
	n1 = write(newsockfd,msg,strlen(msg)+1);
		if (n1 < 0) error("ERROR writing to socket\n");
	fprintf(log_file,"%d,%d,%ld\n",n1,0,all->c_info[i].unique_id);	
	fflush(log_file);
}

// every server has its ow msg delivery process which will dequeue and send msg to respective client
void create_message_delivery_process(int i,FILE * log_file)
{
	int pid,k;
	char buffer[length_of_msg];
	if(pid=fork()==0)
	{
		while(1)
		{
		   
		         if(is_empty(i)==1)
		         {
		                  continue;
		         }
		         
		        
		        // ensuring Mutex
			sem_wait(&(all->c_info[i].mutex)); 
			struct message msg=dequeue(i); 
			sem_post(&(all->c_info[i].mutex));
			
			
			if(msg.sender_id==0)
			{
				continue;
			}
			
				bzero(buffer,length_of_msg);
				int n1,ii;
				int sender_sockfd=all->c_info[msg.s_i].newsockfd;
				int reciever_sockfd=all->c_info[msg.r_i].newsockfd;
				
					
				if(msg.sender_id==0 || msg.reciever_id==0)
				{
					continue;
				}
				
				// writing msg to client and into log_file
				sprintf(buffer,"@%ld:%s",msg.sender_id,msg.msg);
				n1=write(reciever_sockfd,buffer,strlen(buffer)+1);
				if (n1 < 0) error("ERROR writing to socket\n");
				fprintf(log_file,"%d,%ld,%ld\n",n1,msg.sender_id,msg.reciever_id);
				fflush(log_file);
		}
	}

}

// this method delivers a msg from client to server
void msg_delivery(char * msg,FILE * log_file)
{
	
	printf("%s\n",msg);
	long int sender,reciever,i;
	int n1;int k;
	char m[length_of_msg];
	char buffer[256];
	bzero(buffer,256);
	struct message msgg;
	
	
	sscanf(msg,"%ld-%ld-%[^\n]s",&sender,&reciever,m);
	
	
	
	
	printf("%s\n",m);
	strcpy(msgg.msg,m);
	msgg.sender_id=sender;
	msgg.reciever_id=reciever;
	// finding out index of sender in c_info 
	for(i=0;i<max_no_clients;i++)
	{
		if(msgg.sender_id==all->c_info[i].unique_id)
		{
			break;
		}
	}
	
	msgg.s_i=i;
	
	// finding out index of reciever in c_info 
	for(i=0;i<max_no_clients;i++)
	{
		if(msgg.reciever_id==all->c_info[i].unique_id)
		{
			break;
		}
	}
	
	
	msgg.r_i=i;
	// if reciever is not online 
	if(msgg.r_i>=max_no_clients)
	{
		
		bzero(buffer,256);
		sprintf(buffer,"@server:invalid Id or name: %ld",msgg.reciever_id);
		n1=write(all->c_info[msgg.s_i].newsockfd,buffer,length_of_msg+1);
		if (n1 < 0) error("ERROR writing to socket\n");
			return;
	}
	
	// if reciever is disconnected
	if(all->c_info[msgg.r_i].newsockfd==-1)
	{
		bzero(buffer,256);
		sprintf(buffer,"@server:reciever is disconnected!!: %ld",msgg.reciever_id);
		n1=write(all->c_info[msgg.s_i].newsockfd,buffer,length_of_msg+1);
		if (n1 < 0) error("ERROR writing to socket\n");
			return;
	}
	
	// if sender is the reciever
	if(msgg.s_i==msgg.r_i)
	{
		bzero(buffer,256);
		sprintf(buffer,"@server:loopback prohibited!!: %ld",msgg.reciever_id);
		n1=write(all->c_info[msgg.s_i].newsockfd,buffer,length_of_msg+1);
		if (n1 < 0) error("ERROR writing to socket\n");
			return;
	}

	sem_wait(&(all->c_info[i].mutex));
	enqueue(msgg);
	sem_post(&(all->c_info[i].mutex));
	
}



void kill_client(char * msg,FILE *log_file)
{
	long int client_id;
	sscanf(msg,"%ld",&client_id);
	int i;
	
	// sending all online clients that this vlients is gettting disconnected
	for(i=0;i<max_no_clients;i++)
	{
		if(client_id==all->c_info[i].unique_id)
		{
			all->c_info[i].newsockfd=-1;
			all->c_info[i].unique_id=0;
			all->c_info[i].i=-1;
			continue;
		}
			
		char buffer[256];
		bzero(buffer,256);
		sprintf(buffer,"%ld-%ld-I am disconnecting!!!!\n",client_id,all->c_info[i].unique_id);
		msg_delivery(buffer,log_file);
	}
	
	// decrement no_of_clients 
	(all->no_of_clients)--;
	exit(0);
	
	
	
}
int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno;
	socklen_t clilen;
         log_file=fopen("log.txt","w+");
       
	struct sockaddr_in serv_addr, cli_addr;
	int n1,n2;
	if (argc < 2) 
	{
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
	error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR on binding");
    
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	// creating shared memory
	
	
	create_shared_memory();
	
	
	
	
	while(1)
	{
			int i;
			newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
			if (newsockfd < 0) 
				error("ERROR on accept\n");
			else
			{  
				
				if(all->no_of_clients<max_no_clients)
				{	
					
					
					(all->no_of_clients)++;
					i=register_client(newsockfd,log_file); 
					 	
				}	  
				else
				{ 
					
					send_rejection_msg(newsockfd);
					close(newsockfd);
				}
				
			}
					
			
		
			
			
			
			
			
			
					
			int pid;
			//  a server child for every client 
			if((pid=fork())==0)
			{ 
				char string[256];
				// forking a new process to deliver msg from msg queue
				create_message_delivery_process(i,log_file);
				while(1)
				{
					// read process
					bzero(string,256);
					n1=read(newsockfd,string,256);
					if(n1<0)
						error("ERROR reading from socket 1\n");
					
					
					
					char copy_string[256];
					strcpy(copy_string,string);
					char *request;
					request=strtok(copy_string,"_");
	
					if(strcmp("q",request)==0)
					{
						// query 1 online client
						online_clients(newsockfd,log_file);
						
					}
					else if(strcmp("msg",request)==0)
					{
						// send a msg
						char * token=strtok(NULL,"_");
						printf("%s\n",token);
						msg_delivery(token,log_file);
					}
					else if(strcmp("kill",request)==0)
					{
						// killing the client safely
						kill_client(strtok(NULL,"_"),log_file);
					
					}
					
			
				}
			
				
			}
		}		     
	     
     close(newsockfd);
     close(sockfd);
     return 0; 
}
