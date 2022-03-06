#include "register_info.h"
#include "string.h"

#define GET_REG_NUM(base_addr, end_addr)    (end_addr - base_addr + 1)

typedef struct bright_ctrl_reg
{
    /**
     * @brief 对应8个PWM波输出通道
     *
     */
    uint8_t PWM0;
    uint8_t PWM1;
    uint8_t PWM2;
    uint8_t PWM3;
    uint8_t PWM4;
    uint8_t PWM5;
    uint8_t PWM6;
    uint8_t PWM7;
}BrightCtrlRegType;

typedef struct ledout0_reg
{
    /**
     * @brief LDRx ~ LEDx 输出通道端口状态
     *
     */
    uint8_t LDR0:2;
    uint8_t LDR1:2;
    uint8_t LDR2:2;
    uint8_t LDR3:2;
}LEDOUT0RegType;

typedef struct ledout1_reg
{
    uint8_t LDR4:2;
    uint8_t LDR5:2;
    uint8_t LDR6:2;
    uint8_t LDR7:2;
}LEDOUT1RegType;

typedef struct show_color_type
{
    uint8_t ColorRx;
    uint8_t ColorGx;
    uint8_t ColorBx;
}ColorXRegType;

typedef struct tlc59108_reg
{
    /**
     * @brief 工作模式设置寄存器
     *
     * @note  0x01:dimming模式
     *        0x02:blinking模式
     */
    uint8_t WorkMode;
    /**
     * @brief 单通道亮度设置寄存器
     *
     * @note  单通道亮度调节主要通过改变单通道占空比来实现
     *        通道占空比 = (通道亮度寄存器值) / 256
     */
    BrightCtrlRegType BrightCtrl;
    /**
     * @brief 组亮度设置寄存器
     *
     * @note  组亮度调节主要通过改变组占空比来实现
     *        组占空比 = (组亮度寄存器值) / 256
     */
    uint8_t GRPPWM;
    /**
     * @brief 组引脚输出频率控制寄存器 主要用于blinking模式
     *
     * @note  blinking周期 = (该寄存器值 + 1) / 24
     */
    uint8_t GRPFREQ;
    /**
     * @brief LEDx通道输出配置寄存器0
     *
     * @note  LDRx = 0x00:关闭x通道端口PWM波输出
     *        LDRx = 0x01:x通道全速输出(该通道亮度及组dimming/blinking模式参数设置无效)
     *        LDRx = 0x10:x通道亮度受亮度寄存器【BrightCtrl】控制
     *        LDRx = 0x11:x通道亮度和组dimming/blinking模式受寄存器【BrightCtrl/GRPPWM】控制
     *
     */
    union
    {
        uint8_t All;
        LEDOUT0RegType Bits;
    }LEDOUT0;
    /**
     * @brief LEDx通道输出配置寄存器1
     *
     * @note  同上寄存器说明
     *
     */
    union
    {
        uint8_t All;
        LEDOUT1RegType Bits;
    }LEDOUT1;
    /**
     * @brief 通道错误标志寄存器
     *
     */
    uint8_t EFlag;
}TLC59108RegType;

typedef struct ws2812_reg
{
    /**
     * @brief 工作模式设置寄存器
     *
     * @note  0x01:熄灭模式
     *        0x02:常亮模式
     *        0x03:分段闪烁模式
     *        0x04:基本流水灯模式
     *        0x05:渐变流水灯模式
     */
    uint8_t WorkMode;
    /**
     * @brief PWM波输出通道选择寄存器
     *
     * @note  最大可选择8个PWM波输出通道
     *
     */
    uint8_t ChannelOutChoose;
    /**
     * @brief PWM波输出通道使能寄存器
     *
     */
    uint8_t ChannelOutEn;
    /**
     * @brief 通道N控制灯珠起始颜色寄存器
     *
     */
    ColorXRegType ColorStart;
    /**
     * @brief 通道N控制灯珠终止颜色寄存器
     *
     * @note  应用于渐变模式下
     *
     */
    ColorXRegType ColorEnd;
    /**
     * @brief 通道N可控制的灯珠总数寄存器
     *
     * @note  灯珠最大数量: 灯条灯珠数
     *
     */
    uint8_t CtrlLedAll;
    /**
     * @brief 通道N控制的灯珠数寄存器
     *
     * @note  取值范围: 1 ~ 灯珠总数寄存器设定值
     *
     */
    uint8_t CtrlLedNum;
    /**
     * @brief 通道N控制灯珠电量方向设置寄存器
     *
     * @note  流水灯模式下:
     *                    【0x01】: 左流水
     *                    【0x02】: 右流水
     *        分段闪烁模式下:
     *                    【0x01】: 灯条左部分
     *                    【0x02】: 灯条右部分
     * 注：分段模式下 灯条左右部分基于PWM信号输入位置而言
     *     信号输入第一个灯珠为灯条右部分 最后一个灯珠为灯条左部分
     *
     */
    uint8_t CtrlLedDir;
    /**
     * @brief 周期配置寄存器
     *
     * @note  流水灯模式下: 流水灯周期 单位10ms
     *        分段闪烁模式下: 闪烁周期 单位50ms
     *        其他工作模式下: 参数设置无效
     *
     */
    uint8_t PeriodConfig;
}WS2812RegType;

typedef struct reg_info
{
    /**
     * @brief 驱动的设备类型
     *
     * @note  0x01: TLC59108
     *        0x02: WS2812
     */
    uint8_t DriverDevType;

    /**
     * @brief TLC59108寄存器
     *
     */
    TLC59108RegType TLC59108RegInfo;

    /**
     * @brief WS2812寄存器
     *
     */
    WS2812RegType WS2812RegInfo;

    /**
     * @brief 寄存器个数
     *
     */
    uint8_t RegNum;

    /**
     * @brief 记录寄存器首地址
     *
     */
    uint8_t *RegBaseAddr;

    /**
     * @brief 记录WS2812寄存器首地址
     *
     */
    uint8_t *WS2812RegBaseAddr;
}RegInfoType;
typedef RegInfoType* RegInfoType_t;

//======================================================================
typedef struct reg_dev
{
    RegInfoType info;
    void (*init)(void *dev);
    void (*control)(void *dev,
                    const CtrlRegCmdType cmd,
                    const uint8_t regAddr,
                    void *arg,
                    const uint8_t size);
}RegDevType;
typedef RegDevType* RegDevType_t;

//======================================================================
static void _init(void *dev)
{
    RegDevType_t temp_reg_dev = (RegDevType_t)dev;
    if (temp_reg_dev == NULL) {
        return;
    }
    memset(temp_reg_dev, 0, sizeof(RegInfoType));
    // 获取寄存器数量和寄存器首地址
    temp_reg_dev->info.RegNum = &temp_reg_dev->info.WS2812RegInfo.PeriodConfig -
                                &temp_reg_dev->info.DriverDevType + 1;
    temp_reg_dev->info.RegBaseAddr =  &temp_reg_dev->info.DriverDevType;
    temp_reg_dev->info.WS2812RegBaseAddr = &temp_reg_dev->info.WS2812RegInfo.WorkMode;
}

static void _control(void *dev,
                     const CtrlRegCmdType cmd,
                     const uint8_t regAddr,
                     void *arg,
                     uint8_t size)
{
    RegDevType_t temp_reg_dev = (RegDevType_t)dev;
    if (temp_reg_dev == NULL) {
        return;
    }
    if (cmd == RD_REG_INFO) {
        memmove((uint8_t *)arg, temp_reg_dev->info.RegBaseAddr + regAddr - 1, size);
    } else if(cmd == WR_RGE_INFO) {
        if (*(uint8_t *)arg == TLC59108DEV) {
            memmove(temp_reg_dev->info.RegBaseAddr + regAddr - 1, (uint8_t *)arg, size);
        } else {
            *(temp_reg_dev->info.RegBaseAddr + regAddr - 1) = *(uint8_t *)arg;
            // WS2812驱动设备需对首地址进行偏移处理
            memmove(temp_reg_dev->info.WS2812RegBaseAddr, (uint8_t *)arg + 1, size - 1);
        }
    } else if(cmd == GET_REG_NUM_INFO) {
        *(uint8_t *)arg = temp_reg_dev->info.RegNum;
    } else {
        memset(&temp_reg_dev->info, 0, GET_REG_NUM(&temp_reg_dev->info.WS2812RegInfo.PeriodConfig,
                                                   &temp_reg_dev->info.DriverDevType));
    }
}

//======================================================================
static RegDevType regDev = {
    .init = _init,
    .control = _control
};

//======================================================================
void init_register(void)
{
    regDev.init((void *)&regDev);
}

void control_register(const CtrlRegCmdType cmd,
                      const uint8_t regAddr,
                      void *arg,
                      const uint8_t size)
{
    CTRL_REG_CMD_CHECK(cmd);
    REGADDR_CHECK(cmd, regAddr, regDev.info.RegNum);
    CTRL_REG_ARG_CHECK(cmd, arg);
    CTRL_REG_SIZE_CHECK(cmd, size);

    regDev.control((void *)&regDev, cmd, regAddr, arg, size);

set_error:
    return;
}
