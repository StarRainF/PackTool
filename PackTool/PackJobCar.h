#pragma once
#pragma execution_character_set("UTF-8")


#include "PackJobCarTableModel.h"
#include "PackJobCarSetting_OffLine.h"
#include "PackJobCarSetting_OnLine.h"
#include "common.h"
#include "stdafx.h"
#include "ui_win_PackCarJob.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
	class PackJobCarWin;
};

QT_END_NAMESPACE

#ifndef MAX_NUM(lst)
#define MAX_NUM(lst) (*std::max_element(lst.begin(), lst.end()))
#endif

#ifndef MIN_NUM(lst)
#define MIN_NUM(lst) (*std::min_element(lst.begin(), lst.end()))
#endif

class PackJobCar : public QDialog
{
	Q_OBJECT//使用Qt的信号与槽（signal and slot)机制的类必须要添加的一个宏

public:

	enum CarVersion
	{
		ON_LINE,
		OFF_LINE
	};

	PackJobCar(QWidget* parent = nullptr);
	~PackJobCar() override;

	int m_nCountFiles; // 当前待处理的文件数量
	double m_dCurrPgValue; // 当前进度条值

	QString m_sCarPath;
	QString m_sPathJob;
	QString m_sPathFm;
	QString m_sPathOut;

	bool	m_bUseCarPath = true;
	bool	m_bUseJobPath = true;
	bool	m_bUseFmPath = false;
	bool	m_bExtrcBoardsAll = true;		// 目前采取每个批量抽所有板的模式
	bool	m_bExtrcPlsAll = false;

	PJC_TableModel* m_pModelCar ;
	PJC_TableModel* m_pModelJob ;
	PJC_TableModel* m_pModelFm ;

	QMap<QString, int> m_mapSelectedPn; // 选中的料号
	QStringList m_lstPns;				// Car文件夹下的料号名
	QList<int> m_lstNumConstraint = { 1, 2, 3, 4, 5 };		// 对在线板的板号限制

	void loadTableView(QString sRootPath, QTableView* pTableView, int nMode = 0); // 加载car和job文件夹到界面
	QStringList getSubDirs(QString sRootPth, bool bIsCarPth = false); // 获得所有的子文件夹名称列表
	void buildTableViewConnection(QTableView* signal, QTableView* slot); // 检查连接关系

	bool	copyDirWithoutFilter(QString sDirSrcRoot, QString sDirDstRoot, bool bCanOverWrite);
	bool	copyDirWithFilter(QStringList lstWantedPls, QStringList lstWantedBoards, QString sDirSrcRoot, QString sDirDstRoot, bool bCanOverWrite);
	int		getNumOfFiles(QString sDirSrc);

public slots:
	void onParkThreadFinished();
	void onPgValueUpdated(int nPgValue);
	void onPgLabelUpdated(int nMode);

signals:
	void sendMsg(QString sMsg);
	void sigFinished();
	void sendPgValue(int nPgValue);
	void sendPgLabel(int nMode);

private:
	Ui::PackJobCarWin* ui;
	QTextCodec* m_pCoder;
	QSettings* m_pConfig;

	int m_nNumOfPlsForPack = 5;
	int m_nNumOfBoardsForPack = 5;
	int m_nPackMode = 1;
	int m_nLowerBoundIdx = 0;
	int m_nUpperBoundIdx = 999;

	PackCfg_OffLine m_cfg_off_line;
	PackCfg_OnLine m_cfg_on_line;

	void loadIni();
	void saveIni();
	void setScrollBarStyle(QScrollBar* pScrollBar);
	void setUiStateDuringThread(bool bState);
	void startParkThread();
	void packCar_OffLine();
	void packCar_OnLine();
	void packJob();
	void packFm();
	QStringList getCheckedSubDirs(int nMode);
	CarVersion getCarVersion();
};


