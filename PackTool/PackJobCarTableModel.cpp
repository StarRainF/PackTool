#include "stdafx.h"
#include "PackJobCarTableModel.h"

PJC_TableView::PJC_TableView(QWidget* parent) : QTableView(parent)
{
}

void PJC_TableView::paintEvent(QPaintEvent* event)
{
	// 获取选中的行
	QTableView::paintEvent(event);

	if (model())
	{
		QPainter painter(viewport());
		painter.setPen(Qt::blue);

		// 遍历所有选中的行，并绘制背景颜色
		for (int row = 0; row < model()->rowCount(); ++row)
		{
			QModelIndex index = model()->index(row, 0);
			if (model()->data(index, Qt::CheckStateRole) == Qt::Checked)
			{
				QRect rect = visualRect(index);
				painter.fillRect(rect, QColor(0, 0, 255, 50));
			}
		}
	}
}

PJC_TableModel::PJC_TableModel(QObject* parent)
	: QAbstractTableModel(parent)
{
}

void PJC_TableModel::SelectAllCheckBox()
{
	// QItemSelectionModel* selectionModel = this->();
}

int PJC_TableModel::rowCount(const QModelIndex& parent = QModelIndex()) const
{
	return modelData.count();
}

int PJC_TableModel::columnCount(const QModelIndex& parent = QModelIndex()) const
{
	return 3;
}

QVariant PJC_TableModel::data(const QModelIndex& index, int role = Qt::DisplayRole) const
{
	switch (role)
	{
	case Qt::DisplayRole:
		return getTableDisplayData(index);
	case Qt::EditRole:
		return getTableDisplayData(index);
	case Qt::TextAlignmentRole:
		return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
	case Qt::CheckStateRole:
		if (index.column() == COLUMN_CHECK_BOX)
		{
			auto mapCheckList = getCheckList();
			return mapCheckList[index.row()] == Qt::Checked ? Qt::Checked : Qt::Unchecked;
		}
		break;
	default:
		break;
	}
	return QVariant();
}

QVariant PJC_TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	switch (role)
	{
	case Qt::DisplayRole:
		if (orientation == Qt::Horizontal && section >= 0 && section <= columnCount())
		{
			return Headers.at(section);
		}
		break;
	default:
		break;
	}
	return QAbstractItemModel::headerData(section, orientation, role);
}

// 表格可选中、可复选
Qt::ItemFlags PJC_TableModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
	{
		return QAbstractItemModel::flags(index);
	}

	if (index.column() == COLUMN_CHECK_BOX)
	{
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
	}
	else
	{
		return QAbstractItemModel::flags(index);
	}
}

QVariant PJC_TableModel::getTableDisplayData(const QModelIndex& index) const
{
	if (!index.isValid() || index.column() > columnCount() || index.row() > rowCount())
	{
		return QVariant();
	}

	int row = index.row();
	int column = index.column();

	switch (column)
	{
	case 0:
		return modelData.at(row).nCheckedState;
	case 1:
		return modelData.at(row).sName;
	case 2:
		return modelData.at(row).qdtLastModify;
	default:
		return QVariant();
	}
}

void PJC_TableModel::setModelData(const QList<ModelItem>& datas)
{
	//重置model数据之前调用beginResetModel，此时会触发modelAboutToBeReset信号
	beginResetModel();
	//重置model中的数据
	modelData = datas;
	//数据设置结束后调用endResetModel，此时会触发modelReset信号
	endResetModel();
}

bool PJC_TableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (!index.isValid())
		return false;
	Sleep(10);
	if (role == Qt::CheckStateRole && index.column() == COLUMN_CHECK_BOX)
	{
		if (value == Qt::Checked) //  
		{
			modelData[index.row()].nCheckedState = Qt::Checked;
		}
		else
		{
			modelData[index.row()].nCheckedState = Qt::Unchecked;
		}

		emit checkBoxChanged(index);
	}

	return true;
}

void PJC_TableModel::setChecked(const int idx, const QVariant& value, bool bConnect)
{
	Sleep(10);
	if (value == Qt::Checked) 
	{
		modelData[idx].nCheckedState = Qt::Checked;
	}
	else
	{
		modelData[idx].nCheckedState = Qt::Unchecked;
	}
	if (bConnect)
	{
		emit checkBoxChanged(createIndex(idx, 0));
	}

	QModelIndex startIndex = createIndex(idx, COLUMN_CHECK_BOX);
	QModelIndex endIndex = createIndex(idx, COLUMN_CHECK_BOX);
	emit dataChanged(startIndex, endIndex);
}

void PJC_TableModel::sort(int column, Qt::SortOrder order)
{
	switch (column)
	{
	case 0:
		return;
	case 1: // 按文件名排序
		if (order == Qt::AscendingOrder)
		{
			std::sort(modelData.begin(), modelData.end(), [=]
		          (const ModelItem& v1, const ModelItem& v2)
			          {
				          QString t1 = v1.sName;
				          QString t2 = v2.sName;
				          return t1 < t2;
			          });
		}
		else
		{
			std::sort(modelData.begin(), modelData.end(), [=]
		          (const ModelItem& v1, const ModelItem& v2)
			          {
				          QString t1 = v1.sName;
				          QString t2 = v2.sName;
				          return t1 > t2;
			          });
		}
	case 2: // 按修改时间排序
		if (order == Qt::AscendingOrder)
		{
			std::sort(modelData.begin(), modelData.end(), [=]
		          (const ModelItem& v1, const ModelItem& v2)
			          {
				          QDateTime t1 = v1.qdtLastModify;
				          QDateTime t2 = v2.qdtLastModify;
				          return t1 < t2;
			          });
		}
		else
		{
			std::sort(modelData.begin(), modelData.end(), [=]
		          (const ModelItem& v1, const ModelItem& v2)
			          {
				          QDateTime t1 = v1.qdtLastModify;
				          QDateTime t2 = v2.qdtLastModify;
				          return t1 > t2;
			          });
		}
	}

	emit dataChanged(createIndex(0, 0), createIndex(rowCount() - 1, columnCount() - 1));
}

QList<ModelItem> PJC_TableModel::getModelData()
{
	return modelData;
}

int PJC_TableModel::getItemIdx(const QString sName)
{
	for (int i = 0; i < modelData.size(); i++)
	{
		ModelItem tmp = modelData.at(i);
		if (tmp.sName == sName)
		{
			return i;
		}
	}

	return -1;
}

QMap<int, Qt::CheckState> PJC_TableModel::getCheckList() const
{
	QMap<int, Qt::CheckState> map;
	for (int i = 0; i < modelData.size(); i++)
	{
		auto item = modelData.at(i);
		map.insert(i, item.nCheckedState);
	}

	return map;
}

CheckBoxDelegate::CheckBoxDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
}

CheckBoxDelegate::~CheckBoxDelegate()
{
}

void CheckBoxDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
                                            const QModelIndex& index) const
{
	// 将编辑器设置为矩形属性
	editor->setGeometry(option.rect);
}

bool CheckBoxDelegate::getCheckState()
{
	return m_bCheckState;
}

void CheckBoxDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	bool checked = index.model()->data(index, Qt::DisplayRole).toBool();

	if (index.column() == m_iCol)
	{
		QStyleOptionButton checkBoxStyleOption;
		checkBoxStyleOption.state |= QStyle::State_Enabled;
		checkBoxStyleOption.state |= checked ? QStyle::State_On : QStyle::State_Off;
		checkBoxStyleOption.rect = CheckBoxRect(option);

		QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkBoxStyleOption, painter);
	}
	else
	{
		QStyledItemDelegate::paint(painter, option, index);
	}
}

bool CheckBoxDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option,
                                   const QModelIndex& index)
{
	if (index.column() == m_iCol)
	{
		int iType = event->type();
		if ((iType == QEvent::MouseButtonRelease) ||
			(iType == QEvent::MouseButtonDblClick))
		{
			QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
			if (mouseEvent->button() != Qt::LeftButton ||
				!CheckBoxRect(option).contains(mouseEvent->pos()))
			{
				return true;
			}
			if (iType == QEvent::MouseButtonDblClick || iType == QEvent::MouseButtonRelease)
			{
				return true;
			}
		}
		else if (iType == QEvent::KeyPress)
		{
			if (static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space &&
				static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select)
			{
				return false;
			}
		}

		//根据当前单元格的选中状态来在 选中/未选中 之间切换
		QVariant value = index.data(Qt::CheckStateRole);
		Qt::CheckState state = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked
			                        ? Qt::Unchecked
			                        : Qt::Checked);

		//设置当前单元格的选中状态
		bool ok = model->setData(index, state, Qt::CheckStateRole);
		return ok;
	}
	else
	{
		return QStyledItemDelegate::editorEvent(event, model, option, index);
	}
}

QRect CheckBoxDelegate::CheckBoxRect(const QStyleOptionViewItem& viewItemStyleOptions)
{
	//绘制按钮所需要的参数
	QStyleOptionButton checkBoxStyleOption;
	//按照给定的风格参数 返回元素子区域
	QRect checkBoxRect = QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator, &checkBoxStyleOption);
	//返回QCheckBox坐标
	QPoint checkBoxPoint(
		viewItemStyleOptions.rect.x() + viewItemStyleOptions.rect.width() / 2 - checkBoxRect.width() / 2,
		viewItemStyleOptions.rect.y() + viewItemStyleOptions.rect.height() / 2 - checkBoxRect.height() / 2);
	//返回QCheckBox几何形状
	return QRect(checkBoxPoint, checkBoxRect.size());
}
