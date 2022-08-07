// NeuralNetwork.cpp  from https://www.geeksforgeeks.org/ml-neural-network-implementation-in-c-from-scratch/
// Enhanced with work and verify methods
#include <memory>
#include <cmath>
#include "NeuralNetwork.hpp"

// support procs
Scalar activationFunction(Scalar x)
{
	return tanhf(x);
}

Scalar activationFunctionDerivative(Scalar x)
{
	return 1 - tanhf(x) * tanhf(x);
}

// constructor of neural network class
NeuralNetwork::NeuralNetwork(std::vector<uint> topology, Scalar learningRate)
{
	this->topology = topology;
	this->learningRate = learningRate;
	for (uint i = 0; i < topology.size(); i++) {
		// initialize neuron layers
		if (i == topology.size() - 1)
			neuronLayers.push_back(new RowVector(topology[i]));
		else
			neuronLayers.push_back(new RowVector(topology[i] + 1));

		// initialize cache and delta vectors
		cacheLayers.push_back(new RowVector(neuronLayers.size()));
		deltas.push_back(new RowVector(neuronLayers.size()));

		// vector.back() gives the handle to recently added element
		// coeffRef gives the reference of value at that place
		// (using this as we are using pointers here)
		if (i != topology.size() - 1) {
			neuronLayers.back()->coeffRef(topology[i]) = 1.0;
			cacheLayers.back()->coeffRef(topology[i]) = 1.0;
		}

		// initialize weights matrix
		if (i > 0) {
			if (i != topology.size() - 1) {
				weights.push_back(new Matrix(topology[i - 1] + 1, topology[i] + 1));
				weights.back()->setRandom();
				weights.back()->col(topology[i]).setZero();
				weights.back()->coeffRef(topology[i - 1], topology[i]) = 1.0;
			}
			else {
				weights.push_back(new Matrix(topology[i - 1] + 1, topology[i]));
				weights.back()->setRandom();
			}
		}
	}
};

void NeuralNetwork::propagateForward(RowVector& input)
{
	// set the input to input layer
	// block returns a part of the given vector or matrix
	// block takes 4 arguments : startRow, startCol, blockRows, blockCols
	neuronLayers.front()->block(0, 0, 1, neuronLayers.front()->size() - 1) = input;

	// propagate the data forward and then
	// apply the activation function to your network
	// unaryExpr applies the given function to all elements of CURRENT_LAYER
	for (uint i = 1; i < topology.size(); i++) {
		// already explained above
		(*neuronLayers[i]) = (*neuronLayers[i - 1]) * (*weights[i - 1]);
		neuronLayers[i]->block(0, 0, 1, topology[i]).unaryExpr(std::ptr_fun(activationFunction));
	}
}

void NeuralNetwork::calcErrors(RowVector& output)
{
	// calculate the errors made by neurons of last layer
	(*deltas.back()) = output - (*neuronLayers.back());

	// error calculation of hidden layers is different
	// we will begin by the last hidden layer
	// and we will continue till the first hidden layer
	for (uint i = topology.size() - 2; i > 0; i--) {
		(*deltas[i]) = (*deltas[i + 1]) * (weights[i]->transpose());
	}
}

void NeuralNetwork::updateWeights()
{
	// topology.size()-1 = weights.size()
	for (uint i = 0; i < topology.size() - 1; i++) {
		// in this loop we are iterating over the different layers (from first hidden to output layer)
		// if this layer is the output layer, there is no bias neuron there, number of neurons specified = number of cols
		// if this layer not the output layer, there is a bias neuron and number of neurons specified = number of cols -1
		if (i != topology.size() - 2) {
			for (uint c = 0; c < weights[i]->cols() - 1; c++) {
				for (uint r = 0; r < weights[i]->rows(); r++) {
					weights[i]->coeffRef(r, c) += learningRate * deltas[i + 1]->coeffRef(c) * activationFunctionDerivative(cacheLayers[i + 1]->coeffRef(c)) * neuronLayers[i]->coeffRef(r);
				}
			}
		}
		else {
			for (uint c = 0; c < weights[i]->cols(); c++) {
				for (uint r = 0; r < weights[i]->rows(); r++) {
					weights[i]->coeffRef(r, c) += learningRate * deltas[i + 1]->coeffRef(c) * activationFunctionDerivative(cacheLayers[i + 1]->coeffRef(c)) * neuronLayers[i]->coeffRef(r);
				}
			}
		}
	}
}

void NeuralNetwork::propagateBackward(RowVector& output)
{
	calcErrors(output);
	updateWeights();
}


// you can use your own code here!

void NeuralNetwork::train(std::vector<RowVector*> input_data, std::vector<RowVector*> output_data)
{
	int correct = 0;
	for (uint i = 0; i < input_data.size(); i++) {
		std::cout << "Input to neural network is : " << *input_data[i] << std::endl;
		propagateForward(*input_data[i]);
		std::cout << "Expected output is : " << *output_data[i] << std::endl;
		std::cout << "Output produced is : " << *neuronLayers.back() << std::endl;
		auto expected2 = *output_data[i];
		auto expected = expected2.coeff(1,-1);
		auto produced2 = *neuronLayers.back();
		auto produced = produced2.coeff(1,-1);
		std::cout << "Expected output is : " << expected << std::endl;
		std::cout << "Output produced is : " << produced << std::endl;
		if ( round(produced) == expected ) {
			correct++;
			std::cout << "  correct" << std::endl;
		} else {
			std::cout << "  incorrect" << std::endl;
		}
		propagateBackward(*output_data[i]);
		std::cout << "MSE : " << std::sqrt((*deltas.back()).dot((*deltas.back())) / deltas.back()->size()) << std::endl;
	}
	if (input_data.size() > 0)
	{
		std::cout << "Score: " << correct*100/input_data.size() << "%  " << "(" << correct << " of " << input_data.size() << ")" << std::endl;
	}
}

std::vector<RowVector> NeuralNetwork::work(std::vector<RowVector*> input_data)
{
	std::vector<RowVector> output_data;
	for (uint i = 0; i < input_data.size(); i++) {
		std::cout << "Input to neural network is : " << *input_data[i] << std::endl;
		propagateForward(*input_data[i]);
		std::cout << "Output produced is : " << *neuronLayers.back() << std::endl;
		auto out = *neuronLayers.back();
		output_data.push_back(out);
	}
	return output_data;
}

void NeuralNetwork::verify(std::vector <std::shared_ptr<TrainingData>> vData, std::vector<RowVector*> input_data)
{
	int correct = 0;
	for (uint i = 0; i < input_data.size(); i++) {
		std::cout << "Input to neural network is : " << *input_data[i] << std::endl;
		propagateForward(*input_data[i]);
		int expected = vData[i]->id;
		auto produced = *neuronLayers.back();
		auto produced2 = produced.coeff(1,-1);
		std::cout << "Expected output is : " << expected << std::endl;
		std::cout << "Output produced is : " << produced2 << std::endl;
		if ( round(produced2) == expected ) {
			correct++;
			std::cout << "  correct" << std::endl;
		} else {
			std::cout << "  incorrect" << std::endl;
		}
	}
	if (input_data.size() > 0)
	{
		std::cout << "Score: " << correct*100/input_data.size() << "%  " << "(" << correct << " of " << input_data.size() << ")" << std::endl;
	}
}