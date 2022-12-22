#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H
#include "QThread"

class WorkerThread : public QThread
{
    Q_OBJECT
public:
    WorkerThread(QObject *parent = nullptr);

protected:
    void run();
public:
    void stop();

private:
    int speed;
    volatile bool stopped;

public:
signals:
    void sigSnakeMove();
public slots:
    void slotSpeed(int speed, int time);
};

#endif // WORKERTHREAD_H
