/******************************************************************************

               CS553x设备驱动程序接口-静态回函数接口或宏实现
此接口用于实现CS553xDev.c中要求的静态回调函数
此文件与应用相关,可有不同的实现
******************************************************************************/

#ifndef __CS553x_DEV_CB_H
#define __CS553x_DEV_CB_H

//---------------------------由Spi设备得到CS553x设备函数----------------------
//struct _CS553xDev *CS553xDev_cbGet(const struct _SpiDev *pSpiDev);
//因应用层仅一个CS5532,直接实现为:
#include "CS5532App.h"
#define CS553xDev_cbGet(pSpiDev) (&CS5532App.CS5532Dev)

//------------------------------根据片选置IO口低电平----------------------------
//void CS553xDev_cbClrCs(unsigned char Cs);
//因应用层SPI仅与一个CS5532通讯,直接实现为
#include "IOCtrl.h"
#define CS553xDev_cbClrCs(Cs)   do{ClrSPI_nCS5531();}while(0)

//------------------------------根据片选置IO口高电平----------------------------
//void CS553xDev_cbSetCs(unsigned char Cs);
//因应用层SPI仅与一个CS5532通讯，直接实现为
#define CS553xDev_cbSetCs(Cs)   do{SetSPI_nCS5531();}while(0)

//-----------------------------配置挂接的Spi设备通讯函数-------------------------
//void CS553xDev_cbCfgSpiDev(struct _SpiDev *pSpiDev);
//因应用层SPI仅与一个CS5532通讯，通讯参数已预置，故可直接实现为空:
#define CS553xDev_cbCfgSpiDev(pSpiDev) do{}while(0)

//-----------------------------CS553xDevSpi设备通讯完成回调函数-------------------------
void CS553xDev_cbFinal(struct _CS553xDev *pDev);
//因应用层不需要此功能，故可直接实现为空:
//#define CS553xDev_cbFinal(pDev)  do{}while(0)




#endif //#define __CS553x_DEV_CB_H









