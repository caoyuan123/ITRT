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

	DrawingType *m_DrawingType;  //设定的图纸种类
	
	void numInit( ); //用于找数字的那个初始化

private:
	std::vector<std::string> Distorage[DictionaryFiledMax];       //存储字库的结构体

	volatile bool writeLock;  //写入锁 true为上锁 false 为开锁
	volatile bool m_SyncFontPicture;   //是否字和图片是同步的 true - 同步
	
	bool shallowCopy;          //是否为浅拷贝

	bool spOverrun;        //人工指定 条件超限
	bool spProtectFollow;  //人工指定 防护带动道岔

	int m_conditionSingleSwitch;  //是否为 条件单动道岔 0-不是 1-是 2-混合

	void Clear_FontPicture();  //清空字体图片

public:
	std::vector<FontPicture*> _FontPicture[DictionaryFiledMax];    //存图片的结构体
	std::vector<FontPicture*> _FontPicture_Similar[DictionaryFiledMax];    //存在第二字体

	CBI_TableDictionary(DrawingBook *_Parent = nullptr);
	CBI_TableDictionary(const CBI_TableDictionary &copy,bool deep = false);

	~CBI_TableDictionary();

public:	
	virtual std::vector<std::string> getDistorage(int num );  //设定存储的内容 序号为IfDictionary::Field
	virtual void setDistorage(int num,const std::vector<std::string> &input);

	bool getIgnoreOverrun() const;    //是否忽略条件超限道岔
	void setIgnoreOverrun(bool val);

	bool getIgnoreProtectFollow() const;     //是否 忽略带动防护道岔
	void setIgnoreProtectFollow(bool val);

	void setConditionSingleSwitch(int bl);

	void setDrawingType(const DrawingType & dt);  //设置使用的
	
	DrawingType getDrawingType(); 

	void read(const cv::FileNode &node);    //存xml
	void write(cv::FileStorage &fs) const ; //读xml

	volatile bool isFontSync( ); //字体图片生成了么
	bool isShallowCopy( );   //是否为浅拷贝
	bool isPublicDictionary( ); //是否为公有字典

	volatile bool getWriteLock() const;
	void setWriteLock(volatile bool val);

	volatile bool isOcrReady( ); //是否准备好OCR了
public: 
	void slot_GenerateFontPicture();  //生成字体图片

	void slot_readDev(std::string configPath); //从DevInfo中读取
	

public:
	//静态一个生成图片引擎函数
	static void GenerateFontPicture(const std::vector<std::string> &Ipt_Distorage,std::vector<FontPicture*> &Out_FontPicture ,int DicType
		, IfFontOrlPictureGenerater *_Generater ,const FontGraphicTransTemp &_TransTemp);


//	std::string toString(int FieldNum); //把某一行变成显示模式
};





extern void read(const cv::FileNode& node, CBI_TableDictionary& x,const CBI_TableDictionary& default_value = CBI_TableDictionary(nullptr));


extern void write(cv::FileStorage& fs, const std::string& name, const CBI_TableDictionary& x);

