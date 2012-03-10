/*******************************************************************************
**
** Photivo
**
** Copyright (C) 2011 Bernd Schoeler <brjohn@brother-john.net>
**
** This file is part of Photivo.
**
** Photivo is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License version 3
** as published by the Free Software Foundation.
**
** Photivo is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Photivo.  If not, see <http://www.gnu.org/licenses/>.
**
*******************************************************************************/

#include <QCursor>
#include <QFontMetrics>

#include "../ptDefines.h"
#include "../ptTheme.h"
#include "../ptSettings.h"

#include "ptFileMgrDM.h"
#include "ptGraphicsThumbGroup.h"
#include "ptGraphicsSceneEmitter.h"

extern ptSettings* Settings;
extern ptTheme* Theme;

//==============================================================================

/*static*/
ptGraphicsThumbGroup* ptGraphicsThumbGroup::AddRef(ptGraphicsThumbGroup* group /*== NULL*/) {
  if (group == NULL) {
    return new ptGraphicsThumbGroup;
  } else {
    group->m_RefCount++;
    return group;
  }
}

//==============================================================================

/*static*/
int ptGraphicsThumbGroup::RemoveRef(ptGraphicsThumbGroup* group) {
  int result = --group->m_RefCount;

  if (group->m_RefCount == 0) {
    delete group;
    group = NULL;
  }
  return result;
}

//==============================================================================

ptGraphicsThumbGroup::ptGraphicsThumbGroup(QGraphicsItem* parent /*= 0*/)
: QGraphicsRectItem(parent),
  m_Brush(Qt::SolidPattern),
  m_hasHover(false),
  m_Pen(Qt::DashLine)
{
  m_FSOType = fsoUnknown;
  m_FullPath = "";
//  m_Pixmap = NULL;
  m_ImgTypeText = NULL;
  m_InfoText = NULL;
  m_RefCount = 1;
  m_Thumbnail = NULL;

  setFlags(QGraphicsItem::ItemIsFocusable);
  setAcceptHoverEvents(true);
  setAcceptedMouseButtons(Qt::LeftButton);
  setFiltersChildEvents(true);
  setCursor(QCursor(Qt::PointingHandCursor));

  m_Pen.setCosmetic(true);
  SetupPenAndBrush();
}

//==============================================================================

ptGraphicsThumbGroup::~ptGraphicsThumbGroup() {
  DelAndNull(m_Thumbnail);
}

//==============================================================================

void ptGraphicsThumbGroup::addInfoItems(const QString fullPath,
                                        const QString description,
                                        const ptFSOType fsoType)
{
// A thumbnail’s geometry:
// Height:                                  Width:
//   InnerPadding                             InnerPadding
//   m_Pixmap->pixmap().height()              m_Pixmap->pixmap().width()
//   InnerPadding                             InnerPadding
//   m_InfoText->boundingRect().height()
//   InnerPadding
//
  m_FSOType = fsoType;
  m_FullPath = fullPath;
  qreal ThumbSize = (qreal)Settings->GetInt("FileMgrThumbnailSize");

  // main description text: currently just the filename
  if (m_InfoText == NULL) {
    m_InfoText = new QGraphicsSimpleTextItem;
    m_InfoText->setParentItem(this);
  }
  m_InfoText->setText(QFontMetrics(m_InfoText->font()).elidedText(description,
                                                                  Qt::ElideRight,
                                                                  (int)ThumbSize));
  m_InfoText->setBrush(QBrush(Theme->textColor()));
  m_InfoText->setPos(InnerPadding, ThumbSize + InnerPadding*2);

  // file type display in topleft corner (images only)
  if (fsoType == fsoFile) {
    int SuffixStart = fullPath.lastIndexOf(".");
    QString Suffix = SuffixStart == -1 ? "" : fullPath.mid(SuffixStart + 1);

    if (m_ImgTypeText == NULL) {
      m_ImgTypeText = new QGraphicsSimpleTextItem;
      QFont tempFont = m_ImgTypeText->font();
      tempFont.setBold(true);
      m_ImgTypeText->setFont(tempFont);
      m_ImgTypeText->setBrush(QBrush(Theme->textColor()));
      m_ImgTypeText->setPos(InnerPadding, InnerPadding);
      m_ImgTypeText->setParentItem(this);
    }

    m_ImgTypeText->setText(Suffix.toUpper());
  }

  // set rectangle size for the hover border
  this->setRect(0,
                0,
                ThumbSize + InnerPadding*2,
                ThumbSize + InnerPadding*3 + m_InfoText->boundingRect().height());
}

//==============================================================================

void ptGraphicsThumbGroup::addImage(QImage* image) {
  qreal ThumbSize = (qreal)Settings->GetInt("FileMgrThumbnailSize");
  if (m_Thumbnail) {
    delete m_Thumbnail;
  }
  m_Thumbnail = image;

  // center pixmap in the cell if it is not square
  // the +2 offset is for the hover border
  m_ThumbPos.setX(ThumbSize/2 - image->width()/2  + InnerPadding + 0.5);
  m_ThumbPos.setY(ThumbSize/2 - image->height()/2 + InnerPadding + 0.5);
  this->update();

/*
  When working with QPixmap/QPixmapItem Photivo hangs on Linux as soon as the
  pixmap item is parented to the group and the thumbnail cache is full. Why?
  Very good question! As a workaround we keep the QImage from the thumbnailer
  and paint it manually in the paint() method, see below.
*/

//  qreal ThumbSize = (qreal)Settings->GetInt("FileMgrThumbnailSize");
//  if (!m_Pixmap) {
//    m_Pixmap = new QGraphicsPixmapItem();
//    m_Pixmap->setZValue(-1);
//  }
//  m_Pixmap->setPixmap(QPixmap::fromImage(*image));

//  // center pixmap in the cell if it is not square
//  // the +2 offset is for the hover border
//  m_Pixmap->setPos(ThumbSize/2 - image->width()/2  + InnerPadding + 0.5,
//                   ThumbSize/2 - image->height()/2 + InnerPadding + 0.5);

//  m_Pixmap->setParentItem(this);
//  DelAndNull(image);
}

//==============================================================================

bool ptGraphicsThumbGroup::sceneEvent(QEvent* event) {
  if (ptFileMgrDM::GetInstance()->closing()) return true;

  if (ptFileMgrDM::GetInstance()->tryLock()) {
    switch (event->type()) {
      case QEvent::GraphicsSceneHoverEnter: {
        m_hasHover = true;
        this->update();
        break;
      }

      case QEvent::GraphicsSceneHoverLeave: {
        m_hasHover = false;
        this->update();
        break;
      }

      case QEvent::GraphicsSceneMousePress: {
        // Must accept mouse press to get mouse release as well.
        event->accept();
        this->setFocus(Qt::MouseFocusReason);
        ptGraphicsSceneEmitter::EmitFocusChanged();
        break;
      }

      case QEvent::GraphicsSceneMouseDoubleClick: {
        exec();
        break;
      }

      case QEvent::GraphicsSceneMouseRelease: {
        event->accept();
        break;
      }

      case QEvent::KeyPress: {
        QKeyEvent* e = (QKeyEvent*)event;
        if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
          exec();
          break;
        }
        ptFileMgrDM::GetInstance()->unlock();
        return QGraphicsRectItem::sceneEvent(event);
      }

      case QEvent::GraphicsSceneContextMenu: {
        // We set the focus but we don't accept the event
        this->setFocus(Qt::MouseFocusReason);
        ptGraphicsSceneEmitter::EmitFocusChanged();
        ptFileMgrDM::GetInstance()->unlock();
        return QGraphicsRectItem::sceneEvent(event);
      }

      default: {
        ptFileMgrDM::GetInstance()->unlock();
        return QGraphicsRectItem::sceneEvent(event);
      }
    }
    ptFileMgrDM::GetInstance()->unlock();
  }

  return true;
}

//==============================================================================

void ptGraphicsThumbGroup::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
  if (ptFileMgrDM::GetInstance()->closing()) return;

  SetupPenAndBrush();
  painter->setPen(m_Pen);
  painter->setBrush(m_Brush);
  painter->drawRoundedRect(this->rect(), 5, 5);
  if (m_Thumbnail) {
    painter->drawImage(m_ThumbPos.x(), m_ThumbPos.y(), *m_Thumbnail);
  }
}

//==============================================================================

QFont ptGraphicsThumbGroup::font() const {
  if (m_InfoText == NULL) {
    return QApplication::font();
  } else {
    return m_InfoText->font();
  }
}

//==============================================================================

void ptGraphicsThumbGroup::exec() {
  if (ptFileMgrDM::GetInstance()->m_ClickBusy.tryLock(500)) {
    if (m_InfoText) {
      if (m_FSOType == fsoFile) {
        ptGraphicsSceneEmitter::EmitThumbnailAction(tnaLoadImage, m_FullPath);
      } else {
        ptGraphicsSceneEmitter::EmitThumbnailAction(tnaChangeDir, m_FullPath);
      }
    }
    ptFileMgrDM::GetInstance()->m_ClickBusy.unlock();
  }
}

//==============================================================================

void ptGraphicsThumbGroup::SetupPenAndBrush() {
  int PenWidth = m_hasHover ? 2 : 1;

  if (this->hasFocus()) {
    m_Pen.setColor(Theme->highlightColor());
    m_Pen.setWidth(PenWidth);
    m_Brush.setColor(Theme->gradientColor());

  } else {
    m_Pen.setColor(Theme->emphasizedColor());
    m_Pen.setWidth(PenWidth);
    m_Brush.setColor(Theme->altBaseColor());
  }
}

//==============================================================================
