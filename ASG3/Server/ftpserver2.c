#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define MY_PORT_ID 7000
#define MAXLINE 256
#define MAXSIZE 512

#define MAXCONECTION 5


int pid, cliLen;
struct sockaddr_in my_addr, client_addr;
/************************* DONE **************************************/
int readBuffer(int sd,char *ptr,int size){
  int no_left,no_read;
  no_left = size;
  while (no_left > 0) { 
    no_read = read(sd,ptr,no_left);
    if(no_read <0)  return(no_read);
    if (no_read == 0) break;
    no_left -= no_read;
    ptr += no_read;
  }
  return(size - no_left);
}
int writeBuffer(int sd,char *ptr,int size){
  int no_left,no_written;
  no_left = size;
  while (no_left > 0){
    no_written = write(sd,ptr,no_left);
    if(no_written <=0)  return(no_written);
    no_left -= no_written;
    ptr += no_written;
  }
  return(size - no_left);
}
int createSocket(){
  printf("server: creating socket\n");
  int sockIDx = socket(AF_INET,SOCK_STREAM,0);
  if (sockIDx < 0){
    printf("server: socket error : %d\n", errno); exit(0); 
  }
  return sockIDx;
}
void bindSocket(int sockIDx,int port){
  printf("server: binding my local socket\n");
  bzero((char *) &my_addr,sizeof(my_addr));
  my_addr.sin_family = AF_INET;  // setting the address family
  my_addr.sin_port = htons(port);// setting port
  my_addr.sin_addr.s_addr = htons(INADDR_ANY);
  int bFlag = bind(sockIDx ,(struct sockaddr *) &my_addr,sizeof(my_addr));
  if ( bFlag< 0)
  {
    printf("server: bind  error :%d\n", errno); 
    exit(0); 
  }
  return;
}
int initializeFTPServer(){
  int sockID = createSocket();
  bindSocket(sockID,MY_PORT_ID);
  printf("server: starting listen \n");
  int listenFlag = listen(sockID,MAXCONECTION);
  if ( listenFlag< 0)
  {
    printf("server: listen error :%d\n",errno);
    exit(0);
  }
  return sockID;
}
/*********************************************************************/
void getRequestedFileName(int newsd,char *filename){
  int readFlag = read(newsd,filename,MAXLINE);
  if(readFlag < 0)
  {
    printf("server: filename read error :%d\n",errno);
    exit(0);
  }
  return;
}
int isFileExist(const char *filename){
  FILE *fp = fopen(filename,"rb");
  if(fp == NULL){
    return -1;
  }
  return 1;
}
int getFileSize(const char *filename){
  FILE *fp = fopen(filename,"rb");
  int fsize = 0;
  int c;
  while ((c = getc(fp)) != EOF){
    fsize++;
  }
  return fsize;
}
void sendBlockInfo(int newsd,int num_blks,int num_last_blk){
  int num_blks1 = htons(num_blks);
  int num_last_blk1 = htons(num_last_blk);
  if((writeBuffer(newsd,(char *)&num_blks1,sizeof(num_blks1))) < 0){
    printf("server: write error\n");
    exit(0);
  }
  if((writeBuffer(newsd,(char *)&num_last_blk1,sizeof(num_last_blk1))) < 0){
    printf("server: write error :%d\n",errno);
    exit(0);
  }
  return;
}
void writeBlockPart(FILE *fp,int newsd,int num_blks){
  char out_buf[MAXSIZE];
  int ack = 0,i,no_read;
  for(i= 0; i < num_blks; i ++) { 
    no_read = fread(out_buf,sizeof(char),MAXSIZE,fp);
    if (no_read == 0){
      exit(0);
    }
    if (no_read != MAXSIZE){
      exit(0);
    }
    if((writeBuffer(newsd,out_buf,MAXSIZE)) < 0)
    {
      printf("server: error sending block:%d\n",errno);
      exit(0);
    }
  }
  return;
}
void writeRemainderPart(FILE *fp,int newsd,int num_blks,int num_last_blk){
  char out_buf[MAXSIZE];
  int ack = 0;
  int no_read;
  if (num_last_blk > 0) { 
    printf("%d\n",num_blks);
    no_read = fread(out_buf,sizeof(char),num_last_blk,fp); 
    if (no_read == 0) 
    {
      printf("server: file read error\n");
      exit(0);
    }
    if (no_read != num_last_blk) 
    {
      printf("server: file read error : no_read is less 2\n");
      exit(0);
    }
    if((writeBuffer(newsd,out_buf,num_last_blk)) < 0)
    {
      printf("server: file transfer error %d\n",errno);
      exit(0);
    }
  }
  return;
}
void transferFile(int newsd,const char *fname){
  int fsize = getFileSize(fname);
  int num_blks = fsize / MAXSIZE;
  int num_last_blk = fsize % MAXSIZE;

  // printf("\nNUM BLOCKS = %d NUM LAST BLOCK %d",num_blks,num_last_blk);
  sendBlockInfo(newsd,num_blks,num_last_blk);
  FILE *fp = fopen(fname,"rb");
  // printf("\nGOING TO BLOCK TRANSFER");
  writeBlockPart(fp,newsd,num_blks);
  // printf("\nGOING TO REMAINDER TRANSFER");
  writeRemainderPart(fp,newsd,num_blks,num_last_blk);
  // printf("server: FILE TRANSFER COMPLETE on socket %d\n",newsd);
  fclose(fp);
}
void doftp(int newsd){
  char fname[MAXLINE];
  fname[0] = '\0';
  getRequestedFileName(newsd,fname);
  printf("\nRequested FILE : {%s}",fname);
  if(isFileExist(fname)!=1){
    printf("\nFILE not found !!");
    return;
  } 
  transferFile(newsd,fname);
  close(newsd);
  return;
}
void poll(int sockID){
  while(1){ 
    printf("server: starting accept\n");
    int newsd = accept(sockID ,(struct sockaddr *) &client_addr,&cliLen);
    if (newsd < 0){
      printf("server: accept  error :%d\n", errno); 
      exit(0); 
    }
    pid=fork();
    if (pid == 0) {
       close(sockID);
       doftp(newsd);
       close (newsd);
       exit(0);
    }
    // wait();
    close(newsd);
  }
  return;
}
int main(){   
  int sockID = initializeFTPServer();
  poll(sockID);
  return 0; 
}