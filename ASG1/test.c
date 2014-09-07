#include <stdio.h>
#include <stdlib.h>

int main(){
	char call[1000];
	sprintf(call,"gnuplot -persist -e \"plot \\\"errordata.txt\\\"");
	printf("{%s}\n",call);
	return 0;
}