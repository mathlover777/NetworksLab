#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>




#define PI (4*atan(1))
#define FSUPER 5000
/*********** part 1 *****************************/
#define ORIGINAL1_PART1 "original1_part1.txt"
#define ORIGINAL2_PART1 "original2_part1.txt"
#define SAMPLED1_PART1 "sampled1_part1.txt"
#define SAMPLED2_PART1 "sampled2_part1.txt"
#define PADDED1_PART1 "padded1_part1.txt"
#define PADDED2_PART1 "padded2_part1.txt"
#define SPECTRUM1_PART1 "spectrum1_part1.txt"
#define SPECTRUM2_PART1 "spectrum2_part1.txt"
#define SHIFTEDSPECTRUM1_PART1 "shiftedspectrum1_part1.txt"
#define SHIFTEDSPECTRUM2_PART1 "shiftedspectrum2_part1.txt"
#define MIXEDSPECTRUM_PART1 "mixedspectrum_part1.txt"
#define REFLECTED_MIXEDSPECTRUM_PART1 "reflected_mixedspectrum_part1.txt"
#define MIXEDSIGNAL_PART1 "mixedsignal_part1.txt"
#define MIXEDSPECTRUM_OUTPUT_PART1 "mixedspectrum_output_part1.txt"
#define SPECTRUM_OUTPUT1_PART1 "spectrum_output1_part1.txt"
#define SPECTRUM_OUTPUT2_PART1 "spectrum_output2_part1.txt"
#define OUTPUT_SOURCE1_PART1 "output_source1_part1.txt"
#define OUTPUT_SOURCE2_PART1 "output_source2_part1.txt"
#define SPECTRUM_OUTPUT1_SHIFTED_PART1 "spectrum_output1_shifted_part1.txt"
#define SPECTRUM_OUTPUT2_SHIFTED_PART1 "spectrum_output2_shifted_part1.txt"
#define SPECTRUM_OUTPUT1_REFLECTED_PART1 "spectrum_output1_reflected_part1.txt"
#define SPECTRUM_OUTPUT2_REFLECTED_PART1 "spectrum_output2_reflected_part1.txt"

/*************** part 2 ****************************/
#define ORIGINAL1_PART2 "original1_part2.txt"
#define ORIGINAL2_PART2 "original2_part2.txt"
#define SAMPLED1_PART2 "sampled1_part2.txt"
#define SAMPLED2_PART2 "sampled2_part2.txt"
#define PADDED1_PART2 "padded1_part2.txt"
#define PADDED2_PART2 "padded2_part2.txt"
#define SPECTRUM1_PART2 "spectrum1_part2.txt"
#define SPECTRUM2_PART2 "spectrum2_part2.txt"
#define SHIFTEDSPECTRUM1_PART2 "shiftedspectrum1_part2.txt"
#define SHIFTEDSPECTRUM2_PART2 "shiftedspectrum2_part2.txt"
#define MIXEDSPECTRUM_PART2 "mixedspectrum_part2.txt"
#define REFLECTED_MIXEDSPECTRUM_PART2 "reflected_mixedspectrum_part2.txt"
#define MIXEDSIGNAL_PART2 "mixedsignal_part2.txt"
#define MIXEDSPECTRUM_OUTPUT_PART2 "mixedspectrum_output_part2.txt"
#define SPECTRUM_OUTPUT1_PART2 "spectrum_output1_part2.txt"
#define SPECTRUM_OUTPUT2_PART2 "spectrum_output2_part2.txt"
#define OUTPUT_SOURCE1_PART2 "output_source1_part2.txt"
#define OUTPUT_SOURCE2_PART2 "output_source2_part2.txt"
#define SPECTRUM_OUTPUT1_SHIFTED_PART2 "spectrum_output1_shifted_part2.txt"
#define SPECTRUM_OUTPUT2_SHIFTED_PART2 "spectrum_output2_shifted_part2.txt"
#define SPECTRUM_OUTPUT1_REFLECTED_PART2 "spectrum_output1_reflected_part2.txt"
#define SPECTRUM_OUTPUT2_REFLECTED_PART2 "spectrum_output2_reflected_part2.txt"
#define TRANSMITTED_PART2 "transmitted_part2.txt"


/*************** part 3 ****************************/
#define ORIGINAL1_PART3 "original1_part3.txt"
#define ORIGINAL2_PART3 "original2_part3.txt"
#define SAMPLED1_PART3 "sampled1_part3.txt"
#define SAMPLED2_PART3 "sampled2_part3.txt"
#define PADDED1_PART3 "padded1_part3.txt"
#define PADDED2_PART3 "padded2_part3.txt"
#define SPECTRUM1_PART3 "spectrum1_part3.txt"
#define SPECTRUM2_PART3 "spectrum2_part3.txt"
#define SHIFTEDSPECTRUM1_PART3 "shiftedspectrum1_part3.txt"
#define SHIFTEDSPECTRUM2_PART3 "shiftedspectrum2_part3.txt"
#define MIXEDSPECTRUM_PART3 "mixedspectrum_part3.txt"
#define REFLECTED_MIXEDSPECTRUM_PART3 "reflected_mixedspectrum_part3.txt"
#define MIXEDSIGNAL_PART3 "mixedsignal_part3.txt"
#define MIXEDSPECTRUM_OUTPUT_PART3 "mixedspectrum_output_part3.txt"
#define SPECTRUM_OUTPUT1_PART3 "spectrum_output1_part3.txt"
#define SPECTRUM_OUTPUT2_PART3 "spectrum_output2_part3.txt"
#define OUTPUT_SOURCE1_PART3 "output_source1_part3.txt"
#define OUTPUT_SOURCE2_PART3 "output_source2_part3.txt"
#define SPECTRUM_OUTPUT1_SHIFTED_PART3 "spectrum_output1_shifted_part3.txt"
#define SPECTRUM_OUTPUT2_SHIFTED_PART3 "spectrum_output2_shifted_part3.txt"
#define SPECTRUM_OUTPUT1_REFLECTED_PART3 "spectrum_output1_reflected_part3.txt"
#define SPECTRUM_OUTPUT2_REFLECTED_PART3 "spectrum_output2_reflected_part3.txt"
#define TRANSMITTED_PART3 "transmitted_part3.txt"
#define QUANTIZED_PART3 "quantized_part3.txt"
#define REQUANTIZED_PART3 "requantized_part3.txt"
#define INTERPOLATED_PART3 "interpolated_part3.txt"



#define INTERMEDIATE "intermediate_shift.txt"


/****************** FFT CODES ******************************/
struct complex{
	double real;
	double imaginary;
};

typedef struct complex Complex;

Complex add(Complex a, Complex b){
	Complex c;
	c.real = a.real + b.real;
	c.imaginary = a.imaginary + b.imaginary;
	return c;
}
Complex sub(Complex a, Complex b){
	Complex c;
	c.real = a.real - b.real;
	c.imaginary = a.imaginary - b.imaginary;
	return c;
}
Complex mult(Complex a, Complex b){
	Complex c;
	c.real = a.real * b.real - a.imaginary * b.imaginary;
	c.imaginary = a.imaginary * b.real + a.real * b.imaginary;
	return c;
}
Complex getWn(int n){
	Complex a;
	a.real = cos(2*PI/((double)n));
	a.imaginary = sin(2*PI/((double)n));
	return a;
}
double getMagnitude(Complex a){
	return sqrt((a.real*a.real) + (a.imaginary*a.imaginary));
}
void fft(Complex *a,Complex *A,int n){
	if(n<=1){
		if(n==1){
			A[0] = a[0];
		}
		return;
	}
	Complex Wn = getWn(n);
	Complex w;
	w.real = 1;
	w.imaginary = 0;
	int n_2 = n/2;
	// printf("\nn = %d n_2 = %d",n,n_2);
	Complex *a_even,*a_odd,*A_even,*A_odd;
	a_even = (Complex *)malloc(sizeof(Complex)*n_2);
	a_odd = (Complex *)malloc(sizeof(Complex)*n_2);
	A_even = (Complex *)malloc(sizeof(Complex)*n_2);
	A_odd = (Complex *)malloc(sizeof(Complex)*n_2);
	for(int i =0;i<n_2;i++){
		a_even[i] = a[i*2];
		a_odd[i] = a[i*2+1];
	}
	fft(a_even,A_even,n_2);
	fft(a_odd,A_odd,n_2);
	// printf("\n n = %d",n);
	// printf("\nEVEN {");
	// for(int i=0;i<n_2;i++){
	// 	printf(" (%.10lf,%.10lf) ",A_even[i].real,A_even[i].imaginary);
	// }
	// printf("}\n");
	// printf("\nODD {");
	// for(int i=0;i<n_2;i++){
	// 	printf(" (%.10lf,%.10lf) ",A_odd[i].real,A_odd[i].imaginary);
	// }
	// printf("}\n");
	Complex temp;
	for(int k=0;k<n_2;k++){
		// printf("\nw_%d = (%.10lf,%.10lf),A_odd[%d]=(%.10lf,%.10lf)",k,w.real,w.imaginary,k,A_odd[k].real,A_odd[k].imaginary);
		temp = mult(w,A_odd[k]);
		// printf("\ntemp = (%.10lf,%.10lf)",temp.real,temp.imaginary);
		A[k] = add(A_even[k],temp);
		A[k+n_2] = sub(A_even[k],temp);
		temp = mult(w,Wn);
		w = temp;
	}
	free(a_odd);
	free(a_even);
	free(A_even);
	free(A_odd);
	return;
}
void ifft(Complex *A,Complex *a,int n){
	fft(A,a,n);
	int n_2 = n/2;
	for(int i=0;i<n;i++){
		a[i].real = a[i].real/n;
		a[i].imaginary = a[i].imaginary/n;
	}
	Complex temp;
	for(int i=1;i<n_2;i++){
		temp = a[i];
		a[i] = a[n-i];
		a[n-i] = temp;
	}
	return;
}
void FFT(const char *inputfile,const char *outputfile,double FS){
	// printf("\nIN FFT : sampling Freq : %.10lf",FS);
	FILE *fin = fopen(inputfile,"r");
	FILE *fout = fopen(outputfile,"w");
	int n;
	fscanf(fin,"%d",&n);
	Complex *a = (Complex *)malloc(sizeof(Complex)*n);
	Complex *A = (Complex *)malloc(sizeof(Complex)*n);
	double t;
	for(int i=0;i<n;i++){
		fscanf(fin,"%lf%lf",&t,&(a[i].real));
		a[i].imaginary = 0;
	}
	fclose(fin);
	fft(a,A,n);
	free(a);
	fprintf(fout, "%d\n",n );
	double FS_by_n = FS/n;
	for(int i=0;i<n;i++){
		fprintf(fout, "%.10lf %.10lf %.10lf\n",FS_by_n * i,A[i].real,A[i].imaginary );
	}
	fclose(fout);
	free(A);
	return;
}
void IFFT(const char *inputfile,const char *outputfile,double FS){
	FILE *fin = fopen(inputfile,"r");
	FILE *fout = fopen(outputfile,"w");
	// printf("\ninput : %s output : %s",inputfile,outputfile);
	int n;
	fscanf(fin,"%d",&n);
	Complex *a = (Complex *)malloc(sizeof(Complex)*n);
	Complex *A = (Complex *)malloc(sizeof(Complex)*n);
	double f;
	for(int i=0;i<n;i++){
		fscanf(fin,"%lf%lf%lf",&f,&(A[i].real),&(A[i].imaginary));
	}
	fclose(fin);
	ifft(A,a,n);
	free(A);
	fprintf(fout, "%d\n",n );
	double t = 0;
	double FS_INV = 1.0 / FS;
	for(int i=0;i<n;i++){
		fprintf(fout, "%.10lf %.10lf\n",t,a[i].real);
		// if(a[i].imaginary>1){
		// 	printf("i=%d a[i].imaginary = %.10lf",i,a[i].imaginary);
		// }
		t = t+FS_INV;
	}
	fclose(fout);
	free(a);
	return;
}
/***********************************************************/


void filter(const char *inputFile,const char *outputFile,double fLow,double fHigh){
	if(fLow>fHigh){
		double temp;
		temp = fLow;
		fLow = fHigh;
		fHigh = temp;
	}
	FILE *fin = fopen(inputFile,"r");
	FILE *fout = fopen(outputFile,"w");
	int sampleCount;
	fscanf(fin,"%d",&sampleCount);
	fprintf(fout, "%d\n",sampleCount );
	double freq,amp1,amp2;
	for(int i=0;i<sampleCount;i++){
		fscanf(fin,"%lf%lf%lf",&freq,&amp1,&amp2);
		if((freq>=fLow) && (freq<=fHigh)){
			fprintf(fout, "%.10lf %.10lf %.10lf\n",freq,amp1,amp2);
		}else{
			fprintf(fout, "%.10lf %.10lf %.10lf\n",freq,0.0,0.0);
		}
	}
	fclose(fin);
	fclose(fout);
	return;
}

void merge(const char *input1,const char *input2,const char *output){
	FILE *fin1,*fin2,*fout;
	fin1 = fopen(input1,"r");
	fin2 = fopen(input2,"r");
	fout = fopen(output,"w");

	int sampleCount1,sampleCount2;
	fscanf(fin1,"%d",&sampleCount1);
	fscanf(fin2,"%d",&sampleCount2);

	if(sampleCount1!=sampleCount2){
		printf("\nERROR : In merge samaple size is not same !!\n");
		exit(-1);
	}
	double freq1,amp1,amp11,freq2,amp2,amp22;
	fprintf(fout, "%d\n",sampleCount1 );
	for(int i=0;i<sampleCount1;i++){
		fscanf(fin1,"%lf%lf%lf",&freq1,&amp1,&amp11);
		fscanf(fin2,"%lf%lf%lf",&freq2,&amp2,&amp22);

		// if(fabs(freq1-freq2)>5){
		// 	printf("\nERROR : In merge different frequency for same sample !!\n");
		// 	printf("\ni = %d freq1 = %.10lf freq2 = %.10lf",i,freq1,freq2);
		// 	exit(-1);
		// }

		fprintf(fout, "%.10lf %.10lf %.10lf\n",(freq1+freq2)/2,amp1+amp2,amp11+amp22);
	}
	fclose(fin1);
	fclose(fin2);
	fclose(fout);

	return;
}

void separateBands(const char *input,const char *output1,const char *output2,double f1,double f2,double band1,double band2){
	double fLow1,fHigh1,fLow2,fHigh2;
	
	fLow1 = f1 - (band1)/2;
	fHigh1 = f1 + (band1)/2;
	fLow2 = f2 - (band2)/2;
	fHigh2 = f2 + (band2)/2;

	// void filter(const char *inputFile,const char *outputFile,double fLow,double fHigh)
	filter(input,output1,fLow1,fHigh1);
	filter(input,output2,fLow2,fHigh2);

	return;
}
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
void copy_file(const char *input,const char *output){
	int isEmpty;
	FILE *fin = initializeFileRead(input,&isEmpty);
	char line[1000];
	resetfile(output);
	while(1){
		readLine(fin,line,&isEmpty);
		if(line[0] == '\0' && isEmpty == 1){
			break;
		}
		addLine(output,line);
	}
	fclose(fin);
	return;
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
    // printf("\nT = %.10lf",t[sampleCount-1]);
    for(t_i = t[0];t_i<t[sampleCount-1];t_i = t_i + tDiff){
    	amp_i = gsl_spline_eval(spline,t_i,acc);
    	fprintf(f, "%.10lf %.10lf\n",t_i,amp_i);
    	sCount++;
    }
    fclose(f);

    f = fopen(interpolated,"w");
    g = fopen("temp.txt","r");
    
    fprintf(f, "%d\n",sCount);
    for(int i=0;i<sCount;i++){
    	fscanf(g,"%lf %lf",&t_i,&amp_i);
    	fprintf(f, "%.10lf %.10lf\n",t_i,amp_i);
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
        //printf("%.10lf %.10lf\n",value1,value2);
        error += (value1-value2)*(value1-value2);
    }
    fclose(orig);
    fclose(new_);
    error /= n;
    return sqrt(error);
}


/******************** AUXILARY FUNCTIONS **************************/
void removeHeader(const char *source,const char *dest){
	FILE *fIn = fopen(source,"r");
	FILE *fOut = fopen(dest,"w");
	int n;
	double t,x;

	fscanf(fIn,"%d",&n);
	for(int i=0;i<n;i++){
		fscanf(fIn,"%lf %lf",&t,&x);
		fprintf(fOut, "%.10lf %.10lf\n",t,x);
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
void generateFreq_Amp(int nComp,double *freq,double *amp,double fMin,double fMax,double A){
	for(int i=0;i<nComp;i++){
		freq[i] = fRand(fMin,fMax);
		amp[i] = fRand(0,A);
		// printf("\nFREQ[%d] = %.10lf AMP[%d] = %.10lf",i,freq[i],i,amp[i]);
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
		sprintf(line,"%.10lf %.10lf",T,getSignal(nComp,freq,amp,T));
		addLine(filename,line);
		T = T + tDiff;
	}
	return;
}
void createOriginalSignal(int nComp,double *freq,double *amp,const char *filename,double len){
	double tDiff;
	double fSample = FSUPER;
	tDiff = 1.0/fSample;
	int tCount = (int)(len * fSample);
	// printf("\nTcount = %d\tTdiff = %.10lf",tCount,tDiff);
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
		fprintf(fout, "%.10lf %.10lf\n",t,amp);
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
			fprintf(fOut,"%.10lf %.10lf\n",time1,value2);
			continue;
		}
		if(value1 < (-width * l)){
			value2 = -width * l;
			fprintf(fOut,"%.10lf %.10lf\n",time1,value2);
			continue;	
		}
		temp = value1 / width;
		if(temp >= 0){
			value2 = ((int)(temp + 0.5)) * width;
		}
		else{
			value2 = ((int)(temp - 0.5)) * width;
		}
		fprintf(fOut,"%.10lf %.10lf\n",time1,value2);
	}
	fclose(fIn);
	fclose(fOut);
	return;
}
/*******************************************************************/

/**************************************************/
void shift(const char* file1, const char* file2, double a, double b, double f, double width){
    if(fabs((b-a) - width)>.00001){
    	printf("\nWARNING DIFFERENT BANDWIDTH !!\n");
    }
    FILE *orig, *gen, *inter;
    int count, i;
    double val1, val2, newFreq, freq, f1, f2, newA, newB;
    orig = fopen(file1,"r");
    gen = fopen(file2,"w");
    inter  = fopen(INTERMEDIATE,"w");
    fscanf(orig,"%d",&count);
    f1 = f - width / 2;
    f2 = f + width / 2;
    newA = (f1 - f2) / (a - b);
    newB = (f2 * a - f1 * b) / (a - b);
    for(i = 0; i < count; i++){
        fscanf(orig,"%lf%lf%lf",&freq,&val1,&val2);
        if(freq >= a && freq <= b){
            newFreq = newA * freq + newB;
            fprintf(inter,"%.10lf %.10lf %.10lf\n",newFreq,val1,val2);
        }
    }
    fclose(orig);
    fclose(inter);
    orig = fopen(file1,"r");
    inter = fopen(INTERMEDIATE,"r");
    fscanf(orig,"%d",&count);
    fprintf(gen,"%d\n",count);
    for(i = 0; i < count; i++){
        fscanf(orig,"%lf%lf%lf",&freq,&val1,&val2);
        if(freq >= f1 && freq <= f2){
            fscanf(inter,"%lf%lf%lf",&newFreq,&val1,&val2);
            fprintf(gen,"%.10lf %.10lf %.10lf\n",newFreq,val1,val2);
        }
        else{
            fprintf(gen,"%.10lf %.10lf %.10lf\n",freq,0.0,0.0);   
        }
    }
    fclose(orig);
    fclose(inter);
    fclose(gen);
    return;
}
void reflect(const char* file1, const char* file2){
    FILE *orig, *gen;
    int i, count;
    double freq,val1,val2;

    orig = fopen(file1,"r");
    gen = fopen(file2,"w");
    fscanf(orig,"%d",&count);
    fprintf(gen,"%d\n",count);
    Complex *A = (Complex *)malloc(sizeof(Complex)*(count / 2 + 1));
    fscanf(orig,"%lf%lf%lf",&freq,&val1,&val2);
    fprintf(gen,"%.10lf %.10lf %.10lf\n",freq,val1,val2);
    for(i = 0; i < count; i++){
        fscanf(orig,"%lf%lf%lf",&freq,&val1,&val2);
        if(i == (count/2)-1){
            fprintf(gen,"%.10lf %.10lf %.10lf\n",freq,val1,val2);
            break;
        }
        else{
            fprintf(gen,"%.10lf %.10lf %.10lf\n",freq,val1,val2);
            A[i].real = val1;
            A[i].imaginary = val2;
        }
    }
    i--;
    while(i>=0){
        fscanf(orig,"%lf%lf%lf",&freq,&val1,&val2);
        fprintf(gen,"%.10lf %.10lf %.10lf\n",freq,A[i].real,-A[i].imaginary);
        i--;
    }
    free(A);
    fclose(orig);
    fclose(gen);
    return;
}

void inv_shift(const char* file1, const char* file2, double a, double b, double f, double width){
	shift(file1, file2,(f - width / 2),(f + width / 2), ((a + b) / 2), (b - a));
}
/**************************************************/
void createSignal(const char *filename,int n,double ampMax,double *amp,double a,double b,double *freq,double t){
	generateFreq_Amp(n,freq,amp,a,b,ampMax);

	// void createOriginalSignal(int nComp,double *freq,
	// double *amp,const char *filename,double len)

	createOriginalSignal(n,freq,amp,filename,t);
	return;
}


int ensureSameSize(const char *file1,const char *file2){
	FILE *f1,*f2,*temp1,*temp2;
	f1 = fopen(file1,"r");
	f2 = fopen(file2,"r");


	int n1,n2;

	fscanf(f1,"%d",&n1);
	fscanf(f2,"%d",&n2);

	if(n1==n2){
		fclose(f1);
		fclose(f2);
		return n1;
	}
	printf("\nWARNING : Different (%d,%d) sizes of Source Sample !! taking the minimum one for both",n1,n2);

	int n = (n1>n2)?n2:n1;
	double t1,x1,t2,x2;
	char tempfile1[1000],tempfile2[1000];
	sprintf(tempfile1,"temp_%s",file1);
	temp1 = fopen(tempfile1,"w");
	sprintf(tempfile2,"temp_%s",file2);
	temp2 = fopen(tempfile2,"w");
	fprintf(temp1, "%d\n",n );
	fprintf(temp2, "%d\n",n );
	for(int i = 0;i<n;i++){
		fscanf(f1,"%lf%lf",&t1,&x1);
		fscanf(f2,"%lf%lf",&t2,&x2);
		fprintf(temp1,"%.10lf %.10lf\n",t1,x1);
		fprintf(temp2,"%.10lf %.10lf\n",t2,x2);
	}
	fclose(f1);
	fclose(f2);
	fclose(temp1);
	fclose(temp2);

	copy_file(tempfile1,file1);
	copy_file(tempfile1,file2);

	return n;
}
int getNextPowerOfTwo(int n){
	int k = 1;
	while(1){
		if(k<0){
			printf("\nERROR : TOO LARGE SAMPLE SIZE !!\n");
			exit(0);
		}
		if(k>=n){
			return k;
		}
		k = k * 2;
	}
}
int padWithZero(const char *input,const char *output,double FS){
	FILE *fIn = fopen(input,"r");
	int n;
	fscanf(fIn,"%d",&n);
	int k = getNextPowerOfTwo(n);
	double FS_INV = 1.0/FS;
	double t,amp;
	FILE *fOut = fopen(output,"w");
	fprintf(fOut, "%d\n", k);
	for(int i=0;i<n;i++){
		fscanf(fIn,"%lf%lf",&t,&amp);
		fprintf(fOut, "%.10lf %.10lf\n",t,amp );
	}
	fclose(fIn);
	for(int i=n;i<k;i++){
		t = t+FS_INV;
		fprintf(fOut, "%.10lf %.10lf\n",t,0.0 );
	}
	fclose(fOut);
	return k;
}

/***************** plot functions **********************/
void plot1(){

	removeHeader(PADDED1_PART1,"source1_part1");
	removeHeader(PADDED2_PART1,"source2_part1");
	removeHeader(MIXEDSIGNAL_PART1,"Transmitted_Signal_part1");
	removeHeader(OUTPUT_SOURCE1_PART1,"output_source1_part1");
	removeHeader(OUTPUT_SOURCE2_PART1,"output_source2_part1");

	char call[2000];
	
	sprintf(call,"gnuplot -persist -e \"plot \\\"%s\\\" with lines,\\\"%s\\\" with lines\"",
		"source1_part1","output_source1_part1");
	system(call);
	sprintf(call,"gnuplot -persist -e \"plot \\\"%s\\\" with lines,\\\"%s\\\" with lines\"",
		"source2_part1","output_source2_part1");
	system(call);
	sprintf(call,"gnuplot -persist -e \"plot \\\"%s\\\" with lines\"",
		"Transmitted_Signal_part1");
	system(call);

	return;
}
void plot2(){
	removeHeader(PADDED1_PART2,"source1_part2");
	removeHeader(PADDED2_PART2,"source2_part2");
	removeHeader(MIXEDSIGNAL_PART2,"Transmitted_Signal_part2");
	removeHeader(OUTPUT_SOURCE1_PART2,"output_source1_part2");
	removeHeader(OUTPUT_SOURCE2_PART2,"output_source2_part2");
	removeHeader(TRANSMITTED_PART2,"received_part2");

	char call[2000];
	
	sprintf(call,"gnuplot -persist -e \"plot \\\"%s\\\" with lines,\\\"%s\\\" with lines\"",
		"source1_part2","output_source1_part2");
	system(call);
	sprintf(call,"gnuplot -persist -e \"plot \\\"%s\\\" with lines,\\\"%s\\\" with lines\"",
		"source2_part2","output_source2_part2");
	system(call);
	sprintf(call,"gnuplot -persist -e \"plot \\\"%s\\\" with lines,\\\"%s\\\" with lines\"",
	"Transmitted_Signal_part2","received_part2");
	system(call);

	return;
}
void plot3(){
	removeHeader(PADDED1_PART3,"source1_part3");
	removeHeader(PADDED2_PART3,"source2_part3");
	removeHeader(MIXEDSIGNAL_PART3,"mixed_signal_part3");
	removeHeader(OUTPUT_SOURCE1_PART3,"output_source1_part3");
	removeHeader(OUTPUT_SOURCE2_PART3,"output_source2_part3");
	removeHeader(TRANSMITTED_PART3,"received_part3");
	removeHeader(REQUANTIZED_PART3,"reconstructed_mixed_signal_part3");
	removeHeader(QUANTIZED_PART3,"quantized_part3");
	// removeHeader(REQUANTIZED_PART3,"requantized_part3");


	char call[2000];
	
	sprintf(call,"gnuplot -persist -e \"plot \\\"%s\\\" with lines,\\\"%s\\\" with lines\"",
		"source1_part3","output_source1_part3");
	system(call);
	sprintf(call,"gnuplot -persist -e \"plot \\\"%s\\\" with lines,\\\"%s\\\" with lines\"",
		"source2_part3","output_source2_part3");
	system(call);
	sprintf(call,"gnuplot -persist -e \"plot \\\"%s\\\" with lines,\\\"%s\\\" with lines\"",
		"mixed_signal_part3","reconstructed_mixed_signal_part3");
	system(call);
	// sprintf(call,"gnuplot -persist -e \"plot \\\"%s\\\" with lines,\\\"%s\\\" with lines,\\\"%s\\\" with lines\"",
	// "quantized_part3","received_part3","requantized_part3");
	// system(call);
	return;
}

/*******************************************************/
int part3(){

	int temp;
	printf("\nEnter 1 to Execute Part 3 :");
	scanf("%d",&temp);
	if(temp != 1){
		return 0;
	}

	double a1,b1,a2,b2,f1,f2,ampMax1,ampMax2,t,fSample,band;
	int n1,n2,n;

	printf("***************************************\n");
	printf("\nFOR SOURCE 1(U1) ********************\n");

	printf("Enter f1 : ");
	scanf("%lf",&f1);
	printf("Enter ampMax1 : ");
	scanf("%lf",&ampMax1);
	printf("Enter n1 : ");
	scanf("%d",&n1);

	printf("\nFOR SOURCE 2(U2) ********************\n");

	printf("Enter f2 : ");
	scanf("%lf",&f2);
	printf("Enter ampMax2 : ");
	scanf("%lf",&ampMax2);
	printf("Enter n2 : ");
	scanf("%d",&n2);
	printf("***************************************\n");
	printf("Enter fMin : ");
	scanf("%lf",&a2);
	printf("Enter fMax : ");
	scanf("%lf",&b2);

	printf("***************************************\n");
	printf("\nEnter Signal Duration : ");
	scanf("%lf",&t);
	printf("\nEnter Sampling Frequency : ");
	scanf("%lf",&fSample);
	a1 = a2;
	b1 = b2;

	band = b2 - a2;
	printf("***************************************\n");
	printf("***************************************\n");
	printf("*****************OUTPUT****************\n");
	printf("***************************************\n");

	double amp1[n1],amp2[n2],freq1[n1],freq2[n2];

	createSignal(ORIGINAL1_PART3,n1,ampMax1,amp1,a1,b1,freq1,t);
	createSignal(ORIGINAL2_PART3,n2,ampMax2,amp2,a2,b2,freq2,t);

	printf("\nSIGNAL 1 : ");
	for(int i =0;i<n1;i++){
		printf("\nFREQ = %.10lf AMP = %.10lf",freq1[i],amp1[i]);
	}
	printf("\nSIGNAL 2 : ");
	for(int i =0;i<n1;i++){
		printf("\nFREQ = %.10lf AMP = %.10lf",freq2[i],amp2[i]);
	}


	sample(n1,freq1,amp1,fSample,SAMPLED1_PART3,t);
	sample(n2,freq2,amp2,fSample,SAMPLED2_PART3,t);
	
	n = ensureSameSize(SAMPLED1_PART3,SAMPLED2_PART3);

	padWithZero(SAMPLED1_PART3,PADDED1_PART3,fSample);
	padWithZero(SAMPLED2_PART3,PADDED2_PART3,fSample);

	FFT(PADDED1_PART3,SPECTRUM1_PART3,fSample);
	FFT(PADDED2_PART3,SPECTRUM2_PART3,fSample);

	// IFFT(SPECTRUM1_PART3,"signal1.txt",fSample);
	// IFFT(SPECTRUM2_PART3,"signal2.txt",fSample);

	// // shift(const char* file1, const char* file2, double a, 
	// // double b, double f, double width)

	shift(SPECTRUM1_PART3,SHIFTEDSPECTRUM1_PART3,a1,b1,f1,band);
	shift(SPECTRUM2_PART3,SHIFTEDSPECTRUM2_PART3,a2,b2,f2,band);

	merge(SHIFTEDSPECTRUM1_PART3,SHIFTEDSPECTRUM2_PART3,MIXEDSPECTRUM_PART3);

	reflect(MIXEDSPECTRUM_PART3,REFLECTED_MIXEDSPECTRUM_PART3);

	IFFT(REFLECTED_MIXEDSPECTRUM_PART3,MIXEDSIGNAL_PART3,fSample);

	// /***********************************/
	// /******* TRANSMISSION **************/
	double error = (n1 * ampMax1 + n2 * ampMax2)/(double)100;
	int l1 = (int)((ampMax1 * n1) /error);
	int l2 = (int)((ampMax2 * n2) /error);
	int l = (l1 + l2)/2;
	// printf("\nEnter Number of levels each side : ");
	// scanf("%d",&l);
	printf("\nOptimised Level Set = %d\n",l);
	quantize(MIXEDSIGNAL_PART3,QUANTIZED_PART3,l,error);

	transmit(QUANTIZED_PART3,TRANSMITTED_PART3,error/2);

	quantize(TRANSMITTED_PART3,REQUANTIZED_PART3,l,error);

	// interpolate(REQUANTIZED_PART3,INTERPOLATED_PART3,fSample);
	// // /***********************************/

	FFT(REQUANTIZED_PART3,MIXEDSPECTRUM_OUTPUT_PART3,fSample);
	separateBands(MIXEDSPECTRUM_OUTPUT_PART3,SPECTRUM_OUTPUT1_PART3,SPECTRUM_OUTPUT2_PART3,f1,f2,band,band);
	
	inv_shift(SPECTRUM_OUTPUT1_PART3,SPECTRUM_OUTPUT1_SHIFTED_PART3,a1,b1,f1,band);
	inv_shift(SPECTRUM_OUTPUT2_PART3,SPECTRUM_OUTPUT2_SHIFTED_PART3,a2,b2,f2,band);

	reflect(SPECTRUM_OUTPUT1_SHIFTED_PART3,SPECTRUM_OUTPUT1_REFLECTED_PART3);
	reflect(SPECTRUM_OUTPUT2_SHIFTED_PART3,SPECTRUM_OUTPUT2_REFLECTED_PART3);

	IFFT(SPECTRUM_OUTPUT1_REFLECTED_PART3,OUTPUT_SOURCE1_PART3,fSample);
	IFFT(SPECTRUM_OUTPUT2_REFLECTED_PART3,OUTPUT_SOURCE2_PART3,fSample);

	// printf("\nERROR : ");
	double error1 = calc_mean_square_error(SAMPLED1_PART3,OUTPUT_SOURCE1_PART3);
	double error2 = calc_mean_square_error(SAMPLED2_PART3,OUTPUT_SOURCE2_PART3);
	printf("\nMean Square error in Source 1 Signal = %.10lf in Source 2 Signal = %.10lf",error1,error2);
	
	plot3();

	/*******************************************************************************/
	printf("\nEnter 1 for EXPERIMENTATION ON QUANTIZATION LEVELS : ");
	scanf("%d",&temp);
	if(temp != 1){
		return 0;
	}
	int lMin,lMax;
	printf("\nEnter Min Quantization Level : ");
	scanf("%d",&lMin);
	printf("\nEnter Max Quantization Level : ");
	scanf("%d",&lMax);
	printf("\nEnter Number of trials : ");
	int trial;
	scanf("%d",&trial);
	int lDiff = (lMax - lMin) / trial;
	if(lDiff == 0){
		lDiff = 1;
	}
	FILE *Error1 = fopen("errorL_sig1.txt","w");
	FILE *Error2 = fopen("errorL_sig2.txt","w");
	for(int L = lMin;L<=lMax;L = L + lDiff){
		quantize(MIXEDSIGNAL_PART3,QUANTIZED_PART3,L,error);

		transmit(QUANTIZED_PART3,TRANSMITTED_PART3,error/2);

		quantize(TRANSMITTED_PART3,REQUANTIZED_PART3,L,error);

		// interpolate(REQUANTIZED_PART3,INTERPOLATED_PART3,fSample);
		// // /***********************************/

		FFT(REQUANTIZED_PART3,MIXEDSPECTRUM_OUTPUT_PART3,fSample);
		separateBands(MIXEDSPECTRUM_OUTPUT_PART3,SPECTRUM_OUTPUT1_PART3,SPECTRUM_OUTPUT2_PART3,f1,f2,band,band);
		
		inv_shift(SPECTRUM_OUTPUT1_PART3,SPECTRUM_OUTPUT1_SHIFTED_PART3,a1,b1,f1,band);
		inv_shift(SPECTRUM_OUTPUT2_PART3,SPECTRUM_OUTPUT2_SHIFTED_PART3,a2,b2,f2,band);

		reflect(SPECTRUM_OUTPUT1_SHIFTED_PART3,SPECTRUM_OUTPUT1_REFLECTED_PART3);
		reflect(SPECTRUM_OUTPUT2_SHIFTED_PART3,SPECTRUM_OUTPUT2_REFLECTED_PART3);

		IFFT(SPECTRUM_OUTPUT1_REFLECTED_PART3,OUTPUT_SOURCE1_PART3,fSample);
		IFFT(SPECTRUM_OUTPUT2_REFLECTED_PART3,OUTPUT_SOURCE2_PART3,fSample);

		// printf("\nERROR : ");
		double error1 = calc_mean_square_error(SAMPLED1_PART3,OUTPUT_SOURCE1_PART3);
		double error2 = calc_mean_square_error(SAMPLED2_PART3,OUTPUT_SOURCE2_PART3);
		
		fprintf(Error1, "%d %lf\n",L,error1);
		fprintf(Error2, "%d %lf\n",L,error2);

		printf("\nL = %d Error 1 = %lf\tError 2 = %lf",L,error1,error2);
	}
	fclose(Error1);
	fclose(Error2);
	char call[1000];
	sprintf(call,"gnuplot -persist -e \"plot \\\"%s\\\" with lines,\\\"%s\\\" with lines\"",
		"errorL_sig1.txt","errorL_sig2.txt");
	system(call);
	return 0;
}
int part2(){     // part2 working
	int temp;
	printf("\nEnter 1 to Execute Part 2 :");
	scanf("%d",&temp);
	if(temp != 1){
		return 0;
	}
	double a1,b1,a2,b2,f1,f2,ampMax1,ampMax2,t,fSample,band;
	int n1,n2,n;

	printf("***************************************\n");
	printf("\nFOR SOURCE 1(U1) ********************\n");

	printf("Enter f1 : ");
	scanf("%lf",&f1);
	printf("Enter ampMax1 : ");
	scanf("%lf",&ampMax1);
	printf("Enter n1 : ");
	scanf("%d",&n1);

	printf("\nFOR SOURCE 2(U2) ********************\n");

	printf("Enter f2 : ");
	scanf("%lf",&f2);
	printf("Enter ampMax2 : ");
	scanf("%lf",&ampMax2);
	printf("Enter n2 : ");
	scanf("%d",&n2);
	printf("***************************************\n");
	printf("Enter fMin : ");
	scanf("%lf",&a2);
	printf("Enter fMax : ");
	scanf("%lf",&b2);

	printf("***************************************\n");
	printf("\nEnter Signal Duration : ");
	scanf("%lf",&t);
	printf("\nEnter Sampling Frequency : ");
	scanf("%lf",&fSample);
	a1 = a2;
	b1 = b2;

	band = b2 - a2;

	printf("***************************************\n");
	printf("***************************************\n");
	printf("*****************OUTPUT****************\n");
	printf("***************************************\n");

	double amp1[n1],amp2[n2],freq1[n1],freq2[n2];

	createSignal(ORIGINAL1_PART2,n1,ampMax1,amp1,a1,b1,freq1,t);
	createSignal(ORIGINAL2_PART2,n2,ampMax2,amp2,a2,b2,freq2,t);

	printf("\nSIGNAL 1 : ");
	for(int i =0;i<n1;i++){
		printf("\nFREQ = %.10lf AMP = %.10lf",freq1[i],amp1[i]);
	}
	printf("\nSIGNAL 2 : ");
	for(int i =0;i<n1;i++){
		printf("\nFREQ = %.10lf AMP = %.10lf",freq2[i],amp2[i]);
	}


	sample(n1,freq1,amp1,fSample,SAMPLED1_PART2,t);
	sample(n2,freq2,amp2,fSample,SAMPLED2_PART2,t);
	
	n = ensureSameSize(SAMPLED1_PART2,SAMPLED2_PART2);

	padWithZero(SAMPLED1_PART2,PADDED1_PART2,fSample);
	padWithZero(SAMPLED2_PART2,PADDED2_PART2,fSample);

	FFT(PADDED1_PART2,SPECTRUM1_PART2,fSample);
	FFT(PADDED2_PART2,SPECTRUM2_PART2,fSample);



	shift(SPECTRUM1_PART2,SHIFTEDSPECTRUM1_PART2,a1,b1,f1,band);
	shift(SPECTRUM2_PART2,SHIFTEDSPECTRUM2_PART2,a2,b2,f2,band);

	merge(SHIFTEDSPECTRUM1_PART2,SHIFTEDSPECTRUM2_PART2,MIXEDSPECTRUM_PART2);

	reflect(MIXEDSPECTRUM_PART2,REFLECTED_MIXEDSPECTRUM_PART2);

	IFFT(REFLECTED_MIXEDSPECTRUM_PART2,MIXEDSIGNAL_PART2,fSample);

	// /***********************************/
	// /******* TRANSMISSION **************/
	double error = (n1 * ampMax1 + n2 * ampMax2)/10;
	transmit(MIXEDSIGNAL_PART2,TRANSMITTED_PART2,error);
	// /***********************************/

	FFT(TRANSMITTED_PART2,MIXEDSPECTRUM_OUTPUT_PART2,fSample);
	separateBands(MIXEDSPECTRUM_OUTPUT_PART2,SPECTRUM_OUTPUT1_PART2,SPECTRUM_OUTPUT2_PART2,f1,f2,band,band);
	
	inv_shift(SPECTRUM_OUTPUT1_PART2,SPECTRUM_OUTPUT1_SHIFTED_PART2,a1,b1,f1,band);
	inv_shift(SPECTRUM_OUTPUT2_PART2,SPECTRUM_OUTPUT2_SHIFTED_PART2,a2,b2,f2,band);

	reflect(SPECTRUM_OUTPUT1_SHIFTED_PART2,SPECTRUM_OUTPUT1_REFLECTED_PART2);
	reflect(SPECTRUM_OUTPUT2_SHIFTED_PART2,SPECTRUM_OUTPUT2_REFLECTED_PART2);

	IFFT(SPECTRUM_OUTPUT1_REFLECTED_PART2,OUTPUT_SOURCE1_PART2,fSample);
	IFFT(SPECTRUM_OUTPUT2_REFLECTED_PART2,OUTPUT_SOURCE2_PART2,fSample);
	// printf("\nERROR : ");
	double error1 = calc_mean_square_error(SAMPLED1_PART2,OUTPUT_SOURCE1_PART2);
	double error2 = calc_mean_square_error(SAMPLED2_PART2,OUTPUT_SOURCE2_PART2);
	printf("\nMean Square error in Source 1 Signal = %.10lf in Source 2 Signal = %.10lf",error1,error2);

	plot2();

	return 0;
}
int part1(){ // working part 1
	int temp;
	printf("\nEnter 1 to Execute Part 1 :");
	scanf("%d",&temp);
	if(temp != 1){
		return 0;
	}
	double a1,b1,a2,b2,f1,f2,ampMax1,ampMax2,t,fSample,band;
	int n1,n2,n;

	printf("***************************************\n");
	printf("\nFOR SOURCE 1(U1) ********************\n");

	printf("Enter f1 : ");
	scanf("%lf",&f1);
	printf("Enter ampMax1 : ");
	scanf("%lf",&ampMax1);
	printf("Enter n1 : ");
	scanf("%d",&n1);

	printf("\nFOR SOURCE 2(U2) ********************\n");

	printf("Enter f2 : ");
	scanf("%lf",&f2);
	printf("Enter ampMax2 : ");
	scanf("%lf",&ampMax2);
	printf("Enter n2 : ");
	scanf("%d",&n2);
	printf("***************************************\n");
	printf("Enter fMin : ");
	scanf("%lf",&a2);
	printf("Enter fMax : ");
	scanf("%lf",&b2);

	printf("***************************************\n");
	printf("\nEnter Signal Duration : ");
	scanf("%lf",&t);
	printf("\nEnter Sampling Frequency : ");
	scanf("%lf",&fSample);
	a1 = a2;
	b1 = b2;

	band = b2 - a2;

	printf("***************************************\n");
	printf("***************************************\n");
	printf("*****************OUTPUT****************\n");
	printf("***************************************\n");

	double amp1[n1],amp2[n2],freq1[n1],freq2[n2];

	createSignal(ORIGINAL1_PART1,n1,ampMax1,amp1,a1,b1,freq1,t);
	createSignal(ORIGINAL2_PART1,n2,ampMax2,amp2,a2,b2,freq2,t);

	printf("\nSIGNAL 1 : ");
	for(int i =0;i<n1;i++){
		printf("\nFREQ = %.10lf AMP = %.10lf",freq1[i],amp1[i]);
	}
	printf("\nSIGNAL 2 : ");
	for(int i =0;i<n1;i++){
		printf("\nFREQ = %.10lf AMP = %.10lf",freq2[i],amp2[i]);
	}


	sample(n1,freq1,amp1,fSample,SAMPLED1_PART1,t);
	sample(n2,freq2,amp2,fSample,SAMPLED2_PART1,t);
	
	n = ensureSameSize(SAMPLED1_PART1,SAMPLED2_PART1);

	padWithZero(SAMPLED1_PART1,PADDED1_PART1,fSample);
	padWithZero(SAMPLED2_PART1,PADDED2_PART1,fSample);

	FFT(PADDED1_PART1,SPECTRUM1_PART1,fSample);
	FFT(PADDED2_PART1,SPECTRUM2_PART1,fSample);


	shift(SPECTRUM1_PART1,SHIFTEDSPECTRUM1_PART1,a1,b1,f1,band);
	shift(SPECTRUM2_PART1,SHIFTEDSPECTRUM2_PART1,a2,b2,f2,band);

	merge(SHIFTEDSPECTRUM1_PART1,SHIFTEDSPECTRUM2_PART1,MIXEDSPECTRUM_PART1);

	reflect(MIXEDSPECTRUM_PART1,REFLECTED_MIXEDSPECTRUM_PART1);

	IFFT(REFLECTED_MIXEDSPECTRUM_PART1,MIXEDSIGNAL_PART1,fSample);

	// /***********************************/
	// /******* TRANSMISSION **************/
	// /***********************************/

	FFT(MIXEDSIGNAL_PART1,MIXEDSPECTRUM_OUTPUT_PART1,fSample);
	separateBands(MIXEDSPECTRUM_OUTPUT_PART1,SPECTRUM_OUTPUT1_PART1,SPECTRUM_OUTPUT2_PART1,f1,f2,band,band);
	
	inv_shift(SPECTRUM_OUTPUT1_PART1,SPECTRUM_OUTPUT1_SHIFTED_PART1,a1,b1,f1,band);
	inv_shift(SPECTRUM_OUTPUT2_PART1,SPECTRUM_OUTPUT2_SHIFTED_PART1,a2,b2,f2,band);

	reflect(SPECTRUM_OUTPUT1_SHIFTED_PART1,SPECTRUM_OUTPUT1_REFLECTED_PART1);
	reflect(SPECTRUM_OUTPUT2_SHIFTED_PART1,SPECTRUM_OUTPUT2_REFLECTED_PART1);

	IFFT(SPECTRUM_OUTPUT1_REFLECTED_PART1,OUTPUT_SOURCE1_PART1,fSample);
	IFFT(SPECTRUM_OUTPUT2_REFLECTED_PART1,OUTPUT_SOURCE2_PART1,fSample);

	// printf("\nERROR : ");
	double error1 = calc_mean_square_error(SAMPLED1_PART1,OUTPUT_SOURCE1_PART1);
	double error2 = calc_mean_square_error(SAMPLED2_PART1,OUTPUT_SOURCE2_PART1);
	printf("\nMean Square error in Source 1 Signal = %.10lf in Source 2 Signal = %.10lf",error1,error2);
	plot1();
	/*******************************************************************/
	printf("\nEnter 1 to do EXPERIMENTATION ON F1-F2 Separation : ");
	scanf("%d",&temp);
	if(temp != 1){
		return 0;
	}
	double fdiff = (f2 - f1);
	printf("\nEnter number of Separations : ");
	int trials;
	scanf("%d",&trials);
	double fincr = fdiff / trials;
	FILE *Error1 = fopen("error1.txt","w");
	FILE *Error2 = fopen("error2.txt","w");
	
	for(double f_second = f1;f_second <= f2; f_second = f_second + fincr){
		shift(SPECTRUM1_PART1,SHIFTEDSPECTRUM1_PART1,a1,b1,f1,band);
		shift(SPECTRUM2_PART1,SHIFTEDSPECTRUM2_PART1,a2,b2,f_second,band);

		merge(SHIFTEDSPECTRUM1_PART1,SHIFTEDSPECTRUM2_PART1,MIXEDSPECTRUM_PART1);

		reflect(MIXEDSPECTRUM_PART1,REFLECTED_MIXEDSPECTRUM_PART1);

		IFFT(REFLECTED_MIXEDSPECTRUM_PART1,MIXEDSIGNAL_PART1,fSample);

		// /***********************************/
		// /******* TRANSMISSION **************/
		// /***********************************/

		FFT(MIXEDSIGNAL_PART1,MIXEDSPECTRUM_OUTPUT_PART1,fSample);
		separateBands(MIXEDSPECTRUM_OUTPUT_PART1,SPECTRUM_OUTPUT1_PART1,SPECTRUM_OUTPUT2_PART1,f1,f_second,band,band);
		
		inv_shift(SPECTRUM_OUTPUT1_PART1,SPECTRUM_OUTPUT1_SHIFTED_PART1,a1,b1,f1,band);
		inv_shift(SPECTRUM_OUTPUT2_PART1,SPECTRUM_OUTPUT2_SHIFTED_PART1,a2,b2,f_second,band);

		reflect(SPECTRUM_OUTPUT1_SHIFTED_PART1,SPECTRUM_OUTPUT1_REFLECTED_PART1);
		reflect(SPECTRUM_OUTPUT2_SHIFTED_PART1,SPECTRUM_OUTPUT2_REFLECTED_PART1);

		IFFT(SPECTRUM_OUTPUT1_REFLECTED_PART1,OUTPUT_SOURCE1_PART1,fSample);
		IFFT(SPECTRUM_OUTPUT2_REFLECTED_PART1,OUTPUT_SOURCE2_PART1,fSample);

		// printf("\nERROR : ");
		error1 = calc_mean_square_error(SAMPLED1_PART1,OUTPUT_SOURCE1_PART1);
		error2 = calc_mean_square_error(SAMPLED2_PART1,OUTPUT_SOURCE2_PART1);

		printf("\nError1 = %lf\tError2 = %lf",error1,error2);

		fprintf(Error1,"%lf %lf\n",(f_second-f1),error1);
		fprintf(Error2,"%lf %lf\n",(f_second-f1),error2);
	}
	fclose(Error1);
	fclose(Error2);
	char call[1000];
	sprintf(call,"gnuplot -persist -e \"plot \\\"%s\\\" with lines,\\\"%s\\\" with lines\"",
		"error1.txt","error2.txt");
	system(call);

	return 0;
}



int main(){
	printf("***************************************\n");
	printf("***************************************\n");
	printf("***************************************\n");
	printf("******************PART1****************\n");
	printf("***************************************\n");
	part1();
	printf("\n");
	printf("***************************************\n");
	printf("***************************************\n");
	printf("***************************************\n");
	printf("******************PART2****************\n");
	printf("***************************************\n");
	part2();
	printf("\n");
	printf("***************************************\n");
	printf("***************************************\n");
	printf("***************************************\n");
	printf("******************PART3****************\n");
	printf("***************************************\n");
	part3();
	printf("\n");
	printf("\n");
	return 0;
}