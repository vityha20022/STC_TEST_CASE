#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "TcpClient.h"

#include <QWidget>
#include <QTcpServer>
#include <QMessageBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QTcpSocket>
#include <QTime>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QSpacerItem>
#include <QIcon>
#include <QHideEvent>

class TcpServer : public QWidget
{
    Q_OBJECT
public:
    TcpServer(int numberOfPort, QWidget *parent = 0);

private slots:
    virtual void processNewConnection();
    void readMessage();

    void addListenerButtonPressed();
    void addListenerButtonReleased();
    void openClientClicked();
    void refuseClientClicked();
    void addNewListener();
    void clientDisconnected();
    void hideEvent(QHideEvent *event);

private:
    QTcpServer  *tcpServer;
    QTextEdit *messagesField;
    QTableWidget *table;
    QHash<int, QTcpSocket *> activeSockets;
    QHash<int, TcpClient *> activeClients;
    quint16 nextBlockSize;

    QPushButton *addListenerButton;

private:
    void sendToClient(QTcpSocket *socket, const QString& message);
};

#endif // TCPSERVER_H
