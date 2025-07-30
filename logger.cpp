#include "logger.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>


namespace Log {

static constexpr std::string_view LOG_DIR = "Logs";

std::filesystem::path getCurrPath() {
    static std::filesystem::path curr_path;
    try {
        curr_path = std::filesystem::current_path();
    } catch(const std::filesystem::filesystem_error& err) {
        std::cerr << "Can't take path to application. Check rights" << std::endl;
        curr_path = std::filesystem::path("/home") / LOG_DIR; // TODO: Update also for win
    }

    return curr_path;
}

std::filesystem::path getLogPath() {
    static std::filesystem::path log_path = getCurrPath() / LOG_DIR;
    return log_path;
}

static std::string log_filename;
union Filename {
    std::string get() {
        return log_filename;
    }
    void set(const std::string& filename) {
        log_filename = filename;
    }
} filename;

// std::string getFilename(const std::string& new_name = "") {
//     static std::string log_filename;
//     if (!new_name.empty()) {
//         log_filename = new_name;
//     }
//     return log_filename;
// }

bool fileExist() {
    try {
        for (const auto& entry : std::filesystem::directory_iterator(getLogPath())) {
            // std::cout << entry.path().string() << std::endl;

            if (!entry.is_directory() && entry.path().filename().string() == filename.get()) {
                return true;
            }
        }

        return false;
    } catch (const std::filesystem::filesystem_error& err) {
        std::cout << "Filesystem access error: " <<  err.what() << std::endl;
    }
    return false;
}

std::string generateFileName() {
    std::stringstream filename;
    filename << "log_" << TimeUtils::nowStr() << ".txt";

    return filename.str();
}

std::pair<std::filesystem::path, std::uintmax_t> GetLatestFileInfo() {
    std::filesystem::path latest_file;
    std::uintmax_t file_size = 0;
    std::time_t latest_time_access = LONG_MIN;

    for (const auto& entry : std::filesystem::directory_iterator(getLogPath())) {
        if (entry.is_regular_file()) {
            // TODO: Why is it negative time?
            const auto write_time = entry.last_write_time();
            const auto time_since_epoch = write_time.time_since_epoch();
            const auto seconds_since_epoch = std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count();

            if (seconds_since_epoch > latest_time_access) {
                latest_time_access = seconds_since_epoch;
                latest_file = entry.path();
                file_size = entry.file_size();
            }
        }
    }

    return {latest_file, file_size};
}

void writeInFile(const std::stringstream& log_message) {
    static constexpr std::uint32_t five_mbytes = 5 /** 1024*/ * 1024;
    const auto [latest_file, file_size] = GetLatestFileInfo();
    if (filename.get().empty() || file_size > five_mbytes) {
        const std::string actual_name = !latest_file.empty() || file_size > five_mbytes ? generateFileName() : latest_file.filename().string();
        filename.set(actual_name);
    }

    std::fstream file(getLogPath() / filename.get(), std::ios::app);
    if (!file) {
        throw std::runtime_error("Cannot open the log file" + (getLogPath() / filename.get()).string());
    }

    file << log_message.str() << '\n';
}

void writeMessage(const std::stringstream& log_message) {
    std::cout << log_message.str() << std::endl;

    if (!std::filesystem::directory_entry(getCurrPath() / LOG_DIR).is_directory()) {
        std::filesystem::create_directory(getLogPath());
    }
    writeInFile(log_message);
}

}  // namespace Log
