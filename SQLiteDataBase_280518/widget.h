#ifndef WIDGET_H
#define WIDGET_H
#include <QtSql>
#include <QtDebug>
#include <QFileInfo>
#include <QWidget>
#include "seconddialog.h"
#include "editdialog.h"
#include <QMessageBox>
#include <QPixmap>//для отображения картинки

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
//создаем реализацию прямо в файле *.h чтобы потом
//просто подключить h к другим файлам cpp
public:
    QSqlDatabase mydb;//Класс QSqlDatabase обрабатывает соединение с базой данных.
    void connectionClose(){
        mydb.close();
        mydb.removeDatabase(QSqlDatabase::defaultConnection);
    }
    bool connectionOpen(){
        QString DBpath = "DataBase.db";
        mydb= QSqlDatabase::addDatabase("QSQLITE");
        mydb.setDatabaseName(DBpath);

        if(!mydb.open()){
            qDebug() << "DataBase Disconnected";
            return false;
        }
        else{
            qDebug() << "db Connected";
            return true;
        }
    }
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_ButtonLogin_clicked();

private:
    Ui::Widget *ui;
    int w ;//image width
    int h;//image height
    QString username;
    QString password;
};

#endif // WIDGET_H
