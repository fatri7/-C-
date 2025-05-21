  # 项目简介

  本项目基于 C++ 实现了单纯形算法，并通过线程池机制显著提升批量线性规划（LP）问题的求解效率。适用于教学、实验。

  ## 目录结构说明

  
  - **main.cc**：统一线性规划求解器主程序，支持并行/串行模式
  - **paraSim**：无后缀C++头文件，串行 LP 求解器（SerialSolver），支持标准单纯形法与两阶段法
  - **paraSim_threadp**：无后缀C++头文件，并行 LP 求解器（ParallelSolver），基于线程池
  - **ThreadPool**：无后缀C++头文件，支持任务优先级调度的 C++ 线程池实现
  - **create_problems.py**：无后缀C++头文件，随机生成 LP 问题的 Python 脚本

  ---

  ## 主要功能

  - 支持批量 LP 问题的并行/串行求解
  - 自动处理不等式、等式约束及变量上下界
  - 详细的求解统计与异常处理
  - 线程池支持任务优先级与动态线程数调整
  - 提供 LP 问题自动生成脚本，便于测试与教学

  ---

  ## 组件简介

  ### 1. main.cc

  统一入口，支持如下命令行参数：

  ```sh
  ./unified_lp_solver [--parallel|--serial] [--threads N] [--file filename]
  ```

  - 并行模式：利用多线程加速批量求解
  - 串行模式：顺序求解，便于基准测试
  - 支持从 JSON 文件读取批量 LP 问题

  ### 2. SerialSolver（paraSim）

  - 支持标准单纯形法与两阶段法
  - 自动处理人工变量
  - 详细的求解步骤与摘要输出
  - 适合中小规模 LP 问题

  ### 3. ParallelSolver（paraSim_threadp）

  - 基于线程池的批量/单个 LP 问题并行求解
  - 自动收集所有结果
  - 依赖 SerialSolver 实现具体求解逻辑

  ### 4. ThreadPool

  - 多线程并发执行任务，支持优先级调度
  - 动态调整线程数，线程安全
  - 适用于高效任务调度和多线程场景

  ### 5. create_problems.py

  - 随机生成 LP 问题，支持批量导出为 JSON
  - 目标函数、约束、变量上下界均随机
  - 适合算法测试、教学或自动化生成 LP 数据集
  
  ### 6. problems.json

  - 由create_problems.py生成的、符合格式要求的问题集合
  - 在改动文件名时请注意，生成、读取的文件名均为此，一改均改

  ### 7. unified_lp_solver

  - 编译产生的可运行程序，具体使用见后文
  - 将其与problems.json放在一起，自动读取


  ---

  ## 依赖

  - C++11 及以上标准
  - Python 3.x（仅用于问题生成脚本）
  - Python 标准库：random, json
  - 需要 GNU Make、g++（支持 C++17 及以上）
  - 支持 POSIX 线程（Linux/macOS 推荐）

  ### 编译与运行

  项目根目录下提供 `Makefile`，主要内容如下：

  ```makefile
  CXX := g++
  CXXFLAGS := -std=c++17 -O2 -Wall -g -pthread
  LDFLAGS := -pthread

  TARGET := unified_lp_solver
  SOURCES := $(wildcard *.cc)
  OBJS := $(SOURCES:.cc=.o)
  HEADERS := ThreadPool paraSim paraSim_threadp json.hpp

  .PHONY: all clean

  all: $(TARGET)

  $(TARGET): $(OBJS)
    $(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

  %.o: %.cc $(HEADERS)
    $(CXX) $(CXXFLAGS) -c $< -o $@

  clean:
    rm -f $(OBJS) $(TARGET)
  ```

  #### 说明

  - `all`：默认目标，编译所有源文件并生成可执行文件 `unified_lp_solver`
  - `clean`：清理所有中间文件和可执行文件
  - 依赖头文件：`ThreadPool`、`paraSim`、`paraSim_threadp`、`json.hpp`
  - 支持多线程编译与链接（`-pthread`）

  ### 编译命令

  ```sh
  make
  ```

  ### 清理命令

  ```sh
  make clean
  ```
  ---
  ## 用法:
    -unified_lp_solver [选项]
    选项:
    --parallel      使用并行模式 (默认)
    --serial        使用串行模式
    --threads N     设置并行线程数 (默认: 4)
    --file FILE     指定输入文件 (默认: problems.json)
    --help          显示帮助信息
  ## 测试：
  ```sh
  python3 create_problems.py ; make ; ./unified_lp_solver ;./unified_lp_solver --serial
  ```
```
请输入要生成的线性规划问题数量: 50000
请输入输出文件名(直接回车使用默认文件名 `problems.json`): 
成功生成 50000 个线性规划问题
已保存到 problems.json
g++ -std=c++17 -O2 -Wall -g -pthread -c main.cc -o main.o
g++ -std=c++17 -O2 -Wall -g -pthread main.o -o unified_lp_solver -pthread
============ 求解汇总 ============
模式: 并行 (线程数: 4)
求解问题总数: 50000
成功解决的问题数: 4106
失败的问题数: 45894
总迭代次数: 80624
总求解时间(各问题累加): 3427.11 毫秒
实际总耗时: 981 毫秒
并行加速比: 3.49x
答案与题目详情见answer_parallel.txt
平均每个问题耗时: 0.02 毫秒

============ 求解汇总 ============
模式: 串行
求解问题总数: 50000
成功解决的问题数: 4106
失败的问题数: 45894
总迭代次数: 80624
总求解时间(各问题累加): 2500.79 毫秒
实际总耗时: 2565 毫秒
答案与题目详情见answer_serial.txt
平均每个问题耗时: 0.0513 毫秒
```

  ## 注意事项

  - 仅适合中小规模 LP 问题（大规模效率有限）
  - 不支持整数规划、二次规划等扩展问题
  - 线程池不支持任务取消、暂停等高级特性
  - 生成的 LP 问题未必严格可行，仅提高可行性概率

  ---

  ## 作者

  fatri7，2025

