
#include "DrawingPart.h"
#include "DrawingSheet.h"

void DrawingPart::write(cv::FileStorage &fs) const
{
	
	fs << "{";

	fs << "step" << m_step;  //步骤
	
	fs << "PartNum" << PartNum;
	fs << "StorageName" << StorageName;   //保存文件的完整路径


	
	int s = fltPoints.size();  //应该是只有4个点的滤波器参数
	fs << "ctlPoint" <<"[";
	for (int i =0;i < s;i++)
	{
		fs << fltPoints[i];
	}
	fs << "]";


	fs << "effectiveArea0" << effectiveArea[0];
	fs << "effectiveArea1" << effectiveArea[1];
	fs << "effectiveArea2" << effectiveArea[2];
	fs << "effectiveArea3" << effectiveArea[3];

	//fs << "matOrg" << matOrg;     //只存原始图片

	//fs << "matCorrect" << matCorrect;
	fs << "matFlt" << matFlt;   //其余的不重要了
	//fs << "matFinal" << matFinal;
	
	fs << "}";

}

void DrawingPart::read(const cv::FileNode &node)
{
	
	node["step"] >> m_step;

	node ["PartNum"]  >> PartNum;
	node ["StorageName"]  >> StorageName;

	//少一个读滤波参数
	cv::FileNode fn2 = node["ctlPoint"];



	if (fn2.isSeq())
	{
		fltPoints.clear();
		cv::FileNodeIterator it = fn2.begin();
		cv::FileNodeIterator it_E = fn2.end();

		int idx = 0;
		for (;it != it_E;++it,++idx)
		{
			cv::Point p;
			(*it) >> p;
			fltPoints.push_back(p);
		}

	}

	node ["effectiveArea0"] >> effectiveArea[0];
	node ["effectiveArea1"] >> effectiveArea[1];
	node ["effectiveArea2"] >> effectiveArea[2];
	node ["effectiveArea3"] >> effectiveArea[3];

	

	/*调试用所以是这样的*/
//	node  ["matOrg"] >> matOrg;
	node  ["matFlt"] >> matFlt;
//	node  ["matFinal"] >> matFinal;
}




cv::Mat DrawingPart::getFinalMat(int _step)
{
	switch(_step)
	{
	case 0:
		{
			/*
			if ((m_step & 12) == 12)
			{
				return matFinal;
			}*/

			if ((m_step & 8) == 8)
			{
				return matFlt;
			}

			if ((m_step & 4) == 4)
			{
				return matCorrect;
			}

			if ((m_step & 1) == 1)
			{
				return matOrg;
			}
		}break;

	case 1: return matOrg; break;
	case 2: return matCorrect; break;
	case 3: return matFlt; break;
	case 4: return matFinal; break;
	
	default:
		{
			return cv::noArray().getMat();
		}
	}

	

	return cv::noArray().getMat();
}

int DrawingPart::getStep()
{
	return m_step;
}

bool DrawingPart::isReadMat()
{
	return m_step & 0x0001 ? true : false;
}

bool DrawingPart::isFilter()
{
	return m_step & 11 ? true : false;  //1 + 2 +8
}

bool DrawingPart::isCorrect()
{
	return m_step & 5 ? true : false; 
}

bool DrawingPart::isColored()
{
	return m_step & 3 ? true : false;   //1 + 2
}

void DrawingPart::setOrgMat( const cv::Mat& _inputArray,std::string Name,int _PartNum)
{
	matOrg = _inputArray;
	StorageName = Name;
	PartNum = _PartNum;

	m_step = 1;

}


DrawingPart::DrawingPart(DrawingSheet* Parent)
{
	m_step = 0;
	Parent_Ds = Parent;
	
}

void DrawingPart::setImgProcFlt(cv::InputArray _Flt)
{
	matFlt = _Flt.getMat();

	m_step |= 2;
	m_step |= 8;

}

cv::Mat DrawingPart::getMat(int step)
{
	return getFinalMat(step);
}

DrawingBook* DrawingPart::getDrawingBook()
{
	return getDrawingSheet()->getDrawingBook();
}

DrawingSheet* DrawingPart::getDrawingSheet()
{
	return Parent_Ds;
}

DrawingType* DrawingPart::getDrawingType()
{
	return getDrawingSheet()->getDrawingType();
}

unsigned char DrawingPart::getDrawingTableType()
{
	return getDrawingSheet()->getDrawingTableType();
}

void DrawingPart::setImgProcCorrect(CvPoint Area[4],cv::InputArray _Crt)
{
	if (Area != nullptr)
	{
		effectiveArea[0] = Area[0];
		effectiveArea[1] = Area[1];
		effectiveArea[2] = Area[2];
		effectiveArea[3] = Area[3];

		Parent_Ds->flags_Process.setProcess(DwSt_ProcFlags_矫正,true);
		matCorrect = _Crt.getMat();
		m_step |= 4;
	}else
	{
		effectiveArea[0] = cv::Point(0,0);
		effectiveArea[1] = cv::Point(0,0);
		effectiveArea[2] = cv::Point(0,0);
		effectiveArea[3] = cv::Point(0,0);

		Parent_Ds->flags_Process.setProcess(DwSt_ProcFlags_矫正,false);
		matCorrect = cv::noArray().getMat();
		m_step &= 0x0B;
	}
}

void DrawingPart::setImgProcCorrect(cv::Point Area[4],cv::InputArray _Crt)
{
	if (Area != nullptr)
	{
		effectiveArea[0] = Area[0];
		effectiveArea[1] = Area[1];
		effectiveArea[2] = Area[2];
		effectiveArea[3] = Area[3];

		Parent_Ds->flags_Process.setProcess(DwSt_ProcFlags_矫正,true);
		matCorrect = _Crt.getMat();
		m_step |= 4;
	}else
	{
		effectiveArea[0] = cv::Point(0,0);
		effectiveArea[1] = cv::Point(0,0);
		effectiveArea[2] = cv::Point(0,0);
		effectiveArea[3] = cv::Point(0,0);

		Parent_Ds->flags_Process.setProcess(DwSt_ProcFlags_矫正,false);
		matCorrect = cv::noArray().getMat();
		m_step &= 0x0B;
	}
}

void DrawingPart::setImgProcFilter(std::vector<cv::Point> _fltPoints ,cv::InputArray _Crt)
{
	fltPoints = _fltPoints;

	if (_fltPoints.size() > 0)
	{
		fltPoints = _fltPoints;

		Parent_Ds->flags_Process.setProcess(DwSt_ProcFlags_滤波,true);
		matFlt = _Crt.getMat();
		m_step |= 8;

	}else
	{
		fltPoints.clear();

		Parent_Ds->flags_Process.setProcess(DwSt_ProcFlags_滤波,false);
		matFlt = cv::noArray().getMat();

		m_step &= 0x08;
	}
}


void read(const cv::FileNode& node, DrawingPart& x,const DrawingPart& default_value /*= DrawingPart(nullptr)*/)
{
	if (node.empty())
	{
		qWarning("DrawingPart Node读取空");
		x = default_value;
	}else
	{
		x.read(node);
	}
}

void write(cv::FileStorage& fs, const std::string& name, const DrawingPart& x)
{
	x.write(fs);
}
