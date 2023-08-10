#pragma once
#pragma execution_character_set("UTF-8")

#include "common.h"

#define COLUMN_CHECK_BOX 0
#define COLUMN_FILE_PATH 1

struct ModelItem
{
	Qt::CheckState nCheckedState;
	QString sName;
	QDateTime qdtLastModify;
};

class PJC_TableView : public QTableView
{
	Q_OBJECT

public:
	PJC_TableView(QWidget *parent = nullptr);

protected:
	void paintEvent(QPaintEvent* event) override;
};

class PJC_TableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	explicit PJC_TableModel(QObject* parent = nullptr);

	const QStringList Headers = {
		QString("选中"),
		QString("文件名"),
		QString("修改时间")
	};

	void setModelData(const QList<ModelItem>& datas);
	QList<ModelItem> getModelData();
	int getItemIdx(const QString sName);
	QMap<int, Qt::CheckState> getCheckList() const;
	QVariant getTableDisplayData(const QModelIndex& index) const;

	void setChecked(int idx, const QVariant& value, bool bConnect=false);
	int rowCount(const QModelIndex& parent) const override;

signals:
	void checkBoxChanged(const QModelIndex& index);

public slots:
	void SelectAllCheckBox();

private:
	QList<ModelItem> modelData;

	int columnCount(const QModelIndex& parent) const override;

	QVariant data(const QModelIndex& index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	Qt::ItemFlags flags(const QModelIndex& index) const override;

	bool setData(const QModelIndex& index, const QVariant& value, int role) override;
	void sort(int column, Qt::SortOrder order) override;
};


