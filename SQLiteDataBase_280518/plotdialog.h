#ifndef PLOTDIALOG_H
#define PLOTDIALOG_H
#include <QDialog>
#include <QTableWidget>
#include "widget.h"
#include <stdio.h>
#include <stdlib.h>
#include <inaccuracyhistogram.h>
#include <plottrends.h> //построение трендов на графике исходных данных
#include <autocorrelation.h> //построение функции автокорреляции
#include <math.h>

#define PI 3.1415926536

namespace Ui {
    class Dialog;
}
class PlotDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PlotDialog(QWidget *parent = 0);
    ~PlotDialog();



public slots:
    void makePlot();
private slots:
    void on_spinBoxMinRangeX_valueChanged(int arg1);
    void on_spinBoxMaxRangeX_valueChanged(int arg1);
    void on_spinBoxMinRangeY_valueChanged(int arg1);
    void on_spinBoxMaxRangeY_valueChanged(int arg1);
    void on_pushButtonGet_clicked();
    void on_checkBoxDifference_clicked();
    void on_ButtonPlotTransformVar_clicked();
    void on_spinBox_OrderAR_valueChanged(int arg1);
    void on_doubleSpinBoxCoeff2_valueChanged(double arg1);
    void on_checkBoxPrintCoefficients_clicked();
    void on_checkBoxPrint_Initial_Data_clicked();
    void on_comboBoxNumberOfCoefficient_currentIndexChanged(int index);
    void on_spinBoxFromMoment_valueChanged(int arg1);
    double on_pushButtonGenerateWhiteNoise_clicked();
    void on_pushButtonPlotInaccuracyHistogram_clicked();

    void on_pushButtonPlotHistogram_clicked();

    void on_pushButtonPlotAutocorrelation_clicked();

    void on_checkBoxForecastForTransformVar_clicked();

    void on_checkBoxPrintTransformVar_clicked();

    void on_spinBoxOrderMA_valueChanged(int arg1);

    void on_comboBoxCoeffMA_currentIndexChanged(int index);

    void on_doubleSpinBoxCoeffMA_valueChanged(double arg1);

    void on_checkBoxPrintForecast_clicked();

    void on_spinBoxLag_valueChanged(int arg1);

    void on_pushButtonPlotWhiteNoise_clicked();

    void on_pushButtonHistogramWhiteNoise_clicked();

    void on_pushButton_clicked();

    double Max(QVector<double> A);
    double Min(QVector<double> A);
    double Average(QVector<double> A);//поиск среднего

    double lagSumYi(QVector<double> A, int lag);
    double lagSumYi_lag(QVector<double> A, int lag);
    double lagSumYiYi_lag(QVector<double> A, int lag);
    double SqrlagSumYi(QVector<double> A, int lag);//Сумма квадратов yi
    double SqrlagSumYi_lag(QVector<double> A, int lag);//Сумма квадратов yi+lag
    double VCoeffAutocorrelation(QVector<double> A, int lag);//выборочный коэффициент автокоррелации

    void FillTableInitialData(QVector<double> A);//заполнение таблицы на вкладке "Исходные данные"


    void FillTableAutocorrelation();//Заполнение таблицы с результатами посдчетов  АКФ

    void on_pushButtonCalcAKF_clicked();//Слот реалирующий на нажатие кнопки "Подсчет АКФ"
    QVector<double> CalcAKFY(QVector<double> DataY);//подсчет АКФ
    QVector<double> CalcAKFX(QVector<double> DataY);//подсчет АКФ

    void on_pushButtonIrvinCriterion_clicked();
    QString STRAllocation(QString str, QString leftstr, QString rightstr);//Выделение подстроки в строке окруженной контекстом

    void on_checkBoxAutocorrelation_clicked();
    void ShowTableAutocorrelation(QTableWidget *tw, QVector<double> ACRY, QVector<double> ACRX, QStringList Header);//Отобразить таблицу с автокорреляционной функцией
    void on_plotInaccuracyACR_clicked();
    void PlotACR(QVector<double> DataY, QVector<double> ACRY, QVector<double> ACRX);//

    void on_ButtonCalcQcriterion_clicked();

    void on_ButtonCalcXsqr_clicked();
    void FillXsqrModel();//заполняет таблицу значениями хи квадрат распределения

    void on_pushButtonLinearTrend_clicked();

private:
    int         countOfData;
    int         ForecoastCountOfData;
    double      averageOfInitialData;
    double      constTerm;
    double      SelectiveVariance;//Выборочная дисперсия
    double      UnbiasedDispersion;//Несмещенная дисперсия
    double      SquareDeviation;//Среднеквадратическое отклонение
    int         forward;
    int         lag;//порядок разностного ряда;
    int         momentOfForecasting;
    int         countOfCoefficientsAR;
    int         countOfCoefficientsMA;
    int         OrderAR;//порядок авторегрессии
    int         OrderMA;//порядок скользящего среднего
    QVector<double> coefficientsAR;
    QVector<double> coefficientsMA;
    QVector<double> initialDataY;
    QVector<double> initialDataX;
    QVector<double> ForecastX;
    QVector<double> ForecastY;
    QVector<double> InaccuracyX;
    QVector<double> InaccuracyY;
    QVector<double> TransformX;
    QVector<double> TransformY;
    QVector<double> AutocorrelationX;
    QVector<double> AutocorrelationY;
    QVector<double> AutocorrelationInaccuracyX;//Автокорреляция ряда остатков
    QVector<double> AutocorrelationInaccuracyY;
    QVector<double> LinearTrendY;//значения функции линейного тренда

    QVector<double> WhiteNoiseY;
    QVector<double> WhiteNoiseX;
    QStringList     colors;//список цветов для графиков

    QStringList     tableInitialDataHeader;//Список заголовков для таблицы на вкладке "Исходные данные"
    QStringList     tableAutocorrelationHeader;//Список заголовков для таблицы на вкладке "Автокорреляция"

    bool     ForecastPlot;//флаг = истине если граф прогноза построен
    bool     DifferencePlot;//флаг = истине если граф разностей построен
    bool     UseTransformVar;//флаг того, что нужно использовать преобразованные данные для построения прогноза

    int      indexGraphOfForecast;//номер графа - прогноза
    int      indexGraphOfTransform;//номер графа - разностного

    //Табулируемые значения для метода ирвина
    QVector<double> IrvinN;//Количество значений выборки
    QVector<double> IrvinA;//вероятность 0,1
    QVector<double> IrvinB;//Вероятность 0,05
    QVector<double> IrvinC;//Вероятность 0,01
    QVector<double> IrvinCriterion;//I_i
    double          IrvinLimit;//I предельный


    //Модели таблиц
    QSqlQueryModel *modelStiudent;//Таблица распределения Стьюдента
    QSqlQueryModel *modelIrvin;//Таблица распределения Ирвина
    QSqlQueryModel *modelXsqr;//Таблица распределения хи квадрат

    Ui::Dialog  *ui;


};
#endif // PLOTDIALOG_H
