#include "utils.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>

namespace fs = std::filesystem;
using string = std::string;

void LitUtils::sync_checkout(std::filesystem::path dir, const std::vector<std::string> &vec_listoffiles)
{
	for (auto &p : fs::directory_iterator(dir)) {
		if (fs::is_directory(p.path())) {
			if (p.path().filename().string() == ".lit")
				continue;
			sync_checkout(p.path(), vec_listoffiles);
		} else {
			if (std::find(vec_listoffiles.begin(), vec_listoffiles.end(), p.path().string()) != vec_listoffiles.end()) {
				continue;
			} else {
				fs::remove(p.path());
			}
		}
		if (fs::is_directory(p.path())) {
			if (fs::is_empty(p.path())) {
				fs::remove(p.path());
			}
		}
	}
}

void LitUtils::iterate_root_repository(std::vector<string> &list_of_files)
{
	std::vector<string> l_list_of_files;
	fs::path current_working_dir = get_current_working_dir();

	for (auto p = fs::recursive_directory_iterator(current_working_dir); p != fs::recursive_directory_iterator(); ++p) {
		const auto complete_file_path = p->path().string();
		if (is_file_hidden(p->path()))
			continue;

		if (fs::is_regular_file(p->path())
		    && (p->path().string().find("/.lit") != string::npos || p->path().string().find(".vsc") != string::npos)) {
			continue;
		}
		l_list_of_files.push_back(p->path().string());
	}

	for (auto x : l_list_of_files) {

		if (x.find(m_lit_folder) != std::string::npos) {
			continue;
		}
		list_of_files.push_back(x);
	}
}

std::vector<string> LitUtils::get_root_repo_list()
{
	return m_vec_listoffiles;
}

void LitUtils::set_root_repo_list(std::vector<string> vec_root_repo_list)
{
	m_vec_listoffiles = vec_root_repo_list;
}

void LitUtils::clear_root()
{
	for (auto &p : fs::directory_iterator(get_current_working_dir())) {
		if (fs::is_directory(p.path())) {
			if (p.path().filename().string() == ".lit")
				continue;
			fs::remove_all(p.path());
		} else {
			fs::remove(p.path());
		}
	}
}

void LitUtils::iterate_any_repository(std::filesystem::path dir, std::vector<std::string> &list_of_files)
{
	for (auto p = fs::recursive_directory_iterator(dir); p != fs::recursive_directory_iterator(); ++p) {
		if (is_file_hidden(p->path()))
			continue;
		list_of_files.push_back(p->path().string());
	}
}

bool LitUtils::is_file_hidden(const fs::path &p)
{
	const fs::path::string_type name = p.filename();
	if ((name != ".." && name != "." && name[0] == '.') || (name.substr(1, name.length()) == ".")) {
		return true;
	}
	return false;
}

void LitUtils::sync_file_status(bool for_a_commit)
{
	std::ofstream myfiles_self(m_file_list);

	string last_commit_number;
	string last_checkout_number;
	get_info(last_commit_number, last_checkout_number);

	myfiles_self.clear();

	std::vector<string> files = get_root_repo_list();

	for (auto &f : files) {
		myfiles_self << f << "\n";
	}

	myfiles_self.close();
	if (for_a_commit) {
		string last_commit_folder_name = "r" + last_commit_number;
		string last_commit_folder_path = m_commit_dir + "/" + last_commit_folder_name;
		if (fs::exists(last_commit_folder_path)) {
			fs::copy(m_file_list, last_commit_folder_path);
		}
	}
}

void LitUtils::sync_backup_folder()
{
	fs::remove_all(fs::path(m_backup_dir));

	if (!fs::exists(fs::path(m_backup_dir))) {
		fs::create_directories(m_backup_dir);
	}
	std::vector<string> files = get_root_repo_list();

	fs::copy(get_current_working_dir(), m_backup_dir,
	         fs::copy_options::overwrite_existing | fs::copy_options::recursive);

	fs::remove_all(fs::path(m_backup_dir + "/.lit"));
	if (!fs::exists(fs::path(m_backup_dir + "/.lit"))) {
		fs::remove(fs::path(m_backup_dir + "/.lit"));
	}
}

void LitUtils::sync_backup_in_merge(std::string last_checkout, std::vector<std::string> conflicted_files)
{
	getchar();
	string path;
	std::ifstream f_v_f1;
	f_v_f1.open(m_commit_dir + "/r" + last_checkout + "/files");
	while (getline(f_v_f1, path)) {
		string localpath = path;
		string polished_file_name = localpath.erase(0, m_current_working_dir.string().length());
		string backup_file_path = m_backup_dir + polished_file_name;

		if (fs::is_directory(backup_file_path))
			continue;

		string copythis = path;
		bool is_conflicted = false;
		if (std::find(conflicted_files.begin(), conflicted_files.end(), path) != conflicted_files.end()) {
			is_conflicted = true;
			copythis = path + ".r" + last_checkout;
		}
		if (fs::exists(fs::path(copythis))) {
			fs::copy(copythis, backup_file_path, fs::copy_options::recursive | fs::copy_options::update_existing);
		}
	}
}

void LitUtils::sync_info()
{
	fs::path current_working_dir = get_current_working_dir();
	int last_commit_no = -1;
	for (auto &p : fs::directory_iterator(current_working_dir / ".lit")) {
		if (p.path().filename().string() == "commits") {
			for (auto &c : fs::directory_iterator(m_commit_dir)) {
				if (is_file_hidden(c.path()))
					continue;

				string rev_numbers_string = c.path().filename().string();
				int commit_no = std::stoi(rev_numbers_string.substr(rev_numbers_string.find("r") + 1));
				last_commit_no = commit_no > last_commit_no ? commit_no : last_commit_no;
			}
		}
	}
	// syncing commit status
	std::ofstream myinfo_self(m_info_file);
	myinfo_self.clear();
	myinfo_self << std::to_string(last_commit_no) << "\n";
	myinfo_self << std::to_string(last_commit_no);
	myinfo_self.close();
}

void LitUtils::get_info(std::string &last_commit_no, std::string &last_checkout_no)
{
	std::ifstream myinfo(m_info_file);
	string last_commit_number_str;
	string last_checkout_number_str;
	getline(myinfo, last_commit_no);
	getline(myinfo, last_checkout_no);
	myinfo.close();
}

void LitUtils::update_checkout_status(int commit_no)
{
	std::fstream myinfo;
	string last_commit_number_str;
	string last_checkout_number_str;
	myinfo.open(m_info_file);
	getline(myinfo, last_commit_number_str);
	getline(myinfo, last_checkout_number_str);
	myinfo.close();
	fs::remove(m_info_file);
	myinfo.open(m_info_file, std::ios::out);
	myinfo << last_commit_number_str << "\n";
	myinfo << std::to_string(commit_no);
	myinfo.close();
}

bool LitUtils::merge_two_files(const std::string &file1, const std::string &file2)
{
	std::vector<string> v_f1;
	string s1;
	std::vector<string> v_f2;
	string s2;
	std::ifstream f_v_f1;
	f_v_f1.open(file1);
	while (getline(f_v_f1, s1)) {
		v_f1.push_back(s1);
	}
	f_v_f1.close();
	std::ifstream f_v_f2;
	f_v_f2.open(file2);
	while (getline(f_v_f2, s2)) {
		v_f2.push_back(s2);
	}
	f_v_f2.close();

	std::vector<string> v_new;
	bool files_conflicted = false;

	int size_for_iteration = v_f1.size() < v_f2.size() ? v_f1.size() : v_f2.size();
	int i;
	for (i = 0; i < size_for_iteration; i++) {
		if (v_f1[i] == v_f2[i]) {
			v_new.push_back(v_f1[i]);
		} else {
			files_conflicted = true;
			v_new.push_back("<<<<<<<<<< " + fs::path(file1).filename().string());
			v_new.push_back(v_f1[i]);
			v_new.push_back("========== ");
			v_new.push_back(v_f2[i]);
			v_new.push_back(">>>>>>>>>> " + fs::path(file2).filename().string());
		}
	}
	if (size_for_iteration != v_f1.size()) {
		for (int j = i; j < v_f1.size(); j++) {
			v_new.push_back(v_f1[j]);
		}
	} else {
		for (int j = i; j < v_f2.size(); j++) {
			v_new.push_back(v_f2[j]);
		}
	}
	if (!files_conflicted) {
		fs::remove(file1);
		fs::remove(file2);
	}

	string new_file_name = files_conflicted ? file1 + "_mc" : file1;
	std::ofstream new_file;
	new_file.open(new_file_name);
	for (auto x : v_new) {
		new_file << x << "\n";
	}
	new_file.close();

	if (files_conflicted) {
		return false;
	}

	return true;
}

void LitUtils::add_rev_name_to_all_files(const std::vector<string> &vec_listoffiles, string rev_no)
{
	for (const auto &file : vec_listoffiles) {
		if (fs::exists(fs::path(file))) {
			if (fs::is_directory(fs::path(file)))
				continue;

			fs::rename(fs::path(file), fs::path(file + "." + rev_no));
		}
	}
}

void LitUtils::polish_file_name(const std::string &file_path, const std::string &to_find)
{
	size_t pos = file_path.find(to_find);
	string temp_orig;
	if (pos != std::string::npos) {
		temp_orig = file_path;
		temp_orig.erase(pos, (to_find).length());
		fs::rename(fs::path(file_path), fs::path(temp_orig));
	}
}

void LitUtils::check_for_parent(const std::string &file_path, const int curr_branch, std::set<int> &branches,
                                int &final_parent)
{
	branches.insert(curr_branch);
	final_parent = read_file_for_int(file_path);
	string parent_commit_dir = m_commit_dir + "/r" + std::to_string(final_parent) + "/brch";
	if (fs::exists(fs::path(parent_commit_dir))) {
		check_for_parent(parent_commit_dir, final_parent, branches, final_parent);
	}
}

int LitUtils::read_file_for_int(const std::string &file_path)
{
	int res = -1;
	std::ifstream myinfo(file_path);
	string num;
	getline(myinfo, num);
	myinfo.close();
	if (!num.empty()) {
		res = std::stoi(num);
	}
	return res;
}

void LitUtils::create_new_commit_dir(string commit_target_dest, string new_commit_folder_name, int temp_commit_no,
                                     int temp_lastcommit_no, string last_checkout_number, string commit_msg,
                                     bool for_merge)
{
	fs::create_directories(commit_target_dest);
	std::fstream commit_msg_file;
	commit_msg_file.open(commit_target_dest + "/" + "msg", std::ios::out);
	commit_msg_file << "Commit: " << new_commit_folder_name << "\n";
	std::cout << "Commit: " << new_commit_folder_name << "\n";
	if (temp_commit_no < 0) {
		commit_msg_file << "Parent: none"
		                << "\n";
	} else {
		commit_msg_file << "Parent: r" << last_checkout_number << "\n";
	}
	time_t timetoday;
	time(&timetoday);

	commit_msg_file << "Date: " << asctime(localtime(&timetoday)) << "\n";
	std::cout << "Date: " << asctime(localtime(&timetoday)) << "\n";
	commit_msg_file << commit_msg << "\n";
	commit_msg_file << get_deleted_file_name();
	commit_msg_file.close();

	std::fstream branch_file;
	if (temp_lastcommit_no != temp_commit_no) // revisit this logic - check for if either folders are a branch
	{
		if (!for_merge) {
			branch_file.open(commit_target_dest + "/" + "brch", std::ios::out);
		} else {
			branch_file.open(commit_target_dest + "/" + "mrgd", std::ios::out);
		}
	} else if (for_merge) {
		branch_file.open(commit_target_dest + "/" + "mrgd", std::ios::out);
	} else {
		branch_file.open(commit_target_dest + "/" + "cpar", std::ios::out);
	}
	if (temp_commit_no < 0) {
		branch_file << temp_commit_no;
	} else {
		branch_file << last_checkout_number;
	}
	if (branch_file.is_open()) {
		branch_file.close();
	}

	std::vector<string> empty_files = m_litStatus->get_empty_files();
	if (empty_files.size() > 0) {
		std::fstream empty_file_list;
		empty_file_list.open(commit_target_dest + "/" + "empfiles", std::ios::out);
		for (const auto &file_name : empty_files) {
			empty_file_list << file_name + "\n";
		}
		if (empty_file_list.is_open()) {
			empty_file_list.close();
		}
	}
}

string LitUtils::get_deleted_file_name()
{
	std::vector<string> list_of_files_cwd;
	iterate_root_repository(list_of_files_cwd);
	set_root_repo_list(list_of_files_cwd);
	std::vector<string> list_of_file_backup;
	iterate_any_repository(m_backup_dir, list_of_file_backup);

	return m_litStatus->check_for_deleted(list_of_files_cwd, list_of_file_backup);
}

bool LitUtils::validate_commit_no(const std::string commit_no, int last_commit_no)
{
	string commit;
	for (int i = 0; i < commit_no.size(); i++) {
		if (i == 0) {
			if (commit_no.at(i) != 'r')
				return false;
			continue;
		} else if (commit_no.at(i) == 'r') {
			return false;
		}

		commit += commit_no.at(i);
	}
	if (commit.size() > 0) {
		if (last_commit_no < std::stoi(commit))
			return false;
	} else {
		return false;
	}

	return true;
}

std::vector<string> LitUtils::get_branch_files(string commit_no)
{
	std::vector<string> branch_files_list;
	string s1;
	fs::path branch_files_path = fs::path(m_commit_dir + "/" + commit_no + "/files");
	if (fs::exists(branch_files_path)) {
		std::ifstream f;
		f.open(branch_files_path.string());
		while (getline(f, s1)) {
			branch_files_list.push_back(s1);
		}
	}
	return branch_files_list;
}

std::vector<std::string> LitUtils::check_for_adding_conflict(std::vector<std::string> new_files,
                                                             std::vector<std::string> branch_files)
{
	std::vector<std::string> add_conflict;
	for (const auto &file : new_files) {
		if (std::find(branch_files.begin(), branch_files.end(), file) != branch_files.end()) {
			add_conflict.push_back(fs::path(file).string());
		}
	}
	return add_conflict;
}

std::vector<std::string> LitUtils::get_added_file_conflict_string(std::string commit_no)
{
	std::vector<std::string> root_dir_files = get_root_repo_list();
	std::vector<string> list_of_file_backup;
	iterate_any_repository(m_backup_dir, list_of_file_backup);

	m_litStatus->check_for_added_or_modified(root_dir_files, list_of_file_backup);

	std::vector<string> vec_added_files = m_litStatus->get_recently_added_files();
	std::vector<string> vec_branch_files = get_branch_files(commit_no);

	return check_for_adding_conflict(vec_added_files, vec_branch_files);
}

bool LitUtils::is_anything_modified()
{
	std::vector<std::string> root_dir_files = get_root_repo_list();
	std::vector<string> list_of_file_backup;
	iterate_any_repository(m_backup_dir, list_of_file_backup);
	m_litStatus->clear_status();
	m_litStatus->check_for_added_or_modified(root_dir_files, list_of_file_backup);
	m_litStatus->check_for_deleted(root_dir_files, list_of_file_backup);
	return m_litStatus->is_anything_modified();
}

void LitUtils::create_empty_files(std::string commit_dir)
{
	std::ifstream empty_files_list(commit_dir + "/empfiles");
	if (empty_files_list.is_open()) {
		string s;
		while (getline(empty_files_list, s)) {
			if (!fs::exists(fs::path(s))) {
				std::ofstream fs;
				fs.open(s, std::ios::out);
				fs.close();
			}
		}
	}
}