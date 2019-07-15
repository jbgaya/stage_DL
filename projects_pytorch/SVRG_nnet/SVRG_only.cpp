#include "SVRG_only.hpp"
#include "function_gen.hpp"
using namespace std;

int main(){

	
	//______________________________________________Initializing dataset
		int d = 1;
		int n = 2000; 
		int n_test = 200;
		int m = 5; //number of passes
		int iter;
		auto X_train = torch::rand({n,d}).to(options_double) * 6.28;
		auto X_test = torch::rand({n_test,d}).to(options_double) * 6.28;
		
		mt19937 G;
		uniform_int_distribution<int> U(0,n-1);
		
		//auto Y_train = euclidean_norm(X_train).to(options_double);
		//auto Y_test = euclidean_norm(X_test).to(options_double);
		auto Y_train = sin(X_train).to(options_double);
		auto Y_test = sin(X_test).to(options_double);	
	
	
	//_______________________________________Initializing neural network
		int epochs = 8;
		int batch_size = 1;
		double learning_rate = 0.05;
		nnet neuralnet(n,batch_size,d,20,1,learning_rate,"GPU");
		torch::optim::SGD optimizer(neuralnet.parameters(), 0.01);	
		
		
		
	//_________________________________________________Running algorithm
	cout << "Iter" << "\t\t" << "loss" << endl;
	auto t1 = chrono::system_clock::now();

		for(int i=1; i <= epochs;i++){
			
			//Update of mu
			for(int k=0; k < n; k++){
				auto X = X_train[k].clone();
				auto Y = Y_train[k].clone();
				
				X = neuralnet.forward( optimizer, X );
				
				auto loss =  neuralnet.mse_loss( X , Y );		
				
				loss.backward();
				
				neuralnet.update_mu();
			}			
			
			//SVRG algorithm on n*m iterations
			for(int k=0; k < n*m; k++){
				opt.zero_grad();
				
				//Pick a sample randomly
				iter = U(G);
				
				//Snapshot gradient
				auto X = X_train[iter].clone();
				auto Y = Y_train[iter].clone();
				X = neuralnet.forward_snapshot( optimizer, X );
				auto loss =  neuralnet.mse_loss( X , Y );		
				loss.backward();
				
				//Real gradient
				X = X_train[iter].clone();
				Y = Y_train[iter].clone();
				X = neuralnet.forward( optimizer, X );
				loss =  neuralnet.mse_loss( X , Y );
				loss.backward();
				
				//update
				neuralnet.update();
				
			}
			
			neuralnet.set_snapshot();
			
			//cout << "" << i << "\t" << neuralnet.compute_cost() << endl;
		}

	auto t2 = chrono::system_clock::now();
	chrono::duration<double> diff = t2 - t1;
	cout << "Phase d'apprentissage terminée en " << diff.count() << " sec" << endl;
		
	//______________________________________________________Evaluating model
	
	auto test = X_test.clone();
	neuralnet.is_snapshot = false;
	auto Y_hat = neuralnet.forward( optimizer, X_test );
	auto loss =  neuralnet.mse_loss( Y_hat , Y_test );
	
	ofstream file("../../data/sin_app_SVRG_5.dat");
	for(int i=0;i<test.size(0);i++){
		file << test[i].item<double>() << "\t" << Y_hat[i].item<double>() << endl;
	}
	
	cout << "\nTEST SET" << endl;
	cout << "MSE = " << neuralnet.compute_cost() * n << endl;
}
