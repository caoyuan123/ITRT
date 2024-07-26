#pragma once
#include "FontPictureDef.hpp"
#include <QFont>



//ʹ��Qt�Ļ���������������ͼ��ӿ�
 class FontGeneraterQ :public IfFontOrlPictureGenerater
{
	//���ɷ���
	QImage generateModthd(QString str,QFont font,QLabel *sampleLabel = nullptr,std::list<cv::Rect> *wordRect = nullptr); 
	bool calulateWordPlace;   //�Ƿ�����ֵ���ϸλ��
	bool selfProperty; //�����������Լ���Ļ����ñ��˵�
	QFont _QFont;
	bool FontChanged;  //���������б�

protected:
	FontWordProprety* __FontWordProprety; //���Դ����ص�,Refģʽ��������,�ǵ�ַģʽ������

public:
	FontGeneraterQ(void);
	~FontGeneraterQ(void);

	 FontWordProprety getFontWordProprety();
	
	  //�������������-Ref���ñ��˵�����,��Ref�õ����Լ�������
	 void setFontPropertyRef(FontWordProprety &FontProperty);
	 void setFontProperty(const FontWordProprety &FontProperty);

	  //���cv::Mat��ʽ��ͼ���ļ�
	 cv::Mat getOrlFontImageCv(const QString &Str);
	 //���Qtͼ���ʽ������ͼ��,Ҳ���Ի��Qlabel��ʽ�Ļ���
	 QImage getOrlFontImageQt(const QString &Str,QLabel *Label = nullptr);

	 //����Զ��������ͼ���ʽ�ĺ���
	 FontPicture getOrlFontImageFp(const QString &Str,QLabel *Label = nullptr) ;
	 static FontPicture getOrlFontImageFp(const QString &Str,const QFont &Fontq,const FontWordProprety &fontProperty);

	 //�趨��������
	 virtual void setFontName(QString fontName);


 };



