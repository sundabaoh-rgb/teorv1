#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->genButton1, &QPushButton::clicked,
            this, &MainWindow::on_genButton1_clicked); // генерация
    connect(ui->pushButton_2, &QPushButton::clicked,
            this, &MainWindow::on_pushButton_2_clicked); //очистка

    //Таблица 1е задание настройка
    QStringList headers = {"N", "Pi", "Deviation", "Quality"};
    ui->tableWidget_2->setColumnCount(headers.size());
    ui->tableWidget_2->setHorizontalHeaderLabels(headers);
    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // Дополнительные настройки
    ui->tableWidget_2->setAlternatingRowColors(true);
    ui->tableWidget_2->setSelectionBehavior(QAbstractItemView::SelectRows);

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

void MainWindow::updateChart()
{
    if (result.empty()) {
        qDebug() << "Нет данных для графика";
        return;
    }

    QChartView *chartView = ui->widget;
    QChart *chart = new QChart();

    // Основная серия данных
    QLineSeries *series = new QLineSeries();
    series->setName("Вероятность Pi");

    // Теоретическая линия
    QLineSeries *theorySeries = new QLineSeries();
    theorySeries->setName("Теоретическое значение (0.5)");
    theorySeries->setPen(QPen(Qt::red, 2, Qt::DashLine));

    if (!result.empty()) {
        theorySeries->append(0, 0.5);                    // начало: первая категория
        theorySeries->append(result.size() - 1, 0.5);    // конец: последняя категория
    }

    QStringList categories;

    // Заполняем данные
    for (int i = 0; i < result.size(); i++) {
        series->append(i, result[i].Pi);
        categories << QString::number(result[i].N);
    }

    if (!result.empty()) {
        double minN = result.front().N;
        double maxN = result.back().N;
        theorySeries->append(minN, 0.5);
        theorySeries->append(maxN, 0.5);
    }

    // Добавляем серии
    chart->addSeries(series);
    chart->addSeries(theorySeries);
    chart->setTitle("Зависимость Pi от количества опытов");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setTitleText("Количество опытов (N)");

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Вероятность Pi");
    axisY->setRange(0.0, 1.0);
    axisY->setTickCount(11);

    // Добавляем оси к графику
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    // Привязываем серии к осям
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    theorySeries->attachAxis(axisX);
    theorySeries->attachAxis(axisY);

    // Темная тема
    chart->setTheme(QChart::ChartThemeDark);

    // Устанавливаем график
    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    qDebug() << "График обновлен с" << result.size() << "точками";
}

void MainWindow::fillTable()
{
    ui->tableWidget_2->setRowCount(0);

    if (result.empty()) {
        qDebug() << "Нет данных для отображения";
        return;
    }

    ui->tableWidget_2->setRowCount(result.size());

    for (int i = 0; i < result.size(); ++i) {
        const Result &res = result[i];

        QTableWidgetItem *itemN = new QTableWidgetItem(QString::number(res.N));
        itemN->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_2->setItem(i, 0, itemN);

        QTableWidgetItem *itemPi = new QTableWidgetItem(QString::number(res.Pi, 'f', 6));
        itemPi->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_2->setItem(i, 1, itemPi);

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
        ui->tableWidget_2->setItem(i, 2, itemDev);

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

        ui->tableWidget_2->setItem(i, 3, itemQuality);
    }

    qDebug() << "Заполнено строк:" << ui->tableWidget_2->rowCount();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_genButton1_clicked()
{
    on_pushButton_2_clicked();
    gen();
    fillTable();
    updateChart();
}


void MainWindow::on_pushButton_2_clicked()
{
    ui->tableWidget_2->setRowCount(0);
    result.clear();

    QChart *emptyChart = new QChart();
    emptyChart->setTitle("Нет данных");
    ui->widget->setChart(emptyChart);
}

