#include "stdafx.h"
#include "PackJobCar.h"

PackJobCar::PackJobCar(QWidget* parent) : QDialog(parent), ui(new Ui::PackJobCarWin())
{
	ui->setupUi(this);
	ui->btnStart->setAutoDefault(false);
	ui->btnCancle->setAutoDefault(false);
	ui->btnLoad->setAutoDefault(false);
	ui->btnOpen->setAutoDefault(false);
	ui->btnSetting->setAutoDefault(false);
	//ui->groupBox_Pattern->setEnabled(false);
	ui->pgBar->setVisible(false);

	m_pCoder = QTextCodec::codecForName("UTF-8");

	loadIni();

	// 取消
	connect(ui->btnCancle, &QPushButton::clicked, [=]()
	{
		saveIni();
		this->hide();
	});

	// 打包
	connect(ui->btnStart, &QPushButton::clicked, [=]()
	{
		setUiStateDuringThread(false);
		startParkThread();
	});

	// 打开输出
	connect(ui->btnOpen, &QPushButton::clicked, [=]()
	{
		QString path = m_sPathOut; //将地址中的"/"替换为"\"，因为在Windows下使用的是"\"。
		path.replace("/", "\\");

		//打开文件管理器路径
		QProcess::startDetached("explorer " + path);
	});

	// 加载到界面
	connect(ui->btnLoad, &QPushButton::clicked, [=]()
	{
		saveIni();
		m_bUseCarPath = ui->label_CarPath->isChecked();
		m_bUseJobPath = ui->label_JobPath->isChecked();
		m_bUseFmPath = ui->label_FmPath->isChecked();

		// 加载文件夹名到表格
		if (m_bUseCarPath)
		{
			loadTableView(m_sCarPath, ui->tableCar, CAR_MODE);
		}
		if (m_bUseJobPath)
		{
			loadTableView(m_sPathJob, ui->tableJob, JOB_MODE);
		}
		if (m_bUseFmPath)
		{
			loadTableView(m_sPathFm, ui->tableFm, FM_MODE);
		}

		// 更新TableView自身状态与显示样式的对应关系
		if (m_bUseCarPath)
		{
			PJC_TableModel *Model = dynamic_cast<PJC_TableModel *>(ui->tableCar->model());
			disconnect(Model, 0, 0, 0);
			ui->cBox_SelectAll_Car->setEnabled(true);
		}
		if (m_bUseJobPath)
		{
			PJC_TableModel *Model = dynamic_cast<PJC_TableModel *>(ui->tableJob->model());
			disconnect(Model, 0, 0, 0);
			ui->cBox_SelectAll_Job->setEnabled(true);
		}
		if (m_bUseFmPath)
		{
			PJC_TableModel *Model = dynamic_cast<PJC_TableModel *>(ui->tableFm->model());
			disconnect(Model, 0, 0, 0);
			ui->cBox_SelectAll_Fm->setEnabled(true);
		}

		// 更新TableView之间的对应关系
		if (m_bUseCarPath && m_bUseFmPath)
		{
			buildTableViewConnection(ui->tableCar, ui->tableFm);
			buildTableViewConnection(ui->tableFm, ui->tableCar);
		}
		if (m_bUseCarPath && m_bUseJobPath)
		{
			buildTableViewConnection(ui->tableCar, ui->tableJob);
			buildTableViewConnection(ui->tableJob, ui->tableCar);
		}
		if (m_bUseJobPath && m_bUseFmPath)
		{
			buildTableViewConnection(ui->tableJob, ui->tableFm);
			buildTableViewConnection(ui->tableFm, ui->tableJob);
		}

	});

	// TODO
	// 全选Car
	connect(ui->cBox_SelectAll_Car, &QPushButton::toggled, [=](bool bState)
	{
		if (bState)
		{
			PJC_TableModel *model = dynamic_cast<PJC_TableModel *>(ui->tableCar->model());
			for (int idx = 0; idx < model->getModelData().size(); idx++)
			{
				model->setChecked(idx, Qt::Checked, true);
			}
		}
		else
		{
			PJC_TableModel *model = dynamic_cast<PJC_TableModel *>(ui->tableCar->model());
			for (int idx = 0; idx < model->getModelData().size(); idx++)
			{
				model->setChecked(idx, Qt::Unchecked, true);
			}
		}
		ui->tableCar->viewport()->update();
	});

	// 全选Job
	connect(ui->cBox_SelectAll_Job, &QPushButton::toggled, [=](bool bState)
	{
		if (bState)
		{
			PJC_TableModel *model = dynamic_cast<PJC_TableModel *>(ui->tableJob->model());
			for (int idx = 0; idx < model->getModelData().size(); idx++)
			{
				model->setChecked(idx, Qt::Checked, true);
			}
		}
		else
		{
			PJC_TableModel *model = dynamic_cast<PJC_TableModel *>(ui->tableJob->model());
			for (int idx = 0; idx < model->getModelData().size(); idx++)
			{
				model->setChecked(idx, Qt::Unchecked, true);
			}
		}
		ui->tableJob->viewport()->update();
	});

	// 全选Fm
	connect(ui->cBox_SelectAll_Fm, &QPushButton::toggled, [=](bool bState)
	{
		if (bState)
		{
			PJC_TableModel *model = dynamic_cast<PJC_TableModel *>(ui->tableFm->model());
			for (int idx = 0; idx < model->getModelData().size(); idx++)
			{
				model->setChecked(idx, Qt::Checked, true);
			}
		}
		else
		{
			PJC_TableModel *model = dynamic_cast<PJC_TableModel *>(ui->tableFm->model());
			for (int idx = 0; idx < model->getModelData().size(); idx++)
			{
				model->setChecked(idx, Qt::Unchecked, true);
			}
		}
		ui->tableFm->viewport()->update();
	});

	// 按钮选择Car路径
	connect(ui->btn_OpenPathCar, &QPushButton::clicked, [=]()
	{
		//文件夹路径
		m_sCarPath = QFileDialog::getExistingDirectory(this, "Choose Car Path", "\\");
		m_sCarPath.replace("/", "\\");;
		if (m_sCarPath.isEmpty())
		{
			auto MsgBox = new QMessageBox();
			MsgBox->setStyleSheet("QLabel{"
				"min-width:200px;"
				"min-height:40px; "
				"font-size:12px;"
				"}");
			MsgBox->setWindowModality(Qt::NonModal); // 设置为非模态：不阻塞父线程
			MsgBox->setWindowTitle(QString("选择Car文件夹"));
			MsgBox->setText(QString("选中的路径无效！请选择正确的路径"));
			MsgBox->setStandardButtons(QMessageBox::Ok);

			MsgBox->show();
		}
		else
		{
			qDebug() << "srcDirPath=" << m_sCarPath;
			m_sCarPath += "\\";  /*使用\\转义为\*/
		}
		ui->Edit_PathCar->setText(m_sCarPath);
	});

	// 打开设置窗口
	connect(ui->btnSetting, &QPushButton::clicked, [this]()
	{
		if (getCarVersion() == ON_LINE)
		{
			auto winSetting = new PackJobCarSetting_OnLine(this);
			winSetting->show();
			connect(winSetting, &PackJobCarSetting_OnLine::sendConfig, [=](PackCfg_OnLine cfg)
			{
				m_cfg_on_line = cfg;
				m_nNumOfBoardsForPack = *cfg.num_of_boards;
				m_nLowerBoundIdx = *cfg.lower_bound_idx;
				m_nUpperBoundIdx = *cfg.upper_bound_idx;
				m_nPackMode = *m_cfg_off_line.pack_mode;
				//m_bExtrcBoardsAll = cfg.bSelectAll;
			});
		}
		else
		{
			auto winSetting = new PackJobCarSetting_OffLine(this);
			winSetting->show();
			connect(winSetting, &PackJobCarSetting_OffLine::sendConfig, [=](PackCfg_OffLine cfg)
			{
				m_cfg_off_line = cfg;
				m_nNumOfBoardsForPack = *cfg.num_of_boards;
				m_nNumOfPlsForPack = *cfg.num_of_pls;
				m_nPackMode = *cfg.pack_mode;
				m_bExtrcBoardsAll = false;
			});
		}
	});

	// TODO
	// 按钮选择Job路径
	connect(ui->btn_OpenPathJob, &QPushButton::clicked, [=]()
	{
		//文件夹路径
		m_sPathJob = QFileDialog::getExistingDirectory(this, "Choose Job Path", "\\");
		m_sPathJob.replace("/", "\\");
		if (m_sPathJob.isEmpty())
		{
			auto MsgBox = new QMessageBox();
			MsgBox->setStyleSheet("QLabel{"
				"min-width:200px;"
				"min-height:40px; "
				"font-size:12px;"
				"}");
			MsgBox->setWindowModality(Qt::NonModal); // 设置为非模态：不阻塞父线程
			MsgBox->setWindowTitle(QString("选择Job文件夹"));
			MsgBox->setText(QString("选中的路径无效！请选择正确的路径"));
			MsgBox->setStandardButtons(QMessageBox::Ok);

			MsgBox->show(); /*简单显示，适用于非模态*/
		}
		else
		{
			qDebug() << "srcDirPath=" << m_sPathJob;
			m_sPathJob += "\\";
		}
		ui->Edit_PathJob->setText(m_sPathJob);
	});

	// 按钮选择Fm路径
	connect(ui->btn_OpenPathFm, &QPushButton::clicked, [=]()
	{
		//文件夹路径
		m_sPathFm = QFileDialog::getExistingDirectory(this, "Choose Job Path", "\\");
		m_sPathFm.replace("/", "\\");
		if (m_sPathFm.isEmpty())
		{
			auto MsgBox = new QMessageBox();
			MsgBox->setStyleSheet("QLabel{"
				"min-width:200px;"
				"min-height:40px; "
				"font-size:12px;"
				"}");
			MsgBox->setWindowModality(Qt::NonModal); // 设置为非模态：不阻塞父线程
			MsgBox->setWindowTitle("Warning");
			MsgBox->setText(QString("选中的路径无效！请选择正确的路径"));
			MsgBox->setStandardButtons(QMessageBox::Ok);

			MsgBox->show();
		}
		else
		{
			qDebug() << "srcDirPath=" << m_sPathFm;
			m_sPathFm += "\\";
		}
		ui->Edit_PathFm->setText(m_sPathFm);
	});

	// 按钮选择Out路径
	connect(ui->btn_OpenPathOut, &QPushButton::clicked, [=]()
	{
		//文件夹路径
		m_sPathOut = QFileDialog::getExistingDirectory(this, "Choose Out Path", "\\");
		m_sPathOut.replace("/", "\\");
		if (m_sPathOut.isEmpty())
		{
			auto MsgBox = new QMessageBox();
			MsgBox->setStyleSheet("QLabel{"
				"min-width:200px;"
				"min-height:40px; "
				"font-size:12px;"
				"}");
			MsgBox->setWindowModality(Qt::NonModal); // 设置为非模态：不阻塞父线程
			MsgBox->setWindowTitle(QString("选择Fm文件夹"));
			MsgBox->setText(QString("选中的路径无效！请选择正确的路径"));
			MsgBox->setStandardButtons(QMessageBox::Ok);

			MsgBox->show();
		}
		else
		{
			qDebug() << "srcDirPath=" << m_sPathOut;
			m_sPathOut += "\\";
		}
		ui->Edit_PathOut->setText(m_sPathOut);
	});

	// 手动加载Car文件夹
	connect(ui->Edit_PathCar, &QLineEdit::editingFinished, [=]()
	{
		m_sCarPath = ui->Edit_PathCar->text();
		m_sCarPath.replace("/", "\\");
	});

	// 手动加载Job文件夹
	connect(ui->Edit_PathJob, &QLineEdit::editingFinished, [=]()
	{
		m_sPathJob = ui->Edit_PathJob->text();
		m_sPathJob.replace("/", "\\");
	});

	// 手动加载Fm文件夹
	connect(ui->Edit_PathFm, &QLineEdit::editingFinished, [=]()
	{
		m_sPathFm = ui->Edit_PathFm->text();
		m_sPathFm.replace("/", "\\");
	});



	/*隐藏信号*/
	// 更新详细信息
	connect(this, &PackJobCar::sendMsg, this, [&](QString sMsg)
	{
		ui->Details->clear();
		ui->Details->append(sMsg);
	}, Qt::QueuedConnection);

	// 结束后刷新界面
	connect(this, &PackJobCar::sigFinished, this, [&]()
	{
		onParkThreadFinished();
	}, Qt::QueuedConnection);

	// 更新进度标签
	connect(this, &PackJobCar::sendPgLabel, this, [&](int nMode)
	{
		onPgLabelUpdated(nMode);
	}, Qt::QueuedConnection);

	// 更新进度条动画
	connect(this, &PackJobCar::sendPgValue, this, [&](int nValue)
	{
		onPgValueUpdated(nValue);
	}, Qt::QueuedConnection);
}

PackJobCar::~PackJobCar()
{
	delete ui;
}

void PackJobCar::loadIni()
{
	m_pConfig = new QSettings(QCoreApplication::applicationDirPath() +
	                          "/Config.ini", QSettings::IniFormat);
	m_pConfig->setIniCodec("UTF-8");

	m_pConfig->beginGroup("PackJobCar");

	m_sCarPath = m_pConfig->value("car_path").toString().replace("/", "\\");
	m_sPathJob = m_pConfig->value("job_path").toString().replace("/", "\\");
	m_sPathFm = m_pConfig->value("fm_path").toString().replace("/", "\\");
	m_sPathOut = m_pConfig->value("out_path").toString().replace("/", "\\");

	m_nNumOfBoardsForPack = m_pConfig->value("num_of_boards").toInt();
	m_nNumOfPlsForPack = m_pConfig->value("num_of_pls").toInt();

	m_bUseCarPath = m_pConfig->value("pack_car").toBool();
	m_bUseJobPath = m_pConfig->value("pack_job").toBool();
	m_bUseFmPath = m_pConfig->value("pack_fm").toBool();

	m_pConfig->endGroup();

	ui->Edit_PathCar->setText(m_sCarPath);
	ui->Edit_PathJob->setText(m_sPathJob);
	ui->Edit_PathFm->setText(m_sPathFm);
	ui->Edit_PathOut->setText(m_sPathOut);
}

void PackJobCar::saveIni()
{
	m_sCarPath = ui->Edit_PathCar->text();
	m_sPathJob = ui->Edit_PathJob->text();
	m_sPathFm = ui->Edit_PathFm->text();
	m_sPathOut = ui->Edit_PathOut->text();

	m_bUseCarPath = ui->label_CarPath->isChecked();
	m_bUseJobPath = ui->label_JobPath->isChecked();
	m_bUseFmPath = ui->label_FmPath->isChecked();

	m_pConfig->beginGroup("PackJobCar");

	m_pConfig->setValue("car_path", m_sCarPath);
	m_pConfig->setValue("job_path", m_sPathJob);
	m_pConfig->setValue("fm_path", m_sPathFm);
	m_pConfig->setValue("out_path", m_sPathOut);

	m_pConfig->setValue("num_of_boards", m_nNumOfBoardsForPack);
	m_pConfig->setValue("num_of_pls", m_nNumOfPlsForPack);

	m_pConfig->setValue("pack_car", m_bUseCarPath);
	m_pConfig->setValue("pack_job", m_bUseJobPath);
	m_pConfig->setValue("pack_fm", m_bUseFmPath);

	m_pConfig->endGroup();
}

/**
 * \brief 加载SubDir到互动窗口
 * \param sRootPath SubDir的绝对路径
 */
void PackJobCar::loadTableView(QString sRootPath, QTableView* pTableView, int nMode)
{
	QStringList lstSubDirs;
	if (nMode == CAR_MODE)
		lstSubDirs = getSubDirs(sRootPath, true);
	else
		lstSubDirs = getSubDirs(sRootPath, false);

	lstSubDirs.removeDuplicates();

	QFont font;
	font.setFamily(QStringLiteral("Microsoft YaHei UI"));
	font.setPointSize(9);

	if (lstSubDirs.size() != 0)
	{
		QList<ModelItem> data;
		for (int i = 0; i < lstSubDirs.size(); i++)
		{
			QString sPath = lstSubDirs.at(i);
			QFileInfo fileInfo(sPath);
			auto row = new ModelItem;
			row->sName = fileInfo.baseName();

			if (!fileInfo.lastModified().isValid())
			{
				fileInfo = QFileInfo(sPath + ".bot");
			}

			row->qdtLastModify = fileInfo.lastModified();
			row->nCheckedState = Qt::Unchecked;
			data.append(*row);
		}

		auto model = new PJC_TableModel(pTableView);
		model->setModelData(data);

		auto cboxDelegate = new CheckBoxDelegate();
		pTableView->setItemDelegateForColumn(0, cboxDelegate);
		pTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
		pTableView->setModel(model); // 设置视图的模型
		pTableView->resize(237, 597);
		pTableView->resizeColumnsToContents();
		pTableView->setFont(font); // 设置字体
		pTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		pTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

		pTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
		pTableView->horizontalHeader()->setStretchLastSection(true);
		pTableView->horizontalHeader()->setHighlightSections(false);

		pTableView->verticalHeader()->setVisible(false); // 隐藏行索引
		pTableView->setFrameShape(QFrame::NoFrame);

		pTableView->setSortingEnabled(true); // 启用排序功能

		QHeaderView* header = pTableView->horizontalHeader();
		header->setSortIndicator(0, Qt::AscendingOrder);
		//header->setSectionResizeMode(QHeaderView::Fixed);

		setScrollBarStyle(pTableView->verticalScrollBar());
		setScrollBarStyle(pTableView->horizontalScrollBar());


		switch (nMode)
		{
		case CAR_MODE:
			m_pModelCar = model;
			break;
		case JOB_MODE:
			m_pModelJob = model;
			break;
		case FM_MODE:
			m_pModelFm = model;
			break;
		}
	}
}

/**
 * \brief 添加TableView之间的连接关系
 */
void PackJobCar::buildTableViewConnection(QTableView* signal, QTableView* slot)
{
	PJC_TableModel *signalModel = dynamic_cast<PJC_TableModel *>(signal->model());
	PJC_TableModel *slotModel = dynamic_cast<PJC_TableModel *>(slot->model());

	connect(signalModel, &PJC_TableModel::checkBoxChanged, [=](const QModelIndex &index)
	{
		QList<ModelItem> signalModelData = signalModel->getModelData();
		QList<ModelItem> slotModelData = slotModel->getModelData();

		QString signalName = signalModelData.at(index.row()).sName;
		Qt::CheckState nCheckedState = signalModelData.at(index.row()).nCheckedState;

		int slotIdx = slotModel->getItemIdx(signalName);
		if (slotIdx != -1)
		{
			slotModel->setChecked(slotIdx, nCheckedState);
		}

		slot->viewport()->update();
	});

}

/**
 * \brief 开启打包线程
 */
void PackJobCar::startParkThread()
{
	ui->pgBar->setVisible(true);
	ui->pgBar->setValue(0);

	m_sCarPath = ui->Edit_PathCar->text();
	m_sPathJob = ui->Edit_PathJob->text();
	m_sPathFm = ui->Edit_PathFm->text();
	m_sPathOut = ui->Edit_PathOut->text();

	m_sCarPath.replace("/", "\\");
	m_sPathJob.replace("/", "\\");
	m_sPathFm.replace("/", "\\");
	m_sPathOut.replace("/", "\\");

	/*提示信息*/
	if (m_sPathOut.size() == 0)
	{
		auto MsgBox = new QMessageBox();
		MsgBox->setStyleSheet("QLabel{"
			"min-width:200px;"
			"min-height:40px; "
			"font-size:12px;"
			"}");
		MsgBox->setWindowModality(Qt::NonModal); // 设置为非模态：不阻塞父线程
		MsgBox->setWindowTitle("Warning");
		MsgBox->setText(QString("没有设置导出路径，请选择！"));
		MsgBox->setStandardButtons(QMessageBox::Ok);

		MsgBox->show();

		return;
	}

	if (m_sPathOut.right(1) != "\\")
	{
		m_sPathOut += "\\";
	}

	// 开启临时线程
	QFuture<void> future = QtConcurrent::run([=]()
	{
		// 开始打包
		if (m_bUseCarPath)
		{
			/*准备参数*/
			m_dCurrPgValue = 0;
			m_nCountFiles = 0;
			if (getCarVersion() == OFF_LINE)
			{
				packCar_OffLine(); // 打包Car资料
			}
			else
			{
				packCar_OnLine();
			}
		}

		if (m_bUseJobPath)
		{
			m_dCurrPgValue = 0;
			m_nCountFiles = 0;
			packJob();
		}

		if (m_bUseFmPath)
		{
			m_dCurrPgValue = 0;
			m_nCountFiles = 0;
			packFm();
		}

		emit sigFinished();
		emit sendPgLabel(-1);
	});
}

/**
 * \brief 打包Car资料
 */
void PackJobCar::packCar_OffLine()
{
	emit sendPgValue(0); // 重置进度条
	emit sendPgLabel(CAR_MODE);

	QStringList lstCheckedCar = getCheckedSubDirs(CAR_MODE);
	QStringList lstSrc, lstDst;

	/*目标位置和源位置的根目录*/
	foreach(QString car, lstCheckedCar)
	{
		if (m_sCarPath.right(1) == "\\")
		{
			lstSrc.append(m_sCarPath + car);
			lstDst.append(m_sPathOut + "car\\" + car);
		}
		else
		{
			lstSrc.append(m_sCarPath + "\\" + car);
			lstDst.append(m_sPathOut + "car\\" + car);
		}
	}

	// 获得子文件夹下最近的几个批量，缺失不补
	QStringList lstWantedPls;
	for (long i = 0; i < lstSrc.size(); i++)
	{
		QString sDirSrc = lstSrc.at(i);//访问第i个下标文件
		QString sDirDst = lstDst.at(i);//输出的第i个下标文件
		QDir dir(sDirSrc);
		QFileInfoList lstPls = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Time);
		int nCount = 0;

		std::random_device rd;
		std::mt19937 g(rd());
		switch (m_nPackMode)
		{
		case PACKMODE_RANDOM:
			std::shuffle(lstPls.begin(), lstPls.end(), g);
			break;
		case PACKMODE_TIMED_SEQUENTIAL:
			std::sort(lstPls.begin(), lstPls.end(), [=]
		          (const QFileInfo f1, const QFileInfo f2)
			          {
				          return f1.lastModified() > f2.lastModified();
			          });
			break;
		case PACKMODE_TIMED_REVERSE:
			std::sort(lstPls.begin(), lstPls.end(), [=]
		          (const QFileInfo f1, const QFileInfo f2)
			          {
				          return f1.lastModified() < f2.lastModified();
			          });
			break;
		}

		foreach(auto Pl, lstPls)
		{
			if (nCount >= m_nNumOfPlsForPack)
			{
				break;
			}
			lstWantedPls.append(Pl.absoluteFilePath().replace("/", "\\"));
			nCount++;
		}
	}

	// 过滤板号
	QStringList lstWantedBoards;
	// 获得每个批量下最近或随机的几个板
	if (!m_bExtrcBoardsAll)
	{
		QStringList filters;
		filters << "*_b" << "*_t" << "*_ai" << "*_Statis" << "*_True" << "*_Missing";
		switch (m_nPackMode)
		{
		case PACKMODE_RANDOM:
			for (long i = 0; i < lstWantedPls.size(); i++)
			{
				QString sPlPath = lstWantedPls.at(i);
				QDir dir(sPlPath);

				QFileInfoList lstBoards = dir.entryInfoList(filters, QDir::Files);
				QStringList lstBoardNums;
				foreach(auto board, lstBoards)
				{
					QString fileName = board.baseName();//返回文件基名，第一个"."之前的文件名
					lstBoardNums.append(fileName.split("_").at(0));//以_为分隔符？
				}
				lstBoardNums.removeDuplicates();//删除重复项

				int count = 0;
				foreach(auto num, lstBoardNums)
				{
					if (count == m_nNumOfBoardsForPack)
					{
						break;
					}
					count++;
					lstWantedBoards.append(sPlPath + "\\" + num + "_b");
					lstWantedBoards.append(sPlPath + "\\" + num + "_t");
					lstWantedBoards.append(sPlPath + "\\" + num + "_b_ai");
					lstWantedBoards.append(sPlPath + "\\" + num + "_t_ai");
					lstWantedBoards.append(sPlPath + "\\" + num + "_b_True");
					lstWantedBoards.append(sPlPath + "\\" + num + "_t_True");
					lstWantedBoards.append(sPlPath + "\\" + num + "_b_Missing");
					lstWantedBoards.append(sPlPath + "\\" + num + "_t_Missing");
					lstWantedBoards.append(sPlPath + "\\" + num + "_b_Statis");
					lstWantedBoards.append(sPlPath + "\\" + num + "_t_Statis");
					lstWantedBoards.append(sPlPath + "\\" + num + "\\A.vrs");
					lstWantedBoards.append(sPlPath + "\\" + num + "\\B.vrs");
					lstWantedBoards.append(sPlPath + "\\" + num + "\\A.txt");
					lstWantedBoards.append(sPlPath + "\\" + num + "\\B.txt");
				}
			}
			break;
		case PACKMODE_TIMED_SEQUENTIAL:
			for (long i = 0; i < lstWantedPls.size(); i++)
			{
				QString sPlPath = lstWantedPls.at(i);
				QDir dir(sPlPath);
				QFileInfoList lstBoards = dir.entryInfoList(filters, QDir::Files, QDir::Time);
				QStringList lstBoardNums;
				foreach(auto board, lstBoards)
				{
					QString fileName = board.baseName();
					lstBoardNums.append(fileName.split("_").at(0));
				}
				lstBoardNums.removeDuplicates();
				int count = 0;
				foreach(auto num, lstBoardNums)
				{
					if (count == m_nNumOfBoardsForPack)
					{
						break;
					}
					count++;
					lstWantedBoards.append(sPlPath + "\\" + num + "_b");
					lstWantedBoards.append(sPlPath + "\\" + num + "_t");
					lstWantedBoards.append(sPlPath + "\\" + num + "_b_ai");
					lstWantedBoards.append(sPlPath + "\\" + num + "_t_ai");
					lstWantedBoards.append(sPlPath + "\\" + num + "_b_True");
					lstWantedBoards.append(sPlPath + "\\" + num + "_t_True");
					lstWantedBoards.append(sPlPath + "\\" + num + "_b_Missing");
					lstWantedBoards.append(sPlPath + "\\" + num + "_t_Missing");
					lstWantedBoards.append(sPlPath + "\\" + num + "_b_Statis");
					lstWantedBoards.append(sPlPath + "\\" + num + "_t_Statis");
					lstWantedBoards.append(sPlPath + "\\" + num + "\\A.vrs");
					lstWantedBoards.append(sPlPath + "\\" + num + "\\B.vrs");
					lstWantedBoards.append(sPlPath + "\\" + num + "\\A.txt");
					lstWantedBoards.append(sPlPath + "\\" + num + "\\B.txt");
				}
			}
			break;
		case PACKMODE_TIMED_REVERSE:
			for (long i = 0; i < lstWantedPls.size(); i++)
			{
				QString sPlPath = lstWantedPls.at(i);
				QDir dir(sPlPath);
				QFileInfoList lstBoards = dir.entryInfoList(filters, QDir::Files, QDir::Time);
				QStringList lstBoardNums;
				foreach(auto board, lstBoards)
				{
					QString fileName = board.baseName();
					lstBoardNums.append(fileName.split("_").at(0));
				}
				lstBoardNums.removeDuplicates();
				std::reverse(lstBoardNums.begin(), lstBoardNums.end());
				int count = 0;
				foreach(auto num, lstBoardNums)
				{
					if (count == m_nNumOfBoardsForPack)
					{
						break;
					}
					count++;
					lstWantedBoards.append(sPlPath + "\\" + num + "_b");
					lstWantedBoards.append(sPlPath + "\\" + num + "_t");
					lstWantedBoards.append(sPlPath + "\\" + num + "_b_ai");
					lstWantedBoards.append(sPlPath + "\\" + num + "_t_ai");
					lstWantedBoards.append(sPlPath + "\\" + num + "_b_True");
					lstWantedBoards.append(sPlPath + "\\" + num + "_t_True");
					lstWantedBoards.append(sPlPath + "\\" + num + "_b_Missing");
					lstWantedBoards.append(sPlPath + "\\" + num + "_t_Missing");
					lstWantedBoards.append(sPlPath + "\\" + num + "_b_Statis");
					lstWantedBoards.append(sPlPath + "\\" + num + "_t_Statis");
					lstWantedBoards.append(sPlPath + "\\" + num + "\\A.vrs");
					lstWantedBoards.append(sPlPath + "\\" + num + "\\B.vrs");
					lstWantedBoards.append(sPlPath + "\\" + num + "\\A.txt");
					lstWantedBoards.append(sPlPath + "\\" + num + "\\B.txt");
				}
			}
			break;
		}
	}
	// 获得每个批量下的所有板
	else
	{
		for (long i = 0; i < lstWantedPls.size(); i++)
		{
			QString sPlPath = lstWantedPls.at(i);
			QDir dir(sPlPath);  //设置当前目录(批量)

			QStringList filter;
			filter << "*_b*" << "*_t*" << "*.txt" << "*.vrs";

			QFileInfoList lstBoards = dir.entryInfoList(filter, QDir::Files | QDir::NoDotAndDotDot);
			lstBoards.append(dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot));
			for (auto file_info : lstBoards)
			{
				// 取VRS
				if (file_info.isDir())
				{
					QString vrs_path = file_info.absoluteFilePath();

					if (vrs_path.contains(QString("AIRes_show")) || 
						vrs_path.contains(QString("AIRes")))
					{
						continue;
					}

					QDir dir_tmp(vrs_path);
					QFileInfoList lstVrs = dir_tmp.entryInfoList(filter, QDir::Files | QDir::NoDotAndDotDot);
					for (auto file_info_vrs : lstVrs)
					{
						lstWantedBoards.append(file_info_vrs.absoluteFilePath());
					}
					continue;
				}

				// 取_b/_t文件
				lstWantedBoards.append(file_info.absoluteFilePath());
			}
		}
	}

	// 去除可能不存在的文件
	for (int i = 0; i < lstWantedBoards.size(); i++)
	{
		if (!QFile::exists(lstWantedBoards.at(i)))
		{
			// std::cout << lstWantedBoards.at(i).toStdString() << std::endl;
			lstWantedBoards.removeAt(i);
			i--;
		}
		else
		{
			lstWantedBoards[i] = lstWantedBoards[i].replace("/", "\\");
		}
	}

	m_nCountFiles += lstWantedBoards.size();
	// 复制文件
	for (long i = 0; i < lstSrc.size(); i++)
	{
		QString sDirSrc = lstSrc.at(i);
		QString sDirDst = lstDst.at(i);

		copyDirWithFilter(lstWantedPls, lstWantedBoards, sDirSrc, sDirDst, true);
	}
}

/**
 * \brief 打包Car资料（在线板）
 */
void PackJobCar::packCar_OnLine()
{
	emit sendPgValue(0); // 重置进度条
	emit sendPgLabel(CAR_MODE);

	QStringList lstCheckedPls = getCheckedSubDirs(CAR_MODE);
	QStringList lstSrc, lstDst;

	/*目标位置和源位置的根目录*/
	foreach(QString pl, lstCheckedPls)
	{
		if (m_sCarPath.right(1) == "\\")
		{
			lstSrc.append(m_sCarPath + pl);
			lstDst.append(m_sPathOut + "car\\" + pl);
		}
		else
		{
			lstSrc.append(m_sCarPath + "\\" + pl);
			lstDst.append(m_sPathOut + "car\\" + pl);
		}
	}

	// 获得每个批量下最近或随机的几个板
	QStringList lstWantedBoards;
	// 如果是
	if (!m_bExtrcBoardsAll)
	{
		QStringList filters;
		filters << "*_b" << "*_t" << "*_ai" << "*_Statis" << "*_True" << "*_Missing";
		if (m_lstNumConstraint.size() == 0){
			switch (m_nPackMode)
			{
			case PACKMODE_RANDOM:
				for (long i = 0; i < lstCheckedPls.size(); i++)
				{
					QString sPlPath = lstCheckedPls.at(i);
					QDir dir(sPlPath);

					QFileInfoList lstBoards = dir.entryInfoList(filters, QDir::Files);
					QStringList lstBoardNums;
					foreach(auto board, lstBoards)
					{
						QString fileName = board.baseName();
						lstBoardNums.append(fileName.split("_").at(0));
					}
					lstBoardNums.removeDuplicates();

					int count = 0;
					foreach(auto num, lstBoardNums)
					{
						if (count == m_nNumOfBoardsForPack)
						{
							break;
						}
						count++;
						lstWantedBoards.append(sPlPath + "\\" + num + "_b");
						lstWantedBoards.append(sPlPath + "\\" + num + "_t");
						lstWantedBoards.append(sPlPath + "\\" + num + "_b_ai");
						lstWantedBoards.append(sPlPath + "\\" + num + "_t_ai");
						lstWantedBoards.append(sPlPath + "\\" + num + "_b_True");
						lstWantedBoards.append(sPlPath + "\\" + num + "_t_True");
						lstWantedBoards.append(sPlPath + "\\" + num + "_b_Missing");
						lstWantedBoards.append(sPlPath + "\\" + num + "_t_Missing");
						lstWantedBoards.append(sPlPath + "\\" + num + "_b_Statis");
						lstWantedBoards.append(sPlPath + "\\" + num + "_t_Statis");
						lstWantedBoards.append(sPlPath + "\\" + num + "\\A.vrs");
						lstWantedBoards.append(sPlPath + "\\" + num + "\\B.vrs");
						lstWantedBoards.append(sPlPath + "\\" + num + "\\A.txt");
						lstWantedBoards.append(sPlPath + "\\" + num + "\\B.txt");
					}
				}
				break;
			case PACKMODE_TIMED_SEQUENTIAL:
				for (long i = 0; i < lstCheckedPls.size(); i++)
				{
					QString sPlPath = lstCheckedPls.at(i);
					QDir dir(sPlPath);
					QFileInfoList lstBoards = dir.entryInfoList(filters, QDir::Files, QDir::Time);
					QStringList lstBoardNums;
					foreach(auto board, lstBoards)
					{
						QString fileName = board.baseName();
						lstBoardNums.append(fileName.split("_").at(0));
					}
					lstBoardNums.removeDuplicates();
					int count = 0;
					foreach(auto num, lstBoardNums)
					{
						if (count == m_nNumOfBoardsForPack)
						{
							break;
						}
						count++;
						lstWantedBoards.append(sPlPath + "\\" + num + "_b");
						lstWantedBoards.append(sPlPath + "\\" + num + "_t");
						lstWantedBoards.append(sPlPath + "\\" + num + "_b_ai");
						lstWantedBoards.append(sPlPath + "\\" + num + "_t_ai");
						lstWantedBoards.append(sPlPath + "\\" + num + "_b_True");
						lstWantedBoards.append(sPlPath + "\\" + num + "_t_True");
						lstWantedBoards.append(sPlPath + "\\" + num + "_b_Missing");
						lstWantedBoards.append(sPlPath + "\\" + num + "_t_Missing");
						lstWantedBoards.append(sPlPath + "\\" + num + "_b_Statis");
						lstWantedBoards.append(sPlPath + "\\" + num + "_t_Statis");
						lstWantedBoards.append(sPlPath + "\\" + num + "\\A.vrs");
						lstWantedBoards.append(sPlPath + "\\" + num + "\\B.vrs");
						lstWantedBoards.append(sPlPath + "\\" + num + "\\A.txt");
						lstWantedBoards.append(sPlPath + "\\" + num + "\\B.txt");
					}
				}
				break;
			case PACKMODE_TIMED_REVERSE:
				for (long i = 0; i < lstCheckedPls.size(); i++)
				{
					QString sPlPath = lstCheckedPls.at(i);
					QDir dir(sPlPath);
					QFileInfoList lstBoards = dir.entryInfoList(filters, QDir::Files, QDir::Time);
					QStringList lstBoardNums;
					foreach(auto board, lstBoards)
					{
						QString fileName = board.baseName();
						lstBoardNums.append(fileName.split("_").at(0));
					}
					lstBoardNums.removeDuplicates();
					std::reverse(lstBoardNums.begin(), lstBoardNums.end());
					int count = 0;
					foreach(auto num, lstBoardNums)
					{
						if (count == m_nNumOfBoardsForPack)
						{
							break;
						}
						count++;
						lstWantedBoards.append(sPlPath + "\\" + num + "_b");
						lstWantedBoards.append(sPlPath + "\\" + num + "_t");
						lstWantedBoards.append(sPlPath + "\\" + num + "_b_ai");
						lstWantedBoards.append(sPlPath + "\\" + num + "_t_ai");
						lstWantedBoards.append(sPlPath + "\\" + num + "_b_True");
						lstWantedBoards.append(sPlPath + "\\" + num + "_t_True");
						lstWantedBoards.append(sPlPath + "\\" + num + "_b_Missing");
						lstWantedBoards.append(sPlPath + "\\" + num + "_t_Missing");
						lstWantedBoards.append(sPlPath + "\\" + num + "_b_Statis");
						lstWantedBoards.append(sPlPath + "\\" + num + "_t_Statis");
						lstWantedBoards.append(sPlPath + "\\" + num + "\\A.vrs");
						lstWantedBoards.append(sPlPath + "\\" + num + "\\B.vrs");
						lstWantedBoards.append(sPlPath + "\\" + num + "\\A.txt");
						lstWantedBoards.append(sPlPath + "\\" + num + "\\B.txt");
					}
				}
				break;
			}
		}
		else
		{
			int nMaxNum = MAX_NUM(m_lstNumConstraint);
			int nMinNum = MIN_NUM(m_lstNumConstraint);
		}
	}
	// 获得每个批量下的所有板
	else
	{
		for (long i = 0; i < lstSrc.size(); i++)
		{
			QString sPlPath = lstSrc.at(i);
			QDir dir(sPlPath);

			QStringList filter;
			filter << "*_b*" << "*_t*" << "*.txt" << "*.vrs";

			QFileInfoList lstBoards = dir.entryInfoList(filter, QDir::Files | QDir::NoDotAndDotDot);
			lstBoards.append(dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot));

			int nMaxNum = MAX_NUM(m_lstNumConstraint);
			int nMinNum = MIN_NUM(m_lstNumConstraint);

			for (auto file_info : lstBoards)
			{
				// 取VRS
				if (file_info.isDir())
				{
					QString vrs_path = file_info.absoluteFilePath();

					if (vrs_path.contains(QString("AIRes_show")) ||
						vrs_path.contains(QString("AIRes")))
					{
						continue;
					}

					QDir dir_tmp(vrs_path);
					QFileInfoList lstVrs = dir_tmp.entryInfoList(filter, QDir::Files | QDir::NoDotAndDotDot);
					for (auto file_info_vrs : lstVrs)
					{
						lstWantedBoards.append(file_info_vrs.absoluteFilePath());
					}
					continue;
				}

				// 取_b/_t文件
				lstWantedBoards.append(file_info.absoluteFilePath());
			}


		}
	}

	// 去除可能不存在的文件
	for (int i = 0; i < lstWantedBoards.size(); i++)
	{
		if (!QFile::exists(lstWantedBoards.at(i)))
		{
			// std::cout << lstWantedBoards.at(i).toStdString() << std::endl;
			lstWantedBoards.removeAt(i);
			i--;
		}
		else
		{
			lstWantedBoards[i] = lstWantedBoards[i].replace("/", "\\");
		}
	}

	// 去除过滤器之外的文件

	m_nCountFiles += lstWantedBoards.size();
	// 复制文件
	for (long i = 0; i < lstSrc.size(); i++)
	{
		QString sDirSrc = lstSrc.at(i);
		QString sDirDst = lstDst.at(i);

		copyDirWithFilter(lstCheckedPls, lstWantedBoards, sDirSrc, sDirDst, true);
	}
}

/**
 * \brief 打包Job资料
 */
void PackJobCar::packJob()
{
	emit sendPgValue(0);
	emit sendPgLabel(JOB_MODE);

	QStringList lstCheckedJob = getCheckedSubDirs(JOB_MODE);
	QStringList lstSrc, lstDst;


	foreach(QString job, lstCheckedJob)
	{
		if (m_sPathJob.right(1) == "\\")
		{
			lstSrc.append(m_sPathJob + job);
			lstSrc.append(m_sPathJob + job + ".bot");
			lstSrc.append(m_sPathJob + job + ".top");
			lstDst.append(m_sPathOut + "job\\" + job);
			lstDst.append(m_sPathOut + "job\\" + job + ".bot");
			lstDst.append(m_sPathOut + "job\\" + job + ".top");
			QString sTmp = m_sPathOut + "job\\";
			if (!QDir(sTmp).exists())
			{
				QDir(sTmp).mkdir(sTmp);
			}
		}
		else
		{
			lstSrc.append(m_sPathJob + "\\" + job);
			lstSrc.append(m_sPathJob + "\\" + job + ".bot");
			lstSrc.append(m_sPathJob + "\\" + job + ".top");
			lstDst.append(m_sPathOut + "\\job\\" + job);
			lstDst.append(m_sPathOut + "\\job\\" + job + ".bot");
			lstDst.append(m_sPathOut + "\\job\\" + job + ".top");
			QString sTmp = m_sPathOut + "\\job\\";
			if (!QDir(sTmp).exists())
			{
				QDir(sTmp).mkdir(sTmp);
			}
		}
	}

	// 去除可能不存在的文件
	for (int i = 0; i < lstSrc.size(); i++)
	{
		if (!QFile::exists(lstSrc.at(i)))
		{
			// std::cout << lstWantedBoards.at(i).toStdString() << std::endl;
			lstSrc.removeAt(i);
			lstDst.removeAt(i);
			i--;
		}
		else
		{
			lstSrc[i] = lstSrc[i].replace("/", "\\");
			QFileInfo fileInfo(lstSrc[i]);
			if (fileInfo.isFile())
			{
				m_nCountFiles += 1;
			}
			else
			{
				m_nCountFiles += getNumOfFiles(lstSrc[i]);
			}
		}
	}

	// 复制文件
	for (long i = 0; i < lstSrc.size(); i++)
	{
		QString sDirSrc = lstSrc.at(i);
		QString sDirDst = lstDst.at(i);

		copyDirWithoutFilter(sDirSrc, sDirDst, true);
	}
}

/**
 * \brief 打包Fm资料
 */
void PackJobCar::packFm()
{
	emit sendPgValue(0);
	emit sendPgLabel(FM_MODE);

	QStringList lstCheckedFm = getCheckedSubDirs(FM_MODE);
	QStringList lstSrc, lstDst;

	foreach(QString fm, lstCheckedFm)
	{
		if (m_sPathFm.right(1) == "\\")
		{
			lstSrc.append(m_sPathFm + fm);
			lstDst.append(m_sPathOut + "fm\\" + fm);
			QString sTmp = m_sPathOut + "fm\\";
			if (!QDir(sTmp).exists())
			{
				QDir(sTmp).mkdir(sTmp);
			}
		}
		else
		{
			lstSrc.append(m_sPathFm + "\\" + fm);
			lstDst.append(m_sPathOut + "\\fm\\" + fm);
			QString sTmp = m_sPathOut + "\\fm\\";
			if (!QDir(sTmp).exists())
			{
				QDir(sTmp).mkdir(sTmp);
			}
		}
	}

	// 获取需要移动的【文件】的数量
	for (long i = 0; i < lstSrc.size(); i++)
	{
		QString sDirSrc = lstSrc.at(i);
		QString sDirDst = lstDst.at(i);
		QFileInfo tmpFileInfo(sDirSrc);
		if (tmpFileInfo.isFile())
		{
			// 根目录一级的文件
			m_nCountFiles += 1;
		}
	}

	// 去除可能不存在的文件
	for (int i = 0; i < lstSrc.size(); i++)
	{
		if (!QFile::exists(lstSrc.at(i)))
		{
			// std::cout << lstWantedBoards.at(i).toStdString() << std::endl;
			lstSrc.removeAt(i);
			lstDst.removeAt(i);
			i--;
		}
		else
		{
			lstSrc[i] = lstSrc[i].replace("/", "\\");
			QFileInfo fileInfo(lstSrc[i]);
			if (fileInfo.isFile())
			{
				m_nCountFiles += 1;
			}
			else
			{
				m_nCountFiles += getNumOfFiles(lstSrc[i]);
			}
		}
	}

	// 复制文件
	for (long i = 0; i < lstSrc.size(); i++)
	{
		QString sDirSrc = lstSrc.at(i);
		QString sDirDst = lstDst.at(i);

		copyDirWithoutFilter(sDirSrc, sDirDst, true);
	}
}



/**
 * \brief 拷贝路径到指定文件夹（无过滤）
 * \param sDirSrcRoot 源文件根目录
 * \param sDirDstRoot 目标文件根目录
 * \param bCanOverWrite 是否重写目标文件
 * \return 
 */
bool PackJobCar::copyDirWithoutFilter(QString sDirSrcRoot, QString sDirDstRoot,
                                      bool bCanOverWrite)
{
	QDir dirSrc(sDirSrcRoot);
	QDir dirDst(sDirDstRoot);
	QFileInfo fileInfo(sDirSrcRoot);
	if (!fileInfo.exists())
	{
		return false;
	}

	if (!bCanOverWrite && !dirDst.mkpath(sDirDstRoot))
	{
		return false;
	}

	if (bCanOverWrite)
	{
		dirDst.removeRecursively();
	}

	if (!fileInfo.isFile())
	{
		// 如果是文件夹，则需要创建目标位置
		dirSrc.mkpath(sDirDstRoot);
	}
	else
	{
		// 否则直接拷贝
		QFile::copy(sDirSrcRoot, sDirDstRoot);
		QString sMsg = QString("Copying %1 to %2...").arg(sDirSrcRoot).arg(sDirDstRoot);
		emit sendMsg(sMsg);

		m_dCurrPgValue += static_cast<double>(1) / static_cast<double>(m_nCountFiles);
		emit sendPgValue(static_cast<int>(m_dCurrPgValue * 100));
		QThread::msleep(10);
	}

	foreach(QString sDirSrcName, dirSrc.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
	{
		QString sDirDstPath = sDirDstRoot + "\\" + sDirSrcName;
		QString sDirSrcPath = sDirSrcRoot + "\\" + sDirSrcName;

		dirSrc.mkpath(sDirDstPath);
		copyDirWithoutFilter(sDirSrcRoot + "\\" + sDirSrcName, sDirDstPath, bCanOverWrite);
	}

	foreach(QString fileName, dirSrc.entryList(QDir::Files))
	{
		QString sFileSrc = sDirSrcRoot + "\\" + fileName;
		QString sFileDst = sDirDstRoot + "\\" + fileName;

		QFile::copy(sFileSrc, sFileDst);

		QString sMsg = QString("Copying %1 to %2...").arg(sFileSrc).arg(sFileDst);
		emit sendMsg(sMsg);

		m_dCurrPgValue += static_cast<double>(1) / static_cast<double>(m_nCountFiles);
		emit sendPgValue(static_cast<int>(m_dCurrPgValue * 100));
		QThread::msleep(10);
	}
}

/**
 * \brief  复制资料文件夹到目标路径（会过滤）
 * \param lstWantedPls  待拷贝的料号
 * \param lstWantedBoards  待拷贝的板号
 * \param sDirSrcRoot  原Car文件夹
 * \param sDirDstRoot  目标文件夹
 * \param bCanOverWrite 是否可以覆盖目标文件夹
 * \return 
 */
bool PackJobCar::copyDirWithFilter(QStringList lstWantedPls, QStringList lstWantedBoards,
                                   QString sDirSrcRoot, QString sDirDstRoot,
                                   bool bCanOverWrite)
{
	QDir dirSrc(sDirSrcRoot);

	if (!dirSrc.exists())
	{
		return false;
	}

	QDir dirDst(sDirDstRoot);

	if (!bCanOverWrite && !dirDst.mkpath(sDirDstRoot))
	{
		return false;
	}

	if (bCanOverWrite)
	{
		dirDst.removeRecursively();
	}

	foreach(QString sDirSrcName, dirSrc.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
	{
		QString sDirDstPath = sDirDstRoot + "\\" + sDirSrcName;
		QString sDirSrcPath = sDirSrcRoot + "\\" + sDirSrcName;

		// 输进来的是批量号目录，判断是否要进板一级的目录
		bool flag = false;
		for (QString pth : lstWantedPls)
		{
			if (sDirSrcPath.contains(pth) || pth.contains(sDirSrcPath) || pth == sDirSrcPath)
			{
				flag = true;
				if (QFileInfo(sDirDstRoot).isDir())
				{
					dirSrc.mkpath(sDirDstRoot);
				}
				break;
			}
		}

		if (flag)
		{
			dirSrc.mkpath(sDirDstPath);
			copyDirWithFilter(lstWantedPls, lstWantedBoards, sDirSrcRoot + "\\" + sDirSrcName, sDirDstPath,
			                  bCanOverWrite);
		}
	}

	foreach(QString fileName, dirSrc.entryList(QDir::Files))
	{
		QString sFileSrc = sDirSrcRoot + "\\" + fileName;
		QString sFileDst = sDirDstRoot + "\\" + fileName;
		auto dir = new QDir(sFileDst); //D:/Project/Core
		dir->cdUp();

		if (!lstWantedBoards.contains(sFileSrc))
		{
			dir->remove(dir->path());
			continue;
		}

		if (QFileInfo(dir->path()).isDir())
		{
			dir->mkpath(dir->path());
		}
		else
		{
			dir->mkpath(QFileInfo(dir->path()).absolutePath());
		}
		QFile::copy(sFileSrc, sFileDst);

		QString sMsg = QString("Copying %1 to %2...").arg(sFileSrc).arg(sFileDst);
		emit sendMsg(sMsg);

		m_dCurrPgValue += static_cast<double>(1) / static_cast<double>(m_nCountFiles);
		emit sendPgValue(static_cast<int>(m_dCurrPgValue * 100));
		QThread::msleep(10);
	}

	/*! Possible race-condition mitigation? */
	QDir finalDestination(sDirDstRoot);
	finalDestination.refresh();

	if (finalDestination.exists())
	{
		return true;
	}

	return false;
}

void PackJobCar::onParkThreadFinished()
{
	saveIni();
	setUiStateDuringThread(true);
	ui->Details->clear();

	auto MsgBox = new QMessageBox();
	MsgBox->setStyleSheet("QLabel{"
		"min-width:200px;"
		"min-height:40px; "
		"font-size:12px;"
		"}");
	MsgBox->setWindowModality(Qt::NonModal);
	MsgBox->setWindowTitle("提示");
	MsgBox->setText(QString("任务已完成！"));
	MsgBox->setStandardButtons(QMessageBox::Ok);

	MsgBox->show();

	ui->pgBar->setValue(0);
	ui->pgBar->setVisible(false);
}

void PackJobCar::onPgValueUpdated(int nPgValue)
{
	ui->pgBar->setValue(nPgValue);
}

void PackJobCar::onPgLabelUpdated(int nMode)
{
	switch (nMode)
	{
	case CAR_MODE:
		ui->label_Progress->setText(QString("正在打包资料：Car"));
		break;
	case JOB_MODE:
		ui->label_Progress->setText(QString("正在打包资料：Job"));
		break;
	case FM_MODE:
		ui->label_Progress->setText(QString("正在打包资料：Fm"));
		break;
	default:
		ui->label_Progress->setText(QString(""));
		break;
	}
}

/**
 * \brief
 * \param sRootPth 根目录
 * \return 根目录的子级文件夹目录
 */
QStringList PackJobCar::getSubDirs(QString sRootPth, bool bIsCarPth)
{
	QStringList res;

	if (sRootPth.size() != 0)
	{
		QDir dir(sRootPth);
		if (dir.exists())
		{
			QFileInfoList lstSubDirInfo;
			if (!bIsCarPth)
			{
				lstSubDirInfo = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
				QStringList filters;
				filters << QString("*.bot") << QString("*.top");
				dir.setNameFilters(filters);
				lstSubDirInfo += dir.entryInfoList(dir.filter() | QDir::NoDotAndDotDot);
			}
			else
			{
				lstSubDirInfo = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
			}

			foreach(QFileInfo info, lstSubDirInfo)
			{
				if (!bIsCarPth)
				{
					QString path = info.dir().path() + "\\" + info.completeBaseName();
					if (res.contains(path))
					{
						continue;
					}
					if (info.isFile())
					{
						res.append(info.dir().path() + "\\" + info.completeBaseName());
					}
					else
					{
						res.append(info.absoluteFilePath());
					}
				}
				else
				{
					res.append(info.absoluteFilePath());
				}
			}
		}
	}

	return res;
}

/**
 * \brief 获取选中的Car号/Job号/Fm号
 * \param nMode Car还是Job还是Fm
 * \return
 */
QStringList PackJobCar::getCheckedSubDirs(int nMode)
{
	QStringList lstChecked;
	PJC_TableModel* model;
	switch (nMode)
	{
	case CAR_MODE:
		model = m_pModelCar;
		break;
	case JOB_MODE:
		model = m_pModelJob;
		break;
	case FM_MODE:
		model = m_pModelFm;
		break;
	}

	QList<ModelItem> modelData = model->getModelData();
	QMap<int, Qt::CheckState> mapCheckList = model->getCheckList();

	foreach(int key, mapCheckList.keys())
	{
		if (mapCheckList[key] == Qt::Checked)
		{
			lstChecked.append(modelData.at(key).sName);
		}
	}

	return lstChecked;
}

PackJobCar::CarVersion PackJobCar::getCarVersion()
{
	auto carVer = ui->rBtn_Offline->isChecked() ? OFF_LINE : ON_LINE;
	return carVer;
}

int PackJobCar::getNumOfFiles(QString sDirSrc)
{
	int nCount = 0;
	QDir dir(sDirSrc);

	if (!dir.exists())
	{
		return 0;
	}

	QFileInfoList lstDir = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs);
	foreach(auto dir_tmp, lstDir)
	{
		nCount += getNumOfFiles(dir_tmp.absoluteFilePath());
	}

	QFileInfoList lstFile = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
	nCount += lstFile.size();

	return nCount;
}

/**
 * \brief 设置滚动条的样式
 * \param pScrollBar
 */
void PackJobCar::setScrollBarStyle(QScrollBar* pScrollBar)
{
	pScrollBar->setStyleSheet(
		QString(
			"QScrollBar:vertical"
			"{"
			//"background:transparent;"
			"width:10px;"
			"padding:0px;"
			"}"
			"QScrollBar::add-page:vertical, "
			"QScrollBar::sub-page:vertical"
			"{"
			"width:10px;"
			"background-color:rgba(%1);"
			"}" // 非滑块处
			"QScrollBar::add-line:vertical, "
			"QScrollBar::sub-line:vertical"
			"{"
			"border:none;"
			"}"
			"QScrollBar::handle:vertical"
			"{"
			"background-color:rgb(%2);"
			"image:url(:/Main/scrollbar.png);"
			"border-radius:5px;"
			"border-radius:5px;"
			"}"
		).arg(SETTING_BG).arg(HANDEL_BG));
}

void PackJobCar::setUiStateDuringThread(bool bState)
{
	ui->label_CarPath->setEnabled(bState);
	ui->label_Job->setEnabled(bState);
	ui->label_JobPath->setEnabled(bState);
	ui->label_Fm->setEnabled(bState);
	ui->label_FmPath->setEnabled(bState);
	ui->label_PathOut->setEnabled(bState);

	ui->Edit_PathCar->setEnabled(bState);
	ui->Edit_PathJob->setEnabled(bState);
	ui->Edit_PathFm->setEnabled(bState);
	ui->Edit_PathOut->setEnabled(bState);

	ui->cBox_SelectAll_Car->setEnabled(bState);
	ui->cBox_SelectAll_Job->setEnabled(bState);
	ui->cBox_SelectAll_Fm->setEnabled(bState);
	ui->btn_OpenPathCar->setEnabled(bState);
	ui->btn_OpenPathJob->setEnabled(bState);
	ui->btn_OpenPathFm->setEnabled(bState);
	ui->btn_OpenPathOut->setEnabled(bState);

	ui->btnSetting->setEnabled(bState);

	ui->btnStart->setEnabled(bState);
}
