#ifndef CREATE_SIGNAL
#define CREATE_SIGNAL


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "fileio.c"

#define PI 3.141592
#define FSUPER 5000


double fRand(double fMin, double fMax){
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

void generateFreq_Amp(int nComp,double *freq,double *amp,double fMax,double A){
	for(int i=0;i<nComp;i++){
		freq[i] = fRand(0,fMax);
		amp[i] = fRand(0,A);
		printf("\nFREQ[%d] = %lf AMP[%d] = %lf",i,freq[i],i,amp[i]);
	}
	return;
}

double getSignal(int nComp,double *freq,double *amp,double t){
	double sum = 0.0;
	for(int i=0;i<nComp;i++){
		sum = sum + amp[i]*sin(2*PI*freq[i]*t);
	}
	return sum;
}

void saveSignal(int nComp,double *freq,double *amp,double tStart,double tDiff,int tCount,const char *filename){
	resetfile(filename);
	char line[1000];
	double T = tStart;
	sprintf(line,"%d",tCount);
	addLine(filename,line);
	for(int i=0;i<tCount;i++){
		sprintf(line,"%lf %lf",T,getSignal(nComp,freq,amp,T));
		addLine(filename,line);
		T = T + tDiff;
	}
	return;
}
void createOriginalSignal(int nComp,double *freq,double *amp,double fMax,const char *filename,double len){
	double tDiff;
	double fSample = FSUPER;
	tDiff = 1.0/fSample;
	int tCount = (int)(len * fSample);
	printf("\nTcount = %d\tTdiff = %lf",tCount,tDiff);
	saveSignal(nComp,freq,amp,0,tDiff,tCount,filename);
	return;
}
void sample(int nComp,double *freq,double *amp,double fSample,const char *filename,double len){
	double tDiff = 1.0/fSample;
	int tCount = (int)len * fSample;
	saveSignal(nComp,freq,amp,0,tDiff,tCount,filename);
	return;
}
#endif