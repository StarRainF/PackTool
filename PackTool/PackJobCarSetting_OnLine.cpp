#include "stdafx.h"
#include "PackJobCarSetting_OnLine.h"

PackJobCarSetting_OnLine::PackJobCarSetting_OnLine(QWidget* parent)
	: QDialog(parent), ui(new Ui::PackJobCarSettingWin_OnLine())
{
	ui->setupUi(this);
	ui->inputNumOfBoards->setEnabled(false);

	m_cfg.num_of_boards = &m_nNumOfBoards;
	m_cfg.lower_bound_idx = &m_nLowerBoundIdx;
	m_cfg.upper_bound_idx = &m_nUpperBoundIdx;
	m_cfg.pack_mode = m_nPackMode;

	// 创建QButtonGroups
	m_pBtnGroupPackMode = new QButtonGroup(this);
	m_pBtnGroupPackMode->addButton(ui->rbtn_Random, 0);
	m_pBtnGroupPackMode->addButton(ui->rbtn_ByTimeDescend, 1);
	m_pBtnGroupPackMode->addButton(ui->rbtn_ByTimeAscend, 2);

	m_pBtnGroupPackRange = new QButtonGroup(this);
	m_pBtnGroupPackMode->addButton(ui->rbtn_SelectAll, 0);
	m_pBtnGroupPackMode->addButton(ui->rbtn_ByIdxRange, 1);
	m_pBtnGroupPackMode->addButton(ui->rbtn_ByNum, 2);

	loadConfig();

	connect(m_pBtnGroupPackMode, SIGNAL(buttonClicked(QAbstractButton*)), this,
	        SLOT(radioButtonClicked(QAbstractButton*)));
	connect(ui->btnConfirm, &QPushButton::clicked, [=]()
	{
		saveConfig();
		emit sendConfig(m_cfg);
	});
	connect(ui->btnConfirm, SIGNAL(clicked()), this, SLOT(close()));
	connect(ui->btnCancle, SIGNAL(clicked()), this, SLOT(close()));

	connect(ui->inputNumOfBoards, &QLineEdit::editingFinished, [=]()
	{
		m_nNumOfBoards = ui->inputNumOfBoards->text().toInt();
	});
	connect(ui->inputIdxBoards_lower, &QLineEdit::editingFinished, [=]()
	{
		m_nLowerBoundIdx = ui->inputIdxBoards_lower->text().toInt();
	});
	connect(ui->inputIdxBoards_upper, &QLineEdit::editingFinished, [=]()
	{
		m_nUpperBoundIdx = ui->inputIdxBoards_upper->text().toInt();
	});

#ifdef _DEBUG
	connect(ui->btnQuickOpen, &QPushButton::clicked, [=]()
	{
		QString ConfigPath = QCoreApplication::applicationDirPath() + "/Config.ini";
		QDesktopServices::openUrl(QUrl::fromLocalFile(ConfigPath));
	});
#else
	ui->btnQuickOpen->setVisible(false);
#endif
}

PackJobCarSetting_OnLine::~PackJobCarSetting_OnLine()
{
	delete ui;
}

void PackJobCarSetting_OnLine::saveConfig()
{
	auto pConfigIni = new QSettings(m_sConfigPath, QSettings::IniFormat);

	m_nNumOfBoards = ui->inputNumOfBoards->text().toInt();
	m_nLowerBoundIdx = ui->inputIdxBoards_lower->text().toInt();
	m_nUpperBoundIdx = ui->inputIdxBoards_upper->text().toInt();
	m_PackRange = getPackRange();
	m_nPackMode = getPackMode();

	pConfigIni->setIniCodec("UTF-8");
	pConfigIni->beginGroup("PackJobCar_OnLine"); 
	pConfigIni->setValue("num_of_boards", QVariant(m_nNumOfBoards));
	pConfigIni->setValue("lower_bound_idx", QVariant(m_nLowerBoundIdx));
	pConfigIni->setValue("upper_bound_idx", QVariant(m_nUpperBoundIdx));
	pConfigIni->setValue("packRange", QVariant(m_PackRange));
	pConfigIni->setValue("pack_mode", QVariant(m_nPackMode));
	pConfigIni->endGroup();
}

void PackJobCarSetting_OnLine::loadConfig()
{
	auto pConfigIni = new QSettings(m_sConfigPath, QSettings::IniFormat);
	pConfigIni->setIniCodec("UTF-8");

	pConfigIni->beginGroup("PackJobCar_OnLine");
	m_nNumOfBoards = pConfigIni->value("num_of_boards").toInt();
	m_nLowerBoundIdx = pConfigIni->value("lower_bound_idx").toInt();
	m_nUpperBoundIdx = pConfigIni->value("upper_bound_idx").toInt();
	m_nPackMode = PackMode_OnLine(pConfigIni->value("pack_mode").toInt());
	m_PackRange = static_cast<PackRange_OnLine>(pConfigIni->value("packRange").toInt());
	pConfigIni->endGroup();

	ui->inputNumOfBoards->setText(QString::number(m_nNumOfBoards));
	ui->inputIdxBoards_lower->setText(QString::number(m_nLowerBoundIdx));
	ui->inputIdxBoards_upper->setText(QString::number(m_nUpperBoundIdx));
	QAbstractButton* btn = m_pBtnGroupPackMode->button(m_nPackMode);
	btn->setChecked(true);

	btn = m_pBtnGroupPackRange->button(m_PackRange);
	btn->setChecked(true);
}

PackRange_OnLine PackJobCarSetting_OnLine::getPackRange()
{
	if (ui->rbtn_SelectAll->isChecked())
	{
		return SELECT_ALL;
	}
	if (ui->rbtn_ByIdxRange->isChecked())
	{
		return SELECT_BY_IDX_RANGE;
	}
	if (ui->rbtn_Random->isChecked())
	{
		return SELECT_BY_NUM;
	}
}

PackMode_OnLine PackJobCarSetting_OnLine::getPackMode()
{
	if (ui->rbtn_Random->isChecked())
	{
		return RANDOM;
	}
	if (ui->rbtn_ByTimeDescend->isChecked())
	{
		return TIME_DESCEND;
	}
	if (ui->rbtn_ByTimeAscend->isChecked())
	{
		return TIME_ASCEND;
	}
}

void PackJobCarSetting_OnLine::radioButtonClicked(QAbstractButton* button)
{
	// 获取被选中的QRadioButton
	auto selectedRadioButton = qobject_cast<QRadioButton*>(button);
	if (selectedRadioButton)
	{
		QString text = selectedRadioButton->objectName();
		m_nPackMode = PackMode_OnLine(text.split("_")[1].toInt() - 1);
	}
}
