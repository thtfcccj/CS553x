/******************************************************************************

               CS553x�豸��������ӿ�-��̬�غ����ӿڻ��ʵ��
�˽ӿ�����ʵ��CS553xDev.c��Ҫ��ľ�̬�ص�����
���ļ���Ӧ�����,���в�ͬ��ʵ��
******************************************************************************/

#ifndef __CS553x_DEV_CB_H
#define __CS553x_DEV_CB_H

//---------------------------��Spi�豸�õ�CS553x�豸����----------------------
//struct _CS553xDev *CS553xDev_cbGet(const struct _SpiDev *pSpiDev);
//��Ӧ�ò��һ��CS5532,ֱ��ʵ��Ϊ:
#include "CS5532App.h"
#define CS553xDev_cbGet(pSpiDev) (&CS5532App.CS5532Dev)

//------------------------------����Ƭѡ��IO�ڵ͵�ƽ----------------------------
//void CS553xDev_cbClrCs(unsigned char Cs);
//��Ӧ�ò�SPI����һ��CS5532ͨѶ,ֱ��ʵ��Ϊ
#include "IOCtrl.h"
#define CS553xDev_cbClrCs(Cs)   do{ClrSPI_nCS5531();}while(0)

//------------------------------����Ƭѡ��IO�ڸߵ�ƽ----------------------------
//void CS553xDev_cbSetCs(unsigned char Cs);
//��Ӧ�ò�SPI����һ��CS5532ͨѶ��ֱ��ʵ��Ϊ
#define CS553xDev_cbSetCs(Cs)   do{SetSPI_nCS5531();}while(0)

//-----------------------------���ùҽӵ�Spi�豸ͨѶ����-------------------------
//void CS553xDev_cbCfgSpiDev(struct _SpiDev *pSpiDev);
//��Ӧ�ò�SPI����һ��CS5532ͨѶ��ͨѶ������Ԥ�ã��ʿ�ֱ��ʵ��Ϊ��:
#define CS553xDev_cbCfgSpiDev(pSpiDev) do{}while(0)

//-----------------------------CS553xDevSpi�豸ͨѶ��ɻص�����-------------------------
void CS553xDev_cbFinal(struct _CS553xDev *pDev);
//��Ӧ�ò㲻��Ҫ�˹��ܣ��ʿ�ֱ��ʵ��Ϊ��:
//#define CS553xDev_cbFinal(pDev)  do{}while(0)




#endif //#define __CS553x_DEV_CB_H









