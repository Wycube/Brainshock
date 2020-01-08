/*
 * Copyright (c) 2019 Spencer Burton
 */

#include "interpreters/HWI/HWInterpreter.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>

//Semantic Versioning
int major = 0;
int minor = 1;
int patch = 3;

int main(int argc, char *argv[]) {
	std::cout << "Copyright (c) 2019 Spencer Burton" << std::endl;
	std::cout << "Brainshock Alpha v" << major << '.' << minor << '.' << patch << std::endl;
	
	bs::frick::StdHWInterpreter interp(1);

	interp.m_flags.numInput = true;

	bool useFile = false;

	char result;

	//Prompt whether to use a file for program
	while(true) {
		if(argc > 1) {
			useFile = true;
			break;
		}

		std::cout << "File y/n:" << std::endl;

		std::cin >> result;

		if(result == 'y') { 
			useFile = true;
			break;
		} else if(result == 'n') {
			useFile = false;
			break;
		}

	}

	std::string program;

	//Get path of file and load it int 
	if(useFile) {
		if(argc < 2)
			std::cout << "Input File:";

		std::string path;

		if(argc < 2)
			while(path == "")
				std::getline(std::cin, path);

		if(argc > 1)
			path = argv[1];

		std::ifstream file(path);

		if(file.is_open()) {
			int length;

			file.seekg(0, std::ios::end);
			length = file.tellg();
			file.seekg(0, std::ios::beg);

			char data[length];

			file.read(data, length);

			program = std::string(data);

			file.close();
		} else {
			std::cout << "Unable to open file" << std::endl;
			exit(1);
		}
	} else {
		std::cout << "Input Program:";

		while(program == "")
			std::getline(std::cin, program);
	}

	if(interp.loadProgram(program.c_str())) {
		std::cout << "successfully loaded program" << std::endl;
	} else {
		std::cout << "Error in program" << std::endl;
		exit(1);
	}

	//Benchmarking
	auto start = std::chrono::steady_clock::now();

	//Run
	interp.run();

	//Benchmarking
	auto end = std::chrono::steady_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);

	interp.m_tape.printTape(interp.m_cellPtr); 

	std::cout << "finished in " << duration.count() << " seconds" << std::endl;
}