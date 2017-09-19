// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/platform_window/x11/x11_window_ozone.h"

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>

#include "base/bind.h"
#include "ui/events/event.h"
#include "ui/events/ozone/events_ozone.h"
#include "ui/events/platform/x11/x11_event_source.h"
#include "ui/gfx/geometry/point.h"
#include "ui/platform_window/x11/x11_cursor_ozone.h"
#include "ui/platform_window/x11/x11_window_manager_ozone.h"

namespace ui {

X11WindowOzone::X11WindowOzone(X11WindowManagerOzone* window_manager,
                               PlatformWindowDelegate* delegate,
                               const gfx::Rect& bounds)
    : X11WindowBase(delegate, bounds), window_manager_(window_manager) {
  DCHECK(window_manager);
  window_manager_->AddX11Window(this);

  auto* event_source = X11EventSourceLibevent::GetInstance();
  if (event_source) {
    event_source->AddPlatformEventDispatcher(this);
    event_source->AddXEventDispatcher(this);
  }
}

X11WindowOzone::~X11WindowOzone() {
  X11WindowOzone::PrepareForShutdown();
}

void X11WindowOzone::PrepareForShutdown() {
  DCHECK(window_manager_);
  window_manager_->DeleteX11Window(this);

  auto* event_source = X11EventSourceLibevent::GetInstance();
  if (event_source) {
    event_source->RemovePlatformEventDispatcher(this);
    event_source->RemoveXEventDispatcher(this);
  }
}

void X11WindowOzone::SetCapture() {
  window_manager_->GrabEvents(this);
  // Sets pointer grab if events are grabbed.
  if (window_manager_->event_grabber() == this)
    SetPointerGrab();
}

void X11WindowOzone::ReleaseCapture() {
  window_manager_->UngrabEvents(this);
}

void X11WindowOzone::SetCursor(PlatformCursor cursor) {
  X11CursorOzone* cursor_ozone = static_cast<X11CursorOzone*>(cursor);
  XDefineCursor(xdisplay(), xwindow(), cursor_ozone->xcursor());
}

void X11WindowOzone::CheckCanDispatchNextPlatformEvent(XEvent* xev) {
  handle_next_event_ = xwindow() == None ? false : IsEventForXWindow(*xev);
}

void X11WindowOzone::PlatformEventDispatchFinished() {
  handle_next_event_ = false;
}

bool X11WindowOzone::DispatchXEvent(XEvent* xev) {
  if (!IsEventForXWindow(*xev))
    return false;

  ProcessXWindowEvent(xev);
  return true;
}

bool X11WindowOzone::CanDispatchEvent(const PlatformEvent& platform_event) {
  return handle_next_event_;
}

uint32_t X11WindowOzone::DispatchEvent(const PlatformEvent& platform_event) {
  auto* event = static_cast<Event*>(platform_event);
  if (!window_manager_->event_grabber() ||
      window_manager_->event_grabber() == this) {
    if (event->IsMouseEvent() && event->AsMouseEvent()->IsLeftMouseButton()) {
      // Set location of an x root window, which will be used for interactive
      // dragging/resize if a later hittest is positive.
      SetXRootWindowEventLocation(event->AsMouseEvent()->root_location());
    }
    
    // This is unfortunately needed otherwise events that depend on global state
    // (eg. double click) are broken.
    DispatchEventFromNativeUiEvent(
        event, base::Bind(&PlatformWindowDelegate::DispatchEvent,
                          base::Unretained(delegate())));
    return POST_DISPATCH_STOP_PROPAGATION;
  } else {
    if (event->IsLocatedEvent()) {
      // Another X11WindowOzone has installed itself as capture. Translate the
      // event's location and dispatch to the other.
      ConvertEventLocationToCurrentWindowLocation(
          window_manager_->event_grabber(), event->AsLocatedEvent());
    }
    return window_manager_->event_grabber()->DispatchEvent(event);
  }
}

void X11WindowOzone::OnLostCapture() {
  ReleasePointerGrab();
  delegate()->OnLostCapture();
}

void X11WindowOzone::ConvertEventLocationToCurrentWindowLocation(
    X11WindowOzone* target,
    ui::LocatedEvent* located_event) {
  DCHECK_NE(this, target);
  gfx::Vector2d offset = GetBounds().origin() - target->GetBounds().origin();
  gfx::PointF location_in_pixel_in_host =
      located_event->location_f() + gfx::Vector2dF(offset);
  located_event->set_location_f(location_in_pixel_in_host);
  located_event->set_root_location_f(location_in_pixel_in_host);
}

}  // namespace ui
