#include "stdafx.h"
#include "PackJobCarSetting_OffLine.h"

PackJobCarSetting_OffLine::PackJobCarSetting_OffLine(QWidget* parent)
	: QDialog(parent), ui(new Ui::PackJobCarSettingWin_OffLine())
{
	ui->setupUi(this);
	ui->inputNumOfBoards->setEnabled(false);

	m_cfg.num_of_boards = &m_iNumOfBoards;
	m_cfg.num_of_pls = &m_iNumOfPls;
	m_cfg.pack_mode = &m_nPackMode;

	// 创建QButtonGroups
	m_pBtnGroup = new QButtonGroup(this);
	m_pBtnGroup->addButton(ui->rbtn_Random, 0);
	m_pBtnGroup->addButton(ui->rbtn_ByTimeDescend, 1);
	m_pBtnGroup->addButton(ui->rbtn_ByTimeAscend, 2);

	loadConfig();

	connect(m_pBtnGroup, SIGNAL(buttonClicked(QAbstractButton*)), this,
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
		m_iNumOfBoards = ui->inputNumOfBoards->text().toInt();
	});
	connect(ui->inputNumOfPls, &QLineEdit::editingFinished, [=]()
	{
		m_iNumOfPls = ui->inputNumOfPls->text().toInt();
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

PackJobCarSetting_OffLine::~PackJobCarSetting_OffLine()
{
}

void PackJobCarSetting_OffLine::saveConfig()
{
	auto pConfigIni = new QSettings(m_sConfigPath, QSettings::IniFormat);
	pConfigIni->setIniCodec("UTF-8");

	pConfigIni->beginGroup("PackJobCar_OffLine");
	pConfigIni->setValue("num_of_boards", QVariant(m_iNumOfBoards));
	pConfigIni->setValue("num_of_pls", QVariant(m_iNumOfPls));
	pConfigIni->setValue("pack_mode", QVariant(m_nPackMode));
	pConfigIni->endGroup();
}

void PackJobCarSetting_OffLine::loadConfig()
{
	auto pConfigIni = new QSettings(m_sConfigPath, QSettings::IniFormat);
	pConfigIni->setIniCodec("UTF-8");

	pConfigIni->beginGroup("PackJobCar_OffLine");
	m_iNumOfBoards = pConfigIni->value("num_of_boards").toInt();
	m_iNumOfPls = pConfigIni->value("num_of_pls").toInt();
	m_nPackMode = pConfigIni->value("pack_mode").toInt();
	pConfigIni->endGroup();

	ui->inputNumOfBoards->setText(QString::number(m_iNumOfBoards));
	ui->inputNumOfPls->setText(QString::number(m_iNumOfPls));
	QAbstractButton* btn = m_pBtnGroup->button(m_nPackMode);
	btn->setChecked(true);
}

void PackJobCarSetting_OffLine::radioButtonClicked(QAbstractButton* button)
{
	// 获取被选中的QRadioButton
	auto selectedRadioButton = qobject_cast<QRadioButton*>(button);
	if (selectedRadioButton)
	{
		QString text = selectedRadioButton->objectName();
		m_nPackMode = text.split("_")[1].toInt() - 1;
	}
}
