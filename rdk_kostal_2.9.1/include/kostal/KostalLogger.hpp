/**
 * @file KostalLogger.hpp
 * @copyright Copyright (C) 2016-2022 Flexiv Ltd. All Rights Reserved.
 */

#ifndef FLEXIVRDK_KOSTALLOGGER_HPP_
#define FLEXIVRDK_KOSTALLOGGER_HPP_

// Kostal header files
#include <kostal/SystemParams.h>

namespace kostal {

    /**
     * @class Log
     * @brief Loggers to print messages with timestamp, for kostal project only
     */
    class Log
    {
    public:
        Log() = default;
        virtual ~Log() = default;

        /**
         * @brief Print info message with timestamp and coloring
         * @param[in] message Info message
         * @note Color = green
         */
        void info(const std::string& message) const{
            spdlog::info(message);
        }

        /**
         * @brief Print warning message with timestamp and coloring
         * @param[in] message Warning message
         * @note Color = yellow
         */
        void warn(const std::string& message) const{
            spdlog::warn(message);
        }

        /**
         * @brief Print error message with timestamp and coloring
         * @param[in] message Error message
         * @note Color = red
         */
        void error(const std::string& message) const{
            spdlog::error(message);
        }
    };

} /* namespace kostal */

#endif /* FLEXIVRDK_KOSTALLOGGER_HPP_ */
