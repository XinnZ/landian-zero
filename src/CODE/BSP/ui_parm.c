/*
 * ui_parm.c
 *
 *  Created on: 2021年8月1日
 *      Author: XinnZ, Landian, HBUT
 *  XinnZ's Blog: https://blog.xinnz.cn/
 */

#include "headfile.h"

uint8 Flag_Save = 0;

/*
 * @brief   : UI_Parm_Tasks
 * @param   : None
 * @return  : None
 * @example : None
 * @author  : XinnZ
 */
void UI_Parm_Tasks(void)
{
    switch (UI_Focus.CurrentUI)
    {
    /* ****************************  主菜单任务  **************************** */
    case ui_Main:
    {
        switch (Key)
        {
        case Key_Left:
        case Key_Up:
        {
            if (UI_Focus.CurrentFocus <= 1)
                UI_Focus.CurrentFocus = ui_RUN;
            else
                UI_Focus.CurrentFocus--;
        }
        break;
        case Key_Right:
        case Key_Press:
        {
            UI_Focus.CurrentUI = UI_Focus.CurrentFocus; //>选中
        }
        break;
        case Key_Down:
        {
            if (UI_Focus.CurrentFocus >= ui_RUN)
                UI_Focus.CurrentFocus = 1;
            else
                UI_Focus.CurrentFocus++;
        }
        break;
        }
    }
    break;

    /* *************************  电机控制菜单任务  ************************* */
    case ui_Motor:
    {
        switch (Key)
        {
        case Key_Down:
        {
            if (Parm_Focus.CurrentFocus == Parm_Focus.CurrentParm)
            {
                Parm_Focus.CurrentParm = 0xFF; //>取消选中
            }
            Parm_Focus.CurrentFocus++;         //>下移光标
            if (Parm_Focus.CurrentFocus == 12) //>超过范围
                Parm_Focus.CurrentFocus = 0;   //>回到第一位
        }
        break;
        case Key_Right:
        {
            switch (Parm_Focus.CurrentFocus)
            {
            case 4:
                Motor.Velocity  += Parm_Focus.CurrentFocus == Parm_Focus.CurrentParm ?  100  :   50;
                break;
            case 5:
                PID_Location.kP += Parm_Focus.CurrentFocus == Parm_Focus.CurrentParm ? 1.0f :  0.1f;
                break;
            case 6:
                PID_Location.kD += Parm_Focus.CurrentFocus == Parm_Focus.CurrentParm ? 0.1f : 0.01f;
                break;
            case 7:
                PID_Angle.kP    += Parm_Focus.CurrentFocus == Parm_Focus.CurrentParm ? 1.0f :  0.1f;
                break;
            case 8:
                PID_Angle.kD    += Parm_Focus.CurrentFocus == Parm_Focus.CurrentParm ? 0.1f : 0.01f;
                break;
            case 9:
                PID_Velocity_R.kP = PID_Velocity_L.kP += Parm_Focus.CurrentFocus == Parm_Focus.CurrentParm ? 1.0f :  0.1f;
                break;
            case 10:
                PID_Velocity_R.kI = PID_Velocity_L.kI += Parm_Focus.CurrentFocus == Parm_Focus.CurrentParm ? 0.1f : 0.01f;
                break;
            case 11:
                PID_Velocity_R.kD = PID_Velocity_L.kD += Parm_Focus.CurrentFocus == Parm_Focus.CurrentParm ? 0.1f : 0.01f;
                break;
            }
        }
        break;
        case Key_Press:
        {
            if (Parm_Focus.CurrentFocus == 0)       //>回到主界面
            {
                Motor.ENABLE = 0;
                Motor_STBY_L;
                Motor_Stop();
                UI_Init();
            }
            else if (Parm_Focus.CurrentFocus == 1)  //保存参数
            {
                Flag_Save = 1;
            }
            else if (Parm_Focus.CurrentFocus == 2)  //>电机使能
            {
                Motor.ENABLE = Motor.ENABLE ? false : true;
                if (Motor.ENABLE)
                {
                    Motor_STBY_H;
                }
                else
                {
                    Motor_STBY_L;
                    Motor_Stop();
                }
            }
            else if (Parm_Focus.CurrentParm != Parm_Focus.CurrentFocus)
                Parm_Focus.CurrentParm = Parm_Focus.CurrentFocus; //>选中当前光标
            else
                Parm_Focus.CurrentParm = 0xFF;  //>取消选中
        }
        break;
        case Key_Up:
        {
            if (Parm_Focus.CurrentFocus == Parm_Focus.CurrentParm)
            {
                Parm_Focus.CurrentParm = 0xFF;  //>取消选中
            }
            Parm_Focus.CurrentFocus--;          //>上移光标
            if (Parm_Focus.CurrentFocus > 12)   //>超过范围
                Parm_Focus.CurrentFocus = 11;   //>回到最后一位
        }
        break;
        case Key_Left:
        {
            switch (Parm_Focus.CurrentFocus)
            {
            case 4:
                Motor.Velocity  -= Parm_Focus.CurrentFocus == Parm_Focus.CurrentParm ?  100 :    50;
                break;
            case 5:
                PID_Location.kP -= Parm_Focus.CurrentFocus == Parm_Focus.CurrentParm ? 1.0f :  0.1f;
                break;
            case 6:
                PID_Location.kD -= Parm_Focus.CurrentFocus == Parm_Focus.CurrentParm ? 0.1f : 0.01f;
                break;
            case 7:
                PID_Angle.kP    -= Parm_Focus.CurrentFocus == Parm_Focus.CurrentParm ? 1.0f :  0.1f;
                break;
            case 8:
                PID_Angle.kD    -= Parm_Focus.CurrentFocus == Parm_Focus.CurrentParm ? 0.1f : 0.01f;
                break;
            case 9:
                PID_Velocity_R.kP = PID_Velocity_L.kP -= Parm_Focus.CurrentFocus == Parm_Focus.CurrentParm ? 1.0f :  0.1f;
                break;
            case 10:
                PID_Velocity_R.kI = PID_Velocity_L.kI -= Parm_Focus.CurrentFocus == Parm_Focus.CurrentParm ? 0.1f : 0.01f;
                break;
            case 11:
                PID_Velocity_R.kD = PID_Velocity_L.kD -= Parm_Focus.CurrentFocus == Parm_Focus.CurrentParm ? 0.1f : 0.01f;
                break;
            }
        }
        break;
        }
    }
    break;

    /* **************************  摄像头菜单任务  ************************** */
    case ui_MT9V034:
    {
        switch (Key)
        {
        case Key_Up:
        {
            MT9V034.threshold_offset += 10;
        }
        break;
        case Key_Right:
        {
            MT9V034.threshold_offset += 1;
        }
        break;
        case Key_Press:
        {
            Flag_Save = 1; //>保存设置
            UI_Init();
        }
        break;
        case Key_Down:
        {
            MT9V034.threshold_offset -= 10;
        }
        break;
        case Key_Left:
        {
            MT9V034.threshold_offset -= 1;
        }
        break;
        }
    }
    break;

    /* *************************  其他模块菜单任务  ************************* */
    case ui_Modules:
    {
        switch (Key)
        {
        case Key_Down:
        {
            if (Parm_Focus.CurrentFocus == Parm_Focus.CurrentParm)
                Parm_Focus.CurrentParm = 0xFF;  //>取消选中
            Parm_Focus.CurrentFocus++;          //>下移光标
            if (Parm_Focus.CurrentFocus == 3)   //>超过范围
                Parm_Focus.CurrentFocus = 0;    //>回到第一位
        }
        break;
        case Key_Right:
        {
            switch (Parm_Focus.CurrentFocus)
            {
            case 1:
                LIMIT_V_MAX += Parm_Focus.CurrentFocus == Parm_Focus.CurrentParm ? 1.0f : 0.1f;
                break;
            case 2:
                LIMIT_V_MIN += Parm_Focus.CurrentFocus == Parm_Focus.CurrentParm ? 1.0f : 0.1f;
                break;
            }
        }
        break;
        case Key_Press:
        {
            if (Parm_Focus.CurrentFocus == 0)
            {
                Flag_Save = 1;
                UI_Init();  //>回到主菜单
            }
            else if (Parm_Focus.CurrentParm != Parm_Focus.CurrentFocus)
                Parm_Focus.CurrentParm = Parm_Focus.CurrentFocus; //>选中当前光标
            else
                Parm_Focus.CurrentParm = 0xFF; //>取消选中
        }
        break;
        case Key_Up:
        {
            if (Parm_Focus.CurrentFocus == Parm_Focus.CurrentParm)
                Parm_Focus.CurrentParm = 0xFF; //>取消选中
            Parm_Focus.CurrentFocus--;         //>上移光标
            if (Parm_Focus.CurrentFocus > 3)   //>超过范围
                Parm_Focus.CurrentFocus = 2;   //>回到最后一位
        }
        break;
        case Key_Left:
        {
            switch (Parm_Focus.CurrentFocus)
            {
            case 1:
                LIMIT_V_MAX -= Parm_Focus.CurrentFocus == Parm_Focus.CurrentParm ? 1.0f : 0.1f;
                break;
            case 2:
                LIMIT_V_MIN -= Parm_Focus.CurrentFocus == Parm_Focus.CurrentParm ? 1.0f : 0.1f;
                break;
            }
        }
        break;
        }
    }
    }
}

/*
 * @brief   : UI_Parm_Save
 * @param   : None
 * @return  : None
 * @example : None
 * @author  : XinnZ
 */
void UI_Parm_Save(void)
{
    uint32 index = 10;      //>保存索引
    uint32 write_buf = 0;   //>数据暂存
    uint32 SECTOR_buf = 0;  //>扇区

    disableInterrupts();    //>关闭总中断

    Flag_Save = 0;          //>保存标志清零

    LED3_ON;                //>LED3 On

    //>检查当前页是否有数据 如果有数据则需要擦除整个扇区
    for (; SECTOR_buf < 512; SECTOR_buf++)
    {
        if (flash_check(EEPROM_SECTOR, SECTOR_buf) == 1)
        {
            eeprom_erase_sector(EEPROM_SECTOR);
            break;
        }
    }

    //>当写入的数据不是32位时 建议将数据放入uint32 类型的变量中 在将该变量作为参数传入到eeprom_page_program函数
    write_buf = Motor.Velocity;
    eeprom_page_program(EEPROM_SECTOR, index++, &write_buf);    //>保存电机直行目标值

    write_buf = float_conversion_uint32(PID_Angle.kP);
    eeprom_page_program(EEPROM_SECTOR, index++, &write_buf);    //>保存浮点型 PID_Angle-P
    write_buf = float_conversion_uint32(PID_Angle.kD);
    eeprom_page_program(EEPROM_SECTOR, index++, &write_buf);    //>保存浮点型 PID_Angle-D

    write_buf = float_conversion_uint32(PID_Omega.kP);
    eeprom_page_program(EEPROM_SECTOR, index++, &write_buf);    //>保存浮点型 PID_Omega-P
    write_buf = float_conversion_uint32(PID_Omega.kD);
    eeprom_page_program(EEPROM_SECTOR, index++, &write_buf);    //>保存浮点型 PID_Omega-D

    write_buf = float_conversion_uint32(PID_Location.kP);
    eeprom_page_program(EEPROM_SECTOR, index++, &write_buf);    //>保存浮点型 PID_Location-P
    write_buf = float_conversion_uint32(PID_Location.kD);
    eeprom_page_program(EEPROM_SECTOR, index++, &write_buf);    //>保存浮点型 PID_Location-D

    write_buf = float_conversion_uint32(PID_Velocity_R.kP);
    eeprom_page_program(EEPROM_SECTOR, index++, &write_buf);    //>保存浮点型 PID_Velocity_R_L-P
    write_buf = float_conversion_uint32(PID_Velocity_R.kI);
    eeprom_page_program(EEPROM_SECTOR, index++, &write_buf);    //>保存浮点型 PID_Velocity_R_L-I
    write_buf = float_conversion_uint32(PID_Velocity_R.kD);
    eeprom_page_program(EEPROM_SECTOR, index++, &write_buf);    //>保存浮点型 PID_Velocity_R_L-D

    write_buf = (uint32)MT9V034.threshold_offset;
    eeprom_page_program(EEPROM_SECTOR, index++, &write_buf);    //>保存阈值增幅
    write_buf = (uint32)MT9V034.threshold;
    eeprom_page_program(EEPROM_SECTOR, index++, &write_buf);    //>保存阈值

    write_buf = float_conversion_uint32(LIMIT_V_MAX);
    eeprom_page_program(EEPROM_SECTOR, index++, &write_buf);    //>保存充电最大电压值

    write_buf = float_conversion_uint32(LIMIT_V_MIN);
    eeprom_page_program(EEPROM_SECTOR, index,   &write_buf);    //>保存充电最小电压值

    LED3_OFF;           //>LED Off
    enableInterrupts(); //>开启总中断
}

/*
 * @brief   : UI_Parm_Read
 * @param   : None
 * @return  : None
 * @example : None
 * @author  : XinnZ
 */
void UI_Parm_Read(void)
{
    uint8 index = 10;   //>读取索引

    Motor.Velocity = flash_read(EEPROM_SECTOR, index++, int32);     //>读取电机直行目标值

    PID_Angle.kP = flash_read(EEPROM_SECTOR, index++, float);       //>读角度外环P
    PID_Angle.kD = flash_read(EEPROM_SECTOR, index++, float);       //>读角度外环D
    PID_Angle.kI = 0.0f;                                            //>读角度外环I

    PID_Omega.kP = flash_read(EEPROM_SECTOR, index++, float);       //>读角速度内环P
    PID_Omega.kD = flash_read(EEPROM_SECTOR, index++, float);       //>读角速度内环D
    PID_Omega.kI = 0.0f;                                            //>读角速度内环I

    PID_Location.kP = flash_read(EEPROM_SECTOR, index++, float);    //>读位置外环P
    PID_Location.kD = flash_read(EEPROM_SECTOR, index++, float);    //>读位置外环D
    PID_Location.kI = 0.0f;                                         //>读位置外环I

    PID_Velocity_L.kP = flash_read(EEPROM_SECTOR, index++, float);  //>读速度环L_P
    PID_Velocity_L.kI = flash_read(EEPROM_SECTOR, index++, float);  //>读速度环L_I
    PID_Velocity_L.kD = flash_read(EEPROM_SECTOR, index++, float);  //>读速度环L_D

    PID_Velocity_R.kP = PID_Velocity_L.kP;      //>读速度环R_P
    PID_Velocity_R.kI = PID_Velocity_L.kI;      //>读速度环R_I
    PID_Velocity_R.kD = PID_Velocity_L.kD;      //>读速度环R_D

    MT9V034.threshold_offset = flash_read(EEPROM_SECTOR, index++, uint8);   //>读取阈值增幅
    MT9V034.threshold = flash_read(EEPROM_SECTOR, index++, uint8);          //>读取阈值

    LIMIT_V_MAX = flash_read(EEPROM_SECTOR, index++, float);    //>读充电最大电压
    LIMIT_V_MIN = flash_read(EEPROM_SECTOR, index,   float);    //>读充电最小电压

    Mode[0][0] = !gpio_get(DIP4);   // MT9V034
    Mode[0][1] = !gpio_get(DIP5);   // Threshold
    Mode[0][2] = !gpio_get(DIP6);   // Looking
    Mode[0][3] = !gpio_get(DIP7);   // Run
    Mode[1][0] = !gpio_get(DIP0);
    Mode[1][1] = !gpio_get(DIP1);
    Mode[1][2] = !gpio_get(DIP2);
    Mode[1][3] = !gpio_get(DIP3);
}
