#ifndef LABEL_DIALOG_H
#define LABEL_DIALOG_H

#include <QDialog>
#include <QStringListModel>
#include <QCompleter>
#include <QListWidgetItem>

namespace Ui {
class LabelDialog;
}

class LabelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LabelDialog(QStringList listItem, QWidget *parent = nullptr,QString text="Enter object label");
    ~LabelDialog();
    QString popUp(QString text="",  bool ismove=true);

private slots:
    void validate();
    void postProcess();
    void listItemClick(QListWidgetItem *item);
    void listItemDoubleClick(QListWidgetItem *item);

private:


private:
    Ui::LabelDialog *ui;
    QStringListModel model;
    QCompleter completer;
};

#endif // LABEL_DIALOG_H
