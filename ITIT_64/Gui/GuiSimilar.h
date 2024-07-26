#ifndef GUISIMILAR_H
#define GUISIMILAR_H

#include "GuiQtFactory.h"
#include "GuiLine.h"
#include <QThread>

namespace cv
{
	class Mat;
}

class FontSimilar;
class FontApx;
class FontPictureProprety;


//画出来的线
class GuiSimilarLine : public GuiLine
{
	bool m_isVline;
	

public:
	GuiSimilarLine(const QLineF &Line, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

	void setVH(bool isVline);

	virtual int type( ) const;

	

};


//为了做成进度条的样子
class SimilarThead : public QThread 
{
	Q_OBJECT

protected:
	FontSimilar *simEngine;
	

public:
	SimilarThead( );

	~SimilarThead( );

	void setEngine(FontApx &input,FontPictureProprety & fpp);

	virtual void run( );

signals: //信号

	void signal_ProcessBar(int);

};


//中心空的框选图形

//有个菜单可以取消

//移动时是绿框,红色是选定
class GuiSimilarRect : public QObject,public QGraphicsRectItem
{
	Q_OBJECT

protected:
	QPen _pen;

	void init();

public:
	GuiSimilarRect(QGraphicsItem *parent = nullptr, QGraphicsScene *scene = nullptr);
	GuiSimilarRect(const QRectF &rect,QGraphicsItem *parent = nullptr, QGraphicsScene *scene = nullptr);
	~GuiSimilarRect();

	virtual int type() const;

	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);


private:



};



//选择逼近的控件  
namespace Ui {
	class GuiSimTools ;
} // namespace Ui

class GuiSimilar : public GuiQtFactory
{
	Q_OBJECT

protected:
	QPointF startPoint;
	QPointF endPoint;

	bool drawRect;      //画框
	bool drawLineV;     //竖线
	bool drawLineH;		//水平线

	GuiSimilarRect *m_ItemRect;   //当前的矩形
	GuiSimilarLine *m_ItemLine;		//当前的线	

	cv::Mat m_Source; //原始图
	cv::Rect m_Rect; //框选的矩形

	std::vector<int> m_spX; //X轴分隔点
	std::vector<int> m_spY; //Y轴分隔点

	FontSimilar *m_SimialrEngine; //相似逼近引擎
	FontApx * m_FontApx;		//逼近 算子
	SimilarThead *m_simThread;    //线程版本

	//m_modeStye 1是外部框选 2是画线 在父类中

	bool m_complete;   //完成状态机

	QString manualFontName;                  //用户给的初始数据—字体名称 这个名称现在分为_ZH,TXT,宋体
	bool isbold;

public:
	explicit GuiSimilar(QWidget *parent = nullptr);
	~GuiSimilar( );
	
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void setBackGround(const cv::Mat &_mat);  //设置背景

	void clearRects( );
	void clearLines( );

	void calLines( );  //将线计算成分隔点


	//void drawNewVline( );  //画新竖线
	//void drawNewHline( );  //画新横线

	cv::Mat getRectMat( ); //获得节选区域
		
	QDialog* m_Tools;  //工具按钮窗口

	void setManualFontName(const QString &m_FontName = "txt" ,bool isbold = true); //指定字体名称,默认就是txt

public slots:
	void slot_ChangeState(int ); //切换显示模块的状态机
	void slot_RectCfClick( );
	void slot_ReChoiceClick( );
	void slot_calculateStartClick( );  //总确认按下
	void slot_ShowResult( ); //展示结果分
	void slot_PushbuttonConfirm( ); //最后界面按下的确认按钮


	virtual void showEvent(QShowEvent *);

signals:

	void signal_calResult(void* result);  //会完整的发送一个新拷贝过的 字体属性指针出去
	void signal_calOver( );  //计算完毕

	

private:
	Ui::GuiSimTools *ui;
};


//画线的那个窗口



#endif // GUISIMILAR_H





