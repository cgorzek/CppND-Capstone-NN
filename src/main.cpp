#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <vector>
#include <eigen3/Eigen/Eigen>
#include <memory>
#include <regex>

// main.cpp

// don't forget to include out neural network
#include "NeuralNetwork.hpp"
#include "Labels.h"
#include "Data.h"
#include "TrainingData.h"

std::smatch RegexLabel (std::string &line) {
	std::smatch m;
	std::regex regexp_label("(libs_[A-Za-z0-9_]+)");
	std::regex_search(line, m, regexp_label);
	return m;
}

// ReadLabelData - Expected format is just one single word on each line.
void ReadLabelData(std::string filename, std::vector<Labels> &labelData){
	std::ifstream file(filename, std::ifstream::in);
	std::string line;
	std::smatch m;
	Labels l;
	int id = 1;
	// read the file
	if (file.is_open()) {
		while (std::getline(file, line, '\n')) {
			m = RegexLabel(line);
			l.label = m[1].str();
			l.id = id;
			labelData.push_back(l);
			id += 1;
		}
	}
}

void ReadData(std::string filename, std::vector<std::shared_ptr<Data>> &data){
	std::ifstream file(filename, std::ifstream::in);
	std::string line;
	// read the file
	if (file.is_open()) {
		while (std::getline(file, line, '\n')) {
			// parse line and populate parameters
			auto d = std::make_shared<Data>();
            d->add(line);
			data.push_back(d);
		}
	}
}

void ReadTrainingData(std::string filename, const std::vector<Labels> &labelData, std::vector<std::shared_ptr<TrainingData>> &data){
	std::ifstream file(filename, std::ifstream::in);
	std::string line;
	std::string label;
	int id;
	Labels l;
	std::regex regexp_data("^[ ]+-[ ]+");
    std::smatch m;

	if (file.is_open()) {
		while (std::getline(file, line, '\n')) {
			// parse line and populate parameters
			m = RegexLabel(line);
        	if (m[1].length() > 0) {			
				for ( int i = 0; i < labelData.size(); i++ ) {
					l = labelData[i];
					if ( m[1].str() == l.label) {
						label = l.label;
						id = l.id;
					}
				}
			} else if (std::regex_search(line, m, regexp_data)) {
            	auto d = std::make_shared<TrainingData>();
				d->add(line);
				d->label = label;
				d->id = id;
				data.push_back(d);
			}
		}
	}
}

void PrepInputNNData(std::vector<std::shared_ptr<TrainingData>> &idata, std::vector<RowVector*>& data)
{
	data.clear();
	int verbose = 0;

	float sum_v = 0;
	float mean_v = 0;
	float psum_v = 0;
	float stddev_v = 0;
	float sum_t = 0;
	float mean_t = 0;
	float psum_t = 0;
	float stddev_t = 0;
	float sum_x = 0;
	float mean_x = 0;
	float psum_x = 0;
	float stddev_x = 0;
	float sum_tt = 0;
	float mean_tt = 0;
	float psum_tt = 0;
	float stddev_tt = 0;
	float sum_c = 0;
	float mean_c = 0;
	float psum_c = 0;
	float stddev_c = 0;
	// compute mean ands std dev for each param for normalization.
	for ( auto pt : idata) {
		sum_v += pt->get_v();
		sum_t += pt->get_t();
		sum_x += pt->get_x_code();
		sum_tt += pt->get_tt_code();
		sum_c += pt->get_c_code();
	}
	mean_v = sum_v/idata.size();
	mean_t = sum_t/idata.size();
	mean_x = sum_x/idata.size();
	mean_tt = sum_tt/idata.size();
	mean_c = sum_c/idata.size();
	for ( auto pt : idata) {
		psum_v = pow((pt->get_v() - mean_v),2);
		psum_t = pow((pt->get_t() - mean_t),2);
		psum_x = pow((pt->get_x_code() - mean_x),2);
		psum_tt = pow((pt->get_tt_code() - mean_tt),2);
		psum_c = pow((pt->get_c_code() - mean_c),2);
	}
	stddev_v = sqrt(psum_v);
	stddev_t = sqrt(psum_t);
	stddev_x = sqrt(psum_x);
	stddev_tt = sqrt(psum_tt);
	stddev_c = sqrt(psum_c);

	uint cols = 5;
	// enter normalized numbers into row vectors
	for ( auto pt : idata) {
		if (verbose)
		{
			std::cout << "prep - pt " << pt << std::endl;
			std::cout << "prep - label:" << pt->label << " id: " << pt->id << std::endl;
			std::cout << "prep - volt: " << pt->get_v() << std::endl;
			std::cout << "prep - temp: " << pt->get_t() << std::endl;
			std::cout << "prep - extr: " << pt->get_x() << std::endl;
			std::cout << "prep - tt: " << pt->get_tt() << std::endl;
			std::cout << "prep - corner: " << pt->get_c() << std::endl;
		}
		data.push_back(new RowVector(1, cols));
		uint i = 0;
		data.back()->coeffRef(i) = Scalar((pt->get_v() - mean_v)/stddev_v);
		if (verbose) { std::cout << "prep - normalized v " << data.back()->coeffRef(i) << " v: " << pt->get_v() << " mean_v: " << mean_v << " stddev_v: " << stddev_v << std::endl; }
		i++;
		data.back()->coeffRef(i) = Scalar((pt->get_t() - mean_t)/stddev_t);
		if (verbose) { std::cout << "prep - normalized t " << data.back()->coeffRef(i) << " t: " << pt->get_t() << " mean_t: " << mean_t << " stddev_t: " << stddev_t << std::endl; }
		i++;
		data.back()->coeffRef(i) = Scalar((pt->get_x_code() - mean_x)/stddev_x);
		if (verbose) { std::cout << "prep - normalized x " << data.back()->coeffRef(i) << " x: " << pt->get_x_code() << " mean_x: " << mean_x << " stddev_x: " << stddev_x << std::endl; }
		i++;
		data.back()->coeffRef(i) = Scalar((pt->get_tt_code() - mean_tt)/stddev_tt);
		if (verbose) { std::cout << "prep - normalized tt " << data.back()->coeffRef(i) << " tt: " << pt->get_tt_code() << " mean_tt: " << mean_tt << " stddev_tt: " << stddev_tt << std::endl; }
		i++;
		data.back()->coeffRef(i) = Scalar((pt->get_c_code() - mean_c)/stddev_c);
		if (verbose) { std::cout << "prep - normalized c " << data.back()->coeffRef(i) << " c: " << pt->get_c_code() << " mean_c: " << mean_c << " stddev_c: " << stddev_c << std::endl; }
	}
	
}

void PrepInputNNData(std::vector<std::shared_ptr<Data>> &idata, std::vector<RowVector*>& data)
{
	data.clear();
	int verbose = 0;

	float sum_v = 0;
	float mean_v = 0;
	float psum_v = 0;
	float stddev_v = 0;
	float sum_t = 0;
	float mean_t = 0;
	float psum_t = 0;
	float stddev_t = 0;
	float sum_x = 0;
	float mean_x = 0;
	float psum_x = 0;
	float stddev_x = 0;
	float sum_tt = 0;
	float mean_tt = 0;
	float psum_tt = 0;
	float stddev_tt = 0;
	float sum_c = 0;
	float mean_c = 0;
	float psum_c = 0;
	float stddev_c = 0;
	// compute mean ands std dev for each param for normalization.
	for ( auto pt : idata) {
		sum_v += pt->get_v();
		sum_t += pt->get_t();
		sum_x += pt->get_x_code();
		sum_tt += pt->get_tt_code();
		sum_c += pt->get_c_code();
	}
	mean_v = sum_v/idata.size();
	mean_t = sum_t/idata.size();
	mean_x = sum_x/idata.size();
	mean_tt = sum_tt/idata.size();
	mean_c = sum_c/idata.size();
	for ( auto pt : idata) {
		psum_v = pow((pt->get_v() - mean_v),2);
		psum_t = pow((pt->get_t() - mean_t),2);
		psum_x = pow((pt->get_x_code() - mean_x),2);
		psum_tt = pow((pt->get_tt_code() - mean_tt),2);
		psum_c = pow((pt->get_c_code() - mean_c),2);
	}
	stddev_v = sqrt(psum_v);
	stddev_t = sqrt(psum_t);
	stddev_x = sqrt(psum_x);
	stddev_tt = sqrt(psum_tt);
	stddev_c = sqrt(psum_c);

	uint cols = 5;
	// enter normalized numbers into row vectors
	for ( auto pt : idata) {
		if (verbose)
		{
			std::cout << "prep - pt " << pt << std::endl;
			std::cout << "prep - volt: " << pt->get_v() << std::endl;
			std::cout << "prep - temp: " << pt->get_t() << std::endl;
			std::cout << "prep - extr: " << pt->get_x() << std::endl;
			std::cout << "prep - tt: " << pt->get_tt() << std::endl;
			std::cout << "prep - corner: " << pt->get_c() << std::endl;
		}
		data.push_back(new RowVector(1, cols));
		uint i = 0;
		data.back()->coeffRef(i) = Scalar((pt->get_v() - mean_v)/stddev_v);
		if (verbose) { std::cout << "prep - normalized v " << data.back()->coeffRef(i) << " v: " << pt->get_v() << " mean_v: " << mean_v << " stddev_v: " << stddev_v << std::endl; }
		i++;
		data.back()->coeffRef(i) = Scalar((pt->get_t() - mean_t)/stddev_t);
		if (verbose) { std::cout << "prep - normalized t " << data.back()->coeffRef(i) << " t: " << pt->get_t() << " mean_t: " << mean_t << " stddev_t: " << stddev_t << std::endl; }
		i++;
		data.back()->coeffRef(i) = Scalar((pt->get_x_code() - mean_x)/stddev_x);
		if (verbose) { std::cout << "prep - normalized x " << data.back()->coeffRef(i) << " x: " << pt->get_x_code() << " mean_x: " << mean_x << " stddev_x: " << stddev_x << std::endl; }
		i++;
		data.back()->coeffRef(i) = Scalar((pt->get_tt_code() - mean_tt)/stddev_tt);
		if (verbose) { std::cout << "prep - normalized tt " << data.back()->coeffRef(i) << " tt: " << pt->get_tt_code() << " mean_tt: " << mean_tt << " stddev_tt: " << stddev_tt << std::endl; }
		i++;
		data.back()->coeffRef(i) = Scalar((pt->get_c_code() - mean_c)/stddev_c);
		if (verbose) { std::cout << "prep - normalized c " << data.back()->coeffRef(i) << " c: " << pt->get_c_code() << " mean_c: " << mean_c << " stddev_c: " << stddev_c << std::endl; }
	}
	
}

void PrepOutputNNTrainingData(std::vector<std::shared_ptr<TrainingData>> &odata, std::vector<RowVector*>& data)
{
	data.clear();

	int verbose = 0;
	uint cols = 1;
	// enter normalized numbers into row vectors
	for ( auto pt : odata) {
		if (verbose)
		{
			std::cout << "prep - pt " << pt << std::endl;
			std::cout << "prep - label:" << pt->label << " id: " << pt->id << std::endl;
			std::cout << "prep - volt: " << pt->get_v() << std::endl;
			std::cout << "prep - temp: " << pt->get_t() << std::endl;
			std::cout << "prep - extr: " << pt->get_x() << std::endl;
			std::cout << "prep - tt: " << pt->get_tt() << std::endl;
			std::cout << "prep - corner: " << pt->get_c() << std::endl;
		}
		data.push_back(new RowVector(1, cols));
		uint i = 0;
		data.back()->coeffRef(i) = Scalar(pt->id);
		if (verbose) { std::cout << "prep - normalized v " << data.back()->coeffRef(i) << " label: " << pt->label << " id: " << pt->id << std::endl;}
	}
	
}


// NN data
typedef std::vector<RowVector*> data;

int main()
{
	NeuralNetwork n({5, 4, 3, 1});
	std::vector <Labels> LabelData;
	std::vector <std::shared_ptr<Data>> lData;
	std::vector <std::shared_ptr<TrainingData>> tData;
	std::vector <std::shared_ptr<TrainingData>> vData;
	data in_train_data, out_train_data, in_data;
	std::vector<RowVector> out_data;

	ReadLabelData("/home/workspace/CppND-Capstone-NN/data/tags.txt", LabelData);   // label data
	// ReadData("/home/workspace/CppND-Capstone-NN/data/all_lib_files.txt", lData);   // "lib" or raw data in the case of real application
	ReadTrainingData("/home/workspace/CppND-Capstone-NN/data/training_lib.yaml", LabelData, tData);  // training data
	ReadTrainingData("/home/workspace/CppND-Capstone-NN/data/generic.lib.yaml", LabelData, vData);  // verification data, full data set, with lables to verify nn output.

	PrepInputNNData(tData, in_train_data);
	PrepOutputNNTrainingData(tData,out_train_data);
	n.train(in_train_data,out_train_data);
    std::cout << "Finished Training........" << std::endl;

	PrepInputNNData(vData,in_data);
	n.verify(vData, in_data);
    std::cout << "Finished Analysis with Verify........" << std::endl;

    //PrepInputNNData(lData,in_data);
	//out_data = n.work(in_data);
    
	
}

