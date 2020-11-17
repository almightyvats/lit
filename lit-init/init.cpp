#include "init.hpp"
#include "../lit-utils/utils.hpp"

bool create_root_dir(const fs::path &current_working_dir)
{
	bool result = false;
	std::string litFolderName = ".lit";
	std::string backupFolderName = litFolderName + "/backup";

	// creates the root directory folder
	fs::path litFolder = current_working_dir / litFolderName;
	result = fs::create_directory(litFolder);

	fs::path backupFolder = current_working_dir / backupFolderName;
	fs::create_directory(backupFolder);

	LitUtils lu(current_working_dir);
	lu.sync_info();
	lu.sync_file_status(false);

	return result;
}

bool validate_root(const fs::path &current_working_dir)
{
	if (!fs::exists(fs::path(current_working_dir / ".lit")))
		return false;

	return true;
}