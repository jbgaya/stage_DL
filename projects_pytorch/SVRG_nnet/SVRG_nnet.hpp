#include <torch/torch.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <chrono>
#include <vector>
using namespace std;
auto options_int = torch::TensorOptions().dtype(torch::kInt64).device(torch::kCPU);
auto options_double = torch::TensorOptions().dtype(torch::kFloat64).device(torch::kCUDA);


class nnet : public torch::nn::Module {
	public:
		int training_size;
		int batch_size;
		double cost;
		double learning_rate;
		string optimizer;
		
		torch::DeviceType device_type;
		torch::nn::Linear z1{nullptr}, z2{nullptr};
		
		//SAG and SAGA gradients storage
		vector<torch::Tensor> SAGA_W1, SAGA_b1, SAGA_W2, SAGA_b2;
		
		//SVRG gradients storage (snapshots)
		torch::nn::Linear z1_snapshot{nullptr}, z2_snapshot{nullptr};
		torch::Tensor 	mu_W1, mu_b1, mu_W2, mu_b2;
		bool is_snapshot = false;
		int m = 1;
		bool is_mu = true;
		
		nnet(int,int,int,int,int,double,string device="CPU",string opt="SGD");
		torch::Tensor forward(torch::optim::SGD &, torch::Tensor &);
		torch::Tensor predict(torch::Tensor &);
		torch::Tensor cross_entropy_loss(const torch::Tensor &, const torch::Tensor &);
		torch::Tensor mse_loss(const torch::Tensor &, const torch::Tensor &);
		double compute_cost();
		
		//update algorithms
		void update(int,int &);
		void update_SGD();
		void update_SAGA(int,int);
		void update_SAG(int,int);		
		void update_SVRG(int,int &);

};

//________________________________________________________Initialization
nnet::nnet(int n_train, int n_batch, int n_input,int n_hidden,int n_output,double alpha, string device,string opt): training_size(n_train), batch_size(n_batch),
		   cost(0.), learning_rate(alpha), optimizer(opt) {

	//Device choice
	device_type = (device=="GPU")?torch::kCUDA:torch::kCPU;

	//Activation functions initialization
	z1 = register_module("z1", torch::nn::Linear(n_input,n_hidden));
	z2 = register_module("z2", torch::nn::Linear(n_hidden,n_output));
	
	//Custom weight initialization
	//this->parameters()[0].set_data(torch::randn({n_hidden,n_input}));
	//this->parameters()[2].set_data(torch::randn({n_output,n_hidden}));
	
	if(optimizer == "SAGA" or optimizer == "SAG"){
		cout << optimizer << " --Initializing gradients lists..." << endl;
		SAGA_W1.resize(n_train+1);
		SAGA_b1.resize(n_train+1);
		SAGA_W2.resize(n_train+1);
		SAGA_b2.resize(n_train+1);
		for(int i=0; i < training_size+1; i++){
			SAGA_W1[i] = torch::zeros({n_hidden,n_input}).to(options_double);
			SAGA_b1[i] = torch::zeros({n_hidden}).to(options_double);
			SAGA_W2[i] = torch::zeros({n_output,n_hidden}).to(options_double);
			SAGA_b2[i] = torch::zeros({n_output}).to(options_double);
		}
		cout << "initialization ended." << endl;
	}
	if(optimizer == "SVRG"){
		cout << optimizer << " --Snapshot storage..." << endl;
		z1_snapshot = register_module("z1_snapshot", torch::nn::Linear(n_input,n_hidden));
		z2_snapshot = register_module("z2_snapshot", torch::nn::Linear(n_hidden,n_output));
		this->parameters()[4].set_data(this->parameters()[0].clone());
		this->parameters()[6].set_data(this->parameters()[2].clone());	

		mu_W1 = torch::zeros({n_hidden,n_input}).to(options_double);
		mu_b1 = torch::zeros({n_hidden}).to(options_double);
		mu_W2 = torch::zeros({n_output,n_hidden}).to(options_double);
		mu_b2 = torch::zeros({n_output}).to(options_double);
		
		is_snapshot = true;
		
		cout << "initialization ended." << endl;
	}
	
	//Send Module to device and convert to double
	this->to(device_type,torch::kFloat64);
	
}

//_______________________________________________________________Forward
torch::Tensor nnet::forward(torch::optim::SGD & opt, torch::Tensor & X){

	if(optimizer=="SVRG"){
		if(is_snapshot){
			opt.zero_grad();
			X = z1_snapshot->forward(X);
			X = torch::relu(X);
			X = z2_snapshot->forward(X);
			X = torch::tanh(X)*1.2;		
		}
		else{
			X = z1->forward(X);
			X = torch::relu(X);
			X = z2->forward(X);
			X = torch::tanh(X)*1.2;
		}
	}
	
	else{
		opt.zero_grad();
		X = z1->forward(X);
			X = torch::relu(X);
		X = z2->forward(X);
		X = torch::tanh(X)*1.2;		
	}
		
	return X;
}

//____________________________________________________________Reset cost
double nnet::compute_cost(){
	double x = cost * double(batch_size) / ( double(training_size) );
	cost = 0;
	return x;
}

//__________________________________________________________max/min norm

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


//------------------- CUSTOM LOSS FUNCTIONS ----------------------------


//____________________________________________________Cross-entropy loss
torch::Tensor nnet::cross_entropy_loss(const torch::Tensor & X, const torch::Tensor & Y){
	torch::Tensor J = (- ( Y * torch::log( X ) + ( 1 - Y ) * torch::log( 1 - X ))).sum() / double(X.size(0));
	if(is_snapshot==false) cost += J.item<double>();
	return J;
}

//______________________________________________________________MSE loss
torch::Tensor nnet::mse_loss(const torch::Tensor & X, const torch::Tensor & Y){
	torch::Tensor J = ((X-Y)*(X-Y)).sum() / double(X.size(0));
	if(is_snapshot==false) cost += J.item<double>();
	return J;
}

//------------------- CUSTOM UPDATE ALGORITHMS METHODS -----------------

//________________________________________________________________UPDATE
void nnet::update(int epoch, int & iter){
	
	if(optimizer=="SGD") this->update_SGD();
	else if(optimizer=="SAGA") this->update_SAGA(epoch,iter);
	else if(optimizer=="SAG") this->update_SAG(epoch,iter);
	else if(optimizer=="SVRG") this->update_SVRG(epoch, iter);
}





//___________________________________________________________________SGD
void nnet::update_SGD(){
	for(int i=0; i < 4; i++){
		
		this->parameters()[i].set_data(this->parameters()[i] - learning_rate * this->parameters()[i].grad());
				
	}
}

//__________________________________________________________________SAGA
void nnet::update_SAGA(int epoch,int i){
	
	//Init with SGD
	if(epoch==1){
		
		SAGA_W1[training_size] += SAGA_W1[i].clone() / double(training_size);
		SAGA_b1[training_size] += SAGA_b1[i].clone() / double(training_size);
		SAGA_W2[training_size] += SAGA_W2[i].clone() / double(training_size);
		SAGA_b2[training_size] += SAGA_b2[i].clone() / double(training_size);
				
	}
	
	//SAGA
	else{
		
		this->parameters()[0].set_data(this->parameters()[0].clone() - learning_rate * ( this->parameters()[0].grad().clone() - SAGA_W1[i].clone() + SAGA_W1[training_size].clone() ) );
		this->parameters()[1].set_data(this->parameters()[1].clone() - learning_rate * ( this->parameters()[1].grad().clone() - SAGA_b1[i].clone() + SAGA_b1[training_size].clone() ) );
		this->parameters()[2].set_data(this->parameters()[2].clone() - learning_rate * ( this->parameters()[2].grad().clone() - SAGA_W2[i].clone() + SAGA_W2[training_size].clone() ) );
		this->parameters()[3].set_data(this->parameters()[3].clone() - learning_rate * ( this->parameters()[3].grad().clone() - SAGA_b2[i].clone() + SAGA_b2[training_size].clone() ) );

		SAGA_W1[training_size] += ( this->parameters()[0].grad() - SAGA_W1[i].clone() ) / double(training_size);
		SAGA_b1[training_size] += ( this->parameters()[1].grad() - SAGA_b1[i].clone() ) / double(training_size);
		SAGA_W2[training_size] += ( this->parameters()[2].grad() - SAGA_W2[i].clone() ) / double(training_size);
		SAGA_b2[training_size] += ( this->parameters()[3].grad() - SAGA_b2[i].clone() ) / double(training_size);
	
		SAGA_W1[i].set_data(this->parameters()[0].grad().clone());
		SAGA_b1[i].set_data(this->parameters()[1].grad().clone());
		SAGA_W2[i].set_data(this->parameters()[2].grad().clone());
		SAGA_b2[i].set_data(this->parameters()[3].grad().clone());
					
	}
}

//___________________________________________________________________SAG
void nnet::update_SAG(int epoch,int i){
	
	//Init with SGD
	if(epoch==1){

		SAGA_W1[i].set_data(this->parameters()[0].grad().clone());
		SAGA_b1[i].set_data(this->parameters()[1].grad().clone());
		SAGA_W2[i].set_data(this->parameters()[2].grad().clone());
		SAGA_b2[i].set_data(this->parameters()[3].grad().clone());
		
		SAGA_W1[training_size] += SAGA_W1[i].clone() / double(training_size);
		SAGA_b1[training_size] += SAGA_b1[i].clone() / double(training_size);
		SAGA_W2[training_size] += SAGA_W2[i].clone() / double(training_size);
		SAGA_b2[training_size] += SAGA_b2[i].clone() / double(training_size);
				
	}
	
	//SAG
	else{
		
		SAGA_W1[training_size] += ( this->parameters()[0].grad().clone() - SAGA_W1[i].clone() ) / double(training_size);
		SAGA_b1[training_size] += ( this->parameters()[1].grad().clone() - SAGA_b1[i].clone() ) / double(training_size);
		SAGA_W2[training_size] += ( this->parameters()[2].grad().clone() - SAGA_W2[i].clone() ) / double(training_size);
		SAGA_b2[training_size] += ( this->parameters()[3].grad().clone() - SAGA_b2[i].clone() ) / double(training_size);

		
		this->parameters()[0].set_data(this->parameters()[0].clone() - learning_rate * ( SAGA_W1[training_size].clone() ) );
		this->parameters()[1].set_data(this->parameters()[1].clone() - learning_rate * ( SAGA_b1[training_size].clone() ) );
		this->parameters()[2].set_data(this->parameters()[2].clone() - learning_rate * ( SAGA_W2[training_size].clone() ) );
		this->parameters()[3].set_data(this->parameters()[3].clone() - learning_rate * ( SAGA_b2[training_size].clone() ) );

		SAGA_W1[i].set_data(this->parameters()[0].grad().clone());
		SAGA_b1[i].set_data(this->parameters()[1].grad().clone());
		SAGA_W2[i].set_data(this->parameters()[2].grad().clone());
		SAGA_b2[i].set_data(this->parameters()[3].grad().clone());
					
	}
}

//__________________________________________________________________SVRG
void nnet::update_SVRG(int epoch,int & i){
	
	//Init with SGD
	if(is_mu){
		

		mu_W1 += this->parameters()[4].grad().clone() / double(training_size);
		mu_b1 += this->parameters()[5].grad().clone() / double(training_size);
		mu_W2 += this->parameters()[6].grad().clone() / double(training_size);
		mu_b2 += this->parameters()[7].grad().clone() / double(training_size);

	
		if(i==training_size-1){
			i = -1;
			is_mu = false;
		}
	}
	
	//
	else{
		
		if(is_snapshot==false){

			this->parameters()[0].set_data(this->parameters()[0].clone() - learning_rate * ( this->parameters()[0].grad().clone() - this->parameters()[4].grad().clone() + mu_W1 ) );
			this->parameters()[1].set_data(this->parameters()[1].clone() - learning_rate * ( this->parameters()[1].grad().clone() - this->parameters()[5].grad().clone() + mu_b1 ) );
			this->parameters()[2].set_data(this->parameters()[2].clone() - learning_rate * ( this->parameters()[2].grad().clone() - this->parameters()[6].grad().clone() + mu_W2 ) );
			this->parameters()[3].set_data(this->parameters()[3].clone() - learning_rate * ( this->parameters()[3].grad().clone() - this->parameters()[7].grad().clone() + mu_b2 ) );			
			is_snapshot = true;
		}
		
		else {
			is_snapshot = false;
			i--;
		}
		
		if(i==training_size-1){
			cout << "" << (epoch*2)-2+m << "\t\t" << this->compute_cost() << endl;
			if(m<2){
				i = -1;
				m++;
				}
			else{
				this->parameters()[4].set_data( this->parameters()[0].clone() );
				this->parameters()[5].set_data( this->parameters()[1].clone() );
				this->parameters()[6].set_data( this->parameters()[2].clone() );
				this->parameters()[7].set_data( this->parameters()[3].clone() );
				
				mu_W1 -= mu_W1;
				mu_b1 -= mu_b1;
				mu_W2 -= mu_W2;
				mu_b2 -= mu_b2;
				
				m = 1;
				is_mu = true;
			}
		}
	}
}
