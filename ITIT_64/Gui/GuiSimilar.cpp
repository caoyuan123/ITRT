#include "GuiSimilar.h"
#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>
#include <cxcore.hpp>
#include <cv.hpp>

#include "StdAfx.h"
#include "MessyCodeTerminator.h"
#include "ui_GuiSimTools.h"
#include "CvLine.h"
#include "FontSimilar.h"
#include "FontGenerater.h"
#include "ImgProc.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GuiSimilarRect::init()
{
	_pen.setWidth(4);
	//_pen.setColor(QColor(0,255,0,100));
	setFlag(QGraphicsItem::ItemIsFocusable);
	setPen(_pen);
}

GuiSimilarRect::GuiSimilarRect(QGraphicsItem *parent /*= nullptr*/, QGraphicsScene *scene /*= nullptr*/)
	: QGraphicsRectItem(parent)
{
	init( );
}

GuiSimilarRect::GuiSimilarRect(const QRectF &rect,QGraphicsItem *parent /*= nullptr*/, QGraphicsScene *scene /*= nullptr*/)
	: QGraphicsRectItem(rect,parent)
{
	init( );
}

GuiSimilarRect::~GuiSimilarRect( )
{

}

int GuiSimilarRect::type( ) const
{
	return g_GraphicsRectItem;
}

void GuiSimilarRect::contextMenuEvent(QGraphicsSceneContextMenuEvent *e)
{
	QMenu menu;
	QAction *RemoveAction = menu.addAction(U8c("ɾ��"));
	QAction *selectedAction = menu.exec(e->screenPos());

	if (selectedAction == RemoveAction)
	{
		this->scene( )->removeItem(this);
	}
	
}

void GuiSimilarRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
	QGraphicsRectItem::paint(painter,option,widget);
}


////////////////////////////////////////////////////////////////////////////
GuiSimilar::GuiSimilar(QWidget *parent) 
	: GuiQtFactory(parent)
{
	m_ItemRect = nullptr;
	m_ItemLine = nullptr;

	m_modeStye = 1;
	drawRect = false;
	drawLineV = false;
	drawLineH = false;

	m_Rect = cv::Rect(0,0,0,0);

	m_complete = false;

	m_SimialrEngine = new FontSimilar( );
	m_FontApx = new FontApx( );
	m_simThread = new SimilarThead( );

	m_Tools = new QDialog(nullptr);
	m_Tools->setModal(true);

	ui = new Ui::GuiSimTools( );
	ui->setupUi(m_Tools);
	ui->pushButton_Cf->setEnabled(false);
	
	ui->pushButton_DrawV->hide( );
	ui->pushButton_DrawH->hide( );

	ui->verticalLayout_Main->addWidget(this);
	ui->pushButton_reCf->setEnabled(false);
	ui->pushButton_FinalCf->setEnabled(false);

	connect(ui->pushButton_RectCf,SIGNAL(clicked( )),this,SLOT(slot_RectCfClick( )));
	connect(ui->pushButton_reCf,SIGNAL(clicked( )),this,SLOT(slot_ReChoiceClick( )));
	connect(ui->pushButton_FinalCf,SIGNAL(clicked( )),this,SLOT(slot_calculateStartClick( )));
	connect(ui->pushButton_Cancel,SIGNAL(clicked()),m_Tools,SLOT(close()));
	connect(ui->pushButton_Cf,SIGNAL(clicked()),this,SLOT(slot_PushbuttonConfirm()));
	
	this->setMouseTracking(true);

	manualFontName = "txt";
	isbold = true;

	m_Tools->update();
}

GuiSimilar::~GuiSimilar( )
{
	delete(ui);

	if (m_SimialrEngine != nullptr)
	{
		delete(m_SimialrEngine);
	}
	
	if (m_FontApx != nullptr)
	{
		delete(m_FontApx);
	}

	if (m_simThread != nullptr)
	{
		delete(m_simThread);
	}
}

void GuiSimilar::mousePressEvent(QMouseEvent *e)
{
	if((e->buttons( ) & Qt::MidButton) == Qt::MidButton)
	{
		GuiQtFactory::mousePressEvent(e);
		return;
	}else
	{
		if (m_modeStye == 1)  //��ͼģʽ
		{
			if ((e->buttons( ) & Qt::LeftButton) == Qt::LeftButton)
			{
				if (!drawRect)
				{
					//���֮ǰ��
					clearRects( );
					
					startPoint = this->mapToScene(e->pos( ));
					GuiSimilarRect *newRect = new GuiSimilarRect(QRectF(startPoint , startPoint));

					//����,��һ�λ�
					this->scene( )->addItem(newRect);
					m_ItemRect = newRect;
										
					drawRect = true;	

				}else
				{
					drawRect = false;
				}
			}

		}else if (m_modeStye == 2)
		{
			if ((e->buttons( ) & Qt::LeftButton) == Qt::LeftButton)
			{
				if (m_ItemLine != nullptr)
				{
					drawLineV = false;
					drawLineH = false;

					m_ItemLine = nullptr;
				} 
				else /*if (!drawLineV)*/
				{
					//���������
					startPoint = this->mapToScene(e->pos( ));
					GuiSimilarLine *newLine = new GuiSimilarLine(QLineF(startPoint , startPoint));

					this->scene()->addItem(newLine);
					m_ItemLine = newLine;

					drawLineV = true;
				}
			}else if((e->buttons( ) & Qt::RightButton) == Qt::RightButton )
			{

				//�Ҽ���ת ѡ��������
				if (m_ItemLine != nullptr)
				{
					if (drawLineV)
					{
						drawLineV = false;
						drawLineH = true;

						m_ItemLine->setVH(false);
					}
					else if (drawLineH)
					{
						drawLineV = true;
						drawLineH = false;

						m_ItemLine->setVH(true);
					}
					else
					{

					}
				}
				
			}

		}

		QGraphicsView::mousePressEvent(e);
		return;
	}
}

void GuiSimilar::mouseReleaseEvent(QMouseEvent *e)
{
	if (m_modeStye == 2)
	{



	}

	GuiQtFactory::mouseReleaseEvent(e);
}

void GuiSimilar::mouseMoveEvent(QMouseEvent *e)
{
	if (m_modeStye == 1)
	{
		if (drawRect)
		{
			endPoint = this->mapToScene(e->pos( ));

			if (m_ItemRect != nullptr)
			{
				m_ItemRect->setRect(QRectF(startPoint,endPoint));
			}
			
			update( );
		}
	}
	else if (m_modeStye == 2)
	{
		if (drawLineH && !drawLineV)
		{
			endPoint = this->mapToScene(e->pos( ));

			endPoint.setY(startPoint.y( ));
			if (m_ItemLine != nullptr)
			{
				m_ItemLine->setLine(QLineF(startPoint,endPoint));
			}
	
		}

		/**/
		if (drawLineV && !drawLineH)
		{
			endPoint = this->mapToScene(e->pos( ));

			endPoint.setX(startPoint.x( ));
			if (m_ItemLine != nullptr)
			{
				m_ItemLine->setLine(QLineF(startPoint,endPoint));
			}
		}

	}


	QGraphicsView::mouseMoveEvent(e);
}

cv::Mat GuiSimilar::getRectMat( )
{
	if (drawRect == false && m_ItemRect != nullptr)
	{
		QRectF F = m_ItemRect->rect( );

		if (F.width() < 0)
		{
			F.setTopLeft(QPointF(F.x() + F.width() , F.y()));
			F.setSize(QSizeF(0 - F.width(),F.height()));
		}
		
		if (F.height() < 0)
		{
			F.setTopLeft(QPointF(F.x() , F.y() + F.height()));
			F.setSize(QSizeF(F.width() , 0 - F.height()));
		}

		m_Rect = cv::Rect(F.x(),F.y(),F.width(),F.height());

		return m_MatBackGround(m_Rect);
	}
	else if (m_modeStye == 2) //���ߵ�ʱ��Ѿ����Ƶ���
	{
		return m_MatBackGround;
	}
	else
	{
		return cv::noArray( ).getMat( );
	}

	qFatal(U8c("getRectMat ����"));
}

void GuiSimilar::slot_ChangeState(int newState) //�л�״̬���Ĳۺ���
{
	//1->2 ��Ҫ��� �п�ѡ
	if (m_modeStye == 1 && newState == 2)
	{
		//����кϷ���ѡ�Ϳ���ת
		if (m_ItemRect != nullptr)
		{
			if (m_ItemRect->rect( ).x( ) * m_ItemRect->rect( ).y( ) != 0)
			{
				cv::Mat newMat = getRectMat( );  //��modeStye��ź�� ע��

				clearRects( );
				//printf("1");

				changeBackGround(newMat);
				
				ui->pushButton_RectCf->setEnabled(false);
				ui->pushButton_reCf->setEnabled(true);
				ui->pushButton_FinalCf->setEnabled(true);

				m_modeStye = 2;  //������ͼ��ʱ����Ҫ���״̬,����������

				//��ȡ Rect��ͼ
				
				/*
				std::vector<int> vec_int = ImgProc::calWordDistance(newMat);
				for (int i = 0;i < vec_int.size();i++)
				{
					GuiSimilarLine *newLine = new GuiSimilarLine(QLineF(vec_int[i],0,vec_int[i],newMat.size().height));
					this->scene()->addItem(newLine);
				}*/

				int top = 0,down = 0;

				ImgProc::calWordHight(newMat,top,down);

				GuiSimilarLine *newLine = new GuiSimilarLine(QLineF(0,top,newMat.size().width,top));
				newLine->setVH(false);
				this->scene()->addItem(newLine);
			
				newLine = new GuiSimilarLine(QLineF(0,down,newMat.size().width,down));
				newLine->setVH(false);
				this->scene()->addItem(newLine);

				//�ػ�1�Ļ�����������
			}
		}

	}else if (m_modeStye == 2 && newState == 1)
	{
		clearLines( );
		changeBackGround(m_Source);
		
		ui->pushButton_RectCf->setEnabled(true);
		ui->pushButton_reCf->setEnabled(false);
		ui->pushButton_FinalCf->setEnabled(false);

		m_modeStye = 1;   
	}else
	{

	}

}

void GuiSimilar::slot_RectCfClick()
{
	slot_ChangeState(2);
}

void GuiSimilar::slot_ReChoiceClick()
{
	slot_ChangeState(1);
}

void GuiSimilar::slot_calculateStartClick()
{
	//׼����ʼ��ϵĻ���

	//1.�ⲿ����� �������
	QString str = ui->lineEdit->text();
	str = str.trimmed( );

	//�ȼ򵥷�һ�´�
	if (str == QString(""))
	{
		QMessageBox mb(U8c("����δ�ܿ�ʼ"),U8c("����д��ѡ�ַ���") ,QMessageBox::Icon::Warning,QMessageBox::StandardButton::Ok,0,0);

		mb.exec( );

	}
	else if (str.size() < 2)
	{
		QMessageBox mb(U8c("����δ�ܿ�ʼ"),U8c("���ѡ�����ּ����ϵ��ֶ����") ,QMessageBox::Icon::Warning,QMessageBox::StandardButton::Ok,0,0);

		mb.exec( );
	}else
	{
		calLines( );

		//��û����
		if (m_spX.size() < 2)
		{
			QMessageBox mb(U8c("����δ�ܿ�ʼ"),U8c("��ָ�ԭͼ�е��ַ�") ,QMessageBox::Icon::Warning,QMessageBox::StandardButton::Ok,0,0);

			mb.exec( );

		}else
		{
			
#if _DEBUG

#endif
		//2.�ڲ�׼���� �ָ�ͼƬ

		cv::Mat m = getRectMat( );

		if (m.channels( ) == 3)
		{
			cv::cvtColor(m,m,cv::COLOR_BGR2GRAY);
		}

		m_FontApx->_Mat8u1c = m;
		m_FontApx->words = str.toLatin1();

	//	printf("%s",m_FontApx->words);

		m_FontApx->wordPos = m_spX;
		m_FontApx->DistanceY = m_spY;

		IfFontPictureGenerater *IfFpg = new FontPictureGenerater( );

		m_FontApx->FontGenerater = IfFpg;

		if (m_SimialrEngine->List_FontSelect.size() >= 1)
		{
			m_SimialrEngine->List_FontSelect.clear();
		}

		m_SimialrEngine->List_FontSelect.push_back(*m_FontApx);

		//��һ����ȡ�ⲿ�����������б�
		m_SimialrEngine->FontPropertyFinal.FontName = manualFontName;

		m_SimialrEngine->FontPropertyFinal.FontNum = 36;
		m_SimialrEngine->FontPropertyFinal.FontBold = true;
		//ʹ�����濪ʼ����



		//��������������
		m_SimialrEngine->init( );
		m_SimialrEngine->TotalSearch(m_SimialrEngine->List_FontSelect[0]); 
		

		//������Ľ�����ͳ�ȥ
		emit(signal_calOver( ));


		//����һ�����״̬��
		slot_ShowResult( );


		/*

		FontPictureProprety fpp;

		fpp.FontNum = 36;
		fpp.FontBold = true;

		m_simThread->setEngine(*m_FontApx,fpp);

		

		m_simThread->start();*/



		delete(IfFpg);


		}

	
	}
	
	
}

void GuiSimilar::setBackGround(const cv::Mat &_mat)
{
	if (_mat.channels() == 1)
	{
		cv::cvtColor(_mat,m_Source,cv::COLOR_GRAY2BGR);
	}
	else
	{
		m_Source = _mat;
	}
	
	GuiQtFactory::setBackGround(_mat);

	m_Tools->update();
}

void GuiSimilar::clearRects( )
{
	int s = scene( )->items().size();
	if (s >= 2)
	{
		for (int i = 0 ;i < scene( )->items( ).size( );++i)
		{
			if (scene( )->items( )[i]->type( ) == g_GraphicsRectItem)
			{
				scene( )->removeItem(scene( )->items( )[i]) ;
				m_ItemRect = nullptr;
				--i;
			}
		}
	}
}

void GuiSimilar::clearLines( )
{
	int s = scene( )->items().size();
	if (s >= 2)
	{
		for (int i =0 ;i < scene( )->items( ).size( );++i)
		{
			if (scene( )->items( )[i]->type( ) == g_GraphicsLineItemV)
			{
				scene( )->removeItem(scene( )->items( )[i]) ;
				--i;
			}
			else if (scene( )->items( )[i]->type( ) == g_GraphicsLineItemH)
			{
				scene( )->removeItem(scene( )->items( )[i]) ;
				--i;
			}
		}
	}
}

void GuiSimilar::calLines( )
{
	m_spX.clear( );
	m_spY.clear( );

	int Sz = m_Scene->items( ).size( );

	for (int i = 0;i < Sz; i++)
	{
		QGraphicsItem* Qit = m_Scene->items( )[i];

		if (Qit->type( ) == g_GraphicsLineItemH)
		{
			GuiSimilarLine* Gsl = (GuiSimilarLine*) Qit;
			m_spY.push_back(Gsl->line( ).y1( )) ;
		}
		else if (Qit->type( ) == g_GraphicsLineItemV)
		{
			GuiSimilarLine* Gsl = (GuiSimilarLine*) Qit;
			m_spX.push_back(Gsl->line( ).x1( )) ;
		}
	}

	std::sort(m_spX.begin( ),m_spX.end( ));
	std::sort(m_spY.begin( ),m_spY.end( ));
}

void GuiSimilar::slot_ShowResult( )
{
	 double rs = m_SimialrEngine->FontPropertyFinal.CompareRoit;

	 rs = cv::pow(rs,0.2);
	 rs *= 100;
	 char strC[20]; 
	 sprintf(strC,"����֣�%f",rs);

	 //����Ҫ���Ϊ�����Ч�����

	 if (rs > 85)
	 {
		 ui->pushButton_Cf->setEnabled(true);
	 }
	 else
	 {
		 ui->pushButton_Cf->setEnabled(false);
	 }

#ifdef _DEBUG

	  ui->pushButton_Cf->setEnabled(true);

#endif // _DEBUG

	 ui->label_2->setText(  U8c(strC)  );

}

void GuiSimilar::slot_PushbuttonConfirm()
{

	emit(signal_calResult(m_SimialrEngine));

	m_Tools->close( );
}

void GuiSimilar::showEvent(QShowEvent *e)
{
	slot_ChangeState(1);

	GuiQtFactory::showEvent(e);

	m_Tools->resize(900,500);
	

	update();
	//m_Tools->update();
}

void GuiSimilar::setManualFontName(const QString &m_FontName,bool bl_isbold)
{
	if (m_FontName != "")
	{
		manualFontName = m_FontName;
	}else
	{
		manualFontName = "txt";
	}
	
	isbold = bl_isbold;

}


/////////////////////////////////////////////////////////////////////////////////////
GuiSimilarLine::GuiSimilarLine(const QLineF &Line, QGraphicsItem *parent /*= 0*/, QGraphicsScene *scene /*= 0*/)
	: GuiLine(Line,parent ,scene)
{
	pen.setWidth(1);
	m_isVline = true;
}

int GuiSimilarLine::type( ) const
{
	if (m_isVline)
	{
		return g_GraphicsLineItemV;
	}else
	{
		return g_GraphicsLineItemH;
	}
}

void GuiSimilarLine::setVH(bool isVline)
{
	if (isVline)
	{
		m_isVline = true;
		this->setColor(Qt::green,Qt::darkGreen);
	}else 
	{
		m_isVline = false;
		this->setColor(Qt::cyan,Qt::darkCyan);
	}
}

/////////////////////////////////////////////////////////////////////////////////
void SimilarThead::run( )
{
	simEngine->init( );
	simEngine->TotalSearch(simEngine->List_FontSelect[0]);
}

void SimilarThead::setEngine(FontApx &input,FontPictureProprety & fpp)
{
	simEngine->List_FontSelect.push_back(input);
	simEngine->FontPropertyFinal = fpp;
}

SimilarThead::~SimilarThead( )
{
	delete(simEngine);
}

SimilarThead::SimilarThead( )
{
	simEngine = new FontSimilar( );
}




