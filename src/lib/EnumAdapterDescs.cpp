// Copyright 2024, Fred Emmott
// SPDX-License-Identifier: ISC
#include <FredEmmott/MonitorTool/EnumAdapterDescs.hpp>
#include <winrt/base.h>

#include <dxgi1_6.h>

namespace FredEmmott::MonitorTool {

std::vector<DXGI_ADAPTER_DESC1> EnumAdapterDescs() {
  winrt::com_ptr<IDXGIFactory6> dxgi;
  winrt::check_hresult(CreateDXGIFactory2(0, IID_PPV_ARGS(dxgi.put())));

  std::vector<DXGI_ADAPTER_DESC1> ret;

  winrt::com_ptr<IDXGIAdapter1> it;
  // Enum by HIGH_PERFORMANCE so that for GPUs, the order does not depend on current power profile
  for (size_t i = 0;
       dxgi->EnumAdapterByGpuPreference(
         i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(it.put()))
       == S_OK;
       ++i) {
    DXGI_ADAPTER_DESC1 desc;
    it->GetDesc1(&desc);
    it = {};
    ret.push_back(desc);
  }
  return ret;
}

}// namespace FredEmmott::MonitorTool