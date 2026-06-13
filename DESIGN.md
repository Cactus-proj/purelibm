# PureLibm 项目设计

**状态**: 草案 — 从 PureLibm.jl 的经验和 CORE-MATH 的算法出发，构建一个可读、可移植、可验证的正确舍入数学库参考实现。

*本文档随项目推进持续更新。*

**2026-06 更新**: 经语言选择讨论，确定主实现语言为 **C**。PureLibm-rs（Rust 版）设计文档保留作为参考，C 版独立设计。
*初始版本: 2026-05 · 语言决策更新: 2026-06*

---

## 0. 语言选择决策

### 背景

最初设计（PureLibm-rs）以 Rust 为主实现语言，考虑了 Rust 的类型安全、borrow checker 和 Kani/Creusot 等验证工具。经讨论后，将项目目标分解为两层：

- **参考实现层**：可读性 + 可移植性 — 需要最大公约数的语言
- **形式化验证层**：探索性 — 不限定语言，按函数选择工具

### 决策

**参考实现使用 C。**

| 考虑因素 | 评估 |
|----------|------|
| **可读性** | C 的瓶颈不在语法，在文档和注释。对于 ~40 个函数的规模，前缀约定足以管理命名空间。C 对数值计算研究者是透明的——没有模板、重载、隐式类型推导的认知负担 |
| **可移植性** | C ABI 是所有语言的 FFI 通用语。Julia、Python、Rust、MATLAB 零摩擦调用。C 标准库是任何平台的最小公分母 |
| **与 CORE-MATH 的关系** | 同语言直接对比、直接合入、直接回馈。消除"翻译"这一额外变量 |
| **构建** | CMake 是 C 项目的标准构建系统，跨平台一致 |
| **最大公约数** | 后续任何行动（Julia 移植、Rust 翻译、Frama-C 验证、FFI 绑定）都不需要先理解 C++ 语义再翻译 |

### 为何不是 C++

C++ 的 namespace、constexpr、函数重载提供了更优雅的代码组织，但：

- 对 ~40 个函数的规模，命名前缀的视觉噪音完全可接受
- 参考实现的读者是数值计算研究者，不是 C++ 工程师
- C++ 模板增加了"这个展开是什么意思"的阅读负担
- C++ 没有像 Frama-C 这样成熟的浮点形式化验证工具

### 为何不是 Rust

Rust 的类型安全和验证工具仍然有价值，但不是主实现的首选。理由：

- 参考实现的首要目标读者不需要理解 borrow checker
- LLVM 的浮点弱约束（舍入模式不可用、NaN 非确定性、无 fenv）对"正确舍入"的规格定义构成障碍
- Rust 不排除——在形式化验证探索中，对单个函数做 Rust 翻译 + Kani 验证仍是一条路径

### 已排除的选项

- **C++**：见上
- **Rust 作为主实现**：参考实现用 C，Rust 保留为验证层的可选语言
- **Julia 作为主实现**：Julia 是独立项目（PureLibm.jl），不在此项目范围

---

## 1. 项目定位

**PureLibm** 是一个以**可读性**和**正确性**为首要目标的正确舍入（correctly rounded）数学库参考实现，使用 **C** 编写。

### 核心原则

1. **参考实现** — 代码贴近算法的数学表达形式。不做混淆性的位优化。优先使用语义化操作和具名 helper 而非魔法常量。目标读者是"想理解算法的人"，不只是"想跑结果的人"。
2. **详细文档** — 每个函数带完整的算法描述（特殊值处理 → range reduction → 多项式逼近 → reconstruction）、交叉引用（论文、CORE-MATH 源码行）、系数来源标注。
3. **魔术参数均有出处** — 每个魔法常数标注生成脚本（Sollya/Gappa）、生成命令、脚本路径。脚本放在 `coeff/` 目录下，有 Sollya 环境即可复现。
4. **完备的测试** — 一元 f32 全域穷举 + 多元 f32 分层测试 + f64 随机/边界测试 + 高精度 oracle 对比。正确性 oracle 使用 MPFR；一致性 oracle 使用 CORE-MATH。

### 与 CORE-MATH 的关系

| 维度 | 关系 |
|------|------|
| 算法来源 | 从 CORE-MATH C 源码出发，同语言参考，修改以提高可读性和可移植性 |
| 正确性 oracle | 通过高精度参考生成期望返回值 |
| 一致性 oracle | CORE-MATH 源码直接纳入测试，对比返回值 |
| License | 延续 MIT + Apache-2.0 |
| 新贡献 | 可读性改进、文档、系数追溯、消除 GNU C 扩展、消除无意的隐式类型提升 |
| 可逆性 | 发现的 CORE-MATH bug 回馈上游 |

---

## 2. 验证策略

### 2.1 主线测试（以可读性 + 可移植性为目标）

| 级别 | 手段 | 保证内容 |
|------|------|---------|
| **L0** | C 编译器警告（`-Wall -Wextra -Wpedantic`）+ 静态分析（clang-tidy / cppcheck）+ 单元测试 | 消除明显 UB、整数溢出、越界 |
| **L1** | 一元 float32 全域穷举 + 多元 float32 分层采样/边界测试 + float64 随机/边界测试。正确性 oracle 使用 MPFR；一致性 oracle 使用 CORE-MATH | 一元 f32 在全输入域符合正确舍入目标；多元 f32 和 f64 在声明的测试范围内与参考一致 |

**L0–L1 的快速子集是每次提交的 CI。** 一元 f32 全域穷举和大规模采样为 nightly/cron。

### 2.2 探索性验证（函数级，可选）

| 级别 | 手段 | 说明 |
|------|------|------|
| **L2** | CBMC / Frama-C（对 range reduction 的整数逻辑做 bounded model check） | 防御性——消除低级错误，不强求全覆盖。不是"形式化证明正确舍入"，而是"模型检查发现越界和溢出" |
| **L3** | Sollya + Gappa 脚本 | 生成多项式逼近的误差边界证明。记录在 `coeff/` |
| **L4** | Coq / Flocq / 其他 | 完整正确舍入证明。CORE-MATH 已有部分函数完成，观察 + 等待，不做当前承诺 |

**L2–L4 不作为 CI 的 blocking check。** L2 在能跑的范围内跑，L3 作为手工验证文档，L4 是远期观察方向。

### 2.3 验证与语言的关系

- 参考实现用 C 写。验证服务于实现，不是反过来——不为验证换语言
- 防御性验证（L2）在 C 工具链范围内做
- 对特定函数的深入验证探索，可以翻译到 Rust + Kani 或任何合适的环境——但这是独立的探索分支，成果是博客，不是"参考实现通过验证"的承诺
- 等价模型 → 生成可执行代码：当前形式化验证还不能替代穷举测试，保持观察

### 2.4 Oracle 分层

1. **正确性 oracle**：MPFR 高精度参考。一元 f32 全域穷举对比；多元 f32 按函数规格做分层测试；f64 采样对比。
2. **一致性 oracle**：CORE-MATH 源码直接纳入测试。若 CORE-MATH 和 MPFR 结果冲突，以 MPFR 为准，记录差异并反馈上游。

---

## 3. 项目结构

```
purelibm/
├── CMakeLists.txt             ← 构建
├── README.md
├── DESIGN.md                 ← 本设计文档
├── src/
│   ├── purelibm.h           ← 公开 API 头文件
│   ├── utils/               ← 共享工具（内联在 header 或 .c）
│   │   ├── ieee754.h        ← IEEE 754 编码层 helper（extract_exponent, extract_mantissa 等）
│   │   ├── poly.h           ← 多项式求值辅助（Horner, Estrin, double-double）
│   │   └── math_util.h      ← 常用数学辅助（double-double 算术等）
│   └── math/
│       ├── binary32/        ← float 实现
│       │   ├── sinf.c
│       │   ├── cosf.c
│       │   ├── expf.c
│       │   ├── logf.c
│       │   ├── sqrtf.c
│       │   └── ...
│       └── binary64/        ← double 实现（逐步覆盖）
│           └── ...
├── coeff/                    ← 系数来源脚本
│   ├── sinf.sollya
│   ├── expf.sollya
│   └── ...
├── docs/                     ← 参考文档
│   ├── adr/                  ← Architecture Decision Records
│   │   ├── 001-language-choice.md
│   │   └── 001-floating-semantics.md
│   ├── function-spec-template.md
│   ├── oracle-differences.md
│   ├── references.md         ← 论文、标准、参考实现索引
│   └── translation-notes.md  ← 从 CORE-MATH 翻译改进的注意事项和模式总结
├── tests/
│   ├── test_common.h        ← 测试辅助
│   ├── test_sinf.c
│   ├── test_expf.c
│   ├── test_exhaust_sinf.c  ← f32 穷举测试（需要高精度 oracle 链接）
│   └── ...
└── deps/
    └── core-math/            ← git submodule，锁定指定 commit（作为一致性 oracle）
```

### 单个函数的文档模板

```c
/**
 * cr_sinf — 正确舍入的单精度正弦函数
 *
 * 正确性保证:
 *   - 正确性 oracle: MPFR，f32 穷举测试已通过
 *   - 一致性 oracle: CORE-MATH sinf.c (commit a1b2c3d)
 *
 * 算法描述:
 *   1. 特殊值处理（NaN, Inf, subnormal）
 *   2. Range reduction (Payne-Hanek)
 *   3. 多项式逼近 (Sollya remez, degree 5, error < 2^-28)
 *   4. Reconstruction
 *
 * 系数来源:
 *   Sollya 脚本: coeff/sinf.sollya
 *   Command: remez(sin(2*pi*x), 5, [-2^-12, 2^-12]);
 *
 * References:
 *   - [CORE-MATH sinf.c (commit a1b2c3d)](https://github.com/...)
 *   - Payne & Hanek, "Radian Reduction for Trigonometric Functions", 1983
 *   - Muller, "Elementary Functions", 3rd ed., Ch. 10
 */
float cr_sinf(float x);
```

### 位操作策略

| 不鼓励 | 推荐 |
|--------|------|
| 内联 `bits & 0x7fffffff` 取绝对值 | 优先使用 `fabsf(x)`；需要保留 NaN payload 时用 helper |
| 内联 `bits ^ 0x80000000` 翻转符号 | 优先使用 `-x` 或 `copysignf(x, y)` |
| 位运算做 fast-ceil/fast-floor | 优先使用 `ceilf()` / `floorf()` |

IEEE 754 编码层面的操作（提取指数、尾数、区分 signed zero）封装为具名函数：

```c
// utils/ieee754.h — 集中维护，注释说明 IEEE 754 语义
static inline uint32_t ieee754_to_bits_f32(float x) {
    uint32_t bits;
    memcpy(&bits, &x, sizeof(bits));
    return bits;
}

static inline int ieee754_extract_exponent_f32(float x) {
    return (ieee754_to_bits_f32(x) >> 23) & 0xff;
}
static inline uint32_t ieee754_extract_mantissa_f32(float x) {
    return ieee754_to_bits_f32(x) & 0x007fffff;
}
```

### 架构决策摘要

| 问题 | 决策 |
|------|------|
| 主实现语言 | **C11**（禁用 GNU C 扩展，不使用 C23 特性） |
| 标准依赖 | `<math.h>` (C99) 的 `fmaf`/`fma` 替代 `__builtin_fma` |
| fenv/errno | API 合同只保证返回值，不保证 errno 或浮点异常标志 |
| FMA 使用 | 使用 C99 `fmaf()` / `fma()` 表达 FMA 语义；不要求硬件 FMA |
| 命名前缀 | 公开 API: `cr_` 前缀（`cr_sinf`）；内部 helper: `ieee754_` 或 `purelibm_` 前缀 |
| 隐式类型提升 | 禁止无意提升。启用 `-Wdouble-promotion`；故意使用 `double` / double-double 时必须在函数文档中说明 |
| C standard | C11；使用 `_Static_assert` 固化浮点表示前提，不使用 C23 特性 |

---

## 4. M0：语义与验收合同

M0 的目标是在实现第一个函数前冻结“什么算通过”。正确舍入数学库的失败常常不是算法错，而是编译器浮点语义、特殊值比较、位级 helper 或测试范围没有提前定义。M0 不产出函数覆盖率，只产出可执行合同。

### 4.1 浮点语义合同

| 项目 | 决策 |
|------|------|
| 目标语义 | IEEE 754 binary32 / binary64，默认 round-to-nearest-even |
| 舍入模式 | 测试和发布构建均假设默认舍入模式；不支持运行时动态舍入模式 |
| `errno` / fenv flags | API 只保证返回值，不保证 `errno`、异常标志或 `fetestexcept` 结果 |
| NaN | 正确性测试默认只要求结果为 NaN，不比较 payload；若函数需要保留 payload，必须在函数规格中单独声明 |
| signed zero | bitwise 比较，`+0.0` 与 `-0.0` 视为不同结果 |
| subnormal | 默认要求保留 subnormal 语义；禁止依赖 flush-to-zero / denormals-are-zero 模式 |
| FMA | 需要单次舍入时必须显式使用 `fmaf()` / `fma()`；普通 `a * b + c` 不得依赖编译器自动收缩 |
| 平台 | 起步支持常见 IEEE 754 平台；不承诺 x87 扩展精度、非 IEC 60559 `float`、或默认 FTZ/DAZ 平台 |

第一批 CI 平台矩阵：

| 平台 | 编译器 | 角色 |
|------|--------|------|
| `x86_64-unknown-linux-gnu` | GCC stable | 主验收平台 |
| `x86_64-unknown-linux-gnu` | Clang stable | 编译器交叉检查 |
| `x86_64-windows-msvc` 或 `x86_64-windows-gnu` | MSVC / MinGW | 后续可移植性检查，不作为 M0 blocker |

非主验收平台必须先记录浮点环境差异，再决定是否纳入 blocking CI。

编译配置必须显式禁止会破坏上述语义的选项。CI 至少包含一组“严格浮点”构建：

```sh
cc -std=c11 -Wall -Wextra -Wpedantic -Wconversion -Wdouble-promotion \
   -fno-fast-math -ffp-contract=off
```

需要 FMA 语义的代码通过 `fmaf()` / `fma()` 表达，不通过 `-ffp-contract=fast` 或目标 CPU 特性表达。若某个编译器不支持上述 flag，`CMakeLists.txt` 必须记录等价替代或跳过原因。

### 4.2 位级表示与 UB 合同

所有 IEEE 754 编码层 helper 必须集中在 `src/utils/ieee754.h`，并遵守以下规则：

1. `float` / `uint32_t`、`double` / `uint64_t` 之间的转换使用 `memcpy` helper，不使用指针类型双关。
2. helper 顶部用 `_Static_assert` 检查 `sizeof(float) == 4`、`sizeof(double) == 8`、`CHAR_BIT == 8`。
3. 位操作只用于 IEEE 754 语义本身：提取指数/尾数、构造 signed zero、处理 subnormal、区分 NaN/Inf。普通数值操作优先使用 `<math.h>` 函数。
4. 每个 helper 的注释说明它保留或丢弃哪些语义，例如 NaN payload、符号位、subnormal。

### 4.3 Oracle 与比较规则

每个函数在实现前必须有一张规格表，至少覆盖：

- 输入域和目标返回值语义
- 特殊值：`+0.0`、`-0.0`、`+Inf`、`-Inf`、quiet NaN、signaling NaN、subnormal
- 正常路径的目标舍入模式
- 是否要求 bitwise 相等，还是只要求分类相等
- CORE-MATH 与 MPFR 结果冲突时的处理规则

signaling NaN 只规定返回值分类或位模式；本项目不规定 `FE_INVALID` 等浮点异常标志是否被触发。

Oracle 分层固定如下：

| Oracle | 用途 | 决策权 |
|--------|------|--------|
| MPFR / 高精度生成器 | 正确性 oracle，生成目标舍入结果 | 最高 |
| CORE-MATH 固定 commit | 一致性 oracle，检查本项目没有偏离参考实现 | 次级 |
| 手写特殊值表 | 补足 MPFR 不直接表达的 libm 返回值语义 | 与函数规格绑定 |

若 CORE-MATH 与 MPFR / 规格表冲突，以规格表和 MPFR 为准；差异必须进入 `docs/oracle-differences.md`，并记录是否需要反馈上游。

### 4.4 测试范围合同

`f32 穷举` 只对一元 binary32 函数默认成立。测试策略按函数元数分流：

| 函数类型 | L1 测试策略 |
|----------|-------------|
| 一元 f32 | 全输入域穷举 + 特殊值表 + CORE-MATH 一致性 |
| 二元/多元 f32 | 边界笛卡尔积 + 随机采样 + hard-to-round cases + 差异回归集 |
| f64 | 随机采样 + 边界值 + hard-to-round cases；不承诺全域穷举 |

每个函数的测试文件必须声明采用哪种策略、样本规模、随机种子策略和失败样例归档位置。CI 中每次提交运行快速 L0/L1 子集；全域穷举和大规模采样运行在 nightly/cron。

### 4.5 M0 交付物

M0 完成的硬标准：

1. `docs/adr/001-floating-semantics.md`：记录浮点语义、编译 flag、平台假设。
2. `docs/function-spec-template.md`：单函数规格表模板。
3. `docs/oracle-differences.md`：记录 oracle 冲突的格式。
4. `src/utils/ieee754.h`：无 strict-aliasing UB 的位级 helper 原型。
5. `tests/` 中有一个最小 oracle smoke test，覆盖 signed zero、NaN、Inf、subnormal。
6. CI 至少有严格浮点构建和普通调试构建两条路径。

只有 M0 通过后，才进入 M1 基础设施和 M2 pilot 函数。

---

## 5. Pilot 阶段：2-3 个函数验证流程

为确保工作流合理，首先选择 2-3 个原型函数完成全链路验证。

### 选择策略

| 函数 | 复杂度 | 验证价值 |
|------|--------|---------|
| `cr_sqrtf` | **低** — 分支少，range reduction 简单 | 验证基础设施：构建、测试编排、MPFR oracle 对接、coverage 工具链 |
| `cr_sinf` | **高** — 复杂 range reduction（Payne-Hanek）、多项式逼近、多条件分支 | 验证核心改写流程 + 文档模板 + 系数追溯格式 |
| `cr_expf` | **中高** — 特殊值路径多（overflow/denormal, subnormal） | 补充异常路径覆盖 |

Pilot 阶段的验收标准：

1. `cr_sqrtf` 和 `cr_sinf` 全部 f32 输入穷举测试通过（对比 MPFR）
2. `cr_expf` 随机 + 边界值测试通过
3. CORE-MATH 一致性测试通过（`cr_sqrtf` + `cr_sinf`）
4. 文档模板（含系数来源标注）定稿
5. 构建系统（CMake）可用，CI 配置完成
6. 每个 pilot 函数都有完整规格表，并明确特殊值、signed zero、NaN 和 subnormal 语义

Pilot 通过后，每个函数复用文档模板和测试框架；但每个函数仍需独立确认特殊值策略、hard-to-round cases、系数来源、range reduction 边界和 oracle 差异。

### 参考来源

- **CORE-MATH C 源码**：算法主参考。需要消除 GNU C 扩展（`__builtin_*` → C99 标准替代）、无意的隐式类型提升（故意提升需注释说明）和魔法常量（加注释 + 系数脚本）
- **PureLibm.jl**：系数命名和注释风格参考
- **MPFR**：正确性 oracle，通过 C API 直接调用

---

## 6. 可读性要求细则

### 6.1 系数追溯

每个魔法常数标注来源：

```c
// Generated by: Sollya
// Script: coeff/sinf.sollya
// Command: remez(sin(2*pi*x), 5, [-2^-12, 2^-12]);
static const double CR_SINF_C1 = 0x1.921fb54442d17p-3;
```

`coeff/` 目录下提供可独立运行的 `.sollya` 脚本。

### 6.2 消除 GNU C 扩展

| CORE-MATH 原文 | PureLibm 改写 |
|---------------|--------------|
| `__builtin_fma(x, y, z)` | `fmaf(x, y, z)` (C99 `<math.h>`) |
| `__builtin_expect(cond, 0)` | 移除（不依赖分支预测提示） |
| `__attribute__((aligned))` | C11 `_Alignas` 或移除 |
| 无意的 `float` → `double` 提升 | 启用 `-Wdouble-promotion`；故意提升必须注释说明 |

### 6.3 引用文献

```c
/**
 * # References
 * - [CORE-MATH sinf.c (commit a1b2c3d)](https://github.com/...)
 * - J.-M. Muller, "Elementary Functions: Algorithms and Implementation", 3rd ed.
 * - Payne & Hanek, "Radian Reduction for Trigonometric Functions", 1983
 */
```

---

## 7. 路线图（里程碑）

### M0 — 语义与验收合同冻结

- 浮点语义合同、编译 flag、平台假设写入 ADR
- 单函数规格表模板、oracle 差异记录格式完成
- IEEE 754 helper 原型无 strict-aliasing UB
- oracle smoke test 覆盖 signed zero、NaN、Inf、subnormal

### M1 — 基础设施就绪

- C 项目骨架（目录结构、CMakeLists.txt、CI 配置）
- MPFR oracle 测试框架完成；CORE-MATH 以 git submodule 纳入，一致性测试框架完成
- 穷举测试 harness 可用
- `src/utils/` 基础工具（IEEE 754 helper、多项式求值、double-double 算术）

### M2 — Pilot 函数验证

- `cr_sqrtf`：实现 + f32 MPFR 穷举测试通过 + 文档定稿
- `cr_sinf`：实现 + f32 MPFR 穷举测试通过 + 文档（含完整系数追溯）
- `cr_expf`：实现 + 随机/边界测试通过
- 系数脚本（`coeff/sqrtf.sollya`, `coeff/sinf.sollya` 等）补全

### M3 — Float32 全覆盖

- 全部约 40 个 CORE-MATH binary32 函数完成（实现 + 文档 + 测试 + 系数脚本）
- 一元 f32 函数完成全输入域穷举；二元/多元函数完成分层采样、边界组合和差异回归测试
- 代码可读性审查

### M4 — Float64 拓展

- 先行 double 函数实现
- Float64 测试框架（随机 + 边界 + MPFR 对比）
- 随 CORE-MATH 的 binary64 实现进度逐步迁移

### M5 — 深化（远期，不做当前承诺）

- 探索性验证（L2 CBMC/Frama-C 对选定函数的 range reduction）
- L3–L4 对接（Coq/Flocq/Gappa 证明整合）
- Julia 移植（PureLibm.jl）
- Rust 翻译 + Kani 验证（PureLibm-rs）
