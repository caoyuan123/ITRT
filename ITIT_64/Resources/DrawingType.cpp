#include "DrawingType.h"
#include <string>
#include "MessyCodeTerminator.h"

DrawingType::DrawingType()
{	
	memset(HeaderTable,-1,sizeof(int) * 50);
	for (int i =0;i < 50;i++)
	{
		HeaderTableRoit[i] = -1;
	}



	saveType = 1;

	_FontProprety = FontWordProprety();
	small_FontProprety = FontWordProprety();
	_GraphicTransTemp = FontGraphicTransTemp();

	TableEdition = 1992;
	lineNums = 36;

	wordHightRoit = 0.05;
	lineHightRoit = 0.05;

	RowNum = 10;

	stdWordHeight = 0;
	stdWordWidth = 0;
}


std::string DrawingType::toString()
{
	return "";
}

void DrawingType::read(const cv::FileNode &node)
{
	if ( node["Type"].isInt())
	{
		node["Type"] >> saveType;
	}
	else
	{
		saveType = 0;
	}

	node["DesignInstitute"] >> strDesignInstitute;
	node["LineName"] >> strLineName;

	node["FontProprety"] >> _FontProprety;
	node["small_FontProprety"] >> small_FontProprety;
	node["GraphicTransTemp"] >> _GraphicTransTemp;

	node["stdWordHeight"] >> stdWordHeight;
	node["stdWordWidth"] >> stdWordWidth;

	node["TableEditon"] >> TableEdition;
	node["LineNums"] >> lineNums;
	node["WordHightRoit"] >> wordHightRoit;
	node["LineHightRoit"] >> lineHightRoit;

	node["RowNum"] >> RowNum;

	cv::FileNode fn2 = node["HeaderTable"];
	if (fn2.isSeq())
	{
		memset(HeaderTable,-1,sizeof(int) * 50);
	
		cv::FileNodeIterator it = fn2.begin();
		cv::FileNodeIterator it_E = fn2.end();

		int idx = 0;
		for (;it != it_E && idx < 50;++it,++idx)
		{
			(*it) >> HeaderTable[idx];
		}
	}

	fn2 = node["HeaderTableRoit"];
	if (fn2.isSeq( ))
	{
		for (int i =0;i < 50;i++)
		{
			HeaderTableRoit[i] = -1;
		}

		cv::FileNodeIterator it = fn2.begin();
		cv::FileNodeIterator it_E = fn2.end();

		int idx = 0;
		for (;it != it_E && idx < 50;++it,++idx)
		{
			(*it) >> HeaderTableRoit[idx];
		}
	}
}

void DrawingType::write(cv::FileStorage &fs) const
{
	fs << "{";
	{
		fs << "Type" << saveType;

		fs << "DesignInstitute" << strDesignInstitute;
		fs << "LineName" << strLineName;

		fs << "FontProprety" << _FontProprety;
		fs << "small_FontProprety" << small_FontProprety;
		fs << "GraphicTransTemp" << _GraphicTransTemp;

		fs << "stdWordHeight" << stdWordHeight;
		fs << "stdWordWidth" << stdWordWidth;

		fs << "TableEditon" << TableEdition;
		fs << "LineNums" << lineNums;
		fs << "WordHightRoit" << wordHightRoit;
		fs << "LineHightRoit" << lineHightRoit;

		fs << "RowNum" << RowNum;

		fs << "HeaderTable" << "[";
		{
			for (int i = 0;i < 50;i++)
			{
				if (HeaderTable[i] < 0)
				{
					break;
				}
				fs << HeaderTable[i];
			}
		}
		fs << "]";


		fs << "HeaderTableRoit" << "[";
		{
			for (int i = 0;i < 50;i++)
			{
				if (HeaderTableRoit[i] < 0)
				{
					break;
				}
				fs << HeaderTableRoit[i];
			}
		}
		fs << "]";
	}
	fs << "}";
}



void read(const cv::FileNode& node, DrawingType& x,const DrawingType& default_value /*= DrawingType()*/)
{
	if (node.empty( ))
	{
		x = default_value;
	}else
	{
		x.read(node);
	}
}

void write(cv::FileStorage& fs, const std::string& name, const DrawingType& x)
{
	x.write(fs);
}



////////////////////////////////////////////////////////////////////

DrawingTypeList::DrawingTypeList( )
{
	read( );
}

DrawingTypeList::~DrawingTypeList( )
{
	save( );
}


void DrawingTypeList::read( )
{
	//update! 增加backup功能


	//读取固定路径 生成QVector中的内容
	cv::FileStorage fs(DtPath,cv::FileStorage::READ);
	cv::FileNode fn = fs["DrawingTypes"];
	
	this->clear( );

	if (fn.isSeq( ))
	{
		cv::FileNodeIterator it = fn.begin( );
		cv::FileNodeIterator it_E = fn.end( );

		for (  ;it != it_E ; it++ )
		{
			DrawingType dt;
			(*it) >> dt;
			
			this->push_back(dt);
		}

	}
	//


}



void DrawingTypeList::save()
{
	cv::FileStorage fs(DtPath,cv::FileStorage::WRITE);

	fs << "DrawingTypes" << "[";

	int sz = this->size();
	for (int i = 0;i < sz;++i)
	{
		fs << this->at(i);
	}
	fs << "]";

	fs.release();
}


void DrawingTypeList::append(const DrawingType& dt)
{
	this->push_back(dt);


	/*
	cv::FileStorage fs(DtPath,cv::FileStorage::APPEND);

	fs << (dt);

	fs.release();*/
}

bool DrawingTypeList::checkRepeatName(QString designName,QString lineName)
{
	int sz = size( );
	for (int i =0;i < sz ;i++)
	{
		if ( U8c(at(i).strDesignInstitute.data( )) == designName
			&& U8c(at(i).strLineName.data( )) == lineName)
		{
			return true;
		}
	}
	
	return false;
}




