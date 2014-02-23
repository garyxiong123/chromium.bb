/*
 * Copyright (C) 2006, 2007, 2009, 2010, 2011 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AutoscrollController_h
#define AutoscrollController_h

#include "core/platform/Timer.h"
#include "core/platform/graphics/IntPoint.h"
#include "wtf/PassOwnPtr.h"

namespace WebCore {

class EventHandler;
class Frame;
class FrameView;
class Node;
class PlatformMouseEvent;
class RenderBox;
class RenderObject;

enum AutoscrollType {
    NoAutoscroll,
    AutoscrollForDragAndDrop,
    AutoscrollForSelection,
#if OS(WINDOWS)
    AutoscrollForPanCanStop,
    AutoscrollForPan,
#endif
};

// AutscrollController handels autoscroll and pan scroll for EventHandler.
class AutoscrollController {
public:
    bool autoscrollInProgress() const;
    bool autoscrollInProgress(const RenderBox*) const;
    static PassOwnPtr<AutoscrollController> create();
    bool panScrollInProgress() const;
    void startAutoscrollForSelection(RenderObject*);
    void stopAutoscrollTimer();
    void stopAutoscrollIfNeeded(RenderObject*);
    void updateAutoscrollRenderer();
    void updateDragAndDrop(Node* targetNode, const IntPoint& eventPosition, double eventTime);
#if OS(WINDOWS)
    void handleMouseReleaseForPanScrolling(Frame*, const PlatformMouseEvent&);
    void startPanScrolling(RenderBox*, const IntPoint&);
#endif

private:
    AutoscrollController();
    void autoscrollTimerFired(Timer<AutoscrollController>*);
    void startAutoscrollTimer();
#if OS(WINDOWS)
    void updatePanScrollState(FrameView*, const IntPoint&);
#endif

    Timer<AutoscrollController> m_autoscrollTimer;
    RenderBox* m_autoscrollRenderer;
    AutoscrollType m_autoscrollType;
    IntPoint m_dragAndDropAutoscrollReferencePosition;
    double m_dragAndDropAutoscrollStartTime;
#if OS(WINDOWS)
    IntPoint m_panScrollStartPos;
#endif
};

} // namespace WebCore

#endif // AutoscrollController_h
