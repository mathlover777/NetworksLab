#include <stdio.h>
#include <stdlib.h>
#include "createsignal.cpp"

const char *ORIGINAL = "original.txt";
const char *SAMPLED = "sampled.txt";
const char *QUANTIZED = "quantized.txt";
const char *TRANSMITED = "transmited.txt";
const char *REQUANTIZED = "requantized.txt";
void transmit(const char *input,const char *output,double error){
	FILE *fin = fopen(input,"r");
	FILE *fout = fopen(output,"w");
	int sampleCount;
	fscanf(fin,"%d",&sampleCount);
	fprintf(fout,"%d\n",sampleCount);
	double t,amp;
	for(int i=0;i<sampleCount;i++){
		fscanf(fin,"%lf %lf",&t,&amp);
		if(rand()%2==1){
			// positive
			amp = amp + fRand(0,error);
		}else{
			// negative
			amp = amp - fRand(0,error);
		}
		fprintf(fout, "%lf %lf\n",t,amp);
	}
	fclose(fin);
	fclose(fout);
	return;
}
void quantize(const char* sampled,const char *quantized,int l,double width){
	FILE *fIn = fopen(sampled,"r");
	FILE  *fOut = fopen(quantized,"w");
	int i, count;
	double time1,value1,value2,temp;
	fscanf(fIn,"%d",&count);
	fprintf(fOut,"%d\n",count);
	for(i = 0; i < count; i++){
		fscanf(fIn,"%lf%lf",&time1,&value1);
		if(value1 > (width * l)){
			value2 = width * l;
			fprintf(fOut,"%lf %lf\n",time1,value2);
			continue;
		}
		if(value1 < (-width * l)){
			value2 = -width * l;
			fprintf(fOut,"%lf %lf\n",time1,value2);
			continue;	
		}
		temp = value1 / width;
		if(temp >= 0){
			value2 = ((int)(temp + 0.5)) * width;
		}
		else{
			value2 = ((int)(temp - 0.5)) * width;
		}
		fprintf(fOut,"%lf %lf\n",time1,value2);
	}
	fclose(fIn);
	fclose(fOut);
	return;
}

int main(int argc,char **argv){
	double fMax,A,fSample,qDist,eMax,len;
	int nComp;
	double freq[100],amp[100];

	/******** getting the input ******************/
	// printf("\nEnter Fmax : ");
	// scanf("%lf",&fMax);
	// printf("\nEnter Maximum Amplitude : ");
	// scanf("%lf",&A);
	// printf("\nEnter Number of Components : ");
	// scanf("%d",&nComp);
	// printf("\nEnter Sampling Freq : ");
	// scanf("%lf",&fSample);
	// printf("\nEnter Qunatization Level : ");
	// scanf("%lf",&qDist);
	// printf("\nEntet Maximum Error : ");
	// scanf("%lf",&eMax);
	// printf("\nEnter Lenght : ");
	// scanf("%lf",&len);
	sscanf(argv[1],"%lf",&fMax);
	sscanf(argv[2],"%lf",&A);
	sscanf(argv[3],"%d",&nComp);
	sscanf(argv[4],"%lf",&fSample);
	sscanf(argv[5],"%lf",&qDist);
	sscanf(argv[6],"%lf",&eMax);
	sscanf(argv[7],"%lf",&len);
	// /**********************************************/

	// ******** signal parameters *****************
	generateFreq_Amp(nComp,freq,amp,fMax,A);
	// /**********************************************/

	// /********* generate original signal ***********/
	createOriginalSignal(nComp,freq,amp,fMax,ORIGINAL,len);
	// /**********************************************/
	sample(nComp,freq,amp,fSample,SAMPLED,len);

	int l = (A * nComp /eMax);
	printf("\nL = %d\teMax = %lf\n",l,eMax);
	quantize(SAMPLED,QUANTIZED,l,eMax);
	transmit(QUANTIZED,TRANSMITED,eMax/2);
	quantize(TRANSMITED,REQUANTIZED,l,eMax);
	
	printf("\n");
	return 0;
}