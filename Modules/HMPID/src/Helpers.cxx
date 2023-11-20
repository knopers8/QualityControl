// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// \file   Helpers.cxx
/// \author Nicola Nicassio
///

#include "HMPID/Helpers.h"
#include "QualityControl/QualityObject.h"

using namespace o2::quality_control;
using namespace o2::quality_control::core;

namespace o2::quality_control_modules::hmpid
{

//_________________________________________________________________________________________

bool matchHistName(std::string hist, std::string name)
{
  if (name.empty()) {
    return false;
  }

  int64_t pos = hist.find(name);
  int64_t histSize = hist.size();
  int64_t nameSize = name.size();
  int64_t diff = histSize - nameSize;
  return ((pos >= 0) && (pos == diff));
}

//_________________________________________________________________________________________

QualityCheckerDDL::QualityCheckerDDL()
{
  resetDDL();
}

void QualityCheckerDDL::resetDDL()
{
  std::fill(mQualityDDL.begin(), mQualityDDL.end(), Quality::Null);
}

void QualityCheckerDDL::addCheckResultDDL(gsl::span<Quality> result)
{
  if (mQualityDDL.size() != result.size()) // <-- Original was <
  {
    return;
  }

  for (int i = 0; i < mQualityDDL.size(); i++) {
    if ((mQualityDDL[i] == Quality::Null) || (result[i] == Quality::Bad)) {
      mQualityDDL[i] = result[i];
    }
  }
}

Quality QualityCheckerDDL::getQualityDDL()
{
  Quality result = Quality::Null;

  // It at least one entry is not Null, initialize to Good
  for (int i = 0; i < mQualityDDL.size(); i++) {
    if (mQualityDDL[i] != Quality::Null) {
      result = Quality::Good;
    }
  }
  // Count bad links
  int badDdlCounter = 0;
  for (int i = 0; i < mQualityDDL.size(); i++) {
    if (mQualityDDL[i] == Quality::Bad) {
      badDdlCounter++;
    }
  }
  // Find quality
  if (result == Quality::Good) {
    if (badDdlCounter >= mMaxBadDDLForMedium) {
      result = Quality::Medium;
      if (badDdlCounter >= mMaxBadDDLForBad) {
        result = Quality::Bad;
      }
    }
  }
  return result;
}

//_________________________________________________________________________________________

QualityCheckerHV::QualityCheckerHV()
{
  resetHV();
}

void QualityCheckerHV::resetHV()
{
  std::fill(mQualityHV.begin(), mQualityHV.end(), Quality::Null);
}

void QualityCheckerHV::addCheckResultHV(gsl::span<Quality> result)
{
  if (mQualityHV.size() != result.size()) // <-- Original was <
  {
    return;
  }

  for (int i = 0; i < mQualityHV.size(); i++) {
    if ((mQualityHV[i] == Quality::Null) || (result[i] == Quality::Bad)) {
      mQualityHV[i] = result[i];
    }
  }
}

Quality QualityCheckerHV::getQualityHV()
{
  Quality result = Quality::Null;

  // It at least one entry is not Null, initialize to Good
  for (int i = 0; i < mQualityHV.size(); i++) {
    if (mQualityHV[i] != Quality::Null) {
      result = Quality::Good;
    }
  }
  // Count bad links
  int badHvCounter = 0;
  for (int i = 0; i < mQualityHV.size(); i++) {
    if (mQualityHV[i] == Quality::Bad) {
      badHvCounter++;
    }
  }
  // Find quality
  if (result == Quality::Good) {
    if (badHvCounter >= mMaxBadHVForMedium) {
      result = Quality::Medium;
      if (badHvCounter >= mMaxBadHVForBad) {
        result = Quality::Bad;
      }
    }
  }
  return result;
}

//_________________________________________________________________________________________

} // namespace o2::quality_control_modules::hmpid