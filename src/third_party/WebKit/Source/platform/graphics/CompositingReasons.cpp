// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/graphics/CompositingReasons.h"

#include "wtf/StdLibExtras.h"

namespace blink {

const CompositingReasonStringMap kCompositingReasonStringMap[] = {
    { CompositingReasonNone,
        "Unknown",
        "No reason given" },
    { CompositingReason3DTransform,
        "transform3D",
        "Has a 3d transform" },
    { CompositingReasonVideo,
        "video",
        "Is an accelerated video" },
    { CompositingReasonCanvas,
        "canvas",
        "Is an accelerated canvas" },
    { CompositingReasonPlugin,
        "plugin",
        "Is an accelerated plugin" },
    { CompositingReasonIFrame,
        "iFrame",
        "Is an accelerated iFrame" },
    { CompositingReasonBackfaceVisibilityHidden,
        "backfaceVisibilityHidden",
        "Has backface-visibility: hidden" },
    { CompositingReasonActiveAnimation,
        "activeAnimation",
        "Has an active accelerated animation or transition" },
    { CompositingReasonTransitionProperty,
        "transitionProperty",
        "Has an acceleratable transition property (active or inactive)" },
    { CompositingReasonPositionFixed,
        "positionFixed",
        "Is fixed position" },
    { CompositingReasonOverflowScrollingTouch,
        "overflowScrollingTouch",
        "Is a scrollable overflow element" },
    { CompositingReasonOverflowScrollingParent,
        "overflowScrollingParent",
        "Scroll parent is not an ancestor" },
    { CompositingReasonOutOfFlowClipping,
        "outOfFlowClipping",
        "Has clipping ancestor" },
    { CompositingReasonVideoOverlay,
        "videoOverlay",
        "Is overlay controls for video" },
    { CompositingReasonWillChangeCompositingHint,
        "willChange",
        "Has a will-change compositing hint" },
    { CompositingReasonAssumedOverlap,
        "assumedOverlap",
        "Might overlap other composited content" },
    { CompositingReasonOverlap,
        "overlap",
        "Overlaps other composited content" },
    { CompositingReasonNegativeZIndexChildren,
        "negativeZIndexChildren",
        "Parent with composited negative z-index content" },
    { CompositingReasonScrollsWithRespectToSquashingLayer,
        "scrollsWithRespectToSquashingLayer",
        "Cannot be squashed since this layer scrolls with respect to the squashing layer" },
    { CompositingReasonSquashingSparsityExceeded,
        "squashingSparsityExceeded",
        "Cannot be squashed as the squashing layer would become too sparse" },
    { CompositingReasonSquashingClippingContainerMismatch,
        "squashingClippingContainerMismatch",
        "Cannot be squashed because this layer has a different clipping container than the squashing layer" },
    { CompositingReasonSquashingOpacityAncestorMismatch,
        "squashingOpacityAncestorMismatch",
        "Cannot be squashed because this layer has a different opacity ancestor than the squashing layer" },
    { CompositingReasonSquashingTransformAncestorMismatch,
        "squashingTransformAncestorMismatch",
        "Cannot be squashed because this layer has a different transform ancestor than the squashing layer" },
    { CompositingReasonSquashingFilterMismatch,
        "squashingFilterAncestorMismatch",
        "Cannot be squashed because this layer has a different filter ancestor than the squashing layer, or this layer has a filter" },
    { CompositingReasonSquashingWouldBreakPaintOrder,
        "squashingWouldBreakPaintOrder",
        "Cannot be squashed without breaking paint order" },
    { CompositingReasonSquashingVideoIsDisallowed,
        "squashingVideoIsDisallowed",
        "Squashing video is not supported" },
    { CompositingReasonSquashedLayerClipsCompositingDescendants,
        "squashedLayerClipsCompositingDescendants",
        "Squashing a layer that clips composited descendants is not supported." },
    { CompositingReasonSquashingRenderPartIsDisallowed,
        "squashingRenderPartIsDisallowed",
        "Squashing a frame, iframe or plugin is not supported." },
    { CompositingReasonSquashingReflectionIsDisallowed,
        "squashingReflectionDisallowed",
        "Squashing a element with a reflection is not supported." },
    { CompositingReasonTransformWithCompositedDescendants,
        "transformWithCompositedDescendants",
        "Has a transform that needs to be known by compositor because of composited descendants" },
    { CompositingReasonOpacityWithCompositedDescendants,
        "opacityWithCompositedDescendants",
        "Has opacity that needs to be applied by compositor because of composited descendants" },
    { CompositingReasonMaskWithCompositedDescendants,
        "maskWithCompositedDescendants",
        "Has a mask that needs to be known by compositor because of composited descendants" },
    { CompositingReasonReflectionWithCompositedDescendants,
        "reflectionWithCompositedDescendants",
        "Has a reflection that needs to be known by compositor because of composited descendants" },
    { CompositingReasonFilterWithCompositedDescendants,
        "filterWithCompositedDescendants",
        "Has a filter effect that needs to be known by compositor because of composited descendants" },
    { CompositingReasonBlendingWithCompositedDescendants,
        "blendingWithCompositedDescendants",
        "Has a blenidng effect that needs to be known by compositor because of composited descendants" },
    { CompositingReasonClipsCompositingDescendants,
        "clipsCompositingDescendants",
        "Has a clip that needs to be known by compositor because of composited descendants" },
    { CompositingReasonPerspectiveWith3DDescendants,
        "perspectiveWith3DDescendants",
        "Has a perspective transform that needs to be known by compositor because of 3d descendants" },
    { CompositingReasonPreserve3DWith3DDescendants,
        "preserve3DWith3DDescendants",
        "Has a preserves-3d property that needs to be known by compositor because of 3d descendants" },
    { CompositingReasonReflectionOfCompositedParent,
        "reflectionOfCompositedParent",
        "Is a reflection of a composited layer" },
    { CompositingReasonIsolateCompositedDescendants,
        "isolateCompositedDescendants",
        "Should isolate descendants to apply a blend effect" },
    { CompositingReasonRoot,
        "root",
        "Is the root layer" },
    { CompositingReasonLayerForAncestorClip,
        "layerForAncestorClip",
        "Secondary layer, applies a clip due to a sibling in the compositing tree" },
    { CompositingReasonLayerForDescendantClip,
        "layerForDescendantClip",
        "Secondary layer, to clip descendants of the owning layer" },
    { CompositingReasonLayerForPerspective,
        "layerForPerspective",
        "Secondary layer, to house the perspective transform for all descendants" },
    { CompositingReasonLayerForHorizontalScrollbar,
        "layerForHorizontalScrollbar",
        "Secondary layer, the horizontal scrollbar layer" },
    { CompositingReasonLayerForVerticalScrollbar,
        "layerForVerticalScrollbar",
        "Secondary layer, the vertical scrollbar layer" },
    { CompositingReasonLayerForOverflowControlsHost,
        "layerForOverflowControlsHost",
        "Secondary layer, the overflow controls host layer" },
    { CompositingReasonLayerForScrollCorner,
        "layerForScrollCorner",
        "Secondary layer, the scroll corner layer" },
    { CompositingReasonLayerForScrollingContents,
        "layerForScrollingContents",
        "Secondary layer, to house contents that can be scrolled" },
    { CompositingReasonLayerForScrollingContainer,
        "layerForScrollingContainer",
        "Secondary layer, used to position the scolling contents while scrolling" },
    { CompositingReasonLayerForSquashingContents,
        "layerForSquashingContents",
        "Secondary layer, home for a group of squashable content" },
    { CompositingReasonLayerForSquashingContainer,
        "layerForSquashingContainer",
        "Secondary layer, no-op layer to place the squashing layer correctly in the composited layer tree" },
    { CompositingReasonLayerForForeground,
        "layerForForeground",
        "Secondary layer, to contain any normal flow and positive z-index contents on top of a negative z-index layer" },
    { CompositingReasonLayerForBackground,
        "layerForBackground",
        "Secondary layer, to contain acceleratable background content" },
    { CompositingReasonLayerForMask,
        "layerForMask",
        "Secondary layer, to contain the mask contents" },
    { CompositingReasonLayerForClippingMask,
        "layerForClippingMask",
        "Secondary layer, for clipping mask" },
    { CompositingReasonLayerForScrollingBlockSelection,
        "layerForScrollingBlockSelection",
        "Secondary layer, to house block selection gaps for composited scrolling with no scrolling contents" },
    { CompositingReasonInlineTransform,
        "inlineTransform",
        "Has an inline transform, which causes subsequent layers to assume overlap" },
};

size_t kNumberOfCompositingReasons = WTF_ARRAY_LENGTH(kCompositingReasonStringMap);

} // namespace blink
