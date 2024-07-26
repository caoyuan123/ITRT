#pragma once

#include <core/core.hpp>
#include <vector>
#include <qstring.h>

#include "CvLine.h"
#include "DrawingDef.h"

#include "CBITableDef.h"
#include "CBITablePage.h" 



struct DrawingSheetFlags  //一页联锁表的流程控制
{
#define DwSt_ProcFlags_读取			0x00000001
#define DwSt_ProcFlags_矫正			0x00000002
#define DwSt_ProcFlags_滤波			0x00000004
#define DwSt_ProcFlags_合并			0x00000008

#define DwSt_ProcFlags_线处理		0x00000010
#define DwSt_ProcFlags_人工选择线	0x00000020
#define DwSt_ProcFlags_区域计算		0x00000040
#define DwSt_ProcFlags_区域人选		0x00000080

#define DwSt_ProcFlags_字典准备		0x00000100
#define DwSt_ProcFlags_字体整体准备	0x00000200
#define DwSt_ProcFlags_字体单体准备	0x00000400
#define DwSt_ProcFlags_OCR计算		0x00000800

#define DwSt_ProcFlags_人工校核		0x00001000
#define DwSt_ProcFlags_输出Excel	0x00002000

#define DwSt_ProcFlags_ReadyOCR (DwSt_ProcFlags_读取 | DwSt_ProcFlags_矫正 | DwSt_ProcFlags_滤波 | DwSt_ProcFlags_线处理|DwSt_ProcFlags_人工选择线|DwSt_ProcFlags_区域计算|DwSt_ProcFlags_区域人选 )

	unsigned int flags_Process;

	DrawingSheetFlags();
	void setProcess(const unsigned int flags,bool bl);  //写入的位数,写入1-True,2-False
	bool getProcess(const unsigned int flags) const;  //查询情况

	unsigned int& operator = (const unsigned int &input);

	operator unsigned int( ) const
	{
		return flags_Process;
	}
};



class QPixmap;
class QString;
class GuiZone;
class CBI_TableDictionary;

class DrawingSheet : public DrawingAbstruct  //一页联锁表
{
protected:	
	DrawingBook *Parent_DB; //从属的联锁本
	
	cv::Mat mat_whole; //如果有part就是拼接好的图,如果没有就是主Part的浅拷贝。cv_8uc1
	
	std::string Str_ImgName;  //图像名称
	std::string Str_OcrTime;  //识别的时间戳

	bool m_selfDic;  //是否为自有字典
	CBI_TableDictionary *Ptr_dictionary;            //字典指针

public:
	volatile bool threadLock;	//线程锁 OCR任务开始时上锁 结束后下锁
	volatile bool isDictionaryReady( );  //字典是否准备好了
	volatile bool generateDictionary( ); //生成所需字典

	CBI_TableDictionary *getCbiTableDictionary();  //获取所用字典
	bool isDwTypePublic( );  //使用的是图纸还是私有的种类设定


public:   //自身属性
	int PageNum;		//他是第几页联锁表
	bool blParts;       //true是拆分,false是不拆分
	DrawingSheetFlags flags_Process; //处理流程flags
	bool blFocus;      //是否为焦点



public:   //内部加工出来的东西的存储
	std::vector<DrawingPart*> _AllDrawingParts; //所有部分的联锁表
	cv::Mat smallMat;  //缩略图,显示用的

	std::vector<CvLine> VLines;  //列排线  VLine vertical
	std::vector<CvLine> HLines; //行排线  Hline horizontal


public:   //区域划分相关
	std::vector<cv::Rect> Rects; //区域队列（原始）所有待选项

	std::vector<DrawingZone*> m_ZoneBeSelected;                  //容器检索的Zone

	CBITablePage92 out_CbiTableResult;                    //最后的识别结果在这里

public: //构造相关
	DrawingSheet(DrawingBook* Parent = nullptr);
	~DrawingSheet();

	void ReadOne(std::string Str,int num);      //只有一个图的读入

	void testRead(std::string Str);    //调试用的读入函数
	static bool compare(const DrawingSheet &spm1,const DrawingSheet &spm2); //排序方法,按序号排序

public: 
	void setParent(DrawingBook* parent );
	DrawingBook* getParent( );  //取得从属联锁表

	cv::Mat getZoneMat(CBITable::Headers head); //得到切割图片
	cv::Mat getWholeMap(int step = 0 ) const; //一页完整的联锁表	

	cv::Mat getshowMat( ) const;  //返回最后准备好的缩小的完整图
	QPixmap getshowPix( ) const;  //返回最后准备好的缩小的完整图
	QString getName( )const;
	int     getPartNum( );  //共分几页
	
	void setOCRTime(std::string str);
	std::string getOCRTime();

	cv::Mat drawResultPicture(int OrlUsed = 2 );  //画结果的总图
	
	void TempSave( );

	void toiLockTable92( );             //将结果转换成92格式

	int getNextButtonStatus( );			//获取Next按钮的状态

public:  //对外的接口

	virtual cv::Mat getMat(int step = 0);
	virtual DrawingBook* getDrawingBook( );
	virtual DrawingSheet* getDrawingSheet( );
	virtual DrawingType* getDrawingType( );
	virtual unsigned char getDrawingTableType( );


	//设置每一步处理后的中间步骤图
	virtual void setImgProcCorrect(CvPoint Area[4],cv::InputArray _Crt);
	virtual void setImgProcCorrect(cv::Point Area[4],cv::InputArray _Crt);
	virtual void setImgProcFilter(std::vector<cv::Point> _fltPoints ,cv::InputArray _Crt);
	virtual void setImgProcLiner(std::vector<CvLine> &_VLine,std::vector<CvLine> &_Hline);
	virtual void setImgProcZone(std::vector<GuiZone*> _GuiZone );

public:
	void read(const cv::FileNode &node);
	void write(cv::FileStorage &fs) const;


protected:	 //私有的内部运算方法
	void calSmallMap();  //算小图
	void resort(); //利用yaml中存储的信息和参数回溯整个数据结构
};


extern void read(const cv::FileNode& node, DrawingSheet& x,const DrawingSheet& default_value = DrawingSheet(nullptr));
extern void write(cv::FileStorage& fs, const std::string& name, const DrawingSheet& x);


