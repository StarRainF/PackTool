#pragma once

#include <QAbstractTableModel>
#include <QDialog>
#include <QEvent>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QObject>
#include <QSettings>
#include <QtConfig>
#include <QTextCodec>
#include <QWidget>

#include <iostream>
#include <random>

#include "QtConcurrent/qtconcurrentrun.h"

class CheckBoxDelegate : public QStyledItemDelegate
{
public:
	CheckBoxDelegate(QObject* parent = nullptr);
	~CheckBoxDelegate() override;

	// 在单元格被编辑时返回一个 QCheckBox 对象
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index) const override
	{
		auto checkBox = new QCheckBox(parent);
		return checkBox;
	}

	// 在编辑完成后设置单元格的值
	void setModelData(QWidget* editor, QAbstractItemModel* model,
		const QModelIndex& index) const override
	{
		auto checkBox = qobject_cast<QCheckBox*>(editor);
		bool value = checkBox->isChecked();
		model->setData(index, value, Qt::EditRole);
	}

	// 设置编辑器数据
	void setEditorData(QWidget* editor, const QModelIndex& index) const override
	{
		auto box = qobject_cast<QCheckBox*>(editor);
		box->setChecked(m_bCheckState);
	}

	// 更新编辑器集合属性
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
		const QModelIndex& index) const override;

	// 获得选中状态
	bool getCheckState();

	// 绘制复选框
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

	// 响应鼠标事件，更新数据
	bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option,
		const QModelIndex& index) override;

	static QRect CheckBoxRect(const QStyleOptionViewItem& viewItemStyleOptions);

private:
	bool	m_bCheckState = false;
	int		m_iCol = 0;
};
