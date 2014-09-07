#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "exec_wrapper.c"
#include <signal.h>
#include "config.h"


#define PROBCORRUPT 5
#define MINWINSIZE 1
#define MAXWINSIZE 7


int runEXP(const char *expname){
	int pid = fork();
	if(pid == 0){
		char cmd[1000];
		sprintf(cmd,"cnet %s -W",expname);
		printf("\nEXECING : {%s}\n",cmd);
		execwrapper(cmd);
		// system(cmd);
		// printf("\nEXEC UNSUCCESSFUL !!\n");
		exit(0);
	}else{
		return pid;
	}
}

int main(){
	char headerfile[2000];
	char topologyfile[2000];
	char killcmd[1000];
	FILE *f = fopen(RESULT_SELECTIVEARQ,"w");
	FILE *ftop;
	int temp;
	char touchCmd[1000];
	fclose(f);
	for(int i = 1;i<=PROBCORRUPT;i++ ){
		for(int j = MINWINSIZE;j<=MAXWINSIZE;j++){
			int winsize = (int)pow(2,j);
			double probability = pow(.5,i);
			printf("**i = %d j = %d WINSIZE = %d CORRUPTPROB = %lf\n",i,j,winsize,probability);
			sprintf(headerfile,"#ifndef MAXSEQ___\n#define MAXSEQ %d\n#define PROBCORRUPT %lf\n#endif",winsize,probability);
			// printf("\n******************\n%s\n*******************\n",headerfile);
			f = fopen("maxseq.h","w");
			fprintf(f, "%s\n", headerfile);
			fclose(f);
			sprintf(topologyfile,"compile = \"%s\"\nbandwidth = 10Mbps,\npropagationdelay = 400usecs,\nprobframecorrupt = %d,\nhost sender {\n   link to receiver\n}\nhost receiver {\n}",CFILE_SELECTIVEARQ,i);
			ftop = fopen(TOPOLOGYFILE_SELECTIVE,"w");
			fprintf(ftop, "%s\n", topologyfile);
			fclose(ftop);
			sprintf(touchCmd,"touch %s",CFILE_SELECTIVEARQ);
			system(touchCmd);
			int pid = runEXP(TOPOLOGYFILE_SELECTIVE);
			// printf("\nRETURNED FROM FUNCTION");
			sleep(SLEEPTIME);
			// sprintf(killcmd,"kill %d",pid);
			// system(killcmd);
			kill(pid,SIGINT);
			// waitpid(pid,0,0);
			// printf("\nPress Any KEY TO CONTINUE:");
			// scanf("%d",&temp);
		}
	}
	return 0;
}