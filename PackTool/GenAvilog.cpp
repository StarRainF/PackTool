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
//ѡ��Car·��
void GenAvilog::btn_OpenCarPath_clicked()
{
	//�ļ���·��
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
		MsgBox->setWindowModality(Qt::NonModal); // ����Ϊ��ģ̬�����������߳�
		MsgBox->setWindowTitle(QString("ѡ��Car�ļ���"));
		MsgBox->setText(QString("ѡ�е�·����Ч����ѡ����ȷ��·��"));
		MsgBox->setStandardButtons(QMessageBox::Ok);

		MsgBox->show();
	}
	else
	{
		qDebug() << "srcDirPath=" << m_sCarPath;
		m_sCarPath += "\\";  /*ʹ��\\ת��Ϊ\*/
	}
	ui->Edit_CarPath->setText(m_sCarPath);
}
//ѡ��Avilog���·��
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
		MsgBox->setWindowModality(Qt::NonModal);//��Ϊ��ģ̬��������������
		MsgBox->setWindowTitle(QString("ѡ��Avilog���λ��"));
		MsgBox->setText(QString("ѡ�е�·����Ч����ѡ����ȷ��·��"));
		MsgBox->setStandardButtons(QMessageBox::Ok);

		MsgBox->show();
	}
	else
	{
		qDebug() << "srcDirPath=" << m_sAvilogPath;
		ui->Edit_AvilogPath->setText(m_sAvilogPath);
	}
}
//����ʼ��ť
void GenAvilog::btn_Start_clicked(){
	//setUiStateDuringThread(false);
	//startTraverseThread();  //���������߳�
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
	QDir dir(m_sCarPath);//����Ҫ������carĿ¼
	QString Pn, Bn, fi;//�����洢�Ϻţ������ţ������Ϣ
	QStringList Pnlist = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
	//�ж��Ƿ��Ϻŷ���

		//����car��ȡ���Ϻ�
		for (int i = 0; i < Pnlist.size(); i++)
		{
			Pn = Pnlist.at(i);
			QString sPllist = m_sCarPath + Pnlist.at(i);
			QDir dir(sPllist);//����Ҫ�������Ϻ�Ŀ¼

			QStringList Bnlist = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);


			//�����Ϻţ�ȡ��������
			//ΪʲôBnList.size() == 0��---�ѽ����·����Ҫ����m_sCarPath
			for (int Bi = 0; Bi < Bnlist.size(); Bi++)
			{
				Bn = Bnlist.at(Bi);
				QString sBilist = sPllist + "\\" + Bnlist.at(Bi);
				QDir dir(sBilist);
				QStringList pathNum, fileNum;
				QFileInfoList Filist = dir.entryInfoList(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
				QString bNum;

				//��������Ŀ¼
				for (auto file_info : Filist) {
					//�Ƿ�Ϊ����ļ���
					if (file_info.isDir()) {
						//����Ŵ���Filename List��
						if (file_info.fileName().contains("show"))
						{
							continue;
						}
						else
						{
							pathNum.append(file_info.fileName());
						}

					}
					//�Ƿ�Ϊ�ļ�
					if (file_info.isFile())
					{
						fileNum.append(file_info.fileName());
					}
				}
				//�Ա��Ƿ�ΪB�����T�棬��ӡ��Ϣ
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

//��Ϣд��txt�ļ�
void GenAvilog::WAvitxt(QString Pn, QString Bn, QString Fi, QString BorT,bool isPn)
{
	//TODO ��ťѡ��Avilogλ��
	QString fileName;
	//�ж��Ƿ��Ϻŷ���
	if (isPn) 
	{
		fileName = m_sAvilogPath + "\\" + Pn + "Avilog.txt";
	}
	else
	{
		fileName = m_sAvilogPath + "\\" + "Avilog.txt";
	}
	
	QFile f(fileName);
	
	//WAvitxt��ѭ��д�룬��ʾ��Ϣ��һֱ����
	/*if (f.exists())
	{
		QMessageBox::warning(this, "�����ļ�","�ļ��Ѿ����ڣ�");
		
	}*/
	if (!f.open(QIODevice::ReadWrite | QIODevice::Append))   //�Զ�д��׷�ӵķ�ʽд���ı�
	{
		qDebug() << ("���ļ�ʧ��");
	}
	QTextStream txtOutput(&f);
	QTime time = QTime::currentTime();
	QString str;
	QString strtime = time.toString("hh:mm:ss");
	str = strtime + " " + Pn + " " + "admin" + " " + Bn + ":" + Fi + ":" + BorT + " 2";
	ui->textBrowser->append(str);//������ļ�����
	txtOutput << str << endl;
	f.close();

}

void GenAvilog :: loadIni()
{
	
}
GenAvilog::~GenAvilog()
{}
