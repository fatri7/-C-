/**
 * @file unified_lp_solver.cc
 * @brief 统一线性规划求解器（支持并行/串行模式）
 *
 * 本程序提供批量线性规划问题的求解功能，支持两种运行模式：
 * 1. 并行模式：利用多线程加速批量求解
 * 2. 串行模式：顺序求解用于基准测试
 *
 * 主要功能：
 * - 从JSON文件读取批量线性规划问题
 * - 支持并行/串行求解模式切换
 * - 提供详细的求解统计信息
 * - 完善的异常处理机制
 * - 保存成功解答的答案与问题到txt文件
 * 使用方式：
 *   ./unified_lp_solver [--parallel|--serial] [--threads N] [--file filename]
 *
 * 作者：fatri7
 * 日期：2024
 */

#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <tuple>
#include <numeric>
#include <iomanip>
#include <string>
#include <algorithm>
#include <fstream>
#include "json.hpp"
#include "paraSim_threadp"  // 并行求解器
#include "paraSim"         // 串行求解器

using json = nlohmann::json;

// 问题数据结构
using LP_Problem = std::tuple<
    std::vector<double>,
    std::vector<std::vector<double>>,
    std::vector<double>,
    std::vector<std::vector<double>>,
    std::vector<double>,
    std::vector<double>,
    std::vector<double>,
    bool>;

// 结果数据结构
struct SolverStatistics {
    size_t total_problems = 0;
    size_t success_count = 0;
    size_t fail_count = 0;
    double total_iterations = 0;
    double total_solve_time_ms = 0;
    double wall_clock_time_ms = 0;
};

// 从文件读取问题
std::vector<LP_Problem> read_problems_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + filename);
    }

    json j;
    file >> j;

    std::vector<LP_Problem> problems;
    for (const auto& problem_json : j) {
        std::vector<double> c = problem_json["c"];
        std::vector<std::vector<double>> A = problem_json["A"];
        std::vector<double> b = problem_json["b"];
        
        std::vector<std::vector<double>> Aeq = problem_json.value("Aeq", std::vector<std::vector<double>>{});
        std::vector<double> beq = problem_json.value("beq", std::vector<double>{});
        std::vector<double> lb = problem_json.value("lb", std::vector<double>{});
        std::vector<double> ub = problem_json.value("ub", std::vector<double>{});
        bool is_maximization = problem_json.value("ismaximization", false);
        
        problems.emplace_back(c, A, b, Aeq, beq, lb, ub, is_maximization);
    }

    return problems;
}

// 并行求解
SolverStatistics solve_parallel(const std::vector<LP_Problem>& problems, int thread_count) {
    SolverStatistics stats;
    stats.total_problems = problems.size();

    auto start_time = std::chrono::high_resolution_clock::now();
    
    ParallelSolver solver(thread_count);
    auto results = solver.solve_batch(problems);
    
    auto end_time = std::chrono::high_resolution_clock::now();
    stats.wall_clock_time_ms = 
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::string file_path = "answer_parallel.txt";
    std::ofstream out_file(file_path, std::ios::app);
    assert(out_file.is_open());
    for (const auto& result : results) {
        if (result.exitflag == 1) {
            stats.success_count++;
            out_file << result.get_data_string();
        } else {
            stats.fail_count++;
        }
        stats.total_iterations += result.iterations;
        stats.total_solve_time_ms += result.total_time_ms;
    }
    out_file.close();
    return stats;
}

// 串行求解
SolverStatistics solve_serial(const std::vector<LP_Problem>& problems) {
    SolverStatistics stats;
    stats.total_problems = problems.size();

    SerialSolver solver;
    std::vector<SerialSolver::LP_solution> results;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (const auto& problem : problems) {
        results.push_back(std::apply([&solver](auto&&... args) {
            return solver.solve(std::forward<decltype(args)>(args)...);
        }, problem));
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    stats.wall_clock_time_ms = 
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    std::string file_path = "answer_serial.txt";
    std::ofstream out_file(file_path, std::ios::app);
    assert(out_file.is_open());
    for (const auto& result : results) {
        if (result.exitflag == 1) {
            stats.success_count++;
            out_file << result.get_data_string();
        } else {
            stats.fail_count++;
        }
        stats.total_iterations += result.iterations;
        stats.total_solve_time_ms += result.total_time_ms;
    }
    out_file.close();
    return stats;
}

// 打印统计结果
void print_statistics(const SolverStatistics& stats, bool is_parallel, int thread_count = 1) {
    std::cout << "\n============ 求解汇总 ============\n";
    if (is_parallel) {
        std::cout << "模式: 并行 (线程数: " << thread_count << ")\n";
    } else {
        std::cout << "模式: 串行\n";
    }
    
    std::cout << "求解问题总数: " << stats.total_problems << "\n"
              << "成功解决的问题数: " << stats.success_count << "\n"
              << "失败的问题数: " << stats.fail_count << "\n"
              << "总迭代次数: " << stats.total_iterations << "\n"
              << "总求解时间(各问题累加): " << stats.total_solve_time_ms << " 毫秒\n"
              << "实际总耗时: " << stats.wall_clock_time_ms << " 毫秒\n";
    
    if (is_parallel) {
        std::cout << "并行加速比: " << std::fixed << std::setprecision(2) 
                  << stats.total_solve_time_ms/stats.wall_clock_time_ms << "x\n"
                  << "答案与题目详情见answer_parallel.txt\n"
                  << "====================================\n";
    }else{
        std::cout << "答案与题目详情见answer_serial.txt\n"
                  << "====================================\n";
    }
    
    std::cout << "平均每个问题耗时: " 
              << stats.wall_clock_time_ms/stats.total_problems << " 毫秒\n"
              << "====================================\n";
}

// 打印帮助信息
void print_help() {
    std::cout << "用法:\n"
              << "  unified_lp_solver [选项]\n\n"
              << "选项:\n"
              << "  --parallel      使用并行模式 (默认)\n"
              << "  --serial        使用串行模式\n"
              << "  --threads N     设置并行线程数 (默认: 4)\n"
              << "  --file FILE     指定输入文件 (默认: problems.json)\n"
              << "  --help          显示帮助信息\n";
}

int main(int argc, char* argv[]) {
    // 默认参数
    bool parallel_mode = true;
    int thread_count = 4;
    std::string filename = "problems.json";

    // 解析命令行参数
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--parallel") {
            parallel_mode = true;
        } else if (arg == "--serial") {
            parallel_mode = false;
        } else if (arg == "--threads" && i+1 < argc) {
            thread_count = std::stoi(argv[++i]);
        } else if (arg == "--file" && i+1 < argc) {
            filename = argv[++i];
        } else if (arg == "--help") {
            print_help();
            return 0;
        } else {
            std::cerr << "未知参数: " << arg << std::endl;
            print_help();
            return 1;
        }
    }

    try {
        // 读取问题
        auto problems = read_problems_from_file(filename);
        
        // 求解
        SolverStatistics stats;
        if (parallel_mode) {
            stats = solve_parallel(problems, thread_count);
        } else {
            stats = solve_serial(problems);
        }

        // 输出结果
        print_statistics(stats, parallel_mode, thread_count);

    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
