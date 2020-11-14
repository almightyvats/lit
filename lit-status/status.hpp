#ifndef STATUS_HPP
#define STATUS_HPP
#include <string>
#include <vector>
#include <map>
#include <filesystem>

class LitStatus
{
private:
    std::vector<std::string> m_current_working_files;
    std::vector<std::string> m_backup_files;
    std::filesystem::path m_current_working_dir;
    std::map<std::string, std::string> m_status;

    std::string m_backup_folder;

public:
    LitStatus(std::filesystem::path current_working_dir);
    ~LitStatus() {}

    void check_for_added_or_modified(std::vector<std::string> cwd_filepaths, std::vector<std::string> backup_filepaths);
    std::string check_for_deleted(std::vector<std::string> cwd_filepaths, std::vector<std::string> backup_filepaths);
    std::string polish_for_cwd(std::string file_path);
    std::string polish_for_backup(std::string file_path);
    bool compare_two_files(const std::string &file1, const std::string &file2);

    void print_status();
};

#endif