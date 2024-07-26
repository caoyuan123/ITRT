#pragma once
//#include "FontSimilar.h"
#include "FontGraphicTransTemp.h"
#include "FontPictureDef.hpp"



//�洢ͼֽ���͵���
class DrawingType   //ͼֽ���
{
public:
	std::string strDesignInstitute;            //���Ժ����
	std::string strLineName;   //��·����

	int saveType;  //�汾

	/*
	 * �汾0��   CompareRoit��ԭ����
	 * �汾1�Ժ� CompareRoit�ǽ�����
	 */


	FontWordProprety _FontProprety;         //����������
	FontWordProprety small_FontProprety;    //С��������
	FontGraphicTransTemp _GraphicTransTemp;  //����ͼ��任����

	int stdWordHeight;  //��׼�ַ��߶�
	int stdWordWidth;	//��׼�ַ����

	int TableEdition;   //�汾���� ȡֵ1978/1992/2018 ��ʶ�����������
	int lineNums;  //һҳ��Լ�Ƕ����е������� ȡֵ��Χ�� 0-100

	double wordHightRoit;   //�ָ�ռA3��ı���
	double lineHightRoit;   //�и�ռA3��ı���


	int RowNum;            //һ��������
	int HeaderTable[50];   //-1�����ֹ,��ͷ���� ��ֵ���� Headers�е���ֵ
	double HeaderTableRoit[50];   //-1�����ֹ,��ͷ�Ĵ��¿��

	DrawingType();

public:	
	std::string toString() ; //������ʾ
	bool isExisting();  //��ȡ�Ƿ�Ϊ�Ѿ���������

	//��дxml
	void read(const cv::FileNode &node);
	void write(cv::FileStorage &fs) const;
};



extern void read(const cv::FileNode& node, DrawingType& x,const DrawingType& default_value = DrawingType());
extern void write(cv::FileStorage& fs, const std::string& name, const DrawingType& x);




//����ͼֽ����ı���·��
#define DtPath "DrawingTypeList.yaml"  

//�洢ͼֽ����ļ���
class DrawingTypeList : public std::vector<DrawingType>
{
public:
	void append(const DrawingType&);

	bool checkRepeatName(QString designName , QString lineName );  //�з���true û�з���false ��������ظ�
	
	void read( );
	void save( );

	DrawingTypeList();
	~DrawingTypeList();

};




