//#define _OPENMP 1

// #pragma omp critical

#include "MessyCodeTerminator.h"

#define PI 3.14159265358979     //ϵͳfloat���ȵ�PI��3.1415927 ����ɲ��� ��������

#define minFloatAcc 0.00001  //��Сfloat���ȶ���Ϊ6λ����

typedef unsigned char byte;
typedef unsigned short WORD;

#define g_GraphicsZoneItem 30
#define g_GraphicsLineItem 31
#define g_GraphicsRectItem 32

#define g_GraphicsLineItemV 33
#define g_GraphicsLineItemH 34

#define g_Horizontal false
#define g_Vertical true


#pragma warning (disable:4482)

#define selfVer "0.0.2.10"

/*

0014 
�������floatģ���޸�Ϊ�ϸ�,�����˲�С����ˢ������������
�޸�n��C++���Ա���/ָ��û�и����ֵ���µ�żȻ����

0015
�����޸�simģ������벻ˢ��bug
�������ǰ������Ҫ��ͼƬ�ļ�鱨��

0016
ò�������޸� �˲�С���� ��ˢ������
ɾȥĬ�ϵ�LLʶ�𣬣���Ϊ����Ƚ����׺ͱ������У�

0017
���Ӷ�����ʶ����
����Excel��׼�����
ɾȥ�˽����Ϲ������Ĺرհ�ť����ֹ�û��رպ��޷��ٴδ�

0018
�޸����ཻ��������������
�޸��˻��߲��ܹ��ǵ�����
�޸��˻�ѱ����ʶ��� L��7������
���������е�MonoTxt�����ļ�,�޸����ڴ����ļ�����������

0019
�������Ƶȴ����й���
�ٴ��޸����߲��ܹ��ǵĲ�������
����δ�������
���ӵ���ָ����������/ָ������
����������ת����
�޸�simģ���㲻׼���������ʱ������Ǹ��߷ֵ�����
����ͼƬ�ϵ���������
�ܷ�ģ���ȶ�����
ɶ������Ĭ�ϰ�����
��һ����ť��������������������֣����ӿ�ʼ����
�޸����ͼ���ܱ������ʼȫ��ѡȡ������ȷ�������⡣������м���©�Ļ�ƫ�ƣ�

002_10
������ʾ���ͼ



*/



namespace cv
{
	class Mat;
	class FileStorage;
	class FileNode;
}



