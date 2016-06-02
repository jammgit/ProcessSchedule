#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_pScheAlgo = nullptr;
    m_Timer = new QTimer;

    InitWidget();
    InitConnect();
}

MainWindow::~MainWindow()
{
    delete m_pScheAlgo;
    delete m_Timer;
    delete ui;
}

void MainWindow::InitWidget()
{
    QStringList header;
    header << "进程名" << "需要运行时间(s)";
    ui->tableWidget->setHorizontalHeaderLabels(header);

    ui->tableWidget->setItem(0,0, new QTableWidgetItem("Job1"));
    ui->tableWidget->setItem(0,1, new QTableWidgetItem(QString::number(10)));
    ui->tableWidget->setItem(1,0, new QTableWidgetItem("Job2"));
    ui->tableWidget->setItem(1,1, new QTableWidgetItem(QString::number(20)));
    ui->tableWidget->setItem(2,0, new QTableWidgetItem("Job3"));
    ui->tableWidget->setItem(2,1, new QTableWidgetItem(QString::number(30)));

    ui->tableWidget->setRowCount(3);

}

void MainWindow::InitConnect()
{
    connect(m_Timer, SIGNAL(timeout()), this, SLOT(Exec()));
}

void MainWindow::Update()
{
    SCHE_STATE state = m_pScheAlgo->GetState();
    if (state == SCHE_ANOTHER_JOB)
    {
        /* move run to finish */
        int rows = ui->TABLE_FINISH->rowCount();
        ui->TABLE_FINISH->setRowCount(rows + 1);
        ui->TABLE_FINISH->setItem(rows,0, new QTableWidgetItem(ui->TABLE_RUN->item(0,0)->text()));
        ui->TABLE_FINISH->setItem(rows,1, new QTableWidgetItem(ui->TABLE_RUN->item(0,1)->text()));
        ui->TABLE_FINISH->setItem(rows,2, new QTableWidgetItem(ui->TABLE_RUN->item(0,2)->text()));
        ui->TABLE_FINISH->setItem(rows,3, new QTableWidgetItem(QString::number(ui->TABLE_RUN->item(0,3)->text().toInt()+1)));
        if (m_Status == STATUS_LEVEL || m_Status == STATUS_HRRN)
        {
             ui->TABLE_FINISH->setItem(rows,4, new QTableWidgetItem(ui->TABLE_RUN->item(0,4)->text()));
        }
        if (m_Status == STATUS_SJF || m_Status == STATUS_HRRN)
        {
            const std::list<PCB> *pf = m_pScheAlgo->GetFinishList();
            ui->TABLE_FINISH->setItem(rows,5, new QTableWidgetItem(pf->front().finish_time.toString("hh:mm:ss")));
            ui->TABLE_FINISH->setItem(rows,6, new QTableWidgetItem(QString::number(pf->front().arround_time,'f',6)));
            ui->TABLE_FINISH->setItem(rows,7, new QTableWidgetItem(QString::number(pf->front().weight_arround_time,'f',6)));
        }
        /* renew run */
        const std::list<PCB>* p = m_pScheAlgo->GetRunList();
        ui->TABLE_RUN->item(0, 0)->setText(p->front().job_name);
        ui->TABLE_RUN->item(0, 1)->setText(QString::number(p->front().need_time));
        ui->TABLE_RUN->item(0, 2)->setText(p->front().arrive_time.toString("hh:mm:ss"));
        ui->TABLE_RUN->item(0, 3)->setText(QString::number(p->front().exec_time));
        if (m_Status == STATUS_LEVEL)
             ui->TABLE_RUN->item(0, 4)->setText(QString::number(p->front().level));
        if (m_Status == STATUS_HRRN)
            ui->TABLE_RUN->item(0,4)->setText( QString::number(p->front().wait_time));
        /*  renew wait */
        QString jobname(p->front().job_name);
        rows = ui->TABLE_WAIT->rowCount();
        for (int i = 0; i < rows; ++i)
        {
            if (jobname == ui->TABLE_WAIT->item(i,0)->text())
            {
                ui->TABLE_WAIT->removeRow(i);
                break;
            }
        }

    }
    else if (state == SCHE_NEW_JOB)
    {
        const std::list<PCB>* p = m_pScheAlgo->GetRunList();
        QString jobname(p->front().job_name);
        int rows = ui->TABLE_RUN->rowCount();
        ui->TABLE_RUN->setRowCount(rows + 1);
        ui->TABLE_RUN->setItem(rows,0, new QTableWidgetItem(p->front().job_name));
        ui->TABLE_RUN->setItem(rows,1, new QTableWidgetItem(QString::number(p->front().need_time)));
        ui->TABLE_RUN->setItem(rows,2, new QTableWidgetItem(p->front().arrive_time.toString("hh:mm:ss")));
        ui->TABLE_RUN->setItem(rows,3, new QTableWidgetItem(QString::number(p->front().exec_time)));
        if (m_Status == STATUS_LEVEL)
             ui->TABLE_RUN->setItem(rows,4, new QTableWidgetItem(QString::number(p->front().level)));

        if (m_Status == STATUS_HRRN)
            ui->TABLE_RUN->setItem(rows,4, new QTableWidgetItem(QString::number(p->front().wait_time)));
        /*renew table wait*/
        rows = ui->TABLE_WAIT->rowCount();
        for (int i = 0; i < rows; ++i)
        {
            if (jobname == ui->TABLE_WAIT->item(i,0)->text())
            {
                ui->TABLE_WAIT->removeRow(i);
                break;
            }
        }
    }
    else if (state == SCHE_RENEW_TIME)
    {
        ui->TABLE_RUN->item(0,3)->setText(QString::number(ui->TABLE_RUN->item(0,3)->text().toInt() + 1));
    }
    else if (state == SCHE_COMPLETE_JOB)
    {
        int rows = ui->TABLE_FINISH->rowCount();
        ui->TABLE_FINISH->setRowCount(rows + 1);
        ui->TABLE_FINISH->setItem(rows,0, new QTableWidgetItem(ui->TABLE_RUN->item(0,0)->text()));
        ui->TABLE_FINISH->setItem(rows,1, new QTableWidgetItem(ui->TABLE_RUN->item(0,1)->text()));
        ui->TABLE_FINISH->setItem(rows,2, new QTableWidgetItem(ui->TABLE_RUN->item(0,2)->text()));
        ui->TABLE_FINISH->setItem(rows,3, new QTableWidgetItem(QString::number(ui->TABLE_RUN->item(0,3)->text().toInt()+1)));
        if (m_Status == STATUS_LEVEL || m_Status == STATUS_HRRN)
        {
             ui->TABLE_FINISH->setItem(rows,4, new QTableWidgetItem(ui->TABLE_RUN->item(0,4)->text()));
        }

        if (m_Status == STATUS_SJF || m_Status == STATUS_HRRN)
        {

            const std::list<PCB> *pf = m_pScheAlgo->GetFinishList();
            ui->TABLE_FINISH->setItem(rows,5, new QTableWidgetItem(pf->front().finish_time.toString("hh:mm:ss")));
            ui->TABLE_FINISH->setItem(rows,6, new QTableWidgetItem(QString::number(pf->front().arround_time,'f',6)));
            ui->TABLE_FINISH->setItem(rows,7, new QTableWidgetItem(QString::number(pf->front().weight_arround_time,'f',6)));
            /* 计算并打印这组作业的平均周转时间、带权平均周转时间 */
            auto iter = pf->begin();
            size_t idx;
            size_t size = pf->size();
            double arround_time = 0;
            double weight_arround_time = 0;
            for (idx = 0; idx < size; ++idx)
            {
                arround_time += iter->arround_time;
                weight_arround_time += iter->weight_arround_time;
                ++iter;
            }
            arround_time /= size;
            weight_arround_time /= size;
            ui->lineEdit_2->setText(QString::number(arround_time,'f',6));
            ui->lineEdit_3->setText(QString::number(weight_arround_time,'f',6));
        }
        ui->TABLE_RUN->removeRow(0);

    }
    else if (state == SCHE_RRTIME_ARRI)
    {
        int rows = ui->TABLE_WAIT->rowCount();
        ui->TABLE_WAIT->setRowCount(rows + 1);
        ui->TABLE_WAIT->setItem(rows,0, new QTableWidgetItem(ui->TABLE_RUN->item(0,0)->text()));
        ui->TABLE_WAIT->setItem(rows,1, new QTableWidgetItem(ui->TABLE_RUN->item(0,1)->text()));
        ui->TABLE_WAIT->setItem(rows,2, new QTableWidgetItem(ui->TABLE_RUN->item(0,2)->text()));
        ui->TABLE_WAIT->setItem(rows,3, new QTableWidgetItem(QString::number(ui->TABLE_RUN->item(0,3)->text().toInt()+1)));
        if (m_Status == STATUS_LEVEL || m_Status == STATUS_HRRN)
        {
             ui->TABLE_WAIT->setItem(rows,4, new QTableWidgetItem(ui->TABLE_RUN->item(0,4)->text()));
        }
        ui->TABLE_RUN->item(0, 0)->setText(ui->TABLE_WAIT->item(0,0)->text());
        ui->TABLE_RUN->item(0, 1)->setText(ui->TABLE_WAIT->item(0,1)->text());
        ui->TABLE_RUN->item(0, 2)->setText(ui->TABLE_WAIT->item(0,2)->text());
        ui->TABLE_RUN->item(0, 3)->setText(ui->TABLE_WAIT->item(0,3)->text());
        if (m_Status == STATUS_LEVEL || m_Status == STATUS_HRRN)
        {
             ui->TABLE_RUN->item(0, 4)->setText(ui->TABLE_WAIT->item(0,4)->text());
        }
        ui->TABLE_WAIT->removeRow(0);
    }
    if (m_Status == STATUS_HRRN)
    {
        int rows = ui->TABLE_WAIT->rowCount();
        for (int i = 0; i < rows; ++i)
        {
           ui->TABLE_WAIT->item(i,4)->setText(
                       QString::number(ui->TABLE_WAIT->item(i,4)->text().toInt() + 1));
        }
    }
}

void MainWindow::Exec()
{
    m_pScheAlgo->Exec();
    m_Timer->start(1000);
}

void MainWindow::on_BTN_START_clicked()
{
    ui->BTN_START->setEnabled(false);
    ui->BTN_ADD->setEnabled(true);

    if (m_pScheAlgo)
    {
        delete m_pScheAlgo;
        m_pScheAlgo = nullptr;
    }
    QStringList header;

    header << "" << "" << "" << "" << "";
    ui->TABLE_RUN->setHorizontalHeaderLabels(header);
    ui->TABLE_WAIT->setHorizontalHeaderLabels(header);
    header << "" << "" << "";
    ui->TABLE_FINISH->setHorizontalHeaderLabels(header);
    header.clear();

    header << "进程名" << "需要运行时间(s)" << "到达时间" << "已运行时间(S)";
    ui->TABLE_RUN->setHorizontalHeaderLabels(header);
    ui->TABLE_WAIT->setHorizontalHeaderLabels(header);
    ui->TABLE_FINISH->setHorizontalHeaderLabels(header);
    if (ui->FCFS->isChecked())
    {
        m_pScheAlgo = new FCFS;
        m_Status = STATUS_FCFS;
    }
    else if (ui->LEVEL->isChecked())
    {
        m_pScheAlgo = new LEVEL;
        m_Status = STATUS_LEVEL;
        QStringList header;
        header << "" << "" << "" << "" << "";
        ui->TABLE_RUN->setHorizontalHeaderLabels(header);
        ui->TABLE_WAIT->setHorizontalHeaderLabels(header);
        header << "" << "" << "";
        ui->TABLE_FINISH->setHorizontalHeaderLabels(header);
        header.clear();
        header << "进程名" << "需要运行时间(s)" << "到达时间" << "已运行时间(S)" << "优先级";
        ui->TABLE_RUN->setHorizontalHeaderLabels(header);
        ui->TABLE_WAIT->setHorizontalHeaderLabels(header);
        ui->TABLE_FINISH->setHorizontalHeaderLabels(header);
    }
    else if(ui->RR->isChecked())
    {
        m_pScheAlgo = new RR(ui->lineEdit->text().toInt());
        m_Status = STATUS_RR;
    }
    else if(ui->SJF->isChecked())
    {
        m_pScheAlgo = new SJF;
        m_Status = STATUS_SJF;
    }
    else if(ui->HRRN->isChecked())
    {
        m_pScheAlgo = new HRRN;
        m_Status = STATUS_HRRN;
        QStringList header;
        header << "" << "" << "" << "" << "" ;
        ui->TABLE_RUN->setHorizontalHeaderLabels(header);
        ui->TABLE_WAIT->setHorizontalHeaderLabels(header);
        header << "" << "" << "";
        ui->TABLE_FINISH->setHorizontalHeaderLabels(header);
        header.clear();
        header << "进程名" << "需要运行时间(s)" << "到达时间" << "已运行时间(S)" << "已等待时间(S)";
        ui->TABLE_RUN->setHorizontalHeaderLabels(header);
        ui->TABLE_WAIT->setHorizontalHeaderLabels(header);
        header << "完成时刻" << "周转时间(S)" << "带权周转时间(S)";
        ui->TABLE_FINISH->setHorizontalHeaderLabels(header);

    }

    m_pScheAlgo->AddWidget(this);

    m_Timer->start(1000);
}

void MainWindow::on_RR_clicked()
{
    ui->lineEdit->setEnabled(true);
    QStringList header;
    header << "进程名" << "需要运行时间(s)" << "";
    ui->tableWidget->setHorizontalHeaderLabels(header);
}

void MainWindow::on_FCFS_clicked()
{
    ui->lineEdit->setEnabled(false);
    QStringList header;
    header << "进程名" << "需要运行时间(s)" << "";
    ui->tableWidget->setHorizontalHeaderLabels(header);
}

void MainWindow::on_LEVEL_clicked()
{
    ui->lineEdit->setEnabled(false);
    QStringList header;
    header << "进程名" << "需要运行时间(s)" << "优先数";
    ui->tableWidget->setHorizontalHeaderLabels(header);
}

void MainWindow::on_SJF_clicked()
{
    ui->lineEdit->setEnabled(false);
    QStringList header;
    header << "进程名" << "需要运行时间(s)" << "";
    ui->tableWidget->setHorizontalHeaderLabels(header);
}

void MainWindow::on_HRRN_clicked()
{
    ui->lineEdit->setEnabled(false);
    QStringList header;
    header << "进程名" << "需要运行时间(s)" << "";
    ui->tableWidget->setHorizontalHeaderLabels(header);
}


void MainWindow::on_BTN_RESUME_clicked()
{/* back to default config */

    m_Timer->stop();
    ui->BTN_ADD->setEnabled(false);
    ui->BTN_START->setEnabled(true);

    ui->tableWidget->setItem(0,0, new QTableWidgetItem("Job1"));
    ui->tableWidget->setItem(0,1, new QTableWidgetItem(QString::number(10)));
    ui->tableWidget->setItem(1,0, new QTableWidgetItem("Job2"));
    ui->tableWidget->setItem(1,1, new QTableWidgetItem(QString::number(20)));
    ui->tableWidget->setItem(2,0, new QTableWidgetItem("Job3"));
    ui->tableWidget->setItem(2,1, new QTableWidgetItem(QString::number(30)));

    /* INitialize default jobs */
    if (m_pScheAlgo)
    {
        m_pScheAlgo->ClearJobs();
        delete m_pScheAlgo;
        m_pScheAlgo = nullptr;
    }
    int rows = ui->TABLE_RUN->rowCount();
    for (int i = 0; i < rows;++i)
    {
        ui->TABLE_RUN->removeRow(0);
    }
    rows = ui->TABLE_WAIT->rowCount();
    for (int i = 0; i < rows;++i)
    {
        ui->TABLE_WAIT->removeRow(0);
    }
    rows = ui->TABLE_FINISH->rowCount();
    for (int i = 0; i < rows;++i)
    {
        ui->TABLE_FINISH->removeRow(0);
    }

}

void MainWindow::on_BTN_ADD_clicked()
{
    int rows = ui->tableWidget->rowCount();

    for (int i = 0; i < rows; ++i)
    {
        /* add to wait table */
        int row = ui->TABLE_WAIT->rowCount();
        ui->TABLE_WAIT->setRowCount(row + 1);
        ui->TABLE_WAIT->setItem(row,0,new QTableWidgetItem(ui->tableWidget->item(i,0)->text()));
        ui->TABLE_WAIT->setItem(row,1,new QTableWidgetItem(ui->tableWidget->item(i,1)->text()));
        ui->TABLE_WAIT->setItem(row,2,new QTableWidgetItem(QDateTime::currentDateTime().toString("hh:mm:ss")));
        ui->TABLE_WAIT->setItem(row,3,new QTableWidgetItem(QString::number(0)));


        PCB pcb;
        pcb.arrive_time = QDateTime::currentDateTime();
        pcb.job_name = ui->tableWidget->item(i,0)->text();
        pcb.need_time = ui->tableWidget->item(i,1)->text().toInt();
        pcb.exec_time = 0;
        pcb.wait_time = 0;
        if (m_Status == STATUS_LEVEL)
        {
            pcb.level = ui->tableWidget->item(i,2)->text().toInt();
            ui->TABLE_WAIT->setItem(row,4,new QTableWidgetItem(ui->tableWidget->item(i,2)->text()));
        }
        if (m_Status == STATUS_HRRN)
        {
            ui->TABLE_WAIT->setItem(row,4,new QTableWidgetItem(QString::number(pcb.wait_time)));
        }
        else
            pcb.level = 32767;
        m_pScheAlgo->AddNewJob(pcb);   
    }

}

void MainWindow::on_pushButton_clicked()
{
    //qDebug() << ui->tableWidget->rowCount();
    ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);
    //ui->tableWidget->removeRow(0);
}

void MainWindow::on_pushButton_2_clicked()
{
    int rowIndex = ui->tableWidget->currentRow();
    if (rowIndex != -1)
        ui->tableWidget->removeRow(rowIndex);
}


