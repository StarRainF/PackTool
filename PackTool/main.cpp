#include "stdafx.h"
#include "SimpleTools.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	
    QApplication a(argc, argv);
    SimpleTools w;
	w.setWindowTitle("ST 230410");
    w.show();
    return a.exec();
}
