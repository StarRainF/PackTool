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
	Q_OBJECT//ʹ��Qt���ź���ۣ�signal and slot)���Ƶ������Ҫ��ӵ�һ����

public:

	enum CarVersion
	{
		ON_LINE,
		OFF_LINE
	};

	PackJobCar(QWidget* parent = nullptr);
	~PackJobCar() override;

	int m_nCountFiles; // ��ǰ��������ļ�����
	double m_dCurrPgValue; // ��ǰ������ֵ

	QString m_sCarPath;
	QString m_sPathJob;
	QString m_sPathFm;
	QString m_sPathOut;

	bool	m_bUseCarPath = true;
	bool	m_bUseJobPath = true;
	bool	m_bUseFmPath = false;
	bool	m_bExtrcBoardsAll = true;		// Ŀǰ��ȡÿ�����������а��ģʽ
	bool	m_bExtrcPlsAll = false;

	PJC_TableModel* m_pModelCar ;
	PJC_TableModel* m_pModelJob ;
	PJC_TableModel* m_pModelFm ;

	QMap<QString, int> m_mapSelectedPn; // ѡ�е��Ϻ�
	QStringList m_lstPns;				// Car�ļ����µ��Ϻ���
	QList<int> m_lstNumConstraint = { 1, 2, 3, 4, 5 };		// �����߰�İ������

	void loadTableView(QString sRootPath, QTableView* pTableView, int nMode = 0); // ����car��job�ļ��е�����
	QStringList getSubDirs(QString sRootPth, bool bIsCarPth = false); // ������е����ļ��������б�
	void buildTableViewConnection(QTableView* signal, QTableView* slot); // ������ӹ�ϵ

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


