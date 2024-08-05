#include "tspwidget.h"
#include <QGridLayout>
#include <QMessageBox>
#include <cmath>
#include <limits>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QGraphicsPolygonItem>
#include <QPen>
#include <unordered_map>

TSPWidget::TSPWidget(QWidget *parent)
    : QWidget(parent), numCitiesInput(new QLineEdit), resultLabel(new QLabel("Result: ")),
    view(new QGraphicsView), scene(new QGraphicsScene) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    this->setFixedSize(1200, 600);

    //Title
    QLabel *executionLabel = new QLabel("Travelling Salesman Problem", this);
    executionLabel->setAlignment(Qt::AlignCenter);
    executionLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: black;");
    layout()->addWidget(executionLabel);

    //background color
    this->setStyleSheet("background-color: rgb(170, 170, 200); color: black;");

    //enter number of cities
    QGridLayout *inputLayout = new QGridLayout;
    inputLayout->addWidget(new QLabel("Number of cities:"), 0, 0);
    inputLayout->addWidget(numCitiesInput, 0, 1);

    //set city button
    QPushButton *setCitiesButton = new QPushButton("Set Cities");
    connect(setCitiesButton, &QPushButton::clicked, this, &TSPWidget::setCityInputsVisibility);    //start exectution of setCityInputsVisibility

    //input city point(x,y)
    for (int i = 0; i < MAX_CITIES; ++i) {
        QLineEdit *lineEditX = new QLineEdit;
        lineEditX->setPlaceholderText(QString("City %1 X").arg(i));
        lineEditX->setVisible(false);
        QLineEdit *lineEditY = new QLineEdit;
        lineEditY->setPlaceholderText(QString("City %1 Y").arg(i));
        lineEditY->setVisible(false);
        inputLayout->addWidget(lineEditX, i + 1, 0);
        inputLayout->addWidget(lineEditY, i + 1, 1);
        cityInputsX.append(lineEditX);
        cityInputsY.append(lineEditY);
    }

    //exectute TSP
    QPushButton *calculateButton = new QPushButton("Calculate TSP");
    connect(calculateButton, &QPushButton::clicked, this, &TSPWidget::calculateTSP);    //start execution of calculateTSP


    mainLayout->addLayout(inputLayout);
    mainLayout->addWidget(setCitiesButton);
    mainLayout->addWidget(calculateButton);
    mainLayout->addWidget(resultLabel);

    //set GUI
    this->setStyleSheet("background-color: rgb(225,225,225); color: black;");
    setCitiesButton->setStyleSheet("background-color: white; color: black;");
    calculateButton->setStyleSheet("background-color: white; color: black;");
    numCitiesInput->setStyleSheet("background-color: white; color: black;");
    executionLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: black;font-size:20px;");
    resultLabel->setStyleSheet("color: black;font-size:20px;font-weight:bold");

    for (int i = 0; i < MAX_CITIES; ++i) {
        cityInputsX[i]->setStyleSheet("background-color: white; color: black;");
        cityInputsY[i]->setStyleSheet("background-color: white; color: black;");
    }

    //what to display
    view->setScene(scene);
    //placement
    mainLayout->addWidget(view);
}

void TSPWidget::setCityInputsVisibility() {
    bool ok;
    int numCities = numCitiesInput->text().toInt(&ok);
    if (!ok || numCities <= 0 || numCities > MAX_CITIES) {
        QMessageBox::warning(this, "Invalid input", "Please enter a valid number of cities (1-10).");
        return;
    }

    for (int i = 0; i < MAX_CITIES; ++i) {
        cityInputsX[i]->setVisible(i < numCities);
        cityInputsY[i]->setVisible(i < numCities);
    }
}

void TSPWidget::calculateTSP() {
    bool ok;
    int numCities = numCitiesInput->text().toInt(&ok);
    if (!ok || numCities <= 0 || numCities > MAX_CITIES) {
        QMessageBox::warning(this, "Invalid input", "Please enter a valid number of cities (1-10).");
        return;
    }

    QVector<City> cities(numCities);
    for (int i = 0; i < numCities; ++i) {
        int x = cityInputsX[i]->text().toInt(&ok);
        if (!ok) {
            QMessageBox::warning(this, "Invalid input", QString("Please enter a valid x coordinate for city %1.").arg(i));
            return;
        }
        int y = cityInputsY[i]->text().toInt(&ok);
        if (!ok) {
            QMessageBox::warning(this, "Invalid input", QString("Please enter a valid y coordinate for city %1.").arg(i));
            return;
        }
        cities[i].x = x;
        cities[i].y = y;
    }

    tspDynamicProgramming(numCities, cities);
}

double TSPWidget::distance(City city1, City city2) {
    return std::sqrt(std::pow(city1.x - city2.x, 2) + std::pow(city1.y - city2.y, 2));
}

double TSPWidget::tspDP(int mask, int pos, int numCities, const QVector<QVector<double>> &dist, QVector<QVector<double>> &dp, QVector<QVector<int>> &parent) {
    //mask=1->visited
    if (mask == (1 << numCities) - 1) {
        return dist[pos][0];
    }
    //if -1 already stored
    if (dp[mask][pos] != -1) {
        return dp[mask][pos];
    }

    //initially infinity
    double ans = std::numeric_limits<double>::infinity();
    int chosenCity = -1;

    for (int city = 0; city < numCities; ++city) {
        if ((mask & (1 << city)) == 0) {
            double newAns = dist[pos][city] + tspDP(mask | (1 << city), city, numCities, dist, dp, parent);
            if (newAns < ans) {
                ans = newAns;
                chosenCity = city;
            }
        }
    }

    parent[mask][pos] = chosenCity;
    return dp[mask][pos] = ans;
}

void TSPWidget::tspDynamicProgramming(int numCities, const QVector<City> &cities) {
    QVector<QVector<double>> dist(numCities, QVector<double>(numCities));
    for (int i = 0; i < numCities; ++i) {
        for (int j = 0; j < numCities; ++j) {
            dist[i][j] = distance(cities[i], cities[j]);
        }
    }

    QVector<QVector<double>> dp(1 << numCities, QVector<double>(numCities, -1));
    QVector<QVector<int>> parent(1 << numCities, QVector<int>(numCities, -1));

    double shortestDistance = tspDP(1, 0, numCities, dist, dp, parent);

    QVector<int> tour;
    int mask = 1, pos = 0;
    tour.append(0);

    for (int i = 0; i < numCities - 1; ++i) {
        pos = parent[mask][pos];
        mask |= (1 << pos);
        tour.append(pos);
    }

    QString result = "Shortest tour: ";
    for (int i = 0; i < tour.size(); ++i) {
        result += QString::number(tour[i]) + " ";
    }
    result += QString("\nShortest distance: %1").arg(shortestDistance);
    resultLabel->setText(result);

    drawTour(cities, tour);
}

void TSPWidget::drawTour(const QVector<City> &cities, const QVector<int> &tour) {
    scene->clear();
    QPen pen(Qt::red);
    QBrush brush(Qt::SolidPattern);
    QFont font;
    font.setPointSize(15);
    QPen textPen(Qt::black);

    for (int i = 0; i < cities.size(); ++i) {
        const City &city = cities[i];
        if (i == 0) {
            brush.setColor(Qt::black);
        } else {
            brush.setColor(Qt::yellow);
        }
        scene->addEllipse(city.x - 3, city.y - 3, 6, 6, pen, brush);
        QGraphicsTextItem *label = scene->addText(QString::number(i), font);
        label->setDefaultTextColor(Qt::black);
        label->setPos(city.x - 3, city.y - 20);
    }

    for (int i = 0; i < tour.size(); ++i) {
        int from = tour[i];
        int to = tour[(i + 1) % tour.size()];
        QGraphicsLineItem *line = scene->addLine(cities[from].x, cities[from].y, cities[to].x, cities[to].y, pen);
        QGraphicsPolygonItem *arrowHead = new QGraphicsPolygonItem;

        double angle = std::atan2(cities[to].y - cities[from].y, cities[to].x - cities[from].x);

        QPolygonF arrow;
        arrow.append(QPointF(0, 0));
        arrow.append(QPointF(-5, 5));
        arrow.append(QPointF(5, 5));
        arrowHead->setPolygon(arrow);
        arrowHead->setPos(cities[to].x, cities[to].y);
        arrowHead->setRotation(angle * 180 / M_PI + 90);
        arrowHead->setBrush(QBrush(Qt::red));

        scene->addItem(arrowHead);
    }
}
