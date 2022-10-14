#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <unordered_set>
#include <algorithm>
#include "database.h"

Database::Database(QObject *parent): QObject{parent}
{
}

int Database::switchUpdatingState(int lock)
{

    if (lock == 0 && m_updatingState == 0) {
        emit beginUpdate();
        m_updatingState = 1;
        return 1;
    }

    if (lock == 1 && m_updatingState == 1) {
        emit endUpdate();
        m_updatingState = 0;
    }

    return 0;
}

void Database::loadFromJsonFile(QString filename)
{
    QFile file;
    QString contents;

    qInfo() << TAG << "Reading file" << filename;
    file.setFileName(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << TAG << "Cannot open file for reading";
        throw DbException::FILE_OPEN;
    }

    contents = file.readAll();
    file.close();

    m_discs.clear();
    m_songs.clear();
    m_discSongs.clear();
    m_authors.clear();
    m_ids.maxDiscId = -1;
    m_ids.maxSongId = -1;
    m_ids.maxAuthorId = -1;

    qInfo() << TAG << "Loading JSON...";
    QJsonDocument doc = QJsonDocument::fromJson(contents.toUtf8());
    QJsonObject db    = doc.object();

    if (doc.isNull() ||
        db.isEmpty() ||
        !db.contains("Discs") ||
        !db.contains("Songs") ||
        !db.contains("Bands") ||
        !db.contains("Singers") ||
        !db.contains("DiscSongs"))
    {
        qCritical() << TAG << "File structure is not valid.";
        throw DbException::BAD_CONTENT;
    }

    QJsonArray  db_discs     = db["Discs"].toArray();
    QJsonArray  db_songs     = db["Songs"].toArray();
    QJsonArray  db_bands     = db["Bands"].toArray();
    QJsonArray  db_singers   = db["Singers"].toArray();
    QJsonArray  db_discSongs = db["DiscSongs"].toArray();

    for (const QJsonValue &v: db_discs)
    {
        int discId   = v["discId"].toInt();
        QString name = v["name"].toString();

        if (discId > m_ids.maxDiscId)
            m_ids.maxDiscId = discId;

        m_discs.emplace(discId, std::make_unique<Disc>(discId, name));
    }

    qInfo() << TAG << "Discs table loaded.";


    for (const QJsonValue &v: db_songs)
    {
        int songId   = v["songId"].toInt();
        int authorId = v["authorId"].toInt();
        QString name = v["name"].toString();

        if (songId > m_ids.maxSongId)
            m_ids.maxSongId = songId;

        m_songs.emplace(songId, std::make_unique<Song>(songId, authorId, name));
    }

    qInfo() << TAG << "Songs table loaded.";


    for (const QJsonValue &v: db_bands)
    {
        int bandId   = v["authorId"].toInt();
        QString name = v["name"].toString();

        if (bandId > m_ids.maxAuthorId)
            m_ids.maxAuthorId = bandId;

        m_authors.emplace(bandId, std::make_unique<Band>(bandId, name));
    }

    qInfo() << TAG << "Bands table loaded.";


    for (const QJsonValue &v: db_singers)
    {
        int singerId      = v["authorId"].toInt();
        QString firstName = v["firstName"].toString();
        QString lastName  = v["lastName"].toString();

        if (singerId > m_ids.maxAuthorId)
            m_ids.maxAuthorId = singerId;

        m_authors.emplace(singerId, std::make_unique<Singer>(singerId, firstName, lastName));
    }

    qInfo() << TAG << "Singers table loaded.";


    for (const QJsonValue &v: db_discSongs)
    {
        int discId = v["discId"].toInt();
        int songId = v["songId"].toInt();
        m_discSongs.push_back(std::make_unique<DiscSong>(discId, songId));
    }

    qInfo() << TAG << "DiscSongs table loaded.";
    qInfo() << TAG << "Database loading completed !";

    int lock = switchUpdatingState();
    emit discsChanged();
    emit songsChanged();
    emit authorsChanged();
    emit discSongsChanged();
    switchUpdatingState(lock);
}

void Database::saveToJsonFile(QString filename)
{
    qInfo() << TAG << "Saving database...";
    QJsonObject root;
    QJsonArray discs, discSongs, songs, bands, singers;

    for (auto const &i: m_discs) {
        Disc *disc = i.second.get();

        QJsonObject record;
        record.insert("discId", disc->getDiscId());
        record.insert("name", disc->getName());
        discs.push_back(QJsonValue(record));
    }

    qInfo() << TAG << "Discs saved.";

    for (auto const &i: m_discSongs) {
        QJsonObject record;
        record.insert("discId", i->getDiscId());
        record.insert("songId", i->getSongId());
        discSongs.push_back(QJsonValue(record));
    }

    qInfo() << TAG << "DiscSongs saved.";

    for (auto const &i: m_songs) {
        Song *song = i.second.get();

        QJsonObject record;
        record.insert("songId", song->getSongId());
        record.insert("name", song->getName());
        record.insert("authorId", song->getAuthorId());
        songs.push_back(QJsonValue(record));
    }

    qInfo() << TAG << "Songs saved.";

    for (auto const &i: m_authors) {
        Author *author = i.second.get();

        QJsonObject record;
        record.insert("authorId", author->getAuthorId());

        int type = author->getPartyType();

        if (PartyType::BAND == type) {
            Band *band = dynamic_cast<Band *>(author);
            record.insert("name", band->getName());
            bands.push_back(QJsonValue(record));
        } else if (PartyType::SINGER == type) {
            Singer *singer = dynamic_cast<Singer *>(author);
            record.insert("firstName", singer->getFirstName());
            record.insert("lastName", singer->getLastName());
            singers.push_back(QJsonValue(record));
        }
    }

    qInfo() << TAG << "Bands and Singers saved.";

    root.insert("Discs", discs);
    root.insert("DiscSongs", discSongs);
    root.insert("Songs", songs);
    root.insert("Bands", bands);
    root.insert("Singers", singers);

    QFile file;
    qInfo() << TAG << "Writing to file" << filename;
    file.setFileName(filename);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qCritical() << TAG << "Cannot open file for writing";
        throw DbException::FILE_OPEN;
    }

    QJsonDocument doc(root);

    file.write(doc.toJson(QJsonDocument::JsonFormat::Indented));
    file.close();

    qInfo() << TAG << "Database saved !";
}

Records<QVariantMap> Database::selectDiscs()
{
    Records<QVariantMap> records;

    for (auto const &i: m_discs)
        records.emplace_back(new QVariantMap({
            { "discId", (i.second)->getDiscId() },
            { "name", (i.second)->getName() }
        }));

    return records;
}

Records<QVariantMap> Database::selectSongs()
{
    Records<QVariantMap> records;

    for (auto const &i: m_songs)
        records.emplace_back(new QVariantMap({
            { "songId", (i.second)->getSongId() },
            { "name", (i.second)->getName() },
            { "authorId", (i.second)->getAuthorId() }
        }));

    return records;
}

Records<QVariantMap> Database::selectAuthors()
{
    Records<QVariantMap> records;

    for (auto const &i: m_authors)
        records.emplace_back(new QVariantMap({
            { "authorId", (i.second)->getAuthorId() },
            { "partyType", (i.second)->getPartyType() },
            { "displayName", (i.second)->getDisplayName() },
        }));


    return records;
}

Records<QVariantMap> Database::selectSingers()
{
    Records<QVariantMap> records;

    for (auto const &i: m_authors)
    {
        const auto &[authorId, author] = i;

        if (author->getPartyType() == PartyType::SINGER)
        {
            const Singer *ptr = dynamic_cast<Singer*>(author.get());

            records.emplace_back(new QVariantMap({
                { "authorId", ptr->getAuthorId() },
                { "firstName", ptr->getFirstName() },
                { "lastName", ptr->getLastName() },
            }));
        }
    }

    return records;
}

Records<QVariantMap> Database::selectBands()
{
    Records<QVariantMap> records;

    for (auto const &i: m_authors)
    {
        const auto &[authorId, author] = i;

        if (author->getPartyType() == PartyType::BAND)
        {
            Band *ptr = dynamic_cast<Band*>(author.get());

            records.emplace_back(new QVariantMap({
                { "authorId", ptr->getAuthorId() },
                { "name", ptr->getName() }
            }));
        }
    }


    return records;
}

Records<QVariantMap> Database::select(const Filter &filter)
{
    Records<QVariantMap> records;

    for (auto const &d: m_discs)
    {
        const auto &[discId, disc] = d;

        if (filter.discId >= 0 && discId != filter.discId)
            continue;

        // Joining Disc and Song tables using the DiscSongs table
        std::unordered_set<int> songsIds;

        for (auto const &v: m_discSongs)
        {
            if (v->getDiscId() == discId)
                songsIds.insert(v->getSongId());
        }

        for (auto const &s: m_songs)
        {
            const auto &[songId, song] = s;

            // Skip songs that are not linked to the current Disc
            if (!songsIds.count(songId))
                continue;

            if (filter.songId >= 0 && songId != filter.songId)
                continue;

            Author *a = m_authors[song->getAuthorId()].get();

            if (filter.authorId >= 0 && a->getAuthorId() != filter.authorId)
                continue;

            records.emplace_back(new QVariantMap({
                { "discName", disc->getName() },
                { "discId", disc->getDiscId() },
                { "songName", song->getName() },
                { "songId", song->getSongId() },
                { "authorName", a->getDisplayName() },
                { "authorId", a->getAuthorId() },
            }));
        }
    }

    if (!filter.discRequired) {
        // Find all songs that are not linked to any disc
        std::unordered_set<int> songsIds;

        for (auto const &v: m_discSongs)
            songsIds.insert(v->getSongId());

        for (auto const &s: m_songs)
        {
            const auto &[songId, song] = s;

            // Skip songs that are link to some disc
            if (songsIds.count(songId))
                continue;

            if (filter.songId >= 0 && songId != filter.songId)
                continue;

            Author *a = m_authors[song->getAuthorId()].get();

            if (filter.authorId >= 0 && a->getAuthorId() != filter.authorId)
                continue;

            records.emplace_back(new QVariantMap({
                { "discName", "" },
                { "discId", "-1" },
                { "songName", song->getName() },
                { "songId", song->getSongId() },
                { "authorName", a->getDisplayName() },
                { "authorId", a->getAuthorId() },
            }));
        }
    }

    return records;
}

int Database::insertDisc(const QString &name)
{
    int lock = switchUpdatingState();
    int id = ++m_ids.maxDiscId;
    m_discs.emplace(id, std::make_unique<Disc>(id, name));
    emit discsChanged();
    switchUpdatingState(lock);
    return id;
}

int Database::insertSong(const QString &name, int authorId)
{
    int lock = switchUpdatingState();
    int id = ++m_ids.maxSongId;
    m_songs.emplace(id, std::make_unique<Song>(id, authorId, name));
    emit songsChanged();
    switchUpdatingState(lock);
    return id;
}

int Database::insertSinger(const QString &firstName, const QString &lastName)
{
    int lock = switchUpdatingState();
    int id = ++m_ids.maxAuthorId;
    m_authors.emplace(id, std::make_unique<Singer>(id, firstName, lastName));
    emit authorsChanged();
    switchUpdatingState(lock);
    return id;
}

int Database::insertBand(const QString &name)
{
    int lock = switchUpdatingState();
    int id = ++m_ids.maxAuthorId;
    m_authors.emplace(id, std::make_unique<Band>(id, name));
    emit authorsChanged();
    switchUpdatingState(lock);
    return id;
}

void Database::insertDiscSong(int discId, int songId)
{
    int lock = switchUpdatingState();
    m_discSongs.push_back(std::make_unique<DiscSong>(discId, songId));
    emit discSongsChanged();
    switchUpdatingState(lock);
}

void Database::updateDisc(const Disc &udisc)
{
    auto it = m_discs.find(udisc.getDiscId());

    if (it != m_discs.end()) {
        it->second = std::make_unique<Disc>(udisc);
        int lock = switchUpdatingState();
        emit discsChanged();
        switchUpdatingState(lock);
    }

}

void Database::updateSong(const Song &usong)
{
    auto it = m_songs.find(usong.getSongId());

    if (it != m_songs.end()) {
        it->second = std::make_unique<Song>(usong);
        int lock = switchUpdatingState();
        emit songsChanged();
        switchUpdatingState(lock);
    }

}

void Database::updateSinger(const Singer &usinger)
{
    auto it = m_authors.find(usinger.getAuthorId());

    if (it != m_authors.end()) {
        it->second = std::make_unique<Singer>(usinger);
        int lock = switchUpdatingState();
        emit authorsChanged();
        switchUpdatingState(lock);
    }

}

void Database::updateBand(const Band &uband)
{
    auto it = m_authors.find(uband.getAuthorId());

    if (it != m_authors.end()) {
        it->second = std::make_unique<Band>(uband);
        int lock = switchUpdatingState();
        emit authorsChanged();
        switchUpdatingState(lock);
    }

}

void Database::removeDisc(int discId)
{
    if (m_discs.erase(discId)) {
        // Delete all DiscSongs associated with the disc
        int lock = switchUpdatingState();
        removeDiscSong(discId, -1);
        emit discsChanged();
        switchUpdatingState(lock);
    }

}

void Database::removeSong(int songId)
{
    if (m_songs.erase(songId)) {
        // Delete all DiscSongs associated with the song
        int lock = switchUpdatingState();
        removeDiscSong(-1, songId);
        emit beginUpdate();
        emit songsChanged();
        switchUpdatingState(lock);
    }

}

void Database::removeAuthor(int authorId)
{
    if (!m_authors.erase(authorId))
        return;

    int lock = switchUpdatingState();
    std::unordered_set<int> songsIds;

    // Delete all songs belonging to the author
    for(auto i = m_songs.begin(); i != m_songs.end(); ) {
        if (i->second->getAuthorId() == authorId)
        {
            songsIds.insert(i->second->getSongId());
            i = m_songs.erase(i);
        }
        else
            ++i;
    }

    // Delete all DiscSongs associated with the songs
    m_discSongs.erase(
        remove_if(m_discSongs.begin(), m_discSongs.end(),
            [&songsIds](std::unique_ptr<DiscSong> &ds)
            {
                return songsIds.count(ds->getSongId());
            }),
        m_discSongs.end()
    );

    emit authorsChanged();

    if (songsIds.size() > 0) {
        emit songsChanged();
        emit discSongsChanged();
    }

    switchUpdatingState(lock);
}

void Database::removeDiscSong(int discId, int songId)
{
    int lock = switchUpdatingState();

    m_discSongs.erase(
        remove_if(m_discSongs.begin(), m_discSongs.end(),
            [discId, songId](std::unique_ptr<DiscSong> &ds)
            {
                return (discId < 0 || ds->getDiscId() == discId)
                        && (songId < 0 || ds->getSongId() == songId);
            }),
        m_discSongs.end()
    );

    emit discSongsChanged();
    switchUpdatingState(lock);
}
