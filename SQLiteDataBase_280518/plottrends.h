#ifndef PLOTTRENDS_H
#define PLOTTRENDS_H
#include <QDialog>

namespace Ui {
class Dialog;
}

class PlotTrends : public QDialog
{
    Q_OBJECT

public:
    explicit PlotTrends(
            QString graphName,
            QVector<double> X,
            QVector<double> Y,
            QVector<double> tX,//trend data
            QVector<double> tY,
            QDialog *parent = 0);
    ~PlotTrends();
public slots:
    void MakePlot();
    double Max(QVector<double> A);
    double Min(QVector<double> A);

private:
    Ui::Dialog *ui;
    QVector<double> InitDataX;
    QVector<double> InitDataY;
    QVector<double> TrendX;
    QVector<double> TrendY;
    double d;//разница между минимумом и максимумом
};
#endif // PLOTTRENDS_H
