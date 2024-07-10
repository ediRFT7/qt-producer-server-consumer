#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QLineEdit>
#include <QTimerEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    timer = new QTimer(this); // Initialize the timer

    max = 0;
    min = 0;

    // Conexão do timer ao slot putData
    connect(timer, &QTimer::timeout, this, &MainWindow::putData);

    // Conexão do botão start
    connect(ui->pushButtonStart,
            SIGNAL(clicked(bool)),
            this,
            SLOT(startSending()));

    // Conexão do botão stop
    connect(ui->pushButtonStop,
            SIGNAL(clicked(bool)),
            this,
            SLOT(stopSending()));

    // Button to connect to the server
    connect(ui->pushButtonConnect,
            SIGNAL(clicked(bool)),
            this,
            SLOT(tcpConnect()));

    // Button to disconnect from the server
    connect(ui->pushButtonDisconnect,
            SIGNAL(clicked(bool)),
            this,
            SLOT(tcpDisconnect()));

    // Connection for the maximum value horizontal slider
    connect(ui->horizontalSliderMax,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(setValorMax(int)));

    // Connection for the minimum value horizontal slider
    connect(ui->horizontalSliderMin,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(setValorMin(int)));

    // Connection for the timer interval horizontal slider
    connect(ui->horizontalSliderTiming,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(setTimerInterval(int)));
}

void MainWindow::tcpConnect() {
    QString ipservidor = ui->lineEditIp->text(); // QString with the entered IP
    socket->connectToHost(ipservidor, 1234);
    if (socket->waitForConnected(3000)) {
        qDebug() << "Connected";
    } else {
        qDebug() << "Disconnected";
    }
}

void MainWindow::tcpDisconnect() {
    socket->disconnectFromHost();
    timer->stop(); // Stop the timer if it's running at the time of disconnect
}

void MainWindow::putData() {
    QString str;
    QDateTime datetime;
    qint64 msecdate;

    if (socket->state() == QAbstractSocket::ConnectedState) { // Check if the socket is connected
        msecdate = QDateTime::currentDateTime().toMSecsSinceEpoch(); // Get the current time in ms
        str = "set " + QString::number(msecdate) + " " +
              QString::number(min + rand() % (max - min + 1));
        ui->textBrowser->append(str); // Display the data in the text browser
        str = str + "\r\n"; // Add the necessary literal for the server
        qDebug() << str; // Display the literal on the screen
        qDebug() << socket->write(str.toStdString().c_str()) // Write the literal to the socket (bytes)
                 << " bytes written";
        // waitForBytesWritten returns the number of bytes that were written
        if (socket->waitForBytesWritten(3000)) {
            qDebug() << "wrote";
        }
    }
}

void MainWindow::setValorMin(int _min) {
    min = _min;
    repaint();
}

void MainWindow::setValorMax(int _max) {
    max = _max;
    repaint();
}

void MainWindow::setTimerInterval(int interval) {
    timer->setInterval(interval);
}

void MainWindow::startSending() {
    timer->start();
}

void MainWindow::stopSending() {
    timer->stop();
}

MainWindow::~MainWindow() {
    delete socket;
    delete ui;
}
