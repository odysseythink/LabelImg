#ifndef LABELS_WIDGET_H
#define LABELS_WIDGET_H

#include <QWidget>
#include <QAction>
#include <QListWidgetItem>
#include <QMap>
#include "libs/canvas.h"

namespace Ui {
class LabelsWidget;
}

class LabelsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LabelsWidget(QWidget *parent = nullptr);
    ~LabelsWidget();
    void SetCanvas(Canvas* canvas){
        m_iCanvas = canvas;
    }
    void SetLabelListMenu(QMenu* menu){
        m_iLabelListMenu = menu;
    }
    bool UseDefaultLabel();
    QString DefaultLabel();
    bool Difficult();
    void SetDifficult(bool stat);
    void SetEditAction(QAction* a);
    void SetEditVisible(bool stat);
    void ClearLabel();
    int LabelCount();
    void SelectLabel(int idx);
    void RemLabel(Shape* shape);
    QListWidgetItem * AddLabel(Shape* shape);
    QListWidgetItem * GetLabelItem(int idx);
    void ClearSelection();
    QListWidgetItem * CurrentItem();
    bool NoShapes(){
        return m_ItemsToShapesMap.count() == 0;
    }
    void ToggleShapes(bool value);
    void SelectShape(Shape* shape);

public slots:
    void OnLabelSelectionChanged(QListWidgetItem *item = nullptr);

private slots:
    void __OnDifficultChanged(int stat);
    void __OnEditLable(QListWidgetItem *item);
    void __OnLabelChanged(QListWidgetItem *item);
    void __OnPopLabelListMenu(QPoint point);

signals:
    void sigDifficultChanged(int);
    void sigLabelSelectionChanged(QListWidgetItem *item);
    void sigEditLable(QListWidgetItem *item);
    void sigLabelChanged(QListWidgetItem *item);

private:
    Ui::LabelsWidget *ui;
    Canvas* m_iCanvas;
    QMenu* m_iLabelListMenu;
    QMap<QListWidgetItem*, Shape*> m_ItemsToShapesMap;
    QMap<Shape*, QListWidgetItem*> m_ShapesToItemsMap;
};

#endif // LABELS_WIDGET_H
