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
		_GraphicInterFace = new ImgProcFilter(); //调试用的
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

	m_colored = true;  //默认彩色模式
	debugShow = false;

	if (zoomMode == 2) 
	{
		zoomResize(1); //触发一下：需要算出那个大图才行
	}

	this->setBaseSize(m_qt_img.size());
	this->setSizeIncrement(1,1);
	this->resize(m_qt_img.size());

	_GraphicInterFace->drawMat();  //写入

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
// Qualifier: 拷贝cv格式图像至qt格式图像,就是CV与QT接口函数
// Parameter: void
//************************************
void GuiCvFactory::_copyImage(void)//拷贝cv格式图像至qt格式图像
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
			else  //黑白模式
			{
				cv::cvtColor(ReSizeTemp,m_cv_img,cv::COLOR_GRAY2BGRA);
			}
			
		}break;
	case 2:
		{
			if (RoiMove)  //如果是ROI移动不需要更新大图
			{
				RoiMove = false;
			}else
			{

				cv::resize(_GraphicInterFace->showMat(),ReSizeTemp,
					m_cv_img.size());

				if (m_colored)  
				{
					cv::cvtColor(ReSizeTemp,m_cv_img,cv::COLOR_BGR2BGRA);
				}//黑白模式不需要转换
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


	if (e->buttons() != Qt::MidButton)   //非中键拖拽时
	{
		if (_GraphicInterFace->mouseMove(x,y))
		{
			_GraphicInterFace->drawMat();  //写入
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
			_GraphicInterFace->drawMat();  //写入
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
			_GraphicInterFace->drawMat();  //写入
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
			//获取需要画图的地方
			m_qt_img = QImage(QSize(newWidth,newHeight),QImage::Format_ARGB32);
			//创建一个cv图像头的qt图片
			m_cv_img = cv::Mat (cv::Size(newWidth,newHeight),CV_8UC4,m_qt_img.bits());
		}break;
	case QtRoiZoom: 
		{
			//获取需要画图的地方
			m_qt_img = QImage(QSize(newWidth,newHeight),QImage::Format_ARGB32);
			//创建一个cv图像头的qt图片
			m_cv_img = cv::Mat (cv::Size(newWidth,newHeight),CV_8UC4,m_qt_img.bits());
		
		}break;
	case CvRoiZoom:
		{
			//焦点放大模式下 画图是窗口大小
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
	case 0: break;  //不不能放大

	case 1:    //跟随放大
		{
			zoomRoit =e->size().height() / (double)_GraphicInterFace->showMat().size().height  ;
			zoomResize(zoomRoit);
		}break;

	case 2:    //区域放大
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
	if (e->buttons() == Qt::MidButton)  //屏蔽中键
	{
		return;
	}


	switch(zoomMode)
	{
	case 0: break;

	case 1:    //跟随放大
		{
		
		}break;

	case 2:    //区域放大
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

//注意newStep * 0.01倍率
void GuiCvFactory::zoomResizeStep(double newStep)
{
	zoomRoit += 0.01 * newStep;  //自带正负角度
	zoomResize(zoomRoit);
}

void GuiCvFactory::setZoomMode(int Mode)
{
	if (Mode <= 2 && Mode >= 0)
	{
		zoomMode = Mode;
	}else
	{
		qFatal(U8c("程序入参错误"));
	}
	
}

//第二个只显示的窗口
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
	case 1:   //黑白图片 
		{
			if (m_colored || force)
			{
				m_colored = false;  //黑白模式

				m_qt_img = QImage(QSize(temp.cols,temp.rows),QImage::Format_ARGB32);
				m_cv_img = cv::Mat (cv::Size(m_qt_img.width(),m_qt_img.height()),CV_8UC4,m_qt_img.bits()); 
			}
		}break;
	case 3:
		{
			if (!m_colored || force)
			{
				m_colored = true;  //彩色模式

				m_qt_img = QImage(QSize(temp.cols,temp.rows),QImage::Format_ARGB32);
				m_cv_img = cv::Mat (cv::Size(m_qt_img.width(),m_qt_img.height()),CV_8UC4,m_qt_img.bits()); 
			}
		}break;
	case 4:  //彩色图片 
		{
			if (!m_colored || force)
			{
				m_colored = true;  //彩色模式

				m_qt_img = QImage(QSize(temp.cols,temp.rows),QImage::Format_ARGB32);
				m_cv_img = cv::Mat (cv::Size(m_qt_img.width(),m_qt_img.height()),CV_8UC4,m_qt_img.bits()); 
			}


		}break;
	default:
		{
			if (!m_colored || force)
			{
				m_colored = true;  //彩色模式

				m_qt_img = QImage(QSize(temp.cols,temp.rows),QImage::Format_ARGB32);
				m_cv_img = cv::Mat (cv::Size(m_qt_img.width(),m_qt_img.height()),CV_8UC4,m_qt_img.bits()); 
			}


			qFatal(U8c("不支持这种图像格式的输入"));
		}break;

		
		
	}

}









