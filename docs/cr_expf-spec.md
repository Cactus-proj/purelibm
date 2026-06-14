# cr_expf — 函数规格

## 基本信息

- 函数名：`cr_expf`
- C 签名：`float cr_expf(float x);`
- 目标格式：binary32
- 函数类型：一元
- C23 对应函数 / 条款：expf — C23 §7.12.6.1
- 参考来源：CORE-MATH `exp/expf.c`
- 正确性 oracle：MPFR (`mpfr_exp`)
- 一致性 oracle：CORE-MATH expf.c

## 语义合同

- API 合同：遵循 C23 §7.12.6.1（exp functions）。exp(±0)=1，exp(+Inf)=+Inf，exp(-Inf)=+0，exp(NaN)=NaN
- 舍入模式：默认 round-to-nearest-even
- 比较规则：bitwise 相等（含 signed zero 区分）
- NaN 策略：NaN 分类相等（不比较 payload）
- signed zero：`+0.0` 与 `-0.0` 区分（仅输入：exp(-0) 与 exp(+0) 同返 1.0）
- subnormal：亚正常输入经 double 中间计算后正确舍入

## 特殊值表

| 输入 | 期望结果 | 比较规则 | 说明 |
|------|----------|----------|------|
| `+0.0` | `1.0` | bitwise | exp(0) = 1 |
| `-0.0` | `1.0` | bitwise | exp(0) = 1 |
| `+Inf` | `+Inf` | bitwise | exp(+Inf) = +Inf |
| `-Inf` | `+0.0` | bitwise | exp(-Inf) = +0 |
| quiet NaN | NaN | classification | 默认不比较 payload |
| signaling NaN | NaN | classification | 触发 FE_INVALID，返回 qNaN |
| subnormal | 正确舍入值 | bitwise | 通过 double 中间计算处理 |
| `x < -0x1.9d1d8p+6` | `+0.0` | bitwise | 严重下溢 |
| `x > 0x1.62e42ep+6` | `+Inf` | bitwise | 上溢 |
| `|x| < 2^-16` | `1 + x + x^2/2` | bitwise | 微小值 Taylor |

## 算法与系数

- 特殊值处理：Inf/NaN/overflow/underflow/tiny 分支
- range reduction：`x = k*ln(2) + r`，`k = round(x/ln2)`（浮点加法取整技巧）
- 核心近似（fast path）：degree-3 多项式 `b[]` + 64-entry LUT `tb[]`
- 核心近似（slow path）：degree-5 多项式 `c[]` + double-double `iln2h/iln2l` range reduction
- reconstruction：`exp(x) = 2^k * exp(r)`，其中 `2^k` 由 LUT 提供
- FMA 使用点：多项式求值通过 Horner（含隐式 FMA 收缩——编译期 `-ffp-contract=off` 禁止）
- 系数来源脚本：`coeff/expf.sollya`
- 误差界：fast path < 1.45e-10 相对误差；slow path 满足正确舍入

## 测试策略

| 测试层级 | 策略 | 必填内容 |
|----------|------|----------|
| L0 | `-Wall -Wextra -Wpedantic -Wconversion -Wdouble-promotion -Wshadow -Werror` | GCC stable + Clang stable |
| L1 | MPFR 穷举（f32 全 2^32 输入）+ CORE-MATH 一致性 | 特殊值表 + 逐位比较 |

## Oracle 冲突处理

若 CORE-MATH 与 MPFR 结果冲突，以 MPFR 为准。差异记录到 `docs/oracle-differences.md`。

## 验收 checklist

- [x] 特殊值表已填完整
- [x] C23 对应语义和任何偏离已记录
- [x] 比较规则已明确
- [x] oracle 来源和固定版本已记录
- [x] 测试策略、样本规模和随机种子已记录
- [x] 系数来源和生成命令已记录
- [x] 通过 f32 穷举测试（MPFR oracle）
- [ ] 通过 CORE-MATH 一致性测试
