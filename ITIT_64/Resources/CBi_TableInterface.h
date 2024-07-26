#pragma once


#define DictionaryFiledMax 13   //字典最大头数量

//中文用于显示
const std::string FieldChinese [DictionaryFiledMax] = {"独立按钮名称","列车信号机名称","调车信号机名称","列车按钮名称", 
	"调车按钮名称","道岔名称","区段名称","灯显示样式","其他联锁","带防道岔","超限条件","敌对信号","敌对信号道岔"};

//英文用于存储是的Xml/Yaml节点名称
const std::string FieldEnglish [DictionaryFiledMax] = {"IdentyButton","TrainSignal","ShuntingSignal","TrainButton", 
	"ShuntingButton","Turnout","Track","SignalType","OtherLocking","ProtectFollowSwitch","OverrunSwitch","OppositeSignal","OppositeSignalSwitch"};



class IfDictionary
{
public:
	enum Field
	{
		None = -1,
		数字序列 = 0,       
		列车信号机名称 = 1,
		调车信号机名称 = 2,
		列车按钮名称 = 3,
		调车按钮名称 = 4,
		道岔名称 = 5,
		区段名称  = 6,		
		灯显示样式 = 7,
		其他联锁  = 8,

		带防道岔 = 9,  //新增的一些,在界面上的存储位置只用指定{}
		条件道岔 = 10, 
		敌对信号 = 11,  //敌对信号自己的条件
		敌对信号道岔 = 12, //敌对信号自己的条件道岔

	};

public:
	//virtual void slot_GenerateButtom( ) = 0;//由列车和调车信号机名称生成按钮名称
	virtual void slot_readDev(std::string configPath) = 0; //从DevInfo中读取
	virtual std::vector<std::string> getDistorage(int num ) = 0; //获取存储的表 < 0表示全读
};





