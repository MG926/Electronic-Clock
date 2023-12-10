/**
  ******************************************************************************
  * @file           : main.c
  * @version        : v1.0
  * @author         : Xiaohei
  * @date           : 2021-11-20
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Project git: github.com/mobyw/DigitalTubeClock
  *
  ******************************************************************************
  */

#include "main.h"

// 打开蜂鸣器
//#define ENABLE_BELL

// 页面数量
#define NUM_PAGE 9

// 时间动画数量
#define NUM_TIME_ANIMATE 6

// 脱机动画数量
#define NUM_OFFLINE_ANIMATE 7

// 定时器计次
unsigned char timer_cnt = 0;

// 定时器计次
unsigned char tick = 0;

// 显示延时 控制亮度
unsigned int light = 8;

// 存储温度结果
unsigned int temperature = 20;

// 闹钟开关
bit alarm_control = 0;

// 亮度调节开关
bit light_control = 0;

// 时间动画
// 可在此处添加每个动画的注释
// 0: 显示秒
// 1: 反色显示秒
unsigned char time_animate = 0;

// 脱机动画
// 可在此处添加每个动画的注释
// 0: 正向显示动画1
// 1: 反向显示动画1
unsigned char offline_animate = 0;

// 当前页面序号
// 0: 显示时间 时间动画
// 1: 调整分钟
// 2: 调整小时
// 3: 调整闹钟分钟
// 4: 调整闹钟小时
// 5: 闹钟开关
// 6: 显示学号 脱机动画
// 7: 显示温度
// 8: 光敏开关
char page = 0;
// 按键扫描
void keyScan()
{
    // 禁用 ADC 转换
    disableAdc();

    // 关闹钟
    if (alarm_control == 1)
    {
        if (time_hour == alarm_hour && time_min == alarm_min)
        {
            if (BTN1 == 0)
            {
                delay(10);

                if (BTN1 == 0)
                {
                    alarm_control = 0;

                    // 等待按键松开
                    while (BTN1 == 0)
                        ;
                }
            }
            if (BTN2 == 0)
            {
                delay(10);

                if (BTN2 == 0)
                {
                    alarm_control = 0;

                    // 等待按键松开
                    while (BTN2 == 0)
                        ;
                }
            }
        }
    }

    // 按键 1 按下
    if (BTN1 == 0)
    {
        delay(20);

        if (BTN1 == 0)
        {
#ifdef ENABLE_BELL
            // 蜂鸣器响
            bell = 0;
#endif
            // 跳转下一个页面
            page++;

            if (page == 3)
            {
                // 写入设置的时间
                write_time();
            }

            // 到最后一个页面时返回页面 0
            if (page == NUM_PAGE)
            {
                page = 0;
            }

            // 等待按键松开
            while (BTN1 == 0)
                ;
#ifdef ENABLE_BELL
            // 关闭蜂鸣器
            bell = 1;
#endif
        }
        delay(10);
    }

    // 时间界面 按键切换 LED 时间动画处理
    if (page == 0)
    {
        if (BTN2 == 0)
        {
            delay(20);

            if (BTN2 == 0)
            {
#ifdef ENABLE_BELL
                bell = 0;
#endif
                time_animate = (time_animate + 1) % NUM_TIME_ANIMATE;
                while (BTN2 == 0)
                    ;
#ifdef ENABLE_BELL
                bell = 1;
#endif
            }
            delay(10);
        }
				
    }

    // 设置分钟界面 按键处理
    if (page == 1)
    {
        // 按键 2 按下
        if (BTN2 == 0)
        {
            delay(20);

            if (BTN2 == 0)
            {
#ifdef ENABLE_BELL
                // 蜂鸣器响
                bell = 0;
#endif
                // 分钟超过 59 恢复 0
                if (time_min >= 0x60)
                    time_min = 0;

                // 设置分钟加 1
                time_min = time_min + 0x01;

                // 十六进制转换为 BCD
                if ((time_min & 0x0f) >= 0x0a)
                    time_min = (time_min & 0xf0) + 0x10;

                // 等待松开
                while (BTN2 == 0)
                    ;
#ifdef ENABLE_BELL
                // 关闭蜂鸣器
                bell = 1;
#endif
            }
            delay(10);
        }
    }

    // 设置小时界面 按键处理
    if (page == 2)
    {
        if (BTN2 == 0)
        {
            delay(20);

            if (BTN2 == 0)
            {
#ifdef ENABLE_BELL
                bell = 0;
#endif
                time_hour += 0x01;
                if ((time_hour & 0x0f) >= 0x0a)
                    time_hour = (time_hour & 0xf0) + 0x10;
                if (time_hour >= 0x24)
                    time_hour = 0;
                while (BTN2 == 0)
                    ;
#ifdef ENABLE_BELL
                bell = 1;
#endif
            }
            delay(10);
        }
    }

    // 设置闹钟分钟界面 按键处理
    if (page == 3)
    {
        // 按键 2 按下
        if (BTN2 == 0)
        {
            delay(20);

            if (BTN2 == 0)
            {
#ifdef ENABLE_BELL
                // 蜂鸣器响
                bell = 0;
#endif
                // 分钟超过 59 恢复 0
                if (alarm_min >= 0x60)
                    alarm_min = 0;

                // 设置分钟加 1
                alarm_min = alarm_min + 0x01;

                // 十六进制转换为 BCD
                if ((alarm_min & 0x0f) >= 0x0a)
                    alarm_min = (alarm_min & 0xf0) + 0x10;

                // 等待松开
                while (BTN2 == 0)
                    ;
#ifdef ENABLE_BELL
                // 关闭蜂鸣器
                bell = 1;
#endif
            }
            delay(10);
        }
    }

    // 设置闹钟小时界面 按键处理
    if (page == 4)
    {
        if (BTN2 == 0)
        {
            delay(20);

            if (BTN2 == 0)
            {
#ifdef ENABLE_BELL
                bell = 0;
#endif
                alarm_hour += 0x01;
                if ((alarm_hour & 0x0f) >= 0x0a)
                    alarm_hour = (alarm_hour & 0xf0) + 0x10;
                if (alarm_hour >= 0x24)
                    alarm_hour = 0;
                while (BTN2 == 0)
                    ;
#ifdef ENABLE_BELL
                bell = 1;
#endif
            }
            delay(10);
        }
    }

    // 设置闹钟开关界面 按键处理
    if (page == 5)
    {
        if (BTN2 == 0)
        {
            delay(20);

            if (BTN2 == 0)
            {
#ifdef ENABLE_BELL
                bell = 0;
#endif
                alarm_control = !alarm_control;
                while (BTN2 == 0)
                    ;
#ifdef ENABLE_BELL
                bell = 1;
#endif
            }
            delay(10);
        }
    }

    // 学号界面 按键切换 LED 脱机动画处理
    if (page == 6)
    {
        if (BTN2 == 0)
        {
            delay(20);

            if (BTN2 == 0)
            {
#ifdef ENABLE_BELL
                bell = 0;
#endif
                offline_animate = (offline_animate + 1) % NUM_OFFLINE_ANIMATE;
                while (BTN2 == 0)
                    ;
#ifdef ENABLE_BELL
                bell = 1;
#endif
            }
            delay(10);
        }
    }

    // 设置光控开关界面 按键处理
    if (page == 8)
    {
        if (BTN2 == 0)
        {
            delay(20);

            if (BTN2 == 0)
            {
#ifdef ENABLE_BELL
                bell = 0;
#endif
                light_control = !light_control;
                while (BTN2 == 0)
                    ;
#ifdef ENABLE_BELL
                bell = 1;
#endif
            }
            delay(10);
        }
    }

    // 使能 ADC 转换
    enableAdc();
}
#define reset_bit1(x,y) x&=((0xff>>(y+1))<<(y+1))
#define set_bit1(x,y) x|=~(~(0>>(y+1))<<(y+1))
void setLed1(unsigned char index)
{
    index = (index + 59) % 60;
    reset_bit1(led_array[index / 8], index % 8);
}
void resetLed1(unsigned char index)
{
    index = (index + 59) % 60;
    set_bit1(led_array[index / 8], index % 8);
}
// 主函数
void main()
{
    unsigned char i = 0;
	  int k,s;
		
	  
	

    // 初始化
    systemInit();

    // 主循环
    while (1)
    {
        // 按键扫描
        keyScan();

        // 显示时间界面
        if (page == 0)
        {
					
					// 读取时间
            
						
						read_time();
						if(bcd2hex(time_sec)<=2)
						{
							display_array[0] = 1;
							display_array[1] = 8;
							display_array[2] = 7;
							display_array[3] = 8;
						}
						else if(bcd2hex(time_sec)>=30 && bcd2hex(time_sec)<=32)
						{
							temperature = getTemperature();

							display_array[0] = _SPACE;
							display_array[1] = temperature / 1000;
							display_array[2] = temperature % 1000 / 100;
							display_array[3] = _C;
						}
						
						else	// 解析时间
						{
							display_array[0] = time_hour / 16;
							display_array[1] = time_hour % 16;
							display_array[2] = time_min / 16;
							display_array[3] = time_min % 16;
						}	
            // 填充 LED 数组
						
            if (time_animate == 0)
            {
                ledAllOff();
                setLed(bcd2hex(time_sec));
            }
            else if (time_animate == 1)
            {
                ledAllOn();
                resetLed(bcd2hex(time_sec));
            }
            else if (time_animate == 2)
            {
                ledAllOff();
                setLed(bcd2hex(time_sec));
								if (bcd2hex(time_sec)%60>=5)
								{
									led_array[0]&=0xef;
								}
								if (bcd2hex(time_sec)%60>=10)
								{
									led_array[1]&=0xfd;
								}
								if (bcd2hex(time_sec)%60>=15)
								{
									led_array[1]&=0xbd;
								}
								if (bcd2hex(time_sec)%60>=20)
								{
									led_array[2]&=0xf7;
								}
								if (bcd2hex(time_sec)%60>=25)
								{
									led_array[3]&=0xfe;
								}
								if (bcd2hex(time_sec)%60>=30)
								{
									led_array[3]&=0xde;
								}
								if (bcd2hex(time_sec)%60>=35)
								{
									led_array[4]&=0xfb;
								}
								if (bcd2hex(time_sec)%60>=40)
								{
									led_array[4]&=0x7b;
								}
								if (bcd2hex(time_sec)%60>=45)
								{
									led_array[5]&=0xef;
								}
								if (bcd2hex(time_sec)%60>=50)
								{
									led_array[6]&=0xfd;
								}
								if (bcd2hex(time_sec)%60>=55)
								{
									led_array[6]&=0xbd;
								}
								
            }
						else if (time_animate == 3)
						{
								ledAllOn();
                resetLed(bcd2hex(time_sec));
								if (bcd2hex(time_sec)%60>=5)
								{
									led_array[0]|=~0xef;
								}
								if (bcd2hex(time_sec)%60>=10)
								{
									led_array[1]|=~0xfd;
								}
								if (bcd2hex(time_sec)%60>=15)
								{
									led_array[1]|=~0xbd;
								}
								if (bcd2hex(time_sec)%60>=20)
								{
									led_array[2]|=~0xf7;
								}
								if (bcd2hex(time_sec)%60>=25)
								{
									led_array[3]|=~0xfe;
								}
								if (bcd2hex(time_sec)%60>=30)
								{
									led_array[3]|=~0xde;
								}
								if (bcd2hex(time_sec)%60>=35)
								{
									led_array[4]|=~0xfb;
								}
								if (bcd2hex(time_sec)%60>=40)
								{
									led_array[4]|=~0x7b;
								}
								if (bcd2hex(time_sec)%60>=45)
								{
									led_array[5]|=~0xef;
								}
								if (bcd2hex(time_sec)%60>=50)
								{
									led_array[6]|=~0xfd;
								}
								if (bcd2hex(time_sec)%60>=55)
								{
									led_array[6]|=~0xbd;
								}
						}
						else if (time_animate == 4)
						{
								ledAllOff();
                setLed1(bcd2hex(time_sec));
								for(k=0;k<7;k++)
								{
									if(bcd2hex(time_sec)>=8*(k+1))
									{
										led_array[k]=0;
									}
								}
								if(bcd2hex(time_sec)==0)
								{
									led_array[7]=0xf;
									bell=!bell;
									delay(1000);
									bell=1;
								}
								
								
						}
						else
						{
								ledAllOn();
								resetLed1(bcd2hex(time_sec));
								for(k=0;k<7;k++)
								{
									if(bcd2hex(time_sec)>=8*(k+1))
									{
										led_array[k]=0xff;
									}
								}
								if(bcd2hex(time_sec)==0)
								{
									led_array[7]=0;
									bell=!bell;
									delay(1000);
									bell=1;
								}
						}
							
						
        }

        // 设置时间 分钟
        if (page == 1)
        {
            ledAllOff();

            if (blink > 0)
            {
                display_array[2] = time_min / 16;
                display_array[3] = time_min % 16;
            }
            else
            {
                display_array[2] = _SPACE;
                display_array[3] = _SPACE;
            }
            display_array[0] = time_hour / 16;
            display_array[1] = time_hour % 16;
        }

        // 设置时间 小时
        if (page == 2)
        {
            ledAllOff();

            display_array[2] = time_min / 16;
            display_array[3] = time_min % 16;
            if (blink > 0)
            {
                display_array[0] = time_hour / 16;
                display_array[1] = time_hour % 16;
            }
            else
            {
                display_array[0] = _SPACE;
                display_array[1] = _SPACE;
            }
        }

        // 设置闹钟时间 分钟
        if (page == 3)
        {
            ledAllOff();

            if (blink > 0)
            {
                display_array[2] = alarm_min / 16;
                display_array[3] = alarm_min % 16;
            }
            else
            {
                display_array[2] = _SPACE;
                display_array[3] = _SPACE;
            }
            display_array[0] = alarm_hour / 16;
            display_array[1] = alarm_hour % 16;
        }

        // 设置闹钟时间 小时
        if (page == 4)
        {
            ledAllOff();

            display_array[2] = alarm_min / 16;
            display_array[3] = alarm_min % 16;
            if (blink > 0)
            {
                display_array[0] = alarm_hour / 16;
                display_array[1] = alarm_hour % 16;
            }
            else
            {
                display_array[0] = _SPACE;
                display_array[1] = _SPACE;
            }
        }

        // 闹钟开关
        if (page == 5)
        {
            ledAllOff();

            display_array[0] = _C;
            display_array[1] = _SPACE;
            display_array[2] = _SPACE;
            display_array[3] = alarm_control;
        }

        // 显示学号
        if (page == 6)
        {
            display_array[0] = 1;
            display_array[1] = 8;
            display_array[2] = 7;
            display_array[3] = 8;
					
            if (tick == 1)
            {
                tick = 0;
                i = i + 1;
            }

            // 填充 LED 数组
            if (offline_animate == 0)
            {
								i%=15;
                ledAllOff();
                setLed(i);
                setLed(i + 15);
                setLed(i + 30);
                setLed(i + 45);
            }
            else if (offline_animate == 1)
            {
								i%=15;
                ledAllOn();
                resetLed(i);
                resetLed(i + 15);
                resetLed(i + 30);
                resetLed(i + 45);
            }
            else if (offline_animate == 2)
            {
								i%=10;
                ledAllOff();
                setLed(i);
                setLed(i + 10);
                setLed(i + 20);
                setLed(i + 30);
								setLed(i + 40);
								setLed(i + 50);
								
            }
						else if (offline_animate == 3)
						{
								i%=10;
								ledAllOn();
                resetLed(i);
                resetLed(i + 10);
                resetLed(i + 20);
                resetLed(i + 30);
								resetLed(i + 40);
								resetLed(i + 50);
								
						}
						else if (offline_animate == 4)
						{
								i%=30;
								ledAllOff();
								setLed(i);
								setLed(i + 30);
												
						}
						else if (offline_animate == 5)
						{
								i%=30;
								ledAllOn();
                resetLed(i);
                resetLed(i + 30);
								
						}
						else
						{	
								ledAllOff();
								for(s=3;s<8;s++)
								{
									led_array[s]=0;
								}
								i%=60;
								if(i<=24)
								{
									setLed(i);
								}
								else
								{
									resetLed(i);
								}
						}
        }

        // 显示温度
        if (page == 7)
        {
            ledAllOff();

            temperature = getTemperature();

            display_array[0] = _SPACE;
            display_array[1] = temperature / 1000;
            display_array[2] = temperature % 1000 / 100;
            display_array[3] = _C;
        }

        // 光控开关
        if (page == 8)
        {
            ledAllOff();

            display_array[0] = _C;
            display_array[1] = _C;
            display_array[2] = _SPACE;
            display_array[3] = light_control;
        }

        // 光控判断
        if (light_control == 1)
        {
            light = getLight();
        }
        else
        {
            light = 8;
        }

        // 显示
        display();
        delay(light);
    }
}

// 定时器 1 中断
// 1 毫秒 @11.0592MHz
void intTimer1() interrupt 1
{
    TL0 = (65536 - 20000) / 256; // 设置定时初值
    TH0 = (65536 - 20000) % 256; // 设置定时初值

    timer_cnt++;

    if (timer_cnt % 10 == 0)
    {
        tick = 1;
    }

    if (timer_cnt == 40)
    {
        timer_cnt = 0;

        if (page <= 4)
        {
            blink = !blink;
        }
        else
        {
            blink = 1;
        }

        // 闹钟控制
        if (alarm_control == 1)
        {
            if (time_hour == alarm_hour && time_min == alarm_min)
            {
                bell = !bell;
            }
        }
        else
        {
            bell = 1;
        }
    }
}
