//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Abstraction of logger.

#include <string>
#include <sstream>

// LogInfo
void LogInfo(const std::string& content);

template<typename... Args>
void LogInfo(Args const&... args)
{
    std::ostringstream stream;
    using List= int[];
    (void)List{0, ((void)(stream << args), 0) ... };
    LogInfo(stream.str());
}

// LogError
void LogError(const std::string& content);

template<typename... Args>
void LogError(Args const&... args)
{
    std::ostringstream stream;
    using List= int[];
    (void)List{0, ((void)(stream << args), 0) ... };
    LogError(stream.str());
}

// LogDebug
void LogDebug(const std::string& content);

template<typename... Args>
void LogDebug(Args const&... args)
{
    std::ostringstream stream;
    using List= int[];
    (void)List{0, ((void)(stream << args), 0) ... };
    LogDebug(stream.str());
}

// LogBug
void LogBug(const std::string& content);

template<typename... Args>
void LogBug(Args const&... args)
{
    std::ostringstream stream;
    using List= int[];
    (void)List{0, ((void)(stream << args), 0) ... };
    LogBug(stream.str());
}
