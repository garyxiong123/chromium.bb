// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_SHELL_SHELL_RENDER_PROCESS_OBSERVER_H_
#define CONTENT_SHELL_SHELL_RENDER_PROCESS_OBSERVER_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/files/file_path.h"
#include "base/memory/scoped_ptr.h"
#include "content/public/renderer/render_process_observer.h"
#include "ipc/ipc_platform_file.h"

namespace WebKit {
class WebFrame;
}

// SHEZ: Remove upstream test-specific forward declarations here.

namespace content {

class RenderView;
// SHEZ: Remove upstream test-specific forward declaration here.

class ShellRenderProcessObserver : public RenderProcessObserver {
 public:
  static ShellRenderProcessObserver* GetInstance();

  ShellRenderProcessObserver();
  virtual ~ShellRenderProcessObserver();

  // SHEZ: Remove upstream test-specific code here.
  void SetMainWindow(RenderView* view);

  // RenderProcessObserver implementation.
  virtual void WebKitInitialized() OVERRIDE;
  virtual bool OnControlMessageReceived(const IPC::Message& message) OVERRIDE;

  // SHEZ: Remove upstream test-specific code here.
  const base::FilePath& webkit_source_dir() const { return webkit_source_dir_; }

 private:
  // Message handlers.
  void OnSetWebKitSourceDir(const base::FilePath& webkit_source_dir);
  void OnLoadHyphenDictionary(const IPC::PlatformFileForTransit& dict_file);
  // SHEZ: Removed upstream test-specific code here.

  base::FilePath webkit_source_dir_;

  DISALLOW_COPY_AND_ASSIGN(ShellRenderProcessObserver);
};

}  // namespace content

#endif  // CONTENT_SHELL_SHELL_RENDER_PROCESS_OBSERVER_H_
