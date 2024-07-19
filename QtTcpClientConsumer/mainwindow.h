#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QDebug>
#include <QTimer>
#include <QStringList>
#include <QListWidgetItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  
//  void tcpConnect();
public slots:
    void getData();
    void tcpConnect();
    void tcpDisconnect();
    void setTimerInterval(int interval);
    void startSending();
    void stopSending();
    void update();
    void getIp(QListWidgetItem *_ipselect);
    //QStringList getIPList();

private:
  Ui::MainWindow *ui;
  QTcpSocket *socket;
  QTimer *timer; // Timer for sending data at intervals
  QString ipselect;
};

#endif // MAINWINDOW_H
