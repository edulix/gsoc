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

#ifndef KONQUEROR_LOCATION_BAR_H
#define KONQUEROR_LOCATION_BAR_H

#include "konqbookmark_export.h"
#include "klineeditview.h"

#include <QObject>

namespace Konqueror
{
    class KONQBOOKMARK_EXPORT LocationBar : public KLineEditView
    {
        Q_OBJECT
    public:
        LocationBar(QWidget* parent = 0);
        virtual ~LocationBar();
        
    protected:
        void init();
        
    private:
        class Private;
        Private* const d;
    };
}

#endif // KONQUEROR_LOCATION_BAR_H
