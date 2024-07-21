#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QDebug>
#include <QTimer>
#include <QStringList>
#include <QListWidgetItem>
#include "plotter.h" // Inclua a classe Plotter
#include <QVector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void getData();
    void tcpConnect();
    void tcpDisconnect();
    void setTimerInterval(int interval);
    void startSending();
    void stopSending();
    void update();
    void getIp(QListWidgetItem *_ipselect);

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QTimer *timer; // Timer para enviar dados em intervalos
    QString ipselect;
    QVector<qint64> timeVector;
    QVector<float> floatVector;
    Plotter *plotter; // Ponteiro para a classe Plotter
};

#endif // MAINWINDOW_H
