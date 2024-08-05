#include <QApplication>
#include "tspwidget.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    TSPWidget widget;
    widget.show();
    return app.exec();
}
