// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include <Framework/ConfigParamSpec.h>
#include <vector>
using namespace o2::framework;

void customize(std::vector<ConfigParamSpec>& workflowOptions)
{
  workflowOptions.push_back(
    ConfigParamSpec{ "type", VariantType::String, "invalid", { "Containter type to be tested." } });
  workflowOptions.push_back(
    ConfigParamSpec{ "mo-count", VariantType::Int, 1, { "Number of MOs in container." } });
  workflowOptions.push_back(
    ConfigParamSpec{ "time", VariantType::Int, 10, { "How long the benchmark should last (in seconds)." } });
}

#include <chrono>
#include <Framework/runDataProcessing.h>
#include <Framework/ControlService.h>
#include <TH1F.h>
#include <TMap.h>
#include <THashList.h>
#include <FairLogger.h>
#include "QualityControl/MonitorObject.h"

using namespace o2::quality_control::core;
using namespace std::chrono;

AlgorithmSpec::InitCallback producerCallback(std::string type, size_t moCount)
{
  if (type == "map") {
    return [moCount](InitContext&) {
      auto monitorObjects = std::make_shared<std::map<std::string /*object name*/, MonitorObject* /* object */>>();
      for (size_t i = 0; i < moCount; i++) {
        TObject* tobj = new TH1F("h", "h", 100, 0, 99);
        MonitorObject* mo = new MonitorObject("1", tobj, "test");
        (*monitorObjects)[std::to_string(i)] = mo;
      }
      return [monitorObjects](ProcessingContext& ctx) mutable {
        for (auto& pair : *monitorObjects) {
          auto* mo = pair.second;
          ctx.outputs().snapshot<MonitorObject>(Output{ "TST", "MOVECTOR" }, *mo);
        }
      };
    };
  } else if (type == "tmap") {
    return [moCount](InitContext&) {
      auto monitorObjects = std::make_shared<TMap>();
      monitorObjects->SetOwnerKeyValue(true, true);
      for (size_t i = 0; i < moCount; i++) {
        TObject* tobj = new TH1F("h", "h", 100, 0, 99);
        MonitorObject* mo = new MonitorObject("1", tobj, "test");
        monitorObjects->Add(new TObjString(std::to_string(i).c_str()), (TObject*) mo);
      }
      return [monitorObjects](ProcessingContext& ctx) mutable {
        TMap* shallowCopy = new TMap;
        shallowCopy->SetOwnerKeyValue(false, false);

        auto moIter = monitorObjects->MakeIterator();
        TObject* key = nullptr;
        while ((key = moIter->Next())) {
          shallowCopy->Add(key, monitorObjects->GetValue(key));
        }

        ctx.outputs().adopt(Output{ "TST", "MOVECTOR" }, dynamic_cast<TObject*>(shallowCopy));

      };
    };
  } else if (type == "tobjarray") {
    return [moCount](InitContext&) {
      auto monitorObjects = std::make_shared<TObjArray>();
      for (size_t i = 0; i < moCount; i++) {
        TObject* tobj = new TH1F("h", "h", 100, 0, 99);
        MonitorObject* mo = new MonitorObject("1", tobj, "test");
        monitorObjects->Add((TObject*) mo);
      }
      return [monitorObjects](ProcessingContext& ctx) mutable {

        ctx.outputs().snapshot<TObjArray>(Output{ "TST", "MOVECTOR" }, *monitorObjects);
      };
    };
  } else if (type == "thashlist") {
    return [moCount](InitContext&) {
      auto monitorObjects = std::make_shared<THashList>();
      for (size_t i = 0; i < moCount; i++) {
        TObject* tobj = new TH1F("h", "h", 100, 0, 99);
        MonitorObject* mo = new MonitorObject("1", tobj, "test");
        monitorObjects->Add((TObject*) mo);
      }
      return [monitorObjects](ProcessingContext& ctx) mutable {
        ctx.outputs().snapshot<THashList>(Output{ "TST", "MOVECTOR" }, *monitorObjects);
      };
    };
  } else {
    return [type](InitContext&) {
      return [type](ProcessingContext& ctx) {
        LOG(ERROR) << "unknown type " << type;
        sleep(1);
      };
    };
  }
}

AlgorithmSpec::InitCallback receiverTemplate(std::string type, size_t moCount, size_t time, auto receive) {
  return [=](InitContext&) {
    size_t receivedMOs = 0;
    size_t receivedMessages = 0;
    steady_clock::time_point timeStart = steady_clock::now();
    return [=, finished = false](ProcessingContext& ctx) mutable {

      receive(ctx, receivedMOs, receivedMessages);

      auto diff = duration_cast<milliseconds>(steady_clock::now() - timeStart).count();
      if ( diff > time * 1000 && !finished) {
        std::ofstream outfile;
        outfile.open("tcontainers.txt", std::ios_base::app);
        outfile << type << ", " << moCount << " mo/container: "
                << std::to_string(receivedMOs) << " " << std::to_string(receivedMessages) << " "
                << std::to_string(receivedMOs/(diff/1000.0)) << " " << std::to_string(receivedMessages/(diff/1000.0))
                << std::endl;
        ctx.services().get<ControlService>().readyToQuit(true);
        finished = true;
      }
    };
  };
};

AlgorithmSpec::InitCallback receiverCallback(std::string type, size_t moCount, size_t time)
{
  if (type == "map") {
    return receiverTemplate(type, moCount, time, [](ProcessingContext& ctx, size_t& receivedMOs, size_t& receivedMessages){
      std::shared_ptr<MonitorObject> mo{ std::move(DataRefUtils::as<MonitorObject>(*ctx.inputs().begin())) };
      assert(mo->getName().size() == 1);
      receivedMOs += mo->getName().size();
      receivedMessages++;
    });
  } else if (type == "tmap") {
    return receiverTemplate(type, moCount, time, [](ProcessingContext& ctx, size_t& receivedMOs, size_t& receivedMessages){
      std::shared_ptr<TMap> moMap{ std::move(DataRefUtils::as<TMap>(*ctx.inputs().begin())) };
      moMap->SetOwnerKeyValue(true, false);
      auto moIter = moMap->MakeIterator();
      TObject* key = nullptr;
      while(key = moIter->Next()) {
        std::shared_ptr<MonitorObject> mo{ dynamic_cast<MonitorObject*>(moMap->GetValue(key)) };
        assert(mo->getName().size() == 1);
        receivedMOs += mo->getName().size();
      }
      receivedMessages++;
    });
  } else if (type == "tobjarray") {
    return receiverTemplate(type, moCount, time, [](ProcessingContext& ctx, size_t& receivedMOs, size_t& receivedMessages){
      std::shared_ptr<TObjArray> moArray{ std::move(DataRefUtils::as<TObjArray>(*ctx.inputs().begin())) };

      for (const auto& to : *moArray) {
        MonitorObject* mo = dynamic_cast<MonitorObject*>(to);
        assert(mo->getName().size() == 1);
        receivedMOs += mo->getName().size();
      }
      receivedMessages++;
    });
  } else if (type == "thashlist") {
    return receiverTemplate(type, moCount, time, [](ProcessingContext& ctx, size_t& receivedMOs, size_t& receivedMessages){
      std::shared_ptr<THashList> moArray{ std::move(DataRefUtils::as<THashList>(*ctx.inputs().begin())) };

      for (const auto& to : *moArray) {
        MonitorObject* mo = dynamic_cast<MonitorObject*>(to);
        assert(mo->getName().size() == 1);
        receivedMOs += mo->getName().size();
      }
      receivedMessages++;
    });
  } else {
    return [type](InitContext&) {
      return [type](ProcessingContext& ctx) {
        LOG(ERROR) << "unknown type " << type;
      };
    };
  }
}

WorkflowSpec defineDataProcessing(ConfigContext const& config)
{
  auto type = config.options().get<std::string>("type");
  auto moCount = config.options().get<int>("mo-count");
  auto time = config.options().get<int>("time");

  DataProcessorSpec producer{
    "producer",
    Inputs{},
    Outputs{ { "TST", "MOVECTOR" } },
    AlgorithmSpec{ producerCallback(type, moCount) }
  };

  DataProcessorSpec receiver{
    "receiver",
    Inputs{ { "movector", "TST", "MOVECTOR" } },
    Outputs{},
    AlgorithmSpec{ receiverCallback(type, moCount, time) }
  };

  WorkflowSpec specs{
    producer,
    receiver
  };

  return specs;
}
