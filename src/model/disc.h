#ifndef DISC_H
#define DISC_H

#include <QString>

class Disc
{
    int m_discId;
    QString m_name;

public:
    Disc(int discId, const QString &name) {
        m_discId = discId;
        m_name = name;
    };

    int getDiscId() const {
        return m_discId;
    };

    const QString &getName() const {
        return m_name;
    };
};

#endif // DISC_H
