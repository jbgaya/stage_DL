#include <iostream>
#include <vector>
#include <random>
#include <chrono>
using namespace std;
class Vector;
struct LazyExp;



//Classe vecteur___________________________________________PAS IMPORTANT
class Vector{
	public:
		int size;
		double * data;
		
		Vector(int n=0);
		~Vector(){delete [] data;}
		Vector(const Vector &);
		
		double operator[](int i) const {return data[i];}
		double & operator[](int i) {return data[i];}
		
		Vector & operator=(const Vector &);
		Vector & operator=(const LazyExp & E);
			
};

Vector::Vector(int n){
	data = new double[n];
	for(int i=0; i<n; i++) data[i] = 0.; 
}

Vector & Vector::operator=(const Vector & A){
	if(this == &A){return *this;}
	else{
		delete [] data;
		data = new double[A.size];
		for(int i=0; i<size; i++) data[i] = A.data[i]; 
	}
	return *this;
}

Vector::Vector(const Vector & A): size(A.size) {
	data = new double[size];
	for(int i=0;i<size;i++) data[i] = A.data[i];
}


//_________________________________________________________METHODE NAÏVE

vector<double> operator+(const vector<double> & a, const vector<double> & b){
	vector<double> c(a.size());
	for(int i=0; i < c.size(); i++) c[i] = a[i] + b[i];
	return c;
}


//_______________________________________________________LAZY EXPRESSION

struct LazyExp{
	const Vector & A;
	const Vector & B;
	
	LazyExp(const Vector & A_, const Vector & B_): A(A_), B(B_) {};
};

//là où se fait l'évaluation
inline Vector & Vector::operator=(const LazyExp & E){
	for(int i=0; i < E.A.size; i++) data[i] = E.A.data[i] + E.B.data[i];
	return *this;
}

//pas d'évaluation
inline LazyExp operator+(const Vector & A, const Vector & B) {
	return LazyExp(A,B);
}

//______________________________________________________________________


int main(void){

	//initialisation
	mt19937 G;
	uniform_real_distribution<double> U(-1.,1.);
	int n = 10000000;
	Vector A(n), B(n), C(n);
	vector<double> a(n), b(n), c(n);
	for(int i = 0; i < n; i++){
		A[i] = U(G);
		B[i] = U(G);
		a[i] = A[i];
		b[i] = B[i];
	}
	cout << "--- Calcul de A + B (2 vecteurs de taille " << n << ") par différentes méthodes ---" << endl;
	
	
	//méthode naïve
	auto t1 = chrono::system_clock::now();
	c = a + b;
	auto t2 = chrono::system_clock::now();
	chrono::duration<double> diff = t2 - t1;
	cout << " \nTemps de calcul méthode naïve :" << diff.count() << endl;
	
	//lazy expression
	t1 = chrono::system_clock::now();
	C = A + B;
	t2 = chrono::system_clock::now();
	diff = t2 - t1;
	cout << " \nTemps de calcul lazy expression :" << diff.count() << endl;
	
}
