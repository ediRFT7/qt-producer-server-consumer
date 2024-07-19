#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QLineEdit>
#include <QTimerEvent>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    timer = new QTimer(this); // Inicializando o timer

    max = 0; //Iniciando variável de valor máximo
    min = 0; // Iniciando a variável de valor mínimo

    // Conexão do timer ao slot putData
    connect(timer, &QTimer::timeout, this, &MainWindow::putData);

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

    // Conexão do slider de valor máximo com o slot responsável por tratar os dados que serão enviados
    connect(ui->horizontalSliderMax,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(setValorMax(int)));

    // Conexão do slider de valor mínimo com o slot responsável por tratar os dados que serão enviados
    connect(ui->horizontalSliderMin,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(setValorMin(int)));

    // Conexão do slider de intervalo com o slot responsável por tratar o timer de envio de dados
    connect(ui->horizontalSliderTiming,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(setTimerInterval(int)));
}

//Implementação do método responsável por se conectar ao servidor
void MainWindow::tcpConnect() {
    QString ipservidor = ui->lineEditIp->text(); // QString with the entered IP
    socket->connectToHost(ipservidor, 1234);
    if (socket->waitForConnected(3000)) {
        qDebug() << "Connected";
    } else {
        qDebug() << "Disconnected";
    }
}

//Implementação do método responsável por de desconectar do servidor
void MainWindow::tcpDisconnect() {
    socket->disconnectFromHost();
    timer->stop(); // Stop the timer if it's running at the time of disconnect
}

//Implementação do método responsável por toda a geração do dado de envio
void MainWindow::putData() {
    QString str;
    QDateTime datetime;
    qint64 msecdate;
    float random;
   // QString floatString;

    //Gerador de números aleatórios:
    double randomFloat = min + (max - min) * QRandomGenerator::global()->generateDouble();

    if (socket->state() == QAbstractSocket::ConnectedState) { //condição que checa a conexão
        msecdate = QDateTime::currentDateTime().toMSecsSinceEpoch();
        str = "set " + QString::number(msecdate) + " " +
              QString::number(randomFloat, 'f', 2);
        ui->textBrowser->append(str);
        str = str + "\r\n";
        qDebug() << str;
        qDebug() << socket->write(str.toStdString().c_str()) << " bytes written";
        // waitForBytesWritten returns the number of bytes that were written
        if (socket->waitForBytesWritten(3000)) {
            qDebug() << "wrote";
        }
    }
}

//Implementação do método que recebe o valor mínimo que pode ser enviado ao servidor
void MainWindow::setValorMin(int _min) {
    min = _min;
    repaint();
}

//Implementação do método que recebe o valor máximo que pode ser enviado ao servidor
void MainWindow::setValorMax(int _max) {
    max = _max;
    repaint();
}

//Implementação do método que trata o intervalo entre um dado e outro
void MainWindow::setTimerInterval(int interval) {
    timer->setInterval(interval);
}

//Implementação do método que trata o timer para que os dados possam ser enviados ao servidor
void MainWindow::startSending() {
    timer->start();
}

//Implementação do método que para o timer de envio dos dados ao servidor
void MainWindow::stopSending() {
    timer->stop();
}

//Destrutor da classe
MainWindow::~MainWindow() {
    delete socket;
    delete ui;
}
