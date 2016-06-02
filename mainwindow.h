#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTimer>
#include <QDebug>
#include "schedulealgorithm.h"
#include "datastruct.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    enum {STATUS_FCFS, STATUS_RR, STATUS_LEVEL,STATUS_SJF,STATUS_HRRN}m_Status;
public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void InitWidget();
    void InitConnect();
    void Update();

private slots:

    void Exec();

    void on_BTN_START_clicked();

    void on_RR_clicked();

    void on_FCFS_clicked();

    void on_LEVEL_clicked();

    void on_BTN_RESUME_clicked();

    void on_BTN_ADD_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_SJF_clicked();

    void on_HRRN_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *m_Timer;
    ScheduleAlgorithm *m_pScheAlgo;
};

#endif // MAINWINDOW_H
