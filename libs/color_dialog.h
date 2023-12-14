#ifndef COLORDIALOG_H
#define COLORDIALOG_H

#include <QWidget>
#include <QColorDialog>
#include <QDialogButtonBox>
#include <QColor>

class ColorDialog : public QColorDialog
{
    Q_OBJECT
public:
    ColorDialog(QWidget *parent = nullptr);
    ~ColorDialog();
    QColor getColor(QColor value = QColor(), QString title = "", QColor defaultColor1 = QColor());

private slots:
    void checkRestore(QAbstractButton *button);

private:
    QDialogButtonBox* bb;
    QColor defaultColor;
};

#endif // COLORDIALOG_H
