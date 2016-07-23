/******************************************************************************

                CS5532Ӧ�����ʵ��
//��Ҫʵ����SW-24��Ӧ�ó���ӿ�
******************************************************************************/

#include "CS5532App.h"
#include "Spi.h"
//#include <avr/IO.h>  //AVRGCC����ʱ
#include <string.h>

#include "Delay.h" //DelayMs()
#include "Int0.h"  //����ʹ���ж�

extern void vAdDataProRecall(unsigned long AD);

struct _CS5532App CS5532App;  //ֱ��ʵ����

//-------------------------------��ʼ������-------------------------------
void CS5532App_Init(void)
{
  memset(&CS5532App, 0, sizeof(struct _CS5532App));
  //�ж�ϵͳ��ʼ��������
	Int0_Init(INT0_MODE_RISE); //�ȳ�ʼ������Ϊ�½���ģʽ
  //ͨѶ��ʼ��
  SetSPI_nCS5531();     //ȡ��Ƭѡ
  SPCR |= (1<<SPR1); //����ʱ�����ʹ̶�Ϊfck/16
  CS553xDev_Init(&CS5532App.CS5532Dev, &SpiDev, 0);
}
//-------------------------------оƬ��ʼ������-----------------------------
//����ֵ����Ϊ: 0xff:оƬ��λ����,����:ͨѶ�ڼ���Ϸ�������
unsigned char CS5532App_CS5532Init(void)
{
  S553xDev_ClrErrCount(&CS5532App.CS5532Dev);  //�����������λ
  DelayMs(20);

  //ǿ��ֹͣ���е���ת��??
  Int0_Dis();//�رղ�ѯװ������ж�
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
  unsigned long Data = ((CC553x_CSR_CS_0 | CC553x_CSR_G_1 | CC553x_CSR_WR_20480 | CC553x_CSR_UB_B)
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

//-----------------------------CS553xDevSpi�豸ͨѶ��ɻص�����-------------------------
void CS553xDev_cbFinal(struct _CS553xDev *pDev)
{
  if(CS5532App.eState != eCS5532App_State_Done) return;
   //ת����ɺ��ý��������ٴν���ת��
   unsigned char Data = S553xDev_GetULong(&CS5532App.CS5532Dev);
   if(Data & CC553x_DATA_OF){//���������
     if(CS5532App.Timer >2) //������������һ��ֵ�ָ�����
	   CS5532App.Timer--;

   }
   else CS5532App.Timer = CS5532APP_TIMER_COUNT; //����������������λ!

   CS5532App_cbFinal(S553xDev_GetULong(&CS5532App.CS5532Dev)); //�õ�ת�����
   Int0_En(); //�ٴο���Int0�жϵȴ��ж�
  CS5532App.eState = eCS5532App_State_Wait;
}

//-----------------------------CS553xDevMISO�����жϺ���ʵ��-------------------------
void CS553xDev_Int(void)
{
  if(CS5532App.eState != eCS5532App_State_Wait) return;
  //Int0_Dis();//�رղ�ѯװ������ж�
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
  if(CS5532App.Timer){
     CS5532App.Timer--;
	 if(!CS5532App.Timer){//������,�ָ�
	   CS5532App_Init();
       CS5532App_CS5532Init();
     }
   }
  if(CS5532App.eState != eCS5532App_State_Idie) return;
  S553xDev_SetFlagMc(&CS5532App.CS5532Dev); //��ת����־
  CS553xDev_WrCmd(&CS5532App.CS5532Dev,    //�豸
                       CC553x_CMD | CC553x_MC | CC553x_CSRP_0 | CC553x_CC_NOR); //��������ͨ��0����ת��
  CS5532App.eState = eCS5532App_State_Wait;


  Int0_CfgMode(INT0_MODE_CHANGE);//��ƽ�仯�����ж�
  Int0_En(); //����Int�ж�
  CS5532App.Timer = CS5532APP_TIMER_COUNT; //��ʼ�������
}

