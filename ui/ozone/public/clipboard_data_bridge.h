// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PUBLIC_CLIPBOARD_DATA_BRIDGE_H_
#define UI_OZONE_PUBLIC_CLIPBOARD_DATA_BRIDGE_H_

#include <unordered_map>
#include <string>
#include <vector>

#include "base/macros.h"
#include "ui/ozone/ozone_base_export.h"

namespace ui {

// ClipboardDataBridge stores the clipboard backing store, allowing
// exchanging data between Ozone and its clients (eg Mus).
class OZONE_BASE_EXPORT ClipboardDataBridge {
 public:
  using DataMap = std::unordered_map<std::string, std::vector<uint8_t>>;
  explicit ClipboardDataBridge(DataMap& data_map) : data_map_(data_map) {}

  DataMap& data_map() { return data_map_; }
 private:
  DataMap& data_map_;

  DISALLOW_COPY_AND_ASSIGN(ClipboardDataBridge);
};

// ClipboardDelegate allows different classes to exchange ClipboardDataBridge
// content.
class OZONE_BASE_EXPORT ClipboardDelegate {
 public:
  virtual void WriteToWMClipboard(const std::vector<std::string>&) = 0;
  virtual void ReadFromWMClipboard(const std::string& mime_type) = 0;
  virtual std::vector<std::string> GetAvailableMimeTypes() = 0;
  virtual bool ShouldGetClipboardDataFromWMClipboard() = 0;
};

}  // namespace ui

#endif  // UI_OZONE_PUBLIC_CLIPBOARD_DATA_BRIDGE_H_
