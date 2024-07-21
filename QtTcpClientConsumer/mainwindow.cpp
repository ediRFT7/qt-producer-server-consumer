#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QTimerEvent>
#include <QLineEdit>
#include <QVector>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    timer = new QTimer(this); // Inicializando o timer
    plotter = ui->widget; // Inicializando a classe Plotter

    // Conexão do timer ao slot getData
    connect(timer, &QTimer::timeout, this, &MainWindow::getData);


    // Conexão do botão connect com o slot da função responsável por conectar-se ao servidor
    connect(ui->pushButtonConnect,
            SIGNAL(clicked(bool)),
            this,
            SLOT(tcpConnect()));

    // Conexão do botão disconnect com o slot da função responsável por disconectar-se do servidor
    connect(ui->pushButtonDisconnect,
            SIGNAL(clicked(bool)),
            this,
            SLOT(tcpDisconnect()));

    // Conexão do botão start com o slot da função responsável por iniciar o envio de dados
    connect(ui->pushButtonStart,
            SIGNAL(clicked(bool)),
            this,
            SLOT(startSending()));

    // Conexão do botão stop com o slot da função responsável por parar o envio de dados
    connect(ui->pushButtonStop,
            SIGNAL(clicked(bool)),
            this,
            SLOT(stopSending()));


    // Conexão do botão update com o slot da função responsável por atualizar a lista de clientes produtores
    connect(ui->pushButtonUpdate,
            SIGNAL(clicked(bool)),
            this,
            SLOT(update()));

    // Conexão do slider de intervalo com o slot da função responsável por tratar o timer de envio de dados
    connect(ui->horizontalSliderTiming,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(setTimerInterval(int)));

}

//Implementação da função membro  que conecta-se ao servidor
void MainWindow::tcpConnect() {
    QString ipservidor = ui->lineEditIp->text();
    socket->connectToHost(ipservidor, 1234);
    if (socket->waitForConnected(3000)) {
        qDebug() << "Connected";
    } else {
        qDebug() << "Disconnected";
    }
}

//Implementação da função membro  que disconecta-se do servidor
void MainWindow::tcpDisconnect() {
    socket->disconnectFromHost();
    timer->stop();
}

//Implementação da função membro  que recebe so horizontal slider qual devrá ser a velocidade de requisições ao servidor
void MainWindow::setTimerInterval(int interval) {
    timer->setInterval(interval);
}

//Implementação da função membro  que inicia as requisições de dados ao servidor
void MainWindow::startSending() {
    if (!ipselect.isEmpty()) {
        timer->start();
    }
}

//Implementação da função membro  que pausa as requisições de dados ao servidor
void MainWindow::stopSending() {
    timer->stop();
}

//Implementação da função membro  que solicita ao servidor atualização dos clientes produtores
void MainWindow::update() {
    QStringList iplist;
    if (socket->state() == QAbstractSocket::ConnectedState) {
        if (socket->isOpen()) {
            qDebug() << "reading...";
            socket->write("list\r\n");
            socket->waitForBytesWritten();
            socket->waitForReadyRead();
            ui->listWidget->clear();
            while (socket->bytesAvailable()) {
                iplist << socket->readLine().replace("\n", "").replace("\r", "");
                qDebug() << "Connected IP: " << iplist;
                ui->listWidget->addItems(iplist);
                iplist.clear();
            }
        }
    } else {
        qDebug() << "Você não está conectado a nenhum servidor";
    }
}

//Implementação da função membro  que recebe o ip selecionado e prepara o prompt de requisição ao servidor
void MainWindow::getIp(QListWidgetItem *_ipselect) {
    ipselect = _ipselect->text();
    ipselect = "get " + ipselect + " 30\r\n";
    qDebug() << "Ip selecionado: " << ipselect;
}

//Implementação da função membro  que solicita os dados e os recebe
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
                    float floatValue = str.toFloat(&ok); // convertendo str para float (para a plotagem
                    floatVector.append(floatValue); //passa os valores convertidos para o vetor
                    //qDebug() << thetime << ": " << str;
                }
            }
        }
    }

    // Passando os vetores floatVector e timeVector para a classe  Plotter
    plotter->update(); // update the plotter widget
    plotter->setFloatVector(floatVector);
    plotter->setTimeVector(timeVector);
    //esvaziando os vetores antes de entrar no wille dnv
    timeVector.clear();
    floatVector.clear();
}

//destrutor da classe
MainWindow::~MainWindow() {
    delete socket;
    delete ui;
}
