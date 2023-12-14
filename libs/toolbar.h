#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QWidget>
#include <QToolBar>
#include <QToolButton>
#include <QSize>


class ToolButton : public QToolButton
{
    Q_OBJECT
public:
    explicit ToolButton(QWidget *parent = nullptr);
    ~ToolButton(){}
    virtual QSize minimumSizeHint() const;

signals:

private:
    static QSize minSize;
};

class ToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit ToolBar(QString title, QWidget *parent = nullptr);
    explicit ToolBar(QWidget *parent = nullptr);
    ~ToolBar();
    void addAction(QAction* action);
    void addActions(QList<QObject* > actions);

signals:

};

#endif // TOOLBAR_H
