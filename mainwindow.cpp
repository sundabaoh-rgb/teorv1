#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(this->size());
    connect(ui->genButton1, &QPushButton::clicked,
            this, &MainWindow::on_genButton1_clicked); // генерация задание 1
    connect(ui->genButton1_2, &QPushButton::clicked,
            this, &MainWindow::on_genButton1_2_clicked); // генерация задание 2
    connect(ui->pushButton_2, &QPushButton::clicked,
            this, &MainWindow::on_pushButton_2_clicked); //очистка

    //Таблица 1е задание настройка
    QStringList headers = {"N", "Pi", "Deviation", "Quality"};
    ui->tableWidget_2->setColumnCount(headers.size());
    ui->tableWidget_2->setHorizontalHeaderLabels(headers);
    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_2->setAlternatingRowColors(true);
    ui->tableWidget_2->setSelectionBehavior(QAbstractItemView::SelectRows);

    //Таблица 2е задание настройка
    ui->tableWidget_3->setColumnCount(headers.size());
    ui->tableWidget_3->setHorizontalHeaderLabels(headers);
    ui->tableWidget_3->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_3->setAlternatingRowColors(true);
    ui->tableWidget_3->setSelectionBehavior(QAbstractItemView::SelectRows);

    qApp->setStyleSheet(
        "QMainWindow, QDialog, QWidget { "
        "    background-color: #2b2b2b; "
        "    color: #ffffff; "
        "} "
        "QTableWidget { "
        "    background-color: #1e1e1e; "
        "    color: #ffffff; "
        "    gridline-color: #404040; "
        "    alternate-background-color: #252525; "
        "} "
        "QHeaderView::section { "
        "    background-color: #3c3c3c; "
        "    color: #ffffff; "
        "    border: 1px solid #555555; "
        "    padding: 5px; "
        "} "
        "QPushButton { "
        "    background-color: #404040; "
        "    color: #ffffff; "
        "    border: 1px solid #555555; "
        "    padding: 5px 10px; "
        "    border-radius: 3px; "
        "} "
        "QPushButton:hover { "
        "    background-color: #505050; "
        "} "
        "QPushButton:pressed { "
        "    background-color: #606060; "
        "} "
        "QTabWidget::pane { "
        "    border: 1px solid #555555; "
        "    background-color: #2b2b2b; "
        "} "
        "QTabBar::tab { "
        "    background-color: #404040; "
        "    color: #ffffff; "
        "    padding: 8px 16px; "
        "    border: 1px solid #555555; "
        "} "
        "QTabBar::tab:selected { "
        "    background-color: #505050; "
        "}"
        );
}

void MainWindow::updateChart(QChartView *chartView, int tsk)
{
    if (result.empty()) {
        qDebug() << "Нет данных для графика";
        return;
    }

    QChart *chart = new QChart();

    // Основная серия данных
    QLineSeries *series = new QLineSeries();
    series->setName("Вероятность Pi");

    // Теоретическая линия
    QLineSeries *theorySeries = new QLineSeries();
    double theoryValue = (tsk == 2) ? REF_TSK2 : 0.5;

    if (tsk != 2) {
        theorySeries->setName("Теоретическое значение (0.5)");
    } else {
        QString str = QString("Теоретическое значение (%1)").arg(REF_TSK2, 0, 'f', 6);
        theorySeries->setName(str);
    }

    QStringList categories;

    if (tsk != 2) {
        // Для категоричных данных - используем категории как есть
        for (int i = 0; i < result.size(); i++) {
            series->append(i, result[i].Pi);
            categories << QString::number(result[i].N);
        }

        // Теоретическая линия для категорий
        if (!result.empty()) {
            theorySeries->append(0, theoryValue);
            theorySeries->append(result.size() - 1, theoryValue);
        }
    } else {
        // Для вещественных данных - используем числовые значения
        double minN = result.front().N;
        double maxN = result.back().N;

        for (const auto& res : result) {
            series->append(res.N, res.Pi);
        }

        // Теоретическая линия для числовых значений
        theorySeries->append(minN, theoryValue);
        theorySeries->append(maxN, theoryValue);
    }

    // Добавляем серии
    chart->addSeries(series);
    chart->addSeries(theorySeries);

    // Настраиваем заголовок
    if (tsk != 2) {
        chart->setTitle("Зависимость Pi от количества опытов (категоричные данные)");
    } else {
        chart->setTitle("Зависимость Pi от количества опытов (вещественные данные)");
    }

    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Настраиваем оси в зависимости от типа задачи
    if (tsk != 2) {
        // Для категоричных данных - ось X с категориями
        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->append(categories);
        axisX->setTitleText("Количество опытов (N)");

        QValueAxis *axisY = new QValueAxis();
        axisY->setTitleText("Вероятность Pi");
        axisY->setRange(0.0, 1.0);
        axisY->setTickCount(11);

        chart->addAxis(axisX, Qt::AlignBottom);
        chart->addAxis(axisY, Qt::AlignLeft);

        series->attachAxis(axisX);
        series->attachAxis(axisY);
        theorySeries->attachAxis(axisX);
        theorySeries->attachAxis(axisY);
    } else {
        // Для вещественных данных - числовая ось X
        double minN = result.front().N;
        double maxN = result.back().N;

        QValueAxis *axisX = new QValueAxis();
        axisX->setTitleText("Количество опытов (N)");
        axisX->setRange(minN, maxN);

        // Форматируем метки для больших чисел
        if (maxN >= 1000) {
            axisX->setLabelFormat("%.0f");
            axisX->setTickCount(10);
        }

        QValueAxis *axisY = new QValueAxis();
        axisY->setTitleText("Вероятность Pi");

        double minY = qMax(0.0, theoryValue - 0.05);
        double maxY = qMin(1.0, theoryValue + 0.05);
        axisY->setRange(minY, maxY);
        axisY->setTickCount(6);

        chart->addAxis(axisX, Qt::AlignBottom);
        chart->addAxis(axisY, Qt::AlignLeft);

        series->attachAxis(axisX);
        series->attachAxis(axisY);
        theorySeries->attachAxis(axisX);
        theorySeries->attachAxis(axisY);
    }

    chart->setTheme(QChart::ChartThemeDark);
    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    qDebug() << "График обновлен. Тип:" << (tsk != 2 ? "категоричные" : "вещественные")
             << ", точек:" << result.size();
}

void MainWindow::updateDeviationChart(QChartView *chartView, int tsk)
{
    if (result.empty()) return;

    QChart *chart = new QChart();

    // ВКЛЮЧАЕМ АНИМАЦИИ
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setAnimationDuration(2500);


    QLineSeries *series = new QLineSeries();
    series->setName("Отклонение");
    series->setPen(QPen(Qt::cyan, 2));

    QScatterSeries *pointSeries = new QScatterSeries();
    pointSeries->setName("Точки отклонений");
    pointSeries->setMarkerSize(10.0);
    pointSeries->setBorderColor(Qt::white);

    QStringList categories;

    for (int i = 0; i < result.size(); i++) {
        double deviation = result[i].deviation;
        series->append(i, deviation);
        pointSeries->append(i, deviation);

        if (deviation < 0.05) {
            pointSeries->setColor(Qt::green);
        } else if (deviation < 0.1) {
            pointSeries->setColor(QColor(255, 165, 0));
        } else {
            pointSeries->setColor(Qt::red);
        }

        categories << QString::number(result[i].N);
    }

    chart->addSeries(series);
    chart->addSeries(pointSeries);
    chart->setTitle("График отклонений |Pi - 0.5|");

    // Оси
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setTitleText("Количество опытов (N)");

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Отклонение");
    axisY->setRange(0.0, 0.3);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    series->attachAxis(axisX);
    series->attachAxis(axisY);
    pointSeries->attachAxis(axisX);
    pointSeries->attachAxis(axisY);

    // ТЕМА ПОСЛЕ ВСЕГО
    chart->setTheme(QChart::ChartThemeDark);

    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::fillTable(QTableWidget *table)
{
    table->setRowCount(0);

    if (result.empty()) {
        qDebug() << "Нет данных для отображения";
        return;
    }

    table->setRowCount(result.size());

    for (int i = 0; i < result.size(); ++i) {
        const Result &res = result[i];

        QTableWidgetItem *itemN = new QTableWidgetItem(QString::number(res.N));
        itemN->setTextAlignment(Qt::AlignCenter);
        table->setItem(i, 0, itemN);

        QTableWidgetItem *itemPi = new QTableWidgetItem(QString::number(res.Pi, 'f', 6));
        itemPi->setTextAlignment(Qt::AlignCenter);
        table->setItem(i, 1, itemPi);

        QTableWidgetItem *itemDev = new QTableWidgetItem(QString::number(res.deviation, 'f', 6));
        itemDev->setTextAlignment(Qt::AlignCenter);

        //
        if (res.deviation < 0.01) {
            itemDev->setBackground(Qt::darkGreen);
            itemDev->setForeground(Qt::black);
        } else if (res.deviation < 0.05) {
            itemDev->setBackground(QColor(100, 130, 255));
            itemDev->setForeground(Qt::black);
        }
        else if (res.deviation < 0.1) {
            itemDev->setBackground(QColor(255, 140, 0));
            itemDev->setForeground(Qt::black);
        } else {
            itemDev->setBackground(Qt::red);
            itemDev->setForeground(Qt::black);
        }
        table->setItem(i, 2, itemDev);

        QString quality;
        if (res.deviation < 0.01) quality = "Отлично";
        else if (res.deviation < 0.05) quality = "Хорошо";
        else if (res.deviation < 0.1) quality = "Удовлетворительно";
        else quality = "Плохо";

        QTableWidgetItem *itemQuality = new QTableWidgetItem(quality);
        itemQuality->setTextAlignment(Qt::AlignCenter);

        if (res.deviation < 0.01) {
            itemQuality->setForeground(Qt::darkGreen);
        } else if (res.deviation < 0.05) {
            itemQuality->setForeground(Qt::blue);
        } else if (res.deviation < 0.1) {
            itemQuality->setForeground(QColor(255, 140, 0));
        } else {
            itemQuality->setForeground(Qt::red);
        }

        table->setItem(i, 3, itemQuality);
    }

    qDebug() << "Заполнено строк:" << table->rowCount();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_genButton1_clicked()
{
    on_pushButton_2_clicked();
    gen();
    fillTable(ui->tableWidget_2);
    updateChart(ui->widget,1);
    updateDeviationChart(ui->widget_2,1);
}


void MainWindow::on_pushButton_2_clicked()
{
    ui->tableWidget_2->setRowCount(0);
    result.clear();

    // QChart *emptyChart = new QChart();
    // emptyChart->setTitle("Нет данных");
    // ui->widget->setChart(emptyChart);
}


void MainWindow::on_genButton1_2_clicked()
{
    ui->tableWidget_2->setRowCount(0);
    result.clear();
    gen2();
    fillTable(ui->tableWidget_3);
    updateChart(ui->widget_6,2);
    updateDeviationChart(ui->widget_5,2);

}

