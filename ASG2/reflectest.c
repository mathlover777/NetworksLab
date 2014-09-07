#include <stdio.h>
#include <stdlib.h>

struct complex{
    double real;
    double imaginary;
};

typedef struct complex Complex;

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
    fprintf(gen,"%lf %lf %lf\n",freq,val1,val2);
    for(i = 0; i < count; i++){
        fscanf(orig,"%lf%lf%lf",&freq,&val1,&val2);
        if(i == (count/2)-1){
            fprintf(gen,"%lf %lf %lf\n",freq,val1,val2);
            break;
        }
        else{
            fprintf(gen,"%lf %lf %lf\n",freq,val1,val2);
            A[i].real = val1;
            A[i].imaginary = val2;
        }
    }
    i--;
    while(i>=0){
        fscanf(orig,"%lf%lf%lf",&freq,&val1,&val2);
        fprintf(gen,"%lf %lf %lf\n",freq,A[i].real,-A[i].imaginary);
        i--;
    }
    free(A);
}

int main(){
    reflect("inputref.txt","outputref.txt");
    return 0;
}