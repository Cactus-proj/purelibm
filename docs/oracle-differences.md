# Oracle 差异记录

本文档记录 PureLibm、C23 数学库语义、MPFR / 高精度生成器、CORE-MATH 和手写特殊值表之间的结果差异。它不是失败日志；只有经过确认、需要保留上下文的差异才进入本文档。

## 判定规则

1. C23 数学库语义、函数规格表和 MPFR / 高精度生成器是正确性判断的主要依据。
2. 手写特殊值表补足 MPFR 不直接表达的 libm 语义，例如 signed zero、NaN 分类或 payload 策略。
3. CORE-MATH 是一致性 oracle，不是正确性证明。
4. 若 CORE-MATH 与 C23 语义、MPFR / 规格表冲突，以 C23 语义、MPFR / 规格表为准，并记录是否需要反馈上游。

## 记录模板

### `<function>`: `<short description>`

- 状态：open / resolved / upstream-reported / accepted-divergence
- 首次发现：YYYY-MM-DD
- 输入：
- PureLibm 结果：
- C23 期望语义：
- MPFR / 高精度结果：
- CORE-MATH 结果：
- 手写特殊值表结果：
- 比较规则：bitwise / classification / function-specific
- 结论：
- 后续动作：

## 当前差异

暂无已确认差异。
