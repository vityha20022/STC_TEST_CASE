#include "ThreadProgressBar.h"


ThreadProgressBar::ThreadProgressBar(QWidget *parent) : QProgressBar(parent)
{
}

void ThreadProgressBar::processFinished()
{
    setFormat(tr("Process finished!"));
}
