#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QRandomGenerator>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QVBoxLayout>
#include <QPushButton>
#include <QValueAxis>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);

    iteration =0;

    QWidget  *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout*mainLayout=new  QVBoxLayout(central);
    QHBoxLayout *topLayout = new QHBoxLayout();

    QPushButton *startButton = new QPushButton("start");
    QPushButton *stopButton = new QPushButton("stop");
    statusLabel = new QLabel("status Idle");

    topLayout->addWidget(startButton);
    topLayout->addWidget(stopButton);
    topLayout->addWidget(statusLabel);
    mainLayout->addLayout(topLayout);

    QVBoxLayout *speedLayout = new QVBoxLayout;
    QLabel *sliderLabel = new QLabel(tr("Speed (ms)"));
    speedSlider = new QSlider(Qt::Horizontal);
    speedSlider->setRange(100, 2000);
    speedSlider->setValue(500);
    speedSlider->setTickInterval(200);
    speedSlider->setTickPosition(QSlider::TicksBelow);
    connect(speedSlider, &QSlider::valueChanged, this, &MainWindow::changeSpeed);
    speedLayout->addWidget(sliderLabel);
    speedLayout->addWidget(speedSlider);
    mainLayout->addLayout(speedLayout);

    residualSeries = new QLineSeries();
    residualSeries->setName("Residual");
    residualSeries->setColor(Qt::green);

    fosSeries = new QLineSeries();
    fosSeries->setName("Fos");
    fosSeries->setColor(Qt::cyan);

    chart = new QChart();
    chart->addSeries(residualSeries);
    chart->addSeries(fosSeries);
    chart->setTitle("HMI Simulation Dashboard");
  //  chart->setBackgroundBrush(QColor("#1e1e1e"));

    QValueAxis *axisX=new QValueAxis;
    axisX->setTitleText("Iteration");
    axisX->setLabelsColor(Qt::white);

    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText("Values");
    axisY->setLabelsColor(Qt::white);
    axisY->setRange(0, 100);
    chart->addAxis(axisX,Qt::AlignBottom);
    chart->addAxis(axisY,Qt::AlignLeft);

    residualSeries->attachAxis(axisX);
    residualSeries->attachAxis(axisY);
    fosSeries->attachAxis(axisX);
    fosSeries->attachAxis(axisY);


    chartView = new QChartView(chart);
    chartView->setMinimumHeight(300);

    mainLayout->addWidget(chartView);

    table = new QTableWidget();
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels({"Iteration","Residual","Fos","Status"});
    mainLayout->addWidget(table);


    connect(timer,&QTimer::timeout,this,&MainWindow::updateSimulation);
    connect(startButton,&QPushButton::clicked,this,&MainWindow::startSimulation);
   connect(stopButton,&QPushButton::clicked,this,&MainWindow::stopSimulation);

}

void MainWindow::styleui()
{
    const QString buttonStyle = R"(
        QPushButton {
            background:#444444; color:#FFFFFF;
            padding:6px 12px;
            border:none;border-radius:4px;
        }
        QPushButton:hover { background:#555555; }
        QPushButton:pressed{ background:#333333; }
    )";
    startButton->setStyleSheet(buttonStyle);
    stopButton->setStyleSheet(buttonStyle);

    /* 3 – Status label (plain white, bold) */
    statusLabel->setStyleSheet("color:white; font-weight:bold;");

    /* 4 – Slider – dark track, light handle */
    const QString sliderStyle = R"(
        QSlider::groove:horizontal {
            background:#555555; height:7px;
            border-radius:3px;
        }
        QSlider::handle:horizontal {
            background:#AAAAAA; width:12px; height:12px;
            border-radius:6px; margin:-3px 0;
        }
        QSlider::sub-page:horizontal { background:#777777; }
    )";
    speedSlider->setStyleSheet(sliderStyle);

    /* 5 – Table styling : dark background, white text */
    const QString tableStyle = R"(
        QTableWidget { background:#212121; color:white; }
        QHeaderView::section { background:#3a3a3a; color:white; padding:4px; border:none; }
        QTableView::item { padding:4px; }
        QTableView::item:selected { background:#5c6bc0; }
        QTableView::item:hover { background:#444444; }
    )";
    table->setStyleSheet(tableStyle);

    /* 6 – Chart theme + anti‑aliasing (already set in constructor) */
    /* 7 – Layout spacing – give a small margin and compact spacing */
    if (auto *vl = qobject_cast<QVBoxLayout*>(centralWidget()->layout())) {
        vl->setContentsMargins(12, 12, 12, 12);
        vl->setSpacing(10);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startSimulation()
{
    statusLabel->setText("Status: Running");
    timer->start(500);
}

void MainWindow::updateSimulation()
{
    double residual = 1.0 / (iteration + 1)
    + QRandomGenerator::global()->bounded(0.02);

    double fos = 1.0 + iteration *20;

    QString status = (residual < 0.01) ? "Converged" : "Running";
    statusLabel->setText(status);
    int row = table->rowCount();
    table->insertRow(row);

    table->setItem(row, 0, new QTableWidgetItem(QString::number(iteration)));
    table->setItem(row, 1, new QTableWidgetItem(QString::number(residual)));
    table->setItem(row, 2, new QTableWidgetItem(QString::number(fos)));
    table->setItem(row, 3, new QTableWidgetItem(status));

    residualSeries->append(iteration, residual);
    fosSeries->append(iteration, fos);
    iteration++;
}

void MainWindow::stopSimulation()
{
    statusLabel->setText("Status: stop");
    timer->stop();
}

void MainWindow::changeSpeed(int ms)
{
    if (timer->isActive())
        timer->setInterval(ms);
}