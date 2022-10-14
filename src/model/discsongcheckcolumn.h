#ifndef DISCSONGCHECKCOLUMN_H
#define DISCSONGCHECKCOLUMN_H

#include "src/model/variantmaptablemodel.h"
#include "src/database.h"

#undef TAG
#define TAG "[DiscSongCheckColumn]"

class DiscSongCheckColumn : public AbstractColumn
{
public:
    DiscSongCheckColumn(const QString &key, const QString &header = "")
        : AbstractColumn(key, header) { };

private:
    int m_discId = -1;
    QString m_songIdKey;
    QString m_discIdKey;
    Database *m_database = nullptr;
    VariantMapTableModel *m_discSongs = nullptr;
    std::unordered_map<int, int> m_rowBySongId;

public slots:
    void updateFilter(int discId);
    void refresh();

public:
    void setDiscSongsModel(VariantMapTableModel *model) {
        m_discSongs = model;
        QObject::connect(model, &VariantMapTableModel::modelReset, this, &DiscSongCheckColumn::refresh);
        QObject::connect(model, &VariantMapTableModel::rowsInserted, this, &DiscSongCheckColumn::refresh);
        QObject::connect(model, &VariantMapTableModel::rowsRemoved, this, &DiscSongCheckColumn::refresh);
    } ;
    void setSongIdColumnKey(const QString &key) { m_songIdKey = key; };
    void setDiscIdColumnKey(const QString &key) { m_discIdKey = key; }
    void setDatabase(Database *database) { m_database = database; };

public: // AbstractColumn interface
    QVariant colData(const QVariantMap &rowData, int role = Qt::DisplayRole) override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

};

#endif // DISCSONGCHECKCOLUMN_H
