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

	// �ڵ�Ԫ�񱻱༭ʱ����һ�� QCheckBox ����
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index) const override
	{
		auto checkBox = new QCheckBox(parent);
		return checkBox;
	}

	// �ڱ༭��ɺ����õ�Ԫ���ֵ
	void setModelData(QWidget* editor, QAbstractItemModel* model,
		const QModelIndex& index) const override
	{
		auto checkBox = qobject_cast<QCheckBox*>(editor);
		bool value = checkBox->isChecked();
		model->setData(index, value, Qt::EditRole);
	}

	// ���ñ༭������
	void setEditorData(QWidget* editor, const QModelIndex& index) const override
	{
		auto box = qobject_cast<QCheckBox*>(editor);
		box->setChecked(m_bCheckState);
	}

	// ���±༭����������
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
		const QModelIndex& index) const override;

	// ���ѡ��״̬
	bool getCheckState();

	// ���Ƹ�ѡ��
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

	// ��Ӧ����¼�����������
	bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option,
		const QModelIndex& index) override;

	static QRect CheckBoxRect(const QStyleOptionViewItem& viewItemStyleOptions);

private:
	bool	m_bCheckState = false;
	int		m_iCol = 0;
};
