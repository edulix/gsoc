/*
    Copyright (c) 2009 Eduardo Robles Elvira <edulix@gmail.com>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#include "locationbarhighlighter.h"
#include <KDebug>

using namespace Konqueror;

class LocationBarHighlighter::Private
{
public:
    Private(LocationBarHighlighter *parent);
    LocationBarHighlighter *q;
    QColor secureColor;
    QColor domainColor;
    QColor normalColor;
    QRegExp regExp;
    QRegExp regExpRemote;
};

LocationBarHighlighter::Private::Private(LocationBarHighlighter* parent)
    : q(parent)
{
    secureColor = Qt::darkGreen;
    domainColor = Qt::black;
    normalColor = Qt::gray;
    regExp.setPattern("^(([^:/]+)(:/*)|[~/])([^/]*)(.*)$");
    regExpRemote.setPattern("^(http|https|ftp|sftp|fish|nfs|smb|network|remote):");
}


void LocationBarHighlighter::highlightBlock(const QString& text)
{
    if (d->regExp.indexIn(text) == -1) {
        setFormat(0, text.length(), d->domainColor);
        return;
    }
    int pos;

    // Highlight domain, but only for remotes: http, https, ftp, sftp, fish, nfs, smb,
    // network, remote.
    // If it's a remote protocol, then domain uses domainColor and the rest normalColor,
    // so that domain is black and the rest is gray
    // IF it's local, everything uses black color
    QColor normalColor = d->domainColor;
    if (d->regExpRemote.indexIn(text) != -1) {
        normalColor = d->normalColor;
    }

    // Find https
    if (text.startsWith("https")) {
        setFormat(0, 5, d->secureColor);
        if ((pos = d->regExp.pos(3)) != -1) {
            setFormat(pos, d->regExp.matchedLength(), normalColor);
        }
    } else if ((pos = d->regExp.pos(1)) != -1) {
        setFormat(pos, d->regExp.matchedLength(), normalColor);
    }

    if ((pos = d->regExp.pos(4)) != -1) {
        setFormat(pos, d->regExp.matchedLength(), d->domainColor);
    }
    if ((pos = d->regExp.pos(5)) != -1) {
        setFormat(pos, d->regExp.matchedLength(), normalColor);
    }
}

LocationBarHighlighter::LocationBarHighlighter(QTextDocument *document)
    : QSyntaxHighlighter(document), d(new Private(this))
{

}

LocationBarHighlighter::~LocationBarHighlighter()
{

}

