#include "konqbookmark.h"

#include <QDateTime>
#include <QList>
#include <QString>
#include <QUrl>

class KonqBookmark::Private : public QSharedData
{
public:
    Private() {}

    Private( const Private& other ) : QSharedData( other )
    {
        mUrl = other.mUrl;
        mTitle = other.mTitle;
    }

public:
    QUrl mUrl;
    QString mTitle;
};

KonqBookmark::KonqBookmark() : d( new Private )
{
}

KonqBookmark::KonqBookmark( const KonqBookmark &other ) : d( other.d )
{
}

KonqBookmark::~KonqBookmark()
{
}

KonqBookmark &KonqBookmark::operator=( const KonqBookmark &other )
{
    if ( this != &other )
        d = other.d;

    return *this;
}

void KonqBookmark::setUrl( const QUrl &url )
{
    d->mUrl = url;
}

QUrl KonqBookmark::url() const
{
    return d->mUrl;
}


void KonqBookmark::setTitle( const QString &title )
{
    d->mTitle = title;
}

QString KonqBookmark::title() const
{
    return d->mTitle;
}
