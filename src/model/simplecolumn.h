#ifndef SIMPLECOLUMN_H
#define SIMPLECOLUMN_H

#include "src/model/variantmaptablemodel.h"


class SimpleColumn : public AbstractColumn
{
public:
    SimpleColumn(const QString &key, const QString &header = "")
        : AbstractColumn(key, header) { };

public: // AbstractColumn interface
    QVariant colData(const QVariantMap &rowData, int role = Qt::DisplayRole) override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
};

#endif // SIMPLECOLUMN_H
