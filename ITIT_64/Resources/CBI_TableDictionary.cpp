#include "CBI_TableDictionary.h"
#include "FontGeneraterQ.h"
#include "kxConfigReader.h"
#include "DrawingBook.h"
#include "FontPictureDef.hpp"
#include "FontGraphicTransTemp.h"
#include <QDebug>
#include "MessyCodeTerminator.h"

CBI_TableDictionary::CBI_TableDictionary(DrawingBook *_Parent)
{
	Ptr_DrawingBook = _Parent;
	shallowCopy = false;
	spOverrun = false;
	spProtectFollow = false;

	m_conditionSingleSwitch = 0;

	m_SyncFontPicture = false;
	writeLock = false;

	m_DrawingType = new DrawingType();

	numInit();



}

//��������,Ĭ����ǳ����
CBI_TableDictionary::CBI_TableDictionary(const CBI_TableDictionary &copy,bool deep )
{

	if (copy.getWriteLock())
	{
		qWarning(U8c("��������������,δ�ܿ���"));

		return;
	}

	if (deep)  //��ȿ����Ļ�
	{
		for (int i = 0;i < DictionaryFiledMax;i++)
		{
			this->Distorage[i] = copy.Distorage[i];

			int sz = copy._FontPicture[i].size();
			for (int j = 0;j < sz;j++)
			{

				FontPicture *fp = new FontPicture(*(copy._FontPicture[i][j]));
				_FontPicture->push_back(fp);

			}

			sz = copy._FontPicture_Similar[i].size();
			for (int j = 0;j < sz; j++)
			{

				FontPicture *fp = new FontPicture(*(copy._FontPicture_Similar[i][j]));
				_FontPicture_Similar->push_back(fp);

			}

			shallowCopy = false;
		}

		m_SyncFontPicture = copy.m_SyncFontPicture;  //��ǩͬ��
	}
	else  //Ĭ��ǳ�����Ļ�
	{

		for (int i = 0;i < DictionaryFiledMax;i++)
		{
			this->Distorage[i] = copy.Distorage[i];
			this->_FontPicture[i] = copy._FontPicture[i];
			this->_FontPicture_Similar[i] = copy._FontPicture_Similar[i];
	
			shallowCopy = true;
		}

		m_SyncFontPicture = false;
	}

	m_DrawingType = new DrawingType();

	this->spOverrun = copy.spOverrun;
	this->spProtectFollow = copy.spProtectFollow;
	this->Ptr_DrawingBook = copy.Ptr_DrawingBook;
	
	this->m_DrawingType = new DrawingType(*(copy.m_DrawingType) );
	this->writeLock = false;

	this->m_conditionSingleSwitch = copy.m_conditionSingleSwitch;

	numInit();


}


void CBI_TableDictionary::slot_readDev(std::string DevInfoPath)
{
	if (writeLock)
	{
		qWarning(U8c("setDistorage ������������ֹ"));
		return;
	}

	kxConfigReader* kxCr = new kxConfigReader();

	kxCr->g_Path = QString(DevInfoPath.data());



	if (!kxCr->GetMapAttribute())
	{
		//���쳣����

		throw std::logic_error("Error GetMapAttribute");
	}

	
	
	Distorage[Field::��������].clear();
	
	
	int size = kxCr->_AllTrack.size();

	for (int i = 0;i < size;i++)
	{
		if (QString(kxCr->_AllTrack[i]->m_sDevName.data()).toUpper() != "NULL")
		{
			if (kxCr->_AllTrack[i]->m_bVisible == true)
			{
				Distorage[Field::��������].push_back(kxCr->_AllTrack[i]->m_sDevName);
			}
		}
	}

	Distorage[Field::��������].clear();
	size = kxCr->_AllSwitch.size();
	for (int i = 0;i < size;i++)
	{
		QString str = kxCr->_AllSwitch[i]->m_sLogicalName.data( );

		if (str.toUpper() != "NULL")
		{
			bool ctn = false;
			for (int j = 0;j < Distorage[Field::��������].size( );j++)  //3/7���ֻ����
			{
				if (kxCr->_AllSwitch[i]->m_sLogicalName == Distorage[Field::��������][j])
				{
					ctn = true;
					break;
				}

			}

			if (ctn)
			{
				//continue;
			}else
			{
				Distorage[Field::��������].push_back(kxCr->_AllSwitch[i]->m_sLogicalName);
			}


			
		}
	}

	Distorage[Field::�����źŻ�����].clear();
	Distorage[Field::�г��źŻ�����].clear();
	size = kxCr->_AllSingal.size();
	for (int i = 0;i < size;i++)
	{
		if (QString(kxCr->_AllSingal[i]->m_sDevName.data()).toUpper() != "NULL")
		{
			if (kxCr->_AllSingal[i]->m_szDevStyle == 7)  //�����ź�
			{
				Distorage[Field::�����źŻ�����].push_back(kxCr->_AllSingal[i]->m_sDevName);
			}else
			{
				Distorage[Field::�г��źŻ�����].push_back(kxCr->_AllSingal[i]->m_sDevName);
			}
		}
	}

	
	Distorage[Field::�г���ť����].clear();
	Distorage[Field::������ť����].clear();
	size = kxCr->_AllButton.size();
	for (int i = 0;i < size;i++)
	{
		if (QString(kxCr->_AllButton[i]->m_sDevName.data()).toUpper() != "NULL")
		{
			if ((kxCr->_AllButton[i]->m_btSingalClass & 0x02) == 0x02)  //�г���ť
			{
				Distorage[Field::�г���ť����].push_back(kxCr->_AllButton[i]->m_sDevName);
			}else
			{
				Distorage[Field::������ť����].push_back(kxCr->_AllButton[i]->m_sDevName);
			}
		}
	}
	m_SyncFontPicture = false;
	delete(kxCr);

}

CBI_TableDictionary::~CBI_TableDictionary()
{

	delete(m_DrawingType);
	Clear_FontPicture();
}

std::vector<std::string> CBI_TableDictionary::getDistorage(int num )
{
	if (num < 0 || num > DictionaryFiledMax)
	{
		std::vector<std::string> Rt;
		return Rt;
	}else
	{
		return Distorage[num];
	}
}

void CBI_TableDictionary::setDistorage(int num,const std::vector<std::string> &input)
{
	if (writeLock)
	{
		qWarning(U8c("setDistorage ������������ֹ"));
		return;
	}

	if (num < 0 || num > DictionaryFiledMax)
	{
		throw std::logic_error("fatal Error setDistorage");
		return ;
	}else ;

	Distorage[num].clear();

	//Distorage[num] = std::vector<std::string>(input);
	m_SyncFontPicture = false;
	Distorage[num] = input;
}

void CBI_TableDictionary::write(cv::FileStorage &fs) const
{
	fs << "{";  
	{
		//һ�˴�һ��
		for (int i = 0; i< DictionaryFiledMax ;i ++)
		{
			//fs << "Name" << FieldChinese[i];
			fs << FieldEnglish[i] << "[:";
			{
				int s = Distorage[i].size();
				for (int idx = 0 ;idx < s ;idx ++)
				{

					fs <<Distorage[i][idx];

				//	fs << _FontPicture[i][idx]->_Mat8u1c;
				}
			}
			fs << "]";
		}

	}
	fs << "}";
}

void CBI_TableDictionary::read(const cv::FileNode &node)
{
	if (writeLock)
	{
		qWarning(U8c("CBI_TableDictionary::read ������������ֹ"));
		return;
	}

	for(int i =0 ;i < DictionaryFiledMax;i++)
	{
		cv::FileNode fn2 = node[FieldEnglish[i]];
		if (fn2.isSeq())
		{
			cv::FileNodeIterator it = fn2.begin();
			cv::FileNodeIterator it_E = fn2.end();

			int idx = 0;
			for (;it != it_E;++it,++idx)
			{
				std::string str;
				(*it) >> str;

				Distorage[i].push_back(str);
			}

		}

	}

	m_SyncFontPicture = false;  //û��
}

void CBI_TableDictionary::slot_GenerateFontPicture( )
{
	if (writeLock)
	{
		qWarning(U8c("slot_GenerateFontPicture ������������ֹ"));
		return;
	}


	if (Ptr_DrawingBook != nullptr)
	{
		Clear_FontPicture();

		/*update! -> ��cv����д������Ҫ�Ľ�*/
		IfFontOrlPictureGenerater *_Generater = new FontGeneraterQ();
		_Generater->setFontProperty(Ptr_DrawingBook->getDrawingType()->_FontProprety);


		//�������ɶ�Ӧ�� ͼƬ�ļ�
		for (int i = 0;i < DictionaryFiledMax;++i)
		{
			switch(i)
			{
			case Field::��������:
				{
					//�������⴦�� �ӷ�λ��־
					std::vector<std::string> SwitchNormal;
					
					for (int j = 0; j < Distorage[Field::��������].size();++j)
					{
						SwitchNormal.push_back(Distorage[Field::��������][j]);
						SwitchNormal.push_back("(" +  Distorage[Field::��������][j] + ")");

					}
					
					GenerateFontPicture(SwitchNormal,_FontPicture[i],Field::��������,
						_Generater,Ptr_DrawingBook->getDrawingType()->_GraphicTransTemp);

				}break;

			case Field::��������: 
				{
					std::vector<std::string> SwitchProtectFollow;
					//û��ָ�� �������� Ĭ�����ж��д�������

					if (spProtectFollow)
					{
						SwitchProtectFollow = Distorage[Field::��������];  //��ָ��ֱ��ʹ��ָ��

						for (int j = 0; j < Distorage[Field::��������].size();++j)
						{
							SwitchProtectFollow.push_back("[(" + Distorage[Field::��������][j] + ")]");
							SwitchProtectFollow.push_back("[" + Distorage[ Field::��������][j] + "]");
						}

					}else
					{
						for (int j = 0; j < Distorage[Field::��������].size();++j)
						{
							SwitchProtectFollow.push_back("[(" + Distorage[Field::��������][j] + ")]");
							SwitchProtectFollow.push_back("[" + Distorage[ Field::��������][j] + "]");
							SwitchProtectFollow.push_back("{(" + Distorage[ Field::��������][j] + ")}");
							SwitchProtectFollow.push_back("{" + Distorage[ Field::��������][j] + "}");
						}

						
					}

						GenerateFontPicture(SwitchProtectFollow,_FontPicture[i],Field::��������,
							_Generater,Ptr_DrawingBook->getDrawingType()->_GraphicTransTemp);
					
				}break;

			case Field::��������: 
				{
					
						/*
						int sz = _FontPicture[Field::��������].size();
						for (int j = 0;j < sz ; ++j)
						{
							FontPicture *fp = new FontPicture(*(_FontPicture[Field::��������][j]));
							_FontPicture[Field::��������].push_back(fp);
						}
						*/

						//�������⴦�� �ӷ�λ��־
						std::vector<std::string> SwitchOverRun;

						for (int j = 0; j < Distorage[Field::��������].size();++j)
						{

							//����Ѿ���������������Ϊ��������
							if (m_conditionSingleSwitch == 1)
							{
								std::string Str = Distorage[Field::��������][j];
									
								int idx = Str.find('/');

								if (idx > 0)
								{
									std::string StrQ  = Str.substr(0,idx );  //ǰ�벿��
									std::string StrH  = Str.substr(idx + 1, Str.size() - 1); //��벿��

									SwitchOverRun.push_back("<" + StrQ + ">");
									SwitchOverRun.push_back("<(" +  StrQ + ")>");

									SwitchOverRun.push_back("<" + StrH + ">");
									SwitchOverRun.push_back("<(" +  StrH + ")>");

								}else
								{
									SwitchOverRun.push_back("<" + Distorage[Field::��������][j] + ">");
									SwitchOverRun.push_back("<(" +  Distorage[Field::��������][j] + ")>");
								}

							}else if (m_conditionSingleSwitch == 2)
							{

								std::string Str = Distorage[Field::��������][j];

								int idx = Str.find('/');

								if (idx > 0)
								{
									std::string StrQ  = Str.substr(0,idx );  //ǰ�벿��
									std::string StrH  = Str.substr(idx + 1, Str.size() - 1); //��벿��

									SwitchOverRun.push_back("<" + StrQ + ">");
									SwitchOverRun.push_back("<(" +  StrQ + ")>");

									SwitchOverRun.push_back("<" + StrH + ">");
									SwitchOverRun.push_back("<(" +  StrH + ")>");

									SwitchOverRun.push_back("<" + Distorage[Field::��������][j] + ">");
									SwitchOverRun.push_back("<(" +  Distorage[Field::��������][j] + ")>");

								}else
								{
									SwitchOverRun.push_back("<" + Distorage[Field::��������][j] + ">");
									SwitchOverRun.push_back("<(" +  Distorage[Field::��������][j] + ")>");
								}

							}
							else
							{
								SwitchOverRun.push_back("<" + Distorage[Field::��������][j] + ">");
								SwitchOverRun.push_back("<(" +  Distorage[Field::��������][j] + ")>");
							}
								
						}

						SwitchOverRun.push_back("<>"); //�������Գ���
						SwitchOverRun.push_back("< >"); //�������Գ���

						Distorage[Field::�ж��źŵ���] = SwitchOverRun;

						if (!spProtectFollow)
						{
							//����ָ���ľͰ�ָ������
							std::vector<std::string> dis = Distorage[i];

							dis.push_back("<>");
							dis.push_back("< >");

							GenerateFontPicture(dis,_FontPicture[i],Field::��������,
								_Generater,Ptr_DrawingBook->getDrawingType()->_GraphicTransTemp);

							
						}else
						{
							
							GenerateFontPicture(SwitchOverRun,_FontPicture[i],Field::��������,
								_Generater,Ptr_DrawingBook->getDrawingType()->_GraphicTransTemp);
						}
						
						GenerateFontPicture(SwitchOverRun,_FontPicture[Field::�ж��źŵ���],Field::�ж��źŵ���,
							_Generater,Ptr_DrawingBook->getDrawingType()->_GraphicTransTemp);
					

				}break;
			case Field::�ж��źŵ���: break;  //���ű�������������
			case Field::�ж��ź�:
				{
					//�ж��źŸ��г��źż�D,�������źŻ���L
					Distorage[Field::�ж��ź�].clear();

					int sz = Distorage[Field::�г��źŻ�����].size();

					for (int j = 0; j < sz; ++j)
					{
						Distorage[Field::�ж��ź�].push_back( Distorage[Field::�г��źŻ�����][j] );
						Distorage[Field::�ж��ź�].push_back( Distorage[Field::�г��źŻ�����][j] + "D");
						Distorage[Field::�ж��ź�].push_back( Distorage[Field::�г��źŻ�����][j] + "L");
					}

					sz = Distorage[Field::�����źŻ�����].size();
					for (int j = 0; j < sz;++j)
					{
						Distorage[Field::�ж��ź�].push_back( Distorage[Field::�����źŻ�����][j] );
						Distorage[Field::�ж��ź�].push_back( Distorage[Field::�����źŻ�����][j] + "L");
					}

					GenerateFontPicture(Distorage[Field::�ж��ź�],_FontPicture[i],Field::�ж��ź�,
						_Generater,Ptr_DrawingBook->getDrawingType()->_GraphicTransTemp);


				}break;

			default :  //���ϵĶ���Ҫ���⴦��� ��������ͨ��
				{
					GenerateFontPicture(Distorage[i],_FontPicture[i],i,
						_Generater,Ptr_DrawingBook->getDrawingType()->_GraphicTransTemp);
				}break;
			}

		}
		
		shallowCopy = false; //�Լ����ɺ�Ĭ��Ϊ��ȴ洢����
		delete(_Generater);
		m_SyncFontPicture = true;   //���ɺ����ͬ������
		 
	}else
	{
		qWarning(U8c("û��ָ�� ������������ �޷�����"));
	}

}

void CBI_TableDictionary::Clear_FontPicture()
{
	if (writeLock)
	{
		qWarning(U8c("����Ѿ���������"));
	}

	for (int i = 0;i< DictionaryFiledMax;++i)
	{
		int s = _FontPicture[i].size();
		if (s > 0)
		{
			if (!shallowCopy)  //����ǳ������Ҫɾͼ
			{
				for (int j =0;j < s; ++j)
				{
					delete(_FontPicture[i][j]);

					_FontPicture[i][j] = nullptr;
				}
			}
		}
		_FontPicture[i].clear();
	}

	for (int i = 0;i< DictionaryFiledMax;++i)
	{
		int s = _FontPicture_Similar[i].size();
		if (s > 0)
		{
			if (!shallowCopy)  //����ǳ������Ҫɾͼ
			{
				for (int j =0;j < s; ++j)
				{
					delete(_FontPicture_Similar[i][j]);
					_FontPicture_Similar[i][j] = nullptr;
				}
			}
		}
		_FontPicture_Similar[i].clear();
	}
}

void CBI_TableDictionary::GenerateFontPicture(const std::vector<std::string> &Ipt_Distorage,std::vector<FontPicture*> &Out_FontPicture,int DicType
	, IfFontOrlPictureGenerater *_Generater ,const FontGraphicTransTemp &_TransTemp)
{

	//�ⲿ���
	/*if (Out_FontPicture.size() != 0)
	{
		int S = Out_FontPicture.size();
		for (int i = 0; i < S;++i)
		{
			delete(Out_FontPicture[i]);
		}

		Out_FontPicture.clear();
	}*/

	//��������
	int sz = Ipt_Distorage.size();
	for (int i = 0; i < sz ;++i)
	{
		FontPicture fp;
		FontPicture *fp2 = new FontPicture();

		//update! �����µ�

		fp = _Generater->getOrlFontImageFp(Ipt_Distorage[i].data());
		_TransTemp.TransForm(fp,(*fp2));

		fp2->dictionaryType = (Field)DicType;

		Out_FontPicture.push_back(fp2);
	}
	
}

volatile bool CBI_TableDictionary::isFontSync()
{
	return m_SyncFontPicture;
}

bool CBI_TableDictionary::isShallowCopy()
{
	return shallowCopy;
}

bool CBI_TableDictionary::getIgnoreProtectFollow() const
{
	return spProtectFollow;
}

void CBI_TableDictionary::setIgnoreProtectFollow(bool val)
{
	if (writeLock)
	{
		qWarning(U8c("set IgnoreOverrun������������ֹ"));
		return;
	}

	spProtectFollow = val;
	m_SyncFontPicture = false;
}

void CBI_TableDictionary::setIgnoreOverrun(bool val)
{
	if (writeLock)
	{
		qWarning(U8c("set IgnoreOverrun������������ֹ"));
		return;
	}

	spOverrun = val;
	m_SyncFontPicture = false;
}

bool CBI_TableDictionary::getIgnoreOverrun() const
{
	return spOverrun;
}

volatile bool CBI_TableDictionary::getWriteLock() const
{
	return writeLock;
}

void CBI_TableDictionary::setWriteLock(volatile bool val)
{
	writeLock = val;
}

volatile bool CBI_TableDictionary::isOcrReady()
{
	return writeLock && m_SyncFontPicture;
}

void CBI_TableDictionary::setDrawingType(const DrawingType & dt)
{
	if (writeLock)
	{
		qWarning(U8c("set DrawingType������������ֹ"));
		return;
	}
	*m_DrawingType = dt;
	m_SyncFontPicture = false;
}

DrawingType CBI_TableDictionary::getDrawingType()
{
	return *m_DrawingType;
}

bool CBI_TableDictionary::isPublicDictionary()
{
	if (Ptr_DrawingBook == nullptr)  //ָ����DrawingBook���ǹ��е�,ָ��ΪNull����˽���ֵ�
	{
		return false;
	}else
	{
		return true;
	}
}

void CBI_TableDictionary::numInit()
{
	std::string numGroup[] = {"0","1","2","3","4","5","6","7","8","9","/"};


	Distorage[Field::��������] = std::vector<std::string>(numGroup,numGroup + 11);
}

void CBI_TableDictionary::setConditionSingleSwitch(int bl)
{
	switch (bl)
	{
	case 0:
		m_conditionSingleSwitch = 0;
		break;
	case 1:
		m_conditionSingleSwitch = 1;
			break;
	case 2:
		m_conditionSingleSwitch = 2;
				break;
	default:
		m_conditionSingleSwitch = 0;
		break;
	}

}


void read(const cv::FileNode& node, CBI_TableDictionary& x,const CBI_TableDictionary& default_value /*= CBI_TableDictionary(nullptr)*/)
{
	if (node.empty())
	{
		qFatal(U8c("CBI_TableDictionary ��ȡʧ��!!!"));
	}else
	{
		x.read(node);
	}
}

void write(cv::FileStorage& fs, const std::string& name, const CBI_TableDictionary& x)
{
	x.write(fs);
}


