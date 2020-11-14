#include "utils.hpp"
#include "../lit-log/log.hpp"
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
using string = std::string;

LitUtils::LitUtils(fs::path current_working_dir)
{
	m_current_working_dir = current_working_dir;
	m_litStatus = new LitStatus(current_working_dir);
	m_lit_folder = m_current_working_dir / ".lit";
	m_commit_dir = m_lit_folder + "/commits";
	m_conflict_file = m_lit_folder + "/mconflict";
	m_backup_dir = m_lit_folder + "/backup";
	m_file_list = m_lit_folder + "/files";
	m_info_file = m_lit_folder + "/info";
}

LitUtils::~LitUtils()
{
	delete m_litStatus;
	m_litStatus = 0;
}

std::filesystem::path LitUtils::get_current_working_dir()
{
	return m_current_working_dir;
}

bool LitUtils::commit(string commit_msg, bool for_merge /* = false*/)
{
	fs::path current_working_dir = get_current_working_dir();

	if (fs::exists(fs::path(m_conflict_file)))
	{
		for_merge = true;
		fs::remove(fs::path(m_conflict_file));
	}

	string last_commit_number;
	string last_checkout_number;
	get_info(last_commit_number, last_checkout_number);
	std::vector<string> list_of_files;
	iterate_root_repository(list_of_files);
	set_root_repo_list(list_of_files);

	int temp_commit_no = std::stoi(last_commit_number);
	int temp_lastcommit_no = std::stoi(last_checkout_number);

	string new_commit_folder_name = "r" + std::to_string(temp_commit_no + 1);

	string commit_target_dest = m_commit_dir + "/" + new_commit_folder_name;

	string backup_dir = m_backup_dir;
	if (!fs::exists(backup_dir))
		fs::create_directories(backup_dir);

	if (!fs::exists(commit_target_dest))
	{
		create_new_commit_dir(commit_target_dest, new_commit_folder_name, temp_commit_no, temp_lastcommit_no, last_checkout_number, commit_msg, for_merge);
	}

	string commit_diff_minus_u = "diff -Naru -w -x '.*' -x ‘.lit’ " + backup_dir + " " + current_working_dir.string() + " > " + commit_target_dest + "/" + new_commit_folder_name + ".patch";
	system(commit_diff_minus_u.c_str());

	sync_backup_folder();
	sync_info();
	sync_file_status(true);

	return true;
}

bool LitUtils::checkout(std::string commit_no, bool for_merge /* = false*/)
{
	string last_checkout_number, current_checkout_target;
	string last_commit_number;
	get_info(last_commit_number, last_checkout_number);

	if (commit_no != "" && !validate_commit_no(commit_no, std::stoi(last_commit_number)))
		return false;

	if (commit_no.empty())
	{
		commit_no = last_checkout_number;
		current_checkout_target = last_checkout_number;
	}
	else
	{
		current_checkout_target = commit_no.erase(0, 1);
	}

	int r = std::stoi(current_checkout_target);

	fs::remove_all(fs::path(m_backup_dir));

	if (!fs::exists(fs::path(m_backup_dir)))
	{
		fs::create_directories(m_backup_dir);
	}

	if (!for_merge)
	{
		clear_root();
	}

	string desired_commit_dir = m_commit_dir + "/r" + commit_no;
	if (!fs::exists(fs::path(desired_commit_dir)))
		return false;

	bool checkout_merged = fs::exists(fs::path(desired_commit_dir + "/mrgd"));

	bool checkout_branch = fs::exists(fs::path(desired_commit_dir + "/brch"));

	if (checkout_merged)
	{
		patch_root(commit_no);
	}
	else if (checkout_branch)
	{
		int final_parent;
		std::set<int> branch_parents;
		check_for_parent(desired_commit_dir + "/brch", r, branch_parents, final_parent);
		patch_root_recursive(final_parent);
		for (auto &b : branch_parents)
		{
			patch_root(std::to_string(b));
		}
	}
	else
	{
		patch_root_recursive(r);
	}

	if (fs::exists(fs::path(m_conflict_file)))
	{
		fs::remove(fs::path(m_conflict_file));
	}

	if (!for_merge)
	{
		update_checkout_status(r);
		sync_file_status(false);
		sync_backup_folder();
	}

	return true;
}

bool LitUtils::patch_root(const std::string commit_no)
{
	string commit_name = "/r" + commit_no;
	string commit_dir = m_commit_dir + commit_name;
	string patch_str = "patch -s -p0 <" + commit_dir + "/" + commit_name + ".patch";
	system(patch_str.c_str());
	return true;
}

bool LitUtils::patch_root_recursive(const int upto_commit_no)
{
	for (int i = 0; i <= upto_commit_no; i++)
	{
		string commit_name = "/r" + std::to_string(i);
		string commit_dir = m_commit_dir + commit_name;

		bool is_brch_present = fs::exists(fs::path(commit_dir + "/brch"));
		if (is_brch_present)
		{
			continue;
		}

		if (fs::exists(fs::path(commit_dir + "/mrgd")))
		{
			clear_root();
		}

		patch_root(std::to_string(i));
	}
	return true;
}

bool LitUtils::show(std::string commit_no)
{
	string last_checkout_number, current_checkout_target;
	string last_commit_number;
	get_info(last_commit_number, last_checkout_number);

	if (!validate_commit_no(commit_no, std::stoi(last_commit_number)))
		return false;

	string l = commit_no.erase(0, 1);
	int r = std::stoi(l);

	string desired_commit_dir = m_commit_dir + "/r" + commit_no;

	if (!fs::exists(fs::path(desired_commit_dir)))
		return false;

	string str;
	std::ostringstream sm;
	std::ifstream m;
	m.open(desired_commit_dir + "/msg");
	if (m)
	{
		sm << m.rdbuf(); // reading data
		str += sm.str() += "\n";
	}
	m.close();

	std::vector<string> list;
	iterate_any_repository(fs::path(desired_commit_dir), list);

	for (auto &l : list)
	{
		if (fs::is_directory(fs::path(l)) || (l == desired_commit_dir + "/files") || (l == desired_commit_dir + "/brch") || (l == desired_commit_dir + "/msg") || (l == desired_commit_dir + "/mrgd") || (l == desired_commit_dir + "/cpar"))
		{
			continue;
		}

		std::ifstream f;
		string s;
		f.open(l);
		while (getline(f, s))
		{
			size_t pos = s.find("diff -Naru");
			if (pos != string::npos)
			{
				continue;
			}
			str += s += "\n";
		}
		f.close();
	}

	std::cout << str;
	return true;
}

bool LitUtils::merge(std::string commit_no)
{
	string last_commit_number;
	string last_checkout_number;
	get_info(last_commit_number, last_checkout_number);

	if (!validate_commit_no(commit_no, std::stoi(last_commit_number)))
		return false;

	std::vector<string> list_of_files;
	iterate_root_repository(list_of_files);
	set_root_repo_list(list_of_files);

	add_rev_name_to_all_files(list_of_files, "r" + last_checkout_number);

	if (!checkout(commit_no, true))
	{
		std::cout << "Some problem with merge.\n";
		return false;
	}

	add_rev_name_to_all_files(list_of_files, commit_no);

	bool files_merged_status = true;
	string conflicted_file_names;

	for (auto x : list_of_files)
	{
		if (fs::is_directory(fs::path(x)))
			continue;

		const string orig = x + ".r" + last_checkout_number;
		const string brnc = x + "." + commit_no;

		if (fs::exists(fs::path(orig)) && !fs::exists(fs::path(brnc)))
		{
			polish_file_name(orig, ".r" + last_checkout_number);
			continue;
		}

		if (fs::exists(fs::path(brnc)) && fs::exists(fs::path(orig)))
		{
			bool files_merged_ok = merge_two_files(orig, brnc);
			if (!files_merged_ok)
			{
				if (fs::exists(fs::path(orig + "_mc")))
				{
					polish_file_name(orig + "_mc", ".r" + last_checkout_number + "_mc");
				}
				files_merged_status = false;
				conflicted_file_names += "- " + fs::path(x).filename().string() + "\n";
			}
			else
			{
				polish_file_name(orig, ".r" + last_checkout_number);
			}
		}
	}

	if (files_merged_status)
	{
		string commit_msg = "Merge " + commit_no + " into r" + last_checkout_number;
		commit(commit_msg, true);
	}
	else
	{
		std::ofstream fs;
		fs.open(m_conflict_file, std::ios::out);
		fs.close();
		std::cout << "Merge conflict(s) detected:\n"
				  << conflicted_file_names << "\n";
	}

	return true;
}

bool LitUtils::log()
{
	string last_commit;
	string last_checkout;
	get_info(last_commit, last_checkout);

	int last_commit_no = std::stoi(last_commit);
	int last_checkout_no = std::stoi(last_checkout);

	LitLog log(get_current_working_dir(), last_commit_no, last_checkout_no);
	log.print_log();

	return true;
}

bool LitUtils::status()
{
	if (!fs::exists(fs::path(m_backup_dir)))
		return false;

	std::vector<string> list_of_files_cwd;
	iterate_root_repository(list_of_files_cwd);
	set_root_repo_list(list_of_files_cwd);
	std::vector<string> list_of_file_backup;
	iterate_any_repository(m_backup_dir, list_of_file_backup);

	m_litStatus->check_for_added_or_modified(list_of_files_cwd, list_of_file_backup);
	m_litStatus->check_for_deleted(list_of_files_cwd, list_of_file_backup);

	m_litStatus->print_status();

	return true;
}