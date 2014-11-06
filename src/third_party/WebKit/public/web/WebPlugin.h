/*
 * Copyright (C) 2009, 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebPlugin_h
#define WebPlugin_h

#include "../platform/WebCanvas.h"
#include "../platform/WebString.h"
#include "../platform/WebURL.h"
#include "WebDragOperation.h"
#include "WebDragStatus.h"
#include "WebWidget.h"

struct NPObject;
struct _NPP;

namespace blink {

class WebDataSource;
class WebDragData;
class WebInputEvent;
class WebPluginContainer;
class WebURLResponse;
struct WebCompositionUnderline;
struct WebCursorInfo;
struct WebPluginParams;
struct WebPrintParams;
struct WebPoint;
struct WebRect;
struct WebTextInputInfo;
struct WebURLError;
template <typename T> class WebVector;

class WebPlugin {
public:
    virtual bool initialize(WebPluginContainer*) = 0;
    virtual void destroy() = 0;

    virtual WebPluginContainer* container() const { return 0; }

    virtual NPObject* scriptableObject() = 0;
    virtual struct _NPP* pluginNPP() { return 0; }

    // Returns true if the form submission value is successfully obtained
    // from the plugin. The value would be associated with the name attribute
    // of the corresponding object element.
    virtual bool getFormValue(WebString&) { return false; }
    virtual bool supportsKeyboardFocus() const { return false; }
    virtual bool supportsEditCommands() const { return false; }
    // Returns true if this plugin supports input method, which implements
    // setComposition() and confirmComposition() below.
    virtual bool supportsInputMethod() const { return false; }

    virtual bool canProcessDrag() const { return false; }

    virtual void paint(WebCanvas*, const WebRect&) = 0;

    // Coordinates are relative to the containing window.
    virtual void updateGeometry(
        const WebRect& frameRect, const WebRect& clipRect,
        const WebVector<WebRect>& cutOutsRects, bool isVisible) = 0;

    virtual void updateFocus(bool) = 0;
    virtual void updateVisibility(bool) = 0;

    virtual bool acceptsInputEvents() = 0;
    virtual bool handleInputEvent(const WebInputEvent&, WebCursorInfo&) = 0;

    virtual bool handleDragStatusUpdate(WebDragStatus, const WebDragData&, WebDragOperationsMask, const WebPoint& position, const WebPoint& screenPosition) { return false; }

    virtual void didReceiveResponse(const WebURLResponse&) = 0;
    virtual void didReceiveData(const char* data, int dataLength) = 0;
    virtual void didFinishLoading() = 0;
    virtual void didFailLoading(const WebURLError&) = 0;

    // Called in response to WebPluginContainer::loadFrameRequest
    virtual void didFinishLoadingFrameRequest(
        const WebURL&, void* notifyData) = 0;
    virtual void didFailLoadingFrameRequest(
        const WebURL&, void* notifyData, const WebURLError&) = 0;

    // Printing interface.
    // Whether the plugin supports its own paginated print. The other print
    // interface methods are called only if this method returns true.
    virtual bool supportsPaginatedPrint() { return false; }
    // Returns true if the printed content should not be scaled to
    // the printer's printable area.
    virtual bool isPrintScalingDisabled() { return false; }

    // Sets up printing with the specified printParams. Returns the number of
    // pages to be printed at these settings.
    virtual int printBegin(const WebPrintParams& printParams) { return 0; }

    // Prints the page specified by pageNumber (0-based index) into the supplied canvas.
    virtual bool printPage(int pageNumber, WebCanvas* canvas) { return false; }
    // Ends the print operation.
    virtual void printEnd() { }

    virtual bool hasSelection() const { return false; }
    virtual WebString selectionAsText() const { return WebString(); }
    virtual WebString selectionAsMarkup() const { return WebString(); }

    virtual bool executeEditCommand(const WebString& name) { return false; }
    virtual bool executeEditCommand(const WebString& name, const WebString& value) { return false; }

    // Sets composition text from input method, and returns true if the
    // composition is set successfully.
    virtual bool setComposition(const WebString& text, const WebVector<WebCompositionUnderline>& underlines, int selectionStart, int selectionEnd) { return false; }
    // Confirms an ongoing composition and returns true if there is an ongoing
    // composition or the text is inserted.
    virtual bool confirmComposition(const WebString& text, WebWidget::ConfirmCompositionBehavior selectionBehavior) { return false; }
    // Deletes the current selection plus the specified number of characters
    // before and after the selection or caret.
    virtual void extendSelectionAndDelete(int before, int after) { }
    // If the given position is over a link, returns the absolute url.
    // Otherwise an empty url is returned.
    virtual WebURL linkAtPosition(const WebPoint& position) const { return WebURL(); }

    // Used for zooming of full page plugins.
    virtual void setZoomLevel(double level, bool textOnly) { }

    // Find interface.
    // Start a new search.  The plugin should search for a little bit at a time so that it
    // doesn't block the thread in case of a large document.  The results, along with the
    // find's identifier, should be sent asynchronously to WebFrameClient's reportFindInPage* methods.
    // Returns true if the search started, or false if the plugin doesn't support search.
    virtual bool startFind(const WebString& searchText, bool caseSensitive, int identifier) { return false; }
    // Tells the plugin to jump forward or backward in the list of find results.
    virtual void selectFindResult(bool forward) { }
    // Tells the plugin that the user has stopped the find operation.
    virtual void stopFind() { }

    // View rotation types.
    enum RotationType {
        RotationType90Clockwise,
        RotationType90Counterclockwise
    };
    // Whether the plugin can rotate the view of its content.
    virtual bool canRotateView() { return false; }
    // Rotates the plugin's view of its content.
    virtual void rotateView(RotationType type) { }

    virtual bool isPlaceholder() { return true; }

protected:
    ~WebPlugin() { }
};

} // namespace blink

#endif
