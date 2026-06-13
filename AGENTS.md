# 仓库指南

## 项目结构与模块组织

本仓库目前处于 M1 基础设施阶段。当前项目文件包括：

- `README.md`：项目简介和许可证说明。
- `DESIGN.md`：架构、验证等级、浮点语义和路线图的主要依据。
- `src/`：公开头文件和共享 C helper。
- `tests/`：CTest 测试、MPFR smoke test 和穷举 harness 骨架。
- `deps/core-math/`：CORE-MATH submodule，作为一致性 oracle 的基础。
- `docs/`：ADR、函数规格、oracle 差异、参考文献和翻译记录。

新增实现文件时，遵循 `DESIGN.md` 中规划的布局：公开头文件放在 `src/`，共享 helper 放在 `src/utils/`，单精度实现放在 `src/math/binary32/`，双精度实现放在 `src/math/binary64/`，系数生成脚本放在 `coeff/`，测试放在 `tests/`。

## 构建、测试与开发命令

本仓库使用 CMake、Ninja 和 CTest：

- `git submodule update --init --recursive`：初始化 CORE-MATH submodule。
- `cmake -S . -B build -G Ninja -DPURELIBM_REQUIRE_MPFR=ON -DPURELIBM_REQUIRE_CORE_MATH=ON`：配置本地构建目录并强制检查 MPFR / CORE-MATH。
- `cmake --build build`：编译库和测试。
- `ctest --test-dir build`：运行已配置的测试套件。

提交前可运行 `git diff --check`，检查尾随空格和行尾问题。

## 代码风格与命名约定

参考实现使用 C，API 合同以 C23 数学库语义为准。避免 GNU C 扩展；除非 M1 构建合同明确需要，不引入无关的 C23 语法特性。公开 API 使用 `cr_` 前缀，例如 `cr_sinf`；内部 helper 使用 `ieee754_` 或 `purelibm_` 前缀。

优先写清晰、带文档说明的数学代码，避免不透明的位技巧。需要表达 FMA 语义时，使用 `<math.h>` 中的 `fmaf()` 和 `fma()`。IEEE 754 位级操作集中放在 `src/utils/ieee754.h`，并使用基于 `memcpy` 的转换，避免类型双关。

## 测试指南

测试要求以 `DESIGN.md` 为准：L0 包括编译器警告和静态检查，L1 包括 MPFR 与 CORE-MATH oracle 测试，更深入的验证可选用 CBMC、Frama-C、Sollya 或 Gappa。

按函数命名聚焦测试，例如 `tests/test_sinf.c`；高成本穷举测试应在文件名中体现，例如 `tests/test_exhaust_sinf.c`。每个函数都应记录对应 C23 语义、特殊值行为、signed zero 处理、NaN 策略、subnormal 处理、oracle 来源和采样策略。

## 提交与 Pull Request 指南

使用 Conventional Commits，保持与现有历史一致：`docs: add PureLibm design document`、`chore: initialize purelibm project`。只有在能提高清晰度时才添加 scope，例如 `test(sinf): add exhaustive harness`。

Pull Request 应包含简短摘要、已运行的验证命令，以及相关 issue 或设计章节链接。对于数学函数，还应说明 oracle 细节、系数来源，并将任何 CORE-MATH 差异记录到 `docs/oracle-differences.md`。
