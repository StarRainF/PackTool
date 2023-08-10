#include <QCoreApplication>
#include <QDir>
#include <string>
#include <iostream>
#include <vector>
#include <QTextCodec>
#include <qdebug>
#include<qglobal.h>

using namespace std;

int main() {

	QString imagePath = "C:\\Users\\1\\Desktop\\demo1";
	QDir dir(imagePath);
	QStringList ImageList;
	ImageList << "*.tmp" << "*.jpg" << "*.png";
	dir.setNameFilters(ImageList);
	for (int i = 0; i < dir.count(); i++) {
		QString ImageName = dir[i];
		qDebug() << ImageName;
	}


}