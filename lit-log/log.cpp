#include "log.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
using string = std::string;

LitLog::LitLog(fs::path current_working_dir, int last_commit, int last_checkout)
	: m_last_commit(last_commit), m_last_checkout(last_checkout)
{
	m_current_working_dir = current_working_dir;
	m_commit_dir = m_current_working_dir / ".lit" / "commits";
	m_is_branch_present = false;
}

string LitLog::get_symbol_for(symbol_type type)
{
	string symbol = "";
	switch (type)
	{
	case symbol_type::initial:
		symbol = !get_is_branch_present() ? "o" : "o─┘";
		break;
	case symbol_type::none:
		symbol = !get_is_branch_present() ? "o" : "o │";
		break;
	case symbol_type::branch:
		symbol = "│ o";
		break;
	case symbol_type::merge:
		symbol = "o─│";
		break;
	case symbol_type::parent:
		symbol = "─";
		break;
	case symbol_type::lastNone:
		symbol = "o";
		break;
	case symbol_type::lastBranch:
		symbol = "  o";
		break;
	case symbol_type::lastMerged:
		symbol = "o─┐";
		break;
	case symbol_type::currentCheckout:
		symbol = "←";
		break;
	case symbol_type::emptyColumn:
		symbol = " ";
		break;
	default:
		symbol = "Bad";
		break;
	}
	return symbol;
}

std::map<int, commit_type> LitLog::get_log_info()
{
	std::map<int, commit_type> info;

	for (int i = 0; i <= m_last_commit; i++)
	{
		string commit_dir = m_commit_dir + "/r" + std::to_string(i);
		if (fs::exists(fs::path(commit_dir)))
		{
			string none_file = commit_dir + "/cpar";
			string merge_file = commit_dir + "/mrgd";
			string branch_file = commit_dir + "/brch";
			bool is_none = fs::exists(fs::path(none_file));
			bool is_merged = fs::exists(fs::path(merge_file));
			bool is_branched = fs::exists(fs::path(branch_file));
			string file_to_read;
			commit_type c_type;
			if (is_none)
			{
				file_to_read = none_file;
				c_type = commit_type::None;
			}
			else if (is_merged)
			{
				file_to_read = merge_file;
				c_type = commit_type::Merge;
				set_is_branch_present(true);
			}
			else
			{
				file_to_read = is_branched;
				c_type = commit_type::Branch;
				set_is_branch_present(true);
				int parent = read_branch_parent(commit_dir);
				set_branch_parent(i, parent);
			}
			info.insert(std::make_pair(i, c_type));
			read_commit_message(commit_dir, i);
		}
	}
	return info;
}

void LitLog::set_is_branch_present(bool present)
{
	m_is_branch_present = present;
}
bool LitLog::get_is_branch_present()
{
	return m_is_branch_present;
}

void LitLog::read_commit_message(string commit_dir, int commit_no)
{
	std::ifstream myinfo(commit_dir + "/msg");
	int line_no = 0;
	string message;
	while (line_no != 5 && getline(myinfo, message))
	{
		++line_no;
	}

	set_commit_message(commit_no, message);
}

int LitLog::read_branch_parent(string commit_dir)
{
	int res = -1;
	std::ifstream mybrch(commit_dir + "/brch");
	string num;
	getline(mybrch, num);
	mybrch.close();
	if (!num.empty())
	{
		res = std::stoi(num);
	}
	return res;
}

void LitLog::set_branch_parent(int branch_no, int parent_no)
{
	m_branch_parent.insert({branch_no, parent_no});
}

int LitLog::get_branch_if_commit_is_parent(int parent_no)
{
	int branch = -1;
	for (auto &brch : m_branch_parent)
	{
		if (brch.second == parent_no)
		{
			branch = brch.first;
		}
	}
	return branch;
}

void LitLog::set_commit_message(int commit_no, string message)
{
	m_commit_messages.insert({commit_no, message});
}

string LitLog::get_commit_message(int commit_no)
{
	string message;
	for (auto &commit : m_commit_messages)
	{
		if (commit.first == commit_no)
		{
			message = commit.second;
		}
	}
	return message;
}
void LitLog::print_log()
{
	std::map<int, commit_type> info = get_log_info();
	string log;
	int counter = 0;
	int size = info.size();
	if (size < 1)
	{
		std::cout << "No commits yet!" << std::endl;
		return;
	}

	std::map<int, commit_type>::reverse_iterator it;
	for (it = info.rbegin(); it != info.rend(); it++)
	{
		counter++;
		int commit_no = it->first;
		bool is_current_checkedout = commit_no == m_last_checkout;
		commit_type type = it->second;
		bool is_last_commit_a_branch = counter == 1 && type == commit_type::Branch;
		bool is_last_commit_a_merge = counter == 1 && type == commit_type::Merge;
		bool is_last_commit_none = counter == 1 && type == commit_type::None;
		bool is_initial_commit = counter == info.size();

		switch (type)
		{
		case commit_type::None:
		{
			log += is_initial_commit ? get_symbol_for(symbol_type::initial)
									 : (is_last_commit_none ? get_symbol_for(symbol_type::lastNone)
															: get_symbol_for(symbol_type::none));
		}
		break;
		case commit_type::Merge:
		{
			log +=
				is_last_commit_a_merge ? get_symbol_for(symbol_type::lastMerged) : get_symbol_for(symbol_type::merge);
		}
		break;
		case commit_type::Branch:
		{
			log +=
				is_last_commit_a_branch ? get_symbol_for(symbol_type::lastBranch) : get_symbol_for(symbol_type::branch);
		}
		break;
		default:
			break;
		}

		int parent = get_branch_if_commit_is_parent(commit_no);
		if (parent != -1)
		{
			log += get_symbol_for(symbol_type::parent) + " r" + std::to_string(parent);
		}
		else
		{
			if (get_is_branch_present())
				log += "    ";
		}

		log += (is_current_checkedout ? get_symbol_for(symbol_type::currentCheckout)
									  : get_symbol_for(symbol_type::emptyColumn)) +
			   " r" + std::to_string(commit_no) + " " + get_commit_message(--size) + "\n";
	}
	std::cout << log;
}