#ifndef SONG_H
#define SONG_H

#include <QString>

class Song
{
    int m_songId;
    int m_authorId;
    QString m_name;

public:
    Song(int songId, int authorId, const QString &name) {
        m_songId = songId;
        m_authorId = authorId;
        m_name = name;
    };

    int getSongId() const {
        return m_songId;
    };


    int getAuthorId() const {
        return m_authorId;
    };


    const QString &getName() const {
        return m_name;
    };
};

#endif // SONG_H
