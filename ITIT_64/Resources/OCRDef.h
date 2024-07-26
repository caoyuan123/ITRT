#pragma once

#include <string>
#include <map>
#include <cxcore.hpp>

#include "CBi_TableInterface.h"

#include "CBITableDef.h"



class DrawingZone;
class OCR_ResultOrlWord;
class QColor;
class FontPicture;

class IfOCRPoint
{
public:
	virtual std::string getWord( ) const = 0;       //��ȡ��
	virtual double getPossible( )const =0;         //��ȡ������
	virtual cv::Point getZonePoint( )const =0;     //���������ڵ�����
	virtual cv::Point getSheetPoint( )const =0;    //����Sheetҳ������

	virtual cv::Rect getZoneRect( )const;
	virtual cv::Rect getSheetRect( )const;
};

//ɸѡ����ÿ�����Ӧ�ĸ���,�������������ԭʼ��
class ptPsb
{
public:
	cv::Point Zpoint;  //��������
	double possible;		//�����ԭʼ�Ŀ�����
	double MultiPossible; //�����������װ������,����ÿ���ֵĿ����Ե� ����ƽ������
	bool checked;  //�Ƿ����

public:
	ptPsb();

	virtual double setMultiPossbile(const cv::Mat &MainMat,const FontPicture &fp);

	virtual double getStdPossible( ) const;
	virtual double getMultiPossible( ) const;

	static bool compare(const ptPsb& A,const ptPsb& B);

public:
	void read(const cv::FileNode &node);
	void write(cv::FileStorage &fs) const;

};

extern void read(const cv::FileNode& node, ptPsb& x,const ptPsb& default_value = ptPsb());
extern void write(cv::FileStorage& fs, const std::string& name, const ptPsb& x);

//��OCRWordList�еĽ��ͼ��ȡ���ĵ���ŵ����ݽṹ
struct OCRPoint : public ptPsb 
	/*, public IfOCRPoint*/ 
{
	friend struct OCRAlternativePoint;

public:
	std::string word; //����
	bool isInclude;  //�Ƿ���������ѡ��Ŀ�������Լ�
	
	double adjPossible;

protected:
	
	IfDictionary::Field _DicFiled;    //������Ǵ������ֵ��������͵�-�źŻ�,����

public:
	DrawingZone* _OCRZone; //������zone
	OCR_ResultOrlWord* _OCRWord; //��Ӧ��ԭʼͼ
	FontPicture* getFontPicture();

	

public:
	OCRPoint();
	OCRPoint(const ptPsb &_ptPsb,OCR_ResultOrlWord *Word = nullptr);
	virtual std::string getWord( )const ;        //��ȡ��
	virtual double getStdPossible( )const ;         //��ȡ������
	virtual double getWordPossible( )const; 

	virtual void setPossible(double psb ); //���ý��
	virtual void setInclude(bool bl);  //�Ƿ񱻰���

	virtual cv::Point getZonePoint( )const ;     //���������ڵ�����
	virtual cv::Point getSheetPoint( )const ;    //����Sheetҳ������
	cv::Rect getRect( )const;


public:
	void read(const cv::FileNode &node);
	void write(cv::FileStorage &fs) const;

};

extern void read(const cv::FileNode& node, OCRPoint& x,const OCRPoint& default_value = OCRPoint());
extern void write(cv::FileStorage& fs, const std::string& name, const OCRPoint& x);

//��ѡ��Ŀ�ĵ�
struct OCRAlternativePoint : public IfOCRPoint 
{
	friend struct OCRLine;
	friend class OCREngineCv;


	enum AlterType
	{
		unKnow = 0,
		HumanCheck = 1,
		Singal = 2, //��ѡ��ƥ��ֻ��һ��
		SingalLow = 3, //ѡ��ֻ��һ������ ƥ��ȵ��ڱ�׼ 75%
		MultiplePeek = 4,  //��ѡ������һ���Ƕ��ߵ�
		MultipleMixed = 5      //��ѡ����1-2�ĸ��ʲ��

	};

protected:
	int zoneLineNum ; //�к�-OCRLine������洢��
	cv::Point Spoint;  //ѡȡ��
	std::vector<OCRPoint> Alternative;             //��ѡ�б�

	int maxStr; //����ַ����ĸ���

public:
	DrawingZone* m_Zone;                        //������OCRZone

	bool isChecked;

public:
	OCRAlternativePoint(DrawingZone* Zone = nullptr);
	OCRAlternativePoint(const OCRAlternativePoint& copy);
	OCRAlternativePoint(const OCRPoint& copy);

public:
	void addOcrPoint(const OCRPoint& input);  //��ӵ�
	void addOutSideOcrPoint(const OCRPoint& input);  //��ӱ��Լ���ĵ�
	void addOcrPointAuto(const OCRPoint &input);  //��ӵ���Զ��ж� �������
	void newWithPoint(const OCRPoint& input);

//	OCRPoint operator[](int index)const;
	OCRPoint getBest( )const;                  //��ȡ���п��ܵĵ�
	std::string toString()const;       

	void sort( );

	void setAllInclude(int size );  //�����˰���

	AlterType getType( ); //Ⱦɫ����
	cv::Scalar getColorCv( );
	QColor getColorQt( );

public:
	virtual std::string getWord( )const;
	virtual double getPossible( )const;
	virtual cv::Point getZonePoint( )const;  //Ĭ�Ϸ�����������
	virtual cv::Point getSheetPoint( )const;
	

	virtual bool isSpliter( )const;  //��һЩ����Ҫ�ָ����ŵ��������ʹ�õ�,��Ҫ����������

public:
	static bool compareX( OCRAlternativePoint *P1, OCRAlternativePoint *P2);
	

public:
	void read(const cv::FileNode &node);
	void write(cv::FileStorage &fs) const;

};

extern void read(const cv::FileNode& node, OCRAlternativePoint& x,const OCRAlternativePoint& default_value = OCRAlternativePoint(nullptr));
extern void write(cv::FileStorage& fs, const std::string& name, const OCRAlternativePoint& x);

//�ձȶԽ������ԭʼ���(���)
class OCR_ResultOrlWord 
{
public:

	FontPicture* _FontPicture;      //��Ӧ���ֵ����ֵ�ָ��
	DrawingZone* m_Parent;   //������

public:
	std::string getWord();     //ȡ����������
	cv::Mat Mat_32f1c;              //���ͼ32bit

public:
	double getYuzhi();  //ȡ��OCRset�е���ֵ
	std::vector<ptPsb> PointList;   //ɸѡ������
	cv::Size getSize();

public:
	OCR_ResultOrlWord(FontPicture* word,DrawingZone* _parent);
	
};

//OCR��ԭʼ�������,��������ݽṹ��Vector��ʽ
struct OCROrlResults  
{
	std::vector<OCR_ResultOrlWord*> m_OrlResults;  //ԭʼ���
	//	std::map<std::string,OCR_ResultOrlWord*> map_OrlResults;  //���Լ�����ԭʼ���

	void add(OCR_ResultOrlWord* Add_word);  //���
	unsigned int size();

	OCR_ResultOrlWord* at(int num);           //��ż���
	//	OCR_ResultOrlWord* at(std::string str);   //������Ƽ���

	OCR_ResultOrlWord* operator[](int _Pos);  

	void clear();
	~OCROrlResults(); 
};

//ʶ������,��ʶ����������ʹ��
struct OCRLine
{
public:
	DrawingZone *_OCRZone;  	 //��Ӧ��OCRZoneָ��

	int y;                   //������,zone����
	std::vector<OCRAlternativePoint*> Line;   //ֻ���ò��洢

	int LineNum;
public:
	std::string toString(std::string spliter = ",");  //����һ������ת��Ϊ�������,Ĭ�ϴ��ָ�����Ϊ","

	OCRLine(OCRAlternativePoint *AddPoint);
	~OCRLine();

	void addAltPoint(OCRAlternativePoint *AddPoint);

	int getSheetPosY();

	static bool compare( OCRLine *L1, OCRLine *L2);
};






