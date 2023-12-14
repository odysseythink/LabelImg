#include "common.h"
#include <QtMath>
#include <QToolBar>
#include <QCryptographicHash>

QValidator* labelValidator(){
    return new QRegExpValidator(QRegExp("^[^ \t].+"), nullptr);
}


QIcon* newIcon(QString icon){
    return new QIcon("://resources/icons/" + icon);
}

double distance(QPointF p){
    return qSqrt(p.x() * p.x() + p.y() * p.y());
}

QString fmtShortcut(QString text){
    auto tmps = text.split("+");
//    mod, key = text.split('+', 1)
    QString mod = tmps.takeFirst();
    QString key = tmps.join("+");
    return QString("<b>%1</b>+<b>%2</b>").arg(mod).arg(key);
}


void AddActions(QWidget* widget, QList<QObject* > actions){
    QMenu* pMenu = qobject_cast<QMenu*>(widget);
    QToolBar* pToolBar = qobject_cast<QToolBar*>(widget);
    for (auto action : actions){
        if (action  == nullptr){
            if (pMenu != nullptr) pMenu->addSeparator();
            else if (pToolBar != nullptr) pToolBar->addSeparator();
        }else if (qobject_cast<QMenu*>(action) != nullptr){
            if (pMenu != nullptr) pMenu->addMenu(qobject_cast<QMenu*>(action));
        }else if (qobject_cast<QAction*>(action) != nullptr){
            if (pMenu != nullptr) pMenu->addAction(qobject_cast<QAction*>(action));
            else if (pToolBar != nullptr) pToolBar->addAction(qobject_cast<QAction*>(action));
        }
    }
}



QColor generateColorByText(QString text){
//    s = ustr(text);
    auto hashCode = QCryptographicHash::hash(text.toLocal8Bit(), QCryptographicHash::Sha256).toInt();
//    hashCode = int(hashlib.sha256(s.encode('utf-8')).hexdigest(), 16);
    auto r = int((hashCode / 255) % 255);
    auto g = int((hashCode / 65025)  % 255);
    auto b = int((hashCode / 16581375)  % 255);
    return QColor(r, g, b, 100);
}
