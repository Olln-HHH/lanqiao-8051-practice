# 验证记录

日期：2026-11-23。构建环境：Windows、Keil C51 9.59。

## Keil构建

保留的36个工程均已在整理阶段重新构建成功，共0个错误、45条L16告警（保留接口中的未调用函数）。本次精简只删除工程、调整目录和文档，未改动保留工程的C源码、头文件或编译配置。

| 工程 | 错误 | 告警 |
| --- | --- | --- |
| [01-LED跑马灯](../01-modules/01-led/README.md) | 0 | 1 |
| [02-蜂鸣器](../01-modules/02-buzzer/README.md) | 0 | 0 |
| [03-数码管静态显示](../01-modules/03-segment-static/README.md) | 0 | 0 |
| [04-数码管动态显示](../01-modules/04-segment-dynamic/README.md) | 0 | 0 |
| [05-独立按键的基本操作与拓展应用](../01-modules/05-key-independent/README.md) | 0 | 0 |
| [06-矩阵键盘的扫描原理与基本应用](../01-modules/06-key-matrix/README.md) | 0 | 0 |
| [07-外部中断的基本操作](../01-modules/07-interrupt/README.md) | 0 | 0 |
| [08-定时器实现秒闪功能](../01-modules/08-timer-blink/README.md) | 0 | 0 |
| [09-定时器的进阶综合案例](../01-modules/09-stopwatch/README.md) | 0 | 0 |
| [10-利用PWM控制灯光亮度](../01-modules/10-pwm/README.md) | 0 | 0 |
| [11-串口通信基本操作](../01-modules/11-uart-basic/README.md) | 0 | 0 |
| [12-串口通信进阶应用](../01-modules/12-uart-command/README.md) | 0 | 0 |
| [13-IO拓展](../01-modules/13-io-expand/README.md) | 0 | 0 |
| [14-基础技能综合实训](../01-modules/14-lighting-system/README.md) | 0 | 0 |
| [AT24C02](../01-modules/15-eeprom/README.md) | 0 | 4 |
| [ds1302](../01-modules/16-rtc/README.md) | 0 | 3 |
| [ds18b20](../01-modules/17-temperature/README.md) | 0 | 2 |
| [NE555](../01-modules/18-frequency-counter/README.md) | 0 | 0 |
| [PCF8591](../01-modules/19-adc-dac/README.md) | 0 | 4 |
| [超声波1](../01-modules/20-ultrasonic/README.md) | 0 | 0 |
| [频率测量](../01-modules/21-frequency-timers/README.md) | 0 | 0 |
| [01】灯光闪烁与数码管计数](../02-exercises/01-led-counter/README.md) | 0 | 0 |
| [02】长按与短按控制数码管显示](../02-exercises/02-key-long-short/README.md) | 0 | 0 |
| [04】24C02存储按键触发次数](../02-exercises/03-eeprom-key-counter/README.md) | 0 | 4 |
| [05】采样光敏电阻与可调电阻的电压](../02-exercises/04-adc-voltage/README.md) | 0 | 3 |
| [06】基于PCF8591的DAC模拟电压输出](../02-exercises/05-dac-output/README.md) | 0 | 2 |
| [特训案例1】基于PCF8591的智能照明控制器](../02-exercises/06-smart-lighting/README.md) | 0 | 3 |
| [特训案例2】DS18B20温度传感器数据采样与显示](../02-exercises/07-temperature-display/README.md) | 0 | 1 |
| [第12届省赛练习](../03-past-exams/12-provincial/README.md) | 0 | 2 |
| [第13届省赛练习](../03-past-exams/13-provincial/README.md) | 0 | 0 |
| [第15届省赛练习（版本一）](../03-past-exams/15-provincial-v1/README.md) | 0 | 5 |
| [第15届练习（模块化版本）](../03-past-exams/15-provincial-modular/README.md) | 0 | 2 |
| [第16届省赛练习（第二次）](../03-past-exams/16-provincial-v2/README.md) | 0 | 6 |
| [第16届省赛练习（第三次）](../03-past-exams/16-provincial-v3/README.md) | 0 | 0 |
| [第17届模拟练习](../03-past-exams/17-mock-1/README.md) | 0 | 0 |
| [国赛综合练习（原目录未注明届次）](../03-past-exams/national-practice-1/README.md) | 0 | 3 |

## 主机与仓库检查

- ADC全部256个输入码的电压换算。
- PWM的0%~100%全部101个占空比和周期回绕。
- 持续10000个定时节拍的长按计数饱和。
- DS18B20首次启动、转换忙、正确CRC、错误CRC和断线处理。
- 工程依赖、README链接、36份固件的SHA-256及压缩包完整性。

主机逻辑测试使用实际C函数和模拟寄存器/总线响应，不能验证STC实际指令时序。本次修改后的固件未烧录实测，也未逐项对照原题评分表。
