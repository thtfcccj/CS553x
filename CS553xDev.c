/******************************************************************************

               CS5532�豸��������ӿ�
CS5532�豸������SpiDev֮�ϣ�ͨ����ͬƬѡ���ṩ���CS5532оƬ֧��
******************************************************************************/

#include "CS553xDev.h"

#include <string.h>

//------------------------------SpiDevCmd���ú���ʵ��------------------------
static void _cbCfg(void *pSpiDev, //struct _SpiDev *pSpiDev,   //�ҽ��豸
                   unsigned char CallPos)    //����λ��
{
  //struct _CS553xDev *pDev = CS553xDev_cbGet(pSpiDev);
  switch(CallPos){
    case SPI_DEV_CALL_POS_CS_VALID:
	    CS553xDev_cbClrCs(pDev->Cmd.Cs);
	  break;
    case SPI_DEV_CALL_POS_CS_INVALID://����ģʽ������,Ҫ���˳�ʱ���ܷſ�CS
	  if(!(CS553xDev_cbGet(pSpiDev)->Flag & CS553X_DEV_FLAG_MC))//ת����־δ��λʱ�����ͷ�
	   CS553xDev_cbSetCs(pDev->Cmd.Cs);
	  break;
    //case SPI_DEV_CALL_POS_CMD_CFG:
    //case SPI_DEV_CALL_POS_DATA_CFG:
    default: //�����뷢�����ݸ�ʽ��ͬ
      CS553xDev_cbCfgSpiDev(pSpiDev);
    break;
  }
}

//------------------------------SpiDevCmd�ж���ɻص�����ʵ��-----------------
static void _cbEndInt(void *pSpiDev) //struct _SpiDev *pSpiDev)
{
  CS553xDev_cbFinal(CS553xDev_cbGet(pSpiDev));
};

//-------------------------------��ʼ������---------------------------------
//���ô˺���ǰȷ����ӦƬѡ��������Ϊ����ߵ�ƽ״̬
void CS553xDev_Init(struct _CS553xDev *pDev,
                  struct _SpiDev *pSpiDev,   //�ҽ��豸
                  unsigned char Cs)     //Ƭѡλ��
{
  memset(pDev, 0, sizeof(struct _CS553xDev));
  pDev->pSpiDev = pSpiDev;
  //��ʼ������:
  pDev->Cmd.pCmd = pDev->CmdBuf;
  //pDev->Cmd.pData = NULL;  //���ݻ������ڶ�д�������ṩ
  pDev->Cmd.CmdSize = 1;    //�̶�Ϊ1
  //pDev->Cmd.DataSize = 0;  //���ݴ�С�ͻ������ڶ�д�������ṩ
  pDev->Cmd.Cs = Cs;
  //pDev->Cmd.Cfg = 0;       //�������ڶ�д�������ṩ
  pDev->Cmd.cbCfg = _cbCfg;
  pDev->Cmd.cbEndInt = _cbEndInt;
  //��ʼ������:

}

//-----------------------------��д������������---------------------------------
//�˺���������SPIͨѶ,���ѯ��ӦSPI״̬���SPIͨѶ�Ƿ����
//����0:�ɹ� -1:SPI�豸��ռ��δ�ͷ�
signed char CS553xDev_DataStart(struct _CS553xDev *pDev,    //�豸
                            unsigned char Cmd,         //�����CS553xDev������
                            void *pData,              //���շ�������
                            unsigned char Size)        //�շ����ݴ�С
{
  pDev->CmdBuf[0] = Cmd;
  pDev->Cmd.pData = pData;
  pDev->Cmd.DataSize = Size;

  /*/ȷ����д��־,ʹ�ð�˫��SPI����:
  //����ģʽ(���λ��λ),����ͨģʽд(��λδ��λ)�Ҳ�Ϊ0(0Ϊ���ݶ�)ʱ��Ϊд״̬
  if((Cmd >= 0x80) || (!(Cmd & CC553x_RW_RD) && (Cmd)))
    pDev->Cmd.Cfg = SPI_DEV_CMD_DIS_RX; //д����ʱ����������
  else
    pDev->Cmd.Cfg = SPI_DEV_CMD_DIS_TX; //������ʱ����������
  */

  pDev->Cmd.Cfg = SPI_DEV_CMD_EN_CMD_RCV;//ʹ��ȫ˫������,���ڷ�������ͬʱ��������

  if(SpiDev_Restart(pDev->pSpiDev, &pDev->Cmd)) pDev->ErrCount++; //����SPI
  else return 0;

  return -1;
}

//-----------------------------��д���ݺ���---------------------------------
//�˺�����������ʽ��д����,����ͨѶ�����
//����0:�ɹ� -1:SPI�豸��ռ��δ�ͷ� -2;ͨѶ����, -3:ͨѶ��ʱ
signed char CS553xDev_Data(struct _CS553xDev *pDev,    //�豸
                       unsigned char Cmd,         //�����CS553xDev������
                       void *pData,              //���շ�������
                       unsigned char Size)        //�շ����ݴ�С
{
  struct _SpiDev *pSpiDev;
  signed char ErrFlag;
  if(CS553xDev_DataStart(pDev, Cmd, pData, Size)) return  -1; //�Ѽ���
  pSpiDev = pDev->pSpiDev;
  while(pSpiDev->Flag & SPI_DEV_BUSY);//�ȴ����
  if(pSpiDev->Flag & SPI_DEV_ERR) ErrFlag = -2;
  else if(pSpiDev->Flag & SPI_DEV_OV_ERR) ErrFlag = -3;
  else ErrFlag = 0;

  if(ErrFlag) pDev->ErrCount++;
  return ErrFlag;
}

//-----------------------------��д����������������---------------------------------
//�˺�����������ʽ��д����,����ͨѶ�����
//����0:�ɹ� -1:SPI�豸��ռ��δ�ͷ�
//��д���ݺ��������ͨ��S553xDev_GetULong���
signed char CS553xDev_UlongStart(struct _CS553xDev *pDev,    //�豸
                                 unsigned char Cmd,         //�����CS553xDev������
                                 unsigned long Data)       //��д������
{
  pDev->DataBuf[0] = (unsigned char)(Data >> 24);
  pDev->DataBuf[1] = (unsigned char)(Data >> 16);
  pDev->DataBuf[1] = (unsigned char)(Data >> 8);
  pDev->DataBuf[1] = (unsigned char)(Data >> 0);
  return CS553xDev_DataStart(pDev, Cmd, pDev->DataBuf, 4);
}

//-----------------------------��д�������ݺ���---------------------------------
//�˺���������SPIͨѶ,���ѯ��ӦSPI״̬���SPIͨѶ�Ƿ����
//����0:�ɹ� -1:SPI�豸��ռ��δ�ͷ�
//��д���ݺ�ķ���ֵ��ͨ��S553xDev_GetULong���
signed char CS553xDev_Ulong(struct _CS553xDev *pDev,    //�豸
                            unsigned char Cmd,         //�����CS553xDev������
                            unsigned long Data)       //��д������
{
  struct _SpiDev *pSpiDev;
  signed char ErrFlag;
  pDev->DataBuf[0] = (unsigned char)(Data >> 24);
  pDev->DataBuf[1] = (unsigned char)(Data >> 16);
  pDev->DataBuf[2] = (unsigned char)(Data >> 8);
  pDev->DataBuf[3] = (unsigned char)(Data >> 0);
  if(CS553xDev_DataStart(pDev, Cmd, pDev->DataBuf, 4)) return -1;//�Ѽ���
  pSpiDev = pDev->pSpiDev;

  while(pSpiDev->Flag & SPI_DEV_BUSY);//�ȴ����
  if(pSpiDev->Flag & SPI_DEV_ERR) ErrFlag = -2;
  else if(pSpiDev->Flag & SPI_DEV_OV_ERR) ErrFlag = -3;
  else ErrFlag = 0;

  if(ErrFlag) pDev->ErrCount++;
  return ErrFlag;
}

//-----------------------------�õ���д��ĳ�������---------------------------------
//�˺���������CS553xDev_UlongStart() CS553xDev_Ulong()���SPIͨѶ��,�õ�����ֵ
unsigned long S553xDev_GetULong(const struct _CS553xDev *pDev)
{
  unsigned long Data;
  Data = (unsigned long)pDev->DataBuf[0] << 24;
  Data |= (unsigned long)pDev->DataBuf[1] << 16;
  Data |= (unsigned long)pDev->DataBuf[2] << 8;
  Data |= (unsigned long)pDev->DataBuf[3] << 0;
  return Data;
}

//---------------------------д�����---------------------------------
//�˺���������SPIͨѶ,���ѯ��ӦSPI״̬���SPIͨѶ�Ƿ����
//����0:�ɹ� -1:SPI�豸��ռ��δ�ͷ� -2;ͨѶ����, -3:ͨѶ��ʱ
signed char CS553xDev_WrCmd(struct _CS553xDev *pDev,    //�豸
                            unsigned char Cmd)         //�����CS553xDev������
{

  struct _SpiDev *pSpiDev;
  signed char ErrFlag;
  CS553xDev_DataStart(pDev, Cmd, NULL, 0);

  pSpiDev = pDev->pSpiDev;
  while(pSpiDev->Flag & SPI_DEV_BUSY);//�ȴ����
  if(pSpiDev->Flag & SPI_DEV_ERR) ErrFlag = -2;
  else if(pSpiDev->Flag & SPI_DEV_OV_ERR) ErrFlag = -3;
  else ErrFlag = 0;

  if(ErrFlag) pDev->ErrCount++;
  return ErrFlag;
}



