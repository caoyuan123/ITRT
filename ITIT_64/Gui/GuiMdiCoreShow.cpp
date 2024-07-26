#include "GuiMdiCoreShow.h"
#include "GuiDwSheetManager.h"
#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>





GuiMdiCoreShow::GuiMdiCoreShow(GuiDwSheetManager* parent) 
	: QWidget(parent)
{
	m_blClose = false;
	m_Parent = parent;

	vlayout  =  new QVBoxLayout(this);
	vlayout->setMargin(0);
	vlayout->setSpacing(0);

	ShowWidget = nullptr;
}

GuiMdiCoreShow::~GuiMdiCoreShow()
{
	
}

void GuiMdiCoreShow::closeEvent(QCloseEvent *e)
{
	if (m_blClose)
	{
		e->accept();
	}else
	{
		emit(hideAccept());
		e->ignore();
	}
}

void GuiMdiCoreShow::setShowWidget(QWidget* subWidget)
{

	if (ShowWidget != nullptr)
	{
		vlayout->removeWidget(ShowWidget);//将当前Widget退掉		
	}

	vlayout->addWidget(subWidget);
	ShowWidget = subWidget;
}

GuiDwSheetManager * GuiMdiCoreShow::getParent()
{
	return m_Parent;
}

void GuiMdiCoreShow::removeShowWidget()
{
	if (ShowWidget != nullptr)
	{
		vlayout->removeWidget(ShowWidget);//将当前Widget退掉		
	}
	ShowWidget = nullptr;
}
