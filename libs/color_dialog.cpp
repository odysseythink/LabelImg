#include "color_dialog.h"
#include <QLayout>

ColorDialog::ColorDialog(QWidget *parent)
    : QColorDialog(parent)
{
    setOption(QColorDialog::ShowAlphaChannel);
//    # The Mac native dialog does not support our restore button.
    setOption(QColorDialog::DontUseNativeDialog);
//    # Add a restore defaults button.
//    # The default is set at invocation time, so that it
//    # works across dialogs for different elements.
//    self.default = None;
    QWidget* pWidget= layout()->itemAt(1)->widget();
    bb = qobject_cast<QDialogButtonBox*>(pWidget);
    bb->addButton(QDialogButtonBox::RestoreDefaults);
    connect(bb, SIGNAL(clicked(QAbstractButton *)), this, SLOT(checkRestore(QAbstractButton *)));
}

ColorDialog::~ColorDialog()
{

}

QColor ColorDialog::getColor(QColor value, QString title, QColor defaultColor1){
    defaultColor = defaultColor1;
    setWindowTitle(title);
    if (value.isValid())
        setCurrentColor(value);
    if(exec()){
        return currentColor();
    } else {
        return QColor();
    }
}

void ColorDialog::checkRestore(QAbstractButton *button)
{
    if ((bb->buttonRole(button) & QDialogButtonBox::ResetRole) && defaultColor.isValid())
        setCurrentColor(defaultColor);
}
