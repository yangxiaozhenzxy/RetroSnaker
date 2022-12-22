#include "workerthread.h"
#include "QDebug"

WorkerThread::WorkerThread(QObject *parent)
    :QThread(parent)
{
    speed = 1;
    stopped = false;
}

void WorkerThread::run()
{
    while(!stopped)
    {
        for(int i = 0; i < 4 - speed; i++)
        {
            usleep(100000);
        }
        emit sigSnakeMove();
    }
}

void WorkerThread::slotSpeed(int speed, int time)
{
    this->speed = speed;
}

void WorkerThread::stop()
{
    stopped = true;
}


