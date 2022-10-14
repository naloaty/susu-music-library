#include "variantmaptablemodel.h"

VariantMapTableModel::VariantMapTableModel(QObject *parent)
    : QAbstractTableModel{parent}
{

}

void VariantMapTableModel::registerColumn(AbstractColumn *column)
{
    for (auto &col: m_columns) {
        if (col->getKey() == column->getKey())
            return;
    }

    column->setParent(this);
    m_columns.emplace_back(column);
}

void VariantMapTableModel::addRow(QVariantMap *row)
{
    beginInsertRows(QModelIndex(), m_rowData.size(), m_rowData.size());
    m_rowData.emplace_back(row);

    if (!m_idColumn.isEmpty())
        m_rowById.emplace(row->value(m_idColumn).toInt(), m_rowData.size());

    endInsertRows();
}

void VariantMapTableModel::removeRow(int row)
{
    if (row < 0 || row >= m_rowData.size())
        return;

    beginRemoveRows(QModelIndex(), row, row);

    if (!m_idColumn.isEmpty())
        m_rowById.erase(getIdByRow(row));

    auto it = m_rowData.begin();
    advance(it, row);
    m_rowData.erase(it);

    endRemoveRows();
}


void VariantMapTableModel::replaceRows(Records<QVariantMap> rows)
{
    beginResetModel();
    m_rowData.clear();
    m_rowData = move(rows);

    if (!m_idColumn.isEmpty()) {
        m_rowById.clear();

        for (int i = 0; i < m_rowData.size(); ++i) {
            int id = m_rowData.at(i)->value(m_idColumn).toInt();
            m_rowById.emplace(id, i);
        }
    }

    endResetModel();
}

void VariantMapTableModel::clear()
{
    beginResetModel();
    m_rowData.clear();

    if (!m_idColumn.isEmpty())
        m_rowById.clear();

    endResetModel();
}

void VariantMapTableModel::update()
{
    beginResetModel();
    endResetModel();
}

QString VariantMapTableModel::getKeyByCol(int col)
{
    if (col < 0 || col >= m_columns.size())
        return QString();

    return m_columns.at(col)->getKey();
}

int VariantMapTableModel::getColByKey(const QString &key)
{
    for (int col = 0; col < m_columns.size(); col++)
        if (getKeyByCol(col) == key)
            return col;

    return -1;
}

int VariantMapTableModel::getRowById(int id)
{
    if (m_idColumn.isEmpty())
        return -1;

    if (!m_rowById.count(id))
        return -1;

    return m_rowById.at(id);
}

int VariantMapTableModel::getIdByRow(int row)
{
    if (m_idColumn.isEmpty())
        return -1;

    if (row < 0 || row >= m_rowData.size())
        return -1;

    return m_rowData.at(row)->value(m_idColumn).toInt();
}

int VariantMapTableModel::rowCount(const QModelIndex &parent) const
{
    return m_rowData.size();
}

int VariantMapTableModel::columnCount(const QModelIndex &parent) const
{
    return m_columns.size();
}

QVariant VariantMapTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    return m_columns.at(index.column())->colData(*m_rowData.at(index.row()), role);
}

bool VariantMapTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (role == Qt::EditRole) {
        m_rowData.at(index.row())->insert(getKeyByCol(index.column()), value);
        m_columns.at(index.column())->setData(index, value, role);
        emit dataChanged(index, index);
        return true;
    }

    if (role == Qt::CheckStateRole) {
        return m_columns.at(index.column())->setData(index, value, role);
    }

    return false;
}

QVariant VariantMapTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal &&
            (role == Qt::DisplayRole || role == Qt::EditRole)) {

        const QString &header = m_columns.at(section)->getHeader();

        if (!header.isEmpty())
            return header;
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

Qt::ItemFlags VariantMapTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return m_columns.at(index.column())->flags(index);
}
