/*
 * Copyright (c) 2019 Spencer Burton
 */

#include "interpreters/HWOI/HWOInterpreter.hpp"
#include <iostream>
#include <ctime>

//Semantic Versioning
int major = 0;
int minor = 1;
int patch = 0;

int main() {
	std::cout << "Copyright (c) 2019 Spencer Burton" << std::endl;
	std::cout << "Brainshock Alpha v" << major << '.' << minor << '.' << patch << std::endl;
	
	bs::frick::StdHWInterpreter interp = bs::frick::StdHWInterpreter();

	std::cout << "Input Program:";
	
	std::string program;

	std::cin >> program;

	if(interp.loadProgram(program.c_str())) {
		std::cout << "successfully loaded program" << std::endl;
	} else {
		std::cout << "Error in program" << std::endl;
		exit(1);
	}

	//Benchmarking
	std::clock_t start;

	start = clock();

	//Run
	interp.run();

	double duration = (clock() - start) / (double) CLOCKS_PER_SEC;

	std::cout << "finished in " << CLOCKS_PER_SEC << " seconds" << std::endl;
}