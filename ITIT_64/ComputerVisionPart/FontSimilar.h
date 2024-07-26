#pragma once
#include <vector>
#include "FontGraphicTransTemp.h"
#include "FontPictureDef.hpp"


#ifdef _DEBUG
extern void testFuc();
#endif

using namespace std;

class DrawingType;

//���嵽ͼ�����������
struct FontPictureProprety : public FontWordProprety ,public FontGraphicTransTemp
{
//	double CompareRoit;   //ƥ����

	int stdWordHeight;  //��׼�ַ��߶� //��һ��","���ָ��ֿ��� 
	int stdWordWidth;	//��׼�ַ����

	FontPictureProprety()
	{
		CompareRoit = 0;
		stdWordHeight = 0;
		stdWordWidth = 0;
	}

	//	cv::Mat getFontMat(); //ûд��
	//	virtual void read(const cv::FileNode &node)=0;
	//	virtual void write(cv::FileStorage &fs) const=0;

	void setDrawingType(DrawingType& _dt ); //�����ݿ�����DrawingType��
};



//����ƽ��õĽṹ��,���Ը������������������õ�FontPicturePos���͵��Լ����ɵ�����СͼƬ
struct FontApx :public FontPicture
{
public:
	double MaxCompareRoit;  //���������

	FontPictureProprety    *FontPicProprety;//��������_������������Font�Ļ���ͼ��任��4������
	IfFontOrlPictureGenerater *FontGenerater; //������������


	//������ģ�ĺ���,��Wordchoice�Ŀ���ѡ�����Ǹ���
	cv::Mat getFontImageCv(bool TransUsed = true/*�Ƿ��Զ�ʹ��ģ��任*/) const;
	cv::Mat getFontImageCv(int Wordchoice,bool TransUsed = true) const;

	//�ֹ�ָ�����ָ�
	std::vector<int> DistanceY;  

	FontApx( );
	~FontApx( );

	//read
	//write
};

#define MiniCompress 0.2


//�ֹ����Զ���������
//1.Ѱ���������
//2.�������ͼƬ����ʵͼƬ
class FontSimilar
{
private:
	FontGraphicTransTemp ImageTreeble(FontApx &input, int num = 0);

public:
	QString manualFontName;                  //�û����ĳ�ʼ���ݡ��������� ����������ڷ�Ϊ_ZH,TXT,����
	int     manualFontNum;                   //�û����ĳ�ʼ���ݡ��ֺ�  �������������
	double  manualFontSpacing;               //�û����ĳ�ʼ���ݡ��ּ�� ���ĳ�ʼֵ
	double  manualFontCompressRatio;         //�û����ĳ�ʼ���ݡ���ѹ���� ������ʼֵ
	
	double miniThreshold;					 //���ͨ����ֵ
	FontPictureProprety FontPropertyFinal;   //�������Ĳ���,ָ�������Ļ��ᶨ����

	//FontApxNew _FontApxNew;                    //ֻȡһ���ʵı���

	std::vector <FontApx> List_FontSelect;   //���ȡ�ö���ʵĻ�

	IfFontOrlPictureGenerater *FontGenerater;  //������������,ʹ��Ĭ�����漴��
	 
	double singleWord(FontApx ApxTarget,int wordNum,double &SizeRoit,double &FontScretch);
	 
	void init( );  //��ʼ��
	void fittingInit(int FontApxNum = 0);  //�������
	void TotalSearch(FontApx input);    //��������
	void TotalSearch2(FontApx input);

//	void fitting( );      //֮��������

	FontSimilar(void);
	~FontSimilar(void);
};




