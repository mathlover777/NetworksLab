#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "help.c"
#include "gui.cpp"
/*
LIST of commands:

ADD filewithpath

<adds a new session with a default sh file>

UPDATEPAGE session page

UPDATEWIDTH session width

UPDATEHEIGHT session height

UPDATEZOOM session level [level = 0 normal view, level>+1 larger <0 smaller]
*/

#define BUFFER 1000
#define MAXSESSION 100



int sessionCount;
int width[MAXSESSION],height[MAXSESSION],page[MAXSESSION],zoom[MAXSESSION];
int ftpPid = 0;

int addNewSession(int session,const char *file){
	char line[1000];
	sprintf(line,"%d       %s",session,file);
	freezeFTPServer();
	addLine("list.txt",line);
	resumeFTPServer();
	return 1;
}

int createSession(const char *file){
	// creates and initiates a new session
	if(sessionCount==MAXSESSION){
		printf("\nCANT HAVE ANYMORE STATUS !!\n");
		return 0;
	}
	if(isExist(file)!=1){
		printf("\nCANT CREATE NEW SESSION : FILE {%s} NOT FOUND !!",file);
		return 0;
	}
	int session = sessionCount;
	sessionCount++;
	addNewSession(session,file);
	char statusFile[100];
	sprintf(statusFile,"status_%i.txt",session);
	resetFile(statusFile);
	char line[1000];
	// launch the session and store windowid in windowId
	char windowID[100];
	windowID[0] = '\0';
	launchPdf(file,windowID);
	// initialize the new openned session
	initializeWindow(windowID);
	sprintf(line,"%s",windowID);
	addLine(statusFile,line);
	sprintf(line,"%s",file);
	addLine(statusFile,line);
	sprintf(line,"%d",PAGE_DEFAULT);
	addLine(statusFile,line);
	sprintf(line,"%d",ZOOM_DEFAULT);
	addLine(statusFile,line);
	sprintf(line,"%d",WIDTH_DEFAULT);
	addLine(statusFile,line);
	sprintf(line,"%d",HEIGHT_DEFAULT);
	addLine(statusFile,line);
	page[session] = PAGE_DEFAULT;
	zoom[session] = ZOOM_DEFAULT;
	width[session] = WIDTH_DEFAULT;
	height[session] = HEIGHT_DEFAULT;
	return 1;
}
int updateSession(int session,int upage,int uzoom,int uwidth,int uheight){
	// currently content changing ie changing the doc is not supported
	printf("\nSESSION : %d",session);
	printf("\nOLD STATUS:");
	printf("\nPAGE = %d ZOOM = %d WIDTH = %d HEIGHT = %d",page[session],
		zoom[session],width[session],height[session]);
	printf("\nNEW STATUS:");
	page[session] = upage;
	zoom[session] = uzoom;
	width[session] = uwidth;
	height[session] = uheight;
	char status[100];
	sprintf(status,"status_%d.txt",session);
	updateStatus(status,page[session],zoom[session],width[session],height[session]);
	printf("\nPAGE = %d ZOOM = %d WIDTH = %d HEIGHT = %d",page[session],
		zoom[session],width[session],height[session]);
	// execute the script to modify the status
	system("./update.sh");
	return 0;
}

void resetServer(){
	for(int i=0;i<MAXSESSION;i++){
		width[i] = height[i] = page[i] = -1;
		zoom[i] = 0;
	}
	sessionCount = 0;
	resetFile("list.txt");
	return;
}

int updatePage(int session,int upage){
	if(session<0||session>sessionCount){
		printf("\nONLY %d sessions are running !!",sessionCount);
		return 0;
	}
	if(page[session]==upage){
		printf("\nNo change !!\n");
		return 1;
	}
	updateSession(session,upage,zoom[session],width[session],height[session]);
	return 1;
}
int updateWidth(int session,int uwidth){
	if(session<0||session>sessionCount){
		printf("\nONLY %d sessions are running !!",sessionCount);
		return 0;
	}
	if(width[session]==uwidth){
		printf("\nNo change !!\n");
		return 1;
	}
	updateSession(session,page[session],zoom[session],uwidth,height[session]);
	return 1;
}
int updateHeight(int session,int uheight){
	if(session<0||session>sessionCount){
		printf("\nONLY %d sessions are running !!",sessionCount);
		return 0;
	}
	if(height[session]==uheight){
		printf("\nNo change !!\n");
		return 1;
	}
	updateSession(session,page[session],zoom[session],width[session],uheight);
	return 1;
}
int updateZoom(int session,int uzoom){
	if(session<0||session>sessionCount){
		printf("\nONLY %d sessions are running !!",sessionCount);
		return 0;
	}
	if(zoom[session]==uzoom){
		printf("\nNo change !!\n");
		return 1;
	}
	updateSession(session,page[session],uzoom,width[session],height[session]);
	return 1;
}


int getCommand(char *line,char *cmd,int *session,char *file,char *arg3){
	// return the first word
	*session = -1;
	arg3[0] = '\0';
	file[0] = '\0';
	cmd[0] = '\0';
	clearTabs(line);
	char *word[10];
	for(int i=0;i<10;i++){
		word[i] = (char *)malloc(sizeof(char)*100);
	}
	int wCount;
	lineToWords(line,word,&wCount,' ');
	copyString(cmd,word[0]);
	if(strcmp(word[0],"ADD")==0){
		if(wCount!=2){
			for(int i=0;i<10;i++){
				free(word[i]);
			}
			return 0;			
		}
		copyString(file,word[1]);
		for(int i=0;i<10;i++){
			free(word[i]);
		}
		return 1;
	}
	if(wCount!=3){
		for(int i=0;i<10;i++){
			free(word[i]);
		}
		return 0;
	}
	*session = atoi(word[1]);
	copyString(arg3,word[2]);
	for(int i=0;i<10;i++){
		free(word[i]);
	}
	return 1;
}
void commandInterpreter(){
	char line[BUFFER],cmd[BUFFER];
	int session;
	char file[200];
	char arg3[200];
	int flag;
	while(1){
		printf("\nEnter Command:");
		gets(line);
		// printf("\nSTRING :{%s}\n",line);
		if(strcmp(line,"EXIT")==0){
			break;
		}
		if(strcmp(line,"LIST")==0){
			printf("\nACTIVE SESSIONS :\n");
			system("cat list.txt");
			continue;
		}
		flag = getCommand(line,cmd,&session,file,arg3);
		if(flag==0){
			printf("\nINVALID COMMAND:line = {%s}",line);
			continue;
		}
		if(strcmp(cmd,"ADD")==0){
			createSession(file);
			continue;
		}
		if(strcmp(cmd,"UPDATEPAGE")==0){
			updatePage(session,atoi(arg3));
			continue;	
		}
		if(strcmp(cmd,"UPDATEWIDTH")==0){
			updateWidth(session,atoi(arg3));
			continue;
		}
		if(strcmp(cmd,"UPDATEHEIGHT")==0){
			updateHeight(session,atoi(arg3));
			continue;
		}
		if(strcmp(cmd,"UPDATEZOOM")==0){
			updateZoom(session,atoi(arg3));
			continue;
		}
		printf("\nINVALID COMMAND : \nline = {%s}\ncmd = {%s}",line,cmd);
	}
	return;
}

int main(){
	system("reset");
	resetServer();
	commandInterpreter();
	printf("\n");
	return 0;
}