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


//����������
class GuiSimilarLine : public GuiLine
{
	bool m_isVline;
	

public:
	GuiSimilarLine(const QLineF &Line, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

	void setVH(bool isVline);

	virtual int type( ) const;

	

};


//Ϊ�����ɽ�����������
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

signals: //�ź�

	void signal_ProcessBar(int);

};


//���ĿյĿ�ѡͼ��

//�и��˵�����ȡ��

//�ƶ�ʱ���̿�,��ɫ��ѡ��
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



//ѡ��ƽ��Ŀؼ�  
namespace Ui {
	class GuiSimTools ;
} // namespace Ui

class GuiSimilar : public GuiQtFactory
{
	Q_OBJECT

protected:
	QPointF startPoint;
	QPointF endPoint;

	bool drawRect;      //����
	bool drawLineV;     //����
	bool drawLineH;		//ˮƽ��

	GuiSimilarRect *m_ItemRect;   //��ǰ�ľ���
	GuiSimilarLine *m_ItemLine;		//��ǰ����	

	cv::Mat m_Source; //ԭʼͼ
	cv::Rect m_Rect; //��ѡ�ľ���

	std::vector<int> m_spX; //X��ָ���
	std::vector<int> m_spY; //Y��ָ���

	FontSimilar *m_SimialrEngine; //���Ʊƽ�����
	FontApx * m_FontApx;		//�ƽ� ����
	SimilarThead *m_simThread;    //�̰߳汾

	//m_modeStye 1���ⲿ��ѡ 2�ǻ��� �ڸ�����

	bool m_complete;   //���״̬��

	QString manualFontName;                  //�û����ĳ�ʼ���ݡ��������� ����������ڷ�Ϊ_ZH,TXT,����
	bool isbold;

public:
	explicit GuiSimilar(QWidget *parent = nullptr);
	~GuiSimilar( );
	
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void setBackGround(const cv::Mat &_mat);  //���ñ���

	void clearRects( );
	void clearLines( );

	void calLines( );  //���߼���ɷָ���


	//void drawNewVline( );  //��������
	//void drawNewHline( );  //���º���

	cv::Mat getRectMat( ); //��ý�ѡ����
		
	QDialog* m_Tools;  //���߰�ť����

	void setManualFontName(const QString &m_FontName = "txt" ,bool isbold = true); //ָ����������,Ĭ�Ͼ���txt

public slots:
	void slot_ChangeState(int ); //�л���ʾģ���״̬��
	void slot_RectCfClick( );
	void slot_ReChoiceClick( );
	void slot_calculateStartClick( );  //��ȷ�ϰ���
	void slot_ShowResult( ); //չʾ�����
	void slot_PushbuttonConfirm( ); //�����水�µ�ȷ�ϰ�ť


	virtual void showEvent(QShowEvent *);

signals:

	void signal_calResult(void* result);  //�������ķ���һ���¿������� ��������ָ���ȥ
	void signal_calOver( );  //�������

	

private:
	Ui::GuiSimTools *ui;
};


//���ߵ��Ǹ�����



#endif // GUISIMILAR_H





