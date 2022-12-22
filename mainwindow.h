#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QLabel"
#include "QPoint"
#include "workerthread.h"
#include "QThread"
#include "QButtonGroup"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

enum Direction
{
    DIR_INIT,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
};

typedef struct SnakeBody_
{
    QLabel *body;
}SnakeBody;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_start_clicked();

    void on_pushButton_pause_clicked();

    void on_MainWindow_iconSizeChanged(const QSize &iconSize);

    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;

    void on_radioButton_midle_clicked();

    void on_radioButton_easy_clicked();

    void on_radioButton_hard_clicked();

    void keyPressEvent(QKeyEvent *event) ;

    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

    bool verifyDead();
    bool canEat();
    bool isFoodInSnakeBody(const SnakeBody& food);
    SnakeBody getFood();
    void update_score();
    void repaintSnake();

    void on_pushButton_clear_clicked();

private:
    Ui::MainWindow *ui;
    QButtonGroup *groupButton;
    QPalette pal_clr;

    WorkerThread *workthread;

    QList <SnakeBody> snake;
    SnakeBody food;
    Direction direction;
    int speed;
    bool isStoped;
    bool isDead;
    int score;
    int time;

public slots:
    void slotSnakeMove();
    void slotResultQueued();
    void slotResultDirect();

private:
    // Remembers the point in time when mouse button went down
    quint64 mLastPressTime = 0;
    // Pressing and holding for one full second constitutes a "longpress", set whatever value in milliseconds you want here.
    static const quint64 LONG_PRESS_THRESHOLD = 500;
protected:
    // Declare that we are overriding QWidget's events for key press and release
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
signals:
    // Our custom signal to emit once a longpress is detected.
    void longPressEvent(QKeyEvent *event);
public slots:
    void slots_speed();
signals:
    // Our custom signal to emit once a longpress is detected.
    void sigSpeed(int speed, int time);
};

#endif // MAINWINDOW_H
