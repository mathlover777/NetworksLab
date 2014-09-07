#ifndef FILEIO
#define FILEIO

/*****************FILE read write functions**************************/
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
FILE* initializeFileRead(const char *filename,int *fileflag){
	(*fileflag)=0;
	FILE *fp=fopen(filename,"r");
	if(fp==NULL){
		printf("\nFATAL ERROR : FILE NOT FOUND !!\n");
		return NULL;
	}
	return fp;
}
void resetfile(const char *x){
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
	FILE *fp=fopen(FILENAME,"r");
	if(fp==NULL){
		return 0;
	}
	fclose(fp);
	return 1;
}
/********************************************************************/
// INT_CONST prog
// 	{
// 		printf("\nINT = {%s}",$1);
// 	}
// 	|
// 	FLOAT_CONST prog
// 	{
// 		printf("\nFLOAT = {%s}",$1);
// 	}
// 	|

#endif