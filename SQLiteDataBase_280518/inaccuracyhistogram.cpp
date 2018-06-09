#include "inaccuracyhistogram.h"
#include "ui_inaccuracyhistogram.h"

InaccuracyHistogram::InaccuracyHistogram(
        QString graphName,
        QVector<double> X,
        QVector<double> Y,
        int TypeOfGraph,
        QDialog *parent
        ) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    InaccuracyX=X;
    InaccuracyY=Y;
    TypeGraph = TypeOfGraph;
    countOfZones=20;
    ui->setupUi(this);
    this->setWindowTitle(graphName);
    InaccuracyHistogram::MakePlot();
}

InaccuracyHistogram::~InaccuracyHistogram()
{
    delete ui;
}
void InaccuracyHistogram::MakePlot()
{
    // generate some data:
    qDebug()<<"InaccuracyHistogram----------------------------";

    for (int i=0; i<InaccuracyX.size(); ++i)
    {
      qDebug()<<"InaccuracyY["<<i<<"]="<<InaccuracyY[i];
    }

    // create graph and assign data to it:
    ui->customplot->addGraph();
    if (TypeGraph==0){
        ui->customplot->graph(0)->setData(InaccuracyX, InaccuracyY);
        // give the axes some labels:
        ui->customplot->xAxis->setLabel("x");
        ui->customplot->yAxis->setLabel("y");
        // set axes ranges, so we see all data:
        ui->customplot->xAxis->setRange(0, InaccuracyX.size());
        double maximum = Max(InaccuracyY);
        double minimum = Min(InaccuracyY);
        qDebug()<<"MAX::"<<maximum<<" MIN::"<<minimum;
        ui->customplot->yAxis->setRange(minimum, maximum);
        ui->customplot->replot();
    }
    if (TypeGraph==1){
        double maximum = Max(InaccuracyY);
        double minimum = Min(InaccuracyY);      qDebug()<<"MAX::"<<maximum<<" MIN::"<<minimum;
        d=maximum-minimum;                      qDebug()<<"d::"<<d<<" countOfZones::"<<countOfZones;
        step = d/countOfZones;                  qDebug()<<"step::"<<step;

        HistogramX.resize(countOfZones+1);
        HistogramY.resize(countOfZones+1);
        //----------------------------------------------------------------
        for (int i=0; i<HistogramX.size();i++){
            int count=0;
            double low=minimum+step*i;
            double hight = minimum+step*(i+1);
            HistogramX[i]=low;                  qDebug()<<"HistogramX["<<i<<"]="<<HistogramX[i];

            //------------------------------------------------------------
            for(int j=0; j<InaccuracyX.size();j++){
                //qDebug()<<"InaccuracyY["<<j<<"]="<<InaccuracyY[j];
                if ((InaccuracyY[j]<=hight)&&(InaccuracyY[j]>=low)){
                    count++;
                }
            }
            //------------------------------------------------------------
            HistogramY[i]=count;                qDebug()<<"HistogramY["<<i<<"]="<<HistogramY[i];
        }
        //----------------------------------------------------------------
        ui->customplot->addGraph();
        ui->customplot->graph(0)->setData(HistogramX, HistogramY);

        // give the axes some labels:
        ui->customplot->xAxis->setLabel("x");
        ui->customplot->yAxis->setLabel("y");

        //устанавливаем стиль ступенчатой функции
        ui->customplot->graph(0)->setLineStyle(QCPGraph::lsStepCenter);
        ui->customplot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::red, Qt::white, 7));

        // set axes ranges, so we see all data:
        maximum = Max(InaccuracyY);
        minimum = Min(InaccuracyY);
        ui->customplot->xAxis->setRange(minimum, maximum);

        maximum = Max(HistogramY);
        minimum = Min(HistogramY);
        ui->customplot->yAxis->setRange(minimum, maximum);

        //Рисуем график
        ui->customplot->replot();
    }
}

double InaccuracyHistogram::Max(QVector<double> A)
{
    double max=-1000000;
    for (int i=0; i<A.size();i++){
        if (A[i]>max){max=A[i];}
    }
    return max;
}

double InaccuracyHistogram::Min(QVector<double> A)
{
    double min=1000000;
    for (int i=0; i<A.size();i++){
        if (A[i]<min){min=A[i];}
    }
    return min;
}


