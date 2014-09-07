// Following code implements Go-Back-N ARQ protocol with 1 bit Receiver window

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
#define ACK_FRAME_SIZE      6

// Transmission Time of Data and Ack Frame
#define DATA_TRANS_TIME     50
#define ACK_TRANS_TIME      5

#define TIMEOUT     3600
#define CHECK_BYTES 8
// #define MAXSEQ    2
#define RECEIVED    1
#define ACK_WAIT    2
#define RECVREADY   1
#define REJ         2

FILE *f;

typedef enum {
   TRANSMITTER,
   RECEIVER
} TYPE_OF_NODE;

typedef struct {
   char        data[DATA_SIZE_MAX];
} MSG;

typedef struct {
   int  sequence;            // sequence Number of the Frame
   int  length;
   int  checksum;       // Checksum of the frame (first 64 bits)
   MSG  message;            // Message sent by the transmitter
} FRAME_DATA;

typedef struct {
   int         sequence;            // sequence Number of the Frame
                               // (if +ve RECVREADY frame, if non +ve REJ frame)
   uint16_t    check;
} FRAME_ACK;

#define DATA_FRAME_SIZE(f)  (DATA_FRAME_HEADER_SIZE + f.length)


TYPE_OF_NODE type_of_node;

static  CnetTimerID wt_tmr = NULLTIMER;
static  MSG *last_message;
static  int ack_last = -1; 
static  int expected_frame =0;
static  int frame_next = 0;     
static  int sent_packets = 0;

static  FRAME_DATA t_window[MAXSEQ + 1];    // Storing Frames in window
static  int curr_status[MAXSEQ + 1];      // Storing Status of Frames in Window

long  time_start = 0, time_end = 0;

static  void frame_send_data(MSG *message, int length, int sequence)
{
   FRAME_DATA f;
   if(length > DATA_SIZE_MAX)
       length = DATA_SIZE_MAX;
   f.sequence = sequence;
   f.checksum = 0;        
   f.length = length;
   memcpy(&f.message, message, length);
   f.checksum  = CNET_ccitt((unsigned char *) &f, CHECK_BYTES);
   printf("DATA transmitted, sequence = %d\n", sequence);
   t_window[sequence] = f;
   size_t size = DATA_FRAME_SIZE(f);
   CHECK(CNET_write_physical(1, (char *) &f, &size));
}

static  void frame_send_ack(int sequence, int type)
{
   FRAME_ACK f;
   if(type != REJ)
       f.sequence = sequence;
   else
       f.sequence = -sequence;
   f.check = 0;
   size_t size = sizeof(FRAME_ACK);
   f.check = CNET_ccitt((unsigned char *) &f, size);
   printf("ACK transmitted\n");
   CHECK(CNET_write_physical(1, (char *) &f, &size));
}

static EVENT_HANDLER(ready_app)
{
   if(curr_status[frame_next] == ACK_WAIT)
   {
       wt_tmr = CNET_start_timer(EV_TIMER1, TIMEOUT, 0);
       CNET_disable_application(ALLNODES);
       return;
   }
   curr_status[frame_next] = ACK_WAIT;
   size_t length = MAX_MESSAGE_SIZE;
   printf("Down from application, sequence=%d\n",frame_next);
   frame_send_data(last_message, length, frame_next);
   frame_next = (frame_next + 1) % (MAXSEQ + 1);
}

static EVENT_HANDLER(ready_physical)
{
   if(type_of_node != RECEIVER)
   {
       FRAME_ACK f;
       int link;
       size_t length;
       length = sizeof(FRAME_ACK);
       CHECK(CNET_read_physical(&link, (char*) &f, &length));
       uint16_t check = f.check;
       f.check = 0;
       if(CNET_ccitt((unsigned char*) &f, sizeof(FRAME_ACK)) != check)
           return;
       if(f.sequence > 0)
       {
           int i;
           int c = 0;
           printf("\t\t\t\tACK received, sequence=%d\n", f.sequence);
           for(i = (ack_last + 1) % (MAXSEQ + 1); i != (f.sequence - 1) % (MAXSEQ + 1); i = (i + 1) % (MAXSEQ + 1))
           {
               curr_status[i] = RECEIVED;
               c++;
           }
           ack_last = (f.sequence - 1) % (MAXSEQ + 1);
           curr_status[ack_last] = RECEIVED;
           CNET_stop_timer(wt_tmr);
           if(sent_packets < 10000)
           {
               sent_packets += (c+1);
               CNET_enable_application(ALLNODES);
           }
           else
           {
               if(time_end == 0)
                   time_end = nodeinfo.time_of_day.sec;
           }
       }
       else
       {
           // REJ FRAME
           f.sequence = -f.sequence;
           printf("\t\t\t\tREJ received, sequence=%d\n", f.sequence);
           ack_last = (f.sequence - 1) % (MAXSEQ + 1);
           CNET_stop_timer(timer);
           int i;
           for(i = f.sequence; i != frame_next; i = (i + 1) % (MAXSEQ + 1))
           {
               curr_status[i] = ACK_WAIT;
               frame_send_data(&(t_window[i].message), t_window[i].length, i);
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

        if(f.sequence != expected_frame){
           printf("Received out of order. Ignore\n");
           frame_send_ack(expected_frame, REJ);
           return;
       }

       if(CNET_ccitt((unsigned char*) &f, CHECK_BYTES) != checksum)
       {
           // bad checksum, ignore frame
           printf("\t\t\t\tBAD checksum - frame ignored. Sending Rej\n");
           if(f.sequence >= 0 && f.sequence <= MAXSEQ)
               frame_send_ack(expected_frame, REJ);
           return;          
       }
       
       printf("\t\t\t\tDATA received, sequence=%d\n", f.sequence);

       printf("Up to application\n");
       length=f.length;
       // CHECK(CNET_write_application(&f.message, &length));
       expected_frame=(expected_frame + 1)%(MAXSEQ + 1);
       frame_send_ack(f.sequence + 1, RECVREADY);
   }    
}

static EVENT_HANDLER(wait)
{

   printf("Timeout. Resending\n"); 
   int i;
   for(i = (ack_last + 1) % (MAXSEQ + 1); i != frame_next; i = (i + 1) % (MAXSEQ + 1)){
       frame_send_data(&(t_window[i].message), t_window[i].length, i);
   }
   frame_send_data(&(t_window[frame_next].message), t_window[frame_next].length, i);
}

static EVENT_HANDLER(result_record)
{
   if(type_of_node == TRANSMITTER)
   {
       f = fopen(RESULT_GOBACKN, "a");
       if(time_end == 0)
           time_end = nodeinfo.time_of_day.sec;
       fprintf(f, "%lf %d %f\n", PROBCORRUPT, MAXSEQ, sent_packets*1.0*DATA_FRAME_SIZE_MAX*8/(time_end - time_start));
       fprintf(stderr, "%d %ld \n", sent_packets, (time_end - time_start));
       fclose(f);
   }
}

EVENT_HANDLER(reboot_node)
{
   if(nodeinfo.nodenumber <= 1)
   {
     int i;
     for(i=0; i<=MAXSEQ; i++)
         curr_status[i] = 0;
     last_message = calloc(1, sizeof(MSG));
     wt_tmr = NULLTIMER;
     CHECK(CNET_set_handler(EV_APPLICATIONREADY, ready_app, 0));
     CHECK(CNET_set_handler(EV_PHYSICALREADY, ready_physical, 0));
     CHECK(CNET_set_handler(EV_TIMER1, wait, 0));
     CHECK(CNET_set_handler(EV_SHUTDOWN, result_record, 0));
     time_start = nodeinfo.time_of_day.sec;
     if(nodeinfo.nodenumber != 0)
     {
         type_of_node = RECEIVER;
     }
     else{
         type_of_node = TRANSMITTER;
         CNET_enable_application(ALLNODES);
     }
    }
    else{
       fprintf(stderr,"This is not a 2-node network!\n");
       exit(1);
    }
}