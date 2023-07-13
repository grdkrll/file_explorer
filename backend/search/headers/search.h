#include <future>
#include <vector>
#include <filesystem>
#include "ThreadPool.h"

std::future<std::vector<std::filesystem::path>> ls(std::filesystem::path path, ThreadPool &pool);

void searchDir(ThreadPool &pool, std::filesystem::path path, std::function<bool(std::filesystem::path)> predicate,
               std::function<void()> callback);