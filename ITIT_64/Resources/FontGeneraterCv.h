#pragma once
#include "fontpicturedef.hpp"
#include <QFont>
#include <list>

class FontGeneraterCv :
	public IfFontOrlPictureGenerater
{

	cv::Mat ZhFontDic; //zh ������ֵ� 

	cv::Mat generateModthd(QString str,QFont font,QLabel *sampleLabel = nullptr,std::list<cv::Rect> *wordRect = nullptr); //���ɷ���
	bool calulateWordPlace;
	bool selfProperty; //�����������Լ���Ļ����ñ��˵�
	QFont _QFont;
	bool FontChanged;  //���������б�

protected:
	FontWordProprety* __FontWordProprety; //���Դ����ص�,Refģʽ��������,�ǵ�ַģʽ������

public:
	FontGeneraterCv(void);
	~FontGeneraterCv(void);

	 FontWordProprety getFontWordProprety();

	 void setFontPropertyRef(FontWordProprety &FontProperty);

	 void setFontProperty(const FontWordProprety &FontProperty);

	 cv::Mat getOrlFontImageCv(const QString &Str);

	 QImage getOrlFontImageQt(const QString &Str,QLabel *Label = nullptr);

	 FontPicture getOrlFontImageFp(const QString &Str,QLabel *Label = nullptr) ;

	 virtual void setFontName(QString fontName);

};

