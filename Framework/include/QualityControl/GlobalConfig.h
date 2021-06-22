// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#ifndef QUALITYCONTROL_GLOBALCONFIG_H
#define QUALITYCONTROL_GLOBALCONFIG_H

///
/// \file   GlobalConfig.h
/// \author Piotr Konopka
///

#include <string>
#include <unordered_map>
#include <boost/property_tree/ptree_fwd.hpp>

namespace o2::quality_control::core {

struct GlobalConfig {
  GlobalConfig() = default;
  GlobalConfig(const boost::property_tree::ptree&);

  std::unordered_map<std::string, std::string> database;
  size_t activityNumber;
  std::string activityType;
  std::string monitoringUrl;
  std::string consulUrl;
  std::string conditionDBUrl;
  bool infologgerFilterDiscardDebug = false;
  int infologgerDiscardLevel = 21;

  std::string configurationSource;
};

}

#endif //QUALITYCONTROL_GLOBALCONFIG_H
