#pragma once

#include "common.h"
#include "ui_win_PackCarJobSetting_OnLine.h"

using namespace Ui;

enum PackRange_OnLine
{
	SELECT_ALL,
	SELECT_BY_IDX_RANGE,
	SELECT_BY_NUM,
};

enum PackMode_OnLine
{
	RANDOM = 0,
	TIME_DESCEND = 1,
	TIME_ASCEND = 2,
};

struct PackCfg_OnLine
{
	int* num_of_boards;
	int* lower_bound_idx;
	int* upper_bound_idx;
	PackRange_OnLine pack_range;
	PackMode_OnLine pack_mode;
};

class PackJobCarSetting_OnLine  : public QDialog
{
	Q_OBJECT

public:
	PackJobCarSetting_OnLine(QWidget *parent);
	~PackJobCarSetting_OnLine();

	//PackCfg getConfig();

public slots:
	void radioButtonClicked(QAbstractButton* button);

signals:
	void sendConfig(PackCfg_OnLine);

private:
	Ui::PackJobCarSettingWin_OnLine* ui;

	QString m_sConfigPath = QApplication::applicationDirPath() + "\\Config.ini";
	PackCfg_OnLine m_cfg;

	QButtonGroup * m_pBtnGroupPackMode;
	QButtonGroup * m_pBtnGroupPackRange;
	PackRange_OnLine m_PackRange;
	bool m_bPackByIdxRange = true;
	int m_nLowerBoundIdx = 0;
	int m_nUpperBoundIdx = 999;
	int m_nNumOfBoards = 5;
	PackMode_OnLine m_nPackMode = TIME_DESCEND;

	void saveConfig();
	void loadConfig();

	PackRange_OnLine getPackRange();
	PackMode_OnLine getPackMode();
};
