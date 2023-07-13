#include <future>
#include <vector>
#include <filesystem>
#include <list>
#include "search.h"
#include "ThreadPool.h"

namespace Search {
    std::future<std::vector<std::filesystem::path>> ls(std::filesystem::path path, ThreadPool &pool) {
        std::shared_ptr<std::promise<std::vector<std::filesystem::path>>> promisePtr = std::make_shared<std::promise<std::vector<std::filesystem::path>>>();
        auto func = [promisePtr, dirPath = std::move(path)]() {
            std::vector<std::filesystem::path> res;
            try {
                for (const auto &path: std::filesystem::directory_iterator(dirPath)) {
                    res.push_back(path.path());
                }
            } catch (...) {
                //TODO
            }
            promisePtr->set_value(res);
        };
        pool.push_task(func);
        return promisePtr->get_future();
    }

    void searchDir(ThreadPool &pool, std::filesystem::path dirPath,
                   const std::function<bool(std::filesystem::path)> &predicate,
                   const std::function<void(std::filesystem::path)> &callback) {
        std::list<std::future<std::vector<std::filesystem::path>>> futures;
        futures.push_back(std::move(ls(dirPath, pool)));
        while (!futures.empty()) {
            auto iter = futures.end();
            for (auto it = futures.begin(); it != futures.end(); ++it) {
                if (it->wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
                    iter = it;
                    break;
                }
            }
            if (iter != futures.end()) {
                std::vector<std::filesystem::path> res = iter->get();
                futures.erase(iter);
                for (const auto &path: res) {
                    if (predicate(path)) {
                        if (std::filesystem::is_directory(path)) {
                            futures.insert(futures.end(), std::move(ls(path, pool)));
                        }
                        callback(path);
                    }
                }
            }
        }
    }
}