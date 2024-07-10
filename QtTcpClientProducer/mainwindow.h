#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void putData();
    void setValorMin(int _min);
    void setValorMax(int _max);
    void setTimerInterval(int interval);
    void tcpConnect();
    void tcpDisconnect();
    void startSending();
    void stopSending();

private:
    int min, max;
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QTimer *timer; // Timer for sending data at intervals
};

#endif // MAINWINDOW_H
