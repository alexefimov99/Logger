#ifndef LOGGER_H
#define LOGGER_H

#include "utils.h"

#include <sstream>


namespace Log {

enum class Level {
    INFO,
    WARNING,
    ERROR
};

static Level definite_level = Level::INFO;

static void setLogLevel(const Level level) {
    definite_level = level;
}

void writeMessage(const std::stringstream& log_message);

template<typename... Args>
void info(Args&&... args) {
    if (definite_level > Level::INFO) {
        return;
    }

    std::stringstream log_message;
    log_message << TimeUtils::nowStr() << " [INFO] ";
    (log_message << ... << std::forward<Args>(args));
    writeMessage(log_message);
}

template<typename... Args>
void warning(Args&&... args) {
    if (definite_level > Level::WARNING) {
        return;
    }

    std::stringstream log_message;
    log_message << TimeUtils::nowStr() << " [WARNING] ";
    (log_message << ... << std::forward<Args>(args));
    writeMessage(log_message);
}

template<typename... Args>
void error(Args&&... args) {
    if (definite_level > Level::ERROR) {
        return;
    }

    std::stringstream log_message;
    log_message << TimeUtils::nowStr() << " [ERROR] ";
    (log_message << ... << std::forward<Args>(args));
    writeMessage(log_message);
}

}  // namespace Log

#endif // LOGGER_H
