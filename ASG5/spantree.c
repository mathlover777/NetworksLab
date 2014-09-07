#include <cnet.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

static int stateNos[10][3] = {{0,0,0},{1,1,0},{2,2,0},{3,3,0},{4,4,0},{5,5,0},{6,6,0},{7,7,0},{8,8,0},{9,9,0}};
static int lastLink[10] = {0,1,2,3,4,5,6,7,8,9};

typedef struct {
    char        data[MAX_MESSAGE_SIZE];
} MESG;

typedef struct {
    unsigned int len;           /* the length of the msg field only */
    MESG          mesg;
} FRAME;
static  MESG            *lastmsg;
#define FRAME_HEADER_SIZE  (sizeof(FRAME) - sizeof(MESG))
#define FRAME_SIZE(f)      (FRAME_HEADER_SIZE + f.len)


static void transmit_frame(MESG *mesg, int length)
{
	FRAME       frame;
    frame.len       = length;
    int link = nodeinfo.nlinks,i;
    memcpy(&frame.mesg, mesg, length);
    size_t size = FRAME_SIZE(frame);
    for(i=1;i<=link;i++)
        CHECK(CNET_write_physical_reliable(i, (char *) &frame, &size));
    
}

static bool isSteady()
{
    //fprintf(stderr, "\nEntered isSteady\n");
    int i;
    for(i=1;i<10;i++){
        //fprintf(stderr, "%d ",stateNos[i][1]);
        if(stateNos[i][1]!=0)
            return false;
    }
    fprintf(stderr, "isSteady returns true\n");
    return true;

}
static void updateState(char* statemsg)
{
    char *toks;
    int a,b,c,i;
    toks = strtok(statemsg," ");
    a = atoi(toks);
    toks = strtok(NULL," "); b = atoi(toks);
    toks = strtok(NULL," "); c = atoi(toks);
    fprintf(stderr, "Filing started\n");
    FILE *statesFile = fopen("stateinfo.txt","r");
    for(i=0;i<10;i++){
        fscanf(statesFile,"%d %d %d",&stateNos[i][1],&stateNos[i][2],&lastLink[i]);
    }
    fclose(statesFile);
    if(b < stateNos[nodeinfo.nodenumber][1] || (b==stateNos[nodeinfo.nodenumber][1] && c+1<stateNos[nodeinfo.nodenumber][2])){
        fprintf(stderr,"Message from %d to %d saying root is %d\n",a,nodeinfo.nodenumber,b);
        stateNos[nodeinfo.nodenumber][2] = c+1;
        stateNos[nodeinfo.nodenumber][1] = b;
        lastLink[nodeinfo.nodenumber] = a;
        // if(b==0){
        //     fprintf(linksused,"%d %d\n",a,nodeinfo.nodenumber);
        //     fprintf(resfile,"%d %d %d\n",nodeinfo.nodenumber,stateNos[nodeinfo.nodenumber][1],stateNos[nodeinfo.nodenumber][2]);
        // }
    }
    FILE *statesFile2 = fopen("stateinfo.txt","w");
    for(i=0;i<10;i++){
        fprintf(statesFile2,"%d %d %d\n",stateNos[i][1],stateNos[i][2],lastLink[i]);
    }
    fclose(statesFile2);
    fprintf(stderr, "Filing done\n");
    //fprintf(stderr, "\n");
    //fprintf(stderr, "Current state of states\n", );
    if(isSteady()==true){
        FILE *statesFile3 = fopen("stateinfo.txt","w");
        for(i=0;i<10;i++){
            fprintf(statesFile3,"%d %d %d\n",i,0,i);
        }
        fclose(statesFile3);
        char nodeNames[10][15];
        FILE *nodenames = fopen("Nodenames.txt","r");
        for(i=0;i<10;i++){
            fscanf(nodenames,"%s",nodeNames[i]);
        }
        fclose(nodenames);
        remove("Nodenames.txt");
        
        FILE *resfile = fopen("results.txt","w");
        for(i=1;i<10;i++)
            fprintf(resfile,"%s is %d hops away from root\n",nodeNames[i],stateNos[i][2]);
        fprintf(resfile,"\nNon-redundant links are:\n");
        for(i=1;i<10;i++)
            fprintf(resfile,"Joining %s and %s\n",nodeNames[lastLink[i]],nodeNames[i]);
        fclose(resfile);
        exit(0);
    }
}


static EVENT_HANDLER(application_ready)
{
    CnetAddr destaddr;
    size_t lastlength = MAX_MESSAGE_SIZE;
    
    CHECK(CNET_read_application(&destaddr, (char *)lastmsg, &lastlength));
    CNET_disable_application(ALLNODES);
    //printf("down from application, seq=%d\n", nextframetosend);
    MESG statemsg;
    statemsg.data[0]='\0';
    char buffer[50];
    sprintf(buffer,"%d",stateNos[nodeinfo.nodenumber][0]);
    strcpy(statemsg.data,buffer);strcat(statemsg.data," ");
    sprintf(buffer,"%d",stateNos[nodeinfo.nodenumber][1]);
    strcat(statemsg.data,buffer);strcat(statemsg.data," ");
    sprintf(buffer,"%d",stateNos[nodeinfo.nodenumber][2]);
    strcat(statemsg.data,buffer);
    printf("Node %d sends (%s)\n",nodeinfo.nodenumber,statemsg.data);
    transmit_frame(&statemsg,20);
}

static EVENT_HANDLER(physical_ready)
{
    FRAME frame;
    size_t length;
    int link;

    length = sizeof(FRAME);
    CHECK(CNET_read_physical(&link, (char*) &frame, &length));

    printf("Message received - (%s)\n",frame.mesg.data);
    updateState(frame.mesg.data);
    CNET_enable_application(ALLNODES);
}

EVENT_HANDLER(reboot_node)
{
    lastmsg = calloc(1, MAX_MESSAGE_SIZE);
    FILE *nodenames = fopen("Nodenames.txt","a");
    fprintf(nodenames, "%s\n",nodeinfo.nodename);
    fclose(nodenames);
    CHECK(CNET_set_handler( EV_APPLICATIONREADY, application_ready, 0));
    CHECK(CNET_set_handler( EV_PHYSICALREADY,    physical_ready, 0));
    printf("Node %d rebooted\n",nodeinfo.nodenumber);
    
    CNET_enable_application(ALLNODES);
}


