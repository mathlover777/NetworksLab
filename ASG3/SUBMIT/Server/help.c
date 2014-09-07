#ifndef HELP___
#define HELP___

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define PROGRUNBUFFER 1000 
#define MAXNUMOFARGUMENTS 10
#define MAXPATHCOUNT 30
#define MAXPATHSIZE 300
#define MAXARGSIZE 200


void readLine(FILE *fp,char *line,int *endflag){
	// memory for line must be already allocated
	if(*endflag==1){
		line[0]='\0';
		return;
	}
	char x;
	int i=0;
	x=getc(fp);
	while(1){
		if(x==EOF||x=='\n'){
			if(x==EOF){
				(*endflag)=1;
			}
			line[i]='\0';
			if(i>=1){
				if((int)line[i-1]==13){
					line[i-1]='\0';
				}
			}
			return;
		}else{
			line[i]=x;
			// printf("\nline[i] = %c",line[i]);
		}
		x=getc(fp);
		i++;
	}
}
void copyString(char *dest,const char *src){
	while((*src)!='\0'){
		(*dest)=(*src);
		dest++;
		src++;
	}
	(*dest)='\0';
	return;
}
void clearTabs(char *line){
	// removes spaces and tabs from line
	// like "   \t  bbbb   \t\t  rrrr \t  ggg\t  "--->"bbbb rrrr ggg"
	int i,j;
	// convert all tabs into spaces
	// printf("\n*****************BEFORE TAB REMOVAL : '%s'",line);
	i=j=0;
	while(line[i]!='\0'){
		if(line[i]=='\t'||line[i]==9||line[i]==11){
			// printf("\ntab detected !!");
			line[i]=' ';
		}
		i++;
	}
	// printf("\n****************AFTER TAB REMOVAL : '%s'",line);
	// all tabs are replaced by spaces,,,now we will remove redundant spaces[inplace]
	i=j=0;
	while(line[i]==' '){
		i++;
	}
	while(1){
		if(line[i]=='\0'){
			break;
		}
		if(line[i]!=' '){
			line[j]=line[i];
			j++;
			i++;
			continue;
		}
		line[j]=' ';
		i++;
		j++;
		while(line[i]==' '){
			i++;
		}
	}
	if(j-1>=0){
		if(line[j-1]==' '){
			line[j-1]='\0';
		}
	}
	line[j]='\0';
	// printf("\n****************FINAL TAB REMOVAL : '%s'",line);
	return;
}
void lineToWords(const char *xline,char **list,int *wCount,char separator){
	char word[PROGRUNBUFFER];
	char line[PROGRUNBUFFER];
	copyString(line,xline);
	clearTabs(line);
	int i=0;
	int lineend=0;
	int count=0;
	int j;
	while(1){
		j=0;
		while(1){
			if(line[i]=='\0'){
				lineend=1;
				break;
			}
			if(line[i]==separator){
				i++;
				break;
			}
			word[j]=line[i];
			i++;
			j++;
		}
		word[j]='\0';
		if(j!=0){
			copyString(list[count],word);
			count++;
		}
		if(lineend==1){
			break;
		}
	}
	(*wCount)=count;
	return;
}
int countChar(const char *line,char match){
	int spaceCount,i;
	i=spaceCount=0;
	char a=line[0];
	while(a!='\0'){
		// printf("%c\n",a);
		if(a==match){
			spaceCount++;
		}
		i++;
		a=line[i];
	}
	return spaceCount;
}
FILE* initializeFileRead(const char *filename,int *fileflag){
	(*fileflag)=0;
	FILE *fp=fopen(filename,"r");
	if(fp==NULL){
		printf("\nFATAL ERROR : FILE NOT FOUND !!\n");
		return NULL;
	}
	return fp;
}
void resetFile(const char *x){
	FILE *f=fopen(x,"w");
	// strcpy(FILENAME,x);
	fclose(f);
	return;
}
void addLine(const char *filename,const char *line){
	FILE *f=fopen(filename,"a");
	fprintf(f, "%s\n",line);
	fclose(f);
	return;
}
int isExist(const char *FILENAME){
	return 1;
	// int fp = open (FILENAME,  O_WRONLY | O_CREAT | O_TRUNC, 0);
	// if(fp==-1){
	// 	return 0;
	// }
	// close(fp);
	// return 1;
}
void getFirstWord(char *cmd,char *part1,char *part2){
	int i = 0;
	while(cmd[i]!=' '&&cmd[i]!='|'&&cmd[i]!='\0'){
		part1[i] = cmd[i];
		i++;
	}
	part1[i] = '\0';
	int j = 0;
	if(cmd[i]=='\0'){
		part2[0] = '\0';
		return;
	}
	i++;
	while(cmd[i]!='\0'){
		part2[j] = cmd[i];
		i++;
		j++;
	}
	part2[j] = '\0';
	return;
}
int execwrapper(char *cmd){
	// printf("\nIN EXECWRAPPER\n");
	// sleep(1);

	char parameterX[PROGRUNBUFFER];
	char progName[PROGRUNBUFFER];
	char progPath[PROGRUNBUFFER];// useless ... may take a default path parameter
	// in future
	char progPathName[2*PROGRUNBUFFER];
	progPath[0] = '\0';
	progName[0] = '\0';
	parameterX[0] = '\0';

	getFirstWord(cmd,progName,parameterX);

	char *arglist[MAXNUMOFARGUMENTS+1];
	char *pathList[MAXPATHCOUNT];
	for(int i=0;i<MAXPATHCOUNT;i++){
		pathList[i]=(char *)malloc(sizeof(char)*MAXPATHSIZE);
	}
	for(int i=0;i<MAXNUMOFARGUMENTS+1;i++){
		arglist[i] = (char *)malloc(sizeof(char)*MAXARGSIZE);
	}
	int argCount = countChar(parameterX,' ')+1;
	if(argCount>MAXNUMOFARGUMENTS){
		// printf("\nERROR : can not have more than %d arguments\n",MAXNUMOFARGUMENTS);
		return -1;
	}
	lineToWords(parameterX,arglist+1,&argCount,' ');
	copyString(arglist[0],progName);
	argCount++;
	free(arglist[argCount]);
	arglist[argCount]=NULL;
	sprintf(progPathName,"%s%s",progPath,progName);
	// printf("\nPROGPATHNAME = {%s}",progName);
	// printf("\nparameterX = {%s}",parameterX);
	// printf("\nPROGPATHNAME = {%s}",progPathName);
	int success=0;
	// assuming prog is in current directory
	success = execv(progPathName,arglist);
	if(success==-1){
		// if the previous execv is failed
		// then try execv for different path combinations
		// printf("\nEXEC UNSUCCESSFULL !!\n");
		// sleep(1);
		char* pPath;
		pPath = getenv ("PATH");
		int pathCount;
		pathCount = countChar(pPath,':')+1;
		lineToWords(pPath,pathList,&pathCount,':');
		for(int i=0;i<pathCount;i++){
			sprintf(progPathName,"%s/%s",pathList[i],progName);
			success = execv(progPathName,arglist);
		}
		if(success == -1){
			// printf("\nERROR : INVALID COMMAND--{%s}\n",progName);
		}
	}
	for(int i=0;i<argCount;i++){
		free(arglist[i]);
	}
	for(int i=argCount+1;i<MAXNUMOFARGUMENTS+1;i++){
		free(arglist[i]);
	}
	for(int i=0;i<MAXPATHCOUNT;i++){
		free(pathList[i]);
	}
	return success;
}

#endif