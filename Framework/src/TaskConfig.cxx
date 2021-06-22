// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// \file   TaskConfig.cxx
/// \author Piotr Konopka
///

#include "QualityControl/TaskConfig.h"
#include "QualityControl/TaskSpec.h"
#include "QualityControl/GlobalConfig.h"

#include <utility>

namespace o2::quality_control::core {

TaskConfig TaskConfig::from(const GlobalConfig& globalConfig, const TaskSpec& taskSpec, std::optional<int> id, std::optional<int> resetAfterCycles)
{
  return {
    taskSpec.taskName,
    taskSpec.moduleName,
    taskSpec.className,
    taskSpec.cycleDurationSeconds,
    taskSpec.maxNumberCycles,
    globalConfig.consulUrl,
    globalConfig.conditionDBUrl,
    taskSpec.customParameters,
    taskSpec.detectorName,
    id.value_or(0),
    taskSpec.saveObjectsToFile,
    resetAfterCycles.value_or(taskSpec.resetAfterCycles),
    globalConfig.configurationSource
  };
}

} // namespace o2::quality_control::core
