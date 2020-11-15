#include "help.cpp"
#include "init.hpp"
#include "utils.hpp"
#include <iostream>
#include <string>

namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
	std::string current_exec_string;
	if (argc > 1) {
		current_exec_string = argv[1];
	} else {
		print_help_commands();
		return EXIT_FAILURE;
	}

	fs::path current_working_dir = fs::current_path();

	if (current_exec_string.compare("init") != 0 && !validate_root(current_working_dir)) {
		print_err_init_lit();
		return EXIT_FAILURE;
	}

	LitUtils lu(current_working_dir);
	if (current_exec_string.compare("init") == 0) {
		if (create_root_dir(current_working_dir)) {
			std::cout << "This directory is now the root of the repository." << std::endl;
		} else {
			std::cout << "This directory may already be a root of your repository, please check." << std::endl;
		}
	} else if (current_exec_string.compare("help") == 0) {
		print_help_commands();
	} else if (current_exec_string.compare("status") == 0) {
		if (!lu.status()) {
			print_err_status();
		}
	} else if (current_exec_string.compare("commit") == 0) {
		if (argc > 2) {
			std::string commit_msg = argv[2];
			if (!lu.commit(commit_msg)) {
				print_err_init_lit();
			}
		} else {
			print_err_invalid_option("commit");
		}
	} else if (current_exec_string.compare("checkout") == 0) {
		std::string commit_no = "";
		if (argc > 2) {
			commit_no = argv[2];
		}
		if (!lu.checkout(commit_no)) {
			print_err_invalid_commit();
		}
	} else if (current_exec_string.compare("show") == 0) {
		std::string commit_no = "";
		if (argc > 2) {
			commit_no = argv[2];
			if (!lu.show(commit_no)) {
				print_err_invalid_commit();
			}
		} else {
			print_err_invalid_option("show");
		}
	} else if (current_exec_string.compare("merge") == 0) {
		std::string commit_no = "";
		if (argc > 2) {
			commit_no = argv[2];
			if (!lu.merge(commit_no)) {
				print_err_invalid_commit();
			}
		} else {
			print_err_invalid_option("merge");
		}
	} else if (current_exec_string.compare("log") == 0) {
		if (!lu.log()) {
			print_err_init_lit();
		}
	} else {
		std::cout << "Enter a valid command\ncheck usage: lit help" << std::endl;
	}

	return EXIT_SUCCESS;
}