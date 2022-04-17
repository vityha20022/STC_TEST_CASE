#include "TcpClient.h"

TcpClient::TcpClient(const QHostAddress &host, int numberOfPort, QWidget *parent) :
    QWidget(parent),
    nextBlockSize(0)
{
    setWindowTitle("TcpClient");

    tcpSocket = new QTcpSocket(this);

    tcpSocket->connectToHost(host, numberOfPort);
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(connected()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));

    messagesField  = new QTextEdit;
    inputMessageLine = new QLineEdit;

    connect(inputMessageLine, SIGNAL(returnPressed()), this, SLOT(messageSent()));
    messagesField->setReadOnly(true);

    QPushButton *sendButton = new QPushButton(tr("Send"));
    sendButton->setStyleSheet("background-color: green;");
    connect(sendButton, SIGNAL(clicked()), SLOT(messageSent()));

    QPushButton *refuseButton = new QPushButton(tr("Refuse connection"));
    refuseButton->setStyleSheet("background-color: red;");
    connect(refuseButton, SIGNAL(clicked()), this, SLOT(disconnect()));

    //Layout setup
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(messagesField);
    layout->addWidget(inputMessageLine);
    layout->addWidget(sendButton);
    layout->addWidget(refuseButton);
    setLayout(layout);
}

void TcpClient::readyRead()
{
    QDataStream in(tcpSocket);
    while (true)
    {
        if (!nextBlockSize)
        {
            if (tcpSocket->bytesAvailable() < sizeof(quint16))
                break;
            in >> nextBlockSize;
        }

        if (tcpSocket->bytesAvailable() < nextBlockSize)
            break;

        QString message;
        in >> message;

        messagesField->append(message);
        nextBlockSize = 0;
    }
}

void TcpClient::error(QAbstractSocket::SocketError socketError)
{
    QString strError = tr("Error: ") +
                       (socketError == QAbstractSocket::HostNotFoundError ?
                       tr("The host was not found.") :
                       socketError == QAbstractSocket::RemoteHostClosedError ?
                       tr("The remote host is closed.") :
                       socketError == QAbstractSocket::ConnectionRefusedError ?
                       tr("The connection was refused.") :
                       QString(tcpSocket->errorString()));

    messagesField->append(strError);
}

void TcpClient::messageSent()
{
    QString message = inputMessageLine->text();
    sendToServer(message);
    inputMessageLine->setText("");
}

void TcpClient::sendToServer(QString message)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out << quint16(0) << message;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
}

void TcpClient::connected()
{
     sendToServer(tr("Successful!"));
     QString ipStr = QHostAddress(tcpSocket->localAddress().toIPv4Address()).toString();
     QString portStr = QString("%0").arg(tcpSocket->localPort());
     messagesField->append(tr("My address: ") + ipStr + "\n");
     messagesField->append(tr("My port: ") + portStr + "\n");
}

void TcpClient::disconnect()
{
    tcpSocket->disconnectFromHost();
    this->hide();
}
