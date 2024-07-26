#include "CvLine.h"
#include <qline.h>
#include <QtGui/QtGui>
#include <opencv.hpp>
#include "StdAfx.h"
#include "MessyCodeTerminator.h"
#include <QMessageBox>

//�ѿ���˫�� -> ������ :��Զ���Theta�Ǵ���0�ļ�������(�ͻ�������Ҫ��һ����ʽ)
void CvLine::Descartes2Polar(CvLine src,double &rho,double &theta)
{
	double X = src.Point2.x - src.Point1.x ;
	double Y = src.Point2.y - src.Point1.y;

	theta = std::atan2(X,-Y);    //cv������˳ʱ��Ϊ����
	//	theta = std::atan(X / Y);

	rho =  (X * src.Point2.y - Y * src.Point2.x) / std::sqrt( X * X + Y * Y);

	if (theta < 0)
	{
		theta += PI;
		rho = -rho;
	}

	if (theta > PI)    //����������
	{
		if (theta - PI < minFloatAcc)
		{
			theta = 0;
			rho = -rho; //����
		}else
		{
			theta -= PI;
		}
	}
}

bool CvLine::isSimilarLine(CvLine Line1,CvLine Line2,double theta /*= 0.00865*/,double rho /*= 20*/)
{
	double theta1,rho1;
	double theta2,rho2;

	Descartes2Polar(Line1,rho1,theta1);
	Descartes2Polar(Line2,rho2,theta2);

	PolarPostiveRho(rho1,theta1);
	PolarPostiveRho(rho2,theta2);

	if (diffAngleTheta(theta1 , theta2) < theta && abs(rho1 - rho2) < rho)  //2������ ��಻����20 ��Ϊ���ٽ�ֱ��
	{
		return true;
	}
	return false;
}

bool similarPoint(cv::Point p1,cv::Point p2)
{
	if (abs(p1.x - p2.x) > 3)
	{
		return false;
	}

	if (abs(p1.y - p2.y) > 3)
	{
		return false;
	}

	return true;
}


//������ֱ�� ת �ѿ�������ֱ�� theta�ǻ���
CvLine CvLine::Polar2Descartes(const CvSize &Src, double rho, double theta)
{
	if (rho < 0)
	{
		theta -= PI;
		rho = -rho;
	}
	//���ĸ��ߵĽ��ӵ� �ߵ�˳���� ����x Ȼ�� y
	CvPoint pt1 = cvPoint(rho / cos(theta),0);
	CvPoint pt2 = cvPoint(0,rho / sin(theta));
	CvPoint pt3 = cvPoint(Src.width,(rho - Src.width * cos(theta)) / sin(theta));
	CvPoint pt4 = cvPoint((rho - Src.height * sin(theta)) / cos(theta) ,Src.height);

	int Rt = 0;   //��Ч��
	cv::Point p[4];

	p[0] = cv::Point(-1,-1);
	p[1] = cv::Point(-1,-1);
	p[2] = cv::Point(-1,-1);
	p[3] = cv::Point(-1,-1);

	if (pt1.x >= 0 && pt1.x <= Src.width)
	{
		p[Rt] = pt1;
		Rt ++;
	}

	if (pt2.y >= 0 && pt2.y <= Src.height)
	{
		p[Rt] = pt2;
		Rt ++;
	}

	if (pt4.x >= 0 && pt4.x <= Src.width)
	{
		p[Rt] = pt4;
		Rt ++;
	}

	if (pt3.y >= 0 && pt3.y <= Src.height)
	{
		p[Rt] = pt3;
		Rt ++;
	}

	if (Rt == 2)  //ֱ���뻭ͼӦ��ֻ����������
	{
		return CvLine(p[0],p[1]);
	}else if (Rt >= 3)
	{
		//������������ұ߽��������������,����Ӧ�����ظ�ȡ�ظ�������֮һ
		if (similarPoint(p[0] , p[1]))
		{
			return CvLine(p[0],p[2]);
		}else if (similarPoint(p[2] , p[0]))
		{
			return CvLine(p[0],p[1]);
		}
		else if (similarPoint(p[1] , p[2]))
		{
			return CvLine(p[0],p[1]);
		}else
		{
			//���⴦�� ��theta�� Ϊ0��ʱ ���ǿ����߽�Ļ� ���ײ��ߵ��´���
			if (abs(theta) < 0.02 || abs(theta - PI) < 0.02)
			{
				//Ӧ����X����������, Y��ѡ���������������

			//	QMessageBox::warning(nullptr,"!!","|||||||||");

				if (p[0].y > p[1].y)
				{
					if (p[1].y > p[2].y)
					{
						return CvLine(p[2],p[0]);
					}else if(p[0].y > p[2].y)
					{
						return CvLine(p[1],p[0]);
					}else
					{
						return CvLine(p[1],p[2]);
					}

				}else
				{
					if (p[0].y > p[2].y)
					{
						return CvLine(p[2],p[1]);
					}else if (p[1].y > p[2].y)
					{
						return CvLine(p[0],p[1]);
					}else
					{
						return CvLine(p[0],p[2]);
					}

				}


			}


			//���⴦�� ��theta�� Ϊ +/-90��ʱ ���ǿ����߽�Ļ� ���ײ��ߵ��´���
			if (abs(theta + PI/2) < 0.02 || abs(theta - PI/2) < 0.02)
			{
			//	QMessageBox::warning(nullptr,"!!","------------");
				//Ӧ����Y����������, X��ѡ���������������
				if (p[0].x > p[1].x)
				{
					if (p[1].x > p[2].x)
					{
						return CvLine(p[2],p[0]);
					}else if(p[0].x > p[2].x)
					{
						return CvLine(p[1],p[0]);
					}else
					{
						return CvLine(p[1],p[2]);
					}

				}else
				{
					if (p[0].x > p[2].x)
					{
						return CvLine(p[2],p[1]);
					}else if (p[1].x > p[2].x)
					{
						return CvLine(p[0],p[1]);
					}else
					{
						return CvLine(p[0],p[2]);
					}

				}
			}

			if (abs(theta + PI/4) < 0.02 || abs(theta - PI/4) < 0.02)
			{
				qWarning( ) << U8c("�Խ���3?");
				return CvLine(cvPoint(0,0),cvPoint(Src.width,Src.height));
			}

#if _DEBUG
			qFatal("Polar2Descartes Cal ERROR !!!");
#endif

			
			CvLine rt = CvLine(cvPoint(0,0),cvPoint(0,0));
			rt.checked = true;
			return rt;
		}

	}else if (Rt == 0 || Rt == 1)
	{
		//û���ཻ�ķ��� ����
		
		CvLine rt = CvLine(cvPoint(0,0),cvPoint(0,0));
		rt.checked = true;
		return rt;
	}else
	{
		qWarning( ) << U8c("�Խ���4?");
		return CvLine(cvPoint(0,0),cvPoint(Src.width,Src.height));
	}

#if _DEBUG
	qFatal("Polar2Descartes Cal ERROR !!!");
#endif
	CvLine rt = CvLine(cvPoint(0,0),cvPoint(0,0));
	rt.checked = true;
	return rt;
}

CvLine::CvLine()
{
	Point1.x = 0;
	Point1.y = 0;

	Point2.x = 0;
	Point2.y = 0;

	checked = false;
}

CvLine::CvLine(const CvPoint &p1,const CvPoint &p2)
{
	Point1 = p1;
	Point2 = p2;
	checked = false;
}

CvLine::CvLine(const QLine& line)
{
	CvLine(cvPoint(line.p1().x(),line.p1().y()),cvPoint(line.p2().x(),line.p2().y()));
	checked = false;
}

CvLine::CvLine(const CvLine& line)
{
	Point1 = line.Point1;
	Point2 = line.Point2;
	checked = line.checked;
}

CvLine::CvLine(const CvSize &Src,const double &rho,const double &theta)
{
	*this = Polar2Descartes(Src, rho,theta);
	checked = false;
}

CvLine::CvLine(const int p1x,const int p1y,const int p2x,const int p2y)
{

	Point1.x = p1x;
	Point1.y = p1y;
	Point2.x = p2x;
	Point2.y = p2y;

	checked = false;
}

CvLine::CvLine(const QLineF& line)
{
	Point1.x = line.p1().x();
	Point1.y = line.p1().y();
	Point2.x = line.p2().x();
	Point2.y = line.p2().y();

	checked = false;
}

QLine CvLine::getLine(void)
{
	return QLine(Point1.x,Point1.y,Point2.x,Point2.y);
}

void CvLine::getPloar(double &rho,double& theta)
{
	Descartes2Polar( *this, rho, theta);
}

inline void CvLine::PolarPostiveRho(double &rho,double &theta)
{
	if (rho < 0)
	{
		rho = -rho;

		if (theta < 0)  
		{
			theta += PI ;
		}else if (abs(theta - 0)  < minFloatAcc || abs(theta - 2 * PI) < minFloatAcc)
		{
			theta = 0;
		}else
		{
			theta -= PI ;
		}
	}
}

bool CvLine::CompareY(const CvLine &L1,const CvLine &L2)
{
	//1.���߲��и߶ȱȽϵ����� ,�Ǻ��߷���0
	double R1 = 0,T1 = 0;
	double R2 = 0,T2 = 0;

	Descartes2Polar(L1,R1,T1);
	Descartes2Polar(L2,R2,T2);

	double dT1 = T1;  //��Ϊ������ж��ǰ�Theta��Զ��д��
	double dT2 = T2;

	//ͬ��������߲��бȽ�����
	PolarPostiveRho(R1,T1);
	PolarPostiveRho(R2,T2);

	

	if ( abs(T1-T2) < PI / 18) //���߼н����10������
	{
		if ((dT1 >= PI / 4 && dT1 <= PI * 3 / 4) && (dT2 >= PI / 4 && dT2 <= PI * 3 / 4))
		{
			if (R1 > R2)
			{
				return false;
			}else
			{
				return true;
			}

		}else
		{
			qFatal(U8c("CompareY�����⣡��"));
			return false;
		}

	}
	else
	{
		qFatal(U8c("CompareY�����⣡��"));
		return false;
	}


}

bool CvLine::CompareX(const CvLine &L1,const CvLine &L2)
{
	//1.���߲���X��Ƚϵ����� ,�Ǻ��߷���0
	double R1 = 0,T1 = 0;
	double R2 = 0,T2 = 0;

	Descartes2Polar(L1,R1,T1);
	Descartes2Polar(L2,R2,T2);


	double dT1 = T1;  //��Ϊ������ж��ǰ�Theta��Զ��д��
	double dT2 = T2;

	//��rhoͬʱת�����Ĳ��бȽ�����
	PolarPostiveRho(R1,T1);
	PolarPostiveRho(R2,T2);


	if (abs(T1-T2) < PI / 18) //���߼н����10������
	{
		if ((dT1 <= PI / 4 || dT1 >= PI * 3 / 4) && (dT2 <= PI / 4 || dT2 >= PI * 3 / 4))
		{

			if (R1 > R2)
			{
				return false;
			}else
			{
				return true;
			}


		}else
		{
			qFatal(U8c("CompareX�����⣡��"));
			return false;
		}

	}
	else
	{
		qFatal(U8c("CompareX�����⣡��"));
		return false;
	}

}

bool CvLine::isHorizontal()
{
	double r = 0,t = 0;

	getPloar(r,t);


	if (t < 0.75 * PI && t > 0.25 * PI)
	{
		return true;
	}else
	{
		return false;
	}
}

void CvLine::read(const cv::FileNode &node)
{
	
	cv::Point p1,p2;

	node["Point1"] >> p1;
	node["Point2"] >> p2;

	Point1 = p1;
	Point2 = p2;
}

void CvLine::write(cv::FileStorage &fs) const
{
	
	fs << "{:"; 
	{
		fs << "Point1" << cv::Point(Point1) ;
		fs << "Point2" << cv::Point(Point2) ;
	}
	fs << "}";
}

double CvLine::diffAngleTheta(const double &Theta1,const double &Theta2)
{
	double diff = Theta1 - Theta2;
	
	//����ֵת��Ϊ ���Ƕ�
	while(diff < -0.000000001)
	{
		diff += 2 * PI;   
	}

	while(diff - 2 * PI > 0.000000001)
	{
		diff -= 2 * PI;    
	}

	//2PI��0��Χ ����0
	if (abs(diff - 2 * PI) < minFloatAcc || abs(diff) < minFloatAcc)
	{
		return 0;
	}

	double dif = 0;
	
	if (diff > PI)  //��ǵ���СȦ
	{
		dif =  2* PI - diff;  
	}else
	{
		dif = diff;
	}

	//������С�н���˵ ��Զ�����ڼн�
	if (dif > PI /2)
	{
		return dif - PI / 2;

	}else
	{
		return dif;
	}

}





void read(const cv::FileNode& node, CvLine& x,const CvLine& default_value /*= CvLine()*/)
{
	if (node.empty())
	{
		qWarning("CvLine Read Empty Warning!!");
		x = default_value;
	}else
	{
		x.read(node);
	}
}

void write(cv::FileStorage& fs, const std::string& name, const CvLine& x)
{
	x.write(fs);
}
