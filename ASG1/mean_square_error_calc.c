#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double calc_mean_square_error(const char* f1,const char* f2){
	FILE *orig,*new;
	int count,n;
	double time1,time2,value1,value2,error;
	orig = fopen(f1,"r");
	new = fopen(f2,"r");
	fscanf(orig,"%d",&count);
	fscanf(new,"%d",&n);
	if(count < n){
		n = count;
	}
	else{
		count = n;
	}
	//printf("%d %d\n",count,n);
	while(count--){
		fscanf(orig,"%lf%lf",&time1,&value1);
		fscanf(new,"%lf%lf",&time2,&value2);
		//printf("%lf %lf\n",value1,value2);
		error += (value1-value2)*(value1-value2);
	}
	fclose(orig);
	fclose(new);
	error /= n;
	return error;
}

/*
int main(){
	printf("%lf",calc_mean_square_error("old.txt", "new.txt"));
}
*/

