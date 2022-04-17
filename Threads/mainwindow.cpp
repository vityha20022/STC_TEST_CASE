#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ThreadsWidget *threadsWidget = new ThreadsWidget;
    setCentralWidget(threadsWidget);
    setMinimumSize(QSize(800, 600));
    setWindowIcon(QIcon(":/progressBar.png"));
}

MainWindow::~MainWindow()
{
}

