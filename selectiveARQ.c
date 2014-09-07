#include <cnet.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include "maxseq.h"
#include "config.h"

// Regular Frame Size = 512 bits = 64 bytes
#define DATA_FRAME_SIZE_MAX 64

// Data Frame Header consists of checksum and sequence number
#define DATA_FRAME_HEADER_SIZE   (sizeof(uint16_t) + sizeof(int) + sizeof(size_t))

#define DATA_SIZE_MAX       (DATA_FRAME_SIZE_MAX - DATA_FRAME_HEADER_SIZE)

// Ack Frame Size = 48 bits = 6 bytes
#define ACK_FRAME_SIZE_MAX  6

// Transmission Time of Data and Ack Frame
#define DATA_TRANS_TIME    50
#define ACK_TRANS_TIME     5

#define TIMEOUT      3600
#define CHECK_BYTES  8
// #define MAXSEQ     7
#define ACK_RESET    0
#define ACK_RECEIVED 1
#define ACK_WAIT     2
#define RECVREADY    1
#define SREJ         2
#define RECEIVED     1
#define RESET        0


typedef enum {
   TRANSMITTER,
   RECEIVER
} NODE_TYPE;

typedef struct {
   char data[DATA_SIZE_MAX];
} MSG;

typedef struct {
   int seq;            // Seq Number of the Frame
   int len;
   int checksum;       // Checksum of the frame (first 64 bits)
   MSG msg;            // Message sent by the transmitter
} DATA_FRAME;

typedef struct {
   int seq;          // Seq Number of the Frame
                     // (if +ve RECVREADY frame, if non +ve SREJ frame)
   uint16_t check;
} ACK_FRAME;

#define DATA_FRAME_SIZE(f)  (DATA_FRAME_HEADER_SIZE + f.len)


NODE_TYPE nodetype;

static  CnetTimerID wait_timer = NULLTIMER;
static  MSG *last_msg;
static  int last_ack_recv = -1;  
static  int last_ack_sent = -1;  

static  int next_frame = 0;    
static  int packets_sent = 0;

static  DATA_FRAME t_window[MAXSEQ + 1];    
static  int trans_frame_state[MAXSEQ + 1];  

static  int recv_frame_status[MAXSEQ + 1];    


time_t  start_time, end_time;

static  void send_data_frame(MSG *msg, int len, int seq)
{
   if(len > DATA_SIZE_MAX)
       len = DATA_SIZE_MAX;
   
   DATA_FRAME f;
   f.seq = seq;        
   f.len = len;
   f.checksum = 0;
   memcpy(&f.msg, msg, len);
   
   f.checksum  = CNET_ccitt((unsigned char *) &f, CHECK_BYTES);
   
   printf("DATA transmitted, frame seq = %d\n", seq);
   
   t_window[seq] = f;
   
   size_t size = DATA_FRAME_SIZE(f);
   CHECK(CNET_write_physical(1, (char *) &f, &size));
}

static  void send_ack_frame(int seq, int type)
{
   ACK_FRAME f;
   f.check = 0;
   if(type == SREJ)
       f.seq = -seq;
   else
       f.seq = seq;
   
   size_t size = sizeof(ACK_FRAME);
   f.check = CNET_ccitt((unsigned char *) &f, size);

   printf("ACK transmitted\n");

   CHECK(CNET_write_physical(1, (char *) &f, &size));
}

static EVENT_HANDLER(app_ready)
{  
   int num_unack = next_frame - last_ack_recv;
   if(num_unack<0){
       num_unack=MAXSEQ;
   }
   if(num_unack==(MAXSEQ+1)/2){
       wait_timer = CNET_start_timer(EV_TIMER1, TIMEOUT, 0);
       CNET_disable_application(ALLNODES);
       return;
   }

   // if(trans_frame_state[next_frame%((MAXSEQ+1)/2)] == ACK_WAIT)
   // {
   //     wait_timer = CNET_start_timer(EV_TIMER1, TIMEOUT, 0);
   //     CNET_disable_application(ALLNODES);
   //     return;
   // }
   trans_frame_state[next_frame] = ACK_WAIT;
   
   CnetAddr dest;
   
   size_t len = MAX_MESSAGE_SIZE;
   CHECK(CNET_read_application(&dest, (char*) last_msg, &len));
   
   send_data_frame(last_msg, len, next_frame);
   next_frame = (next_frame + 1) % (MAXSEQ + 1);
}

static EVENT_HANDLER(physical_ready)
{
   if(nodetype == RECEIVER)
   {
       DATA_FRAME f;
       size_t len;
       int link;
       int checksum;

       len = sizeof(DATA_FRAME);
       CHECK(CNET_read_physical(&link, (char*) &f, &len));

       checksum = f.checksum;
       f.checksum  = 0;

       if(CNET_ccitt((unsigned char*) &f, CHECK_BYTES) != checksum)
       {
           // bad checksum, ignore frame
           printf("\t\t\t\tBAD checksum - frame ignored\n");
           send_ack_frame(last_ack_sent + 1, SREJ);
           printf("Requesting to send frame %d\n",last_ack_sent+1);
           return;          
       }

       recv_frame_status[f.seq] = RECEIVED;
       printf("\t\t\t\tDATA received, seq=%d\n", f.seq);
       
       int i;
       for(i = (last_ack_sent + 1) % (MAXSEQ + 1); i != f.seq; i = (i + 1) % (MAXSEQ + 1))
       {
           if(recv_frame_status[i] != RECEIVED)
               break;
       }
       if(i == f.seq)
           send_ack_frame(f.seq + 1, RECVREADY);
   }
   else
   {
       ACK_FRAME f;
       int link;
       size_t len;

       len = sizeof(ACK_FRAME);
       CHECK(CNET_read_physical(&link, (char*) &f, &len));

       uint16_t check = f.check;
       f.check = 0;
       if(CNET_ccitt((unsigned char*) &f, sizeof(ACK_FRAME)) != check)
           return;
       
       if(f.seq > 0)
       {
           // RECVREADY FRAME
           printf("\t\t\t\tACK received, seq=%d\n", f.seq);
           
           int i;
           for(i = (last_ack_recv + 1)%(MAXSEQ + 1); i != f.seq - 1; i = (i + 1) % (MAXSEQ + 1))
               trans_frame_state[i] = ACK_RECEIVED;

           last_ack_recv = f.seq - 1;
           
           trans_frame_state[f.seq-1] = ACK_RECEIVED;
           CNET_stop_timer(wait_timer);

           if(packets_sent < 10000)
           {
               packets_sent++;
               CNET_enable_application(ALLNODES);
           }
           else
           {
               if(end_time == 0)
                   end_time = nodeinfo.time_of_day.sec;
           }
       }
       else
       {
           // SREJ FRAME
           f.seq = -f.seq;
           printf("\t\t\t\tSREJ received, seq=%d\n", f.seq);
           CNET_stop_timer(timer);
           
           trans_frame_state[f.seq] = ACK_WAIT;
           send_data_frame(&(t_window[f.seq].msg), t_window[f.seq].len, f.seq);
       }
   }    
}

static EVENT_HANDLER(wait)
{
   printf("Timeout. Resending\n");
   
   // Acknowledgement not received. Resend from last unacknowledged frame
   
   int i;
   for(i = (last_ack_recv + 1) % (MAXSEQ + 1); i != next_frame; i = (i + 1) % (MAXSEQ + 1))
       send_data_frame(&(t_window[i].msg), t_window[i].len, i);
   send_data_frame(&(t_window[next_frame].msg), t_window[next_frame].len, i);
}

static EVENT_HANDLER(record_result)
{
   if(nodetype == TRANSMITTER)
   {
       if(end_time == 0)
           end_time = nodeinfo.time_of_day.sec;
       FILE *f = fopen(RESULT_SELECTIVEARQ, "a");
       fprintf(f, "%lf %d %f\n", PROBCORRUPT, MAXSEQ, packets_sent*1.0*DATA_FRAME_SIZE_MAX*8/(end_time - start_time));
       fprintf(stderr, "%d %ld \n", packets_sent, (long)(end_time - start_time));
       fclose(f);
   }
}

EVENT_HANDLER(reboot_node)
{
   if(nodeinfo.nodenumber > 1)
   {
       fprintf(stderr,"This is not a 2-node network!\n");
       exit(1);
   }

   last_msg = calloc(1, MAX_MESSAGE_SIZE);
   wait_timer = NULLTIMER;
   int i;
   for(i=0; i<=MAXSEQ; i++)
       trans_frame_state[i] = ACK_RESET;

   for(i=0; i<=MAXSEQ; i++)
       recv_frame_status[i] = RESET;

   CHECK(CNET_set_handler(EV_APPLICATIONREADY, app_ready, 0));
   CHECK(CNET_set_handler(EV_PHYSICALREADY, physical_ready, 0));
   CHECK(CNET_set_handler(EV_TIMER1, wait, 0));
   CHECK(CNET_set_handler(EV_SHUTDOWN, record_result, 0));

   start_time = nodeinfo.time_of_day.sec;
   // Only transmitter should send messages
   if(nodeinfo.nodenumber == 0)
   {
       nodetype = TRANSMITTER;
       CNET_enable_application(ALLNODES);
   }
   else
       nodetype = RECEIVER;
}
