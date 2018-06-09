#ifndef EDITDIALOG_H
#define EDITDIALOG_H
#include <QWidget>
#include "widget.h"
namespace Ui {
    class Form;
}

class EditDialog : public QWidget
{
    Q_OBJECT
public:
    explicit EditDialog(QWidget *parent = 0);
    ~EditDialog();

private slots:
    void on_OkButton_clicked();
    void on_UpdateButton_clicked();
    void DeleteButton_clicked();

private:
    int                 waterID;
    QString             strWaterID;

    int                 waterCount;
    QString             strWaterCount;

    QDate               TimeOfRegister;
    QString             strTime;
    QString             strTimeOfRegister;

    QString             query;

    Ui::Form            *ui;
};
#endif // EDITDIALOG_H
