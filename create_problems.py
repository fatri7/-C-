"""
create_problems.py
功能说明:
-----------
本脚本用于随机生成线性规划（Linear Programming, LP）问题，并可将生成的问题保存为JSON文件。每个问题包含目标函数、等式/不等式约束、变量上下界及最大化/最小化类型。适用于算法测试、教学或自动化生成LP数据集。
主要特点:
-----------
- 随机生成目标函数系数、约束矩阵、约束向量、变量上下界。
- 支持不等式和等式约束，数量随机。
- 变量维度（n）和约束数量自动随机确定，增强多样性。
- 目标类型（最大化/最小化）随机选择。
- 支持批量生成多个问题，并可保存为JSON文件。
- 生成的约束右端项（b, beq）基于系数组合，尽量保证可行性。
使用方法:
-----------
1. 直接运行脚本，按提示输入要生成的问题数量。
2. 生成的问题将以列表形式保存到默认JSON文件 `problems.json`，或在终端输出第一个问题示例。
主要函数:
-----------
- generate_random_lp_problem(): 生成单个随机LP问题，返回字典。
- generate_lp_problems(num_problems, output_file=None): 批量生成LP问题，并可保存到文件。
限制说明:
-----------
- 生成的LP问题未必保证严格可行或有唯一解，仅通过右端项的生成方式提高可行性概率。
- 变量上下界、系数范围等参数为固定随机区间，如需调整请修改代码。
- 仅支持标准形式的线性规划问题（不支持整数约束、二次约束等）。
- 生成的约束和目标函数系数为整数，适合教学和测试用途。
依赖:
-----------
- Python 3.x
- 标准库: random, json
作者: fatri7
"""
import random
import json

def generate_random_lp_problem():
    # 随机确定维度（5到20）
    n = random.randint(5, 20)
    
    # 随机确定不等式约束数量（1到n+2）
    m_ineq = random.randint(1, n+2)
    
    # 随机确定等式约束数量（0到n//2）
    m_eq = random.randint(0, max(1, n//2))
    
    # 随机生成目标函数系数
    c = [random.randint(-10, 10) for _ in range(n)]
    
    # 随机生成不等式约束矩阵A和向量b
    A = []
    b = []
    for _ in range(m_ineq):
        row = [random.randint(-5, 5) for _ in range(n)]
        A.append(row)
        # b值设为约束系数的某种组合，确保可行性
        b_val = sum(abs(x)*random.randint(1, 3) for x in row)
        b.append(b_val)
    
    # 随机生成等式约束矩阵Aeq和向量beq
    Aeq = []
    beq = []
    for _ in range(m_eq):
        row = [random.randint(-3, 3) for _ in range(n)]
        Aeq.append(row)
        # beq值设为约束系数的某种组合
        beq_val = sum(abs(x)*random.randint(1, 2) for x in row)
        beq.append(beq_val)
    
    # 随机生成下界和上界
    lb = [random.randint(0, 3) for _ in range(n)]
    ub = [lb[i] + random.randint(3, 10000) for i in range(n)]
    
    # 随机决定是最大化还是最小化
    ismaximization = random.choice([True, False])
    
    return {
        "c": c,
        "A": A,
        "b": b,
        "Aeq": Aeq,
        "beq": beq,
        "lb": lb,
        "ub": ub,
        "ismaximization": ismaximization
    }

def generate_lp_problems(num_problems, output_file="problems.json"):
    problems = [generate_random_lp_problem() for _ in range(num_problems)]
    
    if output_file:
        with open(output_file, 'w') as f:
            json.dump(problems, f, indent=2)
    
    return problems

if __name__ == "__main__":
    num_problems = int(input("请输入要生成的线性规划问题数量: "))
    output_file = input("文件名 `problems.json`): ").strip() or "problems.json"
    
    problems = generate_lp_problems(num_problems, output_file)
    
    print(f"成功生成 {len(problems)} 个线性规划问题")
    print(f"已保存到 {output_file}")

