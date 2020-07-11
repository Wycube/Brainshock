/*
 * Copyright (c) 2019 Spencer Burton
 */

#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <unordered_map>

#include "Brainf.hpp"

//Semantic Versioning
const int major = 0;
const int minor = 2;
const int patch = 0;

static std::unordered_map<std::string, int> strToNum = {
	{"-help", 0}, {"h", 0},    //Print out the usage message
	{"-version", 1}, {"v", 1}, //Print out version info
	{"b", 2},                  //Print out runtime after execution
	{"p", 3}                   //Processes the program before it's ran
};

static struct {
	bool flags[4] = {false};
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

/*
 * A basic REPL with the exit being the word exit only, as input.
 * This does not pre-process the input string, but just runs it 
 * directly in the interpreter.
 */
void evalLoop(bs::BrainfInterpreter &interpreter) {
	std::string input;

	while(true) {
		std::cout << ":"; //This symbol is arbitrary I just needed something thats not a brainf*** instruction

		std::getline(std::cin, input);

		if(input == "exit")
			return;

		interpreter.loadProgram(input.c_str(), false);
		if(!interpreter.run())
		       std::cout << "An error occurred" << std::endl;

		std::cout << std::endl;	
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
		<< " -h --help    Display this help message\n"
		<< " -v --version Display version info\n"
		<< std::endl
		<< " -b           Display the program's runtime after execution\n"
		<< " -p           Processes/Optimizes the program to an IR before it's ran"
		<< std::endl;

		return 0;
	} else if(options.flags[1]) {
		std::cout << "Brainshock, Brainf*** Interpreter\n"
		<< "Copyright (c) 2019 Spencer Burton\n"
		<< "Version: " << major << '.' << minor << '.' << patch
		<< std::endl;

		return 0;
	}

	//Create Interpreter for next steps
	bs::BrainfInterpreter interpreter = bs::BrainfInterpreter();

	//Go into interactive mode
	if(options.repl) {
		evalLoop(interpreter);
		return 0;
	}
}
