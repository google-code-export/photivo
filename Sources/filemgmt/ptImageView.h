/*******************************************************************************
**
** Photivo
**
** Copyright (C) 2011 Bernd Schoeler <brjohn@brother-john.net>
** Copyright (C) 2011-2013 Michael Munzert <mail@mm-log.com>
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

#ifndef PTIMAGEVIEW_H
#define PTIMAGEVIEW_H

//==============================================================================

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGridLayout>
#include <QThread>

#include "../ptReportOverlay.h"
#include "ptFileMgrDM.h"
#include "ptThumbDefines.h"
#include "ptThumbCache.h"

//==============================================================================

class MyWorker;

//==============================================================================

class ptImageView: public QGraphicsView {
Q_OBJECT
public:
  /*! Creates a \c ptImageView instance.
    \param parent
      The image view’s parent widget.
  */
  explicit ptImageView(QWidget *parent = 0, ptFileMgrDM* DataModule = 0);
  ~ptImageView();

  void ShowImage(const QString AFileName);


protected:
  void contextMenuEvent(QContextMenuEvent* event);
  void mouseDoubleClickEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void mousePressEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  void resizeEvent(QResizeEvent* event);
  void showEvent(QShowEvent* event);
  void hideEvent(QHideEvent*);
  void wheelEvent(QWheelEvent* event);


private:
  void ZoomStep(int direction);
  void ZoomTo(float factor, const bool withMsg);  // 1.0 means 100%

  /*! Put the QImage in the scene */
  void ImageToScene(const double Factor);

  /*! This function performs the actual thumbnail generation. */
  void updateView();

  ptFileMgrDM*          m_DataModule;
  const float           MinZoom;
  const float           MaxZoom;
  QList<float>          ZoomFactors;   // steps for wheel zoom
  QGridLayout*          m_parentLayout;
  QGraphicsScene*       m_Scene;
  QString               m_FileName_Current;
  int                   m_ZoomMode;
  float                 m_ZoomFactor;
  int                   m_Zoom;
  QLine*                m_DragDelta;
  bool                  m_LeftMousePressed;
  ptReportOverlay*      m_ZoomSizeOverlay;
  ptReportOverlay*      m_StatusOverlay;
  QGraphicsPixmapItem*  m_PixmapItem;
  int                   m_ResizeTimeOut;
  QTimer*               m_ResizeTimer;
  QTimer                m_ResizeEventTimer;  // to avoid jerky UI during widget resize
                                             // in zoom fit mode

  QAction* ac_Zoom100;
  QAction* ac_ZoomIn;
  QAction* ac_ZoomFit;
  QAction* ac_ZoomOut;

  ptThumbId             FNextImage;
  ptThumbId             FCurrentImage;
  ptThumbPtr            FImage;

public slots:
  int  zoomFit(const bool withMsg = true);  // fit complete image into viewport
  void zoom100();
  void zoomIn();
  void zoomOut();

private slots:
  void ResizeTimerExpired();
  void getImage(const ptThumbId AThumbId,
                ptThumbPtr      AImage);
};

//==============================================================================

#endif // PTIMAGEVIEW_H
