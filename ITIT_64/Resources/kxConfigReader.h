#pragma once
#include "StdAfx.h"
#include <string>
#include <vector>
#include <QString>
#include <cxcore.h>


#define FrontSize 7   //  岔前坐标点数
#define StraitSize  8  //岔后直股坐标点数
#define SideSize    9  //岔后弯股坐标点数
#define SingalButtonCode 12
#define PointNum  8

class CPoint : public cv::Point
{

};

class CRect : public cv::Rect
{

};

class CDevice
{
public:
//	byte    m_byNameScreen;//名称所属屏幕
	std::string m_sDevName;       //该设备名称
	std::string m_sLogicalName;   //设备逻辑名称
	
	WORD    m_wDevId;         //设备代码

	CPoint  m_ptOrigin;       //起始点绝对坐标
	CPoint  m_ptNamePoint;    //名称相对坐标

//	CRect   m_rcDevRect;      //设备区域
//	CRect   m_rcNameRect;     //设备名称的区域

	bool    m_bVisible;       //设备显示属性

	unsigned char  m_szDevStyle;  //设备归属类型
	int  m_iDevType;              //设备包含类型

  //从一行中读入自己的设备信息
	CDevice()
	{
		m_sDevName = "";
		m_sLogicalName = "";

		m_wDevId = 0x0000;
		m_bVisible = false;
		m_szDevStyle = 0;

		m_iDevType = -1;

	}
};

class CSwitchDev : public CDevice
{
public:
	CPoint        m_ptStraitFirst[2];//定位第1,2点
	CPoint        m_ptSideFirst[2];//反位第1,2点
	CPoint        m_ptFrontFirst[2];//岔前第1,2点

	int            m_iArrayNum;  //本道岔在分路不良同步信息数组中的序号

	int            m_iFront[20];//岔前各点属性标志
	int            m_iStrait[20];//直股各点属性标志
	int            m_iSide[20];//弯股各点属性标志

	unsigned short m_nDirection;            //道岔开向   

	CPoint         m_ptFront[FrontSize] ;   //岔前坐标点                              
	CPoint         m_ptStrait[StraitSize];  //岔后直股坐标点                          
	CPoint         m_ptSide[SideSize];      //岔后弯股坐标点  

	bool           m_bStraitBeamed;         //岔后直股是否为定位                      
	WORD           m_wSectionNum;           //道岔所属区段编号 

	WORD           m_wControlSwitch1;       //近端控制道岔序号                        
	WORD           m_wControlSwitch2;       //远端控制道岔序号  
	bool           m_bDirect1;              //近端控制道岔是否为定位控制              
	bool           m_bDirect2;              //远端控制道岔是否为定位控制   

	CRect          m_rcBtnRect;             //按钮区域                                                    
	CRect          m_rcNameRefresh;         //名称刷新区域 

	bool           m_bNoLeftClick;      //左键点击
	byte           m_byTGQFlag;             //脱轨器标志，0-无开通位,不显示,1-定位脱轨位，2-反位脱轨位
public:
	CSwitchDev()
	{
		m_bNoLeftClick = false;
	}

};

class CBtnDev : public CDevice 
{
public: 
//	int  m_iBtnOfDev;      //按钮所属设备类型  - 道岔还是信号机
//	int  m_iBtnClass;      //按钮类型分类  - 按钮代码

	byte m_btSingalClass;  //信号按钮特征 bit1 使能1/0关闭; bit2 列1/调2; bit3 从1/独0;bit4 从属于 列1/调0 bit5 逻辑Dev名称一致1/0

//	bool           m_bDelayBtn;         //是否是延时按钮
//	bool           m_bFeiZiFu;          //是否是非自复按钮

	

	int m_iBtnNum;

public:

};

class CSignalDev : public CDevice  
{
public:
	bool       m_bLeadSeal;// 是否是铅封按钮

	bool       m_bDBtnHave;        //是否有调车按钮
	WORD       m_wDBtnId;          //调车按纽代码
	WORD       m_wDBtnNum;         //调车按钮序号

	bool       m_bLBtnHave;        //是否有列车按钮
	WORD       m_wLBtnId;          //列车按纽代码               
	WORD       m_wLBtnNum;         //列车按钮序号

	bool        m_bDwarfSignal;     //是否画矮柱信号机 

	short      m_nRotateAngle;     //开向角度

	bool       m_bShuntRed;      // 调车信号机关闭是否是蓝色,否为红色
public:


};

class CTrackDev : public CDevice  
{
public:
//	bool           m_bShowTwoName;//是否显示两个名称 
//	CPoint         m_ptNamePoint1;//第2个名称坐标点

	int            m_iArrayNum;  //股道/无岔区段在dev中的排队序号
//	int            m_iShowHide[20];//股道/无岔区段某一段是否显示属性标志
//	bool           m_bTingCheQi;//是否有停车器

	bool           m_bIsNoCross;  //是否是无岔区段 true - wuchaGudao false -道岔
//	CRect          m_rcBtnRect;   //对应按钮屏幕区域   
//	CRect          m_rcNameRect; //名称区域
//	CRect          m_rcBtnRect1;   //对应按钮屏幕区域   
//	CRect          m_rcNameRect1; //名称区域

//	CPoint         m_ptPointArray[PointNum]; //股道点数数组 
//	int            m_nPointNum;    //股道点数

	int            m_nColorType;//股道，区段颜色类型

	bool           m_bDelayFree;// 是否 需要延时出清
	bool           m_bInputTrainNum;//是否要手工输入车次号


	CTrackDev()
	{
		m_iArrayNum = -1;
		m_bDelayFree = false;
		m_bInputTrainNum = false;
		m_nColorType = 0;
	}
	

public:

};

class kxConfigReader
{
public:
	QString m_strRoot;    //配置文件读取的字符串
	QString m_ErrorLine;  //错误信息

	int	  g_nStr ;  //每行数据中字符串数组的维数
	int	  g_nDigit ; //每行数据中数据串数组的维数

	unsigned char m_szTrackStyle;  //无岔区段类型 1-无岔 2-股道 3-道岔
	unsigned char m_szStyle;

	QString g_Path;

	int trackArrayNum;
	int SwitchArrayNum;
	int ButtonArrayNum;

	std::vector<CTrackDev*> _AllTrack; //所有轨道区段
	std::vector<CSignalDev*> _AllSingal; //所有轨道区段
	std::vector<CSwitchDev*> _AllSwitch; //所有轨道区段
	std::vector<CBtnDev*> _AllButton; //所有按钮

protected:
	bool DealLine(const QString &str_line,std::vector<int> &PInt,std::vector<QString> &PStr,QString &sErrorLine);
	bool readSingal(const QString &str_line,CSignalDev* newSingal); //读信号
	bool readTrack(const QString &str_line,CTrackDev* newTrack);  //读区段
	bool readSwitch(const QString &str_line,CSwitchDev* newSwitch);  //读道岔
	bool readButton(const QString &str_line,CBtnDev* newBtn);
	bool readALL(const QString &str_line);

public:
	kxConfigReader();
	~kxConfigReader();
	//分析行字符串，获取配置数据，放入字符串数组和数字数组中
	//cstr：传入行数据
	//Pint：读取数字
	//PStr:读取字符串
	//bool Deal_LineStr(std::string &cstr, int *PInt, std::string *PStr);

	void ClearMark(QString &str);


	bool GetMapAttribute();   //从配置中获取所有站场信息
};



