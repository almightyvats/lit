#ifndef HELP_CPP
#define HELP_CPP

#include <iostream>
#include <string>

void print_err_invalid_option(std::string option)
{
	if (option == "commit") {
		std::cout << "usage: lit commit <message>" << std::endl;
		return;
	}

	std::cout << "usage: lit " + option + " <commit number>" << std::endl;
}

void print_err_invalid_commit()
{
	std::cout << "Wrong commit number specified\nsee: lit log" << std::endl;
}

void print_err_init_lit()
{
	std::cout << "Not a root for lit\nUse lit init to initialise" << std::endl;
}

void print_err_status()
{
	std::cout << "No changes yet!" << std::endl;
}

void print_help_commands()
{
	std::string helpCommands[7] = {"init", "status", "commit", "show", "checkout", "merge", "log"};
	std::string helpCommanddescription[7] = {
	    "This command initialises the current directory as root of lit",
	    "Lists of all files that have been added, removed, or modified, with respect to the currently checked out "
	    "commit",
	    "Creates a new commit containing all changes",
	    "This sub-command is used to inspect the given commit",
	    "This resets the state of all files to the given commit's state",
	    "This command initiates a merge with the currently checked out commit and the specified commit",
	    "Displays a graph of all commits and their relationship"};

	std::cout << "Commands"
	          << "\t"
	          << "\t"
	          << "\t"
	          << "\t"
	          << "Decription" << std::endl
	          << std::endl;

	for (int i = 0; i < 7; i++) {
		std::cout << helpCommands[i] << "\t"
		          << "\t";
		if (i != 4)
			std::cout << "\t";
		std::cout << "\t" << helpCommanddescription[i] << std::endl;
	}
}

#endif