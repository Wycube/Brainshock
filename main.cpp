/*
 * Copyright (c) 2019 Spencer Burton
 */

#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <unordered_map>

#include "src/include/Brainf.hpp"

//Semantic Versioning
const int major = 0;
const int minor = 2;
const int patch = 0;

static std::unordered_map<std::string, int> strToNum = {
	{"-help", 0}, {"h", 0},    //Print out the usage message
	{"-version", 1}, {"v", 1}, //Print out version info
	{"b", 2}                   //Print out runtime after execution
};

static struct {
	bool flags[3] = {false};
	std::string path = "";
	bool repl = true;
} options;

/*
 * Checks if a commandline argument is a valid option, 
 * and sets it if such.
 */
bool isOption(const std::string &option) {
	if(strToNum.count(option)) {
		options.flags[strToNum[option]] = true;
		return true;
	}

	return false;
}

/*
 * Loops through cl arguments to find valid options,
 * a source file, and invalid args.
 */
void parseArgs(int argc, char *argv[]) {
	//It starts at 1 because 0 is just the program name
	if(argc < 2) {
		return;
	}

	//Go through args for flags and the source file path
	for(size_t i = 1; i < argc; i++) {
		if(argv[i][0] == '-') {
			if(!isOption(std::string(argv[i]).substr(1))) {
				std::cout << "Error: " << argv[i] << " is not a valid option." << std::endl;
				exit(1);
			}
		} else {
			if(!options.path.empty()) {
				std::cout << "Error: Multiple source files provided only one expected." << std::endl;
				exit(2);
			} else {
				options.path = argv[i];
				options.repl = false;
			}
		}
	}
}


int main(int argc, char *argv[]) {
	//Set flags and stuff
	parseArgs(argc, argv);

	//Should it print help or version info
	if(options.flags[0]) {
		std::cout << "Brainshock, Brainf*** Interpreter\n\n"
		<< "Usage: " << argv[0] << " [options] [source_file]\n\n"
		<< "Options:\n"
		<< " -h --help    Prints this help message"
		<< " -v --version Prints verision info"
		<< " -b           Times the programs runtime and prints it after execution"
		<< std::endl;

		exit(0);
	} else if(options.flags[1]) {
		std::cout << "Brainshock, Brainf** Interpreter\n"
		<< "Copyright (c) 2019 Spencer Burton"
		<< "Version: " << major << '.' << minor << '.' << patch
		<< std::endl;

		exit(0);
	}




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
			return 1;
		}
	}
}