# ADR 001: 浮点语义与验收边界

## 状态

Accepted

## 背景

PureLibm 的目标是提供可读、可移植、可验证的正确舍入数学库参考实现。实现开始前必须先冻结 API 合同和浮点语义，否则正确性判断会被标准版本、舍入模式、NaN 处理、signed zero、subnormal、FMA 收缩和编译器选项混在一起。

## 决策

API 合同使用 C23 数学库语义作为基准。每个 `cr_*` 函数的特殊值、定义域、极点、溢出、下溢和返回值分类都应按 C23 对相应数学函数的语义解释；若本项目需要偏离，必须写入该函数规格。

目标浮点格式为 IEEE 754 binary32 / binary64，默认舍入模式为 round-to-nearest-even。本项目不支持运行时动态舍入模式。测试和发布构建都假设调用方未改变默认舍入模式。

API 合同只保证 C23 语义下的返回值，不保证 `errno`、浮点异常标志或 `fetestexcept` 结果。signaling NaN 只规定返回值分类或位模式，不规定是否触发 `FE_INVALID`。

NaN 默认只要求结果分类为 NaN，不比较 payload。若某个函数需要保留 payload 或符号位，必须在该函数规格中单独声明。

`+0.0` 和 `-0.0` 视为不同结果，默认使用 bitwise 比较。subnormal 必须按 IEEE 754 语义保留，禁止依赖 flush-to-zero 或 denormals-are-zero 模式。

需要单次舍入的乘加必须显式使用 `fmaf()` 或 `fma()`。普通 `a * b + c` 不得依赖编译器自动收缩，也不得通过 `-ffp-contract=fast` 表达语义要求。

起步目标平台为常见 IEEE 754 平台。M1 的主验收平台为 `x86_64-unknown-linux-gnu` 上的 GCC stable，Clang stable 作为编译器交叉检查。Windows 支持后续加入，不作为 M0 blocker。项目不承诺 x87 扩展精度、非 IEC 60559 `float` 或默认 FTZ/DAZ 平台。

M1 构建系统必须支持至少一组严格浮点编译配置：

```sh
cc -std=c23 -Wall -Wextra -Wpedantic -Wconversion -Wdouble-promotion \
   -fno-fast-math -ffp-contract=off
```

其中 `-std=c23` 用于表达 API 合同对应的标准语义。若某个编译器尚未支持该 spelling 或上述 flag，`CMakeLists.txt` 必须记录等价替代或明确跳过原因。

## 后果

所有函数规格必须显式声明对应的 C23 语义、特殊值、比较规则和 oracle 来源。CORE-MATH 只能作为一致性 oracle；正确性 oracle 以 C23 语义、MPFR / 高精度生成器和手写特殊值表为准。

IEEE 754 位级 helper 在 M1 集中放入 `src/utils/ieee754.h`。`float` / `uint32_t` 和 `double` / `uint64_t` 转换必须使用 `memcpy`，禁止指针类型双关。
