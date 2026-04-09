#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QLabel>
#include <QTableWidget>
#include<QSlider>
#include <QPushButton>
#include<QValueAxis>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    void updateSimulation();
    void startSimulation();
    void stopSimulation();
    void changeSpeed(int ms);

private:
    void styleui();

private:
    Ui::MainWindow *ui;

    QTimer *timer;
    int iteration;

    QLineSeries *residualSeries;
    QLineSeries *fosSeries;
    QLabel *statusLabel ;
    QTableWidget *table;
    QSlider *speedSlider;

    QPushButton  *startButton  = nullptr;
    QPushButton  *stopButton = nullptr;
    QChart       *chart          = nullptr;
    QChartView   *chartView       = nullptr;

};
#endif // MAINWINDOW_H
