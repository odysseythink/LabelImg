#ifndef LABELS_WIDGET_H
#define LABELS_WIDGET_H

#include <QWidget>
#include <QAction>
#include <QListWidgetItem>
#include <QMap>
#include "libs/canvas.h"
#include "libs/shape.h"

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
    Shape * CurrentShape();
    bool NoShapes();
    void ToggleShapes(bool value);
    void SelectShape(Shape* shape);

public slots:
    void OnLabelSelectionChanged(QListWidgetItem *item = nullptr);
    void OnShapeSelected(Shape* shape);

private slots:
    void __OnDifficultChanged(int stat);
    void __OnEditLable(QListWidgetItem *item);
    void __OnLabelChanged(QListWidgetItem *item);
    void __OnPopLabelListMenu(QPoint point);

private:
    QListWidgetItem * __CurrentItem();

signals:
    void sigDifficultChanged(int);
    void sigDirty();

private:
    Ui::LabelsWidget *ui;
    Canvas* m_iCanvas;
    QMenu* m_iLabelListMenu;
    bool m_bNoSelectionSlot;
};

#endif // LABELS_WIDGET_H
