//#define _OPENMP 1

// #pragma omp critical

#include "MessyCodeTerminator.h"

#define PI 3.14159265358979     //系统float精度的PI是3.1415927 会造成部分 方向死锁

#define minFloatAcc 0.00001  //最小float精度定义为6位精度

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
将程序的float模型修改为严格,减轻滤波小窗口刷不出来的问题
修复n多C++语言变量/指针没有赋予初值导致的偶然问题

0015
完美修复sim模块的重入不刷新bug
增加拟合前不符合要求图片的检查报警

0016
貌似完美修复 滤波小窗口 不刷新问题
删去默认的LL识别，（因为这个比较容易和边线误判）

0017
增加多字体识别功能
增加Excel标准表格功能
删去了界面上工具栏的关闭按钮，防止用户关闭后无法再次打开

0018
修复了相交还会遗留的问题
修复了画线不能过角的问题
修复了会把表格线识别成 L或7的问题
变更字体包中的MonoTxt字体文件,修复由于错误文件带来的问题

0019
初步完善等待队列功能
再次修复画线不能过角的残留问题
增加未输出提醒
增加单独指定带动道岔/指定超限
增加输入旋转功能
修复sim模块算不准问题或者有时候算错但是给高分的问题
增加图片上的文字描述
跑分模块稳定功能
啥都不按默认按矫正
下一步按钮超级升级，适配各种文字，增加开始描述
修复结果图功能必须从左开始全都选取才能正确画框问题。（如果中间有漏的会偏移）

002_10
增加显示结果图



*/



namespace cv
{
	class Mat;
	class FileStorage;
	class FileNode;
}



