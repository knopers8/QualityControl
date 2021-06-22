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
/// \file   InfrastructureSpecReader.cxx
/// \author Piotr Konopka
///

#include "QualityControl/InfrastructureSpecReader.h"
#include "QualityControl/QcInfoLogger.h"

#include <boost/property_tree/ptree.hpp>

namespace o2::quality_control::core {

InfrastructureSpec InfrastructureSpecReader::readInfrastructureSpec(const boost::property_tree::ptree& config, const std::string& configurationSource)
{
  InfrastructureSpec spec;
  spec.global = readGlobalConfig(config.get_child("config"));
  if (config.count("tasks") > 0) {
    const auto& tasksConfig = config.get_child("tasks");
    spec.tasks.reserve(tasksConfig.size());
    for (const auto& [taskName, taskConfig] : tasksConfig) {
      spec.tasks.push_back(readTaskSpec(taskName, taskConfig));
    }
  }
  return {};
}

GlobalConfig InfrastructureSpecReader::readGlobalConfig(const boost::property_tree::ptree& config, const std::string& configurationSource)
{
  GlobalConfig gc;
  for (const auto&[key, value] : config.get_child("database")) {
    gc.database.emplace(key, value.get_value<std::string>());
  }
  gc.activityNumber = config.get<size_t>("Activity.number", gc.activityNumber);
  gc.activityType = config.get<std::string>("Activity.type", gc.activityType);
  gc.monitoringUrl = config.get<std::string>("monitoring.url", gc.monitoringUrl);
  gc.consulUrl = config.get<std::string>("consul.url", gc.consulUrl);
  gc.conditionDBUrl = config.get<std::string>("conditionDB.url", gc.conditionDBUrl);
  gc.infologgerFilterDiscardDebug = config.get<bool>("infologger.filterDiscardDebug", gc.infologgerFilterDiscardDebug);
  gc.infologgerDiscardLevel = config.get<int>("infologger.filterDiscardLevel", gc.infologgerDiscardLevel);

  gc.configurationSource = configurationSource;

  return gc;
}


TaskSpec InfrastructureSpecReader::readTaskSpec(std::string taskName, const boost::property_tree::ptree& config)
{
  TaskSpec ts;

  ts.taskName = taskName;
  ts.className = config.get<std::string>("className");
  ts.moduleName = config.get<std::string>("moduleName");
  ts.detectorName = config.get<std::string>("detectorName");
  ts.cycleDurationSeconds = config.get<int>("cycleDurationSeconds");
  ts.dataSource = readDataSourceSpec(config.get_child("dataSource"));

  ts.active = config.get<bool>("active", ts.active);
  ts.maxNumberCycles = config.get<int>("maxNumberCycles", ts.maxNumberCycles);
  ts.resetAfterCycles = config.get<size_t>("resetAfterCycles", ts.resetAfterCycles);
  ts.saveObjectsToFile = config.get<std::string>("saveObjectsToFile", ts.saveObjectsToFile);
  if (config.count("taskParameters") > 0) {
    for (const auto&[key, value] : config.get_child("taskParameters")) {
      ts.customParameters.emplace(key, value.get_value<std::string>());
    }
  }

  bool multinodeSetup = config.find("location") != config.not_found();
  ts.location = gTaskLocationFromString.at(config.get<std::string>("location", "remote"));
  if (config.count("localMachines") > 0) {
    for (const auto&[key, value] : config.get_child("localMachines")) {
      ts.localMachines.emplace_back(value.get_value<std::string>());
    }
  }
  if (multinodeSetup && config.count("remoteMachine") > 0) {
    ILOG(Warning, Devel)
      << "No remote machine was specified for a multinode QC setup."
         " This is fine if running with AliECS, but it will fail in standalone mode."
      << ENDM;
  }
  ts.remoteMachine = config.get<std::string>("remoteMachine", ts.remoteMachine);
  if (multinodeSetup && config.count("remotePort") > 0) {
    ILOG(Warning, Devel)
      << "No remote port was specified for a multinode QC setup."
         " This is fine if running with AliECS, but it might fail in standalone mode."
      << ENDM;
  }
  ts.remotePort = config.get<uint16_t>("remotePort", ts.remotePort);
  ts.mergingMode = config.get<std::string>("mergingMode", ts.mergingMode);
  ts.mergerCycleMultiplier = config.get<int>("mergerCycleMultiplier", ts.mergerCycleMultiplier);

  return {};
}

static DataSourceSpec readDataSourceSpec(const boost::property_tree::ptree& config)
{
  static std::unordered_map<std::string, DataSourceType> const dataSourceTypeFromString = {
    // fixme: the convention is inconsistent and it should be fixed in coordination with configuration files
    { "dataSamplingPolicy", DataSourceType::DataSamplingPolicy },
    { "direct",             DataSourceType::Direct },
    { "Task",               DataSourceType::Task },
    { "Check",              DataSourceType::Check },
    { "Aggregator",         DataSourceType::Aggregator },
    { "PostProcessing",     DataSourceType::PostProcessingTask },
    { "ExternalTask",       DataSourceType::ExternalTask }
  };

  DataSourceSpec dss;
  dss.type = dataSourceTypeFromString.at(config.get<std::string>("type"));

  switch (dss.type) {
    case DataSourceType::DataSamplingPolicy:
      dss.typeSpecificParams.insert({ "name", config.get<std::string>("name") });
      break;
    case DataSourceType::Direct:
      dss.typeSpecificParams.insert({ "query", config.get<std::string>("query") });
      break;
    case DataSourceType::Task:
    case DataSourceType::PostProcessingTask:
    case DataSourceType::Check:
    case DataSourceType::Aggregator:
      dss.typeSpecificParams.insert({ "name", config.get<std::string>("name") });
      break;
    case DataSourceType::ExternalTask:
      dss.typeSpecificParams.insert({ "name", config.get<std::string>("name") });
      dss.typeSpecificParams.insert({ "query", config.get<std::string>("query") });
      break;
    case DataSourceType::Invalid:
      // todo: throw?
      break;
  }

  return dss;
}

}