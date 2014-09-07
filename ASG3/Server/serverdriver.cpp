#include <stdio.h>
#include <stdlib.h>

int main(){
	// launch server side command interpreter
	system("./resetserver");
	int serverpid = launch("./server");
	int updateserverpid = launch("./updateserver");

}