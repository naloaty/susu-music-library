#ifndef CHECKLOOKUPCOLUMN_H
#define CHECKLOOKUPCOLUMN_H


#include "src/model/variantmaptablemodel.h"

class CheckLookupColumn : public AbstractColumn
{
public:
    CheckLookupColumn(
        const QString &key,
        VariantMapTableModel *lookupModel,
        const QString &keyColumn,
        const QString &header = ""
    ) : AbstractColumn(key, false)
    {
        m_lookupModel = lookupModel;
        m_keyColumn = keyColumn;
        setHeader(header);
    };

private:
    VariantMapTableModel *m_lookupModel;
    QString m_keyColumn;

public: // AbstractColumn interface
    QVariant colData(const QVariantMap &rowData, int role = Qt::DisplayRole) override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
};


#endif // CHECKLOOKUPCOLUMN_H
