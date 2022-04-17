#include "TcpServer.h"

TcpServer::TcpServer(int numberOfPort, QWidget *parent) :
    QWidget(parent),
    nextBlockSize(0)
{
    Q_INIT_RESOURCE(resources);

    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen(QHostAddress::Any, numberOfPort))
    {
        QMessageBox::critical(nullptr, tr("Server Error"), tr("Unable to start the server:") + tcpServer->errorString());
        tcpServer->close();
    }

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(processNewConnection()));

    messagesField = new QTextEdit;
    messagesField->setReadOnly(true);

    QVBoxLayout *layout = new QVBoxLayout;
    QString ipStr = QHostAddress(tcpServer->serverAddress().toIPv4Address()).toString();
    QString portStr = QString("%0").arg(tcpServer->serverPort());
    QLabel *ipLabel = new QLabel(tr("Server IP address: ") + ipStr);
    ipLabel->setAlignment(Qt::AlignRight);
    QLabel *portLabel = new QLabel(tr("Server port: ") + portStr);
    portLabel->setAlignment(Qt::AlignRight);
    layout->addWidget(ipLabel);
    layout->addWidget(portLabel);

    table = new QTableWidget;
    table->setEditTriggers(QAbstractItemView::EditTriggers(0));
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    QStringList headers;
    headers << tr("IP address") << tr("Port") << tr("Last message") << "" << "";
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels(headers);
    layout->addWidget(table);

    addListenerButton = new QPushButton();
    addListenerButton->setIcon(QIcon(":/plus.png"));
    addListenerButton->setIconSize(QSize(60, 60));
    addListenerButton->setFixedSize(60, 60);
    QRect rect(0, 0, 60, 60);
    QRegion region(rect, QRegion::Ellipse);
    addListenerButton->setMask(region);
    addListenerButton->setToolTip(tr("Add listener"));
    connect(addListenerButton, SIGNAL(pressed()), this, SLOT(addListenerButtonPressed()));
    connect(addListenerButton, SIGNAL(released()), this, SLOT(addListenerButtonReleased()));
    connect(addListenerButton, SIGNAL(clicked()), this, SLOT(addNewListener()));

    layout->addWidget(addListenerButton);
    layout->setAlignment(addListenerButton, Qt::AlignRight);

    setLayout(layout);
}

TcpServer::~TcpServer()
{
    foreach (auto client, activeClients)
    {
        client->hide();
    }
}

void TcpServer::processNewConnection()
{
    QTcpSocket* clientSocket = tcpServer->nextPendingConnection();
    connect(clientSocket, SIGNAL(disconnected()), clientSocket, SLOT(deleteLater()));
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(readMessage()));

    int currentRow = table->rowCount();
    activeSockets[currentRow] = clientSocket;
    table->insertRow(currentRow);
    QString ipStr = QHostAddress(clientSocket->peerAddress().toIPv4Address()).toString();
    QString portStr = QString("%0").arg(clientSocket->peerPort());
    QString lastMessage = "";
    QStringList rowContents;
    rowContents << ipStr << portStr << lastMessage;

    for(int i = 0; i < rowContents.size(); i++)
    {
        QTextEdit *tableWidget = new QTextEdit();
        tableWidget->setReadOnly(true);
        tableWidget->setText(rowContents[i]);
        table->setCellWidget(currentRow, i, tableWidget);
    }

    QPushButton *openClientButton = new QPushButton(tr("Open client"));
    openClientButton->setStyleSheet("background-color: green;");
    table->setCellWidget(currentRow, rowContents.size(), openClientButton);
    connect(openClientButton, SIGNAL(clicked()), this, SLOT(openClientClicked()));

    QPushButton *refuseClientButton = new QPushButton(tr("Refuse client"));
    refuseClientButton->setStyleSheet("background-color: red;");
    table->setCellWidget(currentRow, rowContents.size() + 1, refuseClientButton);
    connect(refuseClientButton, SIGNAL(clicked()), this, SLOT(refuseClientClicked()));
}

void TcpServer::readMessage()
{
    QTcpSocket* clientSocket = (QTcpSocket*) sender();
    QDataStream in(clientSocket);
    while (true)
    {
        if (!nextBlockSize)
        {
            if (clientSocket->bytesAvailable() < sizeof(quint16))
                break;

            in >> nextBlockSize;
        }

        if (clientSocket->bytesAvailable() < nextBlockSize)
            break;

        QString message;
        in >> message;

        int indexOfRow = activeSockets.key(clientSocket);
        QTextEdit *currentTableWidget = qobject_cast<QTextEdit *>(table->cellWidget(indexOfRow, 2));
        currentTableWidget->setText(message);

        nextBlockSize = 0;

        sendToClient(clientSocket, tr("Server Response: Received \"") + message + "\"");
    }
}

void TcpServer::addListenerButtonPressed()
{
    addListenerButton->setIcon(QIcon(":/pressedPlus.png"));
}

void TcpServer::addListenerButtonReleased()
{
    addListenerButton->setIcon(QIcon(":/plus.png"));
}

void TcpServer::openClientClicked()
{
    QPushButton* openClientButton = qobject_cast<QPushButton *> (sender());
    int indexOfClient = table->indexAt(openClientButton->pos()).row();
    TcpClient *client = activeClients[indexOfClient];
    client->show();
}

void TcpServer::refuseClientClicked()
{
    QPushButton* openClientButton = qobject_cast<QPushButton *> (sender());
    int indexOfClient = table->indexAt(openClientButton->pos()).row();
    TcpClient *client = activeClients[indexOfClient];
    client->disconnect();
}

void TcpServer::addNewListener()
{
    TcpClient *newClient = new TcpClient(tcpServer->serverAddress(), tcpServer->serverPort());
    int currentRow = table->rowCount();
    activeClients[currentRow] = newClient;
    newClient->show();
}

void TcpServer::hideEvent(QHideEvent *event)
{
    foreach (auto client, activeClients)
        client->hide();

    QWidget::hideEvent(event);
}

void TcpServer::clientDisconnected()
{
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket *> (sender());
    int indexOfClient = activeSockets.key(clientSocket);
    for (int i = indexOfClient; i < table->rowCount() - 1; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            QTextEdit *currentWidget = qobject_cast<QTextEdit *> (table->cellWidget(i, j));
            QTextEdit *widgetInNextRow = qobject_cast<QTextEdit *> (table->cellWidget(i + 1, j));
            currentWidget->setText(widgetInNextRow->toPlainText());
        }
        activeClients[i] = activeClients[i + 1];
        activeSockets[i] = activeSockets[i + 1];
    }

    activeClients.remove(table->rowCount() - 1);
    activeSockets.remove(table->rowCount() - 1);
    table->removeRow(table->rowCount() - 1);
}



void TcpServer::sendToClient(QTcpSocket *socket, const QString &message)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out << quint16(0) << message;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    socket->write(arrBlock);
}
