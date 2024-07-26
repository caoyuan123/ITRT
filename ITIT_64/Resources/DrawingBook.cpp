#include "DrawingBook.h"
#include "DrawingSheet.h"
#include "GlobalSettingQt.h"
#include <QFile>
#include <QDir>
#include <QtGui/QtGui>
#include <QString>
#include "CBI_TableDictionary.h"

#include "MessyCodeTerminator.h"

DrawingBook::DrawingBook(QObject* parent) 
	: QObject(parent)

{
	m_DictionaryLock = nullptr;
	m_Dictionary = new CBI_TableDictionary(this);

	readRoteType = 0;
}



DrawingBook::~DrawingBook()
{
	for (int i =0;i < m_Pages.size() ;i ++)
	{
		delete(m_Pages[i]);
	}

	if (m_DictionaryLock != nullptr)
	{
		m_DictionaryLock->setWriteLock(false);
		delete(m_DictionaryLock);
	}

	delete(m_Dictionary);
}

void DrawingBook::readBooks(QString Path)
{
	//打开文件路径
	readPath = Path;
	savePath = Path;


	QDir dir(readPath);
	qint64 size = 0;

	//u_setGlobal.readType;

	DrawingSheet* newSheet = nullptr;

	//清理完事
	for (int i =0;i < m_Pages.size();i++)
	{
		delete(m_Pages[i]);
	}

	m_Pages.clear();

	//检查该路径下所有符合的后缀的名称
	QString Str;   //前半部图像名字

	emit(ReadMessage(U8s("开始读取")));
	Q_FOREACH(QString file,dir.entryList(u_setGlobal.readType,QDir::Files))
	{
		//检查名称格式
		if (u_setGlobal.SpliterOpen)  //一次读各个part
		{
			qFatal(U8c("分开读取未编写完成"));
			
			//拆.前的字符串
			QStringList Flist = file.split(".");

			//在名称中拆出来带分隔字符的算成是一列
			//检查
			//

			if (Flist[0].contains(u_setGlobal.SplitChar))
			{

			}
			else
			{
				newSheet = new DrawingSheet(this);
			}

			QString str = file;
		}else
		{
			emit(ReadMessage(U8s("当前读取：") + file));

			newSheet = new DrawingSheet(this);
			newSheet->ReadOne(file.toStdString(),m_Pages.size());
			m_Pages.push_back(newSheet);

			emit(ReadNewSheet(newSheet));
		}
	}

	emit(ReadMessage(U8s("读取完毕!")));
}

cv::Mat DrawingBook::getMat(int step)
{
	return cv::noArray().getMat();  //确实没有
}

DrawingBook* DrawingBook::getDrawingBook() 
{
	return this;
}

DrawingSheet* DrawingBook::getDrawingSheet() 
{
	return nullptr;
}

DrawingType* DrawingBook::getDrawingType() 
{
	return &m_DrawingType;
}

unsigned char DrawingBook::getDrawingTableType() 
{
	return m_DrawingType.TableEdition;
}

void DrawingBook::read(const cv::FileNode &node)
{
	std::string str = node["readPath"];
	readPath = str.data();

	str = node["savePath"];
	savePath = str.data();

	str = node["StationName"];
	StationName = str.data();

	node["Dictionary"] >> *m_Dictionary;
	node["DrawingType"] >>  m_DrawingType;

	cv::FileNode fn2 = node["DrawingSheets"];

	if (fn2.isSeq())
	{
		for (int i =0;i < m_Pages.size();i++)
		{
			if (m_Pages[i] != nullptr)
			{
				delete(m_Pages[i]);
				i--;
			}
		}
		m_Pages.clear();


		cv::FileNodeIterator it = fn2.begin();
		cv::FileNodeIterator it_E = fn2.end();

		int idx = 0;

		for (;it != it_E;++it,++idx)
		{
			DrawingSheet *p = new DrawingSheet(this);
			(*it) >> *p;

			p->setParent(this);
			m_Pages.push_back(p);
		}
	}
	
}

void DrawingBook::write(cv::FileStorage &fs) const
{
	fs << "{";
	{

		fs << "readPath" << readPath.toStdString();
		fs << "savePath" << savePath.toStdString();
		fs << "StationName" << StationName.toStdString();

		fs << "Dictionary" << *m_Dictionary;

		fs << "DrawingType" << m_DrawingType;
		
		fs << "DrawingSheets" << "[";
		int s = m_Pages.size();
		for(int i =0 ;i < s ;i ++)
		{
			fs <<*(m_Pages[i]);
		}
		fs << "]";


	}
	fs << "}";
}

volatile bool DrawingBook::isDictionary4OCR()
{
	if (m_DictionaryLock != nullptr)
	{
		if (m_DictionaryLock->getWriteLock())
		{
			qWarning(U8c("字典正在被读写"));
			return false;
		}else
		{
			 
			if (m_Dictionary->isFontSync()) //是否 和 锁之前有更改项目
			{
				return true;  //没有变化的话
			}else
			{
				delete(m_DictionaryLock);
			}

		}
	}
	else ;

	m_Dictionary->slot_GenerateFontPicture();  //主字典生成图片 然后Lock,深拷贝一次

	m_DictionaryLock = new CBI_TableDictionary( *m_Dictionary ,true);
	
	return true;  
}

CBI_TableDictionary* DrawingBook::getOcrDictionaryLock()
{
	return m_DictionaryLock;
}

void read(const cv::FileNode& node, DrawingBook& x,const DrawingBook& default_value /*= DrawingBook()*/)
{
	if (node.empty())
	{
		qFatal(U8c("读取失败!!!"));

		//x = default_value;
	}else
	{
		x.read(node);
	}
}

void write(cv::FileStorage& fs, const std::string& name, const DrawingBook& x)
{
	x.write(fs);
}
