#include <iostream>
#include <vector>
#include <omp.h>

#include "chrono_timer.h"


void print(const std::vector<int>& input) {
	int size = input.size();
	for (int i = 0; i < size; i++) {
		std::cout << input[i] << " ";
	}
	std::cout << std::endl;
}

std::vector<int> test_example(int n) {
	std::vector<int> sample(n);
	int j = 0;
	for (int i = n; i > 0; i--) {
		sample[j++] = i;
	}
	return sample;
}

void merge(const std::vector<int>& input, int begin, int middle, int end, std::vector<int>& output) {
	int i = begin, j = middle;
	for (int k = begin; k < end; k++) {
		if (i < middle && (j >= end || input[i] <= input[j])) {
			output[k] = input[i];
			i++;
		}
		else {
			output[k] = input[j];
			j++;
		}
	}
}

void merge_sort(std::vector<int>& input, int low, int high, std::vector<int>& output) {
	if ((high - low) < 2) {
		return;
	}
	int middle = (high + low) / 2;
	if((high-low)<1000){
		merge_sort(output, low, middle, input);
		merge_sort(output, middle, high, input);
		merge(input, low, middle, high, output);
	}else{	
		#pragma omp task shared(input, output)
		merge_sort(output, low, middle, input);
		
		#pragma omp task shared(input, output)
		merge_sort(output, middle, high, input);
		
		#pragma omp taskwait
		merge(input, low, middle, high, output);
	}
}

void copy(const std::vector<int>& input, std::vector<int>& output) {
	int size = input.size();
	for (int i = 0; i < size; i++) {
		output[i] = input[i];
	}
}

bool check_if_sorted(const std::vector<int>& input){
	int size = input.size();
	for(int i = 0; i < size-1; i++){
		if(input[i] > input[i+1])
			return false;
	}
	return true;
}

int main(int argc, char** argv) {
	
	if(argc != 2){
		std::cout << "Invalid argument!" << std::endl;
		return 1;
	}
	int n = atoi(argv[1]);
	std::vector<int> input = test_example(n);
	std::vector<int> output(n);

	copy(input, output);
	{
		ChronoTimer t("Mergesort");
		#pragma omp parallel
		{
			if(omp_get_thread_num() == 0)
				std::cout << "NUMBER of Threads: " << omp_get_num_threads() << std::endl;
			#pragma omp single
			merge_sort(input, 0, n, output);
		}
		
	}
	
	if(!check_if_sorted(output)){
		std::cout << "Sorting wrong!" << std::endl;
		return 1;
	}
	
	
	return 0;
}