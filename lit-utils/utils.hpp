#ifndef UTILS_HPP
#define UTILS_HPP

#include "../lit-status/status.hpp"
#include <filesystem>
#include <set>
#include <string>
#include <vector>

class LitUtils {
  private:
	std::filesystem::path m_current_working_dir;
	std::vector<std::string> m_vec_listoffiles;
	std::string m_commit_dir;
	std::string m_backup_dir;
	std::string m_file_list;
	std::string m_info_file;
	std::string m_conflict_file;
	std::string m_lit_folder;
	LitStatus *m_litStatus;

  public:
	LitUtils(std::filesystem::path current_working_dir);
	~LitUtils();

	bool commit(std::string commit_msg, bool for_merge = false);
	bool checkout(std::string commit_no, bool for_merge = false);
	bool show(std::string commit_no);
	bool merge(std::string commit_no);
	bool log();
	bool status();

	void create_new_commit_dir(std::string commit_target_dest, std::string new_commit_folder_name, int temp_commit_no,
	                           int temp_lastcommit_no, std::string last_checkout_number, std::string commit_msg,
	                           bool for_merge);
	bool patch_root(const std::string commit_no);
	bool patch_root_recursive(const int upto_commit_no);
	std::string get_deleted_file_name();

	void iterate_root_repository(std::vector<std::string> &list_of_files);
	void iterate_any_repository(std::filesystem::path dir, std::vector<std::string> &list_of_files);
	bool is_file_hidden(const std::filesystem::path &p);

	void sync_backup_folder();
	void sync_file_status(bool for_commit);
	void sync_info();
	void sync_checkout(std::filesystem::path dir, const std::vector<std::string> &vec_listoffiles);
	void update_checkout_status(int commit_no);
	bool merge_two_files(const std::string &file1, const std::string &file2);

	void get_info(std::string &last_commit_no, std::string &last_checkout_no);
	std::filesystem::path get_current_working_dir();
	std::vector<std::string> get_root_repo_list();
	void set_root_repo_list(std::vector<std::string> vec_root_repo_list);

	void add_rev_name_to_all_files(const std::vector<std::string> &vec_listoffiles, std::string rev_no);
	void polish_file_name(const std::string &file_path, const std::string &to_find);
	int read_file_for_int(const std::string &file_path);
	void check_for_parent(const std::string &file_path, const int curr_branch, std::set<int> &branches,
	                      int &final_parent);
	void clear_root();
	bool validate_commit_no(const std::string commit_no, int last_commit_no);
	std::vector<std::string> get_branch_files(std::string commit_no);
	std::vector<std::string> check_for_adding_conflict(std::vector<std::string> new_files,
	                                                   std::vector<std::string> branch_files);
	bool is_current_state_mergable();
	bool is_anything_modified();
	void create_empty_files(std::string commit_dir);
	void sync_backup_in_merge(std::string last_checkout, std::vector<std::string> conflicted_files);
};

#endif