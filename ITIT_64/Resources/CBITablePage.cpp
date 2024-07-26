#include "CBITablePage.h"
#include "IfSaveTable.h"
#include "DrawingSheet.h"
#include "DrawingZone.h"
#include "CBITableDef.h"
#include <QtGui/QtGui>
#include <cxcore.h>

using namespace cv;

IfCBITable::IfCBITable()
{



}


IfCBITable::~IfCBITable(void)
{



}



///////////////////////////////////////////////////////////////////
void CBITableLine::init()
{
	switch(TableType)
	{
	case 1992: value.fill("",Table92Max); break;
	case 2018:  break;
	default: qFatal("不存在相关版本"); break;
	}
	
	dataLine = -1;
}

CBITableLine::CBITableLine(int _tableType)
{
	TableType = _tableType;
	init();
	
	posYTop = -1;
	posYDown = -1;
}

CBITableLine::CBITableLine(int _tableType,int yTop,int yDown)
{
	TableType = _tableType;
	init();

	posYTop = yTop;
	posYDown = yDown;
}

void CBITableLine::setValue(int num,QString Str)
{

	if (!value[num].isEmpty())
	{
		value[num] += "\r\n";  //已经有东西了,表示同时有两行需要换行
	}
	value[num] += Str;
}

bool CBITableLine::checkInclude(int y)
{
	if (y >= posYTop && y <= posYDown)
	{
		return true;
	}else
	{
		return false;
	}
}

int CBITableLine::getType()
{
	return TableType;
}


/////////////////////////////////////////////
CBITablePage92::CBITablePage92()
{
	updateValue = true;

	s_lineCalType = 1;

	Ptr_DrawingSheet = nullptr;

}

void CBITablePage92::setData(DrawingZone *Dz)
{
	updateValue = true;

	if (s_lineCalType == 1)
	{
		//update! 应该是有一个把各个分隔Part合并输出的东西
		//第一阶段程序不写 分割合并各个Part 所以默认只有 _value[0]

		QList<CBITableLine> *ptr =  & _value[0];//方便操作


		int Num92 = CBITable::Header92[Dz->m_Header];  //输出列

		//给dz中的OCRline计算数据行

		int ocrLineSz = Dz->All_OCRLine.size();
		for (int i = 0;i < ocrLineSz; ++i)
		{
			int Sy = Dz->All_OCRLine[i]->getSheetPosY( );

			int TableLineSize = ptr->size();
			for (int j = 0;j < TableLineSize; ++j)
			{
				if ( (*ptr)[j].checkInclude(Sy) )
				{
					//允许 同行
					(*ptr)[j].setValue(Num92, Dz->All_OCRLine[i]->toString().data());
					
					break;
				}
			}
		}


	}	
	else if (s_lineCalType == 2)
	{
		qFatal("Error");


	}else
	{
		qFatal("Error");
	}


}

void CBITablePage92::setData(DrawingSheet *_Ds)
{
	updateValue = true;
	Ptr_DrawingSheet = _Ds;
	
	_value.clear( );
	int num = getPartNum( );  //分Part处理

	for(int i = 0;i < num; i++)
	{
		QList<CBITableLine> newPart;

		_value.push_back(newPart);
	}

	//一共两种处理模式 
	if (s_lineCalType == 1) //1.处理方式 按图中的划分预先决定 行
	{	
		//按顺序给每个分隔页初始化
		for (int i = 0;i < num;++i)
		{
			//暂时把Vline 和Hline 放在了Sheet中
			//应该是 逐个抽的 暂时没有这样的考虑所以先不这样了
			int sz = Ptr_DrawingSheet->HLines.size();
			for ( int j = 0; j < sz - 1 ; ++j )
			{
				//初始化一个表行
				CBITableLine Line(1992,Ptr_DrawingSheet->HLines[j].Point1.y,Ptr_DrawingSheet->HLines[j+1].Point1.y);
				Line.dataLine = i + 1;

				_value[i].push_back(Line);
			}
		}

	}else if (s_lineCalType == 2) //2.处理方式 按实际的坐标对应
	{
		qFatal("Error");
	}else
	{
		qFatal("Error");
	}
		
	//遍历所有select的Zone将结果写入

	int zoneSZ = Ptr_DrawingSheet->m_ZoneBeSelected.size();
	for (int i = 0;i < zoneSZ;i++)
	{
		setData(Ptr_DrawingSheet->m_ZoneBeSelected[i]);
	}

	//生成表头



#ifdef _DEBUG
	for (int i = 0;i < _value[0].size();i++)
	{
		printf("L:%d ",i);
		for (int j = 0;j < 15;j++)
		{
			std::string str = _value[0][i].value[j].toStdString();
			printf("%s\t",str.data());
		}
		printf("\r\n");
	}
#endif

}

QList<QVariant> CBITablePage92::getExcelValue()
{

	QVariantList rt;
	if (updateValue)
	{
		calExcelValue();
	}

	for (int i = 0; i < ExcelValue.size();i++)
	{
		rt.append(ExcelValue[i]);
	}

	return rt;
}

int CBITablePage92::getPartNum()
{
	return Ptr_DrawingSheet->_AllDrawingParts.size();
}

void CBITablePage92::calExcelValue()
{
	ExcelValue.clear();

	if (_value.size()  == 1)  //只有一个页面 直接输出
	{
		for (int i = 0;i < _value[0].size();++i)
		{
			ExcelValue.append(_value[0][i].value.toList());
		}
	}
	else  //整合不同页面上的数据到一张表上
	{
		//初始化数量
		for (int j = 0;j < _value[0].size();++j)
		{
			QVector<QString>  vStr;
			vStr.fill("",Table92Max);  //大概15个
			ExcelValue.append(vStr.toList());
		}

		for (int i = 0;i < _value.size();++i)  
		{
			for (int j = 0;j < _value[i].size(); ++j)
			{
				for (int n = 0; n < _value[i][j].value.size();++n)
				{
					ExcelValue[j][n] += _value[i][j].value[n];   //将几张表的结果加和在一起
				}
			}
		}

	}

	updateValue = false;
}

void CBITablePage92::write(cv::FileStorage &fs) const
{
	fs << "{";
	{
		fs << "Line" << "[";
		for (int i = 0;i < ExcelValue.size();++i)		
		{
			fs <<"[";
			for (int j = 0;j < ExcelValue[i].size();++j)
			{
				fs << ExcelValue[i][j].toStdString();
			}
			fs << "]";

		}
		fs << "]";
	}
	fs << "}";
}

void CBITablePage92::read(const cv::FileNode &node)
{
	cv::FileNode Sheet = node["Line"];

	if (Sheet.isSeq())
	{
		ExcelValue.clear();

		cv::FileNodeIterator it = Sheet.begin();
		cv::FileNodeIterator it_E = Sheet.end();

		int idx = 0;
		for (;it != it_E; ++it,++idx)
		{
			cv::FileNode Line = Sheet[idx];

			if (Line.isSeq())
			{
				QList<QString> Lstr;
				cv::FileNodeIterator it2 = Line.begin();
				cv::FileNodeIterator it2_E = Line.end();
			
				for (;it2 != it2_E; ++it2)
				{
					std::string Str;
					(*it2) >> Str;

					Lstr.append(Str.data( ));
				}

				ExcelValue.append(Lstr);
			}

		}
	}

	updateValue = false;
}

int CBITablePage92::getTableType()
{
	return 1992;
}


void read(const cv::FileNode& node, CBITablePage92& x,const CBITablePage92& default_value /*= CBITablePage92( )*/)
{
	if (node.empty())
	{
		x = default_value;
	}else
	{
		x.read(node);
	}
}

void write(cv::FileStorage& fs, const std::string& name, const CBITablePage92& x)
{
	x.write(fs);
}





