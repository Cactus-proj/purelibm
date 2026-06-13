# 函数规格模板

每个函数在实现前必须复制本模板并填完整。规格是实现、测试和 oracle 判断的共同依据。

## 基本信息

- 函数名：`cr_<name>`
- C 签名：`<return-type> cr_<name>(<args>);`
- 目标格式：binary32 / binary64
- 函数类型：一元 / 二元 / 多元
- C23 对应函数 / 条款：
- 参考来源：CORE-MATH 文件和固定 commit
- 正确性 oracle：MPFR / 高精度生成器 / 手写特殊值表

## 语义合同

- API 合同：遵循 C23 数学库语义 / 记录项目内偏离
- 舍入模式：默认 round-to-nearest-even
- 比较规则：bitwise 相等 / 分类相等 / 函数自定义规则
- NaN 策略：只要求 NaN 分类 / 保留 payload / 保留符号位
- signed zero：`+0.0` 与 `-0.0` 是否区分
- subnormal：是否要求保留 IEEE 754 subnormal 语义

## 特殊值表

| 输入 | 期望结果 | 比较规则 | 说明 |
|------|----------|----------|------|
| `+0.0` | TBD | bitwise / classification |  |
| `-0.0` | TBD | bitwise / classification |  |
| `+Inf` | TBD | bitwise / classification |  |
| `-Inf` | TBD | bitwise / classification |  |
| quiet NaN | NaN | classification | 默认不比较 payload |
| signaling NaN | NaN | classification | 按函数规格声明 |
| subnormal | TBD | bitwise / classification |  |

## 算法与系数

- 特殊值处理：
- range reduction：
- 核心近似：
- reconstruction：
- FMA 使用点：
- 系数来源脚本：`coeff/<name>.sollya` / `coeff/<name>.gappa`
- 生成命令：
- 误差界：

## 测试策略

| 测试层级 | 策略 | 必填内容 |
|----------|------|----------|
| L0 | 编译警告 / 静态检查 / 单元测试 | 编译器、flag、检查命令 |
| L1 | MPFR / CORE-MATH / 特殊值表 | 样本规模、随机种子、失败样例位置 |
| L2+ | CBMC / Frama-C / Sollya / Gappa | 可选，说明覆盖的局部性质 |

一元 binary32 函数默认要求全输入域穷举。二元/多元 binary32 函数使用边界笛卡尔积、随机采样、hard-to-round cases 和差异回归集。binary64 函数使用随机采样、边界值和 hard-to-round cases，不承诺全域穷举。

## Oracle 冲突处理

若 CORE-MATH 与 C23 语义、MPFR / 规格表不一致，以 C23 语义、MPFR / 规格表为准。将差异记录到 `docs/oracle-differences.md`，并说明是否需要反馈上游。

## 验收 checklist

- [ ] 特殊值表已填完整
- [ ] C23 对应语义和任何偏离已记录
- [ ] 比较规则已明确
- [ ] oracle 来源和固定版本已记录
- [ ] 测试策略、样本规模和随机种子已记录
- [ ] 系数来源和生成命令已记录
- [ ] CORE-MATH 差异已记录或确认无差异
