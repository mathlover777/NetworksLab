// the main client side program:

// on starting download "list.txt" from server

// ask user to specify session:

// download the specific pdf

// download the status_i.sh file

// open pdf in okular and get the windowid

// update window by xdotools

// start while loop:

// after a time period get the latest the latest status_i.sh file
// server program. And check with old status ... if modified
// run the status file or may be execute always.

// to close press ctrl+C or need to kill the process

// pdf will cease to exist.
// #include <stdio.h>
// #include <stdlib.h>

#include "ftpclient.c"
#include "gui.cpp"

#define MAXSESSIONCOUNT 100
#define LIST "list.txt"
#define CLIENTSTATUS "clientstatus.txt"

int getSessionCount(const char *filename){
	return 0;
}
void getAllSessionData(const char *list,int *sessionCount,int *sessionID,char **sessionNames){
	int flag;
	FILE *f = initializeFileRead(list,&flag);
	if(f==NULL){
		printf("\nERROR {%s} not found in cient !!\n",list);
		exit(-1);
	}
	*sessionCount = 0;
	char line[1000];
	while(1){
		readLine(f,line,&flag);
		if(line[0] == '\0'){
			break;
		}
		if(flag == 1){
			break;
		}
		sscanf(line,"%d %s",sessionID+(*sessionCount),sessionNames[*sessionCount]);
		(*sessionCount)++;
	}
	fclose(f);
	return;
}
void displaySessionData(int sessionCount,int *sessionID,char **sessionNames){
	printf("\n********** ACTIVE SESSIONS : *************\n");
	for(int i=0;i<sessionCount;i++){
		printf("\nSESSION ID = {%d} SESSION NAME = {%s}",sessionID[i],sessionNames[i]);
	}
	printf("\n******************************************");
	return;
}
void getDataFromStatus(const char *statusFile,int *page,int *zoom,int *width,int *height){
	FILE *f = fopen(statusFile,"r");
	char dummy[1000];
	fscanf(f,"%s",dummy);
	fscanf(f,"%s",dummy);
	fscanf(f,"%d %d %d %d",page,zoom,width,height);
	fclose(f);
	return;
}
void initClient(const char *pdfFile,const char *statusDownload,char *windowID,int session){
	int page,zoom,width,height;
	getDataFromStatus(statusDownload,&page,&zoom,&width,&height);
	launchPdf(pdfFile,windowID,page);
	printf("\nINIT STATUS :PAGE = %d ZOOM = %d WIDTH = %d HEIGHT = %d",
		page,zoom,width,height);
	initializeWindow(windowID,width,height);
	char call[1000];
	// sprintf(call,"cp %s clientStatus.txt",statusDownload);
	FILE *f = fopen(CLIENTSTATUS,"w");
	fprintf(f, "%s\n",windowID );
	fprintf(f, "%s\n",pdfFile );
	fprintf(f, "%d\n",page );
	fprintf(f, "%d\n",zoom );
	fprintf(f, "%d\n",width );
	fprintf(f, "%d\n",height );
	fclose(f);
	return;
}
void updateClientStatusFile(const char *status,const char *pdfFile,const char *windowID){
	char dummy[1000];
	int page,zoom,width,height;

	FILE *f = fopen(status,"r");
	fscanf(f, "%s",dummy );
	fscanf(f, "%s",dummy );
	fscanf(f, "%d",&page );
	fscanf(f, "%d",&zoom );
	fscanf(f, "%d",&width );
	fscanf(f, "%d",&height );
	fclose(f);	



	f = fopen(CLIENTSTATUS,"w");
	fprintf(f, "%s\n",windowID );
	fprintf(f, "%s\n",pdfFile );
	fprintf(f, "%d\n",page );
	fprintf(f, "%d\n",zoom );
	fprintf(f, "%d\n",width );
	fprintf(f, "%d\n",height );
	fclose(f);
	return;
}
void updateClient(int session,const char *statusDownload,const char *pdfFile,const char *windowID){
	int oldPage,oldZoom,oldWidth,oldHeight; 
	int newPage,newZoom,newWidth,newHeight;
	int n;
	while(1){
		// printf("\nEnter 1 to update:");
		// scanf("%d",&n);
		sleep(10);
		callClient(statusDownload);
		getDataFromStatus(CLIENTSTATUS,&oldPage,&oldZoom,&oldWidth,&oldHeight);
		getDataFromStatus(statusDownload,&newPage,&newZoom,&newWidth,&newHeight);
		printf("\nOLD STATUS : PAGE = %d ZOOM = %d WIDTH = %d HEIGHT = %d",
			oldPage,oldZoom,oldWidth,oldHeight);
		printf("\nNEW STATUS : PAGE = %d ZOOM = %d WIDTH = %d HEIGHT = %d",
			newPage,newZoom,newWidth,newHeight);
		// printf("\nTRY UPDATESTATUS :");
		// scanf("%d",&n);
		int changeFlag = updateStatus(CLIENTSTATUS,newPage,newZoom,newWidth,newHeight);
		updateClientStatusFile(statusDownload,pdfFile,windowID);
		if(changeFlag==1){
			// printf("\nTRY EXECUTING SCRIPT:");
			// scanf("%d",&n);
			system("./update.sh");
		}
	}
	return;
}
// void spinand
int main(){
	srand((unsigned)time(NULL));
	
	callClient(LIST);
	int sessionCount;
	int sessionID[MAXSESSIONCOUNT];
	char *sessionNames[MAXSESSIONCOUNT];
	for(int i=0;i<MAXSESSIONCOUNT;i++){
		sessionNames[i] = (char *)malloc(sizeof(char)*1000);
	}
	getAllSessionData(LIST,&sessionCount,sessionID,sessionNames);
	displaySessionData(sessionCount,sessionID,sessionNames);
	
	printf("\nEnter Session ID to Start Session :");
	int session;
	scanf("%d",&session);
	if(session<0||session>=sessionCount){
		printf("\nINVALID SESSION!!\n");
		exit(-1);
	}
	char pdfFile[1000];
	strcpy(pdfFile,sessionNames[session]);
	printf("\nSESSION SELECTED : ID {%d} NAME {%s}",session,pdfFile);
	callClient(pdfFile);
	char statusDownload[1000];
	sprintf(statusDownload,"status_%d.txt",session);
	callClient(statusDownload);
	char windowID[100];
	initClient(pdfFile,statusDownload,windowID,session);
	updateClient(session,statusDownload,pdfFile,windowID);
	for(int i=0;i<MAXSESSIONCOUNT;i++){
		free(sessionNames[i]);
	}
	return 0;
}