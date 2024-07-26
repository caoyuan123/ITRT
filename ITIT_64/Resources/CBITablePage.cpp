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
	default: qFatal("��������ذ汾"); break;
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
		value[num] += "\r\n";  //�Ѿ��ж�����,��ʾͬʱ��������Ҫ����
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
		//update! Ӧ������һ���Ѹ����ָ�Part�ϲ�����Ķ���
		//��һ�׶γ���д �ָ�ϲ�����Part ����Ĭ��ֻ�� _value[0]

		QList<CBITableLine> *ptr =  & _value[0];//�������


		int Num92 = CBITable::Header92[Dz->m_Header];  //�����

		//��dz�е�OCRline����������

		int ocrLineSz = Dz->All_OCRLine.size();
		for (int i = 0;i < ocrLineSz; ++i)
		{
			int Sy = Dz->All_OCRLine[i]->getSheetPosY( );

			int TableLineSize = ptr->size();
			for (int j = 0;j < TableLineSize; ++j)
			{
				if ( (*ptr)[j].checkInclude(Sy) )
				{
					//���� ͬ��
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
	int num = getPartNum( );  //��Part����

	for(int i = 0;i < num; i++)
	{
		QList<CBITableLine> newPart;

		_value.push_back(newPart);
	}

	//һ�����ִ���ģʽ 
	if (s_lineCalType == 1) //1.����ʽ ��ͼ�еĻ���Ԥ�Ⱦ��� ��
	{	
		//��˳���ÿ���ָ�ҳ��ʼ��
		for (int i = 0;i < num;++i)
		{
			//��ʱ��Vline ��Hline ������Sheet��
			//Ӧ���� ������ ��ʱû�������Ŀ��������Ȳ�������
			int sz = Ptr_DrawingSheet->HLines.size();
			for ( int j = 0; j < sz - 1 ; ++j )
			{
				//��ʼ��һ������
				CBITableLine Line(1992,Ptr_DrawingSheet->HLines[j].Point1.y,Ptr_DrawingSheet->HLines[j+1].Point1.y);
				Line.dataLine = i + 1;

				_value[i].push_back(Line);
			}
		}

	}else if (s_lineCalType == 2) //2.����ʽ ��ʵ�ʵ������Ӧ
	{
		qFatal("Error");
	}else
	{
		qFatal("Error");
	}
		
	//��������select��Zone�����д��

	int zoneSZ = Ptr_DrawingSheet->m_ZoneBeSelected.size();
	for (int i = 0;i < zoneSZ;i++)
	{
		setData(Ptr_DrawingSheet->m_ZoneBeSelected[i]);
	}

	//���ɱ�ͷ



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

	if (_value.size()  == 1)  //ֻ��һ��ҳ�� ֱ�����
	{
		for (int i = 0;i < _value[0].size();++i)
		{
			ExcelValue.append(_value[0][i].value.toList());
		}
	}
	else  //���ϲ�ͬҳ���ϵ����ݵ�һ�ű���
	{
		//��ʼ������
		for (int j = 0;j < _value[0].size();++j)
		{
			QVector<QString>  vStr;
			vStr.fill("",Table92Max);  //���15��
			ExcelValue.append(vStr.toList());
		}

		for (int i = 0;i < _value.size();++i)  
		{
			for (int j = 0;j < _value[i].size(); ++j)
			{
				for (int n = 0; n < _value[i][j].value.size();++n)
				{
					ExcelValue[j][n] += _value[i][j].value[n];   //�����ű�Ľ���Ӻ���һ��
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





