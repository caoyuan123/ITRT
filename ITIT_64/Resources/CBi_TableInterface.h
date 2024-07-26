#pragma once


#define DictionaryFiledMax 13   //�ֵ����ͷ����

//����������ʾ
const std::string FieldChinese [DictionaryFiledMax] = {"������ť����","�г��źŻ�����","�����źŻ�����","�г���ť����", 
	"������ť����","��������","��������","����ʾ��ʽ","��������","��������","��������","�ж��ź�","�ж��źŵ���"};

//Ӣ�����ڴ洢�ǵ�Xml/Yaml�ڵ�����
const std::string FieldEnglish [DictionaryFiledMax] = {"IdentyButton","TrainSignal","ShuntingSignal","TrainButton", 
	"ShuntingButton","Turnout","Track","SignalType","OtherLocking","ProtectFollowSwitch","OverrunSwitch","OppositeSignal","OppositeSignalSwitch"};



class IfDictionary
{
public:
	enum Field
	{
		None = -1,
		�������� = 0,       
		�г��źŻ����� = 1,
		�����źŻ����� = 2,
		�г���ť���� = 3,
		������ť���� = 4,
		�������� = 5,
		��������  = 6,		
		����ʾ��ʽ = 7,
		��������  = 8,

		�������� = 9,  //������һЩ,�ڽ����ϵĴ洢λ��ֻ��ָ��{}
		�������� = 10, 
		�ж��ź� = 11,  //�ж��ź��Լ�������
		�ж��źŵ��� = 12, //�ж��ź��Լ�����������

	};

public:
	//virtual void slot_GenerateButtom( ) = 0;//���г��͵����źŻ��������ɰ�ť����
	virtual void slot_readDev(std::string configPath) = 0; //��DevInfo�ж�ȡ
	virtual std::vector<std::string> getDistorage(int num ) = 0; //��ȡ�洢�ı� < 0��ʾȫ��
};





