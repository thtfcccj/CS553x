/******************************************************************************

                CS5532应用相关实现
//主要实现与SW-24的应用程序接口
******************************************************************************/

#include "CS5532App.h"
#include "Spi.h"
//#include <avr/IO.h>  //AVRGCC环境时
#include <string.h>

#include "Delay.h" //DelayMs()
#include "Int0.h"  //引脚使用中断

extern void vAdDataProRecall(unsigned long AD);

struct _CS5532App CS5532App;  //直接实例化

//-------------------------------初始化函数-------------------------------
void CS5532App_Init(void)
{
  memset(&CS5532App, 0, sizeof(struct _CS5532App));
  //中断系统初始化与配置
	Int0_Init(INT0_MODE_RISE); //先初始化并配为下降沿模式
  //通讯初始化
  SetSPI_nCS5531();     //取消片选
  SPCR |= (1<<SPR1); //设置时钟速率固定为fck/16
  CS553xDev_Init(&CS5532App.CS5532Dev, &SpiDev, 0);
}
//-------------------------------芯片初始化函数-----------------------------
//返回值定义为: 0xff:芯片复位错误,其它:通讯期间故障发生次数
unsigned char CS5532App_CS5532Init(void)
{
  S553xDev_ClrErrCount(&CS5532App.CS5532Dev);  //错误计数器复位
  DelayMs(20);

  //强制停止现有单次转换??
  Int0_Dis();//关闭查询装换完成中断
  S553xDev_ClrFlagMc(&CS5532App.CS5532Dev); //清转换标志

  //写复位序列
  memset(CS5532App.DataBuf, 0xff,14);
  CS5532App.DataBuf[14] = 0xfe;
  CS553xDev_Data(&CS5532App.CS5532Dev,    //设备
                CC553x_RESET,         //定义的CS553xDev命令字
                CS5532App.DataBuf,              //需收发的数据
                15);        //收发数据大小
  //寄存器复位
  CS553xDev_Ulong(&CS5532App.CS5532Dev,    //设备
                  CC553x_CFG | CC553x_CS_0 | CC553x_RSB_CFG, //定义的CS553xDev命令字
                  CC553x_CFG_RST);              //需收发的数据
  //读寄存器复位完成标志
  CS553xDev_Ulong(&CS5532App.CS5532Dev,    //设备
                  CC553x_CFG | CC553x_RW_RD | CC553x_CS_0 | CC553x_RSB_CFG, //定义的CS553xDev命令字
                  CC553x_CFG_RST);              //需收发的数据

  if(!(S553xDev_GetULong(&CS5532App.CS5532Dev) & CC553x_CFG_RV)) return 0xff;//芯片复位错误


  //配置寄存器相关设置: 参考电压选1V-2.5V,其它默认
  CS553xDev_Ulong(&CS5532App.CS5532Dev,    //设备
                  CC553x_CFG | CC553x_CS_0 | CC553x_RSB_CFG, //定义的CS553xDev命令字
                  CC553x_CFG_VGS );              //需收发的数据
  CS553xDev_Ulong(&CS5532App.CS5532Dev,    //读测试
                  CC553x_CFG | CC553x_RW_RD | CC553x_CS_0 | CC553x_RSB_CFG, //定义的CS553xDev命令字
                  CC553x_CFG_VGS );              //需收发的数据

  //通道设置寄存器0,1,2,3设置:位速率设为20480个主时钟周期
  unsigned long Data = ((CC553x_CSR_CS_0 | CC553x_CSR_G_1 | CC553x_CSR_WR_20480 | CC553x_CSR_UB_B)
                         << CC553x_CSR_EVEN_SHIRT) |
                        (CC553x_CSR_CS_0 | CC553x_CSR_G_1 | CC553x_CSR_WR_20480 | CC553x_CSR_UB_B);
  CS553xDev_Ulong(&CS5532App.CS5532Dev,    //设备
                  CC553x_CFG | CC553x_CS_0 | CC553x_RSB_CSR, //定义的CS553xDev命令字
                  Data);              //需收发的数据
  CS553xDev_Ulong(&CS5532App.CS5532Dev,    //读测试
                  CC553x_CFG | CC553x_RW_RD | CC553x_CS_0 | CC553x_RSB_CSR, //定义的CS553xDev命令字
                  Data);              //需收发的数据

  //CS553xDev_Ulong(&CS5532App.CS5532Dev,    //设备
  //                CC553x_CFG | CC553x_CS_1 | CC553x_RSB_CSR, //定义的CS553xDev命令字
  //                Data);              //需收发的数据

  /*/自校准:零点与增益:
  CS553xDev_Ulong(&CS5532App.CS5532Dev,    //设备
                  CC553x_CMD | CC553x_CSRP_0 | CC553x_CC_GAIN_ADJ_OWN, //定义的CS553xDev命令字
                  0);              //需收发的数据
  DelayMs(100);
  CS553xDev_Ulong(&CS5532App.CS5532Dev,    //设备
                  CC553x_CMD | CC553x_CSRP_0 | CC553x_CC_OFF_ADJ_OWN, //定义的CS553xDev命令字
                  0);              //需收发的数据
  DelayMs(100);*/

  return S553xDev_GetErrCount(&CS5532App.CS5532Dev);
}

//-----------------------------CS553xDevSpi设备通讯完成回调函数-------------------------
void CS553xDev_cbFinal(struct _CS553xDev *pDev)
{
  if(CS5532App.eState != eCS5532App_State_Done) return;
   //转换完成后获得结果计算后，再次进行转换
   unsigned char Data = S553xDev_GetULong(&CS5532App.CS5532Dev);
   if(Data & CC553x_DATA_OF){//数据溢出了
     if(CS5532App.Timer >2) //让主程序减最后一个值恢复错误
	   CS5532App.Timer--;

   }
   else CS5532App.Timer = CS5532APP_TIMER_COUNT; //过程完整结束，复位!

   CS5532App_cbFinal(S553xDev_GetULong(&CS5532App.CS5532Dev)); //得到转换结果
   Int0_En(); //再次开启Int0中断等待中断
  CS5532App.eState = eCS5532App_State_Wait;
}

//-----------------------------CS553xDevMISO引脚中断函数实现-------------------------
void CS553xDev_Int(void)
{
  if(CS5532App.eState != eCS5532App_State_Wait) return;
  //Int0_Dis();//关闭查询装换完成中断
  //AD完成，准备收数据
  CS553xDev_UlongStart(&CS5532App.CS5532Dev,    //设备
                       CC553x_NULL, //读取单次转换结果
                       0);              //需收发的数据

  CS5532App.eState = eCS5532App_State_Done;
}

//-----------------------------任务函数-----------------------------
//放入一个CS5532读写周期及一个单次AD转换周期能够处理完成的系统进程中
void CS5532App_Task(void)
{
  if(CS5532App.Timer){
     CS5532App.Timer--;
	 if(!CS5532App.Timer){//出错了,恢复
	   CS5532App_Init();
       CS5532App_CS5532Init();
     }
   }
  if(CS5532App.eState != eCS5532App_State_Idie) return;
  S553xDev_SetFlagMc(&CS5532App.CS5532Dev); //置转换标志
  CS553xDev_WrCmd(&CS5532App.CS5532Dev,    //设备
                       CC553x_CMD | CC553x_MC | CC553x_CSRP_0 | CC553x_CC_NOR); //启动配置通道0连续转换
  CS5532App.eState = eCS5532App_State_Wait;


  Int0_CfgMode(INT0_MODE_CHANGE);//电平变化产生中断
  Int0_En(); //开启Int中断
  CS5532App.Timer = CS5532APP_TIMER_COUNT; //开始错误计数
}

