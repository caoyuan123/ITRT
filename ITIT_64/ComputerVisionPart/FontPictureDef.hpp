#pragma once

#include <QString>
#include <core\core.hpp>
#include <string>
#include "CBi_TableInterface.h"

class QFont;
class QLabel;
class QImage;

//�����������Լ���
struct FontWordProprety 
{
public:
	QString FontName;  //��������
	int FontNum;      //�ֺ�
	double FontSpacing;  //�ּ��
	double FontCompressRatio; //��ѹ����

	bool FontBold;

	int MarginX; //x��ı߽�
	int MarginY; //y��ı߽�

	//	void* objectOtherUsed;  //�����������;����
public:
	FontWordProprety();

	void setFormQfont(const QFont &font);
	void setQfont(QFont &font) const;  //ͨ��������������Qfont����

	void read(const cv::FileNode &node);
	void write(cv::FileStorage &fs) const;
};


extern void read(const cv::FileNode& node, FontWordProprety& x,const FontWordProprety& default_value = FontWordProprety());
extern void write(cv::FileStorage& fs, const std::string& name, const FontWordProprety& x);


//��������ϸ����λ�õ���ͼ
struct FontPicture 
{
public:
	std::string words;          //������

	std::vector<int> wordPos; //�ֵ�����;��ο�,���ֵ�˳���ߵ�,�п���û�в��ܵ����������ַ���,��һ��ֵ���������
	cv::Mat _Mat8u1c;   //Դͼcv��ʽ8uc1

	IfDictionary::Field dictionaryType;  //�ֵ��е����� -1����δָ��

	FontPicture(const FontPicture& copy);
	FontPicture();
public:
	QImage getQtImg();
	cv::Mat getSingleWordCv(int num,int longth = 1) const; //�г�ֻ�е��ֵ�ͼƬ,��˭��ʼ�м���
	cv::Mat getSingleWordCv(std::string PartStr) const;

	bool isHasWordPos() const;  //�Ƿ������λ����Ϣ

	void ShallowCopy(const FontPicture &input_new);
};


//ͼƬ����ԭʼͼƬ�Ľӿ�
 class IfFontOrlPictureGenerater
{
public:

	//������������
	virtual FontWordProprety getFontWordProprety() =0;	
	//�������������Refģʽ
	virtual void setFontPropertyRef(FontWordProprety &FontProperty) =0;
	//�������������
	virtual void setFontProperty(const FontWordProprety &FontProperty) =0;

	//������������
	virtual void setFontName(QString fontName)=0;


	//���cv::Mat ԭʼ��ʽ��ͼ���ļ�
	virtual cv::Mat getOrlFontImageCv(const QString &Str)=0;
	//���Qtͼ���ʽ������ͼ��,Ҳ���Ի��Qlabel��ʽ�Ļ���
	virtual QImage getOrlFontImageQt(const QString &Str,QLabel *Label = nullptr)=0;
	//����Զ��������ͼ���ʽ�ĺ���
	virtual FontPicture getOrlFontImageFp(const QString &Str,QLabel *Label = nullptr) =0;

};

 class FontGraphicTransTemp;

 class DrawingType;

 //����ͼƬ�ӿ�
 class IfFontPictureGenerater : public IfFontOrlPictureGenerater
 {
 public:
	 virtual void setGraphicTransRef(FontGraphicTransTemp *) = 0;
	 virtual void setGraphicTrans(const FontGraphicTransTemp &) = 0;

	 virtual void setDrawingType(const DrawingType & ) = 0;

	 //���cv::Mat �任���ʽ��ͼ���ļ�
	 virtual cv::Mat getFontImageCv(const QString &Str) = 0;
	 virtual FontPicture getFontImageFp(const QString &Str,QLabel *Label = nullptr) =0;
 };




