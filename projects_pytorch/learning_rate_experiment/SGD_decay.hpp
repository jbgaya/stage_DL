#include <torch/torch.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <chrono>
#include <vector>
using namespace std;
auto options_int = torch::TensorOptions().dtype(torch::kInt64).device(torch::kCUDA);
auto options_double = torch::TensorOptions().dtype(torch::kFloat64).device(torch::kCUDA);


class nnet : public torch::nn::Module {
	public:
		int training_size;
		int batch_size;
		int n_layers = 4;
		double cost;
		double learning_rate;
		string optimizer;
		
		torch::DeviceType device_type;
		torch::nn::Linear z1{nullptr},z2{nullptr},z3{nullptr},z4{nullptr};
		
		nnet(int,int,vector<int> &,double,string device="CPU");
		
		torch::Tensor forward( torch::Tensor &);
		torch::Tensor cross_entropy_loss(const torch::Tensor &, const torch::Tensor &, bool is_cost=true);
		torch::Tensor mse_loss(const torch::Tensor &, const torch::Tensor &);
		double compute_cost();
		void update_SGD();
		
		//auxiliary functions
		torch::Tensor predict(torch::Tensor &);

};

//________________________________________________________Initialization
nnet::nnet(int n_train, int n_batch, vector<int> & layers,double alpha, string device): training_size(n_train), batch_size(n_batch),
		   cost(0.), learning_rate(alpha) {

	//Device choice
	device_type = (device=="GPU")?torch::kCUDA:torch::kCPU;

	//Activation functions initialization
		z1 = register_module("z1", torch::nn::Linear(layers[0],layers[1]));
		z2 = register_module("z2", torch::nn::Linear(layers[1],layers[2]));
		z3 = register_module("z3", torch::nn::Linear(layers[2],layers[3]));
		z4 = register_module("z4", torch::nn::Linear(layers[3],layers[4]));

	//Send Module to device and convert to double
	this->to(device_type,torch::kFloat64);
	
}

//_______________________________________________________________Forward
torch::Tensor nnet::forward( torch::Tensor & X ){	

		X = z1->forward(X);
		X = torch::relu(X);

		X = z2->forward(X);
		X = torch::relu(X);
		
		X = z3->forward(X);
		X = torch::relu(X);

		X = z4->forward(X);
		X = torch::softmax(X,1);
		
	return X;
}

//____________________________________________________________Reset cost
double nnet::compute_cost(){
	double x = cost / ( double(training_size) );
	cost = 0;
	return x;
}


//------------------- CUSTOM LOSS FUNCTIONS ----------------------------

//______________________________________________________________MSE loss
torch::Tensor nnet::mse_loss(const torch::Tensor & X, const torch::Tensor & Y){
	torch::Tensor J = ((X-Y)*(X-Y)).sum() / double(batch_size) ;
	cost += J.item<double>();
	return J;
}

//____________________________________________________Cross-entropy loss
torch::Tensor nnet::cross_entropy_loss(const torch::Tensor & X, const torch::Tensor & Y,bool is_cost){
	torch::Tensor J = (- ( Y * torch::log( X + 0.000000001 ) + ( 1 - Y ) * torch::log( 1 - X + 0.000000001 ))).sum() / double(batch_size);
	if(is_cost) cost += J.item<double>();
	return J;
}

//------------------- CUSTOM UPDATE ALGORITHMS METHODS -----------------

//___________________________________________________________________SGD
void nnet::update_SGD(){
	for(int i=0; i < n_layers * 2; i++){
		this->parameters()[i].set_data(this->parameters()[i] - learning_rate * this->parameters()[i].grad());			
	}
}


//___________________________________________________Auxiliary functions

double max_mnist(){
	double x = 0.;
	auto train_set = torch::data::make_data_loader(
                     torch::data::datasets::MNIST("../../data").map(
                     torch::data::transforms::Stack<>()),1);

	for(auto& sample : *(train_set)){
		auto X_train = sample.data.reshape({1,784}).to(options_double);
		x = max(x,(X_train * X_train).sum().item<double>());
	}
	return x;
}

double min_mnist(){
	double x = 100000.;
	auto train_set = torch::data::make_data_loader(
                     torch::data::datasets::MNIST("../../data").map(
                     torch::data::transforms::Stack<>()),1);

	for(auto& sample : *(train_set)){
		auto X_train = sample.data.reshape({1,784}).to(options_double);
		x = min(x,(X_train * X_train).sum().item<double>());
	}
	return x;
}

torch::Tensor nnet::predict(torch::Tensor & X_test){
	
	return this->forward(X_test).argmax(1).to(torch::TensorOptions().dtype(torch::kInt64));
	
}

double error_rate(const torch::Tensor & Y_test, const torch::Tensor & Y_hat){
	
	return 1 - (at::one_hot(Y_test,10) - at::one_hot(Y_hat,10)).abs().sum().item<double>()/(2.*double(Y_test.size(0)));
	
}