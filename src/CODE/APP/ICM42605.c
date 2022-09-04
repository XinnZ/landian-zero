/*
 * ICM42605.c
 *
 *  Created on: 2021年8月1日
 *      Author: XinnZ, Landian, HBUT
 *  XinnZ's Blog: https://blog.xinnz.cn/
 */

#include <ICM42605.h>
#include "zf_stm_systick.h"
#include "zf_gpio.h"
#include "zf_spi.h"


#define SPI_NUM         SPI_2
#define SPI_SCK_PIN     SPI2_SCLK_P15_3     //>接模块SPC
#define SPI_MOSI_PIN    SPI2_MOSI_P15_5     //>接模块SDI
#define SPI_MISO_PIN    SPI2_MISO_P15_4     //>接模块SDO
#define SPI_CS_PIN      SPI2_CS0_P15_2      //>接模块CS


//-------------------------------------------------------------------------------------------------------------------
//  @brief      ICM42605 SPI写寄存器
//  @param      cmd     寄存器地址
//  @param      val     需要写入的数据
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void icm_spi_w_reg_byte(uint8 cmd, uint8 val)
{
    uint8 dat[2];

    dat[0] = cmd | ICM42605_SPI_W;
    dat[1] = val;

    spi_mosi(SPI_NUM, SPI_CS_PIN, dat, dat, 2, 1);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ICM42605 SPI读寄存器
//  @param      cmd     寄存器地址
//  @param      *val    接收数据的地址
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void icm_spi_r_reg_byte(uint8 cmd, uint8 *val)
{
    uint8 dat[2];

    dat[0] = cmd | ICM42605_SPI_R;
    dat[1] = *val;

    spi_mosi(SPI_NUM, SPI_CS_PIN, dat, dat, 2, 1);

    *val = dat[1];
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ICM42605 SPI多字节读寄存器
//  @param      cmd     寄存器地址
//  @param      *val    接收数据的地址
//  @param      num     读取数量
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void icm_spi_r_reg_bytes(uint8 *val, uint8 num)
{
    spi_mosi(SPI_NUM, SPI_CS_PIN, val, val, num, 1);
}


float accSensitivity, gyroSensitivity;

float bsp_ICM42605GetAres(uint8 Ascale)
{
    switch(Ascale)
    {
    // Possible accelerometer scales (and their register bit settings) are:
    // 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11).
    case AFS_2G:
        accSensitivity = 2000 / 32768.0f;
        break;
    case AFS_4G:
        accSensitivity = 4000 / 32768.0f;
        break;
    case AFS_8G:
        accSensitivity = 8000 / 32768.0f;
        break;
    case AFS_16G:
        accSensitivity = 16000 / 32768.0f;
        break;
    }

    return accSensitivity;
}

float bsp_ICM42605GetGres(uint8 Gscale)
{
    switch(Gscale)
    {
    case GFS_15_125DPS:
        gyroSensitivity = 15.125f / 32768.0f;
        break;
    case GFS_31_25DPS:
        gyroSensitivity = 31.25f / 32768.0f;
        break;
    case GFS_62_5DPS:
        gyroSensitivity = 62.5f / 32768.0f;
        break;
    case GFS_125DPS:
        gyroSensitivity = 125.0f / 32768.0f;
        break;
    case GFS_250DPS:
        gyroSensitivity = 250.0f / 32768.0f;
        break;
    case GFS_500DPS:
        gyroSensitivity = 500.0f / 32768.0f;
        break;
    case GFS_1000DPS:
        gyroSensitivity = 1000.0f / 32768.0f;
        break;
    case GFS_2000DPS:
        gyroSensitivity = 2000.0f / 32768.0f;
        break;
    }
    return gyroSensitivity;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      初始化ICM42605
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void ICM42605_Init(void)
{
    uint8 val = 0x00;

    spi_init(SPI_NUM, SPI_SCK_PIN, SPI_MOSI_PIN, SPI_MISO_PIN, SPI_CS_PIN, 3, 10 * 1000 * 1000);

    while (0x42 != val)     //>读取ICM42605 ID
    {
        icm_spi_r_reg_byte(ICM42605_WHO_AM_I, &val);
        systick_delay_ms(50);
        //>卡在这里原因有以下几点
        //>1 ICM42605坏了，如果是新的这样的概率极低
        //>2 接线错误或者没有接好
        //>3 可能你需要外接上拉电阻，上拉到3.3V
    }

    icm_spi_w_reg_byte(ICM42605_REG_BANK_SEL, 0);       //>设置bank 0区域寄存器
    icm_spi_w_reg_byte(ICM42605_REG_BANK_SEL, 0x01);    //>软复位传感器
    systick_delay_ms(200);

    icm_spi_w_reg_byte(ICM42605_REG_BANK_SEL, 1);       //>设置bank 1区域寄存器
    icm_spi_w_reg_byte(ICM42605_INTF_CONFIG4, 0x02);    //>设置为4线SPI通信

    icm_spi_w_reg_byte(ICM42605_REG_BANK_SEL, 0);       //>设置bank 0区域寄存器
    icm_spi_w_reg_byte(ICM42605_FIFO_CONFIG,  0x40);    //>Stream-to-FIFO Mode(page61)

    icm_spi_r_reg_byte(ICM42605_INT_SOURCE0,  &val);
    icm_spi_w_reg_byte(ICM42605_INT_SOURCE0,  0x00);
    icm_spi_w_reg_byte(ICM42605_FIFO_CONFIG2, 0x00);    // watermark
    icm_spi_w_reg_byte(ICM42605_FIFO_CONFIG3, 0x02);    // watermark
    icm_spi_w_reg_byte(ICM42605_INT_SOURCE0,  val);
    icm_spi_w_reg_byte(ICM42605_FIFO_CONFIG1, 0x62);    // Enable the gyro to the FIFO

    icm_spi_w_reg_byte(ICM42605_REG_BANK_SEL, 0x00);
    icm_spi_w_reg_byte(ICM42605_INT_CONFIG,   0x36);

    icm_spi_w_reg_byte(ICM42605_REG_BANK_SEL, 0x00);
    icm_spi_r_reg_byte(ICM42605_INT_SOURCE0,  &val);
    val |= (1 << 2);                                    // FIFO_THS_INT1_ENABLE
    icm_spi_w_reg_byte(ICM42605_INT_SOURCE0,  val);
/*
    bsp_ICM42605GetAres(AFS_16G);
    icm_spi_w_reg_byte(ICM42605_REG_BANK_SEL, 0x00);
    icm_spi_r_reg_byte(ICM42605_ACCEL_CONFIG0, &val);   // page74
    val |= (AFS_16G << 5);                              //>量程 ±16g
    val |= (AODR_1000Hz);                               //>输出速率 1000HZ  LN mode
    icm_spi_w_reg_byte(ICM42605_ACCEL_CONFIG0, val);
*/
    bsp_ICM42605GetGres(GFS_1000DPS);
    icm_spi_w_reg_byte(ICM42605_REG_BANK_SEL, 0x00);
    icm_spi_r_reg_byte(ICM42605_GYRO_CONFIG0, &val);    // page73
    val |= (GFS_1000DPS << 5);                          //>量程 ±1000dps
    val |= (GODR_2000Hz);                               //>输出速率 2000HZ
    icm_spi_w_reg_byte(ICM42605_GYRO_CONFIG0, val);

    icm_spi_w_reg_byte(ICM42605_REG_BANK_SEL, 0x00);
    icm_spi_r_reg_byte(ICM42605_PWR_MGMT0, &val);       //>读取PWR—MGMT0当前寄存器的值(page72)
    val &= ~((1) << 4);                                 //>设置IDLE
    val |=  ((1) << 3);
    val |=  ((1) << 2);                                 //>设置GYRO_MODE  00:关闭 01:待机 10:预留   11:低噪声
    val &= ~((1) << 1);
    val &= ~((1) << 0);                                 //>设置ACCEL_MODE 00:关闭 01:关闭 10:低功耗 11:低噪声
    icm_spi_w_reg_byte(ICM42605_PWR_MGMT0, val);
    systick_delay_ms(5);                                //>操作完PWR—MGMT0寄存器后 200us内不能有任何读写寄存器的操作
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取ICM42605加速度计数据
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:				执行该函数后，直接查看对应的变量即可
//-------------------------------------------------------------------------------------------------------------------
void ICM42605_AccData(void)
{
    struct
    {
        uint8 reg;
        uint8 dat[6];
    } buf;

    buf.reg = ICM42605_ACCEL_DATA_X1 | ICM42605_SPI_R;

    icm_spi_r_reg_bytes(&buf.reg, 7);

    ICM.ACCEL.X = (float)((int16)(buf.dat[0] << 8) | buf.dat[1]) * accSensitivity;
    ICM.ACCEL.Y = (float)((int16)(buf.dat[2] << 8) | buf.dat[3]) * accSensitivity;
    ICM.ACCEL.Z = (float)((int16)(buf.dat[4] << 8) | buf.dat[5]) * accSensitivity;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取ICM42605陀螺仪数据
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:				执行该函数后，直接查看对应的变量即可
//-------------------------------------------------------------------------------------------------------------------
void ICM42605_GyroData(void)
{
    struct
    {
        uint8 reg;
        uint8 dat[6];
    } buf;

    buf.reg = ICM42605_GYRO_DATA_X1 | ICM42605_SPI_R;

    icm_spi_r_reg_bytes(&buf.reg, 7);

    ICM.GYRO.X = (float)((int16)(buf.dat[0] << 8) | buf.dat[1]) * gyroSensitivity;
    ICM.GYRO.Y = (float)((int16)(buf.dat[2] << 8) | buf.dat[3]) * gyroSensitivity;
    ICM.GYRO.Z = (float)((int16)(buf.dat[4] << 8) | buf.dat[5]) * gyroSensitivity;
}

/*******************************************************************************
* 名    称： ICM42605_TemperatureData
* 功    能： 读取ICM42605 内部传感器温度
* 入口参数： 无
* 出口参数： 无
* 修    改：
* 修改日期：
* 备    注： datasheet page62
*******************************************************************************/
void ICM42605_TemperatureData(void)
{
    struct
    {
        uint8 reg;
        uint8 dat[2];
    } buf;

    buf.reg = ICM42605_TEMP_DATA1 | ICM42605_SPI_R;

    icm_spi_r_reg_bytes(&buf.reg, 3);

    ICM.TEMP = (float)((int16)(buf.dat[0] << 8) | buf.dat[1]) / 126.8f + 25.0f;
}
