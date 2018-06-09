#include "plotdialog.h"
#include "ui_plot.h"
#include <math.h>

PlotDialog::PlotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Form Plot"));

    /*! номер графа с пронозом */
    indexGraphOfForecast = 1;

    /*! Заголовки колонок таблицы на вкладке "Исходные данные"*/
    tableInitialDataHeader <<tr("i")<<tr("Yi") <<tr("(Yi-average)^2");

     /*! Заголовки колонок таблицы на вкладке "Автокорреляция"*/
    tableAutocorrelationHeader <<tr("lag")<<tr("Autocorrelation[i]");

    /*! масштабирование графика*/
    int minScale=-1000000;//-1000000; Min(initialDataY);
    int maxScale=1000000;//; Max(initialDataY);

    /*! инициализация постоянной составляющей модели авторегресии*/
    double constantTerm=37060.911;

    /*! Порядок авторегрессии*/
    OrderAR=1;
    countOfCoefficientsAR=OrderAR;

    /*! Коэффициенты Авторегрессии*/
    coefficientsAR.resize(countOfCoefficientsAR);
    coefficientsAR[0]=0.796;

    /*! момент с которого должен строиться прогноз*/
    momentOfForecasting=0;

    /*! Флаги того что графики еще не остроены*/
    ForecastPlot = false;
    DifferencePlot = false;
    UseTransformVar = false;

    /*! На вкладке "остатки" заполним combobox с вероятностями для xи квадрат распределения */
    QStringList AXsqr;
    AXsqr   <<"0.995"
            <<"0.990"
            <<"0.975"
            <<"0.950"
            <<"0.900"
            <<"0.750"
            <<"0.500"
            <<"0.250"
            <<"0.100"
            <<"0.050"
            <<"0.025"
            <<"0.010"
            <<"0.005";
    ui->comboBoxXsqr->addItems(AXsqr);

    /*! Сделаем невидимыми некоторые элементы интерфейса*/
    ui->pushButtonPlotHistogram->setEnabled(false);
    ui->pushButtonPlotInaccuracyHistogram->setEnabled(false);
    ui->checkBoxPrintTransformVar->setEnabled(false);
    ui->checkBoxForecastForTransformVar->setEnabled(false);
    ui->checkBoxPrintForecast->setEnabled(false);
    ui->tab_6->setEnabled(false);//tab inaccuracy
    ui->tab_5->setEnabled(false);//tab white noise
    ui->groupBoxTransformData->hide();
    ui->groupBox_Coefficients->hide();
    ui->pushButtonGenerateWhiteNoise->hide();//Скрыли неиспользуемую кнопку - потом удали кнопку и слоты с ней связанные
    ui->listView->hide();
    ui->tableInitialData->hide();
    ui->groupBoxForecast->hide();


    /*! Установим границы масштабирования графиков*/
    ui->spinBoxMaxRangeX->setRange(minScale,maxScale);
    ui->spinBoxMinRangeX->setRange(minScale,maxScale);
    ui->spinBoxMaxRangeY->setRange(minScale,maxScale);
    ui->spinBoxMinRangeY->setRange(minScale,maxScale);


    /*! Установим границы для параметров авторегрессии*/
    ui->doubleSpinBoxConstTerm->setRange(-1000000.0,1000000.0);//not coeff - constantTerm
    ui->doubleSpinBoxCoeff2->setRange(-1000000.0,1000000.0);
    ui->spinBoxFromMoment->setRange(0,(countOfData-countOfCoefficientsAR+1));

    /*! Установим значения  параметров авторегрессии*/
    ui->spinBox_OrderAR->setValue(OrderAR);
    ui->doubleSpinBoxConstTerm->setValue(constantTerm);
    ui->doubleSpinBoxCoeff2->setValue(coefficientsAR[0]);

    /*! Установим список цветов для графиков*/
    colors<<"red"<<"blue"<<"green"<<"black";
    ui->comboBoxColors->addItems(colors);

    /*! Отрисуем график исходных данных*/
    PlotDialog::makePlot();

    /*! Выведем количество исходных данных*/
    countOfData= ui->listView->model()->rowCount();

    /*! Свяжем сигнал chekBox "Разностные ряды" со слотом, который делает доступными элементы интерфейса*/
    connect(ui->checkBoxDifference, SIGNAL(clicked(bool)),this, SLOT(on_checkBoxDifference_clicked()));
    emit ui->checkBoxDifference->clicked();


}
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
PlotDialog::~PlotDialog()
{
    delete ui;
}
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void PlotDialog::makePlot()
{

    qDebug() <<"slot makePlot()-----------------------------";

    //-------------------------------------------------------
    /*! Объявление переменных
     * Класс QSqlQueryModel предоставляет модель данных только для чтения
     * для наборов результатов SQL.
    */
    QSqlQueryModel *modal = new QSqlQueryModel();
    Widget conn;

    /*! подключаемся к базе данных*/
    conn.connectionOpen();

    /*! Класс QSqlQuery предоставляет средства для выполнения
     * и обработки операторов SQL
    */
    QSqlQuery *qry = new QSqlQuery(conn.mydb);
    qry->prepare("select waterCount from WaterData");

    if (qry->exec()){
        /*! транспортируем данные полученные после запроса в объект qsqlQueryModel */
        modal->setQuery(*qry);
        ui->listView->setModel(modal);
        ui->tableInitialData->setModel(modal);
    }else{
        //QMessageBox::critical(this,tr("ERROR"), qry.lastError().text());
    }

    /*! после того как запомнили данные выведем их количество */
    countOfData=ui->listView->model()->rowCount();
    ui->lineEditCountOfData->setText(QString::number(countOfData));


    /*! изменим масштаб графика по оси х */
    ui->spinBoxMaxRangeX->setValue(countOfData);


    /*! закрываем соединение с бд */
    conn.connectionClose();
    //-----------------------------------------------------------


   QModelIndex firstIndex = ui->listView->model()->index( 0, 0 );

   /*! Передаем данные в переменную*/
   initialDataY.resize(countOfData);
   initialDataX.resize(countOfData);
    averageOfInitialData=0;
    for (int i=0; i<countOfData; ++i)
    {
      initialDataX[i] = i;

      QModelIndex CurrentIndex = firstIndex.sibling(i, 0);
      initialDataY[i] = ui->listView->model()->data(CurrentIndex).toDouble();
      averageOfInitialData=averageOfInitialData+initialDataY[i];
    }

    /*! Выведем данные в консоль*/
    for (int i = 0; i<countOfData;i++){
        qDebug()<<"initialDataY["<<i<<"]="<<initialDataY[i];
    }

    /*! Установим значение масштаба графиков*/
    ui->spinBoxMinRangeY->setValue(Min(initialDataY));
    ui->spinBoxMaxRangeY->setValue(Max(initialDataY));
    qDebug()<<"Min(initialDataY)="<<Min(initialDataY);
    qDebug()<<"Max(initialDataY)="<<Max(initialDataY);

    /*! Посчитаем среднее значение*/
    averageOfInitialData=averageOfInitialData/countOfData;
    ui->lineEditAverage->setText(QString::number(averageOfInitialData));

     /*! Выполним оценку выборочной дисперсии*/
    SelectiveVariance=0;
    for (int i = 0; i<countOfData;i++){
        SelectiveVariance=SelectiveVariance+(initialDataY[i] - averageOfInitialData)*(initialDataY[i] - averageOfInitialData);
    }
    SelectiveVariance=SelectiveVariance/(countOfData-1);
    ui->lineEditSelectiveVariance->setText(QString::number(SelectiveVariance));
    qDebug()<<"SelectiveVariance="<<SelectiveVariance;

    /*! Вычислим по выборке среднеквадратическое отклонение*/
    SquareDeviation=0;
    for(int i=0; i<initialDataX.size();i++){
        SquareDeviation=SquareDeviation+(initialDataY[i]-averageOfInitialData)*(initialDataY[i]-averageOfInitialData);
    }
    SquareDeviation=SquareDeviation/(initialDataX.size()-1);
    SquareDeviation=sqrt(SquareDeviation);
    ui->lineEditSquareDeviation->setText(QString::number(SquareDeviation));

    /*! Выполним оценку несмещенной дисперсии*/
    UnbiasedDispersion=(countOfData*SelectiveVariance)/(countOfData-1);
    ui->lineEditUnbiasedDispersion->setText(QString::number(UnbiasedDispersion));
    qDebug()<<"UnbiasedDispersion="<<UnbiasedDispersion;

    /*! Заполним таблицу данных расчета дисперсии на вкладке "Исходные данные"*/
    FillTableInitialData(initialDataY);


    /*! Устанавливаем граф №0 */
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setPen(QPen(QColor(255, 100, 0)));
    ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
    ui->customPlot->graph(0)->setName("Initial data");


    /*! SetInteraction - установка возможности управления элементами графика*/
    //QCP :: iRangeZoom
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                      QCP::iSelectLegend | QCP::iSelectPlottables |QCP::iSelectItems);



    ui->customPlot->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    ui->customPlot->legend->setFont(legendFont);
    ui->customPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);


    ui->customPlot->graph(0)->setData(initialDataX, initialDataY);
    /*! Устанавливаем метки на оси */
    ui->customPlot->xAxis->setLabel("x");
    ui->customPlot->yAxis->setLabel("y");

    // set axes ranges, so we see all data:
    ui->customPlot->replot();


}



void PlotDialog::on_spinBoxMinRangeX_valueChanged(int arg1)
{
    int xMax=ui->spinBoxMaxRangeX->value();
    ui->customPlot->xAxis->setRange(arg1, xMax);
    ui->customPlot->replot();
}

void PlotDialog::on_spinBoxMaxRangeX_valueChanged(int arg1)
{
    int xMin=ui->spinBoxMinRangeX->value();
    ui->customPlot->xAxis->setRange(xMin,arg1);
    ui->customPlot->replot();
}

void PlotDialog::on_spinBoxMinRangeY_valueChanged(int arg1)
{
    int yMax=ui->spinBoxMaxRangeY->value();
    ui->customPlot->yAxis->setRange(arg1, yMax);
    ui->customPlot->replot();
}

void PlotDialog::on_spinBoxMaxRangeY_valueChanged(int arg1)
{
    int yMin=ui->spinBoxMinRangeY->value();
    ui->customPlot->yAxis->setRange(yMin,arg1);
    ui->customPlot->replot();
}
//Нажали на кнопку построить прогноз
void PlotDialog::on_pushButtonGet_clicked()
{
    //массив по которому строится прогноз
    QVector<double> DataX;
    QVector<double> DataY;
    if (this->UseTransformVar){
        int size = this->TransformX.size();
        qDebug()<<"TransformX.size()="<<TransformX.size()<<";countOfData"<<countOfData;
        DataX.resize(size);
        DataY.resize(size);
        DataX=TransformX;
        DataY=TransformY;
    }
    else{
        int size = initialDataX.size();
        qDebug()<<"initialDataX.size()="<<initialDataX.size()<<";countOfData"<<countOfData;
        DataX.resize(size);
        DataY.resize(size);
        DataX=initialDataX;
        DataY=initialDataY;
    }
    qDebug()<<"Button Get Clicked-------------------------------------------";
    constTerm           =   ui->doubleSpinBoxConstTerm->value();
    momentOfForecasting =   ui->spinBoxFromMoment->value();
    forward             =   ui->spinBoxForward->value();//на сколько дней прогноз
    countOfCoefficientsAR = coefficientsAR.size();
    ForecoastCountOfData= (DataX.size())+forward;

    qDebug()<<"constTerm="<<constTerm<<" forward="<<forward<<" ForecoastCountOfData="<<ForecoastCountOfData;
    qDebug()<<"countOfData="<<countOfData;

    // generate new data:---------------------------------------------------
    ForecastX.resize(ForecoastCountOfData);//Массив прогноза
    ForecastY.resize(ForecoastCountOfData);

    qDebug()<<"for i="<<0<<" to"<<((DataX.size())-countOfCoefficientsAR+1);
    for (int i=0; i<((DataX.size())-countOfCoefficientsAR+1); i++)
    {
      ForecastX[i] = i; //qDebug()<<"x["<<(i)<<"]::"<<ForecastX[i];
      ForecastY[i] =constTerm; //qDebug()<<"Forecast["<<(i)<<"]="<<ForecastY[i]<<"+" ;


          //прогноз по имеющимся данным
          for (int j=0; j<countOfCoefficientsAR;j++){
              double yValue = DataY[i+j];
              ForecastY[i]=ForecastY[i]+ coefficientsAR[j]*yValue;

              //qDebug()<<"(coefficientsAR["<<j<<"]::"<<coefficientsAR[j]<<")" ;
              //qDebug()<<"*(yValue::"<<yValue<<")+";

              //f0=cf0*y0+cf1*y1+cf2*y2+..
              //f1=cf0*y1+cf1*y2+cf2*y3+..
          }
          //qDebug()<<"="<<ForecastY[i] ;
          qDebug()<<"ForecastY["<<i<<"]::"<<ForecastY[i] ;

    }
    // generate new data end
    //-----------------------------------------------------------------------------
    //строим прогноз для будующего
    qDebug()<<"";
    qDebug()<<"Forecast in future---------------------------------------";
    for (int i=((DataX.size())-countOfCoefficientsAR+1);i<ForecoastCountOfData;i++){
         ForecastX[i] = i;
         ForecastY[i] =constTerm; //qDebug()<<"Forecast["<<i<<"]="<<ForecastY[i]<<"+" ;
        for (int j=1; j<countOfCoefficientsAR+1;j++){
            ForecastY[i]=ForecastY[i]+coefficientsAR[j-1]*ForecastY[i-j];
            //qDebug()<<"(coefficientsAR["<<(j-1)<<"]::"<<coefficientsAR[j-1]<<")" ;
            //qDebug()<<"*(Forecast["<<(i-j)<<"]::"<<ForecastY[i-j]<<")+";
        }
         qDebug()<<"ForecastY["<<i<<"]::"<<ForecastY[i] ;
    }
    //---------------------------------------------------------------------------
    //Если строили прогноз по преобразованным данным то перейти от преобразованных
    //данных (разностных) к рельным
    qDebug()<<"Prepare transform forecast---------------------------";
    if (this->UseTransformVar){
        //массив реальных данных
        QVector<double> RealDataX;
        QVector<double> RealDataY;

        RealDataX.resize((ForecastX.size()+lag));
        RealDataY.resize((ForecastX.size()+lag));
        //Это сработает только если разностный ряд был 1го порядка
        for(int i=0; i<lag; i++){
            RealDataX[i]=i;
            RealDataY[i]=initialDataY[i];
        }
        for (int i=lag; i<(RealDataX.size());i++){
            RealDataX[i]=i;
            RealDataY[i]=RealDataY[i-lag]+ForecastY[i-lag] ;
            qDebug()<<"RealData["<<i<<"]::"<<"(RealDataY["<<(i-lag)<<"]::"<<RealDataY[i-lag]<<")+(ForecastY["<<(i-lag)<<"]::"<<ForecastY[i-lag]<<")="<<RealDataY[i];
        }
        ForecastX=RealDataX;
        ForecastY=RealDataY;
    }
    //---------------------------------------------------------------------------
    //Если порядок МА!=0 то нужно сгенерировать белый шум
    if (this->OrderMA!=0){
        WhiteNoiseX.resize(ForecastX.size());
        WhiteNoiseY.resize(ForecastY.size());
        for(int i=0;i<ForecastY.size();i++){
            WhiteNoiseX[i]=i;
            double n=0;
            for (int k=0;k<25;k++){
                n=n+(rand()/((double)RAND_MAX));
                //n=n+( 4 * ((rand()/((double)RAND_MAX)) - 0.5));
            qDebug()<<"n"<<n;
            }
            n=n/25;
            n=n*2;
            n=n-1;
            qDebug()<<"n"<<n;
            WhiteNoiseY[i]=n;
            qDebug()<<"WhiteNoiseY["<<i<<"]"<<WhiteNoiseY[i];
        }
    }

    //---------------------------------------------------------------------------
    //Посчитаем отклонение истинных данных от прогноза
    InaccuracyX.resize(initialDataX.size());//Массив остатков
    InaccuracyY.resize(initialDataX.size());
    for(int i=0; i<initialDataX.size();i++){
        InaccuracyX[i]=i;
        InaccuracyY[i]=initialDataY[i]-ForecastY[i];
    }

    //Посчитаем сумму квадратов отклонения данных от прогноза
    double estimation=0;
    for(int i=0; i<initialDataX.size();i++){
        estimation = estimation+ (initialDataY[i]-ForecastY[i])*(initialDataY[i]-ForecastY[i]);
    }
    estimation = estimation / initialDataX.size();
    //ui->textEditEstimation->
    QString s = QString::number(estimation);
    ui->textEditEstimation->setText(s);

    //---------------------------------------------------------------------------
    //Добавим к модели авторегрессии модель скользящего среднего
    qDebug()<<"Moving Average model----------------------------------------------";
    if (this->OrderMA!=0){
        for(int i=OrderMA;i<ForecastY.size();i++){
            ForecastY[i]=ForecastY[i]+WhiteNoiseY[i];
            qDebug()<<"ForecastY["<<i<<"]=(WhiteNoiseY["<<i<<"]::"<<WhiteNoiseY[i]<<")";
            for(int j=0; j<coefficientsMA.size();j++){
                qDebug()<<"+(coefficientsMA["<<j<<"]::"<<coefficientsMA[j]<<")*";
                qDebug()<<"(WhiteNoiseY["<<(i-j-1)<<"]::"<<WhiteNoiseY[i-j-1]<<")";
                ForecastY[i]=ForecastY[i]+coefficientsMA[j]*WhiteNoiseY[i-j-1];
            }
            qDebug()<<"="<<ForecastY[i];
        }
    }

    //--------------------------------------------------------------------------
    //если граф еще не был построен
    if(!ForecastPlot){
    //проверка на наличие графика
        if (ui->customPlot->graphCount()==2){
                indexGraphOfForecast=2;
                ui->customPlot->addGraph(ui->customPlot->xAxis, ui->customPlot->yAxis);
        }
        if (ui->customPlot->graphCount()==1){
                indexGraphOfForecast=1;
                ui->customPlot->addGraph(ui->customPlot->xAxis, ui->customPlot->yAxis);
        }
    }
    //-------------------------------------------------------------------------
   //choose the color of line
   //подсказка:
   //QStringList colors;
   //colors<<"red"<<"blue"<<"green"<<"black";

   int currindex = ui->comboBoxColors->currentIndex();
   qDebug()<<"comboBoxColors->currentIndex():"<<currindex;
   QPen pen;

   switch(currindex){
    case 0:
       pen.setColor("red");
       break;
    case 1:
       pen.setColor("blue");
       break;
    case 2:
       pen.setColor("green");
        break;
    default:
       pen.setColor("magenta");

       break;
   }
   ui->customPlot->graph(indexGraphOfForecast)->setPen(pen);
   //------------------------------------------------------------------------

   ui->customPlot->graph(indexGraphOfForecast)->setLineStyle(QCPGraph::lsLine);
   //ui->customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::red, Qt::white, 7));
   ui->customPlot->graph(indexGraphOfForecast)->setName("Forecast");

   ui->customPlot->legend->setVisible(true);
   QFont legendFont = font();  // start out with MainWindow's font..
   legendFont.setPointSize(9); // and make a bit smaller for legend
   ui->customPlot->legend->setFont(legendFont);
   ui->customPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
   // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
   ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

    // create graph and assign data to it:
    ui->customPlot->graph(indexGraphOfForecast)->setData(ForecastX, ForecastY);
    // give the axes some labels:
    ui->customPlot->xAxis->setLabel("x");
    ui->customPlot->yAxis->setLabel("Forecast");
    // set axes ranges, so we see all data:
    ui->customPlot->replot();

    //установим флаг того что граф построен в 1
    ForecastPlot =true;

    //Сделаем видимыми кнопки для показа графиков остатков
    ui->pushButtonPlotHistogram->setEnabled(true);
    ui->pushButtonPlotInaccuracyHistogram->setEnabled(true);
    ui->checkBoxPrintForecast->setEnabled(true);
    ui->tab_6->setEnabled(true);//tab inaccuracy
    ui->tab_5->setEnabled(true);//tab white noise

    //Изменим текст в поле textEditForecast
    //------------------------------------------------------
    if (ForecastY.size()>0){
        QString s="";
        for (int i=0;i<ForecastY.size();i++){
            s=s.append(QString::number(i));
            s=s.append("::");
            s=s.append(QString::number(ForecastY[i]));
            s=s.append("\n");
        }
        ui->textEditForecast->setText(s);
     }
    else {
        QMessageBox::information(this,tr("Varning"), tr("Count records of  Foresact is null"));

    }
}



void PlotDialog::on_checkBoxDifference_clicked()
{
    qDebug()<<"on_checkBoxDifference_clicked";
   if (ui->checkBoxDifference->isChecked()){
        ui->spinBoxLag->setEnabled(true);
        ui->ButtonPlotTransformVar->setEnabled(true);
   }
   else{
       ui->spinBoxLag->setEnabled(false);
       ui->ButtonPlotTransformVar->setEnabled(false);
   }
}

void PlotDialog::on_ButtonPlotTransformVar_clicked()
{
    qDebug()<<"Button PlotTransformVar Clicked-------------------------------------------";
    int lag = ui->spinBoxLag->value();
    int TransformCountOfData = countOfData-lag;

    // generate new data:
    TransformX.resize(TransformCountOfData);
    TransformY.resize(TransformCountOfData);
    //строим новые данные на основе имеющихся данных
    qDebug()<<"TransformCountOfData:"<<TransformCountOfData;
    qDebug()<<"countOfData:"<<countOfData;
    qDebug()<<"lag:"<<lag;

    for (int i=0; i<TransformCountOfData; i++)
    {
      TransformX[i] = i;
      double yCurrent= this->initialDataY[i];
      double yNext = this->initialDataY[i+lag];
      TransformY[i] =yNext-yCurrent;
      qDebug()<<"TransformY["<<i<<"]="<<"(initialDataY["<<i<<"]::"<<initialDataY[i]<<")+(initialDataY["<<(i+lag)<<"]::"<<initialDataY[i+lag]<<")="<<TransformY[i];
    }
    //проверка на наличие графа----------------------------------------------------------
    //если граф еще не был построен
    if (!DifferencePlot){
        qDebug()<<"Graph count:"<<ui->customPlot->graphCount();
        if (ui->customPlot->graphCount()==2){
            indexGraphOfTransform=2;
            ui->customPlot->addGraph(ui->customPlot->xAxis, ui->customPlot->yAxis);
        }
        if (ui->customPlot->graphCount()==1){
                indexGraphOfTransform=1;
                ui->customPlot->addGraph(ui->customPlot->xAxis, ui->customPlot->yAxis);
        }
    }
    qDebug()<<"Graph count:"<<ui->customPlot->graphCount();
    //-----------------------------------------------------------------------------------

    QPen Pen;
    Pen.setColor(QColor(30, 40, 255, 150));
    Pen.setWidthF(4);

    ui->customPlot->graph(indexGraphOfTransform)->setPen(Pen);
    ui->customPlot->graph(indexGraphOfTransform)->setName("Transform values");
    ui->customPlot->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    ui->customPlot->legend->setFont(legendFont);
    ui->customPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
    ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

    // create graph and assign data to it:
    ui->customPlot->graph(indexGraphOfTransform)->setData(TransformX, TransformY);
    // give the axes some labels:
    ui->customPlot->xAxis->setLabel("x");
    ui->customPlot->yAxis->setLabel("Transform");
    // set axes ranges, so we see all data:
    ui->customPlot->replot();

    //Устанавливаем флаг того, что граф уже был построен
    DifferencePlot = true;


    //Меняем доступность некоторых элементов интерфейса
    ui->checkBoxPrintTransformVar->setEnabled(true);
    ui->checkBoxForecastForTransformVar->setEnabled(true);
    //---------------------------------------------------------------------------
    //установим текст для textEditTransformVar
    QString s="";
    for (int i=0;i<this->TransformX.size();i++){
        s=s.append(QString::number(i));
        s=s.append("::");
        s=s.append(QString::number(TransformY[i]));
        s=s.append("\n");
    }
    ui->textEditTransformVar->setText(s);
}



void PlotDialog::on_spinBox_OrderAR_valueChanged(int arg1)
{
    OrderAR=arg1;
    if (arg1<=0){
        ui->doubleSpinBoxCoeff2->setEnabled(false);
    }
    else{
        ui->doubleSpinBoxCoeff2->setEnabled(true);
        ui->comboBoxNumberOfCoefficient->clear();
        qDebug()<<"spinBox_OrderAR_valueChanged:"<<arg1;
        for(int i=1; i<=arg1;i++){
            QString s = QString::number(i);
            ui->comboBoxNumberOfCoefficient->addItem(s);
        }

        //объявим массив коэффициентов авторегресии
        coefficientsAR.resize(arg1);
    }
}

void PlotDialog::on_doubleSpinBoxCoeff2_valueChanged(double arg1)
{
    qDebug()<<"doubleSpinBoxCoeff2_valueChanged:"<<arg1;
    int index = ui->comboBoxNumberOfCoefficient->currentText().toInt();
    qDebug()<<"index:"<<index;
    coefficientsAR[index-1]=arg1;
}

void PlotDialog::on_checkBoxPrintCoefficients_clicked()
{
    QString s = "";//message about coeffisients
    if (ui->checkBoxPrintCoefficients->isChecked()) {

        //SHOW
        ui->groupBox_Coefficients->show();

        int countOfCoefficients = coefficientsAR.size();
        //------------------------------------------------------
        if (countOfCoefficients>0){
            qDebug()<<"countOfCoefficients:"<<countOfCoefficients;

            for (int i=0;i<countOfCoefficients;i++){
                s=s.append("coefficientsAR[");
                s=s.append(QString::number(i));
                s=s.append("]=");
                s=s.append(QString::number(coefficientsAR[i]));
                s.append(";");
                s=s.append("\n");
            }

         }
        else{
             QMessageBox::information(this,tr("Varning"), tr("Count of coefficients AR should be more than null"));
        }

        int countOfCoefficientsMA = coefficientsMA.size();
        //------------------------------------------------------
        if (countOfCoefficientsMA>0){
            qDebug()<<"countOfCoefficientMAs:"<<countOfCoefficientsMA;

            for (int i=0;i<countOfCoefficientsMA;i++){
                s=s.append("coefficientsMA[");
                s=s.append(QString::number(i));
                s=s.append("]=");
                s=s.append(QString::number(coefficientsMA[i]));
                s.append(";");
                s=s.append("\n");
            }

         }
        else{
             QMessageBox::information(this,tr("Varning"), tr("Count of coefficients MA should be more than null"));
        }

        //Установим текст в поле text edit
        ui->textEditCoefficients->setText(s);
        //------------------------------------------------------
    }
    else{
        //HIDE
        ui->groupBox_Coefficients->hide();
        s = "";
    }
}

void PlotDialog::on_checkBoxPrint_Initial_Data_clicked()
{
    if (ui->checkBoxPrint_Initial_Data->isChecked()){
        //ui->listView->show();
        ui->tableInitialData->show();
    }
    else{
        //ui->listView->hide();
        ui->tableInitialData->hide();
    }
}

void PlotDialog::on_comboBoxNumberOfCoefficient_currentIndexChanged(int index)
{
    qDebug()<<"comboBoxNumberOfCoefficient_currentIndexChanged::"<<index;
    ui->doubleSpinBoxCoeff2->setValue(this->coefficientsAR[index]);
}

void PlotDialog::on_spinBoxFromMoment_valueChanged(int arg1)
{
    momentOfForecasting = arg1;
}





double PlotDialog::on_pushButtonGenerateWhiteNoise_clicked()
{
    double temp1;
    double temp2;
    double result;
    int p;

    p = 1;

    while( p > 0 )
    {
      temp2 = ( rand() / ( (double)RAND_MAX ) ); /*  rand() function generates an
                                                         integer between 0 and  RAND_MAX,
                                                         which is defined in stdlib.h.
                                                     */
      qDebug()<<"RAND_MAX"<<RAND_MAX;
      if ( temp2 == 0 )
      {// temp2 is >= (RAND_MAX / 2)
        p = 1;
      }// end if
      else
      {// temp2 is < (RAND_MAX / 2)
         p = -1;
      }// end else

    }// end while()

    temp1 = cos( ( 2.0 * (double)PI ) * rand() / ( (double)RAND_MAX ) );
    result = sqrt( -2.0 * log( temp2 ) ) * temp1;

   qDebug()<<"WhiteNoise:"<<result;	// return the generated random sample to the caller
   return result;
}

void PlotDialog::on_pushButtonPlotInaccuracyHistogram_clicked()
{
    QString graphName=tr("Inaccuracy");
    InaccuracyHistogram *plotH = new InaccuracyHistogram(graphName, InaccuracyX,InaccuracyY,0,this);
    plotH->setModal(true);
    plotH->show();
}
/*!
void PlotDialog::on_pushButtonPlotAutocorrelation_clicked()
{

    if (AutocorrelationX.size()!=0){
        QString graphName=tr("Autocorrelation");
        autocorrelation *plotAutocorrelation = new autocorrelation(graphName, AutocorrelationX,AutocorrelationY,this);
        plotAutocorrelation->setModal(true);
        plotAutocorrelation->show();
    }
    else{
        //Сообщение о том что функция автокорреляции не вычисленна
        //Или просто вычисление авктокорреляции
        on_pushButtonCalcAKF_clicked();//Подсчет АКФ
    }
}
*/
void PlotDialog::on_pushButtonPlotAutocorrelation_clicked()
{
 PlotACR(initialDataY, AutocorrelationY, AutocorrelationX);

}

void PlotDialog:: PlotACR(QVector<double> DataY, QVector<double> ACRY, QVector<double> ACRX)
{
    if (ACRY.size()==0){
        ACRX= CalcAKFX(DataY);
        ACRY= CalcAKFY(DataY);

    }
    QString graphName=tr("Autocorrelation");
    autocorrelation *plotAutocorrelation = new autocorrelation(graphName, ACRX,ACRY,this);
    plotAutocorrelation->setModal(true);
    plotAutocorrelation->show();
}

void PlotDialog::on_pushButtonPlotHistogram_clicked()
{
    QString graphName=tr("Inaccuracy Histogram");
    InaccuracyHistogram *plotH = new InaccuracyHistogram(graphName,InaccuracyX,InaccuracyY,1,this);
    plotH->setModal(true);
    plotH->show();
}

void PlotDialog::on_checkBoxForecastForTransformVar_clicked()
{
    if (ui->checkBoxForecastForTransformVar->isChecked()){
        UseTransformVar=true;
    }
    else{
        UseTransformVar=false;
    }
}

void PlotDialog::on_checkBoxPrintTransformVar_clicked()
{
    if(ui->checkBoxPrintTransformVar->isChecked()){
        ui->groupBoxTransformData->show();
    }
    else{
        ui->groupBoxTransformData->hide();
    }
}

void PlotDialog::on_spinBoxOrderMA_valueChanged(int arg1)
{
    OrderMA=arg1;
    if (arg1<=0){
        ui->doubleSpinBoxCoeffMA->setEnabled(false);
    }
    else{
        ui->doubleSpinBoxCoeffMA->setEnabled(true);
        ui->comboBoxCoeffMA->clear();
        qDebug()<<"spinBox_OrderAM_valueChanged:"<<arg1;
        for(int i=1; i<=arg1;i++){
            QString s = QString::number(i);
            ui->comboBoxCoeffMA->addItem(s);
        }

        //объявим массив коэффициентов скользящего среднего
        coefficientsMA.resize(arg1);
    }
}

void PlotDialog::on_comboBoxCoeffMA_currentIndexChanged(int index)
{
    qDebug()<<"on_comboBoxCoeffMA_currentIndexChanged::"<<index;
    ui->doubleSpinBoxCoeffMA->setValue(this->coefficientsMA[index]);
}

void PlotDialog::on_doubleSpinBoxCoeffMA_valueChanged(double arg1)
{
    qDebug()<<"on_doubleSpinBoxCoeffMA_valueChanged:"<<arg1;
    int index = ui->comboBoxCoeffMA->currentText().toInt();
    qDebug()<<"index:"<<index;
    coefficientsMA[index-1]=arg1;
}

void PlotDialog::on_checkBoxPrintForecast_clicked()
{
    if (ui->checkBoxPrintForecast->isChecked()){
        ui->groupBoxForecast->show();
    }
    else {
        ui->groupBoxForecast->hide();
    }
}

void PlotDialog::on_spinBoxLag_valueChanged(int arg1)
{
    lag=arg1;
}

void PlotDialog::on_pushButtonPlotWhiteNoise_clicked()
{
    QString graphName=tr("White noise");
    InaccuracyHistogram *plotH = new InaccuracyHistogram(graphName,WhiteNoiseX,WhiteNoiseY,0,this);
    plotH->setModal(true);
    plotH->show();

}

void PlotDialog::on_pushButtonHistogramWhiteNoise_clicked()
{
    QString graphName=tr("White noise histogram");
    InaccuracyHistogram *plotH = new InaccuracyHistogram(graphName, WhiteNoiseX,WhiteNoiseY,1,this);
    plotH->setModal(true);
    plotH->show();
}

void PlotDialog::on_pushButton_clicked()
{

}

//функции поиска минимума и максимума вектора
double PlotDialog::Max(QVector<double> A)
{
    double max=-1000000;
    qDebug()<<"A.size()="<<A.size();

    for (int i=0; i<A.size();i++){
        if (A[i]>max){
            max=A[i];
            qDebug()<<"A[i]="<<A[i];
            qDebug()<<"max="<<max;
        }
    }
    return max;
}
double PlotDialog::Min(QVector<double> A)
{
    double min=1000000;
    qDebug()<<"A.size()="<<A.size();

    for (int i=0; i<A.size();i++){
        if (A[i]<min){
            min=A[i];
            qDebug()<<"A[i]="<<A[i];
            qDebug()<<"min="<<min;
        }
    }
    return min;
}

double PlotDialog::Average(QVector<double> A)
{
    /*! Посчитаем среднее значение*/
    double average=0;
    for (int i=0; i<A.size(); ++i)
    {
      average=average+A[i];
    }
    average=average/A.size();
    qDebug()<<"average="<<average;
    return average;
}


void PlotDialog::FillTableInitialData(QVector<double> A)
{


}
//---------------------------------------------------------------------------------------
//ФУНКЦИИ ДЛЯ ПОДСЧЕТА АВТОКОРРЕЛЯЦИИ

/*! Сумма yi от i=1 до n-l, n - количество значений выборки */
double PlotDialog::lagSumYi(QVector<double> A, int lag)
{
    double s=0;
    for (int i=0; i< A.size()-lag;i++){
        s=s+A[i];
    }
    return s;
}

/*! Сумма yi^2 от i=1 до n-l, n - количество значений выборки */
double PlotDialog::SqrlagSumYi(QVector<double> A, int lag)
{
    double s=0;
    for (int i=0; i< A.size()-lag;i++){
        s=s+A[i]*A[i];
    }
    return s;
}

/*! Сумма y(i+lag) от i=1 до n-l, n - количество значений выборки */
double PlotDialog::lagSumYi_lag(QVector<double> A, int lag)
{
    double s=0;
    for (int i=0; i< A.size()-lag;i++){
        s=s+A[i+lag];
    }
    return s;
}

/*! Сумма квадратов y(i+lag) от i=1 до n-l, n - количество значений выборки*/
double PlotDialog::SqrlagSumYi_lag(QVector<double> A, int lag)
{
    double s=0;
    for (int i=0; i< A.size()-lag;i++){
        s=s+A[i+lag]*A[i+lag];
    }
    return s;
}

//Сумма yi*y(i+lag) от i=1 до n-l, n - количество значений выборки
double PlotDialog::lagSumYiYi_lag(QVector<double> A, int lag)
{
    double s=0;
    for (int i=0; i< A.size()-lag;i++){
        s=s+A[i+lag]*A[i];
    }
    return s;
}


double PlotDialog::VCoeffAutocorrelation(QVector<double> A, int lag)
{
    double R=0;
    double b=lagSumYiYi_lag(A,lag);//Сумма произведений
    double c=lagSumYi(A,lag);//Сумма yi
    double d=lagSumYi_lag(A,lag);//Сумма y(i+lag)
    double e=SqrlagSumYi(A,lag);//Сумма квадратов yi
    double f=SqrlagSumYi_lag(A,lag);//Сумма квадратов y(i+lag)

    R=((A.size()-lag)*b-c*d)/ (sqrt((A.size()-lag)*e-c*c)*sqrt((A.size()-lag)*f-d*d));

    return R;
}




void PlotDialog::FillTableAutocorrelation(){
    /*! Заполним таблицу данных расчета функции автокоррелации*/
    ui->tableAutocorrelation->clear();
    ui->tableAutocorrelation->setFixedWidth(600);
    ui->tableAutocorrelation->setRowCount(AutocorrelationX.size());
    ui->tableAutocorrelation->setColumnCount(tableAutocorrelationHeader.count());
    //ui->tableInitialData->setHorizontalHeaderLabels(tableAutocorrelationHeader);//установили заголовки


    for(int i =0; i<ui->tableAutocorrelation->rowCount();i++){
            //сделали активной
            ui->tableAutocorrelation->setCurrentCell(ui->tableAutocorrelation->rowCount()+i,0);

            ui->tableAutocorrelation->setItem(ui->tableAutocorrelation->currentRow()+i+1,0,new QTableWidgetItem(QString::number(AutocorrelationX[i])));
            ui->tableAutocorrelation->setItem(ui->tableAutocorrelation->currentRow()+i+1,0,new QTableWidgetItem(QString::number(AutocorrelationY[i])));

    }

}
/*!
void PlotDialog::on_pushButtonCalcAKF_clicked()
{
    int size = initialDataX.size()-1;


    AutocorrelationX.clear();
    AutocorrelationY.clear();

    AutocorrelationX.resize(size);
    AutocorrelationY.resize(size);


    for(int i=0;i<size;i++){
         AutocorrelationX[i]=i;
         AutocorrelationY[i]=VCoeffAutocorrelation(initialDataY, i);
         qDebug()<<"VCoeffAutocorrelation"<<VCoeffAutocorrelation(initialDataY, i);

    }

    //Выведем таблицу на экран
    ShowTableAutocorrelation(ui->tableAutocorrelation, AutocorrelationY, AutocorrelationX,tableAutocorrelationHeader);
}
*/
void PlotDialog::on_pushButtonCalcAKF_clicked()
{
    //Подсчет АКФ и передача результатов в два последних массива
    //CalcAKF(initialDataY,  AutocorrelationX, AutocorrelationY);
    AutocorrelationX= CalcAKFX(initialDataY);
    AutocorrelationY= CalcAKFY(initialDataY);

    for(int i=0;i<AutocorrelationX.size();i++){
         qDebug()<<"AutocorrelationY"<<i<<"]="<<AutocorrelationY[i];
    }
    qDebug()<<"end";


    //Выведем таблицу на экран
    ShowTableAutocorrelation(ui->tableAutocorrelation, AutocorrelationY, AutocorrelationX,tableAutocorrelationHeader);
}


QVector<double> PlotDialog::CalcAKFY(QVector<double> DataY)
{
    int size = DataY.size()-1;
    QVector<double> ACRY;
    //ACRX.clear();
    ACRY.clear();

    //ACRX.resize(size);
    ACRY.resize(size);


    for(int i=0;i<size;i++){
         //ACRX[i]=i;
         ACRY[i]=VCoeffAutocorrelation(DataY, i);
         qDebug()<<"ACF["<<i<<"]="<<ACRY[i];
    }
    return ACRY;

}
QVector<double> PlotDialog::CalcAKFX(QVector<double> DataY)
{
    int size = DataY.size()-1;
    QVector<double> ACRX;
    ACRX.clear();
    //ACRY.clear();

    ACRX.resize(size);
    //ACRY.resize(size);


    for(int i=0;i<size;i++){
         ACRX[i]=i;
    }
    return ACRX;
}

void PlotDialog::on_pushButtonIrvinCriterion_clicked()
{
    //заполняем таблицу данными распределения стьюдента
    qDebug()<<"ImportFromFile_Irvin  clicked----------------------------------------";
    //-------------------------------------------------------------------------------
    /*! Объявление переменных
     * Класс QSqlQueryModel предоставляет модель данных только для чтения
     * для наборов результатов SQL.
    */
    modelStiudent = new QSqlQueryModel();
    modelIrvin = new QSqlQueryModel();
    Widget conn;

    /*! подключаемся к базе данных*/
    conn.connectionOpen();

    /*! Передаем данные из базы данных о распределении стьюдента-----------------------
    */
    QSqlQuery *qry = new QSqlQuery(conn.mydb);
    qry->prepare("select * from StiudentDistribution; ");//IrvinLimit StiudentDistribution

    if (qry->exec()){
        /*! транспортируем данные полученные после запроса в объект qsqlQueryModel */
        modelStiudent->setQuery(*qry);
        ui->StiudentTableView->setModel(modelStiudent);
    }else{
        QMessageBox::critical(this,tr("ERROR"), qry->lastError().text());
    }



    /*! Передаем данные из базы данных о пределах критерия Ирвина---------------------
    */
    QSqlQuery *qry2 = new QSqlQuery(conn.mydb);
    qry2->prepare("select * from IrvinLimit;");//IrvinLimit StiudentDistribution

    if (qry2->exec()){
        modelIrvin->setQuery(*qry2);
        ui->tableViewIrvinLimit->setModel(modelIrvin);
    }else{
        QMessageBox::critical(this,tr("ERROR"), qry2->lastError().text());
    }

    /*! ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
     * Вычисляем предельный критерий Ирвина*/
    int n=0;//значение критерия
    int indexA=0;//нижняя граница
        for(int i=0;i<modelIrvin->rowCount();i++){
            n=modelIrvin->record(i).value("n").toInt();
            if(n<initialDataY.size()){
                indexA=i;
            }
        }
     int An=modelIrvin->record(indexA).value("n").toInt();
     int Bn=modelIrvin->record(indexA+1).value("n").toInt();
     double Aa=modelIrvin->record(indexA).value("a_0_05").toDouble();
     double Ba=modelIrvin->record(indexA+1).value("a_0_05").toDouble();
     IrvinLimit=((Ba-Aa)/(Bn-An))*(initialDataY.size()-An)+Aa;
     qDebug()<<"IrvinLimit="<<IrvinLimit;



    /*! :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    Посчитаем критерий Ирвина для каждого исходного значения данных*/

    IrvinCriterion.clear();
    IrvinCriterion.resize(initialDataY.size()-1);
    for (int i=0; i<IrvinCriterion.size();i++){
        IrvinCriterion[i]=abs(initialDataY[i+1]-initialDataY[i])/sqrt(SelectiveVariance);
    }

    /*! Заполним таблицу данных расчета Критерия Ирвина*/
    ui->IrvinTableWidget->clear();
    ui->IrvinTableWidget->setFixedWidth(600);
    ui->IrvinTableWidget->setRowCount(initialDataY.size());
    ui->IrvinTableWidget->setColumnCount(2);
    //ui->tableInitialData->setHorizontalHeaderLabels(tableAutocorrelationHeader);//установили заголовки


    for(int i =0; i<initialDataY.size();i++){
            //сделали активной
            ui->IrvinTableWidget->setCurrentCell(ui->IrvinTableWidget->rowCount()+i,0);

            ui->IrvinTableWidget->setItem(ui->IrvinTableWidget->currentRow()+i+1,0,new QTableWidgetItem(QString::number(initialDataY[i])));
            if(i>0){
                ui->IrvinTableWidget->setItem(ui->IrvinTableWidget->currentRow()+i+1,1,new QTableWidgetItem(QString::number(IrvinCriterion[i-1])));
                if(IrvinCriterion[i-1]>IrvinLimit){
                    ui->IrvinTableWidget->item(ui->IrvinTableWidget->currentRow()+i+1,1)->setBackground(QBrush(Qt::Dense7Pattern) );
                }
            }

    }
}


//Выделение подстроки в строке
QString PlotDialog::STRAllocation(QString str, QString leftstr, QString rightstr)
{
    QString result = str;
    QString f=leftstr;
    int pos = result.indexOf(f,0);
    result.remove(0,pos+f.length());
    f=rightstr;
    pos = result.indexOf(f,0);
    result.remove(pos,result.length());


    return result;

}

void PlotDialog::on_checkBoxAutocorrelation_clicked()
{

}

void PlotDialog::ShowTableAutocorrelation(QTableWidget *tw, QVector<double> ACRY, QVector<double> ACRX, QStringList Header)
{
    tw->clear();//ui->tableAutocorrelation
    tw->setRowCount(ACRY.size());
    tw->setColumnCount(Header.size());
    tw->setHorizontalHeaderLabels(Header);

    for(int i =0; i<tw->rowCount();i++){
        //сделали активной
        tw->setCurrentCell(tw->rowCount()+i,0);

        tw->setItem(tw->currentRow()+i+1,0,new QTableWidgetItem(QString::number(ACRX[i])));
        tw->setItem(tw->currentRow()+i+1,1,new QTableWidgetItem(QString::number(ACRY[i])));

    }
    tw->setColumnWidth(0,300);
    tw->setColumnWidth(1,300);
}


void PlotDialog::on_plotInaccuracyACR_clicked()
{
    //Подсчет АКФ
    AutocorrelationInaccuracyX= CalcAKFX(InaccuracyY);
    AutocorrelationInaccuracyY= CalcAKFY(InaccuracyY);
    //Заполняем таблицу данными автокорреляции остатков
    ShowTableAutocorrelation(ui->tableWidgetInaccuracyACR, AutocorrelationInaccuracyY, AutocorrelationInaccuracyX,tableAutocorrelationHeader);
    //Выводим график
    PlotACR(InaccuracyY, AutocorrelationInaccuracyY, AutocorrelationInaccuracyX);


}

void PlotDialog::on_ButtonCalcQcriterion_clicked()
{
    //Подсчет АКФ
    AutocorrelationInaccuracyX= CalcAKFX(InaccuracyY);
    AutocorrelationInaccuracyY= CalcAKFY(InaccuracyY);
    //Заполняем таблицу данными автокорреляции остатков
    ShowTableAutocorrelation(ui->tableWidgetInaccuracyACR, AutocorrelationInaccuracyY, AutocorrelationInaccuracyX,tableAutocorrelationHeader);


    //Вычисление совокупного критерия согласия Q
    double l=ui->doubleSpinBoxCriterionOfAgreement->value();
    qDebug()<<"l="<<l;
    double Qcriterion=0;//критерий согласия

    for (int i= 0;i<l;i++){
        Qcriterion=Qcriterion+AutocorrelationInaccuracyY[i]*AutocorrelationInaccuracyY[i];
    }

    Qcriterion=Qcriterion*initialDataY.size();
    //Qcriterion=Qcriterion*initialDataY.size();
    //Вывод суммы коэффициентов корреляции
    qDebug()<<"Qcriterion="<<Qcriterion;
    qDebug()<<"n="<<AutocorrelationInaccuracyX.size();

    QString s = QString::number(Qcriterion);
    ui->textEditCriterionOfAgreement->setText(s);

    FillXsqrModel();//Заполним таблицу данными хи квадрат распределения


}
void PlotDialog::FillXsqrModel(){
    modelXsqr = new QSqlQueryModel();
    Widget conn;

    /*! подключаемся к базе данных*/
    conn.connectionOpen();

    /*! Передаем данные из базы данных о распределении хи квадрат-----------------------
    */
    QSqlQuery *qry = new QSqlQuery(conn.mydb);
    qry->prepare("select * from XsqrDistribution; ");

    if (qry->exec()){
        /*! транспортируем данные полученные после запроса в объект qsqlQueryModel */
        modelXsqr->setQuery(*qry);
        ui->tableViewXsqrDistribution->setModel(modelXsqr);
    }else{
        QMessageBox::critical(this,tr("ERROR"), qry->lastError().text());
    }

}
void PlotDialog::on_ButtonCalcXsqr_clicked()
{
    //Если таблица хи квадрат не заполнена то заполняем ее
    if(modelXsqr->rowCount()==0){
        FillXsqrModel();
    }
    /*! ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
     * Вычисляем табличный Хи квадрат*/
    int n=0;//значение критерия
    int index=0;//индекс записи
    QSqlRecord rec;
        for(int i=0;i<modelXsqr->rowCount();i++){
            n=modelXsqr->record(i).value("d").toInt();
            //Если запись с таким числом степеней свободы совпала то
            if(n==ui->spinBoxVXsqr->value()){
                index=i;
                rec=modelXsqr->record(index);
                break;
            }
        }

        int currindex = ui->comboBoxXsqr->currentIndex();
        qDebug()<<"comboBoxXsqr->currentIndex():"<<currindex;
        qDebug()<<"index:"<<index;

        double resXsqr=0;
        switch(currindex){
         case 0:
            resXsqr=modelXsqr->record(index).value("a_995").toDouble();
            break;
         case 1:
            resXsqr=modelXsqr->record(index).value("a_990").toDouble();
            break;
         case 2:
            resXsqr=modelXsqr->record(index).value("a_975").toDouble();
             break;
        case 3:
           resXsqr=modelXsqr->record(index).value("a_950").toDouble();
            break;
        case 4:
           resXsqr=modelXsqr->record(index).value("a_900").toDouble();
            break;
        case 5:
           resXsqr=modelXsqr->record(index).value("a_750").toDouble();
            break;
        case 6:
           resXsqr=modelXsqr->record(index).value("a_500").toDouble();
            break;
        case 7:
           resXsqr=modelXsqr->record(index).value("a_250").toDouble();
            break;
        case 8:
           resXsqr=modelXsqr->record(index).value("a_100").toDouble();
            break;
        case 9:
           resXsqr=modelXsqr->record(index).value("a_050").toDouble();
            break;
        case 10:
           resXsqr=modelXsqr->record(index).value("a_025").toDouble();
            break;
        case 11:
           resXsqr=modelXsqr->record(index).value("a_010").toDouble();
            break;
        case 12:
           resXsqr=modelXsqr->record(index).value("a_005").toDouble();
            break;
         default:
            resXsqr=modelXsqr->record(index).value("a_950").toDouble();

            break;
        }
        qDebug()<<"resXsqr="<<resXsqr;
        QString s = QString::number(resXsqr);
        ui->textEditXsqr->setText(s);

}

void PlotDialog::on_pushButtonLinearTrend_clicked()
{
   double A=0;//Сумма [x_i*y_i]
   double B=0;//Сумма [x_i]^2
   double C=0;//Сумма [x_i]
   double D=0;//Сумма [y_i]
   int N=initialDataY.size();

   double tb=0;
   double ta=0;

   //calc A B C D
   for (int i=0; i<initialDataY.size();i++){
       A=A+initialDataY[i]*initialDataX[i];
       B=B+initialDataX[i]*initialDataX[i];
       C=C+initialDataX[i];
       D=D+initialDataY[i];
   }
   qDebug()<<"A,B,C,D="<<A<<","<<B<<","<<C<<","<<D;

   tb=(((D*B)/C)-A)/(((N*B)/C)-C);
   ta=(D-tb*N)/C;

   qDebug()<<"tb="<<tb<<";ta="<<ta;
   LinearTrendY.resize(N);
   for (int i=0; i<initialDataY.size();i++){
    LinearTrendY[i]=ta*initialDataX[i]+tb;
   }

   QString graphName=tr("Linear Trend");
   PlotTrends *plot = new PlotTrends(graphName, initialDataX, initialDataY, initialDataX,LinearTrendY,this);
   plot->setModal(true);
   plot->show();
}
