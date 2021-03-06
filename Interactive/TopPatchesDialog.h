/*=========================================================================
 *
 *  Copyright David Doria 2012 daviddoria@gmail.com
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

#ifndef TopPatchesDialog_H
#define TopPatchesDialog_H

#include "ui_TopPatchesDialog.h"

// Qt
#include <QDialog>
#include <QObject>

// Custom
#include "Node.h"
#include "Interactive/ModelView/ListModelPatches.h"

// Submodules
#include <Mask/Mask.h>

/** This class is necessary because a class template cannot have the Q_OBJECT macro directly. */
class TopPatchesDialogParent : public QDialog, public Ui::TopPatchesDialog
{
Q_OBJECT

public:
  /** Constructor. */
  TopPatchesDialogParent(QWidget* parent = nullptr) : QDialog(parent) {}

signals:
  virtual void signal_SelectedRegion(const itk::ImageRegion<2>&) const;

public slots:

  /** Set the source nodes. Ideally this would be templated on the node type, but since it is a slot it cannot be templated. */
  virtual void SetSourceNodes(const std::vector<Node>& sourceNodes) = 0;

  /** Set the query node. */
  virtual void SetQueryNode(const Node& queryNode) = 0;

  /** The slot to handle when the btnSavePair button is clicked. */
  virtual void on_btnSavePair_clicked() = 0;

  /** When the user clicks a patch in the list, how the proposed completed patch and emit
    * a signal so viewers can update their views. */
  virtual void slot_SingleClicked(const QModelIndex & index) = 0;

  /** When the user double clicks a patch in the list, close the dialog and return that patch as the one to use. */
  virtual void slot_DoubleClicked(const QModelIndex & index) = 0;

  /** Update the source and target region outlines and display the source and target patches.*/
  virtual void slot_UpdateResult(const itk::ImageRegion<2>& sourceRegion,
                                 const itk::ImageRegion<2>& targetRegion) = 0;

  /** The slot to handle when the btnSelectManually button is clicked. */
  virtual void on_btnSelectManually_clicked() = 0;
};

/** This class displays a set of patches in an ordered list. */
template <typename TImage>
class TopPatchesDialog : public TopPatchesDialogParent
{
private:
  /** The image that will be displayed, and the from which the patches will be extracted before being displayed. */
  const TImage* Image;

  /** The mask to use. */
  const Mask* MaskImage;

  /** The query node. */
  Node QueryNode;

  /** Store the selected node. */
  Node SelectedNode;

  /** Indicates if the selection was valid. */
  bool ValidSelection = false;

  /** The half-width of the patch. */
  unsigned int PatchHalfWidth;

  /** The scene for the query patch.*/
  QGraphicsScene* QueryPatchScene;

  /** The item for the query patch.*/
  QGraphicsPixmapItem* MaskedQueryPatchItem;

  /** The scene for the proposed patch.*/
  QGraphicsScene* SourcePatchScene;

  /** The item for the proposed patch.*/
  QGraphicsPixmapItem* SourcePatchItem;

  /** The scene for the proposed patch.*/
  QGraphicsScene* ProposedPatchScene;

  /** The item for the proposed patch.*/
  QGraphicsPixmapItem* ProposedPatchItem;

  /** The color to use as the background of the QGraphicsScenes */
  QColor SceneBackground;

  /** The model to use to display the patches. */
  ListModelPatches<TImage>* PatchesModel;

  /** These actions are performed when the widget is displayed. */
  void showEvent(QShowEvent* event);

  /** Gracefully quit if the user closes the dialog. */
  void closeEvent(QCloseEvent* event);

  /** Show the selected patch. */
  void DisplayPatchSelection(QModelIndex index);

  /** This variable tracks how many times the top patch was selected from the list. */
  unsigned int NumberOfVerifications = 0;

  /** This variable tracks how many times a non-top patch was selected from the list. */
  unsigned int NumberOfCorrections = 0;

  /** This variable tracks how many times the patch was selected by manually positioning. */
  unsigned int NumberOfManualSelections = 0;

public:
  /** Constructor */
  TopPatchesDialog(const TImage* const image, const Mask* const mask,
                   const unsigned int patchHalfWidth, QWidget* parent = nullptr);

  /** Output information about how many selections were used. */
  void Report();

//  /** Destructor. */
//  ~TopPatchesDialog();

  /** Set the source nodes from which the user can choose. */
  void SetSourceNodes(const std::vector<Node>& nodes);

  /** Set the source nodes to display. */
  template <typename TNode>
  void SetSourceNodes(const std::vector<TNode>& sourceNodes);

  /** Set the query node that the user will choose the best match to. */
  void SetQueryNode(const Node& node);

  /** Catch the signal that the ListView emits when it is clicked. */
  void slot_SingleClicked(const QModelIndex & index);

  /** Catch the signal that the ListView emits when it is double clicked. */
  void slot_DoubleClicked(const QModelIndex & index);

  /** The slot to handle when the btnSavePair button is clicked. */
  void on_btnSavePair_clicked();

  /** Update the source and target region outlines and display the source and target patches.*/
  void slot_UpdateResult(const itk::ImageRegion<2>& sourceRegion,
                         const itk::ImageRegion<2>& targetRegion);

  /** The slot to handle when the btnSelectManually button is clicked. */
  void on_btnSelectManually_clicked();

  /** Get the node that user selected. */
  Node GetSelectedNode() const;

  /** The list of nodes to display. */
  std::vector<Node> Nodes;
  //void on_btnRefresh_clicked();

  /** Check if the selection is valid. */
  bool IsSelectionValid() const;

  /** After the listView has been clicked, this variable contains which item is selected. */
  QModelIndex SelectedIndex;

  void PositionNextToParent();
};

#include "TopPatchesDialog.hpp"

#endif // TopPatchesDialog_H
