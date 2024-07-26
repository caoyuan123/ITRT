#pragma once
#include "StdAfx.h"
#include <string>
#include <vector>
#include <QString>
#include <cxcore.h>


#define FrontSize 7   //  ��ǰ�������
#define StraitSize  8  //���ֱ���������
#define SideSize    9  //�������������
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
//	byte    m_byNameScreen;//����������Ļ
	std::string m_sDevName;       //���豸����
	std::string m_sLogicalName;   //�豸�߼�����
	
	WORD    m_wDevId;         //�豸����

	CPoint  m_ptOrigin;       //��ʼ���������
	CPoint  m_ptNamePoint;    //�����������

//	CRect   m_rcDevRect;      //�豸����
//	CRect   m_rcNameRect;     //�豸���Ƶ�����

	bool    m_bVisible;       //�豸��ʾ����

	unsigned char  m_szDevStyle;  //�豸��������
	int  m_iDevType;              //�豸��������

  //��һ���ж����Լ����豸��Ϣ
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
	CPoint        m_ptStraitFirst[2];//��λ��1,2��
	CPoint        m_ptSideFirst[2];//��λ��1,2��
	CPoint        m_ptFrontFirst[2];//��ǰ��1,2��

	int            m_iArrayNum;  //�������ڷ�·����ͬ����Ϣ�����е����

	int            m_iFront[20];//��ǰ�������Ա�־
	int            m_iStrait[20];//ֱ�ɸ������Ա�־
	int            m_iSide[20];//��ɸ������Ա�־

	unsigned short m_nDirection;            //������   

	CPoint         m_ptFront[FrontSize] ;   //��ǰ�����                              
	CPoint         m_ptStrait[StraitSize];  //���ֱ�������                          
	CPoint         m_ptSide[SideSize];      //�����������  

	bool           m_bStraitBeamed;         //���ֱ���Ƿ�Ϊ��λ                      
	WORD           m_wSectionNum;           //�����������α�� 

	WORD           m_wControlSwitch1;       //���˿��Ƶ������                        
	WORD           m_wControlSwitch2;       //Զ�˿��Ƶ������  
	bool           m_bDirect1;              //���˿��Ƶ����Ƿ�Ϊ��λ����              
	bool           m_bDirect2;              //Զ�˿��Ƶ����Ƿ�Ϊ��λ����   

	CRect          m_rcBtnRect;             //��ť����                                                    
	CRect          m_rcNameRefresh;         //����ˢ������ 

	bool           m_bNoLeftClick;      //������
	byte           m_byTGQFlag;             //�ѹ�����־��0-�޿�ͨλ,����ʾ,1-��λ�ѹ�λ��2-��λ�ѹ�λ
public:
	CSwitchDev()
	{
		m_bNoLeftClick = false;
	}

};

class CBtnDev : public CDevice 
{
public: 
//	int  m_iBtnOfDev;      //��ť�����豸����  - �������źŻ�
//	int  m_iBtnClass;      //��ť���ͷ���  - ��ť����

	byte m_btSingalClass;  //�źŰ�ť���� bit1 ʹ��1/0�ر�; bit2 ��1/��2; bit3 ��1/��0;bit4 ������ ��1/��0 bit5 �߼�Dev����һ��1/0

//	bool           m_bDelayBtn;         //�Ƿ�����ʱ��ť
//	bool           m_bFeiZiFu;          //�Ƿ��Ƿ��Ը���ť

	

	int m_iBtnNum;

public:

};

class CSignalDev : public CDevice  
{
public:
	bool       m_bLeadSeal;// �Ƿ���Ǧ�ⰴť

	bool       m_bDBtnHave;        //�Ƿ��е�����ť
	WORD       m_wDBtnId;          //������Ŧ����
	WORD       m_wDBtnNum;         //������ť���

	bool       m_bLBtnHave;        //�Ƿ����г���ť
	WORD       m_wLBtnId;          //�г���Ŧ����               
	WORD       m_wLBtnNum;         //�г���ť���

	bool        m_bDwarfSignal;     //�Ƿ񻭰����źŻ� 

	short      m_nRotateAngle;     //����Ƕ�

	bool       m_bShuntRed;      // �����źŻ��ر��Ƿ�����ɫ,��Ϊ��ɫ
public:


};

class CTrackDev : public CDevice  
{
public:
//	bool           m_bShowTwoName;//�Ƿ���ʾ�������� 
//	CPoint         m_ptNamePoint1;//��2�����������

	int            m_iArrayNum;  //�ɵ�/�޲�������dev�е��Ŷ����
//	int            m_iShowHide[20];//�ɵ�/�޲�����ĳһ���Ƿ���ʾ���Ա�־
//	bool           m_bTingCheQi;//�Ƿ���ͣ����

	bool           m_bIsNoCross;  //�Ƿ����޲����� true - wuchaGudao false -����
//	CRect          m_rcBtnRect;   //��Ӧ��ť��Ļ����   
//	CRect          m_rcNameRect; //��������
//	CRect          m_rcBtnRect1;   //��Ӧ��ť��Ļ����   
//	CRect          m_rcNameRect1; //��������

//	CPoint         m_ptPointArray[PointNum]; //�ɵ��������� 
//	int            m_nPointNum;    //�ɵ�����

	int            m_nColorType;//�ɵ���������ɫ����

	bool           m_bDelayFree;// �Ƿ� ��Ҫ��ʱ����
	bool           m_bInputTrainNum;//�Ƿ�Ҫ�ֹ����복�κ�


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
	QString m_strRoot;    //�����ļ���ȡ���ַ���
	QString m_ErrorLine;  //������Ϣ

	int	  g_nStr ;  //ÿ���������ַ��������ά��
	int	  g_nDigit ; //ÿ�����������ݴ������ά��

	unsigned char m_szTrackStyle;  //�޲��������� 1-�޲� 2-�ɵ� 3-����
	unsigned char m_szStyle;

	QString g_Path;

	int trackArrayNum;
	int SwitchArrayNum;
	int ButtonArrayNum;

	std::vector<CTrackDev*> _AllTrack; //���й������
	std::vector<CSignalDev*> _AllSingal; //���й������
	std::vector<CSwitchDev*> _AllSwitch; //���й������
	std::vector<CBtnDev*> _AllButton; //���а�ť

protected:
	bool DealLine(const QString &str_line,std::vector<int> &PInt,std::vector<QString> &PStr,QString &sErrorLine);
	bool readSingal(const QString &str_line,CSignalDev* newSingal); //���ź�
	bool readTrack(const QString &str_line,CTrackDev* newTrack);  //������
	bool readSwitch(const QString &str_line,CSwitchDev* newSwitch);  //������
	bool readButton(const QString &str_line,CBtnDev* newBtn);
	bool readALL(const QString &str_line);

public:
	kxConfigReader();
	~kxConfigReader();
	//�������ַ�������ȡ�������ݣ������ַ������������������
	//cstr������������
	//Pint����ȡ����
	//PStr:��ȡ�ַ���
	//bool Deal_LineStr(std::string &cstr, int *PInt, std::string *PStr);

	void ClearMark(QString &str);


	bool GetMapAttribute();   //�������л�ȡ����վ����Ϣ
};



