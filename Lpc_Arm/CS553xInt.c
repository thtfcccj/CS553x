/***************************************************************************
                  CS553X的中断实现
此实现直接LPC112X硬件相关
***************************************************************************/

#include "IOCtrl.h"
#include "CS5532App.h"

/******************************************************************************
                    \MISO引脚中断相关回调函数实现   
******************************************************************************/
//-----------------------------中断函数实现-------------------------------
void CS553X_INT_IRQHandler(void)
{
  if(pCS553X_INT->MIS & CS553X_INT_MASK)//引脚中断了
    CS553xDev_MisoInt();
  
  pCS553X_INT->IC = CS553X_INT_MASK; //清中断
}

//----------------------------MISO引脚中断配置回调函数-------------------------
//将此脚配置为下降沿中断模式
void CS553xDev_cbMisoIntCfg(void)
{
  //1.初始化电源控制及时钟
  //LPC_SYSCON->PDRUNCFG &= ~XXX_PD;    //禁止掉电
  //LPC_SYSCON->PRESETCTRL |= XXX_RST_N; //集中复位
  //LPC_SYSCON->SYSAHBCLKCTRL |= (unsigned int)GPIO0_ACC;//已统一开启时钟

  //2.初始化管脚配置
  CfgCS553X_INT();
  InCS553X_INT();
  
  //3.初始化其它
  pCS553X_INT->IE &= ~CS553X_INT_MASK;//默认禁止中断
  pCS553X_INT->IC = CS553X_INT_MASK; //先清中断防止异常
 
  pCS553X_INT->IS &= ~CS553X_INT_MASK;//沿触发模式,上电默认
  pCS553X_INT->IBE &= ~CS553X_INT_MASK;//单边触发模式,上电默认 
  pCS553X_INT->IEV &= ~CS553X_INT_MASK;//下降沿触发模式,上电默认

  //4.初始化变量及模块

  //5.最后允许中断
  NVIC_EnableIRQ(EINT0_IRQn); //开启中断
}