// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#ifndef QUALITYCONTROL_DATASOURCESPEC_H
#define QUALITYCONTROL_DATASOURCESPEC_H

///
/// \file   DataSourceSpec.h
/// \author Piotr Konopka
///

#include <string>
#include <unordered_map>
#include <Framework/InputSpec.h>

namespace o2::quality_control::core {

enum class DataSourceType
{
  DataSamplingPolicy,
  Direct,
  Task,
  Check,
  Aggregator,
  PostProcessingTask,
  ExternalTask,
  Invalid
};

// this should allow us to represent all data sources which come from DPL (and maybe CCDB).
struct DataSourceSpec
{
  explicit DataSourceSpec(DataSourceType type = DataSourceType::Invalid, std::unordered_map<std::string, std::string> params = {});

  // fixme very dumb polymorphism, i am not sure yet about it, e.g. because it won't support arrays (which are used e.g. to list MOs for a Check).
  DataSourceType type;
  std::unordered_map<std::string, std::string> typeSpecificParams;

  // todo. this should be useful for handling different data sources in a concise way.
  static std::vector<framework::InputSpec> inputSpecsFor(const DataSourceSpec& dataSourceSpec);
};

}

#endif //QUALITYCONTROL_DATASOURCESPEC_H
