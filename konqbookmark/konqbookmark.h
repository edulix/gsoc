#ifndef KONQBOOKMARK_H
#define KONQBOOKMARK_H

#include <QtCore/QSharedDataPointer>

class QDateTime;
class QString;
class QUrl;

template <class T> class QList;

class KonqBookmark
{
public:

  KonqBookmark();

  KonqBookmark( const KonqBookmark &other );

  ~KonqBookmark();

  KonqBookmark &operator=( const KonqBookmark &other );

  void setUrl( const QUrl &url );

  QUrl url() const;

  void setTitle( const QString &title );

  QString title() const;

private:
  class Private;
  QSharedDataPointer<Private> d;
};

#endif /* KONQBOOKMARK_H */
