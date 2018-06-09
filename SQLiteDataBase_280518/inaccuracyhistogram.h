#ifndef INACCURACYHISTOGRAM_H
#define INACCURACYHISTOGRAM_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class InaccuracyHistogram : public QDialog
{
    Q_OBJECT

public:
    explicit InaccuracyHistogram(
            QString graphName,
            QVector<double> X,
            QVector<double> Y,
            int TypeOfGraph,
            QDialog *parent = 0);
    ~InaccuracyHistogram();

public slots:
    void MakePlot();
    double Max(QVector<double> A);
    double Min(QVector<double> A);

private:
    Ui::Dialog *ui;
    QVector<double> InaccuracyX;
    QVector<double> InaccuracyY;
    QVector<double> HistogramX;
    QVector<double> HistogramY;
    int TypeGraph;
    int countOfZones;//количество зон
    double step; //шаг для гистограммы
    double d;//разница между минимумом и максимумом
};

#endif // INACCURACYHISTOGRAM_H
