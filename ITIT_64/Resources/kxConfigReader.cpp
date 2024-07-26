#include "kxConfigReader.h"
#include <QFile>
#include <QDebug>
#include <qdatetime.h>


using namespace std;
using namespace Qt;


kxConfigReader::kxConfigReader()
{
	m_ErrorLine = "";
	trackArrayNum = 0;
	SwitchArrayNum = 0;
	ButtonArrayNum = 0;
}


bool kxConfigReader::GetMapAttribute()
{
	QString str_Line = "";
	QString sErrorLine = "";

	QFile file(g_Path + "//Dev.txt");

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << file.errorString();
	}

	int nLine =1;//��ǰ����

    while(!file.atEnd())
	{
		QString str_line = file.readLine();

		
	

		//���ע��
		//ClearMark(str_line);

		int m = str_line.indexOf("//",0);
	

		if (m <0 )
		{
			m = str_line.indexOf("/*",0);
			if(m < 0)
			{
			
			}
			else 
			{
				str_line = str_line.left(m);//ȥ��ע������
			}	
		}
		else
		{
			str_line = str_line.left(m);//ȥ��ע������
		}
		str_line.trimmed();

	

	if (str_line.size() > 0)
	{
		if (str_line[str_line.size()] == 0)  //ɾ���ַ�������\0
		{
			str_line.chop(1);
		}
	}

		if (str_line.isEmpty() == false)
		{
			if(str_line.contains(("[END]")))
			{
				break;
			}else
			{
				
				if(str_line == "[�޲�����]")  //�޲������豸������
				{
					m_szTrackStyle = 1;
					m_szStyle=8;
				}
				else if(str_line == ("[�ı�]"))
				{
					m_szStyle=3;
				}
				else if(str_line == ("[��Ե��]"))
				{
					m_szStyle=2;
				}
				else if(str_line == ("[ָʾ��]"))
				{ 
					m_szStyle=4;
				}
				else if(str_line == ("[��վ�ź�]"))
				{ 
					m_szStyle=5;
				}
				else if(str_line==("[��վ�ź�]"))
				{
					m_szStyle=6;
				}
				else if(str_line==("[�����ź�]"))
				{
					m_szStyle=7;
				}
				else if(str_line==("[�ɵ�]"))
				{
					m_szTrackStyle = 2;
					m_szStyle=8;
				}
				else if(str_line==("[��������]"))
				{
					m_szTrackStyle = 3;
					m_szStyle=9;
				}
				else if(str_line==("[����]"))
				{
					m_szStyle=10;
				}
				else if(str_line==("[���ܰ�ť]"))
				{
					m_szStyle=1;
				}
				else if(str_line==("[�����߶�]"))
				{
					m_szStyle=11;
				}
				else if(str_line==("[���̰�ť]"))
				{
					m_szStyle=0xff;

				}
				else if (str_line.contains("[") && str_line.contains("]") && !str_line.contains(","))
				{
					m_szStyle=0xff;  //Ԥ��֮���¼�һЩĪ����������⵼�¶�ȡʧ��
				}
				else if(str_line.contains("[END]"))
				{
					m_szStyle=0xff;
				}
				else
				{
					if (!readALL(str_line))
					{
						return false;
					}
				}

			}



		}else  //��������
		{

		}
		nLine++;
	}

	
	QFile Bfile(g_Path + "\\BtnInfo.txt");

	if (!Bfile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << Bfile.errorString();
	}

	nLine =1;//��ǰ����

	while(!Bfile.atEnd())
	{
		QString str_line = Bfile.readLine();

		//�������ע��
		//ClearMark(str_Line);
		int m = str_line.indexOf("//",0);
	

		if (m <0 )
		{
			m = str_line.indexOf("/*",0);
			if(m < 0)
			{
			
			}
			else 
			{
				str_line = str_line.left(m);//ȥ��ע������
			}	
		}
		else
		{
			str_line = str_line.left(m);//ȥ��ע������
		}
		str_line.trimmed();

		if (str_line.isEmpty() == false)
		{
			if(str_line.contains("[END]"))
			{
				break;
			}else
			{
				CBtnDev* newBtnDev = nullptr;
				if (!readButton(str_line,newBtnDev))
				{
					return false;
				}


			}

		}else  //��������
		{

		}
		nLine++;
	}



	file.close();
	Bfile.close();

	return true;
}

bool kxConfigReader::readALL(const QString &str_line)
{
	CTrackDev *newTrack = nullptr;
	CSignalDev *newSingal = nullptr;
	CSwitchDev *newSwitch = nullptr;

	switch(m_szStyle)
	{
	case 1://���ܰ�ť						
		
		break;
	case 2:     //��Ե�ڷ�������,
		
		break;
	case 3:     //�ı�
		
		break;
	case 4:  //ָʾ��
		
		break;
	case 5:	//��վ�ź�	
	case 6://��վ�ź�
	case 7://�����ź�
		{
			if (!readSingal(str_line,newSingal))
			{
				return false;
			}
		}
	
		break;
	case 8://�ɵ����޲�  //break;	
	case 9://��������
		{
			
			if (!readTrack(str_line,newTrack))
			{
				return false;
			}
			break;
		}
	
	case 10:
		{//����
			
			if (!readSwitch(str_line,newSwitch))
			{
				return false;
			}
		}	
		break;
	default:
		break;						
	}
	if(m_szStyle !=11)
	{
		
	}
	return true;


}

bool kxConfigReader::DealLine(const QString &str_line,std::vector<int> &PInt,std::vector<QString> &PStr,QString &sErrorLine)
{
	g_nStr =0;
	g_nDigit = 0;

	int nlen = 0;					//���ݳ���
	int nMidlen = 0;				//��ȡ���ݳ���
	int nIntLen = 0;				// PIntά��
	int nStrLen = 0;				//PStrά��
	QChar szCh  = 0x00;				//��ǰ�ַ�
	unsigned char szDataState =0;   //��ǰ����״̬	//0 Ϊ��ͨ״̬  1Ϊ'( '   2Ϊ')'

	QString str = "";
	sErrorLine  = "";
	nlen = str_line.size();

	if (nlen < 2)
	{
		return false;
	}

	int i=0;
	while( i<= nlen )
	{   //ȡ�ַ�
		if(i == nlen)//һ�н���
		{
			szCh = str_line.at(nlen-1);
			if(szCh == 44)
			{
				sErrorLine+="����ĩβ�ж���Ķ��ţ�����Ӱ������׼ȷ\r\n";				
			}
			szCh = 44;
		}
		else
		{
			szCh = str_line.at(i);
		}

		
		switch(szCh.toLatin1())
		{
		case  40: //   (��������
			if(szDataState == 1)
			{

				sErrorLine += "��";
				sErrorLine += i;
				sErrorLine += "�ַ�����";
				sErrorLine += "  ȱ�������Ż��ж���������\r\n";

			}
			else if(szDataState == 2)
			{
				sErrorLine += "��";
				sErrorLine += i;
				sErrorLine += "�ַ�����";
				sErrorLine += "  ������������֮��ȱ�ٶ���\r\n";

			}
			else
			{
				szDataState = 1;
			}

			nMidlen =0;
			break;
		case 41://  ������ )
			if(szDataState == 2)//����������ȱ�������Ż��ж���������
			{
				sErrorLine += "��";
				sErrorLine += i;
				sErrorLine += "�ַ�����";
				sErrorLine += "  ȱ�������Ż��ж���������\r\n";
			}

			else
			{
				szDataState = 2;
			}
			break;
		case 32: //�ո�

			nMidlen++;
			break;

		case 44: //����,
			if(szDataState == 1)
			{
				QString str = "";

				str = str_line.mid(i-nMidlen,nMidlen);
				str = str.trimmed();

				if(str.isEmpty())
				{
					sErrorLine += "��";
					sErrorLine += i;
					sErrorLine += "�ַ�����";
					sErrorLine+="��ĳ��δ��������������\r\n";

				}
				else
				{
					PInt.push_back(str.toInt());
					if(PInt[nIntLen] > 20000 || PInt[nIntLen] < -20000)
					{
						sErrorLine += "��";
						sErrorLine += i;
						sErrorLine += "�ַ�����";
						sErrorLine+="���ּ��󲻿���ȡֵ\r\n";

						PInt[nIntLen]=0;
						return false;
					}
				}

				nIntLen++;


			}
			else if (szDataState == 2)
			{

				QString str=str_line.mid(i-nMidlen-1,nMidlen);

				str = str.trimmed();

				if(str.isEmpty())
				{
					sErrorLine += "��";
					sErrorLine += i;
					sErrorLine += "�ַ�����";
					sErrorLine += "��ĳ��δ��������������\r\n";
				}
				else
				{
					PInt.push_back( str.toInt());
					if(PInt[nIntLen] > 20000 || PInt[nIntLen] < -20000)
					{
						sErrorLine += "��";
						sErrorLine += i;
						sErrorLine += "�ַ�����";	
						sErrorLine+="���ּ��󲻿���ȡֵ\r\n";

						PInt[nIntLen]=0;
						return false;
					}
				}
				nIntLen++;
				szDataState = 0;
			}
			else 
			{
				QString str = str_line.mid(i-nMidlen,nMidlen);
				str = str.trimmed();

				if(str.isEmpty())
				{
					sErrorLine += "��";
					sErrorLine += i;
					sErrorLine += "�ַ�����";	
					sErrorLine+="��ĳ��δ��������������\r\n";
				}
				else
				{
					PStr.push_back(str);
				}
				nStrLen++;
			}
			nMidlen = 0;
			break;
		default://������������
			nMidlen++;
			break;
		}

		i++;
	}

	g_nStr = nStrLen  ;   //�����������ݸ���
	g_nDigit = nIntLen ;  //���������ݸ���


	return true;
}

bool kxConfigReader::readTrack(const QString &str_line,CTrackDev* newTrack)
{
	std::vector<int> PInt;
	std::vector<QString> PStr;
	QString sErrorLine;

	if (DealLine(str_line,PInt,PStr,sErrorLine))
	{

		if (m_szStyle != 8 && m_szStyle !=9)
		{
			newTrack = nullptr;
			return false;
		}else
		{
			newTrack = new CTrackDev();
			if (m_szStyle == 8)
			{
				newTrack->m_bIsNoCross = true;
			}else
			{
				newTrack->m_bIsNoCross = false;
			}
			
			newTrack->m_szDevStyle = m_szStyle;
		}

		int tempI =  PStr[0].toInt();
		if(tempI == 1)
		{
			newTrack->m_bVisible = true;
		}
		else if(tempI == 0)
		{
			newTrack->m_bVisible = false;
		}
		else
		{
			newTrack->m_bVisible = true;
			newTrack->m_nColorType = tempI;

			if(newTrack->m_nColorType == 5)
			{
				newTrack->m_bDelayFree = true;
			}
		}


		

		newTrack->m_iDevType = m_szTrackStyle;
		
		if (PStr.size() > 1)
		{
			newTrack->m_sDevName = PStr[1].toStdString();
		}else
		{
			newTrack->m_sDevName = "NULL";
		}

		if (PStr.size() > 2)
		{
			newTrack->m_sLogicalName = PStr[2].toStdString();
		}else
		{
			newTrack->m_sLogicalName = "NULL";
		}
		

		if (PStr.size() > 3)    //���е�����һ�����
		{
			if(PStr[3] == "1" ||PStr[3] == "2")
			{
				newTrack->m_bInputTrainNum = true;
			}
		}
		

		newTrack->m_ptOrigin.x = PInt[0];
		newTrack->m_ptOrigin.y = PInt[1];
		
		if (PInt.size() > 2)   //�ϵ�Devû��ptNamePoint
		{
			newTrack->m_ptNamePoint.x = PInt[0]+PInt[2];
			newTrack->m_ptNamePoint.y = PInt[1]+PInt[3];
		}
	


	}else
	{
		//ûдErrorReport
		return false;
	}
	trackArrayNum ++;
	newTrack->m_iArrayNum = trackArrayNum;
	_AllTrack.push_back(newTrack);

	return true;
}

bool kxConfigReader::readSingal(const QString &str_line,CSignalDev* newSingal)
{
	std::vector<int> PInt;
	std::vector<QString> PStr;
	QString sErrorLine;

	

	if (DealLine(str_line,PInt,PStr,sErrorLine))
	{
		if (m_szStyle == 5) //��վ�ź�	
		{
			newSingal = new CSignalDev();
			newSingal->m_szDevStyle = m_szStyle;
			
		}else if (m_szStyle == 6) //��վ�ź�	
		{
			newSingal = new CSignalDev();
			newSingal->m_szDevStyle = m_szStyle;

		}else if(m_szStyle == 7) //�����ź�	
		{
			newSingal = new CSignalDev();
			newSingal->m_szDevStyle = m_szStyle;

		}else
		{
			newSingal = nullptr;
			return false;
		}

		int tempI = PStr[0].toInt();

		
		if(tempI == 1)
		{
			newSingal->m_bVisible = true;
		}
		else
		{
			newSingal->m_bVisible = false;
		}

		 tempI = PStr[1].toInt();

		if(tempI == 1)
		{
			newSingal->m_bDwarfSignal = true;
		}
		else
		{
			newSingal->m_bDwarfSignal = false;
		}

		newSingal->m_nRotateAngle = PStr[2].toShort();

		newSingal->m_sDevName = PStr[3].toStdString();
		newSingal->m_sLogicalName = PStr[4].toStdString();


		if (PStr.size() > 5)
		{
			if(PStr[5] =="" )
			{
				newSingal->m_bShuntRed = false;
			}
			else
			{
				newSingal->m_bShuntRed = true;
			}
		}else
		{
			newSingal->m_bShuntRed = false;
		}
		
		newSingal->m_ptOrigin.x = PInt[0];
		newSingal->m_ptOrigin.y = PInt[1];

		if (PInt.size() >= 4)
		{
			newSingal->m_ptNamePoint.x=PInt[2] + PInt[0];
			newSingal->m_ptNamePoint.y=PInt[3] + PInt[1];
		}
	




	}else
	{
		return false;
	}



	_AllSingal.push_back(newSingal);

	return true;
}

bool kxConfigReader::readSwitch(const QString &str_line,CSwitchDev* newSwitch)
{
	std::vector<int> PInt;
	std::vector<QString> PStr;
	QString sErrorLine;



	if (DealLine(str_line,PInt,PStr,sErrorLine))
	{
		if(m_szStyle == 10) //����
		{
			newSwitch = new CSwitchDev();
			newSwitch->m_szDevStyle = m_szStyle;

		}else
		{
			newSwitch = nullptr;
			return false;
		}


		if(PStr[0].toInt() == 2)
		{
			newSwitch->m_bNoLeftClick = true;
			newSwitch->m_bVisible = true;

		}
		else if(PStr[0].toInt() == 1)
		{
			
			newSwitch->m_bVisible = true;
		}
		else
		{
			newSwitch->m_bVisible = false;
		}

		newSwitch->m_sDevName = PStr[1].toStdString();
		newSwitch->m_sLogicalName =  PStr[2].toStdString();

		newSwitch->m_wSectionNum = WORD(PStr[3].toInt());


		if(PStr[4].toInt() == 1)
		{
			newSwitch->m_bStraitBeamed = true;
		}
		else
		{
			newSwitch->m_bStraitBeamed = false;
		}


		if (PStr.size() > 5 && PStr[5].toInt() > 0)
		{
			newSwitch->m_byTGQFlag = unsigned char (PStr[5].toInt());
		}else
		{
			newSwitch->m_byTGQFlag = 0;
		}

		newSwitch->m_wControlSwitch1 = WORD(PInt[0]);

		if(PInt[1] == 1)
		{
			newSwitch->m_bDirect1 = true;
		}
		else
		{
			newSwitch->m_bDirect1 = false;
		}


		newSwitch->m_wControlSwitch2 = WORD(PInt[2]/*+m_iPoint*/);
		if(PInt[3] == 1)
		{
			newSwitch->m_bDirect2 = true;
		}
		else
		{
			newSwitch->m_bDirect2 = false;
		}

		newSwitch->m_ptOrigin.x = PInt[4];
		newSwitch->m_ptOrigin.y = PInt[5];

		newSwitch->m_ptNamePoint.x = PInt[6]+PInt[4];
		newSwitch->m_ptNamePoint.y = PInt[7]+PInt[5];
		



	}else
	{
		return false;
	}

	SwitchArrayNum++;
	newSwitch->m_iArrayNum = SwitchArrayNum;

	_AllSwitch.push_back(newSwitch);

	return true;
}

bool kxConfigReader::readButton(const QString &str_line,CBtnDev* newBtn)
{

	std::vector<int> PInt;
	std::vector<QString> PStr;
	QString sErrorLine;

	

	if (DealLine(str_line,PInt,PStr,sErrorLine))
	{
		if (PInt.size() > 0)
		{
			return false;
		}

		if (PStr.size() != 3)
		{
			sErrorLine += "Button.txt����\r\n";
			return false;
		}
		ButtonArrayNum++;



		PStr[0] = PStr[0].trimmed();

		if (PStr[0].left(1)  == "X" || PStr[0].left(1)  == "x")
		{
			newBtn = new CBtnDev();
			PStr[1] = PStr[1].trimmed();
			PStr[2] = PStr[2].trimmed();
			

			newBtn->m_wDevId = PStr[1].toInt();

			newBtn->m_iBtnNum = ButtonArrayNum;
			newBtn->m_sDevName = PStr[2].toStdString();

			newBtn->m_szDevStyle = SingalButtonCode;  //�Ǳ�׼���� - �źŰ�ť
			

			//���ƽ��� ->D*AΪ ��������ť,D*LAΪ�����г���ť, *DAΪ������ť,*LAΪ�г���ť
			// *TAΪͨ����ť, *DZAΪ������ť,*PZAΪ�г���ť 

			if (PStr[2].left(1).toUpper() == "D" && PStr[2].right(2).toUpper() == "LA")   //D*LA
			{
				newBtn->m_sLogicalName = PStr[2].mid(0,PStr[2].length() - 2).toStdString();
				newBtn->m_btSingalClass = 0x07;

			}else if ( PStr[2].right(3).toUpper() == "LZA")   //*LZA
			{
				newBtn->m_sLogicalName = PStr[2].toStdString();
				newBtn->m_btSingalClass = 0x13;
			}
			else if (PStr[2].left(1).toUpper() == "D" && PStr[2].right(1).toUpper() == "A") //D*A
			{
				newBtn->m_sLogicalName = PStr[2].mid(0,PStr[2].length() - 1).toStdString();
				newBtn->m_btSingalClass = 0x05;

			}else if (PStr[2].right(3).toUpper() == "PZA" || PStr[2].right(3).toUpper() == "PLA") // *PZA / *PLA
			{
				newBtn->m_sLogicalName = PStr[2].toStdString();
				newBtn->m_btSingalClass = 0x03;
			}
			else if (PStr[2].left(1).toUpper() != "D" && PStr[2].right(2).toUpper() == "LA") // *LA
			{
				newBtn->m_sLogicalName = PStr[2].mid(0,PStr[2].length() - 2).toStdString();
				newBtn->m_btSingalClass = 0x0f;

			}else if (PStr[2].left(1).toUpper() != "D" && PStr[2].right(2).toUpper() == "DA") // *DA
			{
				newBtn->m_sLogicalName = PStr[2].mid(0,PStr[2].length() - 2).toStdString();
				newBtn->m_btSingalClass = 0x0D;
			}else if (PStr[2].right(2).toUpper() == "TA" ) // *TA 
			{
				newBtn->m_sLogicalName = PStr[2].toStdString();
				newBtn->m_btSingalClass = 0x13;
			}else if (PStr[2].right(3).toUpper() == "DZA" ) // *DZA 
			{
				newBtn->m_sLogicalName = PStr[2].toStdString();
				newBtn->m_btSingalClass = 0x11;
			}else
			{
				newBtn->m_sLogicalName = PStr[2].toStdString();
				newBtn->m_btSingalClass = 0x10;
			}


			_AllButton.push_back(newBtn);

		}
	

		
		return true;
	}

	return false;
}

kxConfigReader::~kxConfigReader()
{
	int size = _AllTrack.size();
	for (int i =0;i < size;i++)
	{
		delete(_AllTrack[i]);
	}
	size = _AllSingal.size();
	for (int i =0;i < size;i++)
	{
		delete(_AllSingal[i]);
	}

	size = _AllSwitch.size();
	for (int i =0;i < size;i++)
	{
		delete(_AllSwitch[i]);
	}

	size = _AllButton.size();
	for (int i =0;i < size;i++)
	{
		delete(_AllButton[i]);
	}
}

void kxConfigReader::ClearMark(QString &str_line)
{
	int m = str_line.indexOf("//",0);
	

	if (m <0 )
	{
		m = str_line.indexOf("/*",0);
		if(m < 0)
		{
			
		}
		else 
		{
			str_line = str_line.left(m);//ȥ��ע������
		}	
	}
	else
	{
		str_line = str_line.left(m);//ȥ��ע������
	}
	str_line.trimmed();

	

	if (str_line.size() > 0)
	{
		if (str_line[str_line.size()] == 0)  //ɾ���ַ�������\0
		{
			str_line.chop(1);
		}
	}

	
}




