#pragma once
#include <QString>

//检查 是否已经注册
extern bool CheckRegist( );

inline bool isRegist( );  //检测的注册

extern QString getRegCode( ); //获取注册码

extern void RegsiterWiter(QString key,QString value);
extern QString RegsiterRead(QString key);




