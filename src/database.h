#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QVector>
#include <map>

#include "model/disc.h"
#include "model/song.h"
#include "model/author.h"
#include "model/band.h"
#include "model/singer.h"
#include "model/discsong.h"

#undef TAG
#define TAG "[Database]"

enum DbException
{
    FILE_OPEN,
    BAD_CONTENT
};

struct Increments {
    int maxDiscId = -1;
    int maxSongId = -1;
    int maxAuthorId = -1;
};

struct Filter {
    int discId = -1;
    int songId = -1;
    int authorId = -1;
    bool discRequired = true;
};

template <class T>
using Records = std::vector<std::unique_ptr<T>>;

class Database: public QObject
{
    Q_OBJECT

private:
    Increments m_ids;
    Records<DiscSong> m_discSongs;
    std::map<int, std::unique_ptr<Disc>> m_discs;
    std::map<int, std::unique_ptr<Song>> m_songs;
    std::map<int, std::unique_ptr<Author>> m_authors;

    int m_updatingState = 0;
    int switchUpdatingState(int lock = 0);

public:
    explicit Database(QObject *parent = nullptr);

    void loadFromJsonFile(QString filename);
    void saveToJsonFile(QString filename);
    int insertDisc(const QString &name);
    int insertSong(const QString &name, int authorId);
    int insertSinger(const QString &firstName, const QString &lastName);
    int insertBand(const QString &name);
    void insertDiscSong(int discId, int songId);
    void updateDisc(const Disc &udisc);
    void updateSong(const Song &usong);
    void updateSinger(const Singer &usinger);
    void updateBand(const Band &uband);
    void removeDisc(int discId);
    void removeSong(int songId);
    void removeAuthor(int authorId);
    void removeDiscSong(int discId, int songId);
    Records<QVariantMap> selectDiscs();
    Records<QVariantMap> selectSongs();
    Records<QVariantMap> selectAuthors();
    Records<QVariantMap> selectSingers();
    Records<QVariantMap> selectBands();
    Records<QVariantMap> select(const Filter &filter);

signals:
    void discsChanged();
    void songsChanged();
    void authorsChanged();
    void discSongsChanged();
    void beginUpdate();
    void endUpdate();
};

#endif // DATABASE_H
