#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QTimerEvent>
#include <QLineEdit>
#include <QVector>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    timer = new QTimer(this); // Inicializando o timer
    plotter = ui->widget; // Inicializando a classe Plotter

    // Conexão do timer ao slot getData
    connect(timer, &QTimer::timeout, this, &MainWindow::getData);

    connect(ui->pushButtonGet,
            SIGNAL(clicked(bool)),
            this,
            SLOT(getData()));

    // Conexão do botão connect com o slot responsável por conectar-se ao servidor
    connect(ui->pushButtonConnect,
            SIGNAL(clicked(bool)),
            this,
            SLOT(tcpConnect()));

    // Conexão do botão disconnect com o slot responsável por disconectar-se do servidor
    connect(ui->pushButtonDisconnect,
            SIGNAL(clicked(bool)),
            this,
            SLOT(tcpDisconnect()));

    // Conexão do botão start com o slot responsável por iniciar o envio de dados
    connect(ui->pushButtonStart,
            SIGNAL(clicked(bool)),
            this,
            SLOT(startSending()));

    // Conexão do botão stop com o slot responsável por parar o envio de dados
    connect(ui->pushButtonStop,
            SIGNAL(clicked(bool)),
            this,
            SLOT(stopSending()));


    // Conexão do botão update
    connect(ui->pushButtonUpdate,
            SIGNAL(clicked(bool)),
            this,
            SLOT(update()));

    // Conexão do slider de intervalo com o slot responsável por tratar o timer de envio de dados
    connect(ui->horizontalSliderTiming,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(setTimerInterval(int)));




}

void MainWindow::tcpConnect() {
    QString ipservidor = ui->lineEditIp->text();
    socket->connectToHost(ipservidor, 1234);
    if (socket->waitForConnected(3000)) {
        qDebug() << "Connected";
    } else {
        qDebug() << "Disconnected";
    }
}

void MainWindow::tcpDisconnect() {
    socket->disconnectFromHost();
    timer->stop();
}

void MainWindow::setTimerInterval(int interval) {
    timer->setInterval(interval);
}

void MainWindow::startSending() {
    if (!ipselect.isEmpty()) {
        timer->start();
    }
}

void MainWindow::stopSending() {
    timer->stop();
}

void MainWindow::update() {
    QStringList iplist;
    if (socket->state() == QAbstractSocket::ConnectedState) {
        if (socket->isOpen()) {
            qDebug() << "reading...";
            socket->write("list\r\n");
            socket->waitForBytesWritten();
            socket->waitForReadyRead();
            ui->listWidget->clear();
            iplist.clear();
            while (socket->bytesAvailable()) {
                iplist << socket->readLine().replace("\n", "").replace("\r", "");
                qDebug() << "Connected IP: " << iplist;
                ui->listWidget->addItems(iplist);
            }
        }
    } else {
        qDebug() << "Você não está conectado a nenhum servidor";
    }
}

void MainWindow::getIp(QListWidgetItem *_ipselect) {
    ipselect = _ipselect->text();
    ipselect = "get " + ipselect + " 30\r\n";
    qDebug() << "Ip selecionado: " << ipselect;
}

void MainWindow::getData() {
    QStringList list;
    qint64 thetime;
    QString str;
    qDebug() << "to get data...";
    if (socket->state() == QAbstractSocket::ConnectedState) {
        if (socket->isOpen()) {
            qDebug() << "reading...";
            socket->write(ipselect.toStdString().c_str());
            socket->waitForBytesWritten();
            socket->waitForReadyRead();
            qDebug() << socket->bytesAvailable();
            while (socket->bytesAvailable()) {
                str = socket->readLine().replace("\n", "").replace("\r", "");
                list = str.split(" ");
                if (list.size() == 2) {
                    bool ok;
                    str = list.at(0);
                    thetime = str.toLongLong(&ok);
                    str = list.at(1);
                    timeVector.append(thetime);
                    float floatValue = str.toFloat(&ok); // convertendo str para float
                    floatVector.append(floatValue); //passa os valores convertidos para o vetor

                    //qDebug() << thetime << ": " << str;

                    // Passa os dados para o Plotter
                    //plotter->setData(str, thetime);
                }
            }
        }
    }

    //imprimindo os vetores timeVector e strVector
    /*for(int i = 0; i < timeVector.size(); i++){
        qDebug() << timeVector.at(i) << ": " << floatVector.at(i); //como imprimir

    } */
    // pass the floatVector and timeVector to the plotter
    plotter->update(); // update the plotter widget
    plotter->setFloatVector(floatVector);
    plotter->setTimeVector(timeVector);
    //esvaziar antes de entrar no wille dnv
    timeVector.clear();
    floatVector.clear();
}

MainWindow::~MainWindow() {
    delete socket;
    delete ui;
}
