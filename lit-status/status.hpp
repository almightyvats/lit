#ifndef STATUS_HPP
#define STATUS_HPP
#include <filesystem>
#include <map>
#include <string>
#include <vector>

class LitStatus {
  private:
	std::vector<std::string> m_current_working_files;
	std::vector<std::string> m_backup_files;
	std::filesystem::path m_current_working_dir;
	std::map<std::string, std::string> m_status;
	std::vector<std::string> m_recently_added_files;
	std::vector<std::string> m_empty_files;

	std::string m_backup_folder;

  public:
	LitStatus(std::filesystem::path current_working_dir);
	~LitStatus() {}

	void check_for_added_or_modified(std::vector<std::string> cwd_filepaths, std::vector<std::string> backup_filepaths);
	std::string check_for_deleted(std::vector<std::string> cwd_filepaths, std::vector<std::string> backup_filepaths);
	std::string polish_for_cwd(std::string file_path);
	std::string polish_for_backup(std::string file_path);
	bool compare_two_files(const std::string &file1, const std::string &file2);
	bool is_anything_modified();
	std::vector<std::string> get_recently_added_files();
	void clear_status();
	std::vector<std::string> get_empty_files();
	void print_status();
	bool is_file_empty(const std::string &file);
};

#endif