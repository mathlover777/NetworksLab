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

#define ACK                   2
#define NACK                  3
#define REQUESTFILE           100
#define COMMANDNOTSUPPORTED   150
#define COMMANDSUPPORTED      160
#define BADFILENAME           200
#define FILENAMEOK            400
#define STARTTRANSFER         500

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
  // printf("client: creating socket\n");
  //time should write like this .
  int k=(rand()%1000+1);/*<<"this is the number :"<<"\n";*/

  if ((sockid = socket(AF_INET,SOCK_STREAM,0)) < 0)
  { 
    DEBUG printf("client: socket error : %d\n", errno); 
    exit(0);
  }
  
  DEBUG printf("client: binding my local socket\n");
  bzero((char *) &my_addr,sizeof(my_addr));
  my_addr.sin_family = AF_INET;
  my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  my_addr.sin_port = htons(CLIENT_PORT_ID+k);
  if (bind(sockid ,(struct sockaddr *) &my_addr,sizeof(my_addr)) < 0)
  {
    printf("client: bind  error :%d\n", errno); 
    return -1;
  }
                                           
  DEBUG printf("client: starting connect\n");
  bzero((char *) &server_addr,sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(SERVER_HOST_ADDR);
  server_addr.sin_port = htons(SERVER_PORT_ID);
  if (connect(sockid ,(struct sockaddr *) &server_addr,sizeof(server_addr)) < 0)
  {
    DEBUG printf("client: connect  error :%d\n", errno); exit(0);
  }
}

void reportFile(){
  getfile = htons(REQUESTFILE);
  DEBUG printf("client: sending command request to ftp server\n");
  if((writen(sockid,(char *)&getfile,sizeof(getfile))) < 0)
  {
    DEBUG printf("client: write  error :%d\n", errno); 
    exit(0);
  } 

  /* want for go-ahead from server */
  msg = 0;  
  if((readn(sockid,(char *)&msg,sizeof(msg)))< 0)
  {
    DEBUG printf("client: read  error :%d\n", errno); 
    exit(0);
  }
  msg = ntohs(msg);   
  if (msg==COMMANDNOTSUPPORTED) 
  {
    DEBUG printf("client: server refused command. goodbye\n");
    exit(0);
  }
  else
    DEBUG printf("client: server replied %d, command supported\n",msg);
    /* send file name to server */
    DEBUG printf("client: sending filename\n");
    if ((writen(sockid,fileIn,len+1))< 0)
    {
      DEBUG printf("client: write  error :%d\n", errno); 
      exit(0);
    }
    /* see if server replied that file name is OK */
    msg_2 = 0;
    if ((readn(sockid,(char *)&msg_2,sizeof(msg_2)))< 0)
    {
      DEBUG printf("client: read  error :%d\n", errno); 
      exit(0); 
    }   
    msg_2 = ntohs(msg_2);
    if (msg_2 == BADFILENAME) {
       DEBUG printf("client: server reported bad file name. goodbye.\n");
       exit(0);
    }
    else
      DEBUG printf("client: server replied %d, filename OK\n",msg_2);
}

void openfile(){
  DEBUG printf("client: sending start transfer command\n");
  start_xfer = STARTTRANSFER;
  start_xfer = htons(start_xfer);
  if ((writen(sockid,(char *)&start_xfer,sizeof(start_xfer)))< 0)
  {
    DEBUG printf("client: write  error :%d\n", errno); 
    exit(0);
  }
  if ((fp = fopen(fileIn,"wb")) == NULL)
  {
    DEBUG printf(" client: local open file error \n");
    exit(0);
  }  
}

void getBlockInfo(){
  if((readn(sockid,(char *)&num_blks,sizeof(num_blks))) < 0)
  {
    DEBUG printf("client: read error on nblocks :%d\n",errno);
    exit(0);
  }
  num_blks = ntohs(num_blks);
  DEBUG printf("client: server responded: %d blocks in file\n",num_blks);
  ack = ACK;  
  ack = htons(ack);
  if((writen(sockid,(char *)&ack,sizeof(ack))) < 0)
  {
    DEBUG printf("client: ack write error :%d\n",errno);
    exit(0);
  } 
  if((readn(sockid,(char *)&num_last_blk,sizeof(num_last_blk))) < 0)
  {
    DEBUG printf("client: read error :%d on nbytes\n",errno);
    exit(0);
  }
  num_last_blk = ntohs(num_last_blk);  
  DEBUG printf("client: server responded: %d bytes last blk\n",num_last_blk);
  if((writen(sockid,(char *)&ack,sizeof(ack))) < 0)
  {
    DEBUG printf("client: ack write error :%d\n",errno);
    exit(0);
  }
}

void beginread(){
  DEBUG printf("client: starting to get file contents\n");
  for(i= 0; i < num_blks; i ++) 
  {
    if((readn(sockid,in_buf,MAXSIZE)) < 0)
    {
      DEBUG printf("client: block error read: %d\n",errno);
      exit(0);
    }
    no_writen = fwrite(in_buf,sizeof(char),MAXSIZE,fp);
    if (no_writen == 0) 
    {
      DEBUG printf("client: file write error\n");
      exit(0);
    }
    if (no_writen != MAXSIZE) 
    {
      DEBUG printf("client: file write  error : no_writen is less\n");
      exit(0);
    }
    /* send an ACK for this block */
    if((writen(sockid,(char *)&ack,sizeof(ack))) < 0)
    {
      DEBUG printf("client: ack write  error :%d\n",errno);
      exit(0);
    }
    // DEBUG printf(" %d...",i);
  }
}

void readpartial(){
  if (num_last_blk > 0) {
    DEBUG printf("%d\n",num_blks);      
    if((readn(sockid,in_buf,num_last_blk)) < 0)
    {
      DEBUG printf("client: last block error read :%d\n",errno);
      exit(0);
    }
    no_writen = fwrite(in_buf,sizeof(char),num_last_blk,fp); 
    if (no_writen == 0) 
    {
      DEBUG printf("client: last block file write err :%d\n",errno);
      exit(0);
    }
    if (no_writen != num_last_blk) 
    {
      DEBUG printf("client: file write error : no_writen is less 2\n");
      exit(0);
    }
    if((writen(sockid,(char *)&ack,sizeof(ack))) < 0)
    {
      DEBUG printf("client :ack write  error  :%d\n",errno);
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
  // shutdown (sockid,2);
  // system("sudo cutter 127.0.0.1");
  /*FILE TRANSFER ENDS. CLIENT TERMINATES AFTER  CLOSING ALL ITS FILES
  AND SOCKETS*/ 
  fclose(fp);
  DEBUG printf("client: FILE TRANSFER COMPLETE\n");
  close(sockid);
  return;
}

// int main(int argc,char *argv[])
// {
//     callClient();    
// }	     

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
