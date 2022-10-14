#include "lookupcolumn.h"

QVariant LookupColumn::colData(const QVariantMap &rowData, int role)
{
    if (role == Qt::DisplayRole) {
        int id = rowData.value(m_keyColumn).toInt();
        int lookupRow = m_lookupModel->getRowById(id);

        if (lookupRow < 0)
            return QVariant();

        int lookupCol = m_lookupModel->getColByKey(m_lookupColumn);
        return m_lookupModel->index(lookupRow, lookupCol).data(role);
    }

    return QVariant();
}

bool LookupColumn::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}

Qt::ItemFlags LookupColumn::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}
