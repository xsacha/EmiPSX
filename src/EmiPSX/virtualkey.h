/***********************************************************
 Copyright (C) 2013 AndreBotelho(andrebotelhomail@gmail.com)
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the
 Free Software Foundation, Inc.,
 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307 USA.
 ***********************************************************/
 
#ifndef VIRTUALKEY_H
#define VIRTUALKEY_H
#include <Qt>
#include <QRect>
#include <QImage>
#include <QPixmap>

class VirtualKey{
public:
    QRect rect;
    Qt::Key key;
    int iPressed;
    QImage img;
    QImage pressimg;
    QPixmap pix;
    VirtualKey(QRect _rect, Qt::Key _key);
    VirtualKey();
    ~VirtualKey();
    bool isOver(QPoint mpoint);
    QImage getImage();
};

#endif // VIRTUALKEY_H
