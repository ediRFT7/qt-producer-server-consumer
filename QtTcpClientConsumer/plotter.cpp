#include "plotter.h"
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QDebug>
#include <algorithm> // Para lidar com o elemento mínimo e máximo

Plotter::Plotter(QWidget *parent)
    : QWidget{parent}
{}

void Plotter::paintEvent(QPaintEvent *event) {
    QPainter painter(this); // Cria um pintor para desenhar no widget
    QBrush brush; // Define o preenchimento das áreas desenhadas
    QPen pen; // criando uma caneta

    // Defina a cor e o estilo do fundo
    brush.setColor(QColor(255, 255, 100)); // Cor amarela para o fundo
    brush.setStyle(Qt::SolidPattern); // Define o fundo como sólido
    painter.setBrush(brush); // Aplica o pincel ao pintor
    painter.drawRect(0, 0, width(), height()); // Desenha um retângulo preenchido com a cor de fundo

    // Definindo a cor e a largura da caneta para os pontos/linhas
    pen.setColor(QColor(0, 0, 255)); // Cor azul para as linhas
    pen.setWidth(2); // Largura da linha de 2 pixels
    painter.setPen(pen); // Aplica a caneta ao pintor

    // Verificando se os vetores têm o mesmo tamanho(vai que acontece...)
    if (m_timeVector.size() != m_floatVector.size()) {
        qDebug() << "Vetores de tamanho diferentes!";
        return;
    }

    // Verificando se os vetores estão vazios
    if (m_timeVector.isEmpty()) {
        qDebug() << "Vetores estão vazios!";
        return;
    }

    // Normalizando os dados:
    qint64 minTime = *std::min_element(m_timeVector.begin(), m_timeVector.end()); // Encontra o menor valor de tempo
    qint64 maxTime = *std::max_element(m_timeVector.begin(), m_timeVector.end()); // Encontra o maior valor de tempo

    // Ajustando os pontos para se ajustarem ao widget
    QVector<QPointF> points; // Vetor que armazenará os pontos normalizados
    for (int i = 0; i < m_timeVector.size(); ++i) {
        // Normalizando x para o intervalo [0, width()]
        float normalizedX = (m_timeVector[i] - minTime) * width() / (maxTime - minTime);

        // Normalizando y para o intervalo [0, height()] considerando que os valores de m_floatVector estão no intervalo [0, 99]
        float normalizedY = height() - (m_floatVector[i] * height() / 99.0); // Invertido porque a origem é no canto superior esquerdo

        points.append(QPointF(normalizedX, normalizedY)); // Depois de normalizado, adicionando o ponto ao vetor
    }

    // Ligando os pontos por linha
    for (int i = 1; i < points.size(); ++i) {
        painter.drawLine(points[i - 1], points[i]); // Desenha uma linha entre os pontos adjacentes
    }
}

void Plotter::setFloatVector(const QVector<float>& floatVector) {
    m_floatVector = floatVector; // Atualiza o vetor de floats
}

void Plotter::setTimeVector(const QVector<qint64>& timeVector) {
    m_timeVector = timeVector; // Atualiza o vetor de tempos
}
