#pragma once
//#include "FontSimilar.h"
#include "FontGraphicTransTemp.h"
#include "FontPictureDef.hpp"



//存储图纸类型的类
class DrawingType   //图纸类别
{
public:
	std::string strDesignInstitute;            //设计院名称
	std::string strLineName;   //线路名称

	int saveType;  //版本

	/*
	 * 版本0的   CompareRoit是原生率
	 * 版本1以后 CompareRoit是矫正率
	 */


	FontWordProprety _FontProprety;         //大字体属性
	FontWordProprety small_FontProprety;    //小字体属性
	FontGraphicTransTemp _GraphicTransTemp;  //大致图像变换属性

	int stdWordHeight;  //标准字符高度
	int stdWordWidth;	//标准字符宽度

	int TableEdition;   //版本类型 取值1978/1992/2018 标识联锁表的类型
	int lineNums;  //一页大约是多少行的联锁表 取值范围是 0-100

	double wordHightRoit;   //字高占A3宽的比例
	double lineHightRoit;   //行高占A3宽的比例


	int RowNum;            //一共多少列
	int HeaderTable[50];   //-1代表截止,表头排序 数值就是 Headers中的数值
	double HeaderTableRoit[50];   //-1代表截止,表头的大致宽度

	DrawingType();

public:	
	std::string toString() ; //对外显示
	bool isExisting();  //获取是否为已经存贮过的

	//读写xml
	void read(const cv::FileNode &node);
	void write(cv::FileStorage &fs) const;
};



extern void read(const cv::FileNode& node, DrawingType& x,const DrawingType& default_value = DrawingType());
extern void write(cv::FileStorage& fs, const std::string& name, const DrawingType& x);




//所有图纸种类的保存路径
#define DtPath "DrawingTypeList.yaml"  

//存储图纸种类的集合
class DrawingTypeList : public std::vector<DrawingType>
{
public:
	void append(const DrawingType&);

	bool checkRepeatName(QString designName , QString lineName );  //有返回true 没有返回false 检查名称重复
	
	void read( );
	void save( );

	DrawingTypeList();
	~DrawingTypeList();

};




