#ifndef AUTHOR_H
#define AUTHOR_H

#include <QString>

enum PartyType { SINGER, BAND };

class Author
{
    int m_authorId;
    int m_partyType;

public:
    Author(int authorId, int partyType) {
        m_authorId = authorId;
        m_partyType = partyType;
    };

    int getAuthorId() const {
        return m_authorId;
    };


    int getPartyType() const {
        return m_partyType;
    };

    virtual QString getDisplayName() {
        return QString("Author(type=%1, id=%2)")
                .arg(QString::number(m_partyType), QString::number(m_authorId));
    };
};

#endif // AUTHOR_H
