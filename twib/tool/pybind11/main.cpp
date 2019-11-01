// Copyright 2019 leoetlino <leo@leolam.fr>
// Licensed under GPLv3

#include <string>

#include <msgpack11.hpp>
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "../RemoteObject.hpp"
#include "../SocketClient.hpp"
#include "../common/Protocol.hpp"
#include "../common/ResultError.hpp"
#include "../interfaces/ITwibDeviceInterface.hpp"
#include "../interfaces/ITwibMetaInterface.hpp"
#include "common/config.hpp"

namespace py = pybind11;
using namespace py::literals;

PYBIND11_MAKE_OPAQUE(std::vector<std::uint8_t>);

using namespace twili::twib;

static std::unique_ptr<tool::client::Client> connect_unix(std::string path) {
  twili::platform::Socket socket(AF_UNIX, SOCK_STREAM, 0);

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path) - 1);
  socket.Connect((struct sockaddr*)&addr, sizeof(addr));
  return std::make_unique<tool::client::SocketClient>(std::move(socket));
}

PYBIND11_MODULE(pytwib, m) {
  // Client

  py::class_<tool::client::Client>(m, "Client");
  m.def("GetClient", [] { return connect_unix(TWIB_UNIX_FRONTEND_DEFAULT_PATH); });

  // ITwibDeviceInterface

  py::class_<tool::ITwibDeviceInterface>(m, "ITwibDeviceInterface")
      .def("ListProcesses", &tool::ITwibDeviceInterface::ListProcesses)
      .def("OpenActiveDebugger", &tool::ITwibDeviceInterface::OpenActiveDebugger, "pid"_a,
           py::keep_alive<0, 1>());

  py::class_<tool::ProcessListEntry>(m, "ProcessListEntry")
      .def("__repr__",
           [](const tool::ProcessListEntry& entry) {
             return py::str("<Process: {}>").format(entry.process_name);
           })
      .def_readonly("process_id", &tool::ProcessListEntry::process_id)
      .def_readonly("result", &tool::ProcessListEntry::result)
      .def_readonly("title_id", &tool::ProcessListEntry::title_id)
      .def_readonly("process_name", &tool::ProcessListEntry::process_name);

  m.def(
      "GetDeviceInterface",
      [](tool::client::Client& client) {
        tool::ITwibMetaInterface itmi(tool::RemoteObject(client, 0, 0));
        std::vector<msgpack11::MsgPack> devices = itmi.ListDevices();
        if (devices.size() != 1) {
          throw std::logic_error("exactly one device should be connected");
        }
        const uint32_t device_id = devices[0]["device_id"].uint32_value();
        tool::ITwibDeviceInterface itdi(std::make_shared<tool::RemoteObject>(client, device_id, 0));
        return itdi;
      },
      "client"_a,
      py::keep_alive<0, 1>());

  // DebugTypes

  py::class_<nx::DebugEvent> DebugEvent(m, "DebugEvent");
  DebugEvent.def_readonly("event_type", &nx::DebugEvent::event_type)
      .def_readonly("flags", &nx::DebugEvent::flags)
      .def_readonly("thread_id", &nx::DebugEvent::thread_id);
  py::enum_<nx::DebugEvent::EventType>(DebugEvent, "EventType")
      .value("AttachProcess", nx::DebugEvent::EventType::AttachProcess)
      .value("AttachThread", nx::DebugEvent::EventType::AttachThread)
      .value("ExitProcess", nx::DebugEvent::EventType::ExitProcess)
      .value("ExitThread", nx::DebugEvent::EventType::ExitThread)
      .value("Exception", nx::DebugEvent::EventType::Exception);

  py::class_<nx::LoadedModuleInfo>(m, "LoadedModuleInfo")
      .def_property_readonly("build_id",
                             [](const nx::LoadedModuleInfo& info) {
                               return py::bytes((const char*)info.build_id, sizeof(info.build_id));
                             })
      .def_readonly("base_addr", &nx::LoadedModuleInfo::base_addr)
      .def_readonly("size", &nx::LoadedModuleInfo::size);

  // ITwibDebugger

  py::bind_vector<std::vector<std::uint8_t>>(m, "Bytes", py::buffer_protocol());
  py::class_<tool::ITwibDebugger>(m, "ITwibDebugger")
      .def("ReadMemory", &tool::ITwibDebugger::ReadMemory, "addr"_a, "size"_a)
      .def("WriteMemory", &tool::ITwibDebugger::WriteMemory, "addr"_a, "data"_a)
      .def("GetDebugEvent", &tool::ITwibDebugger::GetDebugEvent)
      .def("ContinueDebugEvent", &tool::ITwibDebugger::ContinueDebugEvent, "flags"_a, "thread_ids"_a)
      .def("BreakProcess", &tool::ITwibDebugger::BreakProcess)
      .def("GetTargetEntry", &tool::ITwibDebugger::GetTargetEntry)
      .def("GetNsoInfos", &tool::ITwibDebugger::GetNsoInfos);
}
