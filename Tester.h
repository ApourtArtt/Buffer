#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <functional>
#include <tuple>
#include <chrono>

namespace
{
    struct testRet
    {
        std::vector<std::tuple<std::string, bool, std::string>> statements;
        void AddStatement(std::string Title, bool Status, std::string location)
        {
            statements.push_back({ std::move(Title), std::move(Status), std::move(location) });
        }
    };

    struct benchRet
    {
        int counter = 0;
    };

    struct registrar {
        struct testEntity {
            std::string title, subTitle;
            std::function<testRet()> func;
            testEntity(std::string Title, std::string SubTitle, std::function<testRet()> Func)
                : title(std::move(Title)), subTitle(std::move(SubTitle)), func(Func)
            {}
        };
        struct benchEntity {
            std::string title, subTitle;
            std::function<benchRet()> func;
            benchEntity(std::string Title, std::string SubTitle, std::function<benchRet()> Func)
                : title(std::move(Title)), subTitle(std::move(SubTitle)), func(Func)
            {}
        };

        inline static std::vector<testEntity> testers;
        registrar(std::string Title, std::string SubTitle, std::function<testRet()> Func) {
            testers.push_back(testEntity(std::move(Title), std::move(SubTitle), Func));
        }
        inline static std::vector<benchEntity> benchers;
        registrar(std::string Title, std::string SubTitle, std::function<benchRet()> Func) {
            benchers.push_back(benchEntity(std::move(Title), std::move(SubTitle), Func));
        }
    };

#define GET_LOCATION __FILE__ + std::string(":") + std::to_string(__LINE__)

#define CONCAT(a, b) CONCAT_INNER(a, b)
#define CONCAT_INNER(a, b) a ## b
#define UNIQUE_NAME(base) CONCAT(base, __COUNTER__)

#define TESTER(name, message, func) registrar UNIQUE_NAME(t)(name, message, []() -> testRet { testRet r; func return r; });
#define BENCHER(name, message, func) registrar UNIQUE_NAME(b)(name, message, []() -> benchRet { benchRet r; func return r; });
#define EXPECTER(title, statement) r.AddStatement(std::string("[EXPECT] ")+title, statement, GET_LOCATION);
#define ASSERTER(title, statement) r.AddStatement(std::string("[ASSERT] ")+title, statement, GET_LOCATION); if (statement == false) return r;
}

#define TEST(...) TESTER(__VA_ARGS__)
#define BENCH(...) BENCHER(__VA_ARGS__)
#define EXPECT(...) EXPECTER(__VA_ARGS__)
#define ASSERT(...) ASSERTER(__VA_ARGS__)

#define START_BENCH \
    auto t1 = std::chrono::high_resolution_clock::now(); \
    auto t2 = std::chrono::high_resolution_clock::now(); \
    while (duration_cast<std::chrono::milliseconds>(t2-t1).count() < 1000) [[unlikely]] {

#define STOP_BENCH \
        t2 = std::chrono::high_resolution_clock::now(); \
        r.counter++; \
    }

namespace Tester
{
    int Run()
    {
        static bool hasRan = false;
        if (hasRan) return -1;
        hasRan = true;

        int nbTestFailed = 0, nbTestPassed = 0;
        int nbBigTestFailed = 0;

        std::cout << "Starting tests..." << std::endl;

        for (auto fn : registrar::testers)
        {
            bool success = true;
            std::cout << "Starting to test: " << fn.title << " - " << fn.subTitle << std::endl;
            testRet r = fn.func();

            for (auto& test : r.statements)
            {
                auto [subTitle, status, location] = test;
                if (status)
                {
                    std::cout << "    \033[36mPassed:\033[0m " << subTitle << std::endl;
                    nbTestPassed++;
                }
                else
                {
                    std::cout << "    \033[31mFailed:\033[0m " << subTitle << " (" << location << ")" << std::endl;
                    nbTestFailed++;
                    success = false;
                }
            }

            nbBigTestFailed += !success;
        }

        std::cout << "Tests executed:\t\t" << registrar::testers.size()
            << " (Passed: " << registrar::testers.size() - nbBigTestFailed << ", Failed: " << nbBigTestFailed << ')'
            << "\nStatements executed:\t" << nbTestFailed + nbTestPassed
            << " (Passed: " << nbTestPassed << ", Failed: " << nbTestFailed << ')' << std::endl;

        std::cout << "\nStarting benchs..." << std::endl;

        for (auto& fn : registrar::benchers)
        {
            std::cout << "Starting to benchmark: " << fn.title << " - " << fn.subTitle << "...\t";
            benchRet r = fn.func();
            std::cout << r.counter << " iteration/s" << std::endl;
        }

        std::cout << "Benchs executed\n\nEvery tasks are finished" << std::endl;
        return -nbTestFailed;
    }
}