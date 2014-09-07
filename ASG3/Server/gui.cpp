#ifndef GUI___
#define GUI___
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <locale.h>
#include <langinfo.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include "help.c"

#define PAGE_DEFAULT 1
#define ZOOM_DEFAULT 0
#define WIDTH_DEFAULT 400
#define HEIGHT_DEFAULT 600


void freezeFTPServer(){
	return;
}
void resumeFTPServer(){
	return;
}
int getWindowId(int pid,char *windowID){
	// return -1 if unsuccessful
	char line[1000];
	resetFile("winID.txt");
	sprintf(line,"wmctrl -l -p|grep %d|awk '{print $1}' >winID.txt",pid);
	system(line);
	FILE *f = fopen("winID.txt","r");
	fscanf(f,"%s",windowID);
	fclose(f);
	return 0;
}

int launchPdf(const char *file,char *windowID,int page = 1){
	// returns window of opened pdf
	// -1 if unsuccessful
	int pid = fork();
	if(pid == 0){
		// child process
		char line[1000];
		sprintf(line,"okular %s -p %d",file,page);
		int status = 0;
		status = execwrapper(line);
		if(status==-1){
			printf("\nERROR can not launch {%s}\n",file);
		}
	}else{
		sleep(5);
		printf("\n**************\n");
		printf("\nPID = %d",pid);
		windowID[0]='\0';
		getWindowId(pid,windowID);
		printf("\nWINDOWID = {%s}",windowID);
	}
	return 0;
}
int getDigits(int n,int *digits,int *dCount){
	int x = n;
	*dCount = 0;
	while(x>0){
		x = x/10;
		(*dCount)++;
	}
	if(*dCount==0||*dCount>10){
		return 0;
	}
	int i=0;
	while(n>0){
		digits[i] = n%10;
		n = n/10;
		i++;
	}
	return 1;
}
int updateStatusFile(const char *status,const char *windowID,const char *filename,
	int page,int zoom,int width,int height){
	resetFile(status);
	addLine(status,windowID);
	addLine(status,filename);
	char line[100];
	sprintf(line,"%d",page);
	addLine(status,line);
	sprintf(line,"%d",zoom);
	addLine(status,line);
	sprintf(line,"%d",width);
	addLine(status,line);
	sprintf(line,"%d",height);
	addLine(status,line);
	return 1;
}
int initializeWindow(const char *windowID,int width = WIDTH_DEFAULT,int height = HEIGHT_DEFAULT){
	printf("\nEnter 1 if Window is maximised:");
	int n;
	scanf("%d",&n);
	char script[100];
	sprintf(script,"update.sh");
	resetFile(script);
	char line[1000];
	addLine(script,"export DISPLAY=:0.0");
	sprintf(line,"xdotool windowactivate %s",windowID);
	addLine(script,line);
	sprintf(line,"xdotool windowfocus %s",windowID);
	addLine(script,line);
	if(n==1){
		addLine(script,"xdotool key \"alt+space\"\nxdotool key x");
	}

	// addLine(script,"xdotool key \"alt+space\"\nxdotool key KP_Down\nxdotool key KP_Down\nxdotool key KP_Enter");
	
	sprintf(line,"xdotool windowsize %s %d %d",windowID,width,height);
	addLine(script,line);

	// sprintf(line,"xdotool key \"ctrl+g\"");
	// addLine(script,line);
	// sprintf(line,"xdotool key KP_Delete");
	// addLine(script,line);
	// sprintf(line,"xdotool key \"%d\"",PAGE_DEFAULT);
	// addLine(script,line);
	// sprintf(line,"xdotool key KP_Enter");
	// addLine(script,line);
	sprintf(line,"chmod +x %s",script);
	system(line);
	system("./update.sh");
	return 1;

}
int updateStatus(const char *status,int page,int zoom,int width,int height){
	// update status of a session
	// no check is done whether its on or not
	char script[100];
	sprintf(script,"update.sh");
	resetFile(script);
	int pageOld,zoomOld,widthOld,heightOld;
	char windowID[100];
	char filename[100];
	FILE *statusFile = fopen(status,"r");
	if(statusFile==NULL){
		printf("\nERROR {%s} not found !!\n",status);
		return 0;
	}
	fscanf(statusFile,"%s",windowID);
	fscanf(statusFile,"%s",filename);
	fscanf(statusFile,"%d%d%d%d",&pageOld,&zoomOld,&widthOld,&heightOld);
	fclose(statusFile);

	addLine(script,"export DISPLAY=:0.0");
	char line[1000];
	sprintf(line,"xdotool windowactivate %s",windowID);
	addLine(script,line);
	sprintf(line,"xdotool windowfocus %s",windowID);
	addLine(script,line);
	int changeFlag = 0;
	// set the page:
	if(pageOld!=page){
		changeFlag = 1;
		sprintf(line,"xdotool key \"ctrl+g\"");
		addLine(script,line);
		addLine(script,"sleep 1");
		sprintf(line,"xdotool key KP_Delete");
		addLine(script,line);
		int digits[10];
		int dCount = 0;
		getDigits(page,digits,&dCount);
		printf("\nNEW PAGE = %d\tDcount = %d",page,dCount);
		if(dCount==0||dCount>10){
			printf("\nERROR in modifying page dCount = %d",dCount);
			return 0;
		}
		for(int i=dCount-1;i>=0;i--){
			printf("\nDIGIT : %d",digits[i]);
			sprintf(line,"xdotool key %d",digits[i]);
			addLine(script,line);
			// sprintf(line,"xdotool key KP_Right");
			// addLine(script,line);
		}
		sprintf(line,"xdotool key KP_Enter");
		addLine(script,line);
	}
	// set the zoom level
	if(zoomOld!=zoom){
		changeFlag = 1;
		if(zoomOld<zoom){
			// need to zoom in
			for(int i=zoomOld;i<zoom;i++){
				sprintf(line,"xdotool key \"ctrl+plus\"");
				addLine(script,line);
			}
		}else{
			// need to zoom out
			for(int i=zoom;i<zoomOld;i++){
				sprintf(line,"xdotool key \"ctrl+minus\"");
				addLine(script,line);
			}
		}
	}
	// set the width and height
	if(widthOld!=width&&heightOld!=height){
		changeFlag = 1;
		sprintf(line,"xdotool windowsize %s %d %d",windowID,width,height);
		addLine(script,line);
	}else if(widthOld!=width){
		changeFlag = 1;
		sprintf(line,"xdotool windowsize %s %d %d",windowID,width,heightOld);
		addLine(script,line);
	}else if(heightOld!=height){
		changeFlag = 1;
		sprintf(line,"xdotool windowsize %s %d %d",windowID,widthOld,height);
		addLine(script,line);
	}
	// update the status file
	freezeFTPServer();
	updateStatusFile(status,windowID,filename,page,zoom,width,height);
	resumeFTPServer();
	// make the script executable
	sprintf(line,"chmod +x %s",script);
	system(line);
	return changeFlag;
}
#endif