#include "form_add_edit.h"
#include "ui_form_add_edit.h"

Form_Add_Edit::Form_Add_Edit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_Add_Edit)
{

    ui->setupUi(this);
    this->setWindowTitle(tr("Form Add"));
    ui->spinBoxID->setRange(0,2000);//граница количества записей в бд
    ui->spinBoxWater->setRange(0,1000);//граница количества потребленной воды
    //---------------------------------------------------------
    //проверка подключена ли база данных
    Widget connect;

    if(!connect.connectionOpen()){
        ui->StatusConnectionLabel->setText(tr("Status: db Disconnected"));
    }
    else{
        ui->StatusConnectionLabel->setText(tr("Status: db Connected"));
    }
    //---------------------------------------------------------
}
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
Form_Add_Edit::~Form_Add_Edit()
{
    delete ui;
}
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Form_Add_Edit::on_OkButton_clicked()
{

    //save action -----------------------------------------------------
    Widget connect;
    waterID=ui->spinBoxID->value();
    waterCount=ui->spinBoxWater->value();

    TimeOfRegister=ui->dateTimeEdit->date();

    strTime = ui->dateTimeEdit->time().toString();
    strWaterID=QString::number(waterID);
    strWaterCount = QString::number(waterCount);

    int day = TimeOfRegister.day();
    int month = TimeOfRegister.month();
    int year = TimeOfRegister.year();

    strTimeOfRegister = "";
    strTimeOfRegister =strTimeOfRegister.append(QString::number(day));
    strTimeOfRegister=strTimeOfRegister.append("-");
    strTimeOfRegister=strTimeOfRegister.append(QString::number(month));
    strTimeOfRegister=strTimeOfRegister.append("-");
    strTimeOfRegister=strTimeOfRegister.append(QString::number(year));

    //qDebug()<<"time:"<<strTime<<" date:"<<strTimeOfRegister;




   //Вывести сообщение если База Данных не открывается-----------------
   if (!connect.connectionOpen()){
       qDebug()<<tr("Failed to open the database");
       return;
   }

       //-----------------------------------------------------------------
       connect.connectionOpen();

       //Подготавливает запрос SQL-запроса для выполнения. Возвращает true, если запрос подготовлен успешно; иначе возвращает false.
       //Для SQLite строка запроса может содержать только одну инструкцию за раз. Если задано более одного оператора, функция возвращает false.
       query="";
       query=query.append("insert into WaterData values('");
       query=query.append(strWaterID);
       query=query.append("','");
       query=query.append(strWaterCount);
       query=query.append("','");
       query=query.append(strTimeOfRegister);
       query=query.append("');");

       qDebug()<<"QUERY:"<<query;

       //query = "insert into WaterData (id,waterCount,TimeOfRegister) values('"+waterID+"','"+waterCount+"','"+time+"');";
       QSqlQuery qry;
       qry.prepare(query);
       //проверка на корректность-----------------------------------------------------
       if (qry.exec()){
           QMessageBox::information(this,tr("INFO"), tr("Saved!"));
           connect.connectionClose();
       }
       else {
           QMessageBox::critical(this,tr("ERROR"), qry.lastError().text());
       }
       //проверка на корректность завершена-------------------------------------

}
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Form_Add_Edit::on_UpdateButton_clicked()
{
    //update action -----------------------------------------------------
    Widget connect;
    waterID=ui->spinBoxID->value();
    waterCount=ui->spinBoxWater->value();

    TimeOfRegister=ui->dateTimeEdit->date();

    strTime = ui->dateTimeEdit->time().toString();
    strWaterID=QString::number(waterID);
    strWaterCount = QString::number(waterCount);

    int day = TimeOfRegister.day();
    int month = TimeOfRegister.month();
    int year = TimeOfRegister.year();

    strTimeOfRegister = "";
    strTimeOfRegister =strTimeOfRegister.append(QString::number(day));
    strTimeOfRegister=strTimeOfRegister.append("-");
    strTimeOfRegister=strTimeOfRegister.append(QString::number(month));
    strTimeOfRegister=strTimeOfRegister.append("-");
    strTimeOfRegister=strTimeOfRegister.append(QString::number(year));

    //qDebug()<<"time:"<<strTime<<" date:"<<strTimeOfRegister;




   //Вывести сообщение если База Данных не открывается-----------------
   if (!connect.connectionOpen()){
       qDebug()<<tr("Failed to open the database");
       return;
   }

       //-----------------------------------------------------------------
       connect.connectionOpen();

       //Подготавливает запрос SQL-запроса для выполнения. Возвращает true, если запрос подготовлен успешно; иначе возвращает false.
       //Для SQLite строка запроса может содержать только одну инструкцию за раз. Если задано более одного оператора, функция возвращает false.
       query="";
       query=query.append("update WaterData set id='");
       query=query.append(strWaterID);
       query=query.append("',waterCount='");
       query=query.append(strWaterCount);
       query=query.append("',TimeOfRegister='");
       query=query.append(strTimeOfRegister);
       query=query.append("' where id='");
       query=query.append(strWaterID);
       query=query.append("';");

       qDebug()<<"QUERY:"<<query;

       QSqlQuery qry;
       qry.prepare(query);
       //проверка на корректность-----------------------------------------------------
       if (qry.exec()){
           QMessageBox::information(this,tr("INFO"), tr("Updated!"));
           connect.connectionClose();
       }
       else {
           QMessageBox::critical(this,tr("ERROR"), qry.lastError().text());
       }
       //проверка на корректность завершена-------------------------------------
}
