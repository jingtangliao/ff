/*=========================================================================
 *
 *  Copyright David Doria 2011 daviddoria@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#ifndef TableModelForwardLook_H
#define TableModelForwardLook_H

// Qt
#include <QAbstractTableModel>
#include <QItemSelection>

// STL
#include <vector>

// Custom
#include "CandidatePairs.h"
#include "DebugOutputs.h"
#include "DisplayStyle.h"
#include "InpaintingIterationRecord.h"

class TableModelForwardLook : public QAbstractTableModel, public DebugOutputs
{
public:
  TableModelForwardLook(QObject * parent, std::vector<InpaintingIterationRecord>& iterationRecords, DisplayStyle& style);

  int rowCount(const QModelIndex& parent) const;
  int columnCount(const QModelIndex& parent) const;
  QVariant data(const QModelIndex& index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

  Qt::ItemFlags flags(const QModelIndex& index) const;

  void SetIterationToDisplay(const unsigned int);
  void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

  void Refresh();

  void SetPatchDisplaySize(const unsigned int value);

protected:

  std::vector<InpaintingIterationRecord>& IterationRecords;

  unsigned int IterationToDisplay;

  unsigned int PatchDisplaySize;

  DisplayStyle& ImageDisplayStyle;
};

#endif
