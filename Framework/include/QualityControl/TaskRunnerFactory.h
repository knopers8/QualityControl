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
/// \file   TaskRunnerFactory.h
/// \author Piotr Konopka
///

#ifndef QC_CORE_TASKFACTORY_H
#define QC_CORE_TASKFACTORY_H

#include <string>
#include <vector>

#include <Framework/DataProcessorSpec.h>

namespace o2::framework
{
class CompletionPolicy;
}

namespace o2::quality_control::core
{

struct TaskConfig;

/// \brief Factory in charge of creating DataProcessorSpec of QC task
class TaskRunnerFactory
{
 public:
  TaskRunnerFactory() = default;
  virtual ~TaskRunnerFactory() = default;

  /// \brief Creator of tasks
  ///
  /// \param taskConfig
  static o2::framework::DataProcessorSpec create(const TaskConfig&);

  /// \brief Provides necessary customization of the TaskRunners.
  ///
  /// Provides necessary customization of the Completion Policies of the TaskRunners. This is necessary to make
  /// them work. Put it inside customize() function before including <Framework/runDataProcessing.cxx>.
  /// \param policies - completion policies vector
  static void customizeInfrastructure(std::vector<framework::CompletionPolicy>& policies);
};

} // namespace o2::quality_control::core

#endif // QC_CORE_TASKFACTORY_H
