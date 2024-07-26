#include "itit.h"
#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>
#include <opencv.hpp>
#include <highgui.h>

#include <GlobalSet.h>

#include "GuiFilter.h"
#include "guicvfactory.h"
#include "ImgProcCorrect.h"
#include "GuiCorrect.h"
#include "GuiChoiceZone.h"
#include "CBITableDef.h"
#include "ImgProcLine.h"

#include "kxConfigReader.h"
#include "DrawingBook.h"
#include "DrawingSheet.h"
#include "DrawingPart.h"

#include "GUINewProject.h"
#include "GuiDwSheetManager.h"
#include "GuiShowImg.h"
#include "GuiChoiceFont.h"

#include "Gui_CBITableDictionary.h"
#include "ImgProcFilter.h"
#include "GuiSimilar.h"
#include "FontSimilar.h"
#include "OCREngine.h"

#include "CBI_TableDictionary.h"
#include "MessyCodeTerminator.h"

#include "FontGeneraterQ.h"

#include "Lisense.h"
#include "QxlsxEngine.h"

#include "xlsxdocument.h"
#include "ComApx.h"

using namespace cv;

//#define _DEBUG 1

void MessageOutput(QtMsgType type, const QMessageLogContext &contextconst ,const QString &msg)
{ 
	static QMutex mutex;
	mutex.lock();

	QString txtMessage =  "[" + QDateTime::currentDateTime().toString("hh:mm:ss:zzz") +  "]";

	switch (type) {
	case QtDebugMsg:
		txtMessage += QString("[D] %1").arg(msg);
#ifdef _DEBUG
		

		printf(msg.toLocal8Bit().data());
		printf("\r\n");
#endif

		break;
	case QtWarningMsg:
		txtMessage += QString("[W] %1").arg(msg);
#ifdef _DEBUG
		printf(txtMessage.toLocal8Bit().data());
		printf("\r\n");
#endif
		break;
	case QtCriticalMsg:
		txtMessage += QString("[S] %1").arg(msg);

#ifdef _DEBUG
		printf(txtMessage.toLocal8Bit().data());
		printf("\r\n");
#endif
		break;
	case QtFatalMsg:
		txtMessage += QString("[F] %1").arg(msg);
#ifdef _DEBUG
		printf(txtMessage.toLocal8Bit().data());
		printf("\r\n");
#endif

	default:
		break;
	}
	txtMessage += QString("\r\n");

	QString text;
	text = QDateTime::currentDateTime().toString("yyyy-MM-dd");

	QFile file("log\\" + text + ".txt");


	if(file.open(QIODevice::WriteOnly | QIODevice::Append))
	{
		QTextStream out(&file);
		out << txtMessage;
	}
	file.flush();
	file.close();
	mutex.unlock();
}


class test :virtual public IfGUI
{

public:
	Mat tttt;

	test()
	{
		tttt = imread("E:\\Cpp\\0QtWithOpenCv_INTwoWeek\\EX9-底噪\\平面图联锁表 (2)-2.bmp");
	}
	virtual cv::Mat drawMat(void)
	{
		return tttt;
	}

	virtual cv::Mat showMat(void)
	{
		return tttt;
	}

};

//设置类初始化
void SetInit( )
{
	u_setMp.read();
	u_setCvOCR.read();
}

//初始化Qsetting的ini读写能力
void GlobalInit(QApplication &App)
{
	//QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));
	//QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));

	

	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));
	
	//qInstallMsgHandler(MessageOutput);

	qInstallMessageHandler(MessageOutput);


	App.setStyle(QStyleFactory::create(QObject::tr("Cleanlooks")));
//	App.setStyle(QStyleFactory::create(QObject::tr("Plastique")));


//	QSettings::Format format = QSettings::registerFormat("ini", IniReadFunc, IniWriteFunc);

	
	SetInit( );

	qDebug()  << "----------Application Start!---------------";

}

int main00(int argc, char *argv[])
{

	testZonemian(argc,argv);

	QApplication a(argc, argv);

	GlobalInit(a);

//	GuiCorrect gc;
//	gc.show();
	
	
	/*
	QGraphicsScene scene;
	GuiZoneChoice view;
	view.setScene(&scene);
	view.setForegroundBrush(QColor(255,255,255,100));
	view.resize(400,300);
	
	QGraphicsRectItem * item = new QGraphicsRectItem(0,0,100,100);
	GuiLine * gl = new GuiLine();
	scene.addItem(gl);
	scene.addItem(item);
	view.show();*/
/*
	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
	QGraphicsScene scene;
	scene.setSceneRect(-200,-150,400,300);
	for (int i= 0; i<5;++i)
	{
		MyItem *item = new MyItem;
		item->setColor(QColor(qrand() % 256,qrand() % 256,qrand() % 256));
		item->setPos(i * 50 - 90, -50);

		scene.addItem(item);
	}
	GuiZoneChoice view;

	GuiLine *line = new GuiLine;

	scene.addItem(line);
	scene.setSceneRect(0,0,1280,720);
	view.setScene(&scene);
	view.setBackgroundBrush(QPixmap("E:\\Cpp\\IMG_0862.JPG"));
	view.show();
	*/

//	DrawingSheet _testSheet;

//	_testSheet.testRead("E:\\Cpp\\test.jpg");


	return a.exec();
		
}
int mainDrawingSheet(int argc, char *argv[])
{

	testZonemian(argc,argv);

	QApplication a(argc, argv);

	GlobalInit(a);



//	DrawingSheet _testSheet;

//	_testSheet.testRead("E:\\Cpp\\test.jpg");


	return a.exec();
		
}

//线段识别的程序
int mainLine(int argc, char *argv[])
{
	QApplication a(argc, argv);

	GlobalInit(a);

	cv::Mat mat = imread("E:\\Cpp\\IMG_0862.JPG");
	cv::Mat g;
	cv::cvtColor(mat,g,CV_BGR2GRAY);

	ImgProcLine ipl;
	ipl.ImgProc(g);

	QGraphicsScene scene;
	GuiChoiceLine view;
	view.setScene(&scene);
	view.setForegroundBrush(QColor(255,255,255,50));
	view.setBackgroundBrush(QPixmap("E:\\Cpp\\bj.png"));
	

	QPixmap map("E:\\Cpp\\IMG_0862.JPG");
	view.resize(map.size());

	QGraphicsPixmapItem* backGroud = new QGraphicsPixmapItem(map);
	scene.addItem(backGroud);

	view.backGround = backGroud;
	for (int i = 0;i < ipl.HLine.size();i++)
	{
		GuiLine *line = new GuiLine(ipl.HLine[i]);
		line->setColor(QColor(0,128,255));
		scene.addItem(line);
	}

	for (int i = 0;i < ipl.VLine.size();i++)
	{
		GuiLine *line = new GuiLine(ipl.VLine[i]);
		line->setColor(QColor(255,128,0));
		scene.addItem(line);
	}

	std::sort(ipl.VLine.begin(),ipl.VLine.end(),CvLine::CompareX);

	std::vector<cv::Rect> Vr;

	///////////////////////////////////////////////////////////////
	for(int i = 0;i < ipl.VLine.size() - 1;i++)
	{
		Vr.push_back(cv::Rect(ipl.VLine[i].Point1,ipl.VLine[i+1].Point2));
	}
	///////////////////////////////////////////////////////////////

	GuiChoiceZone gzc = GuiChoiceZone();
	gzc.setRect(Vr);
	gzc.show();

	view.show();
	return a.exec();
}

//区域选择
int mainZone(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GlobalInit(a);

	cv::Mat mat = cv::imread("E:\\Cpp\\IMG_0862.JPG");
	cv::Mat g;
	cv::cvtColor(mat,g,CV_BGR2GRAY);

	ImgProcLine ipl;
	ipl.ImgProc(g);

	QGraphicsScene *scene;
	QPixmap map("E:\\Cpp\\IMG_0862.JPG");
	QGraphicsPixmapItem* backGroud = new QGraphicsPixmapItem(map);
	std::sort(ipl.VLine.begin(),ipl.VLine.end(),CvLine::CompareX);
	std::vector<cv::Rect> Vr;
	///////////////////////////////////////////////////////////////
	for(int i = 0;i < ipl.VLine.size() - 1;i++)
	{
		Vr.push_back(cv::Rect(ipl.VLine[i].Point1,ipl.VLine[i+1].Point2));
	}
	///////////////////////////////////////////////////////////////

	GuiChoiceZone gzc = GuiChoiceZone();
	
	scene = gzc.scene();
	scene->addItem(backGroud);

	gzc.setRect(Vr);
	gzc.setBackgroundBrush(QPixmap("E:\\Cpp\\bj.png"));

	backGroud->setZValue(0);
	gzc.show();

	return a.exec();
}


void test()
{
	kxConfigReader TestCr;
	TestCr.g_Path = ("D:\\工作与学习\\【2019】\\7月\\牛车河\\牛车河 联锁V1.0.1 控显V1.0.1 监测V1.0.1\\牛车河S版\\牛车河控显V1.0.1\\MMI\\config");
	TestCr.GetMapAttribute();
}

int mainSet(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GlobalInit(a);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));



//	Config gf;

//	gf.readSettingQt();

	//test();




	system("pause");
	return a.exec();
}

//结合Sheet
int mainSheet(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GlobalInit(a);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

	DrawingSheet _testSheet;
	_testSheet.testRead("E:\\Cpp\\test.jpg");


	DrawingBook _testBook;


	DrawingType _testType;

//
    //获取

	//读取字典图


	//1. 主引擎计算

	//2. 

	//3.



	system("pause");
	return a.exec();
}

int mainRead(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GlobalInit(a);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

	DrawingBook* Db = new DrawingBook();
	GUINewProjectDialog* newProject = new GUINewProjectDialog();

	newProject->show();
	newProject->setDrawingBook(Db);

	return a.exec();
}

int mainGUidwst(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GlobalInit(a);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));
	
	DrawingSheet _testSheet;
	_testSheet.testRead("E:\\Cpp\\test.jpg");


	GuiDwSheetManager* dw = new GuiDwSheetManager(nullptr);
	dw->slot_setDrawingSheet(&_testSheet);
	dw->show();

	return a.exec();
}

int maindddd(int argc, char *argv[])  //GuiDictionary
{
	QApplication a(argc, argv);
	GlobalInit(a);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));

	Gui_CBITableDictionary *Gcd = new Gui_CBITableDictionary(nullptr);

	Gcd->show();


	return a.exec();
}

int mainFFF(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GlobalInit(a);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));

	GuiShowImg* gsi = new GuiShowImg();

	gsi->setMat(imread("E:\\Cpp\\1.png",cv::IMREAD_UNCHANGED));

	GuiCvFactory* gvf = new GuiCvFactory(nullptr,gsi);
	gvf->setZoomMode(2);
	gvf->show();
//	GuiCorrect* gc = new GuiCorrect(nullptr,_testSheet);
//	gc->show();

	return a.exec();
}

int mainFFFf(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GlobalInit(a);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));

	
	cv::Mat src = imread("E:\\Cpp\\test.jpg",cv::IMREAD_UNCHANGED);
	cv::Mat dst;


	/*
	GuiShowImg* gsi = new GuiShowImg();
	GraphicFilter* gf = new GraphicFilter();
	GuiCvFactory* gcf = new GuiCvFactory(nullptr,gf);

	gf->filterImgProc(src,dst);
	gsi->setMat(dst);
	GuiCvFactory* gcf2 = new GuiCvFactory(nullptr,gsi);


	gcf->show();
	gcf2->show();
	*/

	GuiFilter *gf = new GuiFilter(nullptr);
	gf->setMat(cv::imread("E:\\Cpp\\test.jpg"));
	gf->show();
	

	return a.exec();
}

int mainFC(int argc, char *argv[]) //调试FontChoices
{
	QApplication a(argc, argv);
	GlobalInit(a);

	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));
	//GuiCorrect* gc = new GuiCorrect(nullptr);
	//gc->show();


	GuiChoiceFont *gcf = new GuiChoiceFont(nullptr);

		gcf->show();
	

	return a.exec();
}

int mainGSGS(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GlobalInit(a);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));

	GuiSimilar *gs = new GuiSimilar(nullptr);
	gs->setBackGround(cv::imread("tempSave.png"));
	gs->slot_InitShow( );
	gs->show( );

	gs->m_Tools->show( );


	gs->resize(800,600);
	return a.exec( );
}


/////////调试OCR//////////////
int mainOOO(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GlobalInit(a);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));


	DrawingBook db = new DrawingBook(nullptr);

	cv::FileStorage fs("testSheet.yaml",cv::FileStorage::READ);

	cv::FileNode fn = fs["Project"];

	fn >> db;
	fs.release();

	db.m_Dictionary->slot_GenerateFontPicture();

	OCREngineBase _ocrSet(13,39,0.527217);

	


	OCREngineCv Engine(_ocrSet);

	Engine.OCRDrawingSheet(*(db.m_Pages[0]));

	//cv::imwrite("test.png",db.m_Pages[0]->_AllDrawingParts[0]->matFlt);
	
	//cv::FileStorage fs2("MidResult.yaml" , cv::FileStorage::WRITE);

	//fs2 << "DicTest" << db;

	//fs2.release();


	db.m_Pages[0]->toiLockTable92();


	QxlsxEngine ExcelEngin;

	ExcelEngin.initialize(true);

	ExcelEngin.open("D:\\test.xlsx");

	ExcelEngin.loadSheet("test");

	int rows = db.m_Pages[0]->out_CbiTableResult._value[0].size();

	ExcelEngin.setRangeCell(1,1, rows  , 15, db.m_Pages[0]->out_CbiTableResult.getExcelValue());

	return a.exec();
}

/////////调试Excel//////////////
int mainEEE(int argc, char *argv[])
{
	QApplication a(argc, argv);
//	GlobalInit(a);
//	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));
	
	CBITablePage92 cbi92;

	cv::FileStorage fs("testR.yaml",cv::FileStorage::READ);

	cv::FileNode fn = fs["TempR"];

	fn >> cbi92;
	fs.release();
	
	QxlsxEngine ExcelEngin;

	ExcelEngin.initialize(true);

	ExcelEngin.open("D:\\test.xlsx");

	ExcelEngin.loadSheet("test");

//	ExcelEngin.copySheet("TTT");

	int rows = cbi92.ExcelValue.size();
	
	for (int i =0;i < cbi92.ExcelValue.size();i++)
	{
		qDebug() << cbi92.ExcelValue[i];
	}
	
	ExcelEngin.setRangeCell(3,1, rows + 3 , 16,cbi92.getExcelValue());

	system("pause");

	return a.exec();
}

/////////调试近似
int mainSSS(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GlobalInit(a);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));

#ifdef _DEBUG
//	testFuc();
#endif // _DEBUG

	

	return a.exec();
}



int mainTTHTHTHTH(int argc, char *argv[])
{
	cv::Mat ts1 = imread("ts1.png");
	cv::Mat ts2 = imread("ts2.png");

	cv::Mat r;

	cv::matchTemplate(ts2,ts1,r,TM_CCOEFF_NORMED);
	cv::pow(r,5,r);
	
	double Max,Min;

	cv::minMaxLoc(r,&Min,&Max);

	double nsize = Max - Min + + 0.001;	


	std::vector<Mat> vImg;
	vImg.push_back(r);
	InputArrayOfArrays src_InputArrayOfArrays(vImg);

	std::vector<int> vHistSizes;
	std::vector<int> vChannels;	
	std::vector<float> vRange;

	int dims = 1; //二维数组么

	for (int i = 0; i< dims;i++)
	{
		vChannels.push_back(i);
		vHistSizes.push_back(100);
		vRange.push_back(0);
		vRange.push_back(1);

	}


	cv::Mat histMat;

	calcHist(src_InputArrayOfArrays,vChannels,noArray(),histMat,vHistSizes,vRange);

	histMat /= r.size().area();

	cout << histMat << endl;

	printf("%f",Max);

	imshow("11",ts1);
	imshow("22",ts2);
	imshow("R",r);
	imshow("hist",histMat);
	cvWaitKey();

	return 0;
}

int mainWWWWWWWWWWW()
{
	//1、产生数据
	uchar data[] = { 1,1,1,2,2,2,3,3,3,1,1,1,2,2,2,3,3,3,1,1,1,2,2,2,3,3,3 };
	Mat src_img(3, 3, CV_8UC3,data);
	Mat mats[3];
	split(src_img, mats);
	for (int i = 0; i < 3; i++)
	{
		cout << "channle = " << i << endl;
		cout << mats[i] << endl;
	}

	//2、计算图像中的最大值最小值；用于计算histSize和range参数;
	double minVal, maxVal;
	minMaxLoc(src_img, &minVal, &maxVal);
	int nMin = static_cast<int>(minVal);
	int nMax = static_cast<int>(maxVal);
	int nsize = nMax - nMin + 1;	

	//3、准备各类参数;
	std::vector<Mat> vImg;
	vImg.push_back(src_img);
	InputArrayOfArrays src_InputArrayOfArrays(vImg);


	std::vector<int> vChannels;
	std::vector<int>	vHistSizes;
	std::vector<float> vRanges;
	int dims = 1;
	for (int i = 0; i < dims; i++)
	{
		vChannels.push_back(i);
		vHistSizes.push_back(nsize);
		vRanges.push_back(nMin);
		vRanges.push_back(nMax + 1);
	}

	Mat img_Hist;
	calcHist(src_InputArrayOfArrays, vChannels, noArray(), img_Hist, vHistSizes, vRanges);

	cout << "灰度直方图" << endl;
	cout << img_Hist << endl;

	cvNamedWindow("t",CV_WINDOW_FREERATIO);

	imshow("t",img_Hist);

	cvWaitKey(0);

	return 0;
}


using namespace QXlsx;

int main666(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	// [1]  Writing excel file(*.xlsx)
	QXlsx::Document xlsxW("test.xlsx");
	
	if (!xlsxW.isLoadPackage( ))
	{
		qDebug( ) << "unPackage!";

		app.exec( );
		return 0;
	}




	xlsxW.selectSheet("3");
//	xlsxW.write("A4", "Hello Qt666!"); // write "Hello Qt!" to cell(A,1). it's shared string.




	if ( xlsxW.saveAs("Test.xlsx")) // save the document as 'Test.xlsx'
	{
		qDebug() << "[debug] success to write xlsx file";
	}
	else
	{
		qDebug() << "[debug][error] failed to write xlsx file";
	}

	qDebug() << "[debug] current directory is " << QDir::currentPath();

	// [2] Reading excel file(*.xlsx)
	Document xlsxR("Test.xlsx"); 
	if ( xlsxR.load() ) // load excel file
	{
		qDebug() << "[debug] success to load xlsx file.";

		int row = 1;
		int col = 1;
		Cell* cell = xlsxR.cellAt(row, col); // get cell pointer.
		if ( cell != NULL )
		{
			QVariant var = cell->readValue(); // read cell value (number(double), QDateTime, QString ...)
			qDebug() << "[debug] cell(1,1) is " << var; // Display value. It is 'Hello Qt!'.
		}
		else
		{
			qDebug() << "[debug][error] cell(1,1) is not set.";
		} 
	}
	else
	{
		qDebug() << "[debug][error] failed to load xlsx file.";
	}


	app.exec();

	return 0;

}


int mainexcel(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	/*
	QStringList data;

	data.push_back("1");
	data.push_back("2/4");
	data.push_back("(3)");

	QList<QStringList> dataTable;

	dataTable.push_back(data);
	dataTable.push_back(data);
	dataTable.push_back(data);

	QxlsxEngine qe;

	qe.open("test.xlsx");

	qe.addSheet("3");

	qe.setRangeCell(1,1,dataTable);

	qe.save();

	app.exec();*/

	std::ifstream ifs ("E:\\ITIT_project\\打包范例\\通号南站\\Result.xlsx");

	

	if (ifs.is_open()) {
		// print file:
		char c = ifs.get();
		while (ifs.good()) {
			std::cout << c;
			c = ifs.get();
		}
	}
	else {
		// show message:
		std::cout << "Error opening file";
	}
	
	getchar();

	return 0;
}




/////////调试核心计算函数代码///////////
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GlobalInit(a);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));

	if (!CheckRegist() && false)
	{
		QString str = U8s("产品未注册！");

		str+= "\r\n";
		str += getRegCode();

		QMessageBox  Mb(QMessageBox::Critical,U8s("启动失败"),str );

		Mb.show();

		return a.exec();
	}else
	{
		/*
		if (CheckRegist())
		{
			qDebug() << "Regist!";
		}else
		{
			qDebug() << "unRegist!";
		}*/

		QTranslator *myTr = new QTranslator();
		myTr->load("qt_zh_CN.qm");
		a.installTranslator(myTr);

		QTranslator *myZh = new QTranslator();
		myZh->load("widgets.qm");
		a.installTranslator(myZh);

		ITIT *itit = new ITIT( );
		itit->show();

		return a.exec();
	}

	return 0;
}



