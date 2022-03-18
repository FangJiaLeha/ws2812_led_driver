/**
 * @file register_info.c
 * @author {fangjiale} 
 * @brief 
 * @version 0.1
 * @date 2022-03-15
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "register_info.h"

//==============================================================================
/**
 * @brief 是结构体成员按1字节对齐
 *
 */
#pragma pack (1)

//==============================================================================
/**
 * @brief TLC59108亮度寄存器声明
 *
 */
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
/**
 * @brief TLC59108输出状态寄存器0声明
 *
 */
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
/**
 * @brief TLC59108输出状态寄存器1声明
 *
 */
typedef struct ledout1_reg
{
    uint8_t LDR4:2;
    uint8_t LDR5:2;
    uint8_t LDR6:2;
    uint8_t LDR7:2;
}LEDOUT1RegType;
/**
 * @brief WS2812颜色状态寄存器声明
 *
 */
typedef struct show_color_type
{
    uint8_t ColorRx;
    uint8_t ColorGx;
    uint8_t ColorBx;
}ColorXRegType;

/**
 * @brief TLC59108灯条 寄存器 声明
 *
 */
typedef struct tlc59108_reg
{
    /**
     * @brief 工作模式寄存器1
     *
     * @note  实际未使用
     *
     */
    uint8_t WorkMode1;
    /**
     * @brief 工作模式寄存器2
     *
     * @note  0x01:dimming模式
     *        0x02:blinking模式
     */
    uint8_t WorkMode2;
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
     * @brief I2C设备子地址1/2/3
     *
     * @note  实际未使用
     *
     */
    uint8_t SubAddr1;
    uint8_t SubAddr2;
    uint8_t SubAddr3;
    /**
     * @brief 可操作所有TLC59108设备的地址
     *
     * @note  实际未使用
     *
     */
    uint8_t AllCallAddr;
}TLC59108RegType;

/**
 * @brief WS2812灯条 寄存器 声明
 *
 */
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
     *        0x06:呼吸模式
     */
    uint8_t WorkMode;
    /**
     * @brief PWM波输出通道选择寄存器
     *
     * @note  最大可选择8 - 1个PWM波输出通道 第5个通道不支持DMA搬运
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
     * @note  应用于渐变/呼吸模式下
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
     * @brief 通道N控制灯珠点亮位置设置寄存器
     *
     * @note  取值范围：1~灯条总数寄存器设定值
     *
     */
    uint8_t CtrlLedShowPos;
    /**
     * @brief 通道N控制灯珠电量方向设置寄存器
     *
     * @note  基本流水灯模式下:
     *                    【0x01】: 左流水
     *                    【0x02】: 右流水
     *        渐变流水灯模式下:
     *                    【0x03】: 左渐变流水
     *                    【0x04】: 右渐变流水
     *        分段闪烁模式下:
     *                    【0x05】: 灯条左部分
     *                    【0x06】: 灯条右部分
     * 注：分段闪烁模式下 灯条左右部分基于PWM信号输入位置而言
     *     信号输入第一个灯珠为灯条右部分 最后一个灯珠为灯条左部分
     */
    uint8_t CtrlLedDir;
    /**
     * @brief 周期配置寄存器
     *
     * @note  流水灯模式下:   流水灯周期 单位10ms
     *        分段闪烁模式下: 闪烁周期   单位50ms
     *        呼吸模式下：    呼吸周期   单位100ms
     *        其他工作模式下: 该参数设置无效
     */
    uint16_t PeriodConfig;
}WS2812RegType;

/**
 * @brief 灯条通用驱动模块 寄存器 声明 
 *
 */
typedef struct reg_info
{
    /**
     * @brief TLC59108寄存器
     *
     */
    TLC59108RegType TLC59108RegInfo;

    /**
     * @brief 驱动的设备类型
     *
     * @note  0x01: WS2812
     *        0x02: TLC59108
     */
    uint8_t DriverDevType;

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
    Rtv_Status (*init)(void *dev);
    Rtv_Status (*control)(void *dev,
                          const CtrlRegCmdType cmd,
                          const uint8_t regAddr,
                          void *arg,
                          const uint8_t size);
}RegDevType;
typedef RegDevType* RegDevType_t;

//======================================================================
static Rtv_Status _init(void *dev)
{
    RegDevType_t temp_reg_dev = (RegDevType_t)dev;
    if (temp_reg_dev == NULL) {
        return EINVAL;
    }
    // 清除静态编译生成的一段内存空间
    memset(&temp_reg_dev->info, 0, sizeof(RegInfoType));
    // 获取灯驱寄存器总数和灯驱寄存器的首地址 +2:WS2812的周期寄存器为2个字节的数据(以单个字节为单位)
    temp_reg_dev->info.RegNum = (uint8_t *)&temp_reg_dev->info.WS2812RegInfo.PeriodConfig -
                                &temp_reg_dev->info.TLC59108RegInfo.WorkMode1 + 2;
    temp_reg_dev->info.RegBaseAddr =  &temp_reg_dev->info.TLC59108RegInfo.WorkMode1;
    temp_reg_dev->info.WS2812RegBaseAddr = &temp_reg_dev->info.WS2812RegInfo.WorkMode;
    return SUCCESS;
}

static Rtv_Status _control(void *dev,
                     const CtrlRegCmdType cmd,
                     const uint8_t regAddr,
                     void *arg,
                     uint8_t size)
{
    RegDevType_t temp_reg_dev = (RegDevType_t)dev;
    if (temp_reg_dev == NULL) {
        return EINVAL;
    }

    if (cmd == RD_REG_INFO) {
        memmove((uint8_t *)arg, temp_reg_dev->info.RegBaseAddr + regAddr, size);
    } else if(cmd == WR_RGE_INFO) {
        memmove(temp_reg_dev->info.RegBaseAddr + regAddr, (uint8_t *)arg, size);
    } else if(cmd == GET_REG_NUM_INFO) {
        *(uint8_t *)arg = temp_reg_dev->info.RegNum;
    } else if(cmd == GET_REG_DRIVER_TYPE) {
        *(uint8_t *)arg = temp_reg_dev->info.DriverDevType;
    } else if(cmd == GET_TLC59108REG_NUM_INFO) {
        *(uint8_t *)arg = &temp_reg_dev->info.DriverDevType -
                          &temp_reg_dev->info.TLC59108RegInfo.WorkMode1;
    } else if(cmd == GET_WS2812REG_NUM_INFO) {
        *(uint8_t *)arg = (uint8_t *)&temp_reg_dev->info.WS2812RegInfo.PeriodConfig -
                          temp_reg_dev->info.WS2812RegBaseAddr + 2;
    } else if(cmd == GET_TLC59108REG_POS) {
        *(uint8_t *)arg = &temp_reg_dev->info.TLC59108RegInfo.WorkMode1 -
                          temp_reg_dev->info.RegBaseAddr;
    } else if(cmd == GET_WS2812REG_POS) {
        *(uint8_t *)arg = temp_reg_dev->info.WS2812RegBaseAddr -
                          temp_reg_dev->info.RegBaseAddr;
    } else if(cmd == RESET_PARAM_SEG) {
        if (temp_reg_dev->info.DriverDevType == WS2812DEV) {
            memset(temp_reg_dev->info.RegBaseAddr + regAddr,
                   0,
                   (uint8_t *)&temp_reg_dev->info.WS2812RegInfo.PeriodConfig -
                   (uint8_t *)&temp_reg_dev->info.WS2812RegInfo.ColorStart + 2);
        } else if (temp_reg_dev->info.DriverDevType == TLC59108DEV) {
            memset(temp_reg_dev->info.RegBaseAddr + regAddr,
                   0,
                   &temp_reg_dev->info.DriverDevType -
                   (uint8_t *)&temp_reg_dev->info.TLC59108RegInfo.BrightCtrl);
        }
    } else if (cmd == RESET_REG_INFO){
        memset(&temp_reg_dev->info, 0, temp_reg_dev->info.RegNum);
    } else {
    }
    return SUCCESS;
}

//======================================================================
static volatile RegDevType regDev = {
    .init = _init,
    .control = _control
};

//======================================================================
ErrStatus init_register(void)
{
    if (regDev.init((void *)&regDev) != SUCCESS) {
        return ERROR;
    }
    return SUCCESS;
}

ErrStatus control_register(const CtrlRegCmdType cmd,
                           const uint8_t regAddr,
                           void *arg,
                           uint8_t size)
{
    CTRL_REG_CMD_CHECK(cmd);
    REGADDR_CHECK(cmd, regAddr, regDev.info.RegNum);
    CTRL_REG_ARG_CHECK(cmd, arg);
    CTRL_REG_SIZE_CHECK(cmd, size);

    if (regAddr + size > regDev.info.RegNum) {
        size = regDev.info.RegNum - regAddr;
    }

    if (regDev.control((void *)&regDev, cmd, regAddr, arg, size) != SUCCESS) {
        goto set_error;
    }
    return SUCCESS;
set_error:
    return ERROR;
}
