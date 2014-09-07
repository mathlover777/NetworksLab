#ifndef STRINGFILE
#define STRINGFILE

#include <stdio.h>
#include <stdlib.h>
#include "fileio.c"

void copyString(char *dest,const char *src){
	while((*src)!='\0'){
		(*dest)=(*src);
		dest++;
		src++;
	}
	(*dest)='\0';
	return;
}
int isEqual(const char *a,const char *b){
	while((*a)!='\0'){
		if((*a)!=(*b)){
			return 0;
		}
		a++;
		b++;
	}
	if((*a)!=(*b)){
		return 0;
	}
	return 1;
}
void lineToWords(const char *xline,char **list,int *wCount){
	char word[1000];
	char line[1000];
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
			if(line[i]==' '){
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

#endif