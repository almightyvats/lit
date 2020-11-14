#ifndef INIT_HPP
#define INIT_HPP

#include <filesystem>

namespace fs = std::filesystem;

bool create_root_dir(const fs::path &current_working_dir);
bool validate_root(const fs::path &current_working_dir);

#endif