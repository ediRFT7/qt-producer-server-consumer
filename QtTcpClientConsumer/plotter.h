#ifndef PLOTTER_H
#define PLOTTER_H

#include <QWidget>

class Plotter : public QWidget {
    Q_OBJECT
public:
    explicit Plotter(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event); // Executado toda vez que a tela precisa ser redesenhada
    void setFloatVector(const QVector<float>& floatVector); // set the floatVector
    void setTimeVector(const QVector<qint64>& timeVector); // set the timeVector

private:
    QVector<float> m_floatVector; // store the floatVector
    QVector<qint64> m_timeVector; // store the timeVector
};

#endif // PLOTTER_H
