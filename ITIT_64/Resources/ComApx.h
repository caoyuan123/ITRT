#pragma once
#include <string>

namespace Com
{
	inline double Round(double r);

	//rt:���ֵ,input����ֵ,object �����ڴ�������
	typedef double (*calculateFunction)(double input,void *object);
	//���ַ�������,�������Ž�,���ʼֵ,��ʼ��Χ,���㺯��,�Ƿ�������ΧѰ��,����Χʱÿ����Ѱ�Ҷ��٣�-1 ��ʾ������0Ĭ��Ϊrange / 10 ,��������Ϊÿ���� Ĭ�Ͻ�ֹΪ8�� �� �������ز�ֵС�ڵ�ǰ���Ž�� 0.01 ���Ѿ�Ѱ���˳���Χ��
	extern  double ApxMid(double iLeft,double Range,calculateFunction func,void *object,int outRange = 0);
	extern  int ApxMid_int(int iLeft,int Range,calculateFunction func,void *object,int outRange = 0);

	//�����������,��ֵ-��ֵ-�ֻ���
	extern  double SearchMax(double iLeft,double iRight,double iScale,calculateFunction func,void *object);
	
	extern char ZHFontSavePlace[];  //zH�����ŵص�

	

#define  ImageTypesMax 6    //���Զ����ͼ������
	extern const std::string FileTypes[ImageTypesMax];



	extern double stdCompareRoit(double wordRoit,int szNum);  //�ʱ���ıȽ���ת��׼��
	extern double wordCompareRoit(double stdRoit,int szNum);  //��׼��ת�ʵıȽ���
	

	extern bool isFileInUse(std::string fileName);
}






