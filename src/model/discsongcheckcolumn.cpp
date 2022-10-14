#include "discsongcheckcolumn.h"

void DiscSongCheckColumn::updateFilter(int discId)
{
    m_discId = discId;
    qDebug() << TAG << "Filter updated:" << discId;
    refresh();
}

void DiscSongCheckColumn::refresh()
{
    if (m_discSongs == nullptr)
        return;

    if (m_songIdKey.isEmpty() || m_discIdKey.isEmpty())
        return;

    m_rowBySongId.clear();

    int discCol = m_discSongs->getColByKey(m_discIdKey);
    int songCol = m_discSongs->getColByKey(m_songIdKey);

    if (m_discId == -1)
        return;

    for (int i = 0; i < m_discSongs->rowCount(QModelIndex()); ++i) {
        QModelIndex discIndex = m_discSongs->index(i, discCol);
        QModelIndex songIndex = m_discSongs->index(i, songCol);

        int _discId = m_discSongs->data(discIndex, Qt::DisplayRole).toInt();
        int _songId = m_discSongs->data(songIndex, Qt::DisplayRole).toInt();

        if (_discId == m_discId)
            m_rowBySongId.emplace(_songId, i);
    }

    qDebug() << TAG << "Column state refreshed:" << m_rowBySongId.size() << "items match filter";
}

QVariant DiscSongCheckColumn::colData(const QVariantMap &rowData, int role)
{
    if (role == Qt::CheckStateRole) {
        int songId = rowData.value(m_songIdKey).toInt();

        if (!m_rowBySongId.count(songId))
            return Qt::Unchecked;
        else
            return Qt::Checked;

    }

    return QVariant();
}

bool DiscSongCheckColumn::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::CheckStateRole)
        return false;

    VariantMapTableModel *p =
            dynamic_cast<VariantMapTableModel*>(parent());

    int songCol = p->getColByKey(m_songIdKey);
    QModelIndex songIndex = p->index(index.row(), songCol);
    int songId = p->data(songIndex, Qt::DisplayRole).toInt();

    if ((Qt::CheckState)value.toInt() == Qt::Checked) {
        m_database->insertDiscSong(m_discId, songId);
        return true;
    } else {
        m_database->removeDiscSong(m_discId, songId);
        return true;
    }

    return false;
}

Qt::ItemFlags DiscSongCheckColumn::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
}
