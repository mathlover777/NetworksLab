#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

const char *ORIGINAL = "original.txt";
const char *SAMPLED = "sampled.txt";
const char *QUANTIZED = "quantized.txt";
const char *TRANSMITED = "transmited.txt";
const char *REQUANTIZED = "requantized.txt";
const char *INTERPOLATED = "interpolated.txt";

#define PI 3.141592
#define FSUPER 5000

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
    return error;
}
// int main(){
// 	interpolate("requantized.txt","interpolated.txt",5000);
//     double error = calc_mean_square_error("original.txt","interpolated.txt");
// 	printf("\nERROR = %lf",error);
//     printf("\n");
//     return 0;
// }

int main(int argc,char **argv){
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
	for(fSample = fMax/5;fSample<fMax * 4;fSample = fSample+.5){
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
	printf("\n");
	return 0;
}