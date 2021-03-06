/*******************************************************************************
**
** Photivo
**
** Copyright (C) 2008 Jos De Laender <jos.de_laender@telenet.be>
** Copyright (C) 2009,2010 Michael Munzert <mail@mm-log.com>
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

#ifndef PTPROCESSOR_H
#define PTPROCESSOR_H

//==============================================================================
                        
#include "ptConstants.h"

#include <exiv2/exif.hpp>

#include <QString>
#include <QTime>
#include <QCoreApplication>

#include <vector>

//==============================================================================

// forward for faster compilation
class ptDcRaw;
class ptImage;

//==============================================================================

typedef void (*PReportProgressFunc)(const QString);

//==============================================================================

class ptProcessor {
  Q_DECLARE_TR_FUNCTIONS(ptProcessor)

public:
  /*! Creates a new processor instance.
    \param AReportProgress
      A function pointer to the progress report function.
    \param ARunLocalSpots
      A pointer to the \c RunFiltering() function of the “local adjust” spot model.
      If you do not set a valid pointer in the constructor you MUST call \c setSpotFuncs()
      before the first run of the processor instance.
    \param ARunRepairSpots
      Same as \c ARunLocalSpots, but for “spot repair”.
  */
  ptProcessor(PReportProgressFunc AReportProgress);
  ~ptProcessor();

  // The associated DcRaw.
  ptDcRaw* m_DcRaw;

  /*! Main Graphical Pipe.
      Look here for all operations and all possible future extensions.
      As well Gui mode as JobMode are sharing this part of the code.
      The idea is to have an image object and operating on it.
      Run the graphical pipe from a certain point on.
  */
  void Run(short Phase,
           short SubPhase      = -1,
           short WithIdentify  = 1,
           short ProcessorMode = ptProcessorMode_Preview);

  /*! Rerun Local Edit stage. */
  void RunLocalEdit(ptProcessorStopBefore StopBefore = ptProcessorStopBefore::NoStop);

  /*! Rerun Geometry stage (and stop for crop or rotate tool)
   Use the ptProcessorStopBefore_{Rotate|Crop} constants to stop early.*/
  void RunGeometry(ptProcessorStopBefore StopBefore = ptProcessorStopBefore::NoStop);

  // Exif Related
  Exiv2::ExifData      m_ExifData;
  std::vector<uint8_t> m_ExifBuffer;
  void                 ReadExifBuffer();

  // Cached image versions at different points.
  ptImage*  m_Image_AfterDcRaw;
  ptImage*  m_Image_AfterLocalEdit;
  ptImage*  m_Image_AfterGeometry;
  ptImage*  m_Image_AfterRGB;
  ptImage*  m_Image_AfterLabCC;
  ptImage*  m_Image_AfterLabSN;
  ptImage*  m_Image_AfterLabEyeCandy;
  ptImage*  m_Image_AfterEyeCandy;

  // Cached image for detail preview
  ptImage*  m_Image_DetailPreview;

  // Sidecar image for texture overlay
  ptImage*  m_Image_TextureOverlay;
  ptImage*  m_Image_TextureOverlay2;

  // Reporting back
  //void (*m_ReportProgress)(const QString Message);
  PReportProgressFunc m_ReportProgress;

  // Reporting
  void ReportProgress(const QString Message);

  // Factor for size dependend filters
  float  m_ScaleFactor;

//==============================================================================

private:
  QTime FRunTimer;
};
#endif
