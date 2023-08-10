#include "stdafx.h"
#include "GenAvilog.h"

GenAvilog::GenAvilog(QWidget *parent) : QDialog(parent), ui(new Ui::GenAvilogWin)
{
	ui->setupUi(this);
	
	connect(ui->btn_OpenCarPath, SIGNAL(clicked()), this, SLOT(btn_OpenCarPath_clicked()));
	connect(ui->btn_OpenAvilog, SIGNAL(clicked()), this, SLOT(btn_OpenAvilog_clicked()));
	connect(ui->Edit_CarPath, SIGNAL(clicked()), this, SLOT(Edit_CarPath_clicked()));
	connect(ui->Edit_AvilogPath, SIGNAL(clicked()), this, SLOT(Edit_AvilogPath_clicked()));
	connect(ui->btn_Start, SIGNAL(clicked()), this, SLOT(btn_Start_clicked()));
	connect(ui->checkBox_BigAviByCar, SIGNAL(clicked()), this, SLOT(checkBox_BigAviByCar_clicked()));
	QTextCodec *codec = QTextCodec::codecForName("GBK");
	QTextCodec::setCodecForLocale(codec);
	
	/*connect(ui->btn_Start, &QPushButton::clicked, [=]()
	{
		m_sCarPath = ui->Edit_CarPath->text();

		QDir dirCar(m_sCarPath);
		if (!dirCar.exists())
		{
			return;
		}
	});*/
}
//选择Car路径
void GenAvilog::btn_OpenCarPath_clicked()
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
	ui->Edit_CarPath->setText(m_sCarPath);
}
//选择Avilog输出路径
void GenAvilog::btn_OpenAvilog_clicked()
{
	m_sAvilogPath = QFileDialog::getExistingDirectory(this, "choose Avilog Path", "\\");
	m_sAvilogPath.replace("/", "\\");
	if (m_sAvilogPath.isEmpty())
	{
		auto MsgBox = new QMessageBox();
		MsgBox->setStyleSheet("QLabel{"
			"min-width:200px;"
			"min-height:40px; "
			"font-size:12px;"
			"}");
		MsgBox->setWindowModality(Qt::NonModal);//设为非模态，不阻塞父进程
		MsgBox->setWindowTitle(QString("选择Avilog输出位置"));
		MsgBox->setText(QString("选中的路径无效！请选择正确的路径"));
		MsgBox->setStandardButtons(QMessageBox::Ok);

		MsgBox->show();
	}
	else
	{
		qDebug() << "srcDirPath=" << m_sAvilogPath;
		ui->Edit_AvilogPath->setText(m_sAvilogPath);
	}
}
//按开始按钮
void GenAvilog::btn_Start_clicked(){
	//setUiStateDuringThread(false);
	//startTraverseThread();  //开启遍历线程
	m_sCarPath = ui->Edit_CarPath->text();
	m_sAvilogPath = ui->Edit_AvilogPath->text();

	QDir dirCar(m_sCarPath);
	if (!dirCar.exists())
	{
		return;
	}

	QDir dirAvilog(m_sAvilogPath);
	if (!dirAvilog.exists())
	{
		return;
	}
	startTraverse();
	//getSubDirs(m_sCarPath, true);
}

void GenAvilog::checkBox_BigAviByCar_clicked()
{
	checkBox_BigAviByCar_ = true;
}

void GenAvilog::startTraverse()
{
	
	ui->textBrowser->clear();
	QDir dir(m_sCarPath);//设置要遍历的car目录
	QString Pn, Bn, fi;//用来存储料号，批量号，板号信息
	QStringList Pnlist = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
	//判断是否按料号分类

		//遍历car，取得料号
		for (int i = 0; i < Pnlist.size(); i++)
		{
			Pn = Pnlist.at(i);
			QString sPllist = m_sCarPath + Pnlist.at(i);
			QDir dir(sPllist);//设置要遍历的料号目录

			QStringList Bnlist = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);


			//遍历料号，取得批量号
			//为什么BnList.size() == 0了---已解决，路径需要加上m_sCarPath
			for (int Bi = 0; Bi < Bnlist.size(); Bi++)
			{
				Bn = Bnlist.at(Bi);
				QString sBilist = sPllist + "\\" + Bnlist.at(Bi);
				QDir dir(sBilist);
				QStringList pathNum, fileNum;
				QFileInfoList Filist = dir.entryInfoList(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
				QString bNum;

				//遍历批量目录
				for (auto file_info : Filist) {
					//是否为板号文件夹
					if (file_info.isDir()) {
						//将板号存入Filename List中
						if (file_info.fileName().contains("show"))
						{
							continue;
						}
						else
						{
							pathNum.append(file_info.fileName());
						}

					}
					//是否为文件
					if (file_info.isFile())
					{
						fileNum.append(file_info.fileName());
					}
				}
				//对比是否为B面或者T面，打印信息
				for (auto fi : pathNum) {
					for (auto it : fileNum) {
						if (it == fi + "_b") {
							WAvitxt(Pn, Bn, fi, "B",checkBox_BigAviByCar_);
						}
						else if (it == fi + "_t") {
							WAvitxt(Pn, Bn, fi, "T",checkBox_BigAviByCar_);
						}
					}
				}
			}

		}

}

void GenAvilog::Edit_AvilogPath_clicked() {
	m_sAvilogPath = ui->Edit_AvilogPath->text();
	m_sAvilogPath.replace("/", "\\");

}

void GenAvilog::Edit_CarPath_clicked() {
	m_sCarPath = ui->Edit_CarPath->text();
	m_sCarPath.replace("/", "\\");
}

//信息写入txt文件
void GenAvilog::WAvitxt(QString Pn, QString Bn, QString Fi, QString BorT,bool isPn)
{
	//TODO 按钮选择Avilog位置
	QString fileName;
	//判断是否按料号分类
	if (isPn) 
	{
		fileName = m_sAvilogPath + "\\" + Pn + "Avilog.txt";
	}
	else
	{
		fileName = m_sAvilogPath + "\\" + "Avilog.txt";
	}
	
	QFile f(fileName);
	
	//WAvitxt是循环写入，提示信息会一直弹窗
	/*if (f.exists())
	{
		QMessageBox::warning(this, "创建文件","文件已经存在！");
		
	}*/
	if (!f.open(QIODevice::ReadWrite | QIODevice::Append))   //以读写且追加的方式写入文本
	{
		qDebug() << ("打开文件失败");
	}
	QTextStream txtOutput(&f);
	QTime time = QTime::currentTime();
	QString str;
	QString strtime = time.toString("hh:mm:ss");
	str = strtime + " " + Pn + " " + "admin" + " " + Bn + ":" + Fi + ":" + BorT + " 2";
	ui->textBrowser->append(str);//输出到文件界面
	txtOutput << str << endl;
	f.close();

}

void GenAvilog :: loadIni()
{
	
}
GenAvilog::~GenAvilog()
{}
