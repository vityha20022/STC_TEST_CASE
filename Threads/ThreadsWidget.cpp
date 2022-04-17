#include "ThreadsWidget.h"


ThreadsWidget::ThreadsWidget(QWidget *parent) : QWidget(parent), numberOfThreads(10)
{

    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    QTableWidget *table = new QTableWidget;
    table->setEditTriggers(QAbstractItemView::EditTriggers(0));
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    table->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    QStringList headers;
    headers << tr("Progress");
    table->setColumnCount(1);
    table->setHorizontalHeaderLabels(headers);
    layout->addWidget(table);

    for (int i = 0; i < numberOfThreads; i++)
    {
        ThreadProgressBar *progressBar = new ThreadProgressBar;
        progressBar->setRange(0, 100);
        progressBar->setValue(0);
        progressBar->setTextVisible(true);
        table->insertRow(table->rowCount());
        table->setCellWidget(i, 0, progressBar);
        progressBars.append(progressBar);

        Thread *thread = new Thread;
        connect(thread, SIGNAL(intermediateResult(int)), progressBar, SLOT(setValue(int)));
        connect(thread, SIGNAL(processFinished()), progressBar, SLOT(processFinished()));
        connect(thread, SIGNAL(processFinished()), this, SLOT(processFinished()));
        threads.append(thread);
    }

    startButton = new QPushButton(tr("Start"));
    startButton->setMaximumWidth(100);
    startButton->setStyleSheet("background-color: green;");
    connect(startButton, SIGNAL(clicked()), this, SLOT(startCalculate()));

    layout->addWidget(startButton);
    layout->setAlignment(startButton, Qt::AlignRight);
}

void ThreadsWidget::reset()
{
    startButton->setEnabled(true);
    finishedThreads = 0;
}

void ThreadsWidget::processFinished()
{
    qApp->processEvents();
    Thread *thread = qobject_cast<Thread *> (sender());
    thread->quit();
    finishedThreads++;

    if (finishedThreads == numberOfThreads)
        reset();
}

void ThreadsWidget::startCalculate()
{
    startButton->setEnabled(false);

    foreach (auto progressBar, progressBars)
    {
        progressBar->resetFormat();
        progressBar->setValue(0);
    }

    foreach (auto thread, threads)
        thread->start();
}
