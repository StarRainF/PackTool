#include "stdafx.h"
#include "SimpleTools.h"

SimpleTools::SimpleTools(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SimpleToolsClass())
{
    ui->setupUi(this);

	// 打开窗口：一键打包资料
	QObject::connect(ui->btn_PackCarJob, &QPushButton::clicked, [=](){
		if (!m_pWinPackJobCar)
		{
			m_pWinPackJobCar = new PackJobCar(this);
		}
		m_pWinPackJobCar->show();
	});

	// 打开窗口：一键生成avilog
	QObject::connect(ui->btn_GenAvilog, &QPushButton::clicked, [=]() {
		if (!m_pWinGenAvilog)
		{
			m_pWinGenAvilog = new GenAvilog(this);
		}
		m_pWinGenAvilog->show();
	});

	//demo


	// TODO: 一键删除ai文件

	// TODO：一键打包漏失资料

}

SimpleTools::~SimpleTools()
{
    delete ui;
}
