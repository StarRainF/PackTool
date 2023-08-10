#pragma once

#include <QObject>
#include <QSettings>
#include "ui_win_PackCarJobSetting_OffLine.h"

using namespace Ui;

struct PackCfg_OffLine
{
	int* num_of_boards = 0;
	int* num_of_pls = 0;
	int* pack_mode = 0;
};

class PackJobCarSetting_OffLine  : public QDialog
{
	Q_OBJECT

public:
	PackJobCarSetting_OffLine(QWidget *parent);
	~PackJobCarSetting_OffLine();

	//PackCfg getConfig();

public slots:
	void radioButtonClicked(QAbstractButton* button);


signals:
	void sendConfig(PackCfg_OffLine);

private:
	Ui::PackJobCarSettingWin_OffLine* ui;

	QString m_sConfigPath = QApplication::applicationDirPath() + "\\Config.ini";
	PackCfg_OffLine m_cfg;

	QButtonGroup * m_pBtnGroup;
	int m_iNumOfBoards = 0;
	int m_iNumOfPls = 5;
	int m_nPackMode = 1;

	void saveConfig();
	void loadConfig();
};
