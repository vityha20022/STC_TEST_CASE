#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include <QDataStream>
#include <QTcpSocket>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QHostAddress>


class QTextEdit;
class QLineEdit;

class TcpClient : public QWidget
{
    Q_OBJECT
public:
    TcpClient(const QHostAddress& host, int numberOfPort, QWidget* parent = 0);

public slots:
    void disconnect();

private slots:
    void readyRead();
    void error(QAbstractSocket::SocketError socketError);
    void messageSent();
    void sendToServer(QString message);
    void connected();

private:
    QTcpSocket *tcpSocket;
    QTextEdit *messagesField;
    QLineEdit *inputMessageLine;
    quint16 nextBlockSize;
};

#endif // TCPCLIENT_H
