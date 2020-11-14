#ifndef LOG_HPP
#define LOG_HPP
#include <filesystem>
#include <map>
#include <string>

enum commit_type { None, Branch, Merge };
enum symbol_type {
	initial,
	none,
	branch,
	merge,
	parent,
	lastBranch,
	lastMerged,
	lastNone,
	currentCheckout,
	emptyColumn
};

class LitLog {
  private:
	std::filesystem::path m_current_working_dir;
	std::string m_commit_dir;
	std::map<int, std::string> m_commit_messages;
	std::map<int, int> m_branch_parent;
	int m_last_commit;
	int m_last_checkout;
	bool m_is_branch_present;

	void set_commit_message(int commit_no, std::string message);
	std::string get_commit_message(int commit_no);

	void set_branch_parent(int branch_no, int parent_no);
	int get_branch_if_commit_is_parent(int parent_no);

	void set_is_branch_present(bool present);
	bool get_is_branch_present();
	void read_commit_message(std::string commit_dir, int commit_no);
	int read_branch_parent(std::string commit_dir);

  public:
	LitLog(std::filesystem::path current_working_dir, int last_commit, int last_checkout);
	~LitLog() {}

	std::map<int, commit_type> get_log_info();
	std::string get_symbol_for(symbol_type type);
	void print_log();
};

#endif