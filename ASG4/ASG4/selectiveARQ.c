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
} TYPE_OF_NODE;

typedef struct {
   char data[DATA_SIZE_MAX];
} MESSAGE;

typedef struct {
   int sequence;            // Seq Number of the Frame
   int length;
   int checksum;       // Checksum of the frame (first 64 bits)
   MESSAGE message;            // Message sent by the transmitter
} FRAME_DATA;

typedef struct {
   int sequence;          // Seq Number of the Frame
                     // (if +ve RECVREADY frame, if non +ve SREJ frame)
   uint16_t check;
} ACK_FRAME;

#define DATA_FRAME_SIZE(f)  (DATA_FRAME_HEADER_SIZE + f.length)


FILE *f;
TYPE_OF_NODE nodetype;

static  CnetTimerID timer_wait = NULLTIMER;
static  MESSAGE *last_msg;
static  int last_ack_recv = -1;  
static  int last_ack_sent = -1;  

static  int next_frame = 0;    
static  int packets_sent = 0;

static  FRAME_DATA t_window[MAXSEQ + 1];    
static  int trans_frame_state[MAXSEQ + 1];  

static  int recv_frame_status[MAXSEQ + 1];    


time_t  time_start, time_end;

static  void frame_send(MESSAGE *message, int length, int sequence)
{
   if(length > DATA_SIZE_MAX)
       length = DATA_SIZE_MAX;
   
   FRAME_DATA f;
   f.sequence = sequence;        
   f.length = length;
   f.checksum = 0;
   memcpy(&f.message, message, length);
   
   f.checksum  = CNET_ccitt((unsigned char *) &f, CHECK_BYTES);
   
   printf("DATA transmitted, frame sequence = %d\n", sequence);
   
   t_window[sequence] = f;
   
   size_t size = DATA_FRAME_SIZE(f);
   CHECK(CNET_write_physical(1, (char *) &f, &size));
}

static  void ack_send(int sequence, int type)
{
   ACK_FRAME f;
   f.check = 0;
   if(type == SREJ)
       f.sequence = -sequence;
   else
       f.sequence = sequence;
   
   size_t size = sizeof(ACK_FRAME);
   f.check = CNET_ccitt((unsigned char *) &f, size);

   printf("ACK transmitted\n");

   CHECK(CNET_write_physical(1, (char *) &f, &size));
}

static EVENT_HANDLER(ready_app)
{  
  CnetAddr dest;
   int num_unack = next_frame - last_ack_recv;
   if(num_unack==(MAXSEQ+1)/2){
       timer_wait = CNET_start_timer(EV_TIMER1, TIMEOUT, 0);
       CNET_disable_application(ALLNODES);
       return;
   }

   if(num_unack<0){
       num_unack=MAXSEQ;
   }

   // if(trans_frame_state[next_frame%((MAXSEQ+1)/2)] == ACK_WAIT)
   // {
   //     timer_wait = CNET_start_timer(EV_TIMER1, TIMEOUT, 0);
   //     CNET_disable_application(ALLNODES);
   //     return;
   // }
   trans_frame_state[next_frame] = ACK_WAIT;
   
   size_t length = MAX_MESSAGE_SIZE;
   CHECK(CNET_read_application(&dest, (char*) last_msg, &length));
   
   frame_send(last_msg, length, next_frame);
   next_frame = (next_frame + 1) % (MAXSEQ + 1);
}

static EVENT_HANDLER(ready_physical)
{
   if(nodetype != RECEIVER)
   {
       ACK_FRAME f;
       int link;
       size_t length;

       length = sizeof(ACK_FRAME);
       
       CHECK(CNET_read_physical(&link, (char*) &f, &length));

       uint16_t check = f.check;
       
       f.check = 0;
       
       if(CNET_ccitt((unsigned char*) &f, sizeof(ACK_FRAME)) != check)
           return;
       
       if(f.sequence <= 0)
       {
           // SREJ FRAME
           printf("\t\t\t\tSREJ received, sequence=%d\n", -f.sequence);
           f.sequence = -f.sequence;
           CNET_stop_timer(timer);
           trans_frame_state[f.sequence] = ACK_WAIT;
           frame_send(&(t_window[f.sequence].message), t_window[f.sequence].length, f.sequence);
       }
       else
       {
           // RECVREADY FRAME
           printf("\t\t\t\tACK received, sequence=%d\n", f.sequence);
           int i;
           for(i = (last_ack_recv + 1)%(MAXSEQ + 1); i != f.sequence - 1; i = (i + 1) % (MAXSEQ + 1))
               trans_frame_state[i] = ACK_RECEIVED;
           last_ack_recv = f.sequence - 1;
           trans_frame_state[f.sequence-1] = ACK_RECEIVED;
           CNET_stop_timer(timer_wait);
           if(packets_sent >= 10000)
           {
               if(time_end == 0)
                   time_end = nodeinfo.time_of_day.sec;

           }
           else
           {
               packets_sent++;
               CNET_enable_application(ALLNODES);   
           }
       } 
   }
   else
   {
       FRAME_DATA f;
       size_t length;
       int link;
       int checksum;

       length = sizeof(FRAME_DATA);
       CHECK(CNET_read_physical(&link, (char*) &f, &length));

       checksum = f.checksum;
       f.checksum  = 0;

       if(CNET_ccitt((unsigned char*) &f, CHECK_BYTES) != checksum)
       {
           // bad checksum, ignore frame
           printf("\t\t\t\tBAD checksum - frame ignored\n");
           ack_send(last_ack_sent + 1, SREJ);
           printf("Requesting to send frame %d\n",last_ack_sent+1);
           return;          
       }

       recv_frame_status[f.sequence] = RECEIVED;
       printf("\t\t\t\tDATA received, sequence=%d\n", f.sequence);
       
       int i;
       for(i = (last_ack_sent + 1) % (MAXSEQ + 1); i != f.sequence; i = (i + 1) % (MAXSEQ + 1))
       {
           if(recv_frame_status[i] != RECEIVED)
               break;
       }
       if(i == f.sequence)
           ack_send(f.sequence + 1, RECVREADY);
   }    
}

static EVENT_HANDLER(wait)
{
   int i;
   printf("Timeout. Resending\n");
   for(i = (last_ack_recv + 1) % (MAXSEQ + 1); i != next_frame; i = (i + 1) % (MAXSEQ + 1))
       frame_send(&(t_window[i].message), t_window[i].length, i);
   frame_send(&(t_window[next_frame].message), t_window[next_frame].length, i);
}

static EVENT_HANDLER(result_record)
{
   if(nodetype == TRANSMITTER)
   {
       f = fopen(RESULT_SELECTIVEARQ, "a");
       if(time_end == 0)
           time_end = nodeinfo.time_of_day.sec;
       fprintf(f, "%lf %d %f\n", PROBCORRUPT, MAXSEQ, packets_sent*1.0*DATA_FRAME_SIZE_MAX*8/(time_end - time_start));
       fprintf(stderr, "%d %ld \n", packets_sent, (long)(time_end - time_start));
       fclose(f);
   }
}

EVENT_HANDLER(reboot_node)
{
   // if(nodeinfo.nodenumber > 1)
   // {
   //     fprintf(stderr,"This is not a 2-node network!\n");
   //     exit(1);
   // }

   int i;
   for(i=0; i<=MAXSEQ; i++)
       trans_frame_state[i] = ACK_RESET;

   last_msg = calloc(1, MAX_MESSAGE_SIZE);
   timer_wait = NULLTIMER;

     for(i=0; i<=MAXSEQ; i++)
       recv_frame_status[i] = RESET;


   CHECK(CNET_set_handler(EV_APPLICATIONREADY, ready_app, 0));
   CHECK(CNET_set_handler(EV_PHYSICALREADY, ready_physical, 0));
   CHECK(CNET_set_handler(EV_TIMER1, wait, 0));
   CHECK(CNET_set_handler(EV_SHUTDOWN, result_record, 0));

   time_start = nodeinfo.time_of_day.sec;
   // Only transmitter should send messages
   if(nodeinfo.nodenumber == 0)
   {
       nodetype = TRANSMITTER;
       CNET_enable_application(ALLNODES);
   }
   else
       nodetype = RECEIVER;
}
