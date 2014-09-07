/*  This is an implementation of a stop-and-wait data link protocol.
    It is based on Tanenbaum's `protocol 4', 2nd edition, p227.
    This protocol employs only data and acknowledgement frames -
    piggybacking and negative acknowledgements are not used.

    It is currently written so that only one node (number 0) will
    generate and transmit messages and the other (number 1) will receive
    them. This restriction seems to best demonstrate the protocol to
    those unfamiliar with it.
    The restriction can easily be removed by deleting the line

	    if(nodeinfo.nodenumber == 0)

    in reboot_node(). Both nodes will then transmit and receive (why?).

    Note that this file only provides a reliable data-link layer for a
    network of 2 nodes.
 */


#include <cnet.h>
#include <stdio.h>
#include <string.h>

extern int fprintf	   _PARAMS((FILE *, const char *, ...));


typedef struct {
    char        data[MAX_MESSAGE_SIZE];
} MSG;

typedef enum    { DATA, ACK }   FRAMEKIND;

typedef struct {
    FRAMEKIND   kind;      	/* only ever DATA or ACK */
    int         len;       	/* the length of the msg portion only */
    int         checksum;  	/* checksum of the whole frame */
    int         seq;       	/* only ever 0 or 1 */
    MSG         msg;
} FRAME;

#define FRAME_HEADER_SIZE  (sizeof(FRAMEKIND) + 3*sizeof(int))
#define FRAME_SIZE(f)      (FRAME_HEADER_SIZE + f.len)

#define CHECK(call)        if((call) != 0) { \
				(void)fprintf(stderr,"%s:%s, line%d : %s\n", \
				nodeinfo.nodename, __FILE__, __LINE__, \
				cnet_errstr[cnet_errno]); exit(1); }

static  MSG       	*lastmsg;
static  int       	lastlength;
static  CnetTimestamp	timer;

static  int       ackexpected		= 0,
		  nextframetosend	= 0,
                  frameexpected		= 0;

static  void      transmit_frame	_PARAMS((MSG *, FRAMEKIND, int, int));


void reboot_node(ev, ts, data) CnetEvent ev; CnetTimestamp ts; CnetData data;
{
    extern char *malloc		  _PARAMS((unsigned));

    static void application_ready _PARAMS((CnetEvent, CnetTimestamp, CnetData));
    static void physical_ready    _PARAMS((CnetEvent, CnetTimestamp, CnetData));
    static void timeouts          _PARAMS((CnetEvent, CnetTimestamp, CnetData));
    static void variables	  _PARAMS((CnetEvent, CnetTimestamp, CnetData));

    if(nodeinfo.nodenumber > 1) {
	(void)fprintf(stderr,"This is not a 2-node network!\n"); exit(1);
    }

    lastmsg     = (MSG *)malloc(sizeof(MSG));

    CHECK(CNET_set_handler( EV_APPLICATIONREADY, application_ready, 0));
    CHECK(CNET_set_handler( EV_PHYSICALREADY,    physical_ready, 0));
    CHECK(CNET_set_handler( EV_TIMER1,           timeouts, 0));
    CHECK(CNET_set_handler( EV_DEBUG1,           variables, 0));

    CHECK(CNET_set_debug_string( EV_DEBUG1, "Variables"));

    if(nodeinfo.nodenumber == 0)
	(void)CNET_enable_application(ALLNODES);
}


static void application_ready(ev, ts, data)
				CnetEvent ev; CnetTimestamp ts; CnetData data;
{
    CnetAddr destaddr;

    lastlength  = sizeof(MSG);
    CHECK(CNET_read_application(&destaddr,(char *)lastmsg,&lastlength));
    (void)CNET_disable_application(ALLNODES);

    (void)printf(" down from application, seq=%d\n",nextframetosend);
    transmit_frame(lastmsg, DATA, lastlength, nextframetosend);
    nextframetosend = 1-nextframetosend;
}


static void physical_ready(ev, ts, data)
				CnetEvent ev; CnetTimestamp ts; CnetData data;
{
    FRAME       f;
    int         link, len, checksum;

    len         = sizeof(FRAME);
    CHECK(CNET_read_physical(&link,(char *)&f,&len));

    checksum    = f.checksum;
    f.checksum  = 0;
    if(checksum_ccitt((unsigned char *)&f, len) != checksum) {
        (void)printf("\t\t\t\tBAD checksum - frame ignored\n");
        return;           /* bad checksum, ignore frame */
    }

    if(f.kind == ACK) {
        if(f.seq == ackexpected) {
            (void)printf("\t\t\t\tACK received, seq=%d\n",f.seq);
            (void)CNET_stop_timer(timer);
            ackexpected = 1-ackexpected;
            (void)CNET_enable_application(ALLNODES);
        }
    }
    else if(f.kind == DATA) {
        (void)printf("\t\t\t\tDATA received, seq=%d, ",f.seq);
        if(f.seq == frameexpected) {
            (void)printf("up to application\n");
            len = f.len;
            CHECK(CNET_write_application(f.msg.data,&len));
            frameexpected = 1-frameexpected;
        }
        else
            (void)printf("ignored\n");
        transmit_frame((MSG *)NULL, ACK, 0, f.seq);
    }
}


static void transmit_frame(msg, kind, msglen, seqno)
			    MSG *msg; FRAMEKIND kind; int msglen; int seqno;
{
    FRAME       f;

    f.kind      = kind;
    f.seq       = seqno;
    f.checksum  = 0;
    f.len       = msglen;

    if(kind == ACK)
        (void)printf(" ACK transmitted, seq=%d\n",seqno);
    else if(kind == DATA) {
        (void)memcpy((char *)&f.msg,(char *)msg,msglen);
        (void)printf(" DATA transmitted, seq=%d\n",seqno);
        timer   = CNET_start_timer(EV_TIMER1,3*linkinfo[1].propagationdelay,0);
    }

    msglen      = FRAME_SIZE(f);
    f.checksum  = checksum_ccitt((unsigned char *)&f, msglen);
    CHECK(CNET_write_physical(1, (char *)&f, &msglen));
}


static void timeouts(ev, ts, data)
				CnetEvent ev; CnetTimestamp ts; CnetData data;
{
    if(ts == timer) {
        (void)printf(" timeout, seq=%d\n",ackexpected);
        transmit_frame(lastmsg,DATA,lastlength,ackexpected);
    }
}



static void variables(ev, ts, data)
				CnetEvent ev; CnetTimestamp ts; CnetData data;
{
  (void)printf(
  "\n\tackexpected\t= %d\n\tnextframetosend\t= %d\n\tframeexpected\t= %d\n\n",
		    ackexpected, nextframetosend, frameexpected);
}