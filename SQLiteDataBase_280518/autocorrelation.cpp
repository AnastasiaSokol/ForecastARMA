#include "autocorrelation.h"
#include "ui_autocorrelation.h"

autocorrelation::autocorrelation(
        QString graphName,
        QVector<double> X,
        QVector<double> Y,
        QDialog *parent
        ) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    DataX=X;
    DataY=Y;
    ui->setupUi(this);
    this->setWindowTitle(graphName);
    autocorrelation::MakePlot();
}

autocorrelation::~autocorrelation()
{
    delete ui;
}
void autocorrelation::MakePlot()
{
    // generate some data:
    qDebug()<<"InitialData----------------------------";
    //Вывод в консоль данных
    for (int i=0; i<DataX.size(); ++i)
    {
      qDebug()<<"DataY["<<i<<"]="<<DataY[i];
    }

    // Создаем граф и передаем в него данные
    ui->customplot->addGraph();
    ui->customplot->graph(0)->setData(DataX, DataY);

    // Устанавливаем метки на оси
    ui->customplot->xAxis->setLabel("lag");
    ui->customplot->yAxis->setLabel("p(lag)");

    // Устанавливаем масштаб графика
    ui->customplot->xAxis->setRange(0, DataX.size());
    double maximum = Max(DataY)+5;
    double minimum = Min(DataY)-5;
    qDebug()<<"MAX::"<<maximum<<" MIN::"<<minimum;
    ui->customplot->yAxis->setRange(minimum, maximum);

    //устанавливаем стиль ступенчатой функции
    ui->customplot->graph(0)->setLineStyle(QCPGraph::lsStepCenter);
    ui->customplot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::red, Qt::white, 7));

    //Рисуем график
    ui->customplot->replot();
}

double autocorrelation::Max(QVector<double> A)
{
    double max=-1000000;
    for (int i=0; i<A.size();i++){
        if (A[i]>max){max=A[i];}
    }
    return max;
}

double autocorrelation::Min(QVector<double> A)
{
    double min=1000000;
    for (int i=0; i<A.size();i++){
        if (A[i]<min){min=A[i];}
    }
    return min;
}
