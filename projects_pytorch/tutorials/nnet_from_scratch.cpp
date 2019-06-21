#include <torch/torch.h>
#include <iostream>
using namespace std;

class nnet {
	private:
		double learning_rate; //learning rate
		
		int n_input,n_hidden,n_output; //dimensions
	
		torch::Tensor W1, b1, W2, b2; //Poids et biais
		torch::Tensor z1, g1, z2, g2; //Fonctions intermédiaires linéaires et activations
		torch::Tensor dW1, db1, dW2, db2, dz1, dg1, dz2, dg2; //gradients
		torch::Tensor VdW1,VdW2,Vdb1,Vdb2; //momentum
		torch::Tensor J; //cost function
		
	public:
		nnet(int,int,int,double);
		void forward(torch::Tensor &,int);
		void compute_cost(torch::Tensor &,int batch_size);
		void backward(const torch::Tensor &,const torch::Tensor &,int batch_size);
		void update();
		
	//Visualisation paramètres
		torch::Tensor Weight1() const { return W1;}
		torch::Tensor Weight2() const { return W2;}
		torch::Tensor bias1() const { return b1;}
		torch::Tensor bias2() const { return b2;}
		torch::Tensor cost() const { return J;}
		torch::Tensor pred() const { return g2;}
};

//initialisation
nnet::nnet(int n_i,int n_h,int n_o, double alpha): n_input(n_i), n_hidden(n_h), n_output(n_o), learning_rate(alpha) {
	
	//1ere couche
	W1 = torch::randn({n_hidden,n_input}, torch::dtype(torch::kFloat64));
	b1 = torch::zeros({n_hidden,1}, torch::dtype(torch::kFloat64));
	
	//2eme couche
	W2 = torch::randn({n_output,n_hidden}, torch::dtype(torch::kFloat64));
	b2 = torch::zeros({n_output,1}, torch::dtype(torch::kFloat64));
	
	//momentum
	VdW1 = torch::zeros({n_hidden,n_input}, torch::dtype(torch::kFloat64));
	Vdb1 = torch::zeros({n_hidden,1}, torch::dtype(torch::kFloat64));
	
	VdW2 = torch::zeros({n_output,n_hidden}, torch::dtype(torch::kFloat64));
	Vdb2 = torch::zeros({n_output,1}, torch::dtype(torch::kFloat64));
	
}

//forward propagation
void nnet::forward(torch::Tensor & X,int batch_size){
	
	//1ere couche
	z1 = W1.mm(X) + b1;
	g1 = torch::sigmoid(z1);
	
	//2eme couche
	z2 = W2.mm(g1) + b2;
	g2 = torch::sigmoid(z2);
	
	
	//Problème des infiniment petits :
	
	//g2 += torch::ones({n_output,batch_size}, torch::dtype(torch::kFloat64))*0.00001;
	
}

//Calcul du coût
void nnet::compute_cost(torch::Tensor & Y,int batch_size){
	
	J = - (1/double(batch_size)) * (Y.transpose(0,1).mm(torch::log(g2)) + (1-Y).transpose(0,1).mm(torch::log(1-g2)));
	//J = (g2-Y)*(g2-Y)/double(batch_size);
}

//Backward propagation
void nnet::backward(const torch::Tensor & X,const torch::Tensor & Y,int batch_size){

	// dJ/dg2
	dg2 = -((Y / g2) - ((1-Y) / (1-g2)))/double(batch_size);
	//dg2 = 2*(g2-Y)/double(batch_size);
	//cout << "g2 : "<< g2.size(0) <<" x "  << g2.size(1) << endl;
	//cout << "dg2 : "<< dg2.size(0) <<" x "  << dg2.size(1) << endl;
	
	// dJ/dz2
	dz2 = dg2 * g2 * (1 - g2);
	//cout << "\nz2 : "<< z2.size(0) <<" x "  << z2.size(1) << endl;
	//cout << "dz2 : "<< dz2.size(0) <<" x "  << dz2.size(1) << endl;
	
	// dJ/dW2
	dW2 = dz2.mm(g1.transpose(0,1))/double(batch_size);
	//cout << "\nW2 : "<< W2.size(0) <<" x "  << W2.size(1) << endl;
	//cout << "dW2 : "<< dW2.size(0) <<" x "  << dW2.size(1) << endl;
	
	// dJ/db2
	dz2 = dz2.transpose(0,1);
	db2 = dz2[0].reshape({n_output,1})/double(batch_size);
	for(long i = 1; i < batch_size; i++){
		db2 += dz2[i].reshape({n_output,1})/double(batch_size);
	}
	dz2 = dz2.transpose(0,1);
	//cout << "\nb2 : "<< b2.size(0) <<" x "  << b2.size(1) << endl;
	//cout << "db2 : "<< db2.size(0) <<" x "  << db2.size(1) << endl;
	
	// dJ/dg1
	dg1 = W2.transpose(0,1);
	dg1 = dg1.mm(dz2);
	//cout << "\ng1 : "<< g1.size(0) <<" x "  << g1.size(1) << endl;
	//cout << "dg1 : "<< dg1.size(0) <<" x "  << dg1.size(1) << endl;
	
	// dJ/dz1
	dz1 = dg1 * g1 * (1 - g1);
	//cout << "\nz1 : "<< z1.size(0) <<" x "  << z1.size(1) << endl;
	//cout << "dz1 : "<< dz1.size(0) <<" x "  << dz1.size(1) << endl;
	
	// dJ/dW1 
	dW1 = dz1.mm(X.transpose(0,1))/double(batch_size);
	//cout << "\nW1 : "<< W1.size(0) <<" x "  << W1.size(1) << endl;
	//cout << "dW1 : "<< dW1.size(0) <<" x "  << dW1.size(1) << endl;
	
	// dJ/db1
	dz1 = dz1.transpose(0,1);
	db1 = dz1[0].reshape({n_hidden,1})/double(batch_size);
	for(long i = 1; i < batch_size; i++){
		db1 += dz1[i].reshape({n_hidden,1})/double(batch_size);
	}
	dz1 = dz1.transpose(0,1);
	//cout << "\nb1 : "<< b1.size(0) <<" x "  << b1.size(1) << endl;
	//cout << "db1 : "<< db1.size(0) <<" x "  << db1.size(1) << endl;
}

void nnet::update(){
	//calcul du momentum
	VdW2 = 0.9 * VdW2 + 0.1 * dW2;
	Vdb2 = 0.9 * Vdb2 + 0.1 * db2;
	
	VdW1 = 0.9 * VdW1 + 0.1 * dW1;
	Vdb1 = 0.9 * Vdb1 + 0.1 * db1;
	
	
	//1ere couche
	W2 = W2 - VdW2 * learning_rate;
	b2 = b2 - Vdb2 * learning_rate;

	
	//2eme couche
	W1 = W1 - VdW1 * learning_rate;
	b1 = b1 - Vdb1 * learning_rate;

}



int main(){
	nnet neuralnet(784,64,10,0.01);
	torch::Tensor Y;
	torch::Tensor X;
	
	int batch_size = 1;	
	int epochs = 5;	

	auto data_loader = torch::data::make_data_loader(
                       torch::data::datasets::MNIST("../data").map(
                       torch::data::transforms::Stack<>()),batch_size);
	
	for(int i=0;i<epochs;i++){
		for(auto& sample : *data_loader){

			X = sample.data.reshape({batch_size,784}).to(torch::TensorOptions().dtype(torch::kFloat64)).transpose(0,1);
			Y = at::one_hot(sample.target,10).transpose(0,1).to(torch::TensorOptions().dtype(torch::kFloat64));			

			neuralnet.forward(X,batch_size);
			neuralnet.compute_cost(Y,batch_size);
			neuralnet.backward(X,Y,batch_size);
			neuralnet.update();
		}

		cout << neuralnet.pred().transpose(0,1)[0] << endl;
		cout << Y.transpose(0,1)[0] << endl;
		cout << neuralnet.cost() << endl;
	}
}
