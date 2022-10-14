#include "checklookupcolumn.h"

QVariant CheckLookupColumn::colData(const QVariantMap &rowData, int role)
{
    if (role == Qt::CheckStateRole) {
        int id = rowData.value(m_keyColumn).toInt();
        int lookupRow = m_lookupModel->getRowById(id);

        if (lookupRow < 0)
            return Qt::Unchecked;
        else
            return Qt::Checked;
    }

    return QVariant();
}

bool CheckLookupColumn::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}

Qt::ItemFlags CheckLookupColumn::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}
