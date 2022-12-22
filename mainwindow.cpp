#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include "QPoint"
#include "QMessageBox"
#include "QKeyEvent"
#include "QColorDialog"
#include "QTime"
#include "QRectF"
#include "QRect"
#include "QCoreApplication"
#include "QDebug"
#include "QThread"
#include "workerthread.h"
#include "QThread"
#include "exception"
#include "QPalette"
#include "QBrush"
#include "QPixmap"
#include "QPalette"
#include "QImage"
#include "QThread"
#include "QVBoxLayout"

const int body_size = 15;
const int area = 400;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QSize size(400, 400);
    this->resize(size);

    QString strDirPath = QCoreApplication::applicationDirPath();
    qDebug() << strDirPath;
    //QImage image = QImage(strDirPath + "/yard.jpg").scaled(this->size());
    QImage image = QImage(strDirPath + "/yard.jpg");
    QPalette pal = this->palette();
    pal.setBrush(QPalette::Background, QBrush(image));
    this->setPalette(pal);

    QColor clr = QColorDialog::getColor(Qt::red, this, tr("color dialog"));
    pal_clr = ui->label_2->palette();
    pal_clr.setColor( QPalette::Background, clr);

    setFocusPolicy(Qt::StrongFocus);
    installEventFilter(this);

    QPoint p;
    QList < QPoint> list;
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime())); //不能放在循环里，否则容易出现很多相同随机数
    for(int var =0; var < 1;++var)
    {
        p.setX(qrand() % area);
        p.setY(qrand() % area);
        list.append(p);
    }

    SnakeBody head;
    p.setX(100 % area / body_size * body_size);
    p.setY(100 % area / body_size * body_size);

    head.body = new QLabel(this);
    head.body ->setText("");
    QRect rec = QRect(p.rx(), p.ry() , body_size, body_size);
    head.body->setGeometry(rec);
    head.body ->setOpenExternalLinks(true);
    head.body->setAutoFillBackground(true);
    head.body->setPalette(pal_clr);
    snake.push_back(head);

    direction = DIR_INIT;

    ui->pushButton_start->setCheckable(true);
    ui->pushButton_pause->setCheckable(true);

    ui->radioButton_easy->setFocusPolicy(Qt::NoFocus);
    ui->radioButton_midle->setFocusPolicy(Qt::NoFocus);
    ui->radioButton_hard->setFocusPolicy(Qt::NoFocus);

    groupButton = new QButtonGroup(this);
    groupButton->addButton(ui->radioButton_easy, 0);
    groupButton->addButton(ui->radioButton_midle, 1);
    groupButton->addButton(ui->radioButton_hard, 2);

    ui->radioButton_easy->setChecked(true); //默认选中radioButton_easy

    qDebug() << "main threadId: " << QThread::currentThreadId();
    workthread = new WorkerThread(this);
    //workthread = new WorkerThread();
    //匿名槽函数，槽函数运行在次线程
    connect(workthread, &WorkerThread::sigSnakeMove, [=]()
    {
        //qDebug()<<"lanba slot threadId: " << QThread::currentThreadId();
    });

    //普通连接方式，槽函数运行在主线程
    connect(workthread, &WorkerThread::sigSnakeMove, this, &MainWindow::slotSnakeMove);

    //队列连接
    connect(workthread, &WorkerThread::sigSnakeMove, this, &MainWindow::slotResultQueued, Qt::QueuedConnection);

    //直接连接
    connect(workthread, &WorkerThread::sigSnakeMove, this, &MainWindow::slotResultDirect, Qt::DirectConnection);

    connect(this, &MainWindow::sigSpeed, workthread, &WorkerThread::slotSpeed);

    workthread->start();
    food = getFood();
    isStoped = false;
    isDead = false;
    score = 0;
    update_score();
    //绑定信号和槽函数
    connect(ui->radioButton_easy, SIGNAL(clicked(bool)), this, SLOT(slots_speed()));
    connect(ui->radioButton_midle, SIGNAL(clicked(bool)), this, SLOT(slots_speed()));
    connect(ui->radioButton_hard, SIGNAL(clicked(bool)), this, SLOT(slots_speed()));
    speed = 1;
}

MainWindow::~MainWindow()
{
    workthread->quit();
    workthread->stop();
    workthread->deleteLater();
    workthread->wait();  //必须等待线程结束

    delete workthread;
    delete ui;
}

void MainWindow::on_pushButton_start_clicked()
{
    isStoped = false;
    if(direction == DIR_INIT)
    {
        direction = DIR_RIGHT;
    }
}

void MainWindow::update_score()
{
    QString score_str = QString::number(score, 'd', 0);
    ui->textEdit_score->setPlainText(score_str);
}

void MainWindow::on_pushButton_pause_clicked()
{
    isStoped = true;
}

void MainWindow::on_MainWindow_iconSizeChanged(const QSize &iconSize)
{
    QString strDirPath = QCoreApplication::applicationDirPath();
    qDebug() << strDirPath;
    //QImage image = QImage(strDirPath + "/yard.jpg").scaled(this->size());
    QImage image = QImage(strDirPath + "/yard.jpg");
    QPalette pal = this->palette();
    pal.setBrush(QPalette::Background, QBrush(image));
    this->setPalette(pal);
}

void MainWindow::changeEvent(QEvent *event)
{
    if(QEvent::WindowStateChange == event->type()){
        QWindowStateChangeEvent * stateEvent = dynamic_cast<QWindowStateChangeEvent*>(event);
        if(Q_NULLPTR != stateEvent){
            Qt::WindowStates x = stateEvent->oldState();
            qDebug() << "123123" << stateEvent->oldState() << x;
            if(Qt::WindowMinimized == stateEvent->oldState())
            {
                qDebug() << "minmin";
                QString strDirPath = QCoreApplication::applicationDirPath();
                qDebug() << strDirPath;
                //QImage image = QImage(strDirPath + "/yard.jpg").scaled(this->size());
                QImage image = QImage(strDirPath + "/yard.jpg");
                QPalette pal = this->palette();
                pal.setBrush(QPalette::Background, QBrush(image));
                this->setPalette(pal);
            }
            else if(Qt::WindowMaximized == stateEvent->oldState())
            {
                qDebug() << "maxmax";
                QString strDirPath = QCoreApplication::applicationDirPath();
                qDebug() << strDirPath;
                //QImage image = QImage(strDirPath + "/yard.jpg").scaled(this->size());
                QImage image = QImage(strDirPath + "/yard.jpg");
                QPalette pal = this->palette();
                pal.setBrush(QPalette::Background, QBrush(image));
                this->setPalette(pal);
            }
        }
    }
}


void MainWindow::on_radioButton_easy_clicked()
{
    speed = 1;
    emit sigSpeed(speed, time);
}

void MainWindow::on_radioButton_midle_clicked()
{
    speed = 2;
    emit sigSpeed(speed, time);
}

void MainWindow::on_radioButton_hard_clicked()
{
    speed = 3;
    emit sigSpeed(speed, time);
}

bool MainWindow::isFoodInSnakeBody(const SnakeBody& food)
{
    bool isIn = false;
    SnakeBody *cur = NULL;
    QMutableListIterator<SnakeBody> snake_it(snake);

    for(snake_it.toFront(); snake_it.hasNext();)
    {
        cur = &snake_it.next();
        if(cur->body->pos().x() == food.body->pos().x() && cur->body->pos().y() == food.body->pos().y())
        {
            isIn = true;
        }
    }

    return isIn;
}

SnakeBody MainWindow::getFood()
{
    SnakeBody food;
    do
    {
        QPoint p;
        p.setX(qrand() % area / body_size * body_size);
        p.setY(qrand() % area / body_size * body_size);

        food.body = new QLabel(this);
        food.body ->setText("");

        QRect rec = QRect(p.rx(), p.ry() , body_size, body_size);
        food.body->setGeometry(rec);

        food.body ->setOpenExternalLinks(true);
        food.body->setAutoFillBackground(true);

        food.body->setPalette(pal_clr);

    }while(isFoodInSnakeBody(food));
    food.body->setVisible(true);
    food.body->repaint();
    return food;
}

bool MainWindow::verifyDead()
{
    SnakeBody *head = NULL;
    SnakeBody *cur = NULL;
    try
    {
        QMutableListIterator<SnakeBody> snake_it(snake);
        snake_it.toFront();
        if(snake_it.hasNext())
        {
            head = &snake_it.next();
            if(head->body->pos().x() < 0 || head->body->pos().y() < 0 || head->body->pos().x() > area || head->body->pos().y() > area)
            {
                isDead = true;
                return isDead;
            }

            for(; snake_it.hasNext();)
            {
                cur =&snake_it.next();
                if(head->body->pos().x() == cur->body->pos().x() && head->body->pos().y() == cur->body->pos().y())
                {
                    isDead = true;
                    break;
                }
            }
        }
    }
    catch(std::exception e)
    {
       printf("%s!", e.what());
       qDebug()<<"exception" << e.what();
    }

    return isDead;
}

bool MainWindow::canEat()
{
    bool canEat = false;
    SnakeBody *cur = NULL;
    QMutableListIterator<SnakeBody> snake_it(snake);
    snake_it.toFront();
    if(snake_it.hasNext())
    {
        cur = &snake_it.next();
        switch (direction)
        {
        case DIR_UP:
            if(cur->body->pos().y() - 15 == food.body->pos().y() && cur->body->pos().x() == food.body->pos().x())
            {
                canEat = true;
            }
            break;
        case DIR_DOWN:
            if(cur->body->pos().y() + 15 == food.body->pos().y() && cur->body->pos().x() == food.body->pos().x())
            {
                canEat = true;
            }
            break;
        case DIR_LEFT:
            if(cur->body->pos().x() - 15 == food.body->pos().x() && cur->body->pos().y() == food.body->pos().y())
            {
                canEat = true;
            }
            break;
        case DIR_RIGHT:
            if(cur->body->pos().x() + 15 == food.body->pos().x() && cur->body->pos().y() == food.body->pos().y())
            {
                canEat = true;
            }
            break;
        default:
            break;
        }

    }
    return canEat;
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if(isStoped || isDead || snake.isEmpty())
    {
        return;
    }

    //Remeber last time key was pressed
    mLastPressTime = QDateTime::currentMSecsSinceEpoch();

    QMutableListIterator<SnakeBody> snake_it(snake);
    switch (event->key())
    {
    case Qt::Key_Up:
        if(direction != DIR_UP && direction != DIR_DOWN)
        {
            direction = DIR_UP;
            qDebug() << "press UP";
        }
        break;
    case Qt::Key_Down:
        if(direction != DIR_UP && direction != DIR_DOWN)
        {
            direction = DIR_DOWN;
            qDebug() << "press DOWN";
        }
        break;
    case Qt::Key_Left:
        if(direction != DIR_LEFT && direction != DIR_RIGHT)
        {
            direction = DIR_LEFT;
            qDebug() << "press LEFT";
        }
        break;
    case Qt::Key_Right:
        if(direction != DIR_LEFT && direction != DIR_RIGHT)
        {
            direction = DIR_RIGHT;
            qDebug() << "press RIGHT";
        }
        break;
    default:
        break;
    }

    if(canEat())
    {
        snake.push_back(food);
        food = getFood();
        score++;
        update_score();
        repaintSnake();
    }

    this->repaint();
    QCoreApplication::processEvents();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    disconnect(workthread, &WorkerThread::sigSnakeMove, this, &MainWindow::slotSnakeMove);
    disconnect(workthread, &WorkerThread::sigSnakeMove, this, &MainWindow::slotResultQueued);
    disconnect(workthread, &WorkerThread::sigSnakeMove, this, &MainWindow::slotResultDirect);
}

void MainWindow::repaintSnake()
{
    QMutableListIterator<SnakeBody> snake_it(snake);
    SnakeBody *cur = NULL;
    for(snake_it.toFront(); snake_it.hasNext();)
    {
        cur = &snake_it.next();
        cur->body->repaint();
    }
}

//普通连接方式的槽函数
void MainWindow::slotSnakeMove()
{
    if(isStoped || isDead || snake.isEmpty())
    {
        return;
    }
    QMessageBox::StandardButton result;
    qDebug() << "slotResult threadId: " << QThread::currentThreadId();

    if(verifyDead())
    {
        result = QMessageBox::information(parentWidget(), "通知", "is dead!");
    }
    else
    {
        QMutableListIterator<SnakeBody> snake_it(snake);
        SnakeBody *cur = NULL;
        SnakeBody first;
        SnakeBody last;
        switch (direction)
        {
        case DIR_UP:
            first = snake.at(0);
            last = snake.at(snake.count() - 1);
            if(first.body == last.body)
            {
                first.body->move(first.body->pos().x(), first.body->pos().y() - body_size);
            }
            else
            {
                last.body->move(first.body->pos().x(), first.body->pos().y() - body_size);
                snake.prepend(last);
                snake.removeLast();
            }
            break;
        case DIR_DOWN:
            first = snake.at(0);
            last = snake.at(snake.count() - 1);
            if(first.body == last.body)
            {
                first.body->move(first.body->pos().x() , first.body->pos().y() + body_size);
            }
            else
            {
                last.body->move(first.body->pos().x() , first.body->pos().y() + body_size);
                snake.prepend(last);
                snake.removeLast();
            }
            break;
        case DIR_LEFT:
            first = snake.at(0);
            last = snake.at(snake.count() - 1);
            if(first.body == last.body)
            {
                first.body->move(first.body->pos().x() - body_size, first.body->pos().y());
            }
            else
            {
                last.body->move(first.body->pos().x() - body_size, first.body->pos().y());
                snake.prepend(last);
                snake.removeLast();
            }
            break;
        case DIR_RIGHT:
            first = snake.at(0);
            last = snake.at(snake.count() - 1);
            if(first.body == last.body)
            {
                first.body->move(first.body->pos().x() + body_size, first.body->pos().y());
            }
            else
            {
                last.body->move(first.body->pos().x() + body_size, first.body->pos().y());
                snake.prepend(last);
                snake.removeLast();
            }
            break;
        default:
            break;
        }
        repaintSnake();
        if(canEat())
        {
            snake.push_back(food);
            food = getFood();
            score++;
            update_score();
            repaintSnake();
        }
    }
}

//队列连接方式的槽函数
void MainWindow::slotResultQueued()
{
    qDebug()<<"slotResultQueued threadId: "<<QThread::currentThreadId();
}

//直接连接方式的槽函数
void MainWindow::slotResultDirect()
{
    qDebug()<<"slotResultDirect threadId: "<<QThread::currentThreadId();
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    // Calculate for how long the button has been pressed upon release
    const quint64 pressTime = QDateTime::currentMSecsSinceEpoch() - mLastPressTime;
    // The press time exceeds our "threshold" and this constitutes a longpress

    switch (event->key())
    {
    case Qt::Key_Up:
        if(direction == DIR_UP && pressTime > LONG_PRESS_THRESHOLD)
        {
            // We pass the original mouse event in case it is useful (it contains all sorts of goodies like mouse posittion, which button was pressed etc).
            emit longPressEvent(event);
            qDebug() << "release UP";
        }
        break;
    case Qt::Key_Down:
        if(direction == DIR_DOWN && pressTime > LONG_PRESS_THRESHOLD)
        {
            // We pass the original mouse event in case it is useful (it contains all sorts of goodies like mouse posittion, which button was pressed etc).
            emit longPressEvent(event);
            qDebug() << "release DOWN";
        }
        break;
    case Qt::Key_Left:
        if(direction == DIR_LEFT && pressTime > LONG_PRESS_THRESHOLD)
        {
            // We pass the original mouse event in case it is useful (it contains all sorts of goodies like mouse posittion, which button was pressed etc).
            emit longPressEvent(event);
            qDebug() << "release LEFT";
        }
        break;
    case Qt::Key_Right:
        if(direction == DIR_RIGHT && pressTime > LONG_PRESS_THRESHOLD)
        {
            // We pass the original mouse event in case it is useful (it contains all sorts of goodies like mouse posittion, which button was pressed etc).
            emit longPressEvent(event);
            qDebug() << "release RIGHT";
        }
        break;
    default:
        break;
    }
}

void MainWindow::slots_speed()
{
    switch(groupButton->checkedId())
    {
    case 0:
        speed = 1;
        qDebug() <<"easy speed" << endl;
        break;
    case 1:
        speed = 2;
        qDebug() << "midle speed" << endl;
        break;
    case 2:
        speed = 3;
        qDebug() <<"hard speed" << endl;
        break;
    }
    emit sigSpeed(speed, time);
}

void MainWindow::on_pushButton_clear_clicked()
{
    food = getFood();
    isStoped = false;
    isDead = false;
    score = 0;
    update_score();
    isStoped = false;
    direction = DIR_INIT;
}