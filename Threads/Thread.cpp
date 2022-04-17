#include "Thread.h"


Thread::Thread(QObject *parent) : QThread(parent)
{
}

void Thread::run()
{
    for (int i = 0; i < 100; i++)
    {
        int numberOfFirstCycle = QRandomGenerator::global()->bounded(10000, 100000);
        int numberOfSecondCycle = QRandomGenerator::global()->bounded(100, 1000);
        for (int j = 0; j < numberOfFirstCycle; j++)
        {
            for (int k = 0; k < numberOfSecondCycle; k++)
            {

            }
        }

        emit intermediateResult(i + 1);
    }

    emit processFinished();
}
