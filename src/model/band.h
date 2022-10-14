#ifndef BAND_H
#define BAND_H

#include <QString>

#include "author.h"

class Band: public Author
{
    QString m_name;

public:
    Band(int authorId, const QString &name)
        : Author(authorId, PartyType::BAND) {
        m_name = name;
    };

    QString getName() {
        return m_name;
    }

    QString getDisplayName() {
        return m_name;
    };
};

#endif // BAND_H
