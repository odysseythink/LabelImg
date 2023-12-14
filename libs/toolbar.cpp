#include "toolbar.h"
#include <QLayout>
#include <QtGlobal>
#include <QWidgetAction>

QSize ToolButton::minSize=QSize(60, 60);

ToolButton::ToolButton(QWidget *parent)
    : QToolButton(parent)
{

}

QSize ToolButton::minimumSizeHint() const
{
    auto ms = minimumSizeHint();
    ToolButton::minSize.scale(qMax(ms.width(), minSize.width()), qMax(ms.height(), minSize.height()), Qt::IgnoreAspectRatio);
    return ToolButton::minSize;
}


ToolBar::ToolBar(QString title, QWidget *parent) : QToolBar(title, parent)
{
    auto mainlayout = layout();
    mainlayout->setSpacing(0);
    mainlayout->setContentsMargins(0,0,0,0);
    setContentsMargins(0,0,0,0);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
}

ToolBar::ToolBar(QWidget *parent)
 : QToolBar(parent)
{
    auto mainlayout = layout();
    mainlayout->setSpacing(0);
    mainlayout->setContentsMargins(0,0,0,0);
    setContentsMargins(0,0,0,0);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
}

ToolBar::~ToolBar()
{

}

void ToolBar::addAction(QAction* action){
    if (qobject_cast<QWidgetAction* >(action) != nullptr)
        return addAction(action);
    auto btn = new ToolButton();
    btn->setDefaultAction(action);
    btn->setToolButtonStyle(toolButtonStyle());
    addWidget(btn);
}


void ToolBar::addActions(QList<QObject* > actions){
    for (auto action : actions){
        if (action  == nullptr)
            addSeparator();
        else if (qobject_cast<QAction*>(action) != nullptr)
            addAction(qobject_cast<QAction*>(action));
    }
}
