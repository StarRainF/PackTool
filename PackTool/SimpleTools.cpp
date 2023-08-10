#include "stdafx.h"
#include "SimpleTools.h"

SimpleTools::SimpleTools(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SimpleToolsClass())
{
    ui->setupUi(this);

	// �򿪴��ڣ�һ���������
	QObject::connect(ui->btn_PackCarJob, &QPushButton::clicked, [=](){
		if (!m_pWinPackJobCar)
		{
			m_pWinPackJobCar = new PackJobCar(this);
		}
		m_pWinPackJobCar->show();
	});

	// �򿪴��ڣ�һ������avilog
	QObject::connect(ui->btn_GenAvilog, &QPushButton::clicked, [=]() {
		if (!m_pWinGenAvilog)
		{
			m_pWinGenAvilog = new GenAvilog(this);
		}
		m_pWinGenAvilog->show();
	});

	//demo


	// TODO: һ��ɾ��ai�ļ�

	// TODO��һ�����©ʧ����

}

SimpleTools::~SimpleTools()
{
    delete ui;
}
