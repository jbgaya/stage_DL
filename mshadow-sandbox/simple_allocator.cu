#include "mshadow/tensor.h"
#include <iostream>
#include <random>

using namespace mshadow;
using namespace mshadow::expr;
using namespace std;

int main(void){
	
	//génération d'un tableau de doubles à allouer
	int n = 1000;
	double * data = new double[n];
	for(int i=0;i<n;i++) data[i] = 0.;
	
	
	//initialisation (obligatoire pour utiliser CuBLAS)
	InitTensorEngine<gpu>();
	
	Tensor<gpu,2, double> T1,T2;
	
	//allocation du tableau de double
	cout << "Allocation d'un tableau de " << n << " valeurs sur dans deux tenseurs différents :" << endl;
	T1.dptr_ = data;
	T2.dptr_ = data;
	
	//test sur les dimensions
	cout << "size  :" << T1.size(0) << endl;
	cout << "shape : " << T1.shape_ << endl;
	cout << "\nTenseur redimensionné au maximum en utilisant la méthode .Slice() :" << endl;
	T1 = T1.Slice(0,n);
	T2 = T2.Slice(0,n);
	cout << "size  : " << T1.size(0) << endl;
	cout << "shape : " << T1.shape_ << endl;
	
	index_t i;
	T1[i] = 5;
	cout << T1[i] << endl;
	
	//Arrêt (obligatoire pour utiliser CuBLAS)
	ShutdownTensorEngine<gpu>();
}
