#include "widget.h"
#include <qapplication.h>

int main(int args, char* argv[]) {
    QApplication app(args, argv);
    myWidget w;
    w.resize(960, 640);
    w.show();
    return app.exec();
}
