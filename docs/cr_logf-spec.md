# cr_logf — 函数规格

## 基本信息

- 函数名：`cr_logf`
- C 签名：`float cr_logf(float x);`
- 目标格式：binary32
- 函数类型：一元
- C23 对应函数 / 条款：logf — C23 §7.12.6.7
- 参考来源：CORE-MATH `log/logf.c`
- 正确性 oracle：MPFR (`mpfr_log`)
- 一致性 oracle：CORE-MATH logf.c

## 语义合同

- API 合同：遵循 C23 §7.12.6.7。log(1)=0，log(+Inf)=+Inf，log(±0)=-Inf，log(x<0)=NaN
- 舍入模式：默认 round-to-nearest-even
- 比较规则：bitwise 相等（含 signed zero 区分）
- NaN 策略：NaN 分类相等
- signed zero：`+0.0` 与 `-0.0` 区分（log(±0) = -Inf，signed zero 仅影响 log(-0) 的 NaN 行为）
- subnormal：通过 portable clz 标准化，后续计算在 double 精度完成

## 特殊值表

| 输入 | 期望结果 | 比较规则 | 说明 |
|------|----------|----------|------|
| `+0.0` | `-Inf` | bitwise | log(0) = -Inf |
| `-0.0` | `-Inf` | bitwise | log(-0) = -Inf |
| `1.0` | `+0.0` | bitwise | log(1) = 0 |
| `+Inf` | `+Inf` | bitwise | log(+Inf) = +Inf |
| `x < 0` (finite) | qNaN | classification | domain error |
| quiet NaN | NaN | classification | 默认不比较 payload |
| signaling NaN | NaN | classification | FE_INVALID，返回 qNaN |
| subnormal | 正确舍入值 | bitwise | 通过 clz 标准化 + double 计算 |

## 算法与系数

- 特殊值处理：±0/-Inf/x<0/NaN/+Inf 分支
- range reduction：`x = 2^e * m`，通过 64-entry LUT 将 `m` 映射到 `z ≈ 0`，`log(x) = e*ln2 + log(m)`
- 核心近似（fast path）：degree-2 多项式 `b[]` 逼近 `log(1+z)`
- 核心近似（slow path）：degree-6 多项式 `c[]` 逼近 `log(1+z)`
- 亚正常处理：portable `clz32` 计算前导零，缩放指数
- FMA 使用点：无显式 FMA 调用；依赖 double 精度足够
- 系数来源脚本：`coeff/logf.sollya`
- 误差界：fast path 保守边界 1.45e-10；slow path 满足正确舍入

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
- [ ] 通过 f32 穷举测试（MPFR oracle）
- [ ] 通过 CORE-MATH 一致性测试
