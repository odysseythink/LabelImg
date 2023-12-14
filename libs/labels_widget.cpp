#include "labels_widget.h"
#include "ui_labels_widget.h"

LabelsWidget::LabelsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LabelsWidget)
{
    ui->setupUi(this);
}

LabelsWidget::~LabelsWidget()
{
    delete ui;
}

bool LabelsWidget::UseDefaultLabel()
{
    return ui->m_iUseDefaultLabelEdit->isChecked();
}

QString LabelsWidget::DefaultLabel()
{
    return ui->m_iDefaultLabelEdit->text();
}

bool LabelsWidget::Difficult()
{
    return ui->m_iDiffcBtn->isChecked();
}

void LabelsWidget::SetDifficult(bool stat)
{
    ui->m_iDiffcBtn->setChecked(stat);
}

void LabelsWidget::SetEditAction(QAction *a)
{
    ui->m_iEditBtn->setDefaultAction(a);
}

void LabelsWidget::SetEditVisible(bool stat)
{
    ui->m_iEditBtn->setVisible(stat);
}

void LabelsWidget::__DifficultChanged(int stat)
{
    emit sigDifficultChanged(stat);
}

void LabelsWidget::__OnLabelSelectionChanged(QListWidgetItem *item){
    if (item != nullptr){
        emit sigLabelSelectionChanged(item);
    } else {
        auto items = ui->m_iLabelListWidget->selectedItems();
        if (items.size())
            emit sigLabelSelectionChanged(items[0]);
    }
}

void LabelsWidget::__OnEditLable(QListWidgetItem *item)
{
    if(item != nullptr){
        emit sigEditLable(item);
    }
}

void LabelsWidget::__OnLabelChanged(QListWidgetItem *item)
{
    if(item != nullptr){
        emit sigLabelChanged(item);
    }
}
