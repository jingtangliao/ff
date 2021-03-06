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

#ifndef WeightedSSDInpainting_HPP
#define WeightedSSDInpainting_HPP

// Custom
#include "Utilities/IndirectPriorityQueue.h"

// Submodules
#include <Helpers/Helpers.h>

// Pixel descriptors
#include "PixelDescriptors/ImagePatchPixelDescriptor.h"

// Descriptor visitors
#include "Visitors/DescriptorVisitors/ImagePatchDescriptorVisitor.hpp"

// Inpainting visitors
#include "Visitors/InpaintingVisitor.hpp"
#include "Visitors/AcceptanceVisitors/DefaultAcceptanceVisitor.hpp"

// Nearest neighbors
#include "NearestNeighbor/LinearSearchBest/Property.hpp"

// Initializers
#include "Initializers/InitializeFromMaskImage.hpp"
#include "Initializers/InitializePriority.hpp"

// Inpainters
#include "Inpainters/CompositePatchInpainter.hpp"
#include "Inpainters/PatchInpainter.hpp"

// Difference functions
#include "DifferenceFunctions/ImagePatchDifference.hpp"
#include "DifferenceFunctions/WeightedSumSquaredPixelDifference.hpp"

// Utilities
#include "Utilities/PatchHelpers.h"

// Inpainting
#include "Algorithms/InpaintingAlgorithm.hpp"

// Priority
#include "Priority/PriorityCriminisi.h"

// Boost
#include <boost/graph/grid_graph.hpp>
#include <boost/property_map/property_map.hpp>

template <typename TImage>
void WeightedSSDInpainting(TImage* const originalImage, Mask* const mask, const unsigned int patchHalfWidth)
{
  itk::ImageRegion<2> fullRegion = originalImage->GetLargestPossibleRegion();

  // Blur the image
  typedef TImage BlurredImageType; // Usually the blurred image is the same type as the original image.
  typename BlurredImageType::Pointer blurredImage = BlurredImageType::New();
  float blurVariance = 2.0f;
  MaskOperations::MaskedBlur(originalImage, mask, blurVariance, blurredImage.GetPointer());

  ITKHelpers::WriteRGBImage(blurredImage.GetPointer(), "BlurredImage.png");

  typedef ImagePatchPixelDescriptor<TImage> ImagePatchPixelDescriptorType;

  // Create the graph
  typedef boost::grid_graph<2> VertexListGraphType;
  boost::array<std::size_t, 2> graphSideLengths = { { fullRegion.GetSize()[0],
                                                      fullRegion.GetSize()[1] } };
  VertexListGraphType graph(graphSideLengths);
  typedef boost::graph_traits<VertexListGraphType>::vertex_descriptor VertexDescriptorType;
  typedef boost::graph_traits<VertexListGraphType>::vertex_iterator VertexIteratorType;

  // Queue
  typedef IndirectPriorityQueue<VertexListGraphType> BoundaryNodeQueueType;
  BoundaryNodeQueueType boundaryNodeQueue(graph);

  // Create the descriptor map. This is where the data for each pixel is stored.
  typedef boost::vector_property_map<ImagePatchPixelDescriptorType,
      BoundaryNodeQueueType::IndexMapType> ImagePatchDescriptorMapType;
  ImagePatchDescriptorMapType imagePatchDescriptorMap(num_vertices(graph), boundaryNodeQueue.IndexMap);

  // Create the patch inpainter.
  typedef PatchInpainter<TImage> OriginalImageInpainterType;
  OriginalImageInpainterType originalImagePatchInpainter(patchHalfWidth, originalImage, mask);
  originalImagePatchInpainter.SetDebugImages(true);
  originalImagePatchInpainter.SetImageName("RGB");

  // Create an inpainter for the blurred image.
  typedef PatchInpainter<BlurredImageType> BlurredImageInpainterType;
  BlurredImageInpainterType blurredImagePatchInpainter(patchHalfWidth, blurredImage, mask);

  // Create a composite inpainter.
  CompositePatchInpainter inpainter;
  inpainter.AddInpainter(&originalImagePatchInpainter);
  inpainter.AddInpainter(&blurredImagePatchInpainter);

  // Create the priority function
  typedef PriorityCriminisi<BlurredImageType> PriorityType;
  PriorityType priorityFunction(blurredImage, mask, patchHalfWidth);

  // Create the descriptor visitor
  typedef ImagePatchDescriptorVisitor<VertexListGraphType, TImage, ImagePatchDescriptorMapType>
      ImagePatchDescriptorVisitorType;
  ImagePatchDescriptorVisitorType imagePatchDescriptorVisitor(originalImage, mask, imagePatchDescriptorMap, patchHalfWidth);

  typedef DefaultAcceptanceVisitor<VertexListGraphType> AcceptanceVisitorType;
  AcceptanceVisitorType acceptanceVisitor;

  // Create the inpainting visitor
  typedef InpaintingVisitor<VertexListGraphType, BoundaryNodeQueueType,
                            ImagePatchDescriptorVisitorType, AcceptanceVisitorType, PriorityType, TImage>
                            InpaintingVisitorType;
  InpaintingVisitorType inpaintingVisitor(mask, boundaryNodeQueue,
                                          imagePatchDescriptorVisitor, acceptanceVisitor,
                                          &priorityFunction, patchHalfWidth, "InpaintingVisitor", originalImage);
  inpaintingVisitor.SetAllowNewPatches(false);
  inpaintingVisitor.SetDebugImages(true);

  InitializePriority(mask, boundaryNodeQueue, &priorityFunction);

  // Initialize the boundary node queue from the user provided mask image.
  InitializeFromMaskImage<InpaintingVisitorType, VertexDescriptorType>(mask, &inpaintingVisitor);
  std::cout << "PatchBasedInpaintingNonInteractive: There are " << boundaryNodeQueue.CountValidNodes()
            << " nodes in the boundaryNodeQueue" << std::endl;

  // Create the nearest neighbor finder
  // SAD
//  typedef ImagePatchDifference<ImagePatchPixelDescriptorType,
//      SumAbsolutePixelDifference<typename TImage::PixelType> > PatchDifferenceType;

  // Compute weights

  typename TImage::PixelType channelMins;
  ITKHelpers::ComputeMinOfAllChannels(originalImage, channelMins);

  typename TImage::PixelType channelMaxs;
  ITKHelpers::ComputeMaxOfAllChannels(originalImage, channelMaxs);

  float minX = fabs(channelMins[3]);
  float maxX = fabs(channelMaxs[3]);
  float maxValueX = std::max(minX, maxX);
  std::cout << "maxValueX = " << maxValueX << std::endl;
  float depthDerivativeWeightX = 255.0f / maxValueX;
  std::cout << "Computed depthDerivativeWeightX = " << depthDerivativeWeightX << std::endl;

  float minY = fabs(channelMins[4]);
  float maxY = fabs(channelMaxs[4]);
  float maxValueY = std::max(minY, maxY);
  std::cout << "maxValueY = " << maxValueY << std::endl;
  float depthDerivativeWeightY = 255.0f / maxValueY;
  std::cout << "Computed depthDerivativeWeightY = " << depthDerivativeWeightY << std::endl;

  // Use all channels
  std::vector<float> weights = {1.0f, 1.0f, 1.0f, depthDerivativeWeightX, depthDerivativeWeightY};

  // Pixel difference functor
  typedef WeightedSumSquaredPixelDifference<typename TImage::PixelType> PixelDifferenceFunctor;
  PixelDifferenceFunctor pixelDifferenceFunctor(weights);

  // SSD (takes about the same time as SAD)
  typedef ImagePatchDifference<ImagePatchPixelDescriptorType,
      PixelDifferenceFunctor > PatchDifferenceType;

  typedef LinearSearchBestProperty<ImagePatchDescriptorMapType,
                                   PatchDifferenceType> BestSearchType;
  BestSearchType linearSearchBest(imagePatchDescriptorMap, pixelDifferenceFunctor);

  // Perform the inpainting
  InpaintingAlgorithm(graph, inpaintingVisitor, &boundaryNodeQueue,
                      linearSearchBest, &inpainter);
}

#endif
