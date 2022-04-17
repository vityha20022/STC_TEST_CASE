#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include <QRandomGenerator>

class Thread : public QThread
{
    Q_OBJECT
public:
    Thread(QObject *parent = 0);

    void run() override;

signals:
    void intermediateResult(int);
    void processFinished();
};

#endif // THREAD_H
