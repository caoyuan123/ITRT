#pragma once
#include <string>
#include <vector>
#include <core\core.hpp>

#include "CBi_TableInterface.h"


class DrawingBook;
class DrawingType;
class IfFontOrlPictureGenerater;
class FontGraphicTransTemp;

class FontPicture;



struct CBI_TableDictionary : public IfDictionary
{
	friend class Gui_CBITableDictionary;

private:
	DrawingBook *Ptr_DrawingBook;

	DrawingType *m_DrawingType;  //�趨��ͼֽ����
	
	void numInit( ); //���������ֵ��Ǹ���ʼ��

private:
	std::vector<std::string> Distorage[DictionaryFiledMax];       //�洢�ֿ�Ľṹ��

	volatile bool writeLock;  //д���� trueΪ���� false Ϊ����
	volatile bool m_SyncFontPicture;   //�Ƿ��ֺ�ͼƬ��ͬ���� true - ͬ��
	
	bool shallowCopy;          //�Ƿ�Ϊǳ����

	bool spOverrun;        //�˹�ָ�� ��������
	bool spProtectFollow;  //�˹�ָ�� ������������

	int m_conditionSingleSwitch;  //�Ƿ�Ϊ ������������ 0-���� 1-�� 2-���

	void Clear_FontPicture();  //�������ͼƬ

public:
	std::vector<FontPicture*> _FontPicture[DictionaryFiledMax];    //��ͼƬ�Ľṹ��
	std::vector<FontPicture*> _FontPicture_Similar[DictionaryFiledMax];    //���ڵڶ�����

	CBI_TableDictionary(DrawingBook *_Parent = nullptr);
	CBI_TableDictionary(const CBI_TableDictionary &copy,bool deep = false);

	~CBI_TableDictionary();

public:	
	virtual std::vector<std::string> getDistorage(int num );  //�趨�洢������ ���ΪIfDictionary::Field
	virtual void setDistorage(int num,const std::vector<std::string> &input);

	bool getIgnoreOverrun() const;    //�Ƿ�����������޵���
	void setIgnoreOverrun(bool val);

	bool getIgnoreProtectFollow() const;     //�Ƿ� ���Դ�����������
	void setIgnoreProtectFollow(bool val);

	void setConditionSingleSwitch(int bl);

	void setDrawingType(const DrawingType & dt);  //����ʹ�õ�
	
	DrawingType getDrawingType(); 

	void read(const cv::FileNode &node);    //��xml
	void write(cv::FileStorage &fs) const ; //��xml

	volatile bool isFontSync( ); //����ͼƬ������ô
	bool isShallowCopy( );   //�Ƿ�Ϊǳ����
	bool isPublicDictionary( ); //�Ƿ�Ϊ�����ֵ�

	volatile bool getWriteLock() const;
	void setWriteLock(volatile bool val);

	volatile bool isOcrReady( ); //�Ƿ�׼����OCR��
public: 
	void slot_GenerateFontPicture();  //��������ͼƬ

	void slot_readDev(std::string configPath); //��DevInfo�ж�ȡ
	

public:
	//��̬һ������ͼƬ���溯��
	static void GenerateFontPicture(const std::vector<std::string> &Ipt_Distorage,std::vector<FontPicture*> &Out_FontPicture ,int DicType
		, IfFontOrlPictureGenerater *_Generater ,const FontGraphicTransTemp &_TransTemp);


//	std::string toString(int FieldNum); //��ĳһ�б����ʾģʽ
};





extern void read(const cv::FileNode& node, CBI_TableDictionary& x,const CBI_TableDictionary& default_value = CBI_TableDictionary(nullptr));


extern void write(cv::FileStorage& fs, const std::string& name, const CBI_TableDictionary& x);

