/******************************************************************************

               CS553x�豸��������ӿ�
CS553x�豸������һ��SpiDev�豸֮�ϣ�ͨ����ͬƬѡ���ṩ���CS553xоƬ֧��
����оƬ��ʹ����ͬͨѶ����
******************************************************************************/

#ifndef __CS553x_DEV_H
#define __CS553x_DEV_H

/******************************************************************************
                             CS553x����ģʽ�ֶ���
******************************************************************************/
//CS553x��Spi��һ��������+��������ģʽ��
//�����������λ��������ģʽ������ģʽ�����嶨��Ϊ:
//���λ��Ϊ0Ϊ����ģʽ,��ģʽ�����"CS553x����ģʽ�����"
#define CC553x_CFG   0x00

//���λ��Ϊ1Ϊ����ģʽ(��ת����У׼����),��ģʽ�����"CS553x����ģʽ�����"
#define CC553x_CMD   0x80

//�ر��,������15��0xffʹоƬ��λʱ,��һ��0xff��Ϊ����ʹ�ã����14������0xff��һ��0xfe:
//CC553x��Ӳ����λ�������ϵ�20ms�����ø�λ���и�λоƬ����ʹ�ø�λָ��RS��λ���Ĵ���
#define CC553x_RESET  0xff

//�崮�ڱ�־��ʹת��������������ת��ģʽ
#define CC553x_NULL  0x00
//�崮�ڱ�־��ʹת�����˳�����ת��ģʽ
#define CC553x_FULL  0xFF
//---------------------------CS553x����ģʽ�����-------------------------------
//���з�ʽ����0-3�ɼ�ͨ��ָ���Ĵ�����־,��λ��λʱ,�ɼ�ͨ��ѡ��λӦ��0
#define CC553x_ARA 0x40

//�ɼ�ͨ��ѡ��,0~3;
#define CC553x_CS_SHIFT  4
#define CC553x_CS_MASK   ((unsigned char)3 << CC553x_CS_SHIFT)  //����,ֱ�Ӷ���ʱΪ:
#define CC553x_CS_0      ((unsigned char)0 << CC553x_CS_SHIFT)  //�ɼ�ͨ��0
#define CC553x_CS_1      ((unsigned char)1 << CC553x_CS_SHIFT)  //�ɼ�ͨ��1
#define CC553x_CS_2      ((unsigned char)2 << CC553x_CS_SHIFT)  //�ɼ�ͨ��2
#define CC553x_CS_3      ((unsigned char)3 << CC553x_CS_SHIFT)  //�ɼ�ͨ��3

//����־,����Ϊд
#define CC553x_RW_RD  0x08

//���3bit�����˸��ɼ�ͨ���ڲ��Ĵ���λ��(δ����ʱΪ����):
#define CC553x_RSB_OFFSET      1  //ƫ�ƼĴ���,(Q24����Ĭ��Ϊ0)ÿͨ��1��
#define CC553x_RSB_GAIN        2  //����Ĵ���,(Q24����0-32,Ĭ��Ϊ1)ÿͨ��1��
#define CC553x_RSB_CFG         3  //(ϵͳ)���üĴ���������ͨ������1��
#define CC553x_RSB_DATA        4  //ת�����ݼĴ���(��ת�����ADֵ,���вɼ�ͨ������1��,ֻ��)
#define CC553x_RSB_CSR         5  //ͨ�����üĴ���,��8��ÿ��16bit,�������Ϊһ�������������

//---------------------------CS553x����ģʽ�����-------------------------------
//ִ������ת����־,����ִ����ȫ�ȶ��ĵ���ת��
#define CC553x_MC          0x40

//ͨ�����üĴ���ѡ��λ, ��8��0~7
#define CC553x_CSRP_SHIFT  3
#define CC553x_CSRP_MASK   ((unsigned char)7 << CC553x_CS_SHIFT)  //����,ֱ�Ӷ���ʱΪ:
#define CC553x_CSRP_0      ((unsigned char)0 << CC553x_CS_SHIFT)  //ͨ�����üĴ���0
#define CC553x_CSRP_1      ((unsigned char)1 << CC553x_CS_SHIFT)  //ͨ�����üĴ���1
#define CC553x_CSRP_2      ((unsigned char)2 << CC553x_CS_SHIFT)  //ͨ�����üĴ���2
#define CC553x_CSRP_3      ((unsigned char)3 << CC553x_CS_SHIFT)  //ͨ�����üĴ���3
#define CC553x_CSRP_4      ((unsigned char)4 << CC553x_CS_SHIFT)  //ͨ�����üĴ���4
#define CC553x_CSRP_5      ((unsigned char)5 << CC553x_CS_SHIFT)  //ͨ�����üĴ���5
#define CC553x_CSRP_6      ((unsigned char)6 << CC553x_CS_SHIFT)  //ͨ�����üĴ���6
#define CC553x_CSRP_7      ((unsigned char)7 << CC553x_CS_SHIFT)  //ͨ�����üĴ���7

//���3bit�����˶�ָ��ͨ���Ĵ������õ���ز���(ת��/У׼λ��δ����ʱΪ����):
#define CC553x_CC_NOR             0  //(��ָ��ͨ�����üĴ���������)ִ������ת��
#define CC553x_CC_OFF_ADJ_OWN     1  //(��ָ��ͨ�����üĴ���������)ִ����ƫ��У׼
#define CC553x_CC_GAIN_ADJ_OWN    2  //(��ָ��ͨ�����üĴ���������)ִ��������У׼
#define CC553x_CC_OFF_ADJ_SYS     5  //(��ָ��ͨ�����üĴ���������)ִ��ϵͳƫ��У׼
#define CC553x_CC_GAIN_ADJ_SYS    6  //(��ָ��ͨ�����üĴ���������)ִ��ϵͳ����У׼

//����ת��ģʽʱ��˵��:
//1.����ʱ���ú�ʹ�õ�CSRͨ��
//2.�ø�CSRͨ��ִ��ת��,�����Զ���������ģʽ
//3.ת�����ΪSDO��Ϊ�ͣ�ʹ��CC553x_NULLָ��(SDO������Ϊ�߲�)��ת���Ĵ�������
//4.�Զ���������ģʽ
//ע��
//1.����ת������ʹ��CC553x_NULLָ���ȡ���ݲ��ܽ������к�̲���(��SDO���ָ����ָܻ�Ϊ��)
//2.����ת��ʱ,оƬ�ڲ�����5��ת���Լ�Sinc5�˲�����ȡ���һ��ת�����

//����ת��ģʽʱ��˵��:
//1.����ʱ���ú�ʹ�õ�CSRͨ��
//2.�ø�CSRͨ��ִ��ת��,�����Զ���������ģʽ
//3.ת�����ΪSDO��Ϊ�ͣ�ʹ��CC553x_NULLָ��(SDO������Ϊ�߲�)��ת���Ĵ������ݲ��Ե�ǰת��ģʽ
//����ת��,ʹ��CC553x_FULLָ���ת���Ĵ������ݲ��˳�����ת��ģʽ���Զ���������ģʽ
//ע��
//1.����ÿ��ʹ��CC553x_NULL����ȡת����,�ڲ�ת����������ת��
//2.����ת��ģʽ�������ǰ3�λ�5��ת�����


//---------------------------ͨ�����üĴ���λ����-------------------------------
//ͨ�����üĴ���Ϊ16Bit,�������Ϊһ���Ĵ������в���
//�ӼĴ�������ȡ������ض���:
#define CC553x_CSR_EVEN_SHIRT	16	 //�����żͨ�����üĴ�������λֵ
#define CC553x_CSR_ODD_MASK  	0xffff	 //�������ͨ�����üĴ���������
//�Ĵ���λ����Ϊ:
//�ɼ�ͨ��ѡ��,0~3;
#define CC553x_CSR_CS_SHIFT  14
#define CC553x_CSR_CS_MASK   ((unsigned long)3 << CC553x_CSR_CS_SHIFT)  //����,ֱ�Ӷ���ʱΪ:
#define CC553x_CSR_CS_0      ((unsigned long)0 << CC553x_CSR_CS_SHIFT)  //�ɼ�ͨ��0
#define CC553x_CSR_CS_1      ((unsigned long)1 << CC553x_CSR_CS_SHIFT)  //�ɼ�ͨ��1
#define CC553x_CSR_CS_2      ((unsigned long)2 << CC553x_CSR_CS_SHIFT)  //�ɼ�ͨ��2
#define CC553x_CSR_CS_3      ((unsigned long)3 << CC553x_CSR_CS_SHIFT)  //�ɼ�ͨ��3
//����ѡ��;
#define CC553x_CSR_G_SHIFT   11
#define CC553x_CSR_G_MASK    ((unsigned long)7 << CC553x_CSR_G_SHIFT)  //����,ֱ�Ӷ���ʱΪ:
#define CC553x_CSR_G_1      ((unsigned long)0 << CC553x_CSR_G_SHIFT)  //����Ϊ1
#define CC553x_CSR_G_2      ((unsigned long)1 << CC553x_CSR_G_SHIFT)  //����Ϊ2
#define CC553x_CSR_G_4      ((unsigned long)2 << CC553x_CSR_G_SHIFT)  //����Ϊ4
#define CC553x_CSR_G_8      ((unsigned long)3 << CC553x_CSR_G_SHIFT)  //����Ϊ8
#define CC553x_CSR_G_16      ((unsigned long)4 << CC553x_CSR_G_SHIFT)  //����Ϊ16
#define CC553x_CSR_G_32      ((unsigned long)5 << CC553x_CSR_G_SHIFT)  //����Ϊ32
#define CC553x_CSR_G_64      ((unsigned long)6 << CC553x_CSR_G_SHIFT)  //����Ϊ64
//λ����:
#define CC553x_CSR_WR_SHIFT   7
#define CC553x_CSR_WR_MASK     ((unsigned long)f << CC553x_CSR_WR_SHIFT)  //����,ֱ�Ӷ���ʱΪ:
#define CC553x_CSR_WR_40960    ((unsigned long)0 << CC553x_CSR_WR_SHIFT)  //40960����ʱ������
#define CC553x_CSR_WR_81920    ((unsigned long)1 << CC553x_CSR_WR_SHIFT)  //81920����ʱ������
#define CC553x_CSR_WR_163840   ((unsigned long)2 << CC553x_CSR_WR_SHIFT)  //163840����ʱ������
#define CC553x_CSR_WR_327680   ((unsigned long)3 << CC553x_CSR_WR_SHIFT)  //327680����ʱ������
#define CC553x_CSR_WR_655360   ((unsigned long)4 << CC553x_CSR_WR_SHIFT)  //655360����ʱ������
#define CC553x_CSR_WR_1280     ((unsigned long)5 << CC553x_CSR_WR_SHIFT)  //1280����ʱ������
#define CC553x_CSR_WR_2560     ((unsigned long)6 << CC553x_CSR_WR_SHIFT)  //2560����ʱ������
#define CC553x_CSR_WR_5120     ((unsigned long)7 << CC553x_CSR_WR_SHIFT)  //5120����ʱ������
#define CC553x_CSR_WR_10240    ((unsigned long)9 << CC553x_CSR_WR_SHIFT)  //10240����ʱ������
#define CC553x_CSR_WR_20480    ((unsigned long)10 << CC553x_CSR_WR_SHIFT) //20480����ʱ������
//��/˫����:
#define CC553x_CSR_UB_U         ((unsigned long)1 << 6)  //˫����ģʽ
#define CC553x_CSR_UB_B         ((unsigned long)0 << 6)  //������ģʽ
//�������λ,��OLS��λʱ,����A0,A1�߼�״̬:
#define CC553x_CSR_OL_A1        ((unsigned long)1 << 5)   //A1�ĵ�ƽ:0��1��
#define CC553x_CSR_OL_A0        ((unsigned long)1 << 4)   //A0�ĵ�ƽ:0��1��
//ʱ��λ:
#define CC553x_CSR_DT          ((unsigned long)1 << 3)  //ʱ������,��������
//��·���λ:
#define CC553x_CSR_OCD          ((unsigned long)1 << 2)  //��·�������,����Դ����,����Ϊ����ģʽ
//δ���岿�ֱ���

//---------------------------���üĴ���λ����-------------------------------
#define CC553x_CFG_PSS  ((unsigned long)1 << 31)  //�ڵ�ģʽѡ��:0����,1����
#define CC553x_CFG_PDW  ((unsigned long)1 << 30)  //�ڵ�ģʽ:0����,1�ڵ�
#define CC553x_CFG_RST  ((unsigned long)1 << 29)  //ϵͳ��λ:0����,1��λ����
#define CC553x_CFG_RV   ((unsigned long)1 << 28)   //��λָʾ:0����,1�Ѹ�λ,�����ú�����0
#define CC553x_CFG_IS   ((unsigned long)1 << 27)   //�����·:0����,1��ͨ���ڲ��̽�
#define CC553x_CFG_GB   ((unsigned long)1 << 26)   //�����ź�:0�������,1��ģ��ѹ
#define CC553x_CFG_VGS  ((unsigned long)1 << 25)   //��ѹ�ο�ѡ��:0:2.5~VA+ 1:1V~2.5V
#define CC553x_CFG_OL_A1 ((unsigned long)1 << 24)   //A1�ĵ�ƽ:0��1��
#define CC553x_CFG_OL_A0 ((unsigned long)1 << 23)   //A0�ĵ�ƽ:0��1��
#define CC553x_CFG_OLS  ((unsigned long)1 << 22)   //A0A1���Դѡ��:0�͵�ƽʱA0A1ͨ�����üĴ�������
//δʹ�õ�����λ������0

//--------------------------ת���Ĵ���λ����-------------------------------
//AD����λ,��Զ�����λ
#define CC553x_DATA_AD16_SHIFT  ((unsigned long)1 << 16)  //16λADоƬʱ��AD������λֵ
#define CC553x_DATA_AD24_SHIFT  ((unsigned long)1 << 8)  //24λADоƬʱ��AD������λֵ
#define CC553x_DATA_OF          ((unsigned long)1 << 2)  //���������־,�����ݳ�����
//���2λָʾ���ɼ�������ͨ��:
#define CC553x_DATA_CI_SHIFT  0
#define CC553x_DATA_CI_MASK   ((unsigned long)3 << CC553x_DATA_CI_SHIFT)  //����,ֱ�Ӷ���ʱΪ:
#define CC553x_DATA_CI_0      ((unsigned long)0 << CC553x_DATA_CI_SHIFT)  //�ɼ�ͨ��0
#define CC553x_DATA_CI_1      ((unsigned long)1 << CC553x_DATA_CI_SHIFT)  //�ɼ�ͨ��1
#define CC553x_DATA_CI_2      ((unsigned long)2 << CC553x_DATA_CI_SHIFT)  //�ɼ�ͨ��2
#define CC553x_DATA_CI_3      ((unsigned long)3 << CC553x_DATA_CI_SHIFT)  //�ɼ�ͨ��3


/******************************************************************************
                             ��ؽṹ
******************************************************************************/
#include"SpiDev.h"    //Spi�豸����

struct _CS553xDev{
  //�ײ��豸���:
  struct _SpiDevCmd Cmd;  //�豸������
  struct _SpiDev *pSpiDev;   //�ҽ��豸
  unsigned char ErrCount;   //���ϼ���
  unsigned char Flag;		//��ر�־��������
  unsigned char CmdBuf[1];   //�豸�����ֵ��������
  unsigned char DataBuf[4];  //���ڶ�д�����������ڲ�ʹ��
};

//���У���ر�־����Ϊ:
#define CS553X_DEV_FLAG_MC	 0x01//����ת����־MC��λ��־

/******************************************************************************
                             ��غ���
******************************************************************************/

//-------------------------------��ʼ������---------------------------------
//���ô˺���ǰȷ����ӦƬѡ��������Ϊ����ߵ�ƽ״̬
void CS553xDev_Init(struct _CS553xDev *pDev,
                  struct _SpiDev *pSpiDev,   //�ҽ��豸
                   unsigned char Cs);     //Ƭѡλ��

//-----------------------------��д������������---------------------------------
//�˺���������SPIͨѶ,���ѯ��ӦSPI״̬���SPIͨѶ�Ƿ����
//����0:�ɹ� -1:SPI�豸��ռ��δ�ͷ�
signed char CS553xDev_DataStart(struct _CS553xDev *pDev,    //�豸
                            unsigned char Cmd,         //�����CS553xDev������
                            void *pData,              //���շ�������
                            unsigned char Size);        //�շ����ݴ�С

//-----------------------------����д�����Ƿ��������---------------------------
//��д���������ɹ�����ô˺������SpiͨѶ�Ƿ����
//void CS553xDev_IsEnd(struct _CS553xDev *pDev);
#define CS553xDev_IsEnd(pDev) \
  (!((pDev)->pSpiDev->Flag & SPI_DEV_BUSY))

//-----------------------------��д���ݺ���---------------------------------
//�˺�����������ʽ��д����,����ͨѶ�����
//����0:�ɹ� -1:SPI�豸��ռ��δ�ͷ� -2;ͨѶ����, -3:ͨѶ��ʱ
signed char CS553xDev_Data(struct _CS553xDev *pDev,    //�豸
                       unsigned char Cmd,         //�����CS553xDev������
                       void *pData,              //���շ�������
                       unsigned char Size);        //�շ����ݴ�С

//-----------------------------��д����������������---------------------------------
//�˺�����������ʽ��д����,����ͨѶ�����
//����0:�ɹ� -1:SPI�豸��ռ��δ�ͷ�
//��д���ݺ��������ͨ��S553xDev_GetULong���
signed char CS553xDev_UlongStart(struct _CS553xDev *pDev,    //�豸
                                 unsigned char Cmd,         //�����CS553xDev������
                                 unsigned long Data);       //��д������


//-----------------------------��д�������ݺ���---------------------------------
//�˺���������SPIͨѶ,���ѯ��ӦSPI״̬���SPIͨѶ�Ƿ����
//����0:�ɹ� -1:SPI�豸��ռ��δ�ͷ� -2;ͨѶ����, -3:ͨѶ��ʱ
//��д���ݺ�ķ���ֵ��ͨ��S553xDev_GetULong���
signed char CS553xDev_Ulong(struct _CS553xDev *pDev,    //�豸
                            unsigned char Cmd,         //�����CS553xDev������
                            unsigned long Data);       //��д������

//-----------------------------�õ���д��ĳ�������---------------------------------
//�˺���������CS553xDev_UlongStart() CS553xDev_Ulong()���SPIͨѶ��,�õ�����ֵ
unsigned long S553xDev_GetULong(const struct _CS553xDev *pDev);

//-----------------------------�õ����ص������ֺ���---------------------------------
//�˺������������SPIͨѶ��,������дʱ���ص�����
//unsigned char S553xDev_GetReturnCmd(const struct _CS553xDev *pDev);
#define S553xDev_GetReturnCmd(pDev)  ((pDev)->CmdBuf[0])

//-----------------------------д������������---------------------------------
//�˺�������������ͨѶ
//void CS553xDev_WrCmdStart(struct _CS553xDev *pDev,    //�豸
//                                 unsigned char Cmd);         //�����CS553xDev������
//ֱ��ʵ��Ϊ
#define CS553xDev_WrCmdStart(pDev, Cmd) \
  do{CS553xDev_DataStart(pDev, Cmd, NULL, 0);}while(0)

//---------------------------д�����---------------------------------
//�˺���������SPIͨѶ,���ѯ��ӦSPI״̬���SPIͨѶ�Ƿ����
//����0:�ɹ� -1:SPI�豸��ռ��δ�ͷ� -2;ͨѶ����, -3:ͨѶ��ʱ
signed char CS553xDev_WrCmd(struct _CS553xDev *pDev,    //�豸
                            unsigned char Cmd);         //�����CS553xDev������


//-----------------------------������ϼ���������---------------------------------
#define S553xDev_ClrErrCount(pDev)  do{(pDev)->ErrCount = 0;}while(0)

//-----------------------------�ù��ϼ���ֵ����---------------------------------
//�˺������ڶ�ȡ���ϴι��ϼ���������������������Ĵ���
#define S553xDev_GetErrCount(pDev)  ((pDev)->ErrCount)

//----------------------------������ת����־---------------------------------
//�ô˱�־��CS�������ͷ�ֱ��ȡ���ñ�־���ٴζ���
#define S553xDev_SetFlagMc(pDev)  \
    do{(pDev)->Flag |= CS553X_DEV_FLAG_MC;}while(0)

//----------------------------������ת����־---------------------------------
//�ô˱�־��CS������һ��SPIͨѶ������ɺ��ͷ�
#define S553xDev_ClrFlagMc(pDev)  \
    do{(pDev)->Flag &= ~CS553X_DEV_FLAG_MC;}while(0)

//----------------------------״̬�Ƿ�������ת��ģʽ---------------------------------
//�ô˱�־��CS������һ��SPIͨѶ������ɺ��ͷ�
#define S553xDev_IsFlagMc(pDev)  ((pDev)->Flag & CS553X_DEV_FLAG_MC)



/******************************************************************************
                             ��ػص�����
******************************************************************************/
#include "Cs553xDev_CB.h"  //��˲���Ӧ�����,�ʵ����ļ�����ʵ��




#endif //#define __CS553x_DEV_H









