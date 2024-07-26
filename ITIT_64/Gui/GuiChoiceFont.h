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

//ѡ����������GUI
class GuiChoiceFont : public QDialog
{
	Q_OBJECT

protected:
	bool selfProprety;  //����ʹ�õ�����

	IfFontOrlPictureGenerater * m_FontGenerater;  //������������
	FontWordProprety * m_WordProperty;  //��������

	FontPictureGenerater* m_FontGeneraterFull; //ȫ���ܵ���������

	DrawingBook * Ptr_DrawingBook;  //��������

	QFontDialog * m_FontDialog;		//����Ի���
	
	GuiSimilar *m_GuiSimilar;  //�����ָ�����

	FontWordProprety * getWordProprety();  //��ȡ��ǰ������ �������ԡ� ��DrawingBook��������DrawingBook

	void GuiInit();  //�����ʼ������
	void ImgProcInit(); //ͼ�����йس�ʼ������

	DrawingTypeList * Ptr_DwTypeList;   //�ִ�ͼֽ����ָ��

	

	void calSheetList(DrawingBook * ptr ); //��Sheet�г�

	cv::Mat m_Sample; //����ͼ
	cv::Mat m_Tample;  //����ͼ
	QString m_StrSample; //������
	

public:
	GuiChoiceFont(QWidget *parent = nullptr);
	~GuiChoiceFont();

	QFont m_Font;
	//��������

	void setGui(const DrawingType & _dt);  // �����ݵ�����
	void getGui(DrawingType& _dt);		   // �ӽ��浽����

	

public slots:
	//����ӿ��������
	void slot_setDrawingBook(DrawingBook* _Tar);  //����ͼֽ��
	void slot_setDrawingTypeList(DrawingTypeList *_Tar);  //����
	void slot_usingDrawingType(int choiceNum); //ʹ�ü��еĴ洢��DrawingType
	void slot_setFontWord(FontWordProprety* fwp);  //��nullptr��ʾʹ�� �Լ��ġ�ʹ��Drawingbook�е� ͼֽ������Ϊ��̨����

	void slot_writeFontWordPrep(/*FontPictureProprety**/ void* fpp); //��simģ��Ľ���д�������ϵĽӿں���

	//������ʾ���
	void slot_ShowString1(QString str);
	void slot_ShowString2(QString str);
	void slot_ShowString3(QString str);
	void slot_ShowString4(QString str);

	void slot_updateSimShow(); //�����˹���Ͻ������

	void slot_updateAllWordProp();  //ȫ���������
	void slot_updateAllImg();  //����ȫ��ͼƬ


	//��ť�������
	void slot_FontDialog();  //��ʾ����ѡ��Ի���
	void slot_FontDialog_Accept();

	void slot_GenerateFontPicture();  //�����ֵ�ͼƬ

	//�����������
	void slot_setStretch(double Roit); //���������ѹ����
	void slot_setWordSpacing(double Spacing); //��������ļ��

	//ת�����
	void slot_SetGui2Prop();  //��Fontdialog�е�����д��m_Font��
	void slot_Prop2SetGui();  //�������ؽ���
	void slot_simClick( );  //��ѡ��ϰ�ť����
	void slot_saveNewType( );  //�������µ�Ԥ��

	void slot_pushOKButton( ); //����ȷ�ϰ�ť
	
public :
	

signals:
	void signal_calDictionayDialog();  //�����ֵ��ļ�


private :


	Ui::IkChoiceFontClass *ui;


};

#endif // GUICHOICEFONT_H
