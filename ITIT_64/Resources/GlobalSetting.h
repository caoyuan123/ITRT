#pragma once
#include "StdAfx.h"





//所有的真实设置项目
static class SettingMp  //ocr相关设置
{
public:
	bool useMp;			     //是否开启 多线程加速识别
	int MpNums;				 //开始多线程的最大数量是多少 <= 1 为默认最大数量,超过计算可达到最大数量的话默认以计算机最大数量为准
	bool mainThreadRetain;   //主线程保留

	SettingMp();

	void read( );
	void write( );

}u_setMp;




static class SettingOpenCV  //ocr相关设置
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



static class SettingCvOCR  //ocr相关设置
{
public:
	float g_fWidthRoit;  //字宽判定倍率

	float g_fLineRoit;  //行高容差倍率

	float g_fWordRoit; //字同位容差倍率

	float g_fMinThresholdRoit;  //最小阈值容差倍率

	bool g_blSaveTempResult;  //是否保存临时结果 用于增强算法

	char g_charSplit;  //计算分隔符号

	bool g_blWordPosJudge;  //是否启用单字的重新判定

	SettingCvOCR();


	void read();
	void write();

}u_setCvOCR;




