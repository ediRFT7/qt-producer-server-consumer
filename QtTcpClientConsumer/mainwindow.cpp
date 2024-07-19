#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QTimerEvent>
#include <QLineEdit>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  socket = new QTcpSocket(this);
  timer = new QTimer(this); // Inicializando o timer


  // Conexão do timer ao slot putData
  connect(timer, &QTimer::timeout, this, &MainWindow::getData);
  //tcpConnect();

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


//implementação do método que faz a conexão com o servidor
void MainWindow::tcpConnect(){
  QString ipservidor = ui->lineEditIp->text();
  socket->connectToHost(ipservidor,1234);
  if(socket->waitForConnected(3000)){
    qDebug() << "Connected";
  }
  else{
    qDebug() << "Disconnected";
  }
}

//Implementação do método responsável por de desconectar do servidor
void MainWindow::tcpDisconnect() {
    socket->disconnectFromHost();
   timer->stop(); // para o timer para que ao se reconectar o pedido de dados não esteja engatilhado
}

//Implementação do método que trata o intervalo entre uma requisição e outra.
void MainWindow::setTimerInterval(int interval) {
    timer->setInterval(interval);
}

//Implementação do método que trata o timer para que os dados possam ser enviados ao servidor
void MainWindow::startSending() {
    if(!ipselect.isEmpty()){ //verifica se o ip do cliente produtor foi selecionado.
        timer->start();
    }
}

//Implementação do método que para o timer de envio dos dados ao servidor
void MainWindow::stopSending() {
    timer->stop();
}

//Implementação do método que atualiza a lista de clientes produtores de dados
void MainWindow::update(){
    QStringList iplist;
    if(socket->state() == QAbstractSocket::ConnectedState){
        if(socket->isOpen()){
            qDebug() << "reading...";
            socket->write("list\r\n");
            socket->waitForBytesWritten(); //aguarda até que os bytes sejam escritos
            socket->waitForReadyRead(); //aguarda até que haja uma resposta disponível para leitura
            //qDebug() << socket->bytesAvailable();
            ui->listWidget->clear(); //limpa o listwidget antes de atualizar
            iplist.clear();
            while(socket->bytesAvailable()){
                iplist << socket->readLine().replace("\n","").replace("\r","");
                qDebug() << "Connected IP: " << iplist; //recebendo ip
                ui->listWidget->addItems(iplist); //mandando os ips para o listwidget

            }
        }
    }
    else{
        qDebug() << "Você não está conectado a nenhum servidor";

    }
}
//recebendo Ip selecionado pelo cliente e preparando a requisição para o servidor:
void MainWindow::getIp(QListWidgetItem *_ipselect){
    ipselect = _ipselect->text();
    ipselect = "get " + ipselect + " 1\r\n"; //Preparando requisição para o servidor
    qDebug() << "Ip selecionado: " <<ipselect;

}

//recebendo os dados solicitados ao servidor:
void MainWindow::getData(){
  QString str;
  QStringList list;
  qint64 thetime;
  qDebug() << "to get data...";
  if(socket->state() == QAbstractSocket::ConnectedState){
    if(socket->isOpen()){
      qDebug() << "reading...";
      socket->write(ipselect.toStdString().c_str());
      socket->waitForBytesWritten();
      socket->waitForReadyRead();
      qDebug() << socket->bytesAvailable();
      while(socket->bytesAvailable()){
        str = socket->readLine().replace("\n","").replace("\r","");
        list = str.split(" ");
        if(list.size() == 2){
          bool ok;
          str = list.at(0);
          thetime = str.toLongLong(&ok);
          str = list.at(1);
          qDebug() << thetime << ": " << str;
        }
      }
    }
  }
}


MainWindow::~MainWindow()
{
  delete socket;
  delete ui;
}
