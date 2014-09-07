#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(){
	double sum = 0;
	for(int i=1;i<=9999;i++){
		sum = sum +((1.0/2.0) * (sqrt(i)));
	}
	printf("\nANSWER = %lf \n",sum);
	return 0;
}