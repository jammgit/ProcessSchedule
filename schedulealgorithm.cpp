#include "schedulealgorithm.h"
#include "mainwindow.h"


ScheduleAlgorithm::ScheduleAlgorithm(){
    m_Mutex = PTHREAD_MUTEX_INITIALIZER;
    m_State = SCHE_NULL;
}

ScheduleAlgorithm::~ScheduleAlgorithm(){
    pthread_mutex_destroy(&m_Mutex);
}

void ScheduleAlgorithm::AddNewJob(const PCB &pcb)
{
    pthread_mutex_lock(&m_Mutex);
    m_PCBList_Wait.push_back(pcb);
    pthread_mutex_unlock(&m_Mutex);
}

SCHE_STATE ScheduleAlgorithm::GetState()
{
    return m_State;
}

void ScheduleAlgorithm::AddWidget(MainWindow *widget)
{
    m_WidgetList.push_back(widget);
}

void ScheduleAlgorithm::DeleteWidget(MainWindow *widget)
{
    m_WidgetList.remove(widget);
}

void ScheduleAlgorithm::ClearJobs()
{
    pthread_mutex_lock(&m_Mutex);
    m_PCBList_Run.clear();
    m_PCBList_Wait.clear();
    m_PCBList_Finish.clear();
    m_State = SCHE_NULL;
    pthread_mutex_unlock(&m_Mutex);
}

void ScheduleAlgorithm::Exec()
{
    pthread_mutex_lock(&m_Mutex);
    qDebug() << m_PCBList_Run.size() << '\n'
             << m_PCBList_Wait.size() << '\n'
             << m_PCBList_Finish.size() << '\n';
    if (m_PCBList_Run.empty())
    {
        if (!m_PCBList_Wait.empty())
        {
            m_PCBList_Run.push_back(m_PCBList_Wait.front());
            m_PCBList_Wait.pop_front();
            m_Timer = m_PCBList_Run.front().exec_time;
            m_State = SCHE_NEW_JOB;
        }
        else
            m_State = SCHE_NO_JOB;
    }
    else
    {
        int needtime = m_PCBList_Run.front().need_time;
        m_Timer++;
        if (needtime == m_Timer)
        {/* new job */
            m_PCBList_Run.front().exec_time = m_Timer;
            m_PCBList_Finish.push_front(m_PCBList_Run.front());
            m_PCBList_Run.pop_front();

            if (!m_PCBList_Wait.empty())
            {
                m_PCBList_Run.push_back(m_PCBList_Wait.front());
                m_PCBList_Wait.pop_front();
                m_Timer = m_PCBList_Run.front().exec_time;
                m_State = SCHE_ANOTHER_JOB;
            }
            else
                m_State = SCHE_COMPLETE_JOB;
        }
        else
        {/* renew exec time */
            m_PCBList_Run.front().exec_time = m_Timer;
            m_State = SCHE_RENEW_TIME;
        }
    }
    pthread_mutex_unlock(&m_Mutex);
    /* Renew all widget */
    size_t size = m_WidgetList.size();
    auto iter = m_WidgetList.begin();
    size_t idx;
    for (idx = 0; idx < size; ++idx)
    {
        (*iter)->Update();
        ++iter;
    }
}
const std::list<PCB>* ScheduleAlgorithm::GetWaitList()
{
    return &m_PCBList_Wait;
}
const std::list<PCB>* ScheduleAlgorithm::GetRunList()
{
    return &m_PCBList_Run;
}
const std::list<PCB>* ScheduleAlgorithm::GetFinishList()
{
    return &m_PCBList_Finish;
}

///////////////////////////////////////////////////////////////////
///////////////////////----FCFS----///////////////////////////////


///////////////////////////////////////////////////////////////////
///////////////////////----LEVEL----///////////////////////////////
void LEVEL::AddNewJob(const PCB &pcb)
{
    pthread_mutex_lock(&m_Mutex);
    size_t size = m_PCBList_Wait.size();
    auto iter = m_PCBList_Wait.begin();
    size_t i;
    for (i = 0; i < size; ++i)
    {
        if (pcb.level < (*iter).level)
        {
            m_PCBList_Wait.insert(iter, pcb);
            break;
        }
        ++iter;
    }
    if (i == size)
        m_PCBList_Wait.push_back(pcb);
    pthread_mutex_unlock(&m_Mutex);
}
///////////////////////////////////////////////////////////////////
///////////////////////----RR----///////////////////////////////
void RR::Exec()
{
    pthread_mutex_lock(&m_Mutex);
    qDebug() << m_PCBList_Run.size() << '\n'
             << m_PCBList_Wait.size() << '\n'
             << m_PCBList_Finish.size() << '\n';
    if (m_PCBList_Run.empty())
    {
        if (!m_PCBList_Wait.empty())
        {
            m_PCBList_Run.push_back(m_PCBList_Wait.front());
            m_PCBList_Wait.pop_front();
            m_Timer = m_PCBList_Run.front().exec_time;
            m_State = SCHE_NEW_JOB;
        }
        else
            m_State = SCHE_NO_JOB;
        m_TimeCounter = 0;
    }
    else
    {
        int needtime = m_PCBList_Run.front().need_time;
        if (needtime == ++m_Timer)
        {/* new job */
            m_PCBList_Run.front().exec_time = m_Timer;
            m_PCBList_Finish.push_front(m_PCBList_Run.front());
            m_PCBList_Run.pop_front();

            if (!m_PCBList_Wait.empty())
            {
                m_PCBList_Run.push_back(m_PCBList_Wait.front());
                m_PCBList_Wait.pop_front();
                m_Timer = m_PCBList_Run.front().exec_time;
                m_State = SCHE_ANOTHER_JOB;
            }
            else
                m_State = SCHE_COMPLETE_JOB;
            m_TimeCounter = 0;
        }
        else
        {/* renew exec time */
            m_PCBList_Run.front().exec_time = m_Timer;
            m_TimeCounter++;
            if (m_TimeCounter == m_RRTime)
            {

                m_PCBList_Wait.push_back(m_PCBList_Run.front());
                m_PCBList_Run.pop_front();
                m_PCBList_Run.push_front(m_PCBList_Wait.front());
                m_PCBList_Wait.pop_front();
                m_Timer = m_PCBList_Run.front().exec_time;
                m_TimeCounter = 0;
                m_State = SCHE_RRTIME_ARRI;
            }
            else
            {

                m_State = SCHE_RENEW_TIME;
            }
        }
    }
    pthread_mutex_unlock(&m_Mutex);
    /* Renew all widget */
    size_t size = m_WidgetList.size();
    auto iter = m_WidgetList.begin();
    size_t idx;
    for (idx = 0; idx < size; ++idx)
    {
        (*iter)->Update();
        ++iter;
    }
}
///////////////////////////////////////////////////////////////////
///////////////////////----SJF----///////////////////////////////
void SJF::AddNewJob(const PCB &pcb)
{
    pthread_mutex_lock(&m_Mutex);
    size_t size = m_PCBList_Wait.size();
    size_t idx;
    auto iter = m_PCBList_Wait.begin();
    for (idx = 0; idx < size; ++idx)
    {
        if (iter->need_time > pcb.need_time)
        {
            m_PCBList_Wait.insert(iter, pcb);
            break;
        }
        ++iter;
    }
    if (idx == size)
    {
        m_PCBList_Wait.push_back(pcb);
    }
    pthread_mutex_unlock(&m_Mutex);
}
void SJF::Exec()
{
    pthread_mutex_lock(&m_Mutex);
    qDebug() << m_PCBList_Run.size() << '\n'
             << m_PCBList_Wait.size() << '\n'
             << m_PCBList_Finish.size() << '\n';
    if (m_PCBList_Run.empty())
    {
        if (!m_PCBList_Wait.empty())
        {
            m_PCBList_Run.push_back(m_PCBList_Wait.front());
            m_PCBList_Wait.pop_front();
            m_Timer = m_PCBList_Run.front().exec_time;
            m_State = SCHE_NEW_JOB;
        }
        else
            m_State = SCHE_NO_JOB;
    }
    else
    {
        int needtime = m_PCBList_Run.front().need_time;
        m_Timer++;
        if (needtime == m_Timer)
        {/* new job */
            m_PCBList_Run.front().finish_time = QDateTime::currentDateTime();
            m_PCBList_Run.front().arround_time = m_PCBList_Run.front().finish_time.toTime_t()
                    - m_PCBList_Run.front().arrive_time.toTime_t();
            m_PCBList_Run.front().weight_arround_time = m_PCBList_Run.front().arround_time / m_PCBList_Run.front().need_time;

            m_PCBList_Run.front().exec_time = m_Timer;
            m_PCBList_Finish.push_front(m_PCBList_Run.front());
            m_PCBList_Run.pop_front();

            if (!m_PCBList_Wait.empty())
            {
                m_PCBList_Run.push_back(m_PCBList_Wait.front());
                m_PCBList_Wait.pop_front();
                m_Timer = m_PCBList_Run.front().exec_time;
                m_State = SCHE_ANOTHER_JOB;
            }
            else
                m_State = SCHE_COMPLETE_JOB;
        }
        else
        {/* renew exec time */
            m_PCBList_Run.front().exec_time = m_Timer;
            m_State = SCHE_RENEW_TIME;
        }
    }
    pthread_mutex_unlock(&m_Mutex);
    /* Renew all widget */
    size_t size = m_WidgetList.size();
    auto iter = m_WidgetList.begin();
    size_t idx;
    for (idx = 0; idx < size; ++idx)
    {
        (*iter)->Update();
        ++iter;
    }
}

///////////////////////////////////////////////////////////////////
///////////////////////----HRRN----///////////////////////////////

void HRRN::Exec()
{
    /* Calculate the max of Waitlist */
    pthread_mutex_lock(&m_Mutex);

    qDebug() << m_PCBList_Run.size() << '\n'
             << m_PCBList_Wait.size() << '\n'
             << m_PCBList_Finish.size() << '\n';
    if (m_PCBList_Run.empty())
    {
        if (!m_PCBList_Wait.empty())
        {
            auto iter = getNextJobIter();
            m_PCBList_Run.push_back(*iter);
            m_PCBList_Wait.erase(iter);
            m_Timer = m_PCBList_Run.front().exec_time;
            m_State = SCHE_NEW_JOB;
        }
        else
            m_State = SCHE_NO_JOB;
    }
    else
    {
        int needtime = m_PCBList_Run.front().need_time;
        if (needtime == ++m_Timer)
        {/* new job */
            m_PCBList_Run.front().finish_time = QDateTime::currentDateTime();
            m_PCBList_Run.front().arround_time = m_PCBList_Run.front().finish_time.toTime_t()
                    - m_PCBList_Run.front().arrive_time.toTime_t();
            m_PCBList_Run.front().weight_arround_time = m_PCBList_Run.front().arround_time / m_PCBList_Run.front().need_time;

            m_PCBList_Run.front().exec_time = m_Timer;
            m_PCBList_Finish.push_front(m_PCBList_Run.front());
            m_PCBList_Run.pop_front();

            if (!m_PCBList_Wait.empty())
            {
                auto iter = getNextJobIter();
                m_PCBList_Run.push_front(*iter);
                m_PCBList_Wait.erase(iter);
                m_Timer = m_PCBList_Run.front().exec_time;
                m_State = SCHE_ANOTHER_JOB;
            }
            else
                m_State = SCHE_COMPLETE_JOB;
        }
        else
        {/* renew exec time */
            m_PCBList_Run.front().exec_time = m_Timer;
            m_State = SCHE_RENEW_TIME;
        }
    }
    /* renew wait list wait_time*/
    size_t size = m_PCBList_Wait.size();
    auto iter = m_PCBList_Wait.begin();
    size_t idx;
    for (idx = 0; idx < size; ++idx)
    {
        iter->wait_time++;
        iter++;
    }
    pthread_mutex_unlock(&m_Mutex);

    /* Renew all widget */
    size = m_WidgetList.size();
    auto iter1 = m_WidgetList.begin();
    for (idx = 0; idx < size; ++idx)
    {
        (*iter1)->Update();
        ++iter1;
    }
}

std::list<PCB>::iterator HRRN::getNextJobIter()
{

    auto iter = m_PCBList_Wait.begin();
    auto newjobiter = iter;
    double wait_priority;
    wait_priority = 1.0 * (iter->wait_time + iter->need_time) / iter->need_time;
    ++iter;
    double tmp;
    size_t size = m_PCBList_Wait.size();
    size_t idx;
    for (idx = 1; idx < size; ++idx)
    {
        tmp = 1.0 * (iter->wait_time + iter->need_time) / iter->need_time;
        if (tmp - wait_priority > 0.000001)
        {
            wait_priority = tmp;
            newjobiter = iter;
        }
        ++iter;
    }
    return newjobiter;
}
