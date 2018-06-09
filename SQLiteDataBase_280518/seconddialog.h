#ifndef SECONDDIALOG_H
#define SECONDDIALOG_H
#include <QDialog>
#include <QFileDialog>
#include <QMenuBar>
#include "widget.h"
#include "plotdialog.h"
namespace Ui {
    class SecondDialog;
}

class SecondDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SecondDialog(QWidget *parent = 0);
    ~SecondDialog();

private slots:
    void on_OkButton_clicked();
    void on_UpdateButton_clicked();
    void on_pushButtonDelete_clicked();
    void on_pushButtonLoadTable_clicked();
    void on_tableView_activated(const QModelIndex &index);
    void on_pushButtonPlot_clicked();
    void on_pushButtonDeleteAll_clicked();
    void import_from_file();
    void export_to_file();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_tableView_clicked(const QModelIndex &index);

signals:
    void loadDB();

private:
    int        waterID;
    QString    strWaterID;

    double     waterCount;
    QString    strWaterCount;

    QDate      TimeOfRegister;
    QString    strTime;
    QString    strTimeOfRegister;

    QString    query;

    //menu
    QMenuBar   *menuBar;
    QMenu      *mainMenu;
    QAction    *ImportFromFile;//пункт меню импорта данных водопотребления из txt файла в таблицу приложения
    QAction    *ExportToFile;//пункт меню экспорта таблицы данных водопотребления в файл

    QString     dateseparator;//example 28-12-2017 or 28/12/2017 or 28.12.2017

    Ui::SecondDialog    *ui;
};

#endif // SECONDDIALOG_H
