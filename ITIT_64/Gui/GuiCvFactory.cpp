#include "guicvfactory.h"
#include "IfGUI.h"
#include <opencv.hpp>
#include <QtGui/QtGui>
#include <QDebug>
#include <QMessageBox>
#include "ImgProcFilter.h"
#include "MessyCodeTerminator.h"
#include <exception>


GuiCvFactory::GuiCvFactory(QWidget *parent ,IfGUI* GraphicInterFace )
	: QWidget(parent)
{
	if (GraphicInterFace != nullptr)
	{
		_GraphicInterFace = GraphicInterFace;
		_slefGui = nullptr;
	}else
	{
		_GraphicInterFace = new ImgProcFilter(); //�����õ�
		_slefGui = _GraphicInterFace;
	}
		
	zoomRoit = 1.0000000f;

	_initCopyImage(true);

	//this->setMinimumSize(m_qt_img.size());
	//this->setMaximumSize(m_qt_img.size());
	firstRoi = true;
	if (zoomMode < 2)
	{
		zoomMode = 1;
	}
	 
	RoiZone.x = 0;
	RoiZone.y = 0;
	RoiZone.width = m_qt_img.width();
	RoiZone.height = m_qt_img.height();
	RoiMoveButtonDown = false;
	RoiMove = false;

	m_colored = true;  //Ĭ�ϲ�ɫģʽ
	debugShow = false;

	if (zoomMode == 2) 
	{
		zoomResize(1); //����һ�£���Ҫ����Ǹ���ͼ����
	}

	this->setBaseSize(m_qt_img.size());
	this->setSizeIncrement(1,1);
	this->resize(m_qt_img.size());

	_GraphicInterFace->drawMat();  //д��

}

GuiCvFactory::~GuiCvFactory()
{
	if (_slefGui != nullptr)
	{
		delete(_slefGui);  
	}
	
	//qDebug() <<"FactoryR";

}

void GuiCvFactory::paintEvent(QPaintEvent * q)
{	
	_copyImage();
	QPainter painter(this);

	if (zoomMode == 2)
	{
		painter.drawImage(QPoint(-RoiZone.x,-RoiZone.y),m_qt_img);
			
	}else
	{
		painter.drawImage(QPoint(0,0),m_qt_img);
	}


#ifdef __DEBUG 
	imshow("debugcv",m_cv_img);
#endif
	
}

//************************************
// Method:    _copyImage
// FullName:  GuiCvFactory::_copyImage
// Access:    protected private 
// Returns:   void
// Qualifier: ����cv��ʽͼ����qt��ʽͼ��,����CV��QT�ӿں���
// Parameter: void
//************************************
void GuiCvFactory::_copyImage(void)//����cv��ʽͼ����qt��ʽͼ��
{
	_initCopyImage();
	switch(zoomMode)
	{
	case 0:
	case 1:
		{
			cv::resize(_GraphicInterFace->showMat(),ReSizeTemp,
				cv::Size(m_qt_img.width(),m_qt_img.height()));

			

			if (m_colored)
			{
				if (ReSizeTemp.channels() == 3)
				{
					cv::cvtColor(ReSizeTemp,m_cv_img,cv::COLOR_BGR2BGRA);
				}else if (ReSizeTemp.channels() == 4)
				{

					QMessageBox::information(nullptr,"1","1");
					m_cv_img = ReSizeTemp.clone();
				}else
				{
					qFatal( "ReSizeTemp channel Error");
				}
				
				/*
				if (debugShow)
				{
					imshow("debugShow1",_GraphicInterFace->showMat());
					//imshow("debugShow2",ReSizeTemp);
					imshow("debugShow3",ReSizeTemp);
					printf("1");
				}*/


				
			}
			else  //�ڰ�ģʽ
			{
				cv::cvtColor(ReSizeTemp,m_cv_img,cv::COLOR_GRAY2BGRA);
			}
			
		}break;
	case 2:
		{
			if (RoiMove)  //�����ROI�ƶ�����Ҫ���´�ͼ
			{
				RoiMove = false;
			}else
			{

				cv::resize(_GraphicInterFace->showMat(),ReSizeTemp,
					m_cv_img.size());

				if (m_colored)  
				{
					cv::cvtColor(ReSizeTemp,m_cv_img,cv::COLOR_BGR2BGRA);
				}//�ڰ�ģʽ����Ҫת��
				else
				{
					cv::cvtColor(ReSizeTemp,m_cv_img,cv::COLOR_GRAY2BGRA);
				}
				
			}
		}break;

	case 3:
		{

		}break;
	}
	
		
	

}

void GuiCvFactory::mouseMoveEvent(QMouseEvent * e)
{
	int x = 0,y = 0;

	if (zoomMode == QtRoiZoom)
	{
		x = (e->x() + RoiZone.x) /zoomRoit;
		y = (e->y() + RoiZone.y) /zoomRoit;
	}else
	{
		if (0.999999 < zoomRoit && zoomRoit < 1.000001)
		{
			x = e->x();
			y = e->y();
		}
		else
		{
			x = e->x() / zoomRoit ;
			y = e->y() / zoomRoit  ;
		}
	}


	if (e->buttons() != Qt::MidButton)   //���м���קʱ
	{
		if (_GraphicInterFace->mouseMove(x,y))
		{
			_GraphicInterFace->drawMat();  //д��
			update();
		}
	}

	if (e->buttons() == Qt::MidButton && RoiMoveButtonDown)
	{
		RoiZone.x = RoiMoveStart.x - (e->x() - RoiMoveBegin.x) ;
		RoiZone.y = RoiMoveStart.y - (e->y() - RoiMoveBegin.y) ;

#ifdef __DEBUG  
		std::cout << "x:"<<RoiZone.x << " ";
		std::cout << "y:"<<RoiZone.y << std::endl;
#endif
		
		/*
		if (RoiZone.x < 0)
		{
			RoiZone.x = 0;
		}

		if (RoiZone.y < 0)
		{
			RoiZone.y = 0;
		}*/
		RoiMove = true;
		update();
	}
}

void GuiCvFactory::mousePressEvent(QMouseEvent *e)
{
	int x = 0,y = 0;
	if (0.999999 < zoomRoit && zoomRoit < 1.000001)
	{
		x = e->x();
		y = e->y();
	}
	else
	{
		x = e->x() / zoomRoit ;
		y = e->y() / zoomRoit  ;
	}

	if (zoomMode == QtRoiZoom)
	{
		x = (e->x() + RoiZone.x) /zoomRoit;
		y = (e->y() + RoiZone.y) /zoomRoit;
	}


	if (e->buttons() == Qt::LeftButton)
	{
		if (_GraphicInterFace->mouseDown(x,y))
		{
		//	printf("2");
			_GraphicInterFace->drawMat();  //д��
			update();
		}
	}

	if (e->buttons() == Qt::MidButton)
	{
		RoiMoveStart.x = RoiZone.x;
		RoiMoveStart.y = RoiZone.y;
		RoiMoveBegin = cv::Point(e->x(),e->y());
		RoiMoveButtonDown = true;
		setCursor(Qt::ClosedHandCursor);

	}

}

void GuiCvFactory::mouseReleaseEvent(QMouseEvent *e)
{
	int x = 0,y = 0;
	if (0.999999 < zoomRoit && zoomRoit < 1.000001)
	{
		x = e->x();
		y = e->y();
	}
	else
	{
		x = e->x() / zoomRoit ;
		y = e->y() / zoomRoit  ;
	}

	int ddd = e->buttons();

	if (e->buttons() != Qt::LeftButton)
	{
		if (_GraphicInterFace->mouseUp(x,y))
		{
			_GraphicInterFace->drawMat();  //д��
			update();
		}
	}

	if (e->buttons() != Qt::MidButton)
	{
		RoiMoveButtonDown = false;
		setCursor(Qt::ArrowCursor);
	}
}

void GuiCvFactory::keyReleaseEvent(QKeyEvent *e)
{
	if (_GraphicInterFace->keyUp(0,e->key()))
	{
		_GraphicInterFace->drawMat();
		update();
	}


	if (zoomMode < 2)
	{
		return;
	}

	switch(e->key())
	{
	case '+': 
		{
			if (zoomMode == 2)
			{
				zoomResizeStep(5);
				update();
			}
		}break;

	case '-': 
		{
			if (zoomMode == 2)
			{
				zoomResizeStep(-5);
				update();
			}

		}break;
	}
}

void GuiCvFactory::zoomResize(double newSize /*= 100*/)
{
		
	int newWidth = _GraphicInterFace->showMat().cols * newSize;
	int newHeight = _GraphicInterFace->showMat().rows * newSize;

	if (newWidth < 2)
	{
		newWidth = 2;
	}
	if (newHeight < 2)
	{
		newHeight = 2;
	}

	
	switch(zoomMode)
	{
	case 0: 
	case 1:
		{
			//��ȡ��Ҫ��ͼ�ĵط�
			m_qt_img = QImage(QSize(newWidth,newHeight),QImage::Format_ARGB32);
			//����һ��cvͼ��ͷ��qtͼƬ
			m_cv_img = cv::Mat (cv::Size(newWidth,newHeight),CV_8UC4,m_qt_img.bits());
		}break;
	case QtRoiZoom: 
		{
			//��ȡ��Ҫ��ͼ�ĵط�
			m_qt_img = QImage(QSize(newWidth,newHeight),QImage::Format_ARGB32);
			//����һ��cvͼ��ͷ��qtͼƬ
			m_cv_img = cv::Mat (cv::Size(newWidth,newHeight),CV_8UC4,m_qt_img.bits());
		
		}break;
	case CvRoiZoom:
		{
			//����Ŵ�ģʽ�� ��ͼ�Ǵ��ڴ�С
			//	m_qt_img = QImage(QSize(RoiZone.width,RoiZone.height),QImage::Format_ARGB32);
			//	m_cv_img = cv::Mat (cv::Size(m_qt_img.width(),m_qt_img.height()),CV_8UC4,m_qt_img.bits());
		}
	}


	//_copyImage();
	

}

void GuiCvFactory::resizeEvent(QResizeEvent *e)
{
	switch(zoomMode)
	{
	case 0: break;  //�����ܷŴ�

	case 1:    //����Ŵ�
		{
			zoomRoit =e->size().height() / (double)_GraphicInterFace->showMat().size().height  ;
			zoomResize(zoomRoit);
		}break;

	case 2:    //����Ŵ�
		{

			if (firstRoi)
			{
				zoomRoit =e->size().height() / (double)_GraphicInterFace->showMat().size().height  ;
				zoomResize(zoomRoit);
				firstRoi = false;
			}
			RoiZone.height = e->size().height();
			RoiZone.width = e->size().width();

		}break;

	default: 
		{

		}break;
	}
}

void GuiCvFactory::wheelEvent(QWheelEvent *e)
{
	if (e->buttons() == Qt::MidButton)  //�����м�
	{
		return;
	}


	switch(zoomMode)
	{
	case 0: break;

	case 1:    //����Ŵ�
		{
		
		}break;

	case 2:    //����Ŵ�
		{
			int numDegree = e->delta() / 60 * 3.1415926535f;
			
			zoomResizeStep(numDegree);
			update();

		}break;

	default: 
		{

		}break;
	}
}

//ע��newStep * 0.01����
void GuiCvFactory::zoomResizeStep(double newStep)
{
	zoomRoit += 0.01 * newStep;  //�Դ������Ƕ�
	zoomResize(zoomRoit);
}

void GuiCvFactory::setZoomMode(int Mode)
{
	if (Mode <= 2 && Mode >= 0)
	{
		zoomMode = Mode;
	}else
	{
		qFatal(U8c("������δ���"));
	}
	
}

//�ڶ���ֻ��ʾ�Ĵ���
void GuiCvFactory::setIfGui(IfGUI* newShow)
{
	if (_slefGui != nullptr)
	{
		delete(_slefGui);
		_slefGui = nullptr;
	}

	_GraphicInterFace = newShow;

	_initCopyImage();

	_GraphicInterFace->drawMat();

}

void GuiCvFactory::imgProc(cv::InputArray src,cv::OutputArray dst)
{
	_GraphicInterFace->imgProc(src,dst);
	_initCopyImage();
}

void GuiCvFactory::_initCopyImage(bool force)
{
	cv::Mat temp = _GraphicInterFace->showMat();

	switch(temp.channels())
	{
	case 1:   //�ڰ�ͼƬ 
		{
			if (m_colored || force)
			{
				m_colored = false;  //�ڰ�ģʽ

				m_qt_img = QImage(QSize(temp.cols,temp.rows),QImage::Format_ARGB32);
				m_cv_img = cv::Mat (cv::Size(m_qt_img.width(),m_qt_img.height()),CV_8UC4,m_qt_img.bits()); 
			}
		}break;
	case 3:
		{
			if (!m_colored || force)
			{
				m_colored = true;  //��ɫģʽ

				m_qt_img = QImage(QSize(temp.cols,temp.rows),QImage::Format_ARGB32);
				m_cv_img = cv::Mat (cv::Size(m_qt_img.width(),m_qt_img.height()),CV_8UC4,m_qt_img.bits()); 
			}
		}break;
	case 4:  //��ɫͼƬ 
		{
			if (!m_colored || force)
			{
				m_colored = true;  //��ɫģʽ

				m_qt_img = QImage(QSize(temp.cols,temp.rows),QImage::Format_ARGB32);
				m_cv_img = cv::Mat (cv::Size(m_qt_img.width(),m_qt_img.height()),CV_8UC4,m_qt_img.bits()); 
			}


		}break;
	default:
		{
			if (!m_colored || force)
			{
				m_colored = true;  //��ɫģʽ

				m_qt_img = QImage(QSize(temp.cols,temp.rows),QImage::Format_ARGB32);
				m_cv_img = cv::Mat (cv::Size(m_qt_img.width(),m_qt_img.height()),CV_8UC4,m_qt_img.bits()); 
			}


			qFatal(U8c("��֧������ͼ���ʽ������"));
		}break;

		
		
	}

}









