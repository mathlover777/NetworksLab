#ifndef RECON
#define RECON

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

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
int main(){
	interpolate("requantized.txt","interpolated.txt",5000);
    double error = calc_mean_square_error("original.txt","interpolated.txt");
	printf("\nERROR = %lf",error);
    printf("\n");
    return 0;
}
#endif