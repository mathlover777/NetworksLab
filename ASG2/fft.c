#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI (4*atan(1))

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
void FFT(Complex *a,Complex *A,int n){
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
	FFT(a_even,A_even,n_2);
	FFT(a_odd,A_odd,n_2);
	// printf("\n n = %d",n);
	// printf("\nEVEN {");
	// for(int i=0;i<n_2;i++){
	// 	printf(" (%lf,%lf) ",A_even[i].real,A_even[i].imaginary);
	// }
	// printf("}\n");
	// printf("\nODD {");
	// for(int i=0;i<n_2;i++){
	// 	printf(" (%lf,%lf) ",A_odd[i].real,A_odd[i].imaginary);
	// }
	// printf("}\n");
	Complex temp;
	for(int k=0;k<n_2;k++){
		// printf("\nw_%d = (%lf,%lf),A_odd[%d]=(%lf,%lf)",k,w.real,w.imaginary,k,A_odd[k].real,A_odd[k].imaginary);
		temp = mult(w,A_odd[k]);
		// printf("\ntemp = (%lf,%lf)",temp.real,temp.imaginary);
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
void IFFT(Complex *A,Complex *a,int n){
	FFT(A,a,n);
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
double distance(Complex a,Complex b){
	return sqrt((a.real-b.real)*(a.real-b.real) + (a.imaginary-b.imaginary)*(a.imaginary-b.imaginary));
}

int main(){
	int n = 1048576;
	Complex *a,*A,*b;
	a= (Complex *)malloc(sizeof(Complex)*n);
	A= (Complex *)malloc(sizeof(Complex)*n);
	b= (Complex *)malloc(sizeof(Complex)*n);
	Complex w;
	// w = getWn(3);
	// printf("(%lf,%lf)\n",w.real,w.imaginary);
	for(int i=0;i<n;i++){
		a[i].real=(double)(rand()%10);
		// a[i].real = 0;
		a[i].imaginary = 0.0;
		// A[i].real = 0;
		// A[i].imaginary = 0;
	}
	// printf("\n{");
	// for(int i=0;i<n;i++){
	// 	printf(" (%lf,%lf) ",a[i].real,a[i].imaginary);
	// }
	// printf("}\n");

	FFT(a,A,n);

	// printf("\n{");
	// for(int i=0;i<n;i++){
	// 	printf(" (%lf,%lf) ",A[i].real,A[i].imaginary);
	// }
	// printf("}\n");

	IFFT(A,b,n);
	// printf("\n{");
	// for(int i=0;i<n;i++){
	// 	printf(" (%lf,%lf) ",b[i].real,b[i].imaginary);
	// }
	// printf("}\n");
	for(int i=0;i<n;i++){
		if(distance(a[i],b[i])>.00001){
			printf("\ni = %d a= (%lf,%lf) b= (%lf,%lf)",i,a[i].real,a[i].imaginary,b[i].real,b[i].imaginary);
		}
	}
	return 0;
}