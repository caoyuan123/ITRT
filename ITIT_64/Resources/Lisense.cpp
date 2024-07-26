#include "Lisense.h"
#include "MD5.h"
#include <iostream>
#include <QSettings>

#include <QtGui/QtGui>
#include <QApplication>

using namespace std;

bool HexSinglePlus(char *a,char *b,bool *c,char *r);
short hex2dec(char *a);

//16进制相加
char* HexPLus(std::string Hex1,std::string Hex2)
{
	static char RRs[17];
	bool c = false;
	char r;
	for (int i = 0;i< 16 ;i++)
	{
		c = HexSinglePlus(&Hex1[15-i],&Hex2[15-i],&c,&r);
		RRs[15 - i] = r;
	}
	RRs[16] = 0;
	return RRs;
}

//字符串十六进制相加带进位
inline bool HexSinglePlus(char *a,char *b,bool *c,char *r)
{
	short Re = hex2dec(a) +  hex2dec(b) + *c;
	*r = "0123456789abcdef"[Re % 16];
	return bool(Re / 16);
}

short hex2dec(char *a)
{
	if (*a < 58 && *a > 47)
	{
		return *a - 48;
	}else if (*a < 71 && *a > 64)
	{
		return *a - 55;
	}else if (*a < 103 && *a > 96)
	{
		return *a - 87;
	}
	return -1;
}

std::string getCpuId()
{
	std::string  strCPUID;
	char Str[33] = {NULL};
	unsigned long s1 = 0, s2 = 0;

#ifdef _WIN64  //64位用微软 已经弄好的CPUID 内联函数

	int cpui[4] = {-1};
	std::vector<int[4]> data_;

	__cpuidex(cpui,1,1);

	s1 = cpui[3];
	s2 = cpui[0];
	

#else  // _WIN32

	__asm{
			mov eax, 01h 
			xor edx, edx 
			cpuid  
			mov s1, edx 
			mov s2, eax 
	}

#endif

	sprintf(Str,"%08X%08X", s1, s2);

	strCPUID = Str;
	return strCPUID;
}

//取md5 前16位不动884ba4dea410aee66b31ebab2cdf5c11
//私钥为xdy123的md5_16=> 6b4a72e0bfd0bb8d与后项相加 进位舍去 基偶相叠 生成字符串 MD5_16 作为注册码 验证其MD5_32相等
std::string Register(const char * input) //生成注册串
{
	std::string openStr = input;
	std::string privateKey = MD5("ITIT123").toString();

	QString qs = privateKey.data();

	qs  = qs.left(16);

	privateKey = qs.toStdString();
	

	std::string low = openStr.c_str() + 16;
	std::string high = openStr.c_str();

	std::string backPlus;
	backPlus = HexPLus(low,privateKey);

	char middle[33];
	for (int i = 0;i < 16;++i)
	{
		middle[i * 2] = backPlus[i];
		middle[i * 2 + 1] = high[i];
	}
	middle[32] = 0;


	return  MD5(middle).toString();
}


//判断注册与否
bool isResigst(std::string input)
{
	//算出私钥
	std::string openStr = MD5(getCpuId()).toString();
	std::string privateKey = MD5("ITIT123").toString();

	QString qs = privateKey.data();

	qs  = qs.left(16);

	privateKey = qs.toStdString();

	const char* low = openStr.c_str() + 16;
	const char* high = openStr.c_str();

	char *backPlus;
	backPlus = HexPLus(low,privateKey);

	char middle[33];
	for (int i = 0;i < 16;++i)
	{
		middle[i * 2] = backPlus[i];
		middle[i * 2 + 1] = high[i];
	}
	middle[32] = 0;



	std::string part1 = MD5(MD5(middle).toString()).toString();


	//比对两个部分
	std::string part2 = MD5(input).toString();

	if (part1 == part2)
	{
		return 1;
	}
	else
	{
		return 0;
	}

}


#define RUNKEY "HKEY_CURRENT_USER\\ITIT"


void RegsiterWiter(QString key,QString value)
{
	QSettings *qReg = new QSettings(RUNKEY,QSettings::NativeFormat);

	qReg->setValue(key,value);
	delete(qReg);
}

QString RegsiterRead(QString key)
{
	QSettings *qReg = new QSettings(RUNKEY,QSettings::NativeFormat);

	QVariant v = qReg->value(key);
	return v.toString();
	
}





 int mainZZZ(int argc, char *argv[])
{
	QApplication a(argc,argv);

	string str = getCpuId();

	cout << str << endl;

	string registerL = Register(MD5(str).toString().data());

	QString R = RegsiterRead("Register");

	bool bl = isResigst(R.toStdString());

	cout << bl <<endl;


	RegsiterWiter("Register",registerL.data());


	bl = isResigst(R.toStdString());

	cout << bl <<endl;

	
	QString test = RegsiterRead("Register");

	qDebug() << test;
	return a.exec();
}

 extern bool CheckRegist()
 {
	 return true;

	 QString R = RegsiterRead("Register");

	 bool bl = isResigst(R.toStdString());

	 return bl;
 }

 bool isRegist()
 {
	 return true;
 }

 extern QString getRegCode()
 {
	 return QString(MD5(getCpuId()).toString().data()) ;
 }

 
 
 
 

