#ifndef KONQBOOKMARKXMLIO_H
#define KONQBOOKMARKXMLIO_H

class KonqBookmark;

class QIODevice;

class KonqBookmarkXmlIo
{
public:
  static bool writeBookmarkToXml( const KonqBookmark &konqBookmark, QIODevice *device );

  static bool readBookmarkFromXml( QIODevice *device, KonqBookmark &konqBookmark );
};

#endif /* KONQBOOKMARKXMLIO_H */
