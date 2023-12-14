#ifndef LABELS_WIDGET_H
#define LABELS_WIDGET_H

#include <QWidget>
#include <QAction>
#include <QListWidgetItem>

namespace Ui {
class LabelsWidget;
}

class LabelsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LabelsWidget(QWidget *parent = nullptr);
    ~LabelsWidget();
    bool UseDefaultLabel();
    QString DefaultLabel();
    bool Difficult();
    void SetDifficult(bool stat);
    void SetEditAction(QAction* a);
    void SetEditVisible(bool stat);

private slots:
    void __DifficultChanged(int stat);
    void __OnLabelSelectionChanged(QListWidgetItem *item = nullptr);
    void __OnEditLable(QListWidgetItem *item);
    void __OnLabelChanged(QListWidgetItem *item);

signals:
    void sigDifficultChanged(int);
    void sigLabelSelectionChanged(QListWidgetItem *item);
    void sigEditLable(QListWidgetItem *item);
    void sigLabelChanged(QListWidgetItem *item);

private:
    Ui::LabelsWidget *ui;
};

#endif // LABELS_WIDGET_H
