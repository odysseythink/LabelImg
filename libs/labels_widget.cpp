#include "labels_widget.h"
#include "ui_labels_widget.h"
#include <QDebug>
#include "common.h"

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
    m_ItemsToShapesMap.clear();
    m_ShapesToItemsMap.clear();
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
    auto item = m_ShapesToItemsMap[shape];
    if (item != nullptr){
        ui->m_iLabelListWidget->takeItem(ui->m_iLabelListWidget->row(item));
        delete m_ItemsToShapesMap.take(item);
    }
    delete m_ShapesToItemsMap.take(shape);
}

QListWidgetItem *LabelsWidget::AddLabel(Shape* shape)
{
    if (shape == nullptr){
        qCritical("invalid arg");
        return nullptr;
    }
    auto item = new QListWidgetItem(shape->label);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Checked);
    item->setBackground(generateColorByText(shape->label));
    ui->m_iLabelListWidget->addItem(item);
    m_ItemsToShapesMap[item] = shape;
    m_ShapesToItemsMap[shape] = item;
    return item;
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

QListWidgetItem *LabelsWidget::CurrentItem()
{
    auto items = ui->m_iLabelListWidget->selectedItems();
    if (items.size())
        return items[0];
    return nullptr;
}

void LabelsWidget::ToggleShapes(bool value)
{
    for (auto item : m_ItemsToShapesMap.keys())
        item->setCheckState(value?Qt::Checked : Qt::Unchecked);
}

void LabelsWidget::SelectShape(Shape *shape)
{
    if (shape != nullptr)
        m_ShapesToItemsMap[shape]->setSelected(true);
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

    auto item = CurrentItem();
    if (!item) //# If not selected Item, take the first one
        item = GetLabelItem(LabelCount()-1);

    auto difficult = stat == Qt::Checked?true:false;

    if (m_ItemsToShapesMap.contains(item) && m_ItemsToShapesMap[item] != nullptr){
        auto shape = m_ItemsToShapesMap[item];
        if (difficult != shape->difficult){
            shape->difficult = difficult;
            emit sigDifficultChanged(stat);
        }else{ // # User probably changed item visibility
            shape->SetVisible(item->checkState() == Qt::Checked);
        }
    }

}

void LabelsWidget::OnLabelSelectionChanged(QListWidgetItem *item){
    if (item == nullptr){
        auto items = ui->m_iLabelListWidget->selectedItems();
        if (items.size())
            item = items[0];
    }

    if (item != nullptr && m_iCanvas != nullptr && m_iCanvas->editing()){
        m_iCanvas->selectShape(m_ItemsToShapesMap[item]);
        auto shape = m_ItemsToShapesMap[item];
//        # Add Chris
        SetDifficult(shape->difficult);
        m_bNoSelectionSlot = true;
    }
}

void LabelsWidget::OnShapeSelected(Shape *shape)
{
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
    if(item != nullptr){
        emit sigEditLable(item);
    }
}

void LabelsWidget::__OnLabelChanged(QListWidgetItem *item)
{
    if(item != nullptr && m_iCanvas != nullptr){
        auto shape = m_ItemsToShapesMap[item];
        auto label = item->text();
        if (label != shape->label){
            shape->label = item->text();
            shape->line_color = generateColorByText(shape->label);
            emit sigLabelChanged(item);
        }else{ // # User probably changed item visibility
            shape->SetVisible(item->checkState() == Qt::Checked);
        }
    //# Callback functions:
    }
}

void LabelsWidget::__OnPopLabelListMenu(QPoint point){
    if (m_iLabelListMenu != nullptr){
        m_iLabelListMenu->exec(ui->m_iLabelListWidget->mapToGlobal(point));
    }
}
