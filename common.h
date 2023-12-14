#ifndef _COMMON_H_
#define _COMMON_H_

#include <QRegExpValidator>
#include <QRegExp>
#include <QValidator>
#include <QIcon>
#include <QAction>
#include <QMenu>
#include <QList>
#include <QObject>


QValidator* labelValidator();
QIcon* newIcon(QString icon);
double distance(QPointF p);
QString fmtShortcut(QString text);
void AddActions(QWidget* widget, QList<QObject* > actions);
QColor generateColorByText(QString text);
#endif // _COMMON_H_
