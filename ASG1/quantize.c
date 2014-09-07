#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void quantize(const char* sampled,const char *quantized,int l,double width){
	FILE *fIn = fopen(sampled,"r");
	FILE  *fOut = fopen(quantized,"w");
	int i, count;
	double time1,value1,value2,temp;
	fscanf(fIn,"%d",&count);
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

/*
int main(){
	const char* w;
	w = quantize("sampled.txt",5,30);
}
*/