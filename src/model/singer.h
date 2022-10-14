#ifndef SINGER_H
#define SINGER_H

#include <QString>

#include "author.h"

class Singer: public Author
{
    QString m_firstName;
    QString m_lastName;

public:
    Singer(int authorId, const QString &firstName, const QString &lastName)
        : Author(authorId, PartyType::SINGER) {
        m_firstName = firstName;
        m_lastName = lastName;
    };

    const QString &getFirstName() const {
        return m_firstName;
    };

    const QString &getLastName() const {
        return m_lastName;
    };

    QString getDisplayName() {
        return QString("%1 %2").arg(m_firstName, m_lastName);
    };
};

#endif // SINGER_H
