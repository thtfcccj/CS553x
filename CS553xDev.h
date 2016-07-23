/******************************************************************************

               CS553x设备驱动程序接口
CS553x设备建立在一个SpiDev设备之上，通过不同片选可提供多个CS553x芯片支持
所有芯片均使用相同通讯配置
******************************************************************************/

#ifndef __CS553x_DEV_H
#define __CS553x_DEV_H

/******************************************************************************
                             CS553x命令模式字定义
******************************************************************************/
//CS553x的Spi有一个命令字+若干数据模式，
//命令字以最高位区分命令模式与配置模式，具体定义为:
//最高位置为0为配置模式,该模式定义见"CS553x配置模式命令定义"
#define CC553x_CFG   0x00

//最高位置为1为命令模式(即转换或校准控制),该模式定义见"CS553x命令模式命令定义"
#define CC553x_CMD   0x80

//特别地,当发送15个0xff使芯片复位时,第一个0xff作为命令使用，后跟14个数据0xff和一个0xfe:
//CC553x无硬件复位，需在上电20ms后命用复位序列复位芯片，并使用复位指令RS复位各寄存器
#define CC553x_RESET  0xff

//清串口标志并使转换器保持在连续转换模式
#define CC553x_NULL  0x00
//清串口标志并使转换器退出连续转换模式
#define CC553x_FULL  0xFF
//---------------------------CS553x配置模式命令定义-------------------------------
//阵列方式访问0-3采集通道指定寄存器标志,此位置位时,采集通道选择位应置0
#define CC553x_ARA 0x40

//采集通道选择,0~3;
#define CC553x_CS_SHIFT  4
#define CC553x_CS_MASK   ((unsigned char)3 << CC553x_CS_SHIFT)  //掩码,直接定义时为:
#define CC553x_CS_0      ((unsigned char)0 << CC553x_CS_SHIFT)  //采集通道0
#define CC553x_CS_1      ((unsigned char)1 << CC553x_CS_SHIFT)  //采集通道1
#define CC553x_CS_2      ((unsigned char)2 << CC553x_CS_SHIFT)  //采集通道2
#define CC553x_CS_3      ((unsigned char)3 << CC553x_CS_SHIFT)  //采集通道3

//读标志,否则为写
#define CC553x_RW_RD  0x08

//最低3bit定义了各采集通道内部寄存器位置(未定义时为保留):
#define CC553x_RSB_OFFSET      1  //偏移寄存器,(Q24数，默认为0)每通道1个
#define CC553x_RSB_GAIN        2  //增益寄存器,(Q24数，0-32,默认为1)每通道1个
#define CC553x_RSB_CFG         3  //(系统)配置寄存器，所有通道共用1个
#define CC553x_RSB_DATA        4  //转换数据寄存器(即转换后的AD值,所有采集通道共用1个,只读)
#define CC553x_RSB_CSR         5  //通道设置寄存器,共8个每个16bit,两两组合为一组必须连续配置

//---------------------------CS553x命令模式命令定义-------------------------------
//执行连续转换标志,否则执行完全稳定的单次转换
#define CC553x_MC          0x40

//通道设置寄存器选择位, 共8个0~7
#define CC553x_CSRP_SHIFT  3
#define CC553x_CSRP_MASK   ((unsigned char)7 << CC553x_CS_SHIFT)  //掩码,直接定义时为:
#define CC553x_CSRP_0      ((unsigned char)0 << CC553x_CS_SHIFT)  //通道设置寄存器0
#define CC553x_CSRP_1      ((unsigned char)1 << CC553x_CS_SHIFT)  //通道设置寄存器1
#define CC553x_CSRP_2      ((unsigned char)2 << CC553x_CS_SHIFT)  //通道设置寄存器2
#define CC553x_CSRP_3      ((unsigned char)3 << CC553x_CS_SHIFT)  //通道设置寄存器3
#define CC553x_CSRP_4      ((unsigned char)4 << CC553x_CS_SHIFT)  //通道设置寄存器4
#define CC553x_CSRP_5      ((unsigned char)5 << CC553x_CS_SHIFT)  //通道设置寄存器5
#define CC553x_CSRP_6      ((unsigned char)6 << CC553x_CS_SHIFT)  //通道设置寄存器6
#define CC553x_CSRP_7      ((unsigned char)7 << CC553x_CS_SHIFT)  //通道设置寄存器7

//最低3bit定义了对指定通道寄存器配置的相关操作(转换/校准位等未定义时为保留):
#define CC553x_CC_NOR             0  //(对指定通道设置寄存器的配置)执行正常转换
#define CC553x_CC_OFF_ADJ_OWN     1  //(对指定通道设置寄存器的配置)执行自偏移校准
#define CC553x_CC_GAIN_ADJ_OWN    2  //(对指定通道设置寄存器的配置)执行自增益校准
#define CC553x_CC_OFF_ADJ_SYS     5  //(对指定通道设置寄存器的配置)执行系统偏移校准
#define CC553x_CC_GAIN_ADJ_SYS    6  //(对指定通道设置寄存器的配置)执行系统增益校准

//单次转换模式时序说明:
//1.开机时配置好使用的CSR通道
//2.用该CSR通道执行转换,串口自动进入数据模式
//3.转换完成为SDO置为低，使用CC553x_NULL指令(SDO才能置为高并)读转换寄存器数据
//4.自动返回配置模式
//注：
//1.单次转换必须使用CC553x_NULL指令读取数据才能进行所有后继操作(因SDO需该指令才能恢复为高)
//2.单次转换时,芯片内部进行5次转换以及Sinc5滤波处理，取最后一次转换结果

//连续转换模式时序说明:
//1.开机时配置好使用的CSR通道
//2.用该CSR通道执行转换,串口自动进入数据模式
//3.转换完成为SDO置为低，使用CC553x_NULL指令(SDO才能置为高并)读转换寄存器数据并以当前转换模式
//继续转换,使用CC553x_FULL指令读转换寄存器数据并退出连续转换模式并自动返回配置模式
//注：
//1.不必每次使用CC553x_NULL来读取转换字,内部转将动周期性转换
//2.连续转换模式必须忽略前3次或5次转换结果


//---------------------------通道设置寄存器位定义-------------------------------
//通道设置寄存器为16Bit,两两组合为一个寄存器进行操作
//从寄存器中提取数据相关定义:
#define CC553x_CSR_EVEN_SHIRT	16	 //定义对偶通道设置寄存器的移位值
#define CC553x_CSR_ODD_MASK  	0xffff	 //定义对奇通道设置寄存器的掩码
//寄存器位定义为:
//采集通道选择,0~3;
#define CC553x_CSR_CS_SHIFT  14
#define CC553x_CSR_CS_MASK   ((unsigned long)3 << CC553x_CSR_CS_SHIFT)  //掩码,直接定义时为:
#define CC553x_CSR_CS_0      ((unsigned long)0 << CC553x_CSR_CS_SHIFT)  //采集通道0
#define CC553x_CSR_CS_1      ((unsigned long)1 << CC553x_CSR_CS_SHIFT)  //采集通道1
#define CC553x_CSR_CS_2      ((unsigned long)2 << CC553x_CSR_CS_SHIFT)  //采集通道2
#define CC553x_CSR_CS_3      ((unsigned long)3 << CC553x_CSR_CS_SHIFT)  //采集通道3
//增益选择;
#define CC553x_CSR_G_SHIFT   11
#define CC553x_CSR_G_MASK    ((unsigned long)7 << CC553x_CSR_G_SHIFT)  //掩码,直接定义时为:
#define CC553x_CSR_G_1      ((unsigned long)0 << CC553x_CSR_G_SHIFT)  //增益为1
#define CC553x_CSR_G_2      ((unsigned long)1 << CC553x_CSR_G_SHIFT)  //增益为2
#define CC553x_CSR_G_4      ((unsigned long)2 << CC553x_CSR_G_SHIFT)  //增益为4
#define CC553x_CSR_G_8      ((unsigned long)3 << CC553x_CSR_G_SHIFT)  //增益为8
#define CC553x_CSR_G_16      ((unsigned long)4 << CC553x_CSR_G_SHIFT)  //增益为16
#define CC553x_CSR_G_32      ((unsigned long)5 << CC553x_CSR_G_SHIFT)  //增益为32
#define CC553x_CSR_G_64      ((unsigned long)6 << CC553x_CSR_G_SHIFT)  //增益为64
//位速率:
#define CC553x_CSR_WR_SHIFT   7
#define CC553x_CSR_WR_MASK     ((unsigned long)f << CC553x_CSR_WR_SHIFT)  //掩码,直接定义时为:
#define CC553x_CSR_WR_40960    ((unsigned long)0 << CC553x_CSR_WR_SHIFT)  //40960个主时钟周期
#define CC553x_CSR_WR_81920    ((unsigned long)1 << CC553x_CSR_WR_SHIFT)  //81920个主时钟周期
#define CC553x_CSR_WR_163840   ((unsigned long)2 << CC553x_CSR_WR_SHIFT)  //163840个主时钟周期
#define CC553x_CSR_WR_327680   ((unsigned long)3 << CC553x_CSR_WR_SHIFT)  //327680个主时钟周期
#define CC553x_CSR_WR_655360   ((unsigned long)4 << CC553x_CSR_WR_SHIFT)  //655360个主时钟周期
#define CC553x_CSR_WR_1280     ((unsigned long)5 << CC553x_CSR_WR_SHIFT)  //1280个主时钟周期
#define CC553x_CSR_WR_2560     ((unsigned long)6 << CC553x_CSR_WR_SHIFT)  //2560个主时钟周期
#define CC553x_CSR_WR_5120     ((unsigned long)7 << CC553x_CSR_WR_SHIFT)  //5120个主时钟周期
#define CC553x_CSR_WR_10240    ((unsigned long)9 << CC553x_CSR_WR_SHIFT)  //10240个主时钟周期
#define CC553x_CSR_WR_20480    ((unsigned long)10 << CC553x_CSR_WR_SHIFT) //20480个主时钟周期
//单/双级性:
#define CC553x_CSR_UB_U         ((unsigned long)1 << 6)  //双极性模式
#define CC553x_CSR_UB_B         ((unsigned long)0 << 6)  //单极性模式
//输出锁存位,当OLS置位时,决定A0,A1逻辑状态:
#define CC553x_CSR_OL_A1        ((unsigned long)1 << 5)   //A1的电平:0低1高
#define CC553x_CSR_OL_A0        ((unsigned long)1 << 4)   //A0的电平:0低1高
//时延位:
#define CC553x_CSR_DT          ((unsigned long)1 << 3)  //时延允许,否则正常
//开路检测位:
#define CC553x_CSR_OCD          ((unsigned long)1 << 2)  //开路检测允许,电流源激活,否则为正常模式
//未定义部分保留

//---------------------------配置寄存器位定义-------------------------------
#define CC553x_CFG_PSS  ((unsigned long)1 << 31)  //节电模式选择:0待机,1休眼
#define CC553x_CFG_PDW  ((unsigned long)1 << 30)  //节电模式:0常归,1节电
#define CC553x_CFG_RST  ((unsigned long)1 << 29)  //系统复位:0正常,1复位操作
#define CC553x_CFG_RV   ((unsigned long)1 << 28)   //复位指示:0正常,1已复位,读配置后清零0
#define CC553x_CFG_IS   ((unsigned long)1 << 27)   //输入短路:0正常,1各通道内部短接
#define CC553x_CFG_GB   ((unsigned long)1 << 26)   //保护信号:0正常输出,1共模电压
#define CC553x_CFG_VGS  ((unsigned long)1 << 25)   //电压参考选择:0:2.5~VA+ 1:1V~2.5V
#define CC553x_CFG_OL_A1 ((unsigned long)1 << 24)   //A1的电平:0低1高
#define CC553x_CFG_OL_A0 ((unsigned long)1 << 23)   //A0的电平:0低1高
#define CC553x_CFG_OLS  ((unsigned long)1 << 22)   //A0A1输出源选择:0低电平时A0A1通道设置寄存器控制
//未使用的其它位必须置0

//--------------------------转换寄存器位定义-------------------------------
//AD数据位,永远在最高位
#define CC553x_DATA_AD16_SHIFT  ((unsigned long)1 << 16)  //16位AD芯片时的AD数据移位值
#define CC553x_DATA_AD24_SHIFT  ((unsigned long)1 << 8)  //24位AD芯片时的AD数据移位值
#define CC553x_DATA_OF          ((unsigned long)1 << 2)  //数据溢出标志,即数据超量程
//最低2位指示被采集的物理通道:
#define CC553x_DATA_CI_SHIFT  0
#define CC553x_DATA_CI_MASK   ((unsigned long)3 << CC553x_DATA_CI_SHIFT)  //掩码,直接定义时为:
#define CC553x_DATA_CI_0      ((unsigned long)0 << CC553x_DATA_CI_SHIFT)  //采集通道0
#define CC553x_DATA_CI_1      ((unsigned long)1 << CC553x_DATA_CI_SHIFT)  //采集通道1
#define CC553x_DATA_CI_2      ((unsigned long)2 << CC553x_DATA_CI_SHIFT)  //采集通道2
#define CC553x_DATA_CI_3      ((unsigned long)3 << CC553x_DATA_CI_SHIFT)  //采集通道3


/******************************************************************************
                             相关结构
******************************************************************************/
#include"SpiDev.h"    //Spi设备定义

struct _CS553xDev{
  //底层设备相关:
  struct _SpiDevCmd Cmd;  //设备命令字
  struct _SpiDev *pSpiDev;   //挂接设备
  unsigned char ErrCount;   //故障计数
  unsigned char Flag;		//相关标志，见定义
  unsigned char CmdBuf[1];   //设备命令字的命令缓冲区
  unsigned char DataBuf[4];  //用于读写长整理数据内部使用
};

//其中，相关标志定义为:
#define CS553X_DEV_FLAG_MC	 0x01//连续转换标志MC置位标志

/******************************************************************************
                             相关函数
******************************************************************************/

//-------------------------------初始化函数---------------------------------
//调用此函数前确保对应片选引脚已置为输出高电平状态
void CS553xDev_Init(struct _CS553xDev *pDev,
                  struct _SpiDev *pSpiDev,   //挂接设备
                   unsigned char Cs);     //片选位置

//-----------------------------读写数据启动函数---------------------------------
//此函数仅启动SPI通讯,需查询对应SPI状态获得SPI通讯是否完成
//返回0:成功 -1:SPI设备被占用未释放
signed char CS553xDev_DataStart(struct _CS553xDev *pDev,    //设备
                            unsigned char Cmd,         //定义的CS553xDev命令字
                            void *pData,              //需收发的数据
                            unsigned char Size);        //收发数据大小

//-----------------------------检查读写数据是否结束函数---------------------------
//读写数据启动成功后调用此函数检查Spi通讯是否结束
//void CS553xDev_IsEnd(struct _CS553xDev *pDev);
#define CS553xDev_IsEnd(pDev) \
  (!((pDev)->pSpiDev->Flag & SPI_DEV_BUSY))

//-----------------------------读写数据函数---------------------------------
//此函数以阻塞方式读写数据,返回通讯即完成
//返回0:成功 -1:SPI设备被占用未释放 -2;通讯错误, -3:通讯超时
signed char CS553xDev_Data(struct _CS553xDev *pDev,    //设备
                       unsigned char Cmd,         //定义的CS553xDev命令字
                       void *pData,              //需收发的数据
                       unsigned char Size);        //收发数据大小

//-----------------------------读写长整数据启动函数---------------------------------
//此函数以阻塞方式读写数据,返回通讯即完成
//返回0:成功 -1:SPI设备被占用未释放
//读写数据后的数据需通过S553xDev_GetULong获得
signed char CS553xDev_UlongStart(struct _CS553xDev *pDev,    //设备
                                 unsigned char Cmd,         //定义的CS553xDev命令字
                                 unsigned long Data);       //被写的数据


//-----------------------------读写长整数据函数---------------------------------
//此函数仅启动SPI通讯,需查询对应SPI状态获得SPI通讯是否完成
//返回0:成功 -1:SPI设备被占用未释放 -2;通讯错误, -3:通讯超时
//读写数据后的返回值需通过S553xDev_GetULong获得
signed char CS553xDev_Ulong(struct _CS553xDev *pDev,    //设备
                            unsigned char Cmd,         //定义的CS553xDev命令字
                            unsigned long Data);       //被写的数据

//-----------------------------得到读写后的长整数据---------------------------------
//此函数用于在CS553xDev_UlongStart() CS553xDev_Ulong()完成SPI通讯后,得到返回值
unsigned long S553xDev_GetULong(const struct _CS553xDev *pDev);

//-----------------------------得到返回的命令字函数---------------------------------
//此函数用于在完成SPI通讯后,命令字写时返回的数据
//unsigned char S553xDev_GetReturnCmd(const struct _CS553xDev *pDev);
#define S553xDev_GetReturnCmd(pDev)  ((pDev)->CmdBuf[0])

//-----------------------------写命令启动函数---------------------------------
//此函数仅启动数据通讯
//void CS553xDev_WrCmdStart(struct _CS553xDev *pDev,    //设备
//                                 unsigned char Cmd);         //定义的CS553xDev命令字
//直接实现为
#define CS553xDev_WrCmdStart(pDev, Cmd) \
  do{CS553xDev_DataStart(pDev, Cmd, NULL, 0);}while(0)

//---------------------------写命令函数---------------------------------
//此函数仅启动SPI通讯,需查询对应SPI状态获得SPI通讯是否完成
//返回0:成功 -1:SPI设备被占用未释放 -2;通讯错误, -3:通讯超时
signed char CS553xDev_WrCmd(struct _CS553xDev *pDev,    //设备
                            unsigned char Cmd);         //定义的CS553xDev命令字


//-----------------------------清除故障计数器函数---------------------------------
#define S553xDev_ClrErrCount(pDev)  do{(pDev)->ErrCount = 0;}while(0)

//-----------------------------得故障计数值函数---------------------------------
//此函数用于读取自上次故障记数器清零以来发生命令的次数
#define S553xDev_GetErrCount(pDev)  ((pDev)->ErrCount)

//----------------------------置连续转换标志---------------------------------
//置此标志后，CS将不再释放直到取消该标志后再次读数
#define S553xDev_SetFlagMc(pDev)  \
    do{(pDev)->Flag |= CS553X_DEV_FLAG_MC;}while(0)

//----------------------------清连续转换标志---------------------------------
//置此标志后，CS将在下一个SPI通讯周期完成后释放
#define S553xDev_ClrFlagMc(pDev)  \
    do{(pDev)->Flag &= ~CS553X_DEV_FLAG_MC;}while(0)

//----------------------------状态是否在连续转换模式---------------------------------
//置此标志后，CS将在下一个SPI通讯周期完成后释放
#define S553xDev_IsFlagMc(pDev)  ((pDev)->Flag & CS553X_DEV_FLAG_MC)



/******************************************************************************
                             相关回调函数
******************************************************************************/
#include "Cs553xDev_CB.h"  //因此部分应用相关,故单独文件调用实现




#endif //#define __CS553x_DEV_H









