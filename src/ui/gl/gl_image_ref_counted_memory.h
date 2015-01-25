// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GL_GL_IMAGE_REF_COUNTED_MEMORY_H_
#define UI_GL_GL_IMAGE_REF_COUNTED_MEMORY_H_

#include "base/memory/ref_counted.h"
#include "ui/gl/gl_image_memory.h"

namespace base {
class RefCountedMemory;
}

namespace gfx {

class GL_EXPORT GLImageRefCountedMemory : public GLImageMemory {
 public:
  GLImageRefCountedMemory(const gfx::Size& size, unsigned internalformat);

  bool Initialize(base::RefCountedMemory* ref_counted_memory);

  // Overridden from GLImage:
  virtual void Destroy(bool have_context) OVERRIDE;

 protected:
  virtual ~GLImageRefCountedMemory();

 private:
  scoped_refptr<base::RefCountedMemory> ref_counted_memory_;

  DISALLOW_COPY_AND_ASSIGN(GLImageRefCountedMemory);
};

}  // namespace gfx

#endif  // UI_GL_GL_IMAGE_REF_COUNTED_MEMORY_H_
