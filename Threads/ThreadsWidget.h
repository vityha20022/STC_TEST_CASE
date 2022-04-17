#ifndef THREADSWIDGET_H
#define THREADSWIDGET_H

#include "Thread.h"
#include "ThreadProgressBar.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QApplication>

class ThreadsWidget : public QWidget
{
    Q_OBJECT
public:
    ThreadsWidget(QWidget *parent = 0);

private:
    void reset();

private slots:
    void processFinished();
    void startCalculate();

private:
    QList<Thread *> threads;
    QList<ThreadProgressBar *> progressBars;
    int finishedThreads;
    int numberOfThreads;

    QPushButton *startButton;
};

#endif // THREADSWIDGET_H
