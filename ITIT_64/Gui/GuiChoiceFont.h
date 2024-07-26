#ifndef GUICHOICEFONT_H
#define GUICHOICEFONT_H

#include <QDialog>
#include <QFont>
#include <cxcore.h>
#include "GuiShowImg.h"


namespace Ui {
	class IkChoiceFontClass;
} // namespace Ui


struct FontWordProprety; 
struct FontPictureProprety; 
class DrawingBook;  
class IfFontOrlPictureGenerater; 
class FontPictureGenerater;

class QFontDialog;

class DrawingType;
class DrawingTypeList;
class GuiSimilar;

//选择字体界面的GUI
class GuiChoiceFont : public QDialog
{
	Q_OBJECT

protected:
	bool selfProprety;  //自身使用的属性

	IfFontOrlPictureGenerater * m_FontGenerater;  //字体生成引擎
	FontWordProprety * m_WordProperty;  //字体属性

	FontPictureGenerater* m_FontGeneraterFull; //全功能的字体引擎

	DrawingBook * Ptr_DrawingBook;  //操作对象

	QFontDialog * m_FontDialog;		//字体对话框
	
	GuiSimilar *m_GuiSimilar;  //界面的指针界面

	FontWordProprety * getWordProprety();  //获取当前操作的 字体属性。 有DrawingBook的优先用DrawingBook

	void GuiInit();  //界面初始化部分
	void ImgProcInit(); //图像处理有关初始化部分

	DrawingTypeList * Ptr_DwTypeList;   //现存图纸种类指针

	

	void calSheetList(DrawingBook * ptr ); //从Sheet中抽

	cv::Mat m_Sample; //样例图
	cv::Mat m_Tample;  //生成图
	QString m_StrSample; //样例字
	

public:
	GuiChoiceFont(QWidget *parent = nullptr);
	~GuiChoiceFont();

	QFont m_Font;
	//自用属性

	void setGui(const DrawingType & _dt);  // 从数据到界面
	void getGui(DrawingType& _dt);		   // 从界面到数据

	

public slots:
	//对外接口配置相关
	void slot_setDrawingBook(DrawingBook* _Tar);  //设置图纸簿
	void slot_setDrawingTypeList(DrawingTypeList *_Tar);  //设置
	void slot_usingDrawingType(int choiceNum); //使用既有的存储的DrawingType
	void slot_setFontWord(FontWordProprety* fwp);  //给nullptr表示使用 自己的。使用Drawingbook中的 图纸种类作为后台数据

	void slot_writeFontWordPrep(/*FontPictureProprety**/ void* fpp); //给sim模块的结论写到界面上的接口函数

	//界面显示相关
	void slot_ShowString1(QString str);
	void slot_ShowString2(QString str);
	void slot_ShowString3(QString str);
	void slot_ShowString4(QString str);

	void slot_updateSimShow(); //更新人工拟合结果窗口

	void slot_updateAllWordProp();  //全体参数更新
	void slot_updateAllImg();  //更新全体图片


	//按钮触发相关
	void slot_FontDialog();  //显示字体选择对话框
	void slot_FontDialog_Accept();

	void slot_GenerateFontPicture();  //生成字典图片

	//配置设置相关
	void slot_setStretch(double Roit); //设置字体的压缩比
	void slot_setWordSpacing(double Spacing); //设置字体的间距

	//转换相关
	void slot_SetGui2Prop();  //将Fontdialog中的数据写入m_Font中
	void slot_Prop2SetGui();  //数据上载界面
	void slot_simClick( );  //抽选拟合按钮按下
	void slot_saveNewType( );  //保存最新的预存

	void slot_pushOKButton( ); //按下确认按钮
	
public :
	

signals:
	void signal_calDictionayDialog();  //呼唤字典文件


private :


	Ui::IkChoiceFontClass *ui;


};

#endif // GUICHOICEFONT_H
