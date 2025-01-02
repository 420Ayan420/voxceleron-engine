#include "Logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cstdarg>

Logger::Logger() : 
    logDirectory("logs"),
    logToConsole(false),
    maxLogSize(10 * 1024 * 1024)  // 10MB default max size
{
    // Enable all log levels by default
    for (int level = static_cast<int>(LogLevel::INFO); 
         level <= static_cast<int>(LogLevel::PERFORMANCE); 
         level++) {
        enabledLevels[static_cast<LogLevel>(level)] = true;
    }
    initStartupTimestamp();
    createLogDirectoryIfNeeded();
    openLogFile();
}

Logger::~Logger() {
    // File will be automatically closed by unique_ptr
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::log(LogLevel level, const std::string& message) {
    if (!enabledLevels[level]) return;

    std::lock_guard<std::mutex> lock(logMutex);
    
    // Create the log entry with timestamp
    std::stringstream entry;
    entry << "[" << getTimestamp() << "] "
          << "[" << getLevelString(level) << "] "
          << message << "\n";

    // Write to file
    if (logFile && logFile->is_open()) {
        *logFile << entry.str();
        logFile->flush();
    }

    rotateLogFileIfNeeded();
}

void Logger::logf(LogLevel level, const char* format, ...) {
    if (!enabledLevels[level]) return;

    char buffer[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    log(level, std::string(buffer));
}

std::string Logger::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::stringstream ss;
    struct tm timeinfo;
    localtime_s(&timeinfo, &time);
    ss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S")
       << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

void Logger::initStartupTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    struct tm timeinfo;
    localtime_s(&timeinfo, &time);
    ss << std::put_time(&timeinfo, "%Y%m%d_%H%M%S");
    startupTimestamp = ss.str();
}

std::string Logger::getLevelString(LogLevel level) const {
    switch (level) {
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::WORLDGEN: return "WORLDGEN";
        case LogLevel::RENDER: return "RENDER";
        case LogLevel::PHYSICS: return "PHYSICS";
        case LogLevel::NETWORK: return "NETWORK";
        case LogLevel::PERFORMANCE: return "PERFORMANCE";
        default: return "UNKNOWN";
    }
}

void Logger::setLogDirectory(const std::string& dir) {
    std::lock_guard<std::mutex> lock(logMutex);
    logDirectory = dir;
    createLogDirectoryIfNeeded();
    openLogFile();
}

void Logger::setLogLevel(LogLevel level, bool enabled) {
    std::lock_guard<std::mutex> lock(logMutex);
    enabledLevels[level] = enabled;
}

void Logger::createLogDirectoryIfNeeded() {
    std::filesystem::create_directories(logDirectory);
}

void Logger::openLogFile() {
    std::string filename = logDirectory + "/engine_" + startupTimestamp + ".log";
    logFile = std::make_unique<std::ofstream>(filename, std::ios::app);
}

void Logger::rotateLogFileIfNeeded() {
    if (!logFile || !logFile->is_open()) return;

    // Get current position in file
    auto pos = static_cast<size_t>(logFile->tellp());
    if (pos > maxLogSize) {
        // Close current file
        logFile->close();

        // Generate new filename with current timestamp
        std::string currentFilename = logDirectory + "/engine_" + startupTimestamp + ".log";
        std::string newFilename = logDirectory + "/engine_" + startupTimestamp + "_part" + getTimestamp() + ".log";

        // Rename old file and create new one
        std::filesystem::rename(currentFilename, newFilename);
        openLogFile();
    }
} 