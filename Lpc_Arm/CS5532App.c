/******************************************************************************

                CS5532Ӧ�����ʵ��
//��Ҫʵ����RCM-6E��Ӧ�ó���ӿ�
******************************************************************************/

#include "CS5532App.h"
#include "Spi.h"
#include <string.h>
#include "LPC12xxBit.h"

#include "Delay.h" //DelayMs()

extern void vAdDataProRecall(unsigned long AD);

struct _CS5532App CS5532App;  //ֱ��ʵ����

//-------------------------------��ʼ������-------------------------------
void CS5532App_Init(void)
{
  memset(&CS5532App, 0, sizeof(struct _CS5532App));

  CS5532App_cbCsInit();//Ƭѡ��
  CS553xDev_cbMisoIntCfg();//�����ж�

  //ע:LPC_CPOL LPC_CPLA����Ϊ0
  //�趨ʱ��=(SYSCLK/ SSPCLKDIV /(CPSR �� [SCR+1])
  //ʱ������Ϊ200K,36Mʱ����
  LPC_SYSCON->SSPCLKDIV = 4;
  LPC_SSP->CR0 = LPC_FRF_SPI | (44 << LPC_SCR_SHIFT) | (8 - 1);
  LPC_SSP->CPSR = 2; //����Ϊż��

  //����ʼ���豸
  CS553xDev_Init(&CS5532App.CS5532Dev, &SpiDev, 0);
  if(CS5532App_CS5532Init()) //�����豸
    CS5532App_CS5532Init(); //���ɹ�������һ��
}
//-------------------------------оƬ��ʼ������-----------------------------
//����ֵ����Ϊ: 0xff:оƬ��λ����,����:ͨѶ�ڼ���Ϸ�������
unsigned char CS5532App_CS5532Init(void)
{
  S553xDev_ClrErrCount(&CS5532App.CS5532Dev);  //�����������λ

  //ǿ��ֹͣ���е���ת��??
  CS553xDev_cbMisoIntDis();//�رղ�ѯװ������ж�
  S553xDev_ClrFlagMc(&CS5532App.CS5532Dev); //��ת����־

  //д��λ����
  memset(CS5532App.DataBuf, 0xff,14);
  CS5532App.DataBuf[14] = 0xfe;
  CS553xDev_Data(&CS5532App.CS5532Dev,    //�豸
                CC553x_RESET,         //�����CS553xDev������
                CS5532App.DataBuf,              //���շ�������
                15);        //�շ����ݴ�С
  //�Ĵ�����λ
  CS553xDev_Ulong(&CS5532App.CS5532Dev,    //�豸
                  CC553x_CFG | CC553x_CS_0 | CC553x_RSB_CFG, //�����CS553xDev������
                  CC553x_CFG_RST);              //���շ�������
  //���Ĵ�����λ��ɱ�־
  CS553xDev_Ulong(&CS5532App.CS5532Dev,    //�豸
                  CC553x_CFG | CC553x_RW_RD | CC553x_CS_0 | CC553x_RSB_CFG, //�����CS553xDev������
                  CC553x_CFG_RST);              //���շ�������

  if(!(S553xDev_GetULong(&CS5532App.CS5532Dev) & CC553x_CFG_RV)) return 0xff;//оƬ��λ����


  //���üĴ����������: �ο���ѹѡ1V-2.5V,����Ĭ��
  CS553xDev_Ulong(&CS5532App.CS5532Dev,    //�豸
                  CC553x_CFG | CC553x_CS_0 | CC553x_RSB_CFG, //�����CS553xDev������
                  CC553x_CFG_VGS );              //���շ�������
  CS553xDev_Ulong(&CS5532App.CS5532Dev,    //������
                  CC553x_CFG | CC553x_RW_RD | CC553x_CS_0 | CC553x_RSB_CFG, //�����CS553xDev������
                  CC553x_CFG_VGS );              //���շ�������

  //ͨ�����üĴ���0,1,2,3����:λ������Ϊ20480����ʱ������
  //�ڴ�Ӧ����,������ͨ��ͬʱ�ɼ�������ⲿ��Ƭѡ,���õĲ���Ϊ:ʹ��ǰ2�����üĴ���
  //���е�1���ɼ����һͨ������,��2���ɼ�ÿ2ͨ������
  unsigned long Data = ((CC553x_CSR_CS_1 | CC553x_CSR_G_1 | CC553x_CSR_WR_20480 | CC553x_CSR_UB_B)
                         << CC553x_CSR_EVEN_SHIRT) |
                        (CC553x_CSR_CS_0 | CC553x_CSR_G_1 | CC553x_CSR_WR_20480 | CC553x_CSR_UB_B);
  CS553xDev_Ulong(&CS5532App.CS5532Dev,    //�豸
                  CC553x_CFG | CC553x_CS_0 | CC553x_RSB_CSR, //�����CS553xDev������
                  Data);              //���շ�������
  CS553xDev_Ulong(&CS5532App.CS5532Dev,    //������
                  CC553x_CFG | CC553x_RW_RD | CC553x_CS_0 | CC553x_RSB_CSR, //�����CS553xDev������
                  Data);              //���շ�������

  //CS553xDev_Ulong(&CS5532App.CS5532Dev,    //�豸
  //                CC553x_CFG | CC553x_CS_1 | CC553x_RSB_CSR, //�����CS553xDev������
  //                Data);              //���շ�������

  /*/��У׼:���������:
  CS553xDev_Ulong(&CS5532App.CS5532Dev,    //�豸
                  CC553x_CMD | CC553x_CSRP_0 | CC553x_CC_GAIN_ADJ_OWN, //�����CS553xDev������
                  0);              //���շ�������
  DelayMs(100);
  CS553xDev_Ulong(&CS5532App.CS5532Dev,    //�豸
                  CC553x_CMD | CC553x_CSRP_0 | CC553x_CC_OFF_ADJ_OWN, //�����CS553xDev������
                  0);              //���շ�������
  DelayMs(100);*/

  return S553xDev_GetErrCount(&CS5532App.CS5532Dev);
}

//-----------------------------CS553xDevSpi�豸ͨѶ��ɻص�����-----------------
void CS553xDev_cbFinal(struct _CS553xDev *pDev)
{
  //3�������ص�����״̬��Ϊ���õȴ�״̬ʱ
  if(CS5532App.eState == eCS5532App_State_CfgWait){
    CS553xDev_cbMisoIntEn(); //�����ж�
    CS5532App.eState = eCS5532App_State_DataWait;//�ȴ�����
  }

  if(CS5532App.eState != eCS5532App_State_Done) return;

  //=============5�������ص�����״̬��Ϊת�����״̬ʱ=============
  S553xDev_ClrFlagMc(&CS5532App.CS5532Dev); //��ת����־,�����ڽ���
  CS553xDev_cbSetCs();//Ƭѡȡ��
   //ת����ɺ��ý��������ٴν���ת��
   unsigned long Data = S553xDev_GetULong(&CS5532App.CS5532Dev);
   if(Data & CC553x_DATA_OF){//���������
     if(CS5532App.Timer >2) //������������һ��ֵ�ָ�����
	   CS5532App.Timer--;
   }
   else CS5532App.Timer = CS5532APP_TIMER_COUNT; //����������������λ!


   CS5532App_cbFinal(CS5532App.Ch, Data); //�õ�ת�����

   //��ǰͨ�����������,����һͨ��
   if(CS5532App.Ch) CS5532App.Ch = 0;
   else  CS5532App.Ch = 1;
   //��CSҪ����ʱ,�ʷ��ڴ�
   S553xDev_SetFlagMc(&CS5532App.CS5532Dev); //����ת����־
   if(CS5532App.Ch)
     CS553xDev_WrCmdStart(&CS5532App.CS5532Dev,    //�豸
                       CC553x_CMD | CC553x_CSRP_1 | CC553x_CC_NOR); //��������ͨ��0����ת��
   else
     CS553xDev_WrCmdStart(&CS5532App.CS5532Dev,    //�豸
                       CC553x_CMD | CC553x_CSRP_0 | CC553x_CC_NOR); //��������ͨ��1����ת��

   CS5532App.eState = eCS5532App_State_CfgWait;
}

//-----------------------------CS553xDev MISO�����жϺ���ʵ��------------------
//���˺�������MISO�����жϳ�����
void CS553xDev_MisoInt(void)
{
  if(CS5532App.eState != eCS5532App_State_DataWait) return;
  //4�������жϻص�����״̬��Ϊ���ݵȴ�״̬ʱ

  if(CS5532App.Ch) CS5532App_cbSwitchAdSel();//�л���һӲ��ͨ��׼��

  CS553xDev_cbMisoIntDis();   //�ر��ж�
  //AD��ɣ�׼��������
  CS553xDev_UlongStart(&CS5532App.CS5532Dev,    //�豸
                       CC553x_NULL, //��ȡ����ת�����
                       0);              //���շ�������

  CS5532App.eState = eCS5532App_State_Done;
}

//-----------------------------������-----------------------------
//����һ��CS5532��д���ڼ�һ������ADת�������ܹ�������ɵ�ϵͳ������
void CS5532App_Task(void)
{
  SpiDev_Task(&SpiDev);//SPI����,��ռ
  if(CS5532App.Timer){
     CS5532App.Timer--;
     if(!CS5532App.Timer){//������,�ָ�
       SpiDev_Stop(&SpiDev);
       CS5532App_Init();
       CS5532App_CS5532Init();
     }
   }
  if(CS5532App.eState != eCS5532App_State_Idie) return;

  //2.�����ѯ������ʱ
  S553xDev_SetFlagMc(&CS5532App.CS5532Dev); //��ת����־
  CS5532App.Ch = 0;
  CS553xDev_WrCmdStart(&CS5532App.CS5532Dev,    //�豸
                       CC553x_CMD | CC553x_CSRP_0 | CC553x_CC_NOR); //��������ͨ��0����ת��
  CS5532App.eState = eCS5532App_State_CfgWait;
  CS5532App.Timer = CS5532APP_TIMER_COUNT; //��ʼ�������
}