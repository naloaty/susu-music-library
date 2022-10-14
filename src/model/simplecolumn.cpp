#include "simplecolumn.h"

QVariant SimpleColumn::colData(const QVariantMap &rowData, int role)
{
    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return rowData.value(m_key);

    return QVariant();
}


bool SimpleColumn::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}

Qt::ItemFlags SimpleColumn::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}
