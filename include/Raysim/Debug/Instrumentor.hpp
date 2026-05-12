/**********************************************************************************************
 *   Raysim - A C++ framework for 2D graphics and interactive applications
 *
 *   LICENSE: Apache License, Version 2.0
 *
 *            Copyright 2026 Dayron Mustelier (@DMsuDev)
 *
 *   Raysim is licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *             http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 **********************************************************************************************/

#pragma once

#include "Raysim/Core/Log.hpp"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>
#include <thread>
#include <mutex>
#include <sstream>

namespace RS
{
    /// @brief Stores the result of a single profile event.
    struct ProfileResult
    {
        std::string Name;
        int64_t Start;
        int64_t Elapsed;
        uint32_t ThreadID;
    };

    struct InstrumentationSession
    {
        std::string Name;
    };

    /**
     * @brief Instrumentor class for profiling code execution.
     * It allows you to start and end profiling sessions, and write profile results to a JSON file.
     */
    class Instrumentor
    {
    public:
        Instrumentor(const Instrumentor &) = delete;
        Instrumentor(Instrumentor &&) = delete;

        void BeginSession(const std::string &name, const std::string &filepath = "results.json");
        void EndSession();

        void WriteProfile(const ProfileResult &result);

        static Instrumentor &Get()
        {
            static Instrumentor instance;
            return instance;
        }

    private:
        Instrumentor();
        ~Instrumentor();

        void WriteHeader();
        void WriteFooter();

        // Note: you must already own lock on m_Mutex before
        // calling InternalEndSession()
        void InternalEndSession();

    private:
        InstrumentationSession *m_CurrentSession;
        std::ofstream m_OutputStream;
        std::mutex m_Mutex;
        int m_ProfileCount;
    };

// ==============================================================
// Instrumentation Timer
// ==============================================================

    /**
     * @brief RAII timer class for profiling a scope of code.
     * It automatically starts timing when created and stops when destroyed.
     */
    class InstrumentationTimer
    {

    public:
        InstrumentationTimer(const char *name);
        ~InstrumentationTimer();

        void Stop();

    private:
        const char *m_Name;
        std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
        bool m_Stopped;
    };

    namespace InstrumentorUtils {

        template <size_t N>
        struct ChangeResult
        {
            char Data[N];
        };

        template <size_t N, size_t K>
        constexpr auto CleanupOutputString(const char (&expr)[N], const char (&remove)[K])
        {
            ChangeResult<N> result = {};

            size_t srcIndex = 0;
            size_t dstIndex = 0;
            while (srcIndex < N)
            {
                size_t matchIndex = 0;
                while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && expr[srcIndex + matchIndex] == remove[matchIndex])
                    matchIndex++;
                if (matchIndex == K - 1)
                    srcIndex += matchIndex;
                result.Data[dstIndex++] = expr[srcIndex] == '"' ? '\'' : expr[srcIndex];
                srcIndex++;
            }
            return result;
        }
    }
} // namespace RS

#ifdef RS_ENABLE_PROFILE
    // Resolve which function signature macro will be used. Note that this only
    // works with the Visual Studio compiler. GCC and clang have different macros for this.
    #if defined(_MSC_VER)
        #define RS_FUNC_SIG __FUNCSIG__
    #elif defined(__GNUC__) || defined(__clang__)
        #define RS_FUNC_SIG __PRETTY_FUNCTION__
    #else
        #define RS_FUNC_SIG __func__
    #endif

    #define RS_PROFILE_BEGIN_SESSION(name, filepath) ::RS::Instrumentor::Get().BeginSession(name, filepath)
    #define RS_PROFILE_END_SESSION() ::RS::Instrumentor::Get().EndSession()

	#define RS_PROFILE_SCOPE(name) constexpr auto fixedName##__LINE__ = ::RS::InstrumentorUtils::CleanupOutputString(name, "__cdecl ");\
                                   ::RS::InstrumentationTimer timer##__LINE__(fixedName##__LINE__.Data)
    #define RS_PROFILE_FUNCTION() RS_PROFILE_SCOPE(RS_FUNC_SIG)
#else
    #define RS_PROFILE_BEGIN_SESSION(name, filepath)
    #define RS_PROFILE_END_SESSION()
    #define RS_PROFILE_SCOPE(name)
    #define RS_PROFILE_FUNCTION()
#endif
