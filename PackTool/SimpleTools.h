#pragma once

#include "common.h"
#include "GenAvilog.h"
#include "PackJobCar.h"
#include "ui_SimpleTools.h"
#include "ui_win_GenAvilog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SimpleToolsClass; };
QT_END_NAMESPACE

class SimpleTools : public QWidget
{
    Q_OBJECT

public:
    SimpleTools(QWidget *parent = nullptr);
    ~SimpleTools();

private:
    Ui::SimpleToolsClass *ui;

	PackJobCar* m_pWinPackJobCar = nullptr;
	GenAvilog* m_pWinGenAvilog = nullptr;
};
