#include "seconddialog.h"
#include "ui_seconddialog.h"

SecondDialog::SecondDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SecondDialog)
{
    qDebug()<<"Seconddialog is run";
    ui->setupUi(this);
    this->setWindowTitle(tr("Form Add"));
    ui->spinBoxID->setRange(0,1000);//граница количества записей в бд
    ui->spinBoxWater->setRange(0,1000000);//граница количества потребленной воды
    ui->comboBoxWaterCount->hide();//скроем элементы которые нам не нужны пока
    //создадим меню
    menuBar           = new QMenuBar();
    mainMenu          = new QMenu(tr("File"),this);
    ImportFromFile    = new QAction(tr("Import DB from file"),this);
    ExportToFile      = new QAction(tr("Export DB  to file"),this);
    dateseparator     = ".";

    mainMenu    ->addAction(ImportFromFile);
    mainMenu    ->addAction(ExportToFile);
    menuBar     ->addMenu(mainMenu);
    menuBar     ->setStyleSheet("background:#f2f2f2;");
    ImportFromFile->setEnabled(true);
    ExportToFile->setEnabled(true);
    ui->verticalLayout_2->addWidget(menuBar);
    //set-----------------------------------------------------
    QString style         =
            "background-color: #f2f2f2;\
            text-align:center;\
            text-decoration:none;\
            border-width: 1px;\
            border-color: #5a5a5a;\
            border-style:solid;\
            padding: 2px;\
            font-weight:bold;\
            color: #5a5a5a;\
            ";
     mainMenu->setStyleSheet(style);
    //---------------------------------------------------------  
    //проверка подключена ли база данных
    Widget conn;

    if(!conn.connectionOpen()){
        ui->StatusConnectionLabel->setText(tr("Status: db Disconnected"));
    }
    else{
        ui->StatusConnectionLabel->setText(tr("Status: db Connected"));
    }

    //Обновление данных в БД
    //on_pushButtonLoadTable_clicked();
    //---------------------------------------------------------
    connect(this, SIGNAL(loadDB()),this, SLOT(on_pushButtonLoadTable_clicked()));
    connect(ImportFromFile,SIGNAL(triggered(bool)),this,SLOT(import_from_file()));
    connect(ExportToFile,SIGNAL(triggered(bool)),this,SLOT(export_to_file()));
    emit loadDB();//обновить БД
}
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
SecondDialog::~SecondDialog()
{
    delete ui;
}
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void SecondDialog::on_OkButton_clicked()
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
    strTimeOfRegister=strTimeOfRegister.append(dateseparator);
    strTimeOfRegister=strTimeOfRegister.append(QString::number(month));
    strTimeOfRegister=strTimeOfRegister.append(dateseparator);
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
           emit loadDB();//обновить БД
           connect.connectionClose();
       }
       else {
           QMessageBox::critical(this,tr("ERROR"), qry.lastError().text());
       }
       //проверка на корректность завершена-------------------------------------

}
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void SecondDialog::on_UpdateButton_clicked()
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
    strTimeOfRegister=strTimeOfRegister.append(dateseparator);
    strTimeOfRegister=strTimeOfRegister.append(QString::number(month));
    strTimeOfRegister=strTimeOfRegister.append(dateseparator);
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
            emit loadDB();//обновить БД
           connect.connectionClose();
       }
       else {
           QMessageBox::critical(this,tr("ERROR"), qry.lastError().text());
       }
       //проверка на корректность завершена-------------------------------------
}

void SecondDialog::on_pushButtonDelete_clicked()
{
    //delete action -----------------------------------------------------
    Widget connect;
    waterID=ui->spinBoxID->value();
     strWaterID=QString::number(waterID);
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
       query=query.append("delete from WaterData where id='");
       query=query.append(strWaterID);
       query=query.append("';");

       qDebug()<<"QUERY:"<<query;

       QSqlQuery qry;
       qry.prepare(query);
       //проверка на корректность-----------------------------------------------------
       if (qry.exec()){
           QMessageBox::information(this,tr("INFO"), tr("Deleted!"));
            emit loadDB();//обновить БД
           connect.connectionClose();
       }
       else {
           QMessageBox::critical(this,tr("ERROR"), qry.lastError().text());
       }
       //проверка на корректность завершена-------------------------------------

}

void SecondDialog::on_pushButtonLoadTable_clicked()
{
    Widget connect;
    //Класс QSqlQueryModel предоставляет модель данных только для чтения для наборов результатов SQL.
     QSqlQueryModel *modal = new QSqlQueryModel();
     QSqlQueryModel *modalForComboBox = new QSqlQueryModel();

    connect.connectionOpen();
    //Класс QSqlQuery предоставляет средства для выполнения и обработки операторов SQL.
    QSqlQuery *qry = new QSqlQuery(connect.mydb);
    qry->prepare("select * from WaterData");
    if (qry->exec()){
        //транспортируем данные полученные после запроса в объект qsqlQueryModel
        modal->setQuery(*qry);
        ui->tableView->setModel(modal);
        qDebug() << (modal->rowCount());//вывести количество записей в таблице
    }


    //пример того как можно устанавливать модель для комбобокса
    qry->prepare("select waterCount from WaterData");
    if (qry->exec()){
        //транспортируем данные полученные после запроса в объект qsqlQueryModel
        modalForComboBox->setQuery(*qry);
        ui->comboBoxWaterCount->setModel(modalForComboBox);
        qDebug() << (modalForComboBox->rowCount());//вывести количество записей в таблице

        QString str;

        while(qry->next()){
             str = qry->value(0).toString();
            qDebug() <<str;
        }
    }
    //--------------------------------------------------------------


    //закрываем соединение с бд
  connect.connectionClose();


}

void SecondDialog::on_tableView_activated(const QModelIndex &index)
{
    qDebug()<<"=============================";
    //занесли выделенные данные в список
    int row = index.row();
    QStringList dataList;

    for(int i=0;i<ui->tableView->model()->columnCount();i++){
        QModelIndex rowData = index.sibling(row, i);
        dataList << ui->tableView->model()->data(rowData).toString();
        qDebug()<<"selectedData="<<dataList.at(i);
    }

    Widget connect;
    if(!connect.connectionOpen()){
        qDebug()<<"Faled to open data base";
        return;
    }
    connect.connectionOpen();
    QSqlQuery qry;

    qry.prepare("select * from WaterData where id='"+dataList.at(0)+"' and waterCount='"+dataList.at(1)+"' and TimeOfRegister='"+dataList.at(2)+"';");
    //Соединение с БД не будет закрыто пока не выполнится sql запрос
    if(qry.exec()){
        int count=0;
        while(qry.next()){
           count++;
        }
        if(count==1){
            QString  idWater=dataList.at(0);
            QString  countWater=dataList.at(1);
            QString  dataWater=dataList.at(2);
            qDebug()<<dataWater;
            QDate Date;

            //qDebug()<<"Date::"<<Date.fromString(dataWater,"d-M-yyyy");
            //qDebug()<<"Date2::"<<Date.fromString("05/08/2017","dd/MM/yyyy").toString();

            ui->spinBoxID->setValue(idWater.toInt());
            ui->spinBoxWater->setValue(countWater.toInt());
            QString DatePerformance="d";
            DatePerformance=DatePerformance.append(this->dateseparator);
            DatePerformance=DatePerformance.append("M");
            DatePerformance=DatePerformance.append(this->dateseparator);
            DatePerformance=DatePerformance.append("yyyy");
            ui->dateTimeEdit->setDate(Date.fromString(dataWater,DatePerformance));
        }
        connect.connectionClose();
    }
    else{
        QMessageBox::critical(this,tr("ERROR:"),qry.lastError().text());
        connect.connectionClose();
    }

}
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


void SecondDialog::on_pushButtonPlot_clicked()
{
    PlotDialog *plotD = new PlotDialog(this);
    plotD->setModal(true);
    plotD->show();
}

void SecondDialog::on_pushButtonDeleteAll_clicked()
{
    //delete action -----------------------------------------------------
    Widget connect;
   //Вывести сообщение если База Данных не открывается
   if (!connect.connectionOpen()){
       qDebug()<<tr("Failed to open the database");
       return;
   }

          //-----------------------------------------------------------------
          connect.connectionOpen();

          //Подготавливает запрос SQL-запроса для выполнения. Возвращает true, если запрос подготовлен успешно; иначе возвращает false.
          qDebug()<<"QUERY:"<<query;

          QSqlQuery qry;
          qry.prepare("delete from WaterData;");
          //проверка на корректность-----------------------------------------------------
          if (qry.exec()){
              QMessageBox::information(this,tr("INFO"), tr("Willl be deleted all!"));
              emit loadDB();//обновить БД
              connect.connectionClose();
          }
          else {
              QMessageBox::critical(this,tr("ERROR"), qry.lastError().text());
          }
          //проверка на корректность завершена-------------------------------------

}

void SecondDialog::import_from_file()
{

    qDebug()<<"ImportFromFile  clicked----------------------------------------";

    //QString filename = "db.txt";
     QString filename = QFileDialog::getOpenFileName(this,tr("Open Data File"), "", tr("text files(*.txt)"));
    QString line="";

    int row=1;
    if (filename!=""){
        QFile file(filename);
        on_pushButtonDeleteAll_clicked();//очищаем таблицу

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
     {
        QTextStream textfromfile(&file);

        // проходим по каждой строке пока файл не закончится чтобы посчитать количество записей
        while(!(textfromfile.atEnd())){
            line = textfromfile.readLine();
            //выделяем данные из строки------------------------------------------


            strWaterID=QString::number(row);
            //qDebug() <<"strWaterID="<<strWaterID;

            strWaterCount = line;
            QString str="water=";
            int pos = strWaterCount.indexOf(str,0);
            strWaterCount.remove(0,pos+str.length());
            str=";";
            pos = strWaterCount.indexOf(str,0);
            strWaterCount.remove(pos,strWaterCount.length());
            //qDebug() <<"strWaterCount="<<strWaterCount;

            strTimeOfRegister = line;
             str="date=";
             pos = strTimeOfRegister.indexOf(str,0);
            strTimeOfRegister.remove(0,pos+str.length());
            str=";";
            pos = strTimeOfRegister.indexOf(str,0);
            strTimeOfRegister.remove(pos,strTimeOfRegister.length());
            //qDebug() <<"strTimeOfRegister="<<strTimeOfRegister;

            //insert action -----------------------------------------------------
            Widget connect;
            //Вывести сообщение если База Данных не открывается
            if (!connect.connectionOpen()){
               qDebug()<<tr("Failed to open the database");
               return;
            }  //-----------------------------------------------------------------
               connect.connectionOpen();

               query="";
               query=query.append("insert into WaterData values('");
               query=query.append(strWaterID);
               query=query.append("','");
               query=query.append(strWaterCount);
               query=query.append("','");
               query=query.append(strTimeOfRegister);
               query=query.append("');");

               qDebug()<<"QUERY:"<<query;

               QSqlQuery qry;
               qry.prepare(query);
               //проверка на корректность-----------------------------------------------------
               if (qry.exec()){
                    emit loadDB();//обновить БД
                    row++;
                    connect.connectionClose();
               }
               else {
                   QMessageBox::critical(this,tr("ERROR"), qry.lastError().text());
               }
               //проверка на корректность завершена-------------------------------------
        }
        file.close(); // Закрываем файл
        qDebug() << "row="<<row;

     }
    //если файл не открывается
    else{
        QMessageBox::critical(this,tr("ERROR"), file.errorString());
    }
    }




}

void SecondDialog::export_to_file()
{
    qDebug()<<"ExportToFile  clicked----------------------------------------";
    /*getSaveFileName(QWidget *parent = Q_NULLPTR, const QString &caption = QString(), const QString &dir = QString(), const QString &filter = QString(), QString *selectedFilter = Q_NULLPTR, Options options = Options())*/
    QString filename = QFileDialog::getSaveFileName(this,tr("Save Data File"), "", tr("text files(*.txt);; All Files (*.*)"));

    if (filename != "") {
            QFile file(filename);
            if (!file.open(QIODevice::WriteOnly)) {
                QMessageBox::critical(this,tr("ERROR"), file.errorString());
                // error message
            } else {
                QTextStream stream(&file);



                //insert action -----------------------------------------------------
                Widget connect;
                //Вывести сообщение если База Данных не открывается
                if (!connect.connectionOpen()){
                   qDebug()<<tr("Failed to open the database");
                   return;
                }  //-----------------------------------------------------------------
                   connect.connectionOpen();

                   query="";
                   query=query.append("select * from  WaterData;");

                   qDebug()<<"QUERY:"<<query;

                   QSqlQuery qry;
                   qry.prepare(query);
                   //проверка на корректность-----------------------------------------------------
                   if (qry.exec()){


                       while (qry.next()) {
                           QString id = qry.value(0).toString();
                           QString water = qry.value(1).toString();
                           QString date = qry.value(2).toString();
                           /* water=177810;date=01.09.2015*/
                           stream << "water="<<water<<";date="<<date<<"; \n";

                           qDebug()<<"water:"<<water;
                       }

                        connect.connectionClose();
                   }
                   else {
                       QMessageBox::critical(this,tr("ERROR"), qry.lastError().text());
                   }
                   //проверка на корректность завершена-------------------------------------
                stream.flush();
                file.close();
                connect.connectionClose();
            }
     }

}

void SecondDialog::on_tableView_doubleClicked(const QModelIndex &index)
{

}

void SecondDialog::on_tableView_clicked(const QModelIndex &index)
{

}
