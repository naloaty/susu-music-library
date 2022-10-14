#ifndef LOOKUPCOLUMN_H
#define LOOKUPCOLUMN_H

#include "src/model/variantmaptablemodel.h"

class LookupColumn : public AbstractColumn
{
public:
    LookupColumn(
        const QString &key,
        VariantMapTableModel *lookupModel,
        const QString &keyColumn,
        const QString &lookupColumn,
        const QString &header = ""
    ) : AbstractColumn(key, header)
    {
        m_lookupModel = lookupModel;
        m_keyColumn = keyColumn;
        m_lookupColumn = lookupColumn;
    };

private:
    VariantMapTableModel *m_lookupModel;
    QString m_keyColumn;
    QString m_lookupColumn;

public: // AbstractColumn interface
    QVariant colData(const QVariantMap &rowData, int role = Qt::DisplayRole) override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
};

#endif // LOOKUPCOLUMN_H
