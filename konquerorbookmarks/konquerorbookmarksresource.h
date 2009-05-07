#ifndef KONQUERORBOOKMARKSRESOURCE_H
#define KONQUERORBOOKMARKSRESOURCE_H

#include <QtCore/QSharedDataPointer>
#include <akonadi/resourcebase.h>

class KonquerorBookmarksResource : public Akonadi::ResourceBase,
                           public Akonadi::AgentBase::Observer
{
Q_OBJECT

public:
    KonquerorBookmarksResource( const QString &id );
    ~KonquerorBookmarksResource();

public Q_SLOTS:
    virtual void configure( WId windowId );

protected Q_SLOTS:
    void retrieveCollections();
    void retrieveItems( const Akonadi::Collection &col );
    bool retrieveItem( const Akonadi::Item &item, const QSet<QByteArray> &parts );

protected:
    virtual void aboutToQuit();

    virtual void itemAdded( const Akonadi::Item &item, const Akonadi::Collection &collection );
    virtual void itemChanged( const Akonadi::Item &item, const QSet<QByteArray> &parts );
    virtual void itemRemoved( const Akonadi::Item &item );
    
private:
    class Private;
    QSharedDataPointer<Private> d;
};

#endif
