#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h> 
#include <time.h>

#define SERVER_PORT_ID 7000
#define CLIENT_PORT_ID 6066
#define SERVER_HOST_ADDR "127.0.0.1"
#define MAXSIZE 512

#define DEBUG if(debug == 1)

int debug = 0;
int sockid, newsockid,i,getfile,ack,msg,msg_2,c,len;
int no_writen,start_xfer, num_blks,num_last_blk;
struct sockaddr_in my_addr, server_addr; 
FILE *fp; 
char in_buf[MAXSIZE];
char fileIn[MAXSIZE];

int readn(int sd,char *ptr,int size);
int writen(int sd,char *ptr,int size);

int setupConnection(){
  int k=(rand()%1000+1);
  if ((sockid = socket(AF_INET,SOCK_STREAM,0)) < 0)
  {  
    exit(0);
  }
  bzero((char *) &my_addr,sizeof(my_addr));
  my_addr.sin_family = AF_INET;
  my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  my_addr.sin_port = htons(CLIENT_PORT_ID+k);
  if (bind(sockid ,(struct sockaddr *) &my_addr,sizeof(my_addr)) < 0)
  {
    return -1;
  }                                        
  bzero((char *) &server_addr,sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(SERVER_HOST_ADDR);
  server_addr.sin_port = htons(SERVER_PORT_ID);
  if (connect(sockid ,(struct sockaddr *) &server_addr,sizeof(server_addr)) < 0)
  {
    exit(0);
  }
}

void reportFile(){
    if ((writen(sockid,fileIn,len+1))< 0)
    {
      exit(0);
    }
}

void openfile(){
  if ((fp = fopen(fileIn,"wb")) == NULL)
  {
    exit(0);
  }  
}

void getBlockInfo(){
  if((readn(sockid,(char *)&num_blks,sizeof(num_blks))) < 0)
  {
    exit(0);
  }
  num_blks = ntohs(num_blks);
  if((readn(sockid,(char *)&num_last_blk,sizeof(num_last_blk))) < 0)
  {
    exit(0);
  }
  num_last_blk = ntohs(num_last_blk);  
  return;
}

void beginread(){
  for(i= 0; i < num_blks; i ++) 
  {
    if((readn(sockid,in_buf,MAXSIZE)) < 0)
    {
      exit(0);
    }
    no_writen = fwrite(in_buf,sizeof(char),MAXSIZE,fp);
    if (no_writen == 0) 
    {
      exit(0);
    }
    if (no_writen != MAXSIZE) 
    {
      exit(0);
    }
  }
}

void readpartial(){
  if (num_last_blk > 0) {
    DEBUG printf("%d\n",num_blks);      
    if((readn(sockid,in_buf,num_last_blk)) < 0)
    {
      exit(0);
    }
    no_writen = fwrite(in_buf,sizeof(char),num_last_blk,fp); 
    if (no_writen == 0) 
    {
      exit(0);
    }
    if (no_writen != num_last_blk) 
    {
      exit(0);
    }
  }
  else DEBUG printf("\n");
}

void callClient(const char *fileInx){
  
  no_writen = 0;
  num_blks = 0;
  num_last_blk = 0;
  // gets(fileIn);
  strcpy(fileIn,fileInx);
  len = strlen(fileIn);
  printf("\nAttempting Download {%s}\n",fileIn);
  // DEBUG printf("\nLen = %d\n",len);
  while(setupConnection()==-1);
  reportFile();
  openfile();
  getBlockInfo();
  beginread();
  readpartial();
  fclose(fp);
  close(sockid);
  return;
}
int readn(int sd,char *ptr,int size)
{
  int no_left,no_read;
  no_left = size;
  while (no_left > 0) 
  { 
    no_read = read(sd,ptr,no_left);
    if(no_read <0)  return(no_read);
    if (no_read == 0) break;
    no_left -= no_read;
    ptr += no_read;
  }
  return(size - no_left);
}


int writen(int sd,char *ptr,int size)
{         
  int no_left,no_written;
  no_left = size;
  while (no_left > 0) 
  { 
    no_written = write(sd,ptr,no_left);
    if(no_written <=0)  return(no_written);
    no_left -= no_written;
    ptr += no_written;
  }
  return(size - no_left);
}
