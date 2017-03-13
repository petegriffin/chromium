// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/browser_process_platform_part_ozone.h"

#include "services/ui/common/image_cursors_set.h"
#include "services/ui/public/interfaces/constants.mojom.h"
#include "services/ui/service.h"

BrowserProcessPlatformPart::BrowserProcessPlatformPart() = default;

BrowserProcessPlatformPart::~BrowserProcessPlatformPart() = default;

void BrowserProcessPlatformPart::RegisterInProcessServices(
    content::ContentBrowserClient::StaticServiceMap* services) {
  // After https://chromium-review.googlesource.com/c/754087/,
  // embedding to ui service is not needed here. But this class
  // is left in case if something else needs to be extended like in chromeos.
  NOTIMPLEMENTED();
}
