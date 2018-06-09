#include "plottrends.h"
#include "ui_plottrends.h"
PlotTrends::PlotTrends(
        QString graphName,
        QVector<double> X,
        QVector<double> Y,
        QVector<double> tX,
        QVector<double> tY,
        QDialog *parent
        ) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    InitDataX=X;
    InitDataY=Y;
    TrendX=tX;
    TrendY=tY;

    ui->setupUi(this);
    this->setWindowTitle(graphName);
    PlotTrends::MakePlot();
}
PlotTrends::~PlotTrends()
{
    delete ui;
}

void PlotTrends::MakePlot()
{

    qDebug()<<"Plot Trends----------------------------";

    for (int i=0; i<InitDataX.size(); ++i)
    {
      qDebug()<<"InitDataY["<<i<<"]="<<InitDataY[i];
    }

    //граф исходных данных
    ui->customplot->addGraph();
        ui->customplot->graph(0)->setData(InitDataX, InitDataY);
    ui->customplot->addGraph();
        ui->customplot->graph(1)->setData(TrendX, TrendY);

        ui->customplot->xAxis->setLabel("x");
        ui->customplot->yAxis->setLabel("y");

        ui->customplot->xAxis->setRange(0, InitDataX.size());

        double maximum = Max(InitDataY);
        double minimum = Min(InitDataY);
        qDebug()<<"MAX::"<<maximum<<" MIN::"<<minimum;
        ui->customplot->yAxis->setRange(minimum, maximum);
        ui->customplot->replot();
}

double PlotTrends::Max(QVector<double> A)
{
    double max=-1000000;
    for (int i=0; i<A.size();i++){
        if (A[i]>max){max=A[i];}
    }
    return max;
}

double PlotTrends::Min(QVector<double> A)
{
    double min=1000000;
    for (int i=0; i<A.size();i++){
        if (A[i]<min){min=A[i];}
    }
    return min;
}
