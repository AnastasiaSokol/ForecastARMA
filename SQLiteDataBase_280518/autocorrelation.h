#ifndef AUTOCORRELATION_H
#define AUTOCORRELATION_H
#include <QDialog>

namespace Ui {
class Dialog; //ссылка на ui
}

class autocorrelation : public QDialog
{
    Q_OBJECT

public:
    explicit autocorrelation(
            QString graphName,
            QVector<double> X, //массив исходных данных временного ряда
            QVector<double> Y,
            QDialog *parent = 0);
    ~autocorrelation();

public slots:
    void MakePlot();
    double Max(QVector<double> A);
    double Min(QVector<double> A);

private:
    Ui::Dialog *ui;
    QVector<double> DataX;
    QVector<double> DataY;
};


#endif // AUTOCORRELATION_H
