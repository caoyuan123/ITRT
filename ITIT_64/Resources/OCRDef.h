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
	virtual std::string getWord( ) const = 0;       //获取字
	virtual double getPossible( )const =0;         //获取可能性
	virtual cv::Point getZonePoint( )const =0;     //基于区域内的坐标
	virtual cv::Point getSheetPoint( )const =0;    //基于Sheet页的坐标

	virtual cv::Rect getZoneRect( )const;
	virtual cv::Rect getSheetRect( )const;
};

//筛选出的每个点对应的概率,核心引擎出来的原始点
class ptPsb
{
public:
	cv::Point Zpoint;  //区域坐标
	double possible;		//整体的原始的可能性
	double MultiPossible; //字体坐标多联装可能性,是由每个字的可能性的 几何平均构成
	bool checked;  //是否检查过

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

//将OCRWordList中的结果图提取核心点后存放的数据结构
struct OCRPoint : public ptPsb 
	/*, public IfOCRPoint*/ 
{
	friend struct OCRAlternativePoint;

public:
	std::string word; //文字
	bool isInclude;  //是否有其他待选项目包含着自己
	
	double adjPossible;

protected:
	
	IfDictionary::Field _DicFiled;    //这个词是从属于字典那种类型的-信号机,道岔

public:
	DrawingZone* _OCRZone; //从属的zone
	OCR_ResultOrlWord* _OCRWord; //对应的原始图
	FontPicture* getFontPicture();

	

public:
	OCRPoint();
	OCRPoint(const ptPsb &_ptPsb,OCR_ResultOrlWord *Word = nullptr);
	virtual std::string getWord( )const ;        //获取字
	virtual double getStdPossible( )const ;         //获取可能性
	virtual double getWordPossible( )const; 

	virtual void setPossible(double psb ); //设置结果
	virtual void setInclude(bool bl);  //是否被包含

	virtual cv::Point getZonePoint( )const ;     //基于区域内的坐标
	virtual cv::Point getSheetPoint( )const ;    //基于Sheet页的坐标
	cv::Rect getRect( )const;


public:
	void read(const cv::FileNode &node);
	void write(cv::FileStorage &fs) const;

};

extern void read(const cv::FileNode& node, OCRPoint& x,const OCRPoint& default_value = OCRPoint());
extern void write(cv::FileStorage& fs, const std::string& name, const OCRPoint& x);

//备选项目的点
struct OCRAlternativePoint : public IfOCRPoint 
{
	friend struct OCRLine;
	friend class OCREngineCv;


	enum AlterType
	{
		unKnow = 0,
		HumanCheck = 1,
		Singal = 2, //该选项匹配只有一个
		SingalLow = 3, //选项只有一个但是 匹配度低于标准 75%
		MultiplePeek = 4,  //多选但是有一项是独高的
		MultipleMixed = 5      //多选但是1-2的概率差不多

	};

protected:
	int zoneLineNum ; //行号-OCRLine借过来存储的
	cv::Point Spoint;  //选取点
	std::vector<OCRPoint> Alternative;             //备选列表

	int maxStr; //最大字符串的个数

public:
	DrawingZone* m_Zone;                        //从属的OCRZone

	bool isChecked;

public:
	OCRAlternativePoint(DrawingZone* Zone = nullptr);
	OCRAlternativePoint(const OCRAlternativePoint& copy);
	OCRAlternativePoint(const OCRPoint& copy);

public:
	void addOcrPoint(const OCRPoint& input);  //添加点
	void addOutSideOcrPoint(const OCRPoint& input);  //添加比自己大的点
	void addOcrPointAuto(const OCRPoint &input);  //添加点后自动判断 变更主点
	void newWithPoint(const OCRPoint& input);

//	OCRPoint operator[](int index)const;
	OCRPoint getBest( )const;                  //获取最有可能的点
	std::string toString()const;       

	void sort( );

	void setAllInclude(int size );  //都被人包了

	AlterType getType( ); //染色种类
	cv::Scalar getColorCv( );
	QColor getColorQt( );

public:
	virtual std::string getWord( )const;
	virtual double getPossible( )const;
	virtual cv::Point getZonePoint( )const;  //默认返回最大点的情况
	virtual cv::Point getSheetPoint( )const;
	

	virtual bool isSpliter( )const;  //给一些不需要分隔符号的特殊情况使用的,主要是条件超限

public:
	static bool compareX( OCRAlternativePoint *P1, OCRAlternativePoint *P2);
	

public:
	void read(const cv::FileNode &node);
	void write(cv::FileStorage &fs) const;

};

extern void read(const cv::FileNode& node, OCRAlternativePoint& x,const OCRAlternativePoint& default_value = OCRAlternativePoint(nullptr));
extern void write(cv::FileStorage& fs, const std::string& name, const OCRAlternativePoint& x);

//刚比对结束后的原始结果(多点)
class OCR_ResultOrlWord 
{
public:

	FontPicture* _FontPicture;      //对应的字典文字的指针
	DrawingZone* m_Parent;   //从属区

public:
	std::string getWord();     //取得所用文字
	cv::Mat Mat_32f1c;              //结果图32bit

public:
	double getYuzhi();  //取得OCRset中的阈值
	std::vector<ptPsb> PointList;   //筛选结果结果
	cv::Size getSize();

public:
	OCR_ResultOrlWord(FontPicture* word,DrawingZone* _parent);
	
};

//OCR的原始结果集合,上面的数据结构的Vector形式
struct OCROrlResults  
{
	std::vector<OCR_ResultOrlWord*> m_OrlResults;  //原始结果
	//	std::map<std::string,OCR_ResultOrlWord*> map_OrlResults;  //可以检索的原始结果

	void add(OCR_ResultOrlWord* Add_word);  //添加
	unsigned int size();

	OCR_ResultOrlWord* at(int num);           //序号检索
	//	OCR_ResultOrlWord* at(std::string str);   //随机名称检索

	OCR_ResultOrlWord* operator[](int _Pos);  

	void clear();
	~OCROrlResults(); 
};

//识别结果行,给识别结果后处理部分使用
struct OCRLine
{
public:
	DrawingZone *_OCRZone;  	 //对应的OCRZone指针

	int y;                   //行坐标,zone坐标
	std::vector<OCRAlternativePoint*> Line;   //只引用不存储

	int LineNum;
public:
	std::string toString(std::string spliter = ",");  //将这一行文字转化为输出文字,默认带分隔符号为","

	OCRLine(OCRAlternativePoint *AddPoint);
	~OCRLine();

	void addAltPoint(OCRAlternativePoint *AddPoint);

	int getSheetPosY();

	static bool compare( OCRLine *L1, OCRLine *L2);
};






