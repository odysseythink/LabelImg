#include "label_dialog.h"
#include "ui_label_dialog.h"
#include "common.h"
#include <QPushButton>

LabelDialog::LabelDialog(QStringList listItem, QWidget *parent,QString text) :
    QDialog(parent),
    ui(new Ui::LabelDialog)
{
    ui->setupUi(this);
    ui->m_iLblEdit->setText(text);
    ui->m_iLblEdit->setValidator(labelValidator());
    connect(ui->m_iLblEdit, SIGNAL(editingFinished()), this, SLOT(postProcess()));

    model.setStringList(listItem);
    completer.setModel(&model);
    ui->m_iLblEdit->setCompleter(&completer);

    QPushButton* pBtn = ui->m_iBtnBox->button(QDialogButtonBox::Ok);
    if(pBtn != nullptr){
       pBtn->setIcon(*newIcon("done"));
    }
    pBtn = ui->m_iBtnBox->button(QDialogButtonBox::Cancel);
    if(pBtn != nullptr){
       pBtn->setIcon(*newIcon("undo"));
    }
    connect(ui->m_iBtnBox, SIGNAL(accepted()), this, SLOT(validate()));
    connect(ui->m_iBtnBox, SIGNAL(rejected()), this, SLOT(reject()));

    if (listItem.size() > 0){
        for (auto item : listItem){
            ui->listWidget->addItem(item);
        }
        connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(listItemClick(QListWidgetItem *)));
        connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(listItemDoubleClick(QListWidgetItem *)));
        ui->listWidget->setVisible(true);
    } else {
        ui->listWidget->setVisible(false);
    }
}

LabelDialog::~LabelDialog()
{
    delete ui;
}


void LabelDialog::validate(){
    ui->m_iLblEdit->setText(ui->m_iLblEdit->text().trimmed());
    accept();
}

void LabelDialog::postProcess(){
    ui->m_iLblEdit->setText(ui->m_iLblEdit->text().trimmed());
}
QString LabelDialog::popUp(QString text,  bool ismove){
    ui->m_iLblEdit->setText(text);
    ui->m_iLblEdit->setSelection(0, text.size());
    ui->m_iLblEdit->setFocus(Qt::PopupFocusReason);
    if (ismove) move(QCursor().pos());
    if(exec()){
        return ui->m_iLblEdit->text();
    } else {
        return "";
    }
}
void LabelDialog::listItemClick(QListWidgetItem *item){
    QString  text = item->text().trimmed();
    ui->m_iLblEdit->setText(text);
}

void LabelDialog::listItemDoubleClick(QListWidgetItem *item){
    listItemClick(item);
    validate();
}
