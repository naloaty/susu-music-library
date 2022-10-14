#ifndef DISCSONGS_H
#define DISCSONGS_H


class DiscSong
{
    int m_discId;
    int m_songId;

public:
    DiscSong(int discId, int songId) {
        m_discId = discId;
        m_songId = songId;
    };

    int getDiscId() const {
        return m_discId;
    };

    int getSongId() const {
        return m_songId;
    };
};

#endif // DISCSONGS_H
