#pragma once
#include "StdAfx.h"





//���е���ʵ������Ŀ
static class SettingMp  //ocr�������
{
public:
	bool useMp;			     //�Ƿ��� ���̼߳���ʶ��
	int MpNums;				 //��ʼ���̵߳���������Ƕ��� <= 1 ΪĬ���������,��������ɴﵽ��������Ļ�Ĭ���Լ�����������Ϊ׼
	bool mainThreadRetain;   //���̱߳���

	SettingMp();

	void read( );
	void write( );

}u_setMp;




static class SettingOpenCV  //ocr�������
{
public:
	byte cpuType;

	bool g_blCpuSp;
	bool g_blSetMMX;
	bool g_blSetAVX;
	bool g_blSetAVX2;
	bool g_blSetSEE2;
	bool g_blSetSEE4_1;
	bool g_blSetSEE4_2;


	SettingOpenCV();

}u_setOpenCV;



static class SettingCvOCR  //ocr�������
{
public:
	float g_fWidthRoit;  //�ֿ��ж�����

	float g_fLineRoit;  //�и��ݲ��

	float g_fWordRoit; //��ͬλ�ݲ��

	float g_fMinThresholdRoit;  //��С��ֵ�ݲ��

	bool g_blSaveTempResult;  //�Ƿ񱣴���ʱ��� ������ǿ�㷨

	char g_charSplit;  //����ָ�����

	bool g_blWordPosJudge;  //�Ƿ����õ��ֵ������ж�

	SettingCvOCR();


	void read();
	void write();

}u_setCvOCR;




