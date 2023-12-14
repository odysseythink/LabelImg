#ifndef ZOOMWIDGET_H
#define ZOOMWIDGET_H

#include <QObject>
#include <QSpinBox>

class ZoomWidget : public QSpinBox
{
    Q_OBJECT
public:
    ZoomWidget(int value=100, QWidget *parent = nullptr);
    ~ZoomWidget();

//    virtual QSize minimumSizeHint() const;
};

#endif // ZOOMWIDGET_H
