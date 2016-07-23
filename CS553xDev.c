/******************************************************************************

               CS5532设备驱动程序接口
CS5532设备建立在SpiDev之上，通过不同片选可提供多个CS5532芯片支持
******************************************************************************/

#include "CS553xDev.h"

#include <string.h>

//------------------------------SpiDevCmd配置函数实现------------------------
static void _cbCfg(void *pSpiDev, //struct _SpiDev *pSpiDev,   //挂接设备
                   unsigned char CallPos)    //设用位置
{
  //struct _CS553xDev *pDev = CS553xDev_cbGet(pSpiDev);
  switch(CallPos){
    case SPI_DEV_CALL_POS_CS_VALID:
	    CS553xDev_cbClrCs(pDev->Cmd.Cs);
	  break;
    case SPI_DEV_CALL_POS_CS_INVALID://命令模式无数据,要求退出时不能放开CS
	  if(!(CS553xDev_cbGet(pSpiDev)->Flag & CS553X_DEV_FLAG_MC))//转换标志未置位时允许释放
	   CS553xDev_cbSetCs(pDev->Cmd.Cs);
	  break;
    //case SPI_DEV_CALL_POS_CMD_CFG:
    //case SPI_DEV_CALL_POS_DATA_CFG:
    default: //命令与发送数据格式相同
      CS553xDev_cbCfgSpiDev(pSpiDev);
    break;
  }
}

//------------------------------SpiDevCmd中断完成回调函数实现-----------------
static void _cbEndInt(void *pSpiDev) //struct _SpiDev *pSpiDev)
{
  CS553xDev_cbFinal(CS553xDev_cbGet(pSpiDev));
};

//-------------------------------初始化函数---------------------------------
//调用此函数前确保对应片选引脚已置为输出高电平状态
void CS553xDev_Init(struct _CS553xDev *pDev,
                  struct _SpiDev *pSpiDev,   //挂接设备
                  unsigned char Cs)     //片选位置
{
  memset(pDev, 0, sizeof(struct _CS553xDev));
  pDev->pSpiDev = pSpiDev;
  //初始化命令:
  pDev->Cmd.pCmd = pDev->CmdBuf;
  //pDev->Cmd.pData = NULL;  //数据缓冲区在读写数据中提供
  pDev->Cmd.CmdSize = 1;    //固定为1
  //pDev->Cmd.DataSize = 0;  //数据大小和缓冲区在读写数据中提供
  pDev->Cmd.Cs = Cs;
  //pDev->Cmd.Cfg = 0;       //控制字在读写数据中提供
  pDev->Cmd.cbCfg = _cbCfg;
  pDev->Cmd.cbEndInt = _cbEndInt;
  //初始化其它:

}

//-----------------------------读写数据启动函数---------------------------------
//此函数仅启动SPI通讯,需查询对应SPI状态获得SPI通讯是否完成
//返回0:成功 -1:SPI设备被占用未释放
signed char CS553xDev_DataStart(struct _CS553xDev *pDev,    //设备
                            unsigned char Cmd,         //定义的CS553xDev命令字
                            void *pData,              //需收发的数据
                            unsigned char Size)        //收发数据大小
{
  pDev->CmdBuf[0] = Cmd;
  pDev->Cmd.pData = pData;
  pDev->Cmd.DataSize = Size;

  /*/确定读写标志,使用半双工SPI操作:
  //命令模式(最高位置位),或普通模式写(读位未置位)且不为0(0为数据读)时均为写状态
  if((Cmd >= 0x80) || (!(Cmd & CC553x_RW_RD) && (Cmd)))
    pDev->Cmd.Cfg = SPI_DEV_CMD_DIS_RX; //写数据时不接收数据
  else
    pDev->Cmd.Cfg = SPI_DEV_CMD_DIS_TX; //读数据时不发送数据
  */

  pDev->Cmd.Cfg = SPI_DEV_CMD_EN_CMD_RCV;//使用全双工操作,将在发送命令同时接收数据

  if(SpiDev_Restart(pDev->pSpiDev, &pDev->Cmd)) pDev->ErrCount++; //启动SPI
  else return 0;

  return -1;
}

//-----------------------------读写数据函数---------------------------------
//此函数以阻塞方式读写数据,返回通讯即完成
//返回0:成功 -1:SPI设备被占用未释放 -2;通讯错误, -3:通讯超时
signed char CS553xDev_Data(struct _CS553xDev *pDev,    //设备
                       unsigned char Cmd,         //定义的CS553xDev命令字
                       void *pData,              //需收发的数据
                       unsigned char Size)        //收发数据大小
{
  struct _SpiDev *pSpiDev;
  signed char ErrFlag;
  if(CS553xDev_DataStart(pDev, Cmd, pData, Size)) return  -1; //已计数
  pSpiDev = pDev->pSpiDev;
  while(pSpiDev->Flag & SPI_DEV_BUSY);//等待完成
  if(pSpiDev->Flag & SPI_DEV_ERR) ErrFlag = -2;
  else if(pSpiDev->Flag & SPI_DEV_OV_ERR) ErrFlag = -3;
  else ErrFlag = 0;

  if(ErrFlag) pDev->ErrCount++;
  return ErrFlag;
}

//-----------------------------读写长整数据启动函数---------------------------------
//此函数以阻塞方式读写数据,返回通讯即完成
//返回0:成功 -1:SPI设备被占用未释放
//读写数据后的数据需通过S553xDev_GetULong获得
signed char CS553xDev_UlongStart(struct _CS553xDev *pDev,    //设备
                                 unsigned char Cmd,         //定义的CS553xDev命令字
                                 unsigned long Data)       //被写的数据
{
  pDev->DataBuf[0] = (unsigned char)(Data >> 24);
  pDev->DataBuf[1] = (unsigned char)(Data >> 16);
  pDev->DataBuf[1] = (unsigned char)(Data >> 8);
  pDev->DataBuf[1] = (unsigned char)(Data >> 0);
  return CS553xDev_DataStart(pDev, Cmd, pDev->DataBuf, 4);
}

//-----------------------------读写长整数据函数---------------------------------
//此函数仅启动SPI通讯,需查询对应SPI状态获得SPI通讯是否完成
//返回0:成功 -1:SPI设备被占用未释放
//读写数据后的返回值需通过S553xDev_GetULong获得
signed char CS553xDev_Ulong(struct _CS553xDev *pDev,    //设备
                            unsigned char Cmd,         //定义的CS553xDev命令字
                            unsigned long Data)       //被写的数据
{
  struct _SpiDev *pSpiDev;
  signed char ErrFlag;
  pDev->DataBuf[0] = (unsigned char)(Data >> 24);
  pDev->DataBuf[1] = (unsigned char)(Data >> 16);
  pDev->DataBuf[2] = (unsigned char)(Data >> 8);
  pDev->DataBuf[3] = (unsigned char)(Data >> 0);
  if(CS553xDev_DataStart(pDev, Cmd, pDev->DataBuf, 4)) return -1;//已计数
  pSpiDev = pDev->pSpiDev;

  while(pSpiDev->Flag & SPI_DEV_BUSY);//等待完成
  if(pSpiDev->Flag & SPI_DEV_ERR) ErrFlag = -2;
  else if(pSpiDev->Flag & SPI_DEV_OV_ERR) ErrFlag = -3;
  else ErrFlag = 0;

  if(ErrFlag) pDev->ErrCount++;
  return ErrFlag;
}

//-----------------------------得到读写后的长整数据---------------------------------
//此函数用于在CS553xDev_UlongStart() CS553xDev_Ulong()完成SPI通讯后,得到返回值
unsigned long S553xDev_GetULong(const struct _CS553xDev *pDev)
{
  unsigned long Data;
  Data = (unsigned long)pDev->DataBuf[0] << 24;
  Data |= (unsigned long)pDev->DataBuf[1] << 16;
  Data |= (unsigned long)pDev->DataBuf[2] << 8;
  Data |= (unsigned long)pDev->DataBuf[3] << 0;
  return Data;
}

//---------------------------写命令函数---------------------------------
//此函数仅启动SPI通讯,需查询对应SPI状态获得SPI通讯是否完成
//返回0:成功 -1:SPI设备被占用未释放 -2;通讯错误, -3:通讯超时
signed char CS553xDev_WrCmd(struct _CS553xDev *pDev,    //设备
                            unsigned char Cmd)         //定义的CS553xDev命令字
{

  struct _SpiDev *pSpiDev;
  signed char ErrFlag;
  CS553xDev_DataStart(pDev, Cmd, NULL, 0);

  pSpiDev = pDev->pSpiDev;
  while(pSpiDev->Flag & SPI_DEV_BUSY);//等待完成
  if(pSpiDev->Flag & SPI_DEV_ERR) ErrFlag = -2;
  else if(pSpiDev->Flag & SPI_DEV_OV_ERR) ErrFlag = -3;
  else ErrFlag = 0;

  if(ErrFlag) pDev->ErrCount++;
  return ErrFlag;
}



