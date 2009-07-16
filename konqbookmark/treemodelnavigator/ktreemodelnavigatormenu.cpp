/*
   Copyright (C) 2009 by Rahman Duran <rahman.duran@gmail.com>
   Copyright (c) 2009 Eduardo Robles Elvira <edulix@gmail.com>
                                                                          
   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU Library General Public License as published
   by the Free Software Foundation; either version 2 of the License or      
   ( at your option ) version 3 or, at the discretion of KDE e.V.           
   ( which shall act as a proxy as in section 14 of the GPLv3 ), any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of 
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.                 

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to     
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,     
   Boston, MA 02110-1301, USA.  
*/

#include "ktreemodelnavigatormenu_p.h"
#include "ktreemodelnavigator.h"

#include <QtGui/QKeyEvent>
#include <QStringList>


KTreeModelNavigatorMenu::KTreeModelNavigatorMenu(KTreeModelNavigator *treeModelNavigator, QWidget* parent) :
    KMenu(parent), m_treeModelNavigator(treeModelNavigator)
{
    setAcceptDrops(true);
}

KTreeModelNavigatorMenu::~KTreeModelNavigatorMenu()
{
}

void KTreeModelNavigatorMenu::dragEnterEvent(QDragEnterEvent* event)
{
    bool match = m_treeModelNavigator->haveCommonMimetypes(event->mimeData());
    if (match) {
        event->acceptProposedAction();
    }   
}

void KTreeModelNavigatorMenu::dragMoveEvent(QDragMoveEvent* event)
{      
      QMouseEvent mouseEvent(QMouseEvent(QEvent::MouseMove, event->pos(), 
          Qt::LeftButton, event->mouseButtons(), event->keyboardModifiers()));
      mouseMoveEvent(&mouseEvent);
}

void KTreeModelNavigatorMenu::dropEvent(QDropEvent* event)
{
    QAction* action = actionAt(event->pos());
    if (action != 0) {
        emit mimeDataDropped(action, event);
    }
}

#include "ktreemodelnavigatormenu_p.moc"
