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
	//���ļ�·��
	readPath = Path;
	savePath = Path;


	QDir dir(readPath);
	qint64 size = 0;

	//u_setGlobal.readType;

	DrawingSheet* newSheet = nullptr;

	//��������
	for (int i =0;i < m_Pages.size();i++)
	{
		delete(m_Pages[i]);
	}

	m_Pages.clear();

	//����·�������з��ϵĺ�׺������
	QString Str;   //ǰ�벿ͼ������

	emit(ReadMessage(U8s("��ʼ��ȡ")));
	Q_FOREACH(QString file,dir.entryList(u_setGlobal.readType,QDir::Files))
	{
		//������Ƹ�ʽ
		if (u_setGlobal.SpliterOpen)  //һ�ζ�����part
		{
			qFatal(U8c("�ֿ���ȡδ��д���"));
			
			//��.ǰ���ַ���
			QStringList Flist = file.split(".");

			//�������в�������ָ��ַ��������һ��
			//���
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
			emit(ReadMessage(U8s("��ǰ��ȡ��") + file));

			newSheet = new DrawingSheet(this);
			newSheet->ReadOne(file.toStdString(),m_Pages.size());
			m_Pages.push_back(newSheet);

			emit(ReadNewSheet(newSheet));
		}
	}

	emit(ReadMessage(U8s("��ȡ���!")));
}

cv::Mat DrawingBook::getMat(int step)
{
	return cv::noArray().getMat();  //ȷʵû��
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
			qWarning(U8c("�ֵ����ڱ���д"));
			return false;
		}else
		{
			 
			if (m_Dictionary->isFontSync()) //�Ƿ� �� ��֮ǰ�и�����Ŀ
			{
				return true;  //û�б仯�Ļ�
			}else
			{
				delete(m_DictionaryLock);
			}

		}
	}
	else ;

	m_Dictionary->slot_GenerateFontPicture();  //���ֵ�����ͼƬ Ȼ��Lock,���һ��

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
		qFatal(U8c("��ȡʧ��!!!"));

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
