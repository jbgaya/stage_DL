#include <cmath>
#include "mshadow/tensor.h"
#include <iostream>

using namespace mshadow;
using namespace mshadow::expr;
using namespace std;

//_______________________________________________DEFINITION D'OPÉRATEURS

struct tanh_ {
	template<typename type>
	MSHADOW_XINLINE static type Map(type x) {
		return (exp(2*x)-1)/(exp(2*x)+1);
	}
};

struct ReLu_ {
	template<typename type>
	MSHADOW_XINLINE static type Map(type x){
		return (x>0.)?x:0.;
	}
};

int main(void){
	int n = 10;
	InitTensorEngine<gpu>();
	
	//Initialisation du vecteur
	Stream<gpu> * stream_ = NewStream<gpu>(0);
	Tensor<gpu,1, double> Vec = NewTensor<gpu, double>(Shape1(n), 1., stream_);
	
	//Mapping de la fonction
	cout << "Vec avant mapping :" << endl;
	for(index_t i = 0; i < Vec.size(0); i++) cout << Vec[i] << "\t";	
	Vec = F<tanh_>(F<ReLu_>(Vec)+1);
	cout << "\n\nVec après mapping (x -> tanh( Relu(x) + 1 ) :" << endl;
	for(index_t i = 0; i < Vec.size(0); i++) cout << Vec[i] << "\t";
	
	//Nettoyage du device
	
	DeleteStream(stream_);
	ShutdownTensorEngine<gpu>();	
	cout << "\n";
}
