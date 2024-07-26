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

//拷贝构造,默认是浅拷贝
CBI_TableDictionary::CBI_TableDictionary(const CBI_TableDictionary &copy,bool deep )
{

	if (copy.getWriteLock())
	{
		qWarning(U8c("拷贝对象以上锁,未能拷贝"));

		return;
	}

	if (deep)  //深度拷贝的话
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

		m_SyncFontPicture = copy.m_SyncFontPicture;  //标签同步
	}
	else  //默认浅拷贝的话
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
		qWarning(U8c("setDistorage 由于上锁以阻止"));
		return;
	}

	kxConfigReader* kxCr = new kxConfigReader();

	kxCr->g_Path = QString(DevInfoPath.data());



	if (!kxCr->GetMapAttribute())
	{
		//抛异常处理

		throw std::logic_error("Error GetMapAttribute");
	}

	
	
	Distorage[Field::区段名称].clear();
	
	
	int size = kxCr->_AllTrack.size();

	for (int i = 0;i < size;i++)
	{
		if (QString(kxCr->_AllTrack[i]->m_sDevName.data()).toUpper() != "NULL")
		{
			if (kxCr->_AllTrack[i]->m_bVisible == true)
			{
				Distorage[Field::区段名称].push_back(kxCr->_AllTrack[i]->m_sDevName);
			}
		}
	}

	Distorage[Field::道岔名称].clear();
	size = kxCr->_AllSwitch.size();
	for (int i = 0;i < size;i++)
	{
		QString str = kxCr->_AllSwitch[i]->m_sLogicalName.data( );

		if (str.toUpper() != "NULL")
		{
			bool ctn = false;
			for (int j = 0;j < Distorage[Field::道岔名称].size( );j++)  //3/7这种会算次
			{
				if (kxCr->_AllSwitch[i]->m_sLogicalName == Distorage[Field::道岔名称][j])
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
				Distorage[Field::道岔名称].push_back(kxCr->_AllSwitch[i]->m_sLogicalName);
			}


			
		}
	}

	Distorage[Field::调车信号机名称].clear();
	Distorage[Field::列车信号机名称].clear();
	size = kxCr->_AllSingal.size();
	for (int i = 0;i < size;i++)
	{
		if (QString(kxCr->_AllSingal[i]->m_sDevName.data()).toUpper() != "NULL")
		{
			if (kxCr->_AllSingal[i]->m_szDevStyle == 7)  //调车信号
			{
				Distorage[Field::调车信号机名称].push_back(kxCr->_AllSingal[i]->m_sDevName);
			}else
			{
				Distorage[Field::列车信号机名称].push_back(kxCr->_AllSingal[i]->m_sDevName);
			}
		}
	}

	
	Distorage[Field::列车按钮名称].clear();
	Distorage[Field::调车按钮名称].clear();
	size = kxCr->_AllButton.size();
	for (int i = 0;i < size;i++)
	{
		if (QString(kxCr->_AllButton[i]->m_sDevName.data()).toUpper() != "NULL")
		{
			if ((kxCr->_AllButton[i]->m_btSingalClass & 0x02) == 0x02)  //列车按钮
			{
				Distorage[Field::列车按钮名称].push_back(kxCr->_AllButton[i]->m_sDevName);
			}else
			{
				Distorage[Field::调车按钮名称].push_back(kxCr->_AllButton[i]->m_sDevName);
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
		qWarning(U8c("setDistorage 由于上锁以阻止"));
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
		//一人存一遍
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
		qWarning(U8c("CBI_TableDictionary::read 由于上锁以阻止"));
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

	m_SyncFontPicture = false;  //没有
}

void CBI_TableDictionary::slot_GenerateFontPicture( )
{
	if (writeLock)
	{
		qWarning(U8c("slot_GenerateFontPicture 由于上锁以阻止"));
		return;
	}


	if (Ptr_DrawingBook != nullptr)
	{
		Clear_FontPicture();

		/*update! -> 等cv引擎写好了需要改进*/
		IfFontOrlPictureGenerater *_Generater = new FontGeneraterQ();
		_Generater->setFontProperty(Ptr_DrawingBook->getDrawingType()->_FontProprety);


		//依次生成对应的 图片文件
		for (int i = 0;i < DictionaryFiledMax;++i)
		{
			switch(i)
			{
			case Field::道岔名称:
				{
					//道岔特殊处理 加反位标志
					std::vector<std::string> SwitchNormal;
					
					for (int j = 0; j < Distorage[Field::道岔名称].size();++j)
					{
						SwitchNormal.push_back(Distorage[Field::道岔名称][j]);
						SwitchNormal.push_back("(" +  Distorage[Field::道岔名称][j] + ")");

					}
					
					GenerateFontPicture(SwitchNormal,_FontPicture[i],Field::道岔名称,
						_Generater,Ptr_DrawingBook->getDrawingType()->_GraphicTransTemp);

				}break;

			case Field::带防道岔: 
				{
					std::vector<std::string> SwitchProtectFollow;
					//没有指定 带防道岔 默认所有都有带动防护

					if (spProtectFollow)
					{
						SwitchProtectFollow = Distorage[Field::带防道岔];  //有指定直接使用指定

						for (int j = 0; j < Distorage[Field::道岔名称].size();++j)
						{
							SwitchProtectFollow.push_back("[(" + Distorage[Field::道岔名称][j] + ")]");
							SwitchProtectFollow.push_back("[" + Distorage[ Field::道岔名称][j] + "]");
						}

					}else
					{
						for (int j = 0; j < Distorage[Field::道岔名称].size();++j)
						{
							SwitchProtectFollow.push_back("[(" + Distorage[Field::道岔名称][j] + ")]");
							SwitchProtectFollow.push_back("[" + Distorage[ Field::道岔名称][j] + "]");
							SwitchProtectFollow.push_back("{(" + Distorage[ Field::道岔名称][j] + ")}");
							SwitchProtectFollow.push_back("{" + Distorage[ Field::道岔名称][j] + "}");
						}

						
					}

						GenerateFontPicture(SwitchProtectFollow,_FontPicture[i],Field::带防道岔,
							_Generater,Ptr_DrawingBook->getDrawingType()->_GraphicTransTemp);
					
				}break;

			case Field::条件道岔: 
				{
					
						/*
						int sz = _FontPicture[Field::道岔名称].size();
						for (int j = 0;j < sz ; ++j)
						{
							FontPicture *fp = new FontPicture(*(_FontPicture[Field::道岔名称][j]));
							_FontPicture[Field::超限条件].push_back(fp);
						}
						*/

						//道岔特殊处理 加反位标志
						std::vector<std::string> SwitchOverRun;

						for (int j = 0; j < Distorage[Field::道岔名称].size();++j)
						{

							//如果已经开启了条件道岔为单动道岔
							if (m_conditionSingleSwitch == 1)
							{
								std::string Str = Distorage[Field::道岔名称][j];
									
								int idx = Str.find('/');

								if (idx > 0)
								{
									std::string StrQ  = Str.substr(0,idx );  //前半部分
									std::string StrH  = Str.substr(idx + 1, Str.size() - 1); //后半部分

									SwitchOverRun.push_back("<" + StrQ + ">");
									SwitchOverRun.push_back("<(" +  StrQ + ")>");

									SwitchOverRun.push_back("<" + StrH + ">");
									SwitchOverRun.push_back("<(" +  StrH + ")>");

								}else
								{
									SwitchOverRun.push_back("<" + Distorage[Field::道岔名称][j] + ">");
									SwitchOverRun.push_back("<(" +  Distorage[Field::道岔名称][j] + ")>");
								}

							}else if (m_conditionSingleSwitch == 2)
							{

								std::string Str = Distorage[Field::道岔名称][j];

								int idx = Str.find('/');

								if (idx > 0)
								{
									std::string StrQ  = Str.substr(0,idx );  //前半部分
									std::string StrH  = Str.substr(idx + 1, Str.size() - 1); //后半部分

									SwitchOverRun.push_back("<" + StrQ + ">");
									SwitchOverRun.push_back("<(" +  StrQ + ")>");

									SwitchOverRun.push_back("<" + StrH + ">");
									SwitchOverRun.push_back("<(" +  StrH + ")>");

									SwitchOverRun.push_back("<" + Distorage[Field::道岔名称][j] + ">");
									SwitchOverRun.push_back("<(" +  Distorage[Field::道岔名称][j] + ")>");

								}else
								{
									SwitchOverRun.push_back("<" + Distorage[Field::道岔名称][j] + ">");
									SwitchOverRun.push_back("<(" +  Distorage[Field::道岔名称][j] + ")>");
								}

							}
							else
							{
								SwitchOverRun.push_back("<" + Distorage[Field::道岔名称][j] + ">");
								SwitchOverRun.push_back("<(" +  Distorage[Field::道岔名称][j] + ")>");
							}
								
						}

						SwitchOverRun.push_back("<>"); //条件绝对超限
						SwitchOverRun.push_back("< >"); //条件绝对超限

						Distorage[Field::敌对信号道岔] = SwitchOverRun;

						if (!spProtectFollow)
						{
							//给了指定的就按指定的来
							std::vector<std::string> dis = Distorage[i];

							dis.push_back("<>");
							dis.push_back("< >");

							GenerateFontPicture(dis,_FontPicture[i],Field::条件道岔,
								_Generater,Ptr_DrawingBook->getDrawingType()->_GraphicTransTemp);

							
						}else
						{
							
							GenerateFontPicture(SwitchOverRun,_FontPicture[i],Field::条件道岔,
								_Generater,Ptr_DrawingBook->getDrawingType()->_GraphicTransTemp);
						}
						
						GenerateFontPicture(SwitchOverRun,_FontPicture[Field::敌对信号道岔],Field::敌对信号道岔,
							_Generater,Ptr_DrawingBook->getDrawingType()->_GraphicTransTemp);
					

				}break;
			case Field::敌对信号道岔: break;  //跟着别人生成在上面
			case Field::敌对信号:
				{
					//敌对信号给列车信号加D,给调车信号机加L
					Distorage[Field::敌对信号].clear();

					int sz = Distorage[Field::列车信号机名称].size();

					for (int j = 0; j < sz; ++j)
					{
						Distorage[Field::敌对信号].push_back( Distorage[Field::列车信号机名称][j] );
						Distorage[Field::敌对信号].push_back( Distorage[Field::列车信号机名称][j] + "D");
						Distorage[Field::敌对信号].push_back( Distorage[Field::列车信号机名称][j] + "L");
					}

					sz = Distorage[Field::调车信号机名称].size();
					for (int j = 0; j < sz;++j)
					{
						Distorage[Field::敌对信号].push_back( Distorage[Field::调车信号机名称][j] );
						Distorage[Field::敌对信号].push_back( Distorage[Field::调车信号机名称][j] + "L");
					}

					GenerateFontPicture(Distorage[Field::敌对信号],_FontPicture[i],Field::敌对信号,
						_Generater,Ptr_DrawingBook->getDrawingType()->_GraphicTransTemp);


				}break;

			default :  //以上的都是要特殊处理的 下面是普通的
				{
					GenerateFontPicture(Distorage[i],_FontPicture[i],i,
						_Generater,Ptr_DrawingBook->getDrawingType()->_GraphicTransTemp);
				}break;
			}

		}
		
		shallowCopy = false; //自己生成后默认为深度存储构造
		delete(_Generater);
		m_SyncFontPicture = true;   //生成后就是同步的了
		 
	}else
	{
		qWarning(U8c("没有指定 从属的联锁簿 无法生成"));
	}

}

void CBI_TableDictionary::Clear_FontPicture()
{
	if (writeLock)
	{
		qWarning(U8c("清除已经上锁数据"));
	}

	for (int i = 0;i< DictionaryFiledMax;++i)
	{
		int s = _FontPicture[i].size();
		if (s > 0)
		{
			if (!shallowCopy)  //不是浅拷贝需要删图
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
			if (!shallowCopy)  //不是浅拷贝需要删图
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

	//外部清空
	/*if (Out_FontPicture.size() != 0)
	{
		int S = Out_FontPicture.size();
		for (int i = 0; i < S;++i)
		{
			delete(Out_FontPicture[i]);
		}

		Out_FontPicture.clear();
	}*/

	//依次生成
	int sz = Ipt_Distorage.size();
	for (int i = 0; i < sz ;++i)
	{
		FontPicture fp;
		FontPicture *fp2 = new FontPicture();

		//update! 换个新的

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
		qWarning(U8c("set IgnoreOverrun由于上锁以阻止"));
		return;
	}

	spProtectFollow = val;
	m_SyncFontPicture = false;
}

void CBI_TableDictionary::setIgnoreOverrun(bool val)
{
	if (writeLock)
	{
		qWarning(U8c("set IgnoreOverrun由于上锁以阻止"));
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
		qWarning(U8c("set DrawingType由于上锁以阻止"));
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
	if (Ptr_DrawingBook == nullptr)  //指定了DrawingBook就是公有的,指定为Null就是私有字典
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


	Distorage[Field::数字序列] = std::vector<std::string>(numGroup,numGroup + 11);
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
		qFatal(U8c("CBI_TableDictionary 读取失败!!!"));
	}else
	{
		x.read(node);
	}
}

void write(cv::FileStorage& fs, const std::string& name, const CBI_TableDictionary& x)
{
	x.write(fs);
}


