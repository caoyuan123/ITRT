#pragma once
#include <QString>

//��� �Ƿ��Ѿ�ע��
extern bool CheckRegist( );

inline bool isRegist( );  //����ע��

extern QString getRegCode( ); //��ȡע����

extern void RegsiterWiter(QString key,QString value);
extern QString RegsiterRead(QString key);




