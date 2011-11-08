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

#ifndef PIXELDIFFERENCE_H
#define PIXELDIFFERENCE_H

struct FullPixelDifference
{
  FullPixelDifference(typename FloatVectorImageType::PixelType& examplePixel)
  {
    this->NumberOfComponentsPerPixel = examplePixel.GetNumberOfElements();
  }
  
  FullPixelDifference(const unsigned int numberOfComponents)
  {
    this->NumberOfComponentsPerPixel = numberOfComponents;
  }
  
  float Difference(const typename FloatVectorImageType::PixelType &a, const typename FloatVectorImageType::PixelType &b)
  {
    float difference = 0;

    float diff = 0;
    for(unsigned int i = 0; i < this->NumberOfComponentsPerPixel; ++i)
      {
      diff = fabs(a[i] - b[i]);
      difference += diff;
      }
    return difference;
  }
  
  static float Difference(const typename FloatVectorImageType::PixelType &a, const typename FloatVectorImageType::PixelType &b, unsigned int numberOfComponents)
  {
    float difference = 0;

    float diff = 0;
    for(unsigned int i = 0; i < numberOfComponents; ++i)
      {
      diff = fabs(a[i] - b[i]);
      difference += diff;
      }
    return difference;
  }
  
  unsigned int NumberOfComponentsPerPixel;
};

struct FullSquaredPixelDifference
{
  FullSquaredPixelDifference(typename FloatVectorImageType::PixelType& examplePixel)
  {
    this->NumberOfComponentsPerPixel = examplePixel.GetNumberOfElements();
  }
  
  FullSquaredPixelDifference(const unsigned int numberOfComponents)
  {
    this->NumberOfComponentsPerPixel = numberOfComponents;
  }
  
  float Difference(const typename FloatVectorImageType::PixelType &a, const typename FloatVectorImageType::PixelType &b)
  {
    float difference = 0;

    float diff = 0;
    for(unsigned int i = 0; i < this->NumberOfComponentsPerPixel; ++i)
      {
      diff = fabs(a[i] - b[i]);
      difference += diff;
      }
    return difference*difference;
  }
  
  static float Difference(const typename FloatVectorImageType::PixelType &a, const typename FloatVectorImageType::PixelType &b, unsigned int numberOfComponents)
  {
    float difference = 0;

    float diff = 0;
    for(unsigned int i = 0; i < numberOfComponents; ++i)
      {
      diff = fabs(a[i] - b[i]);
      difference += diff;
      }
    return difference*difference;
  }
  
  unsigned int NumberOfComponentsPerPixel;
};

struct ColorPixelDifference
{
  static float Difference(const typename FloatVectorImageType::PixelType &a, const FloatVectorImageType::PixelType &b)
  {
    float difference = 0;

    float diff = 0;
    for(unsigned int i = 0; i < 3; ++i)
      {
      diff = fabs(a[i] - b[i]);
      difference += diff;
      }
    return difference;
  }
  

};
 
struct DepthPixelDifference
{
  static float Difference(const typename FloatVectorImageType::PixelType &a, const FloatVectorImageType::PixelType &b)
  {
    return fabs(a[3] - b[3]);
  }
};

#endif