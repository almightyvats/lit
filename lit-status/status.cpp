#include "status.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
using string = std::string;

LitStatus::LitStatus(std::filesystem::path current_working_dir)
{
	m_current_working_dir = current_working_dir;
	m_backup_folder = m_current_working_dir / ".lit" / "backup";
}

void LitStatus::check_for_added_or_modified(std::vector<std::string> cwd_filepaths,
                                            std::vector<std::string> backup_filepaths)
{
	if (backup_filepaths.empty()) {
		for (auto &file : cwd_filepaths) {
			if (fs::is_directory(fs::path(file)))
				continue;

			m_status.insert({file, "Added"});
			if (is_file_empty(file)) {
				m_empty_files.push_back(file);
			}
		}
		return;
	}

	for (auto file : cwd_filepaths) {
		if (fs::is_directory(fs::path(file)))
			continue;

		string file_to_find = polish_for_backup(file);
		if (is_file_empty(file)) {
			m_empty_files.push_back(file);
		}

		if (std::find(backup_filepaths.begin(), backup_filepaths.end(), file_to_find) != backup_filepaths.end()) {
			if (compare_two_files(file, file_to_find)) {
				m_status.insert({file, "Modified"});
			}
		} else {
			m_status.insert({file, "Added"});
			m_recently_added_files.push_back(file);
		}
	}
}

bool LitStatus::compare_two_files(const std::string &file1, const std::string &file2)
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

	bool files_modified = false;

	files_modified = v_f1.size() != v_f2.size();
	if (!files_modified) {
		for (int i = 0; i < v_f1.size(); i++) {
			if (v_f1[i] != v_f2[i]) {
				files_modified = true;
				break;
			}
		}
	}
	return files_modified;
}

string LitStatus::check_for_deleted(std::vector<std::string> cwd_filepaths, std::vector<std::string> backup_filepaths)
{
	string deleted_files = "\n";
	if (cwd_filepaths.empty()) {
		for (auto &file : backup_filepaths) {
			if (fs::is_directory(fs::path(file)))
				continue;

			m_status.insert({file, "Deleted"});
			deleted_files += "Deleted\t" + fs::path(file).filename().string() + "\n";
		}
		return deleted_files;
	}
	for (auto file : backup_filepaths) {
		if (fs::is_directory(fs::path(file)))
			continue;

		string file_to_find = polish_for_cwd(file);

		if (std::find(cwd_filepaths.begin(), cwd_filepaths.end(), file_to_find) == cwd_filepaths.end()) {
			m_status.insert({file, "Deleted"});
			deleted_files += "Deleted\t" + fs::path(file).filename().string() + "\n";
		}
	}
	return deleted_files;
}

string LitStatus::polish_for_cwd(std::string file_path)
{
	string polished_file_name = file_path.erase(0, m_backup_folder.length());
	string original_file_path = m_current_working_dir.string() + polished_file_name;

	return original_file_path;
}

string LitStatus::polish_for_backup(std::string file_path)
{
	string polished_file_name = file_path.erase(0, m_current_working_dir.string().length());
	string backup_file_path = m_backup_folder + polished_file_name;

	return backup_file_path;
}

void LitStatus::print_status()
{
	for (auto &i : m_status) {
		std::cout << i.second << "\t" << fs::path(i.first).filename() << "\n";
	}
}

bool LitStatus::is_anything_modified()
{
	return (m_status.size() > 0) ? true : false;
}

std::vector<string> LitStatus::get_recently_added_files()
{
	return m_recently_added_files;
}

std::vector<std::string> LitStatus::get_empty_files()
{
	return m_empty_files;
}
bool LitStatus::is_file_empty(const std::string &file)
{
	std::ifstream pFile(file);
	return pFile.peek() == std::ifstream::traits_type::eof();
}

void LitStatus::clear_status()
{
	m_status.clear();
}