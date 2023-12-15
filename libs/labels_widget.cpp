#include "labels_widget.h"
#include "ui_labels_widget.h"
#include <QDebug>
#include "common.h"
#include "libs/label_dialog.h"
#include "settings.h"

LabelsWidget::LabelsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LabelsWidget)
{
    ui->setupUi(this);
    m_iCanvas = nullptr;
    m_iLabelListMenu = nullptr;
    m_bNoSelectionSlot = false;
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

void LabelsWidget::ClearLabel()
{
    ui->m_iLabelListWidget->clear();
}

int LabelsWidget::LabelCount()
{
    return ui->m_iLabelListWidget->count();
}

void LabelsWidget::SelectLabel(int idx)
{
    if (idx < 0 || idx >= ui->m_iLabelListWidget->count()){
        qCritical("idx(%d) beyond range(0,%d)", idx, ui->m_iLabelListWidget->count());
        return;
    }
    ui->m_iLabelListWidget->setCurrentItem(ui->m_iLabelListWidget->item(idx));
    ui->m_iLabelListWidget->item(idx)->setSelected(true);
}

void LabelsWidget::RemLabel(Shape* shape)
{
    if (shape == nullptr){
        qCritical("invalid arg");
        return;
    }

    ui->m_iLabelListWidget->takeItem(ui->m_iLabelListWidget->row(shape));
}

QListWidgetItem *LabelsWidget::AddLabel(Shape* shape)
{
    if (shape == nullptr){
        qCritical("invalid arg");
        return nullptr;
    }
    shape->setFlags(shape->flags() | Qt::ItemIsUserCheckable);
    shape->setCheckState(Qt::Checked);
    shape->setBackground(generateColorByText(shape->text()));
    ui->m_iLabelListWidget->addItem(shape);
    return shape;
}

QListWidgetItem *LabelsWidget::GetLabelItem(int idx)
{
    if (idx < 0 || idx >= ui->m_iLabelListWidget->count()){
        qCritical("idx(%d) beyond range(0,%d)", idx, ui->m_iLabelListWidget->count());
        return nullptr;
    }
    return ui->m_iLabelListWidget->item(idx);
}

void LabelsWidget::ClearSelection()
{
    ui->m_iLabelListWidget->clearSelection();
}

QListWidgetItem *LabelsWidget::__CurrentItem()
{
    auto items = ui->m_iLabelListWidget->selectedItems();
    if (items.size())
        return items[0];
    return nullptr;
}

Shape *LabelsWidget::CurrentShape()
{
    QListWidgetItem *item = __CurrentItem();
    if(item != nullptr){
        return static_cast<Shape* >(item);
    }
    return nullptr;
}

bool LabelsWidget::NoShapes()
{
    return ui->m_iLabelListWidget->count() == 0;
}

void LabelsWidget::ToggleShapes(bool value)
{
    for (int iLoop = 0; iLoop < ui->m_iLabelListWidget->count(); iLoop++){
        ui->m_iLabelListWidget->item(iLoop)->setCheckState(value?Qt::Checked : Qt::Unchecked);
    }
}

void LabelsWidget::SelectShape(Shape *shape)
{
    if (shape != nullptr)
        shape->setSelected(true);
}

void LabelsWidget::__OnDifficultChanged(int stat)
{
    if(m_iCanvas == nullptr){
        return;
    }
    //    """ Function to handle difficult exampless
    //    Update on each object """
    if (!m_iCanvas->editing())
        return;

    auto item = __CurrentItem();
    if (!item) //# If not selected Item, take the first one
        item = GetLabelItem(LabelCount()-1);

    auto difficult = stat == Qt::Checked?true:false;
    Shape* shape = dynamic_cast<Shape*>(item);
    if (shape != nullptr){
        if (difficult != shape->difficult){
            shape->difficult = difficult;
            emit sigDifficultChanged(stat);
        }else{ // # User probably changed item visibility
            shape->SetVisible(item->checkState() == Qt::Checked);
        }
    }
}

void LabelsWidget::OnLabelSelectionChanged(QListWidgetItem *item){
//    qDebug("----------");
    printf("OnLabelSelectionChanged\n");
    if (item == nullptr){
        auto items = ui->m_iLabelListWidget->selectedItems();
        if (items.size())
            item = items[0];
    }

    if (item != nullptr && m_iCanvas != nullptr && m_iCanvas->editing()){
        printf("OnLabelSelectionChanged-----%s\n", item->text().toStdString().c_str());
        Shape* shape = dynamic_cast<Shape*>(item);
        if (shape != nullptr){
            m_iCanvas->selectShape(shape);
    //        # Add Chris
            SetDifficult(shape->difficult);
        }
        m_bNoSelectionSlot = true;
    }
}

void LabelsWidget::OnShapeSelected(Shape *shape)
{
    qDebug("----------");
    if (m_bNoSelectionSlot){
        m_bNoSelectionSlot = false;
    }else{
        if (shape != nullptr)
            SelectShape(shape);
        else
            ClearSelection();
    }
}

void LabelsWidget::__OnEditLable(QListWidgetItem *item)
{
    qDebug("----------");
    if(item != nullptr){
        if (m_iCanvas == nullptr || !m_iCanvas->editing())
            return;
        QStringList labels = Settings::GetInstance()->Get(SETTING_LABEL_HIST, QStringList());
        LabelDialog* pLabelDialog = new LabelDialog(labels, this);
        auto text = pLabelDialog->popUp(item->text());
        if (text != ""){
            item->setText(text);
            item->setBackground(generateColorByText(text));
            emit sigDirty();
            if (!labels.contains(text)){
                labels.append(text);
                Settings::GetInstance()->Set(SETTING_LABEL_HIST, labels);
            }
        }
        delete pLabelDialog;
    }
}

void LabelsWidget::__OnLabelChanged(QListWidgetItem *item)
{
    qDebug("----------");
    if(item != nullptr && m_iCanvas != nullptr){
        Shape* shape = dynamic_cast<Shape*>(item);
        if (shape != nullptr){
            shape->SetVisible(item->checkState() == Qt::Checked);
            emit sigDirty();
        }
    //# Callback functions:
    }
}

void LabelsWidget::__OnPopLabelListMenu(QPoint point){
    if (m_iLabelListMenu != nullptr){
        m_iLabelListMenu->exec(ui->m_iLabelListWidget->mapToGlobal(point));
    }
}
