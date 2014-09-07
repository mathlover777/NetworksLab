#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>




#define PI 3.141592
#define FSUPER 5000


const char *ORIGINAL = "original.txt";
const char *SAMPLED = "sampled.txt";
const char *QUANTIZED = "quantized.txt";
const char *TRANSMITED = "transmited.txt";
const char *REQUANTIZED = "requantized.txt";
const char *INTERPOLATED = "interpolated.txt";

const char *ORIGINAL_PLOT = "original_plot";
const char *SAMPLED_PLOT = "sampled_plot";
const char *QUANTIZED_PLOT = "quantized_plot";
const char *TRANSMITED_PLOT = "transmited_plot";
const char *REQUANTIZED_PLOT = "requantized_plot";
const char *INTERPOLATED_PLOT = "interpolated_plot";




/*****************FILE read write functions**************************/
double getLogBase2(double x){
	if(x<=0){
		printf("\nTaking log of non - positive number ,,, returning zero");
		return 0;
	}
	return log(x)/log(2);
}
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
/*******************************************************************/

/****************** Interpolation Code ******************************/
void interpolate(const char *inputfile,const char *interpolated,double freq){
	FILE *f,*g;
	f = fopen(inputfile,"r");
	int sampleCount;
	fscanf(f,"%d",&sampleCount);
	double t[sampleCount],amp[sampleCount];
	for(int i=0;i<sampleCount;i++){
		fscanf(f,"%lf %lf",t+i,amp+i);
	}
	fclose(f);

	gsl_interp_accel *acc = gsl_interp_accel_alloc ();
    gsl_spline *spline = gsl_spline_alloc (gsl_interp_cspline, sampleCount);
    gsl_spline_init(spline, t, amp, sampleCount);

    f = fopen("temp.txt","w");
    double tDiff = 1.0/freq;
    double t_i,amp_i;
    int sCount = 0;
    // printf("\nT = %lf",t[sampleCount-1]);
    for(t_i = t[0];t_i<t[sampleCount-1];t_i = t_i + tDiff){
    	amp_i = gsl_spline_eval(spline,t_i,acc);
    	fprintf(f, "%lf %lf\n",t_i,amp_i);
    	sCount++;
    }
    fclose(f);

    f = fopen(interpolated,"w");
    g = fopen("temp.txt","r");
    
    fprintf(f, "%d\n",sCount);
    for(int i=0;i<sCount;i++){
    	fscanf(g,"%lf %lf",&t_i,&amp_i);
    	fprintf(f, "%lf %lf\n",t_i,amp_i);
    }
    fclose(f);
    fclose(g);
    return;
}
/*****************************************************************/


/************** MEAN SQUARE ERROR COMPUTATION ********************/
double calc_mean_square_error(const char* f1,const char* f2){
    FILE *orig,*new_;
    int count,n;
    double time1,time2,value1,value2,error;
    orig = fopen(f1,"r");
    new_ = fopen(f2,"r");
    fscanf(orig,"%d",&count);
    fscanf(new_,"%d",&n);
    if(count < n){
        n = count;
    }
    else{
        count = n;
    }
    //printf("%d %d\n",count,n);
    while(count--){
        fscanf(orig,"%lf%lf",&time1,&value1);
        fscanf(new_,"%lf%lf",&time2,&value2);
        //printf("%lf %lf\n",value1,value2);
        error += (value1-value2)*(value1-value2);
    }
    fclose(orig);
    fclose(new_);
    error /= n;
    return sqrt(error);
}
/******************************************************************/
// int main(){
// 	interpolate("requantized.txt","interpolated.txt",5000);
//     double error = calc_mean_square_error("original.txt","interpolated.txt");
// 	printf("\nERROR = %lf",error);
//     printf("\n");
//     return 0;
// }

/******************** AUXILARY FUNCTIONS **************************/
void removeHeader(const char *source,const char *dest){
	FILE *fIn = fopen(source,"r");
	FILE *fOut = fopen(dest,"w");
	int n;
	double t,x;

	fscanf(fIn,"%d",&n);
	for(int i=0;i<n;i++){
		fscanf(fIn,"%lf %lf",&t,&x);
		fprintf(fOut, "%lf %lf\n",t,x);
	}
	fclose(fIn);
	fclose(fOut);
	return;
}
double fRand(double fMin, double fMax){
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}
/******************************************************************/

/************ SIGNAL GENERATION CODE ******************************/
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
	// printf("\nTcount = %d\tTdiff = %lf",tCount,tDiff);
	saveSignal(nComp,freq,amp,0,tDiff,tCount,filename);
	return;
}
void sample(int nComp,double *freq,double *amp,double fSample,const char *filename,double len){
	double tDiff = 1.0/fSample;
	int tCount = (int)(len * fSample);
	saveSignal(nComp,freq,amp,0,tDiff,tCount,filename);
	return;
}
/*********************************************************************/


/********************* TRANSMISSION CODE ******************************/
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
/***********************************************************************/

/************************ QUANTIZATION *********************************/
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
/*******************************************************************/

void plot(){
	
	removeHeader(ORIGINAL,ORIGINAL_PLOT);
	removeHeader(SAMPLED,SAMPLED_PLOT);
	removeHeader(QUANTIZED,QUANTIZED_PLOT);
	removeHeader(TRANSMITED,TRANSMITED_PLOT);
	removeHeader(REQUANTIZED,REQUANTIZED_PLOT);
	removeHeader(INTERPOLATED,INTERPOLATED_PLOT);

	char call[1000];
	/********* plot1 *******************/
	// syscall("gnuplot -persist -e "plot \"errordata.txt\" with lines");
	sprintf(call,"gnuplot -persist -e \"plot \\\"%s\\\" with lines,\\\"%s\\\" with points,\\\"%s\\\" with lines,\\\"%s\\\" with lines,\\\"%s\\\" with lines\"",
		ORIGINAL_PLOT,SAMPLED_PLOT,QUANTIZED_PLOT,TRANSMITED_PLOT,REQUANTIZED_PLOT);
	// printf("\n{%s}",call);
	system(call);
	/***********************************/
	/********* plot2 *******************/
	sprintf(call,"gnuplot -persist -e \"plot \\\"%s\\\" with lines,\\\"%s\\\" with lines,\\\"%s\\\" with lines,\\\"%s\\\" with lines\"",
		ORIGINAL_PLOT,QUANTIZED_PLOT,REQUANTIZED_PLOT,INTERPOLATED_PLOT);
	// printf("\n{%s}",call);
	system(call);
	/***********************************/
	return;
}

int part1(){
	double fMax,A,fSample,qDist,eMax,len;
	int nComp;
	double freq[100],amp[100];

	/******** getting the input **************************/
	printf("\nEnter Fmax : ");
	scanf("%lf",&fMax);
	printf("\nEnter Maximum Amplitude : ");
	scanf("%lf",&A);
	printf("\nEnter Number of Components : ");
	scanf("%d",&nComp);
	printf("\nEnter Sampling Freq : ");
	scanf("%lf",&fSample);
	// printf("\nEnter Qunatization Level : ");
	// scanf("%lf",&qDist);
	printf("\nEnter Maximum Error : ");
	scanf("%lf",&eMax);
	printf("\nEnter Length : ");
	scanf("%lf",&len);
	/*****************************************************/

	/*****************************************************/
	// sscanf(argv[1],"%lf",&fMax);
	// sscanf(argv[2],"%lf",&A);
	// sscanf(argv[3],"%d",&nComp);
	// sscanf(argv[4],"%lf",&fSample);
	// sscanf(argv[5],"%lf",&qDist);
	// sscanf(argv[6],"%lf",&eMax);
	// sscanf(argv[7],"%lf",&len);
	/*****************************************************/

	/******** generate signal parameters *****************/
	generateFreq_Amp(nComp,freq,amp,fMax,A);
	/*****************************************************/

	/**************** generate original signal ***********/
	createOriginalSignal(nComp,freq,amp,fMax,ORIGINAL,len);
	/*****************************************************/

	/**************** SAMPLING ***************************/
	sample(nComp,freq,amp,fSample,SAMPLED,len);
	/*****************************************************/

	/************* QUANTIZATION **************************/
	int l = (A * nComp /eMax);
	// printf("\nL = %d\teMax = %lf\n",l,eMax);
	quantize(SAMPLED,QUANTIZED,l,eMax);
	/******************************************************/

	/************* TRANSMISSION ***************************/
	transmit(QUANTIZED,TRANSMITED,eMax/2);
	/******************************************************/

	/************* REQUANTIZATION after getting the signal*/
	quantize(TRANSMITED,REQUANTIZED,l,eMax);
	/******************************************************/

	/****************** CUBIC SPLINE INTERPOLATION*********/
	interpolate(REQUANTIZED,INTERPOLATED,FSUPER);
	/******************************************************/

	/****************** Calculating MEAN SQUARE ERROR *****/
	double error = calc_mean_square_error(ORIGINAL,INTERPOLATED);
	printf("\nERROR = %lf",error);
	/******************************************************/
	
	/************** PLOTTING THE DATA *********************/
	plot();
	/******************************************************/

	printf("\n");
	return 0;
}
double predictfMax(){

	double freq,error,fMax,errorchange,freq_old,error_old;
	double fMax2;
	double errorDiff,errorDiffMax;
	int fileRead;
	FILE *f = initializeFileRead("errordata.txt",&fileRead);
	char line[1000];
	readLine(f,line,&fileRead);

	sscanf(line,"%lf %lf",&freq_old,&error_old);
	errorDiffMax =0;
	while(1){
		readLine(f,line,&fileRead);
		if(fileRead==1){
			break;
		}
		sscanf(line,"%lf %lf",&freq,&error);
		errorDiff = error_old - error;
		if(errorDiff>errorDiffMax){
			errorDiffMax = errorDiff;
			fMax2 = freq;
		}
		error_old = error;
		freq_old = freq;
	}
	fclose(f);
	return (fMax2/2);
}
double getMaximum(double *A,int n){
	if(n <0){
		printf("\nINVALID ARRAY !!");
		return 0;
	}
	double max = A[0];
	for(int i=1;i<n;i++){
		if(A[i]>max){
			max = A[i];
		}
	}
	return max;
}

int part2(){
	double fMax,A,fSample,qDist,eMax,len;
	int nComp;
	double freq[100],amp[100];
	// sscanf(argv[1],"%lf",&fMax);
	// sscanf(argv[2],"%lf",&A);
	// sscanf(argv[3],"%d",&nComp);
	// // sscanf(argv[4],"%lf",&fSample);
	// // sscanf(argv[5],"%lf",&qDist);
	// sscanf(argv[4],"%lf",&eMax);
	// sscanf(argv[5],"%lf",&len);

	printf("\nEnter Fmax : ");
	scanf("%lf",&fMax);
	printf("\nEnter Maximum Amplitude : ");
	scanf("%lf",&A);
	printf("\nEnter Number of Components : ");
	scanf("%d",&nComp);
	// printf("\nEnter Sampling Freq : ");
	// scanf("%lf",&fSample);
	// printf("\nEnter Qunatization Level : ");
	// scanf("%lf",&qDist);
	printf("\nEnter Maximum Error : ");
	scanf("%lf",&eMax);
	printf("\nEnter Length : ");
	scanf("%lf",&len);


	// /**********************************************/

	// ******** signal parameters *****************
	generateFreq_Amp(nComp,freq,amp,fMax,A);
	// /**********************************************/

	// /********* generate original signal ***********/
	createOriginalSignal(nComp,freq,amp,fMax,ORIGINAL,len);
	// /**********************************************/
	int l = (A * nComp /eMax);
	
	printf("\noriginal created\n");

	FILE *f = fopen("errordata.txt","w");
	double error;
	// printf("\nFMAX * 4 = %lf",fMax *4);
	for(fSample = fMax;fSample<fMax * 4;fSample = fSample+.5){
		// printf("\nFSAMPLE = %lf",fSample);
		sample(nComp,freq,amp,fSample,SAMPLED,len);
		// printf("\nL = %d\teMax = %lf\n",l,eMax);
		quantize(SAMPLED,QUANTIZED,l,eMax);
		transmit(QUANTIZED,TRANSMITED,eMax/2);
		quantize(TRANSMITED,REQUANTIZED,l,eMax);
		interpolate(REQUANTIZED,INTERPOLATED,5000);
		error = calc_mean_square_error("original.txt","interpolated.txt");
		fprintf(f,"%lf %lf\n",fSample,error);
	}
	fclose(f);
	char call[1000];
	sprintf(call,"gnuplot -persist -e \"plot \\\"errordata.txt\\\" with lines\"");
	system(call);
	// printf("\nWE ARE HERE!!\n");
	double FMAX = getMaximum(freq,nComp);
	// printf("\nFmax = %lf",(predictfMax()+FMAX)/2);
	printf("\nFmax = %lf",predictfMax());
	/******************************************************************/
	f = fopen("errordata_vsl.txt","w");
	// printf("\nFMAX * 4 = %lf",fMax *4);
	double SNR = (double)l;
	double LEFT = (fMax * getLogBase2(1+SNR))/(2*FMAX);
	int maxL = (int)pow(2,LEFT);
	printf("\nMAX L = %d",maxL);
	fSample = 2*fMax;
	for(l = 1;l<maxL;l++){
		// printf("\nFSAMPLE = %lf",fSample);
		sample(nComp,freq,amp,fSample,SAMPLED,len);
		// printf("\nL = %d\teMax = %lf\n",l,eMax);
		quantize(SAMPLED,QUANTIZED,l,((double)(A * nComp))/((double)l));
		transmit(QUANTIZED,TRANSMITED,eMax);
		quantize(TRANSMITED,REQUANTIZED,l,((double)(A * nComp))/((double)l));
		interpolate(REQUANTIZED,INTERPOLATED,5000);
		error = calc_mean_square_error("original.txt","interpolated.txt");
		fprintf(f,"%d %lf\n",l,error);
	}
	fclose(f);
	// char call[1000];
	sprintf(call,"gnuplot -persist -e \"plot \\\"errordata_vsl.txt\\\" with lines\"");
	system(call);
	// printf("\nFmax = %lf",predictfMax());

	printf("\n");
	return 0;
}


int main(){
	printf("\nRUNNING PART1 : ");
	part1();
	printf("\nPART1 FINISHED !!");

	printf("\nRUNNING PART2 : ");
	part2();
	// printf("\nPART2 FINISHED !!");

	printf("\n");
	return 0;
}