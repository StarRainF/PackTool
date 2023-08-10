#pragma once

#include <QObject>
#include "ui_win_GenAvilog.h"
#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <qfuture.h>
#include <QtConcurrent/qtconcurrentrun.h>
#include <QTime>
#include <QTextStream>

QT_BEGIN_NAMESPACE

namespace Ui
{
	class GenAvilogWin;
};

QT_END_NAMESPACE

class GenAvilog  : public QDialog
{
	Q_OBJECT

public:
	GenAvilog(QWidget *parent = nullptr);
	~GenAvilog();

	QString m_sCarPath;
	QString m_sAvilogPath;
	bool  checkBox_BigAviByCar_ = false;


private:
	Ui::GenAvilogWin* ui;
	void startTraverse();
	void WAvitxt(QString path, QString path1, QString path2, QString path3, bool mode);
	void loadIni();


private slots:
	void btn_OpenCarPath_clicked();
	void btn_OpenAvilog_clicked();

	void Edit_CarPath_clicked();
	void Edit_AvilogPath_clicked();
	void btn_Start_clicked();
	void checkBox_BigAviByCar_clicked();
};
