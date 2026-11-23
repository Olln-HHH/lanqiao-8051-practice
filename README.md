# 蓝桥杯51单片机学习与真题实践

个人学习与备赛代码：蓝桥杯单片机组资料。

收录34个独立Keil C51工程：21个模块练习、7个应用练习、6个历年真题/模拟练习。附源码、工程说明与对应HEX。

**验证状态：保留工程均已通过Keil C51构建；4类主机逻辑回归通过。全部收录工程已实物验证通过，真题练习不作为官方标准答案。**

## 使用方法

1. 在下方目录选例程，核对其README中的时钟和译码方式。
2. 用Keil打开例程内的project.uvproj，选择Practice并Rebuild。
3. 下载build/firmware.hex，或使用[对应编译固件](firmware/README.md)。

实际芯片、接线和下载配置见[硬件说明](docs/HARDWARE.md)。各工程单独编译，不要合并多个main.c。

## 目录

| 目录 | 内容 | 工程数 |
| --- | --- | --- |
| 01-modules | 基础模块与进阶外设 | 21 |
| 02-exercises | 应用练习 | 7 |
| 03-past-exams | 第12、13、15、16届练习，第17届模拟及国赛练习 | 6 |
| firmware | 对应工程的HEX与校验清单 | 34 |

## 模块练习

| 工程 | 内容 |
| --- | --- |
| [01-LED跑马灯](01-modules/01-led/README.md) | 通过锁存器控制LED，当前主循环演示L8翻转，保留原有LED控制函数。 |
| [02-蜂鸣器](01-modules/02-buzzer/README.md) | 演示LED变化、继电器与蜂鸣器的锁存控制。 |
| [03-数码管静态显示](01-modules/03-segment-static/README.md) | 数码管逐位显示及全部位同时显示。 |
| [04-数码管动态显示](01-modules/04-segment-dynamic/README.md) | 循环扫描数码管，显示日期样式内容与月份计数。 |
| [05-独立按键的基本操作与拓展应用](01-modules/05-key-independent/README.md) | 独立按键切换LED工作状态。 |
| [06-矩阵键盘的扫描原理与基本应用](01-modules/06-key-matrix/README.md) | 扫描矩阵键盘，显示0~15的键码。 |
| [07-外部中断的基本操作](01-modules/07-interrupt/README.md) | 外部中断触发LED提示；保留入门例程中的阻塞处理方式。 |
| [08-定时器实现秒闪功能](01-modules/08-timer-blink/README.md) | 使用定时器累计节拍，控制不同周期的LED翻转。 |
| [09-定时器的进阶综合案例](01-modules/09-stopwatch/README.md) | 分钟、秒与节拍显示，按键控制启停和清零。 |
| [10-利用PWM控制灯光亮度](01-modules/10-pwm/README.md) | S7循环选择10%、50%、90%亮度及关闭，软件PWM周期分100份。 |
| [11-串口通信基本操作](01-modules/11-uart-basic/README.md) | 上电发送0x5A、0xA5，收到一个字节后回发加1的结果；接收中断与主循环回发分开。 |
| [12-串口通信进阶应用](01-modules/12-uart-command/README.md) | 按命令高四位解析LED控制与状态字符串查询。 |
| [13-IO拓展](01-modules/13-io-expand/README.md) | 使用XBYTE访问扩展地址，演示LED与数码管控制。 |
| [14-基础技能综合实训](01-modules/14-lighting-system/README.md) | 定时器、按键、显示与串口组合的灯光控制练习。 |
| [AT24C02](01-modules/15-eeprom/README.md) | 上电读取三组EEPROM数据，修改后写回并显示。 |
| [ds1302](01-modules/16-rtc/README.md) | 配置并读取DS1302的BCD时间，数码管显示时分秒。 |
| [ds18b20](01-modules/17-temperature/README.md) | DS18B20采集与数码管温度显示。 |
| [NE555](01-modules/18-frequency-counter/README.md) | 定时器0计数，定时器1建立测量时间窗，数码管显示计数结果。 |
| [PCF8591](01-modules/19-adc-dac/README.md) | 当前主循环采集PCF8591 ADC并显示电压；DAC接口与原有示例保留。 |
| [超声波1](01-modules/20-ultrasonic/README.md) | 发射超声波，定时测量回波时间，显示距离或超时标志。 |
| [频率测量](01-modules/21-frequency-timers/README.md) | 计数中断累计脉冲、定时中断建立1秒窗口；显示使用存储器映射方式。 |

## 应用练习

| 工程 | 内容 |
| --- | --- |
| [01】灯光闪烁与数码管计数](02-exercises/01-led-counter/README.md) | LED周期变化与两组数码管计数。 |
| [02】长按与短按控制数码管显示](02-exercises/02-key-long-short/README.md) | S4短按计数加1，长按清零；长按时间计数采用饱和处理。 |
| [04】24C02存储按键触发次数](02-exercises/03-eeprom-key-counter/README.md) | S4/S5/S6分别累计次数并存储；上电恢复计数，只有计数变化才写EEPROM。 |
| [05】采样光敏电阻与可调电阻的电压](02-exercises/04-adc-voltage/README.md) | 采集PCF8591的0x01与0x03通道，显示光敏与可调电阻输入电压。 |
| [06】基于PCF8591的DAC模拟电压输出](02-exercises/05-dac-output/README.md) | S4切换2V、4V、跟随可调电阻三种DAC模式。 |
| [特训案例1】基于PCF8591的智能照明控制器](02-exercises/06-smart-lighting/README.md) | 根据光敏电压控制LED数量，支持保存和回看当前模式、电压。 |
| [特训案例2】DS18B20温度传感器数据采样与显示](02-exercises/07-temperature-display/README.md) | 温度采集与数码管显示的应用练习。 |

## 历年真题与模拟练习

| 工程 | 内容 |
| --- | --- |
| [第12届省赛练习](03-past-exams/12-provincial/README.md) | 温度显示、参数调整、DAC控制与界面指示灯。 |
| [第13届省赛练习](03-past-exams/13-provincial/README.md) | 温度、RTC时间与继电器控制的综合练习。 |
| [第15届省赛练习](03-past-exams/15-provincial/README.md) | 采用user/driver结构的频率、校准、RTC、DAC与LED综合版本。 |
| [第16届省赛练习](03-past-exams/16-provincial/README.md) | 温度、光照等级、超声波距离与变化量、继电器次数综合练习。 |
| [第17届模拟练习](03-past-exams/17-mock-1/README.md) | 测距、RTC、近距离时间记录及蜂鸣器提示的模拟练习。 |
| [国赛综合练习（原目录未注明届次）](03-past-exams/national-practice-1/README.md) | 温度、RTC、ADC、超声波与LED的国赛综合训练，原目录未注明届次。 |

## 构建与检查

脚本需要Python 3.10及以上，仅使用标准库。

```powershell
python tools/build.py --keil "你的Keil安装目录/UV4/UV4.exe"
python tools/check_repository.py
python tools/test_logic.py --cc "你的GCC路径/gcc.exe"
```

只编译单个例程可添加 `--project 01-modules/10-pwm`。GCC逻辑回归不代替固件构建和硬件测试。

## 说明

- [修复要点](docs/CHANGES.md) · [验证记录](docs/VALIDATION.md) · [上传步骤](docs/UPLOAD.md)
- 模块练习保留入门时的阻塞延时和直接寄存器写法，各例独立保留驱动。
- 原创部分采用[MIT许可证](LICENSE)，第三方文件保留原有条款，见[来源说明](THIRD_PARTY_NOTICES.md)。
