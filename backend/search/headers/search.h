#include <future>
#include <vector>
#include <filesystem>
#include "ThreadPool.h"

namespace Search {
    /**
     * Used to get a future of a vector of filepath of all objects in a directory
     * @param path the path to the directory
     * @param pool an instance of ThreadPool to which the task will be added
     * @return a future of a vector of filepath of all objects in a directory
     */
    std::future<std::vector<std::filesystem::path>> ls(std::filesystem::path path, ThreadPool &pool);

    /**
     * Used to search for certain objects in a directory
     * @param pool an instance of ThreadPool to which the tasks of searching will be added
     * @param dirPath the dirPath to the directory
     * @param predicate the predicate to check whether to call a callback on a filepath or not
     * @param callback the callback to be called when predicate returns true
     */
    void searchDir(ThreadPool &pool, std::filesystem::path dirPath, std::function<bool(std::filesystem::path)> predicate,
                   std::function<void()> callback);
}