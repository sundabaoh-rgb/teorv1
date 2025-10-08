#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QChartView>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QLineSeries>
#include <QVBoxLayout>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QScatterSeries>
#include <QTableWidget>
#include <format>
#include "lib/mlib.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void fillTable(QTableWidget *table);
    void updateChart(QChartView *chartView, int tsk);
    void updateDeviationChart(QChartView *chartView, int tsk);
    ~MainWindow();

private slots:
    void on_pushButton_2_clicked();

    void on_genButton1_2_clicked();

private:
    Ui::MainWindow *ui;
    void on_genButton1_clicked();
};
#endif // MAINWINDOW_H
