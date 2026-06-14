# cr_sqrtf — 函数规格

## 基本信息

- 函数名：`cr_sqrtf`
- C 签名：`float cr_sqrtf(float x);`
- 目标格式：binary32
- 函数类型：一元
- C23 对应函数 / 条款：sqrtf — C23 §7.12.7.5
- 参考来源：独立实现（CORE-MATH 无 binary32 sqrtf）
- 正确性 oracle：MPFR (`mpfr_sqrt`)

## 语义合同

- API 合同：遵循 C23 §7.12.7.5。sqrt(±0)=±0，sqrt(+Inf)=+Inf，sqrt(x<0)=NaN
- 舍入模式：默认 round-to-nearest-even
- 比较规则：bitwise 相等（含 signed zero 区分）
- NaN 策略：NaN 分类相等。输入为 sNaN 时触发 FE_INVALID 并返回 qNaN
- signed zero：`+0.0` 与 `-0.0` 严格区分（sqrt(±0) = ±0）
- subnormal：亚正常输入通过 double 中间计算后正确舍入；亚正常结果通过 FMA 校正处理

## 特殊值表

| 输入 | 期望结果 | 比较规则 | 说明 |
|------|----------|----------|------|
| `+0.0` | `+0.0` | bitwise | sqrt(0) = 0 |
| `-0.0` | `-0.0` | bitwise | sqrt(-0) = -0 |
| `+Inf` | `+Inf` | bitwise | sqrt(+Inf) = +Inf |
| `-Inf` | qNaN | classification | sqrt(-Inf) = NaN (domain error) |
| `x < 0` (finite) | qNaN | classification | domain error |
| quiet NaN | NaN | classification | 默认不比较 payload |
| signaling NaN | NaN | classification | FE_INVALID，返回 qNaN |
| subnormal | 正确舍入值 | bitwise | 通过 double sqrt + FMA 校正处理 |

## 算法与系数

- 特殊值处理：NaN/±Inf/±0/x<0 按 C23 分支
- range reduction：无需（sqrt 无 range reduction）
- 核心近似：`sqrt((double)x)` → 53-bit 正确舍入初值
- reconstruction：`fmaf(y, y, -x)` 计算残差，`nextafterf` 做 1-ulp 校正
- FMA 使用点：残差计算 `fmaf(y, y, -x)`
- 系数来源脚本：无（不使用多项式逼近）

## 测试策略

| 测试层级 | 策略 | 必填内容 |
|----------|------|----------|
| L0 | `-Wall -Wextra -Wpedantic -Wconversion -Wdouble-promotion -Wshadow -Werror` | GCC stable + Clang stable |
| L1 | MPFR 穷举（f32 全 2^32 输入） | 特殊值表 + 逐位比较 |

## Oracle 冲突处理

CORE-MATH 无 binary32 sqrtf，无一致性 oracle。MPFR 为唯一正确性 oracle。

## 验收 checklist

- [x] 特殊值表已填完整
- [x] C23 对应语义和任何偏离已记录
- [x] 比较规则已明确
- [x] oracle 来源和固定版本已记录
- [x] 测试策略、样本规模和随机种子已记录
- [x] 系数来源已记录（无多项式系数）
- [ ] 通过 f32 穷举测试（MPFR oracle）
