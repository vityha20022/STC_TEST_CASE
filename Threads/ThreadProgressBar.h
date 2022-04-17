#ifndef THREADPROGRESSBAR_H
#define THREADPROGRESSBAR_H

#include <QProgressBar>
#include <QWidget>

class ThreadProgressBar : public QProgressBar
{
    Q_OBJECT
public:
    ThreadProgressBar(QWidget *parent = 0);

private slots:
    void processFinished();
};

#endif // THREADPROGRESSBAR_H
