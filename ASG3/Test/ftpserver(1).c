
/* This is the server for a very simple file transfer
   service.  This is a "concurrent server" that can
   handle requests from multiple simultaneous clients.
   For each client:
    - get file name and check if it exists
    - send size of file to client
    - send file to client, a block at a time
    - close connection with client
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define MY_PORT_ID 7000
#define MAXLINE 256
#define MAXSIZE 512   

#define ACK                   2
#define NACK                  3
#define REQUESTFILE           100
#define COMMANDNOTSUPPORTED   150
#define COMMANDSUPPORTED      160
#define BADFILENAME           200
#define FILENAMEOK            400

int writen(int sd,char *ptr,int size);
int readn(int sd,char *ptr,int size);
int sockid, newsd, pid, clilen;

int main()  {

   
   struct sockaddr_in my_addr, client_addr;   
   sockid = socket(AF_INET,SOCK_STREAM,0);
   bzero((char *) &my_addr,sizeof(my_addr));
   my_addr.sin_family = AF_INET;
   my_addr.sin_port = htons(MY_PORT_ID);
   my_addr.sin_addr.s_addr = htons(INADDR_ANY);
   bind(sockid ,(struct sockaddr *) &my_addr,sizeof(my_addr));
   listen(sockid,5);
   while(1) { 
    newsd = accept(sockid ,(struct sockaddr *) &client_addr,&clilen);
    if ( (pid=fork()) == 0) {
         /* CHILD PROC STARTS HERE. IT WILL DO ACTUAL FILE TRANSFER */
         close(sockid);   /* child shouldn't do an accept */
         doftp(newsd);
         close (newsd);
         exit(0);         /* child all done with work */
         }
         else{
          wait(0);
         }
      /* PARENT CONTINUES BELOW HERE */
     close(newsd);        /* parent all done with client, only child */
     }              /* will communicate with that client from now on */
}   
     
int i,fsize,fd,msg_ok,fail,fail1,req,c,ack;
int no_read ,num_blks , num_blks1,num_last_blk,num_last_blk1,tmp;
char fname[MAXLINE];
char out_buf[MAXSIZE];
FILE *fp;
/* CHILD PROCEDURE, WHICH ACTUALLY DOES THE FILE TRANSFER */
void setuptransfer(){
  no_read = 0;
  num_blks = 0;
  num_last_blk = 0; 
  req = 0;
  readn(newsd,(char *)&req,sizeof(req));
  req = ntohs(req);
  msg_ok = COMMANDSUPPORTED; 
  msg_ok = htons(msg_ok);
  writen(newsd,(char *)&msg_ok,sizeof(msg_ok)); 
  tmp = htons(fail);
  writen(newsd,(char *)&tmp,sizeof(tmp));  
  req = 0;
  readn(newsd,(char *)&req,sizeof(req));
  fsize = 0;ack = 0;   
  while ((c = getc(fp)) != EOF) {fsize++;}
  num_blks = fsize / MAXSIZE; 
  num_blks1 = htons(num_blks);
  num_last_blk = fsize % MAXSIZE; 
  num_last_blk1 = htons(num_last_blk);
  writen(newsd,(char *)&num_blks1,sizeof(num_blks1));  
  readn(newsd,(char *)&ack,sizeof(ack));
  writen(newsd,(char *)&num_last_blk1,sizeof(num_last_blk1));  
  readn(newsd,(char *)&ack,sizeof(ack));
  rewind(fp);
}

void transferfile(){
  for(i= 0; i < num_blks; i ++) { 
    no_read = fread(out_buf,sizeof(char),MAXSIZE,fp);
    writen(newsd,out_buf,MAXSIZE);
    readn(newsd,(char *)&ack,sizeof(ack));  
  }

  if (num_last_blk > 0) { 
    no_read = fread(out_buf,sizeof(char),num_last_blk,fp); 
    writen(newsd,out_buf,num_last_blk);
    readn(newsd,(char *)&ack,sizeof(ack));                    
  }
  /* FILE TRANSFER ENDS */
  printf("server: FILE TRANSFER COMPLETE on socket %d\n",newsd);
  fclose(fp);
  close(newsd);
}
doftp(int newsd)
{             
    setuptransfer();    
    /* ACTUAL FILE TRANSFER STARTS  BLOCK BY BLOCK*/        
    transferfile();
}


/*
  TO TAKE CARE OF THE POSSIBILITY OF BUFFER LIMMITS IN THE KERNEL FOR THE
 SOCKET BEING REACHED (WHICH MAY CAUSE READ OR WRITE TO RETURN FEWER CHARACTERS
  THAN REQUESTED), WE USE THE FOLLOWING TWO FUNCTIONS */  
   
int readn(int sd,char *ptr,int size)
{         int no_left,no_read;
          no_left = size;
          while (no_left > 0) 
                     { no_read = read(sd,ptr,no_left);
                       if(no_read <0)  return(no_read);
                       if (no_read == 0) break;
                       no_left -= no_read;
                       ptr += no_read;
                     }
          return(size - no_left);
}

int writen(int sd,char *ptr,int size)
{         int no_left,no_written;
          no_left = size;
          while (no_left > 0) 
                     { no_written = write(sd,ptr,no_left);
                       if(no_written <=0)  return(no_written);
                       no_left -= no_written;
                       ptr += no_written;
                     }
          return(size - no_left);
}


           
