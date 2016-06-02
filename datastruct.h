#ifndef DATASTRUCT_H
#define DATASTRUCT_H
#include <QString>
#include <QDateTime>
#include <list>

typedef struct
{
    QString job_name;
    int need_time;
    int exec_time;
    int wait_time;
    int level;
    QDateTime arrive_time;

    QDateTime finish_time;
    double arround_time;
    double weight_arround_time;

}PCB,JCB;

//typedef struct
//{
//    PCB pcb;
//    QDateTime finish_time;
//    double arround_time;
//    double weight_arround_time;
//}JCB;


//class Jobs
//{
//public:
//    std::list<PCB> m_PCBList_Wait;
//    std::list<PCB> m_PCBList_Run;
//    std::list<PCB> m_PCBList_Finish;
//};

#endif // DATASTRUCT_H
