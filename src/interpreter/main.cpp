/*
 * Copyright (c) 2019 Spencer Burton
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <string>
#include <unordered_map>

#include "Brainf.hpp"

//Semantic Versioning
const int major = 0;
const int minor = 3;
const int patch = 0;

//---------- Command Line / Args ----------//

static std::unordered_map<std::string, int> strToNum = {
	{"-help",    0}, {"h", 0}, //Print out the usage message
	{"-version", 1}, {"v", 1}, //Print out version info
	{"p",  2},                 //Processes the program before it's ran
	{"O1", 3},                 //Optimizes the processed program significantly
	{"O2", 4},                 //Optimizes the processed program a bit more
	{"b",  5},                 //Print out runtime after execution
	{"md", 6},                 //Prints a dump of the entire memory
	{"mp", 7},                 //Prints the current cell and some around it
	{"pg",  8}                 //Prints the program thats going to be run, it would only be different if processed
};

static struct {
	bool flags[9] = {false};
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


//---------- REPL Stuff ----------//

//This will tell the program which commands are set or input
static struct CommandFlags {
	bool exit = false; //Exits the REPl
	bool help = false; //Displays a help message showing all commands
	bool prog = false; //Displays the program that is being interpreted after processing and such
	bool dump = false; //Dumps the memory in a certain base
	bs::DUMP_BASE dumpBase;
	bs::DUMP_BASE setBase;
	bool time = false; //Prints the amount of time the interpreter took to run the last program
	bool mem = false;  //Prints the current cells value and some around it, in decimal
	bool set[4] = { false };

	void clear() {
		exit = false;
		help = false;
		prog = set[0];
		dump = set[1];
		time = set[2];
		mem  = set[3];

		if(set[1])
			dumpBase = setBase;
	}
} comflags;

//A helper function for finding out if and which command was inputted
bool parseCommand(std::string str) {
	std::string input = str;
	bool set = false, unset = false;

	//Check for set
	if(str.substr(0, 4) == "set ") {
		input = str.substr(4);
		set = true;
	} else if(str.substr(0, 6) == "unset ") {
		input = str.substr(6);
		unset = true;
	}


	if(input == "exit")           comflags.exit = true && !unset; //Can't set exit or help, don't be absurd
	else if(input == "help")      comflags.help = true && !unset; //AND true with !unset to not run if unsetting

	else if(input == "prog")    { comflags.prog = true && !unset;
	       			      comflags.set[0] = set ? true : unset ? false : comflags.set[0]; }

	else if(input == "hexdump") { comflags.dump = true && !unset;
	       			      comflags.dumpBase = bs::BASE_HEX;
				      if(set) comflags.setBase = bs::BASE_HEX;
				      comflags.set[1] = set ? true : unset ? false : comflags.set[1]; }
	else if(input == "decdump") { comflags.dump = true && !unset; 
				      comflags.dumpBase = bs::BASE_DEC;
				      if(set) comflags.setBase = bs::BASE_DEC;
				      comflags.set[1] = set ? true : unset ? false : comflags.set[1]; }
	else if(input == "bindump") { comflags.dump = true && !unset;
	       		       	      comflags.dumpBase = bs::BASE_BIN;
				      if(set) comflags.setBase = bs::BASE_BIN; 
				      comflags.set[1] = set ? true : unset ? false : comflags.set[1]; }

	else if(input == "time")    { comflags.time = true && !unset;
	       			      comflags.set[2] = set ? true : unset ? false : comflags.set[2]; }

	else if(input == "mem")     { comflags.mem = true && !unset;
	      		     	      comflags.set[3] = set ? true : unset ? false : comflags.set[3]; }
	else return false;

	return true;
}

//Prints whatever the set commands are suppose to
void printInfo(bs::BrainfInterpreter &interpreter, std::chrono::microseconds runtime) {
	bs::Program &program = interpreter.getProgram();

	//Help message
	if(comflags.help) {
		std::cout << " Commands will print useful information about the state"
			  << " of the interpreter.\n"
			  << " Commands can be set to run everytime with \"set [command]\""
			  << " and \"unset [command]\".\n\n"  
			  << " help - Prints this help message\n"
			  << " exit - Exits the REPL\n"
			  << " prog - Prints the programs instructions, only useful with optimizing\n"
			  << " dump - Prints entire memory, append base \"hexdump bindump decdump\"\n"
			  << " mem  - Prints the memory at and around the current cell\n"
			  << " time - Prints the runtime of the last program"
			  << std::endl;

		return; //Don't do anything else
	} 

	if(comflags.prog) {
		std::cout << "Program: ";
		
		std::size_t length = program.processed ? program.tokens.size() : program.program.size();
		for(std::size_t i = 0; i < length; i++)
			std::cout << program[i] << (program.processed ? program.tokens[i].data : '\0');
		std::cout << std::endl;
	} 
	if(comflags.dump)
		interpreter.getMemory().fDump(comflags.dumpBase, true); //I'll just keep the ascii on
	if(comflags.mem)
		interpreter.getMemory().fPrint(interpreter.getDataPtr());
	if(comflags.time)
		std::cout << "Finished in " << static_cast<double>(runtime.count() / 1000.0) << "ms or " << static_cast<double>(runtime.count() / 1000000.0)<< "s" << std::endl; //Divide by a thousand for milliseconds and a million for seconds
}

/*
 * A basic REPL, with the commands aswell.
 */
void evalLoop(bs::BrainfInterpreter &interpreter, std::stringbuf &buffer) {
	std::string input;
	std::chrono::microseconds delta;

	unsigned int optLevel = options.flags[4] ? 2 : options.flags[3] ? 1 : 0; //Optimization level, 2, 1, or 0(none)

	//Some flags set commands
	if(options.flags[5]) //-b setting the "time" command
	       comflags.set[2] = true;	
	if(options.flags[6]) //-md setting the "dump" command
		comflags.set[1] = true;
	if(options.flags[7]) //-mp setting the "mem" command
		comflags.set[3] = true;
	if(options.flags[8]) //-p setting the "prog" command
		comflags.set[0] = true;
	comflags.clear(); //Just to reset the flags at the beginning	

	//Check for unused flags and warn
	//-O1 and/or -O2 if -p is not set
	if((options.flags[3] || options.flags[4]) && !options.flags[2]) {
		std::cerr << "Warning: ";
		if(options.flags[3]) std::cerr << "-O1 ";
		if(options.flags[4]) std::cerr << "-O2 ";
		std::cerr << "unused" << std::endl;
	}


	while(true) {
		std::cout << ": "; //This symbol is arbitrary I just needed something thats not a brainf*** instruction
		std::getline(std::cin, input);

		//Command stuff, if it's not a command run it in the interpreter
		if(parseCommand(input) || input == "") {
			if(comflags.exit)
				return;

		//-p Process the program if set 
		//-O1 and -O2 Specific optimizations
		} else if(!interpreter.loadProgram(input.c_str(), options.flags[2], false, optLevel)) {
			std::cerr << "Error: " << interpreter.getError() << std::endl;
		} else {
			//Timing Start
			auto start = std::chrono::steady_clock::now();
			

			if(!interpreter.run())
				std::cerr << "Error: " << interpreter.getError() << std::endl;


			//Timing End
			auto end = std::chrono::steady_clock::now();
			delta = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		}

		//Determine if the program output anything and clear the buffer
		if(buffer.str() != "") {
			std::cout << buffer.str() << std::endl;
			buffer.str("");
		}

		//Print info from commands then clear them
		printInfo(interpreter, delta);
		comflags.clear();
	}
}


//---------- Main ---------//

int main(int argc, char *argv[]) {
	//Set flags and stuff
	parseArgs(argc, argv);

	//Should it print help or version info
	if(options.flags[0]) {
		std::cout << "Brainshock, Brainf*** Interpreter\n\n"
		<< "Usage: " << argv[0] << " [options...] [source_file]\n\n"
		<< "Options:\n"
		<< " -h --help    Display this help message\n"
		<< " -v --version Display version info\n"
		<< std::endl
		<< " -p           Processes/Optimizes the program to an IR before it's ran\n"
		<< " -O1          Optimizes the processed program significantly\n"
		<< " -O2          Optimizes the processed program a bit more past O1\n"
		<< " -b           Display the program's run time after execution\n"
		<< " -md          Display a dump of the entire memory after execution\n"
		<< " -mp          Display the current cell and a few around it after execution"
		<< " -pg          Display the program that was interpreted(should only change if preprocessed)"
		<< std::endl;

		return 0;
	} else if(options.flags[1]) {
		std::cout << "Brainshock, Brainf*** Interpreter\n"
		<< "Copyright (c) 2019 Spencer Burton\n"
		<< "Version: " << major << '.' << minor << '.' << patch
		<< std::endl;

		return 0;
	}

	//Go into interactive mode
	if(options.repl) {
		std::stringbuf buffer;
		std::ostream stream(nullptr);
		stream.rdbuf(&buffer);
		bs::BrainfInterpreter interpreter = bs::BrainfInterpreter(stream);

		evalLoop(interpreter, buffer);
		return 0;
	} else {
		bs::BrainfInterpreter interpreter = bs::BrainfInterpreter(std::cout);
		std::ifstream file(options.path);

		//Check for unused flags and warn
		//-O1 and/or -O2 if -p is not set
		if((options.flags[3] || options.flags[4]) && !options.flags[2]) {
			std::cerr << "Warning: ";
			if(options.flags[3]) std::cerr << "-O1 ";
			if(options.flags[4]) std::cerr << "-O2 ";
			std::cerr << "unused" << std::endl;
		}

		//Check for file validity
		if(!std::filesystem::exists(options.path)) {
			std::cerr << "Error: File provided does not exist" << std::endl;
			return 3;
		} else if(!file.good()) {
			std::cerr << "Error: Could not access file" << std::endl;
			return 3;
		}

		//Read in program from the file
		std::stringstream buffer;
		buffer << file.rdbuf();
		file.close();

		unsigned int optLevel = options.flags[4] ? 2 : options.flags[3] ? 1 : 0;
		std::chrono::microseconds delta;
		
		//-p should the program be preprocessed
		if(!interpreter.loadProgram(buffer.str().c_str(), options.flags[2], true, optLevel)) {
			std::cerr << "Error :" << interpreter.getError() << std::endl;
			return 4;
		} else {
			//Timing start
			auto start = std::chrono::steady_clock::now();	
		

			if(!interpreter.run()) {
				std::cerr << "Error: " << interpreter.getError() << std::endl;		
				for(size_t i = interpreter.getInstPtr() - 30; i < interpreter.getInstPtr() + 30; i++) {
					std::cout << interpreter.getProgram()[i];
				}
			}

			//Timing end
			auto end = std::chrono::steady_clock::now();
			delta = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		}

		//Use the command struct and functions to print the information
		comflags.time = options.flags[5];
		comflags.dump = options.flags[6];
		comflags.mem  = options.flags[7];
		comflags.prog = options.flags[8];

		printInfo(interpreter, delta);
	}
}
