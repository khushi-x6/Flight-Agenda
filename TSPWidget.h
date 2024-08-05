#ifndef TSPWIDGET_H
#define TSPWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVector>

#define MAX_CITIES 10

struct City {
    int x, y;
};

class TSPWidget : public QWidget {
    Q_OBJECT

public:
    explicit TSPWidget(QWidget *parent = nullptr);

private slots:
    void setCityInputsVisibility();
    void calculateTSP();

private:
    QLineEdit *numCitiesInput;
    QLabel *resultLabel;
    QGraphicsView *view;
    QGraphicsScene *scene;
    QVector<QLineEdit*> cityInputsX;
    QVector<QLineEdit*> cityInputsY;

    double distance(City city1, City city2);
    void tspDynamicProgramming(int numCities, const QVector<City> &cities);
    double tspDP(int mask, int pos, int numCities, const QVector<QVector<double>> &dist, QVector<QVector<double>> &dp, QVector<QVector<int>> &parent);
    void drawTour(const QVector<City> &cities, const QVector<int> &tour);
};

#endif // TSPWIDGET_H
