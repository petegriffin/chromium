// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_EVENT_AUTO_REPEAT_HANDLER_H
#define UI_EVENTS_EVENT_AUTO_REPEAT_HANDLER_H

#include <linux/input.h>

#include "base/callback.h"
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#include "ui/events/events_export.h"

namespace ui {

class EVENTS_EXPORT EventAutoRepeatHandler {
 public:
  EventAutoRepeatHandler();
  ~EventAutoRepeatHandler();

  using DispatchAutoRepeatEventCallback =
      base::Callback<void(unsigned int key,
                          bool down,
                          bool repeat,
                          base::TimeTicks timestamp,
                          int device_id)>;
  void SetCallback(const DispatchAutoRepeatEventCallback& callback);

  void UpdateKeyRepeat(unsigned int key,
                       bool down,
                       bool suppress_auto_repeat,
                       int device_id);

  // Configuration for key repeat.
  bool IsAutoRepeatEnabled();
  void SetAutoRepeatEnabled(bool enabled);
  void SetAutoRepeatRate(const base::TimeDelta& delay,
                         const base::TimeDelta& interval);
  void GetAutoRepeatRate(base::TimeDelta* delay, base::TimeDelta* interval);

 private:
  void StartKeyRepeat(unsigned int key, int device_id);
  void StopKeyRepeat();
  void ScheduleKeyRepeat(const base::TimeDelta& delay);
  void OnRepeatTimeout(unsigned int sequence);
  void OnRepeatCommit(unsigned int sequence);

  DispatchAutoRepeatEventCallback callback_;

  // Key repeat state.
  bool auto_repeat_enabled_ = true;
  unsigned int repeat_key_ = KEY_RESERVED;
  unsigned int repeat_sequence_ = 0;
  int repeat_device_id_ = 0;
  base::TimeDelta repeat_delay_;
  base::TimeDelta repeat_interval_;

  base::WeakPtrFactory<EventAutoRepeatHandler> weak_ptr_factory_;

  DISALLOW_COPY_AND_ASSIGN(EventAutoRepeatHandler);
};

}  // namespace ui

#endif  // UI_EVENTS_EVENT_AUTO_REPEAT_HANDLER_H
