#pragma once
#include <string>
#include <QString>
#include <vector>
#include <QObject>


#include "CBITableDef.h"
#include "DrawingDef.h"
#include "DrawingType.h"


class QString;
class CBI_TableDictionary;


//����һ��ʶ����������������ݵ����洢�ṹ,�½�project�����½����������
class DrawingBook : public QObject,  public DrawingAbstruct //һ��������
{
	Q_OBJECT
private:
	//ִ��Ocrʶ���ʱ��Ḵ�� �������ֵ��ļ�
	CBI_TableDictionary *m_DictionaryLock; 
	

public:
	volatile bool isDictionary4OCR();   //ΪOCR��׼������ͼƬ,���û��׼������ʼ׼��
	CBI_TableDictionary * getOcrDictionaryLock();  //��ȡ���ݵ��ֵ��ļ�

	DrawingType *  getOcrDwType();  //��ȡ�趨�� ͼֽ��ʽ�ļ�

public:
	QString readPath;    //ͼƬ�ļ�·��-ȡֵ����windows·���淶����""��Ϊ��ֵ
	QString savePath;    //���������·��-ȡֵ����windows·���淶����""��Ϊ��ֵ
	QString StationName; //�������������ʱ�����ơ���ȡʱд��ͼƬ�ļ�������,ͬʱҲ�������п��Ը��ģ����硰�ذ���վ1802��

public:
	CBI_TableDictionary *m_Dictionary;  //�ֵ�
	DrawingType m_DrawingType;  //ͼֽ����
	std::vector<DrawingSheet*> m_Pages; //����������

public:
//	void usingDrawingType(const DrawingType& newType); //ʹ�ö��е�ͼֽ����

	int readRoteType; //�����ʱ�����ת����
	void readBooks(QString Path); //��ȡĿ¼�����а����������ļ�,����_Pages��

public:
	DrawingBook(QObject* parent = nullptr);
	~DrawingBook();

	virtual cv::Mat getMat(int step = 0);
	virtual DrawingBook* getDrawingBook() ;
	virtual DrawingSheet* getDrawingSheet() ;
	virtual DrawingType* getDrawingType() ;
	virtual unsigned char getDrawingTableType() ;

	// void read() = 0;   //��xml�лָ�
	// void write() = 0;  //д��xml��

Q_SIGNALS:
	void ReadMessage(QString Str);
	void ReadNewSheet(DrawingSheet* newSheet);


public:

	void read(const cv::FileNode &node);
	void write(cv::FileStorage &fs) const ;
};



extern void read(const cv::FileNode& node, DrawingBook& x,const DrawingBook& default_value = DrawingBook());

extern void write(cv::FileStorage& fs, const std::string& name, const DrawingBook& x);




