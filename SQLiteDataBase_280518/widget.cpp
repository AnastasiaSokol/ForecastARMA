#include "widget.h"
#include "ui_widget.h"


//https://www.youtube.com/watch?v=6_elY8O20I8
//-------------------------------------------------------
/**
 * @brief Widget::Widget
 * @param parent
 */
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    //initialize
    QPixmap pix("login.png");
    ui->label_pic->setFixedWidth(200);
    ui->label_pic->setFixedHeight(200);
    w = ui->label_pic->width();
    h = ui->label_pic->height();



    //Settings
    //ui->groupBoxPict->setFixedHeight(h);
    //ui->groupBoxPict->setFixedWidth(w);

    ui->label_pic->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));

    ui->lineEditPassword->setEchoMode(QLineEdit::Password);//чтобы не отображался пароль
    this->setWindowTitle(tr("Login"));
    //установим имя всем элементам интерфейса------------------
    ui->ButtonLogin->setText(tr("Login"));
    ui->labelPassword->setText(tr("Password"));
    ui->labelUserName->setText(tr("UserName"));
    //---------------------------------------------------------
    //проверка подключена ли база данных
    if(!connectionOpen()){
        ui->label->setText(tr("db Disconnected"));
    }
    else{
        ui->label->setText(tr("db Connected"));
    }

}
//-------------------------------------------------------
Widget::~Widget()
{
    delete ui;
}
//-------------------------------------------------------
void Widget::on_ButtonLogin_clicked()
{
     username = ui->lineEditUserName->text();
     password = ui->lineEditPassword->text();

    qDebug()<<"username:"<<username;
    qDebug()<<"password:"<<password;
    //---------------------------------------------
    //if (!connectionOpen()){
    //    qDebug()<<"Failed to open the database";
    //    return;
    //}
    //---------------------------------------------
    //connectionOpen(DBpath);
    QSqlQuery qry;
    //Подготавливает запрос SQL-запроса для выполнения. Возвращает true, если запрос подготовлен успешно; иначе возвращает false.
    //Для SQLite строка запроса может содержать только одну инструкцию за раз. Если задано более одного оператора, функция возвращает false.
    qry.prepare("select * from users where name='"+username+"' and password='"+password+"';");
    //проверка на корректность-----------------------------------------------------
    if (qry.exec()){
        int count=0;
        while(qry.next()){
            count++;
            qDebug() <<"Count=" <<count;
        }
        if (count==1){
            ui->label->setText(tr("UserName and Password is correct"));

            connectionClose();

            //QMessageBox::information(this, tr("Login"),tr("UserName and Password is correct"));

            //this->close();//закрыть окно для login

            this->hide();//скрыть окно для login
            SecondDialog  *secondD = new SecondDialog(this);
            secondD->setModal(true);
            secondD->show();

            //EditDialog *editform = new EditDialog();
            //editform->show();
        }
        if (count>1){
            ui->label->setText(tr("Duplicate username and password"));
            QMessageBox::information(this,tr("Login"), tr("Duplicate username and password"));
        }
        if (count==0){
            ui->label->setText(tr("UserName and Password is not correct"));
            QMessageBox::information(this,tr("Login"), tr("UserName and Password is not correct"));
        }
    }
    //проверка на корректность имени пользователя и его пароля--------------------
}
//-------------------------------------------------------

