#include "zoom_widget.h"
#include <QFontMetrics>

ZoomWidget::ZoomWidget(int value, QWidget *parent)
    : QSpinBox(parent)
{
    setButtonSymbols(QAbstractSpinBox::NoButtons);
    setRange(1, 500);
    setSuffix(" %");
    setValue(value);
    setToolTip("Zoom Level");
    setStatusTip(toolTip());
    setAlignment(Qt::AlignCenter);
}

ZoomWidget::~ZoomWidget()
{

}

//QSize ZoomWidget::minimumSizeHint() const
//{
//    int height = minimumSizeHint().height();
//    QFontMetrics fm(font());
//    int width = fm.width(QString::number(maximum()));
//    return QSize(width, height);
//}

