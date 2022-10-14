#ifndef VARIANTMAPTABLEMODEL_H
#define VARIANTMAPTABLEMODEL_H

#include <QAbstractTableModel>

#include "src/database.h"


class AbstractColumn : public QObject
{
    Q_OBJECT
protected:
    QString m_key;
    QString m_header = "";

public:
    AbstractColumn(const QString &key, const QString &header)
        : QObject { 0 } {
        m_key = key;
        setHeader(header);
    };

    QString getKey() {
        return m_key;
    }

    QString getHeader() {
        return m_header;
    }

    void setHeader(const QString &header) {
        m_header = header;
    }

public:
    virtual QVariant colData(const QVariantMap &rowData, int role = Qt::DisplayRole) = 0;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) = 0;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const = 0;
};


class VariantMapTableModel : public QAbstractTableModel
{
    Q_OBJECT
private:
    QString m_idColumn = "";
    Records<QVariantMap> m_rowData;
    Records<AbstractColumn> m_columns;
    std::unordered_map<int, int> m_rowById;

public:
    explicit VariantMapTableModel(QObject *parent = nullptr);
    void setIdColumn(const QString &columnKey) { m_idColumn = columnKey; };
    void registerColumn(AbstractColumn *column);
    void addRow(QVariantMap *row);
    void removeRow(int row);
    void replaceRows(Records<QVariantMap> rows);

    // Convinience methods
    QString getKeyByCol(int col);
        int getColByKey(const QString &key);
        int getRowById(int id);
        int getIdByRow(int row);

public slots:
    void clear();
    void update();

public: // QAbstractItemModel interface
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual int columnCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override; 
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
};

#endif // VARIANTMAPTABLEMODEL_H
