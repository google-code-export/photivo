/*******************************************************************************
**
** Photivo
**
** Copyright (C) 2011-2012 Bernd Schoeler <brjohn@brother-john.net>
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

#include "../ptConstants.h"
#include "../ptTheme.h"
#include "../ptGuiOptions.h"
#include "ptRepairSpotItemDelegate.h"
#include "ptRepairSpotEditor.h"
#include "ptRepairSpot.h"
#include "ptRepairSpotModel.h"

extern ptTheme* Theme;
extern ptGuiOptions* GuiOptions;

//==============================================================================

ptRepairSpotItemDelegate::ptRepairSpotItemDelegate(QObject *AParent)
: QStyledItemDelegate(AParent)
{}

//==============================================================================

QWidget* ptRepairSpotItemDelegate::createEditor(QWidget *parent,
                                                const QStyleOptionViewItem&,
                                                const QModelIndex &index) const
{
  return new ptRepairSpotEditor(
    parent,
    (int)qobject_cast<const ptRepairSpotModel*>(index.model())->spot(index.row())->algorithm()
  );
}

//==============================================================================

void ptRepairSpotItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
//  ptRepairSpotEditor *ed = qobject_cast<ptRepairSpotEditor*>(editor);
//  ed->AlgoCombo->setCurrentIndex(
//        static_cast<ptRepairSpot*>(RepairSpotList->at(index.row()))->algorithm() );
}

//==============================================================================

void ptRepairSpotItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                            const QModelIndex &index) const
{
  ptRepairSpotEditor *hEd   = qobject_cast<ptRepairSpotEditor*>(editor);
  model->setData(index,
                 GuiOptions->SpotRepair[hEd->AlgoCombo->currentIndex()].Text,
                 Qt::DisplayRole);
}

//==============================================================================
