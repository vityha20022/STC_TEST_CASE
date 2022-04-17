#include "mainwindow.h"
#include "TcpServer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    int port = 5000;
    TcpServer *tcpServer = new TcpServer(port);
    setCentralWidget(tcpServer);
    setMinimumSize(QSize(800, 600));
}

MainWindow::~MainWindow()
{
}

