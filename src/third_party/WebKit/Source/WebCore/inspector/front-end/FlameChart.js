/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

/**
 * @constructor
 * @extends {WebInspector.View}
 * @param {WebInspector.CPUProfileView} cpuProfileView
 */
WebInspector.FlameChart = function(cpuProfileView)
{
    WebInspector.View.call(this);
    this.registerRequiredCSS("flameChart.css");
    this.element.className = "fill";
    this.element.id = "cpu-flame-chart";

    this._overviewContainer = this.element.createChild("div", "overview-container");
    this._overviewGrid = new WebInspector.OverviewGrid("flame-chart");
    this._overviewContainer.appendChild(this._overviewGrid.element);
    this._overviewCalculator = new WebInspector.FlameChart.OverviewCalculator();
    this._overviewGrid.addEventListener(WebInspector.OverviewGrid.Events.WindowChanged, this._onWindowChanged, this);
    this._overviewCanvas = this._overviewContainer.createChild("canvas");

    this._chartContainer = this.element.createChild("div", "chart-container");
    this._timelineGrid = new WebInspector.TimelineGrid();
    this._chartContainer.appendChild(this._timelineGrid.element);
    this._calculator = new WebInspector.FlameChart.Calculator();

    this._canvas = this._chartContainer.createChild("canvas");
    WebInspector.installDragHandle(this._canvas, this._startCanvasDragging.bind(this), this._canvasDragging.bind(this), this._endCanvasDragging.bind(this), "col-resize");

    this._cpuProfileView = cpuProfileView;
    this._windowLeft = 0.0;
    this._windowRight = 1.0;
    this._barHeight = 15;
    this._minWidth = 1;
    this._canvas.addEventListener("mousemove", this._onMouseMove.bind(this), false);
    this._canvas.addEventListener("mousewheel", this._onMouseWheel.bind(this), false);
    this.element.addEventListener("click", this._onClick.bind(this), false);
    this._popoverHelper = new WebInspector.PopoverHelper(this._chartContainer, this._getPopoverAnchor.bind(this), this._showPopover.bind(this));
    this._popoverHelper.setTimeout(250);
    this._anchorElement = this._chartContainer.createChild("span");
    this._anchorElement.className = "item-anchor";
    this._linkifier = new WebInspector.Linkifier();
    this._highlightedNodeIndex = -1;
}

/**
 * @constructor
 * @implements {WebInspector.TimelineGrid.Calculator}
 */
WebInspector.FlameChart.Calculator = function()
{
}

WebInspector.FlameChart.Calculator.prototype = {
    /**
     * @param {WebInspector.FlameChart} flameChart
     */
    _updateBoundaries: function(flameChart)
    {
        this._minimumBoundaries = flameChart._windowLeft * flameChart._timelineData.totalTime;
        this._maximumBoundaries = flameChart._windowRight * flameChart._timelineData.totalTime;
        this._timeToPixel = flameChart._canvas.width / this.boundarySpan();
    },

    /**
     * @param {number} time
     */
    computePosition: function(time)
    {
        return (time - this._minimumBoundaries) * this._timeToPixel;
    },

    formatTime: function(value)
    {
        return Number.secondsToString((value + this._minimumBoundaries) / 1000);
    },

    maximumBoundary: function()
    {
        return this._maximumBoundaries;
    },

    minimumBoundary: function()
    {
        return this._minimumBoundaries;
    },

    boundarySpan: function()
    {
        return this._maximumBoundaries - this._minimumBoundaries;
    }
}

/**
 * @constructor
 * @implements {WebInspector.TimelineGrid.Calculator}
 */
WebInspector.FlameChart.OverviewCalculator = function()
{
}

WebInspector.FlameChart.OverviewCalculator.prototype = {
    /**
     * @param {WebInspector.FlameChart} flameChart
     */
    _updateBoundaries: function(flameChart)
    {
        this._minimumBoundaries = 0;
        this._maximumBoundaries = flameChart._timelineData.totalTime;
        this._xScaleFactor = flameChart._canvas.width / flameChart._timelineData.totalTime;
    },

    /**
     * @param {number} time
     */
    computePosition: function(time)
    {
        return (time - this._minimumBoundaries) * this._xScaleFactor;
    },

    formatTime: function(value)
    {
        return Number.secondsToString((value + this._minimumBoundaries) / 1000);
    },

    maximumBoundary: function()
    {
        return this._maximumBoundaries;
    },

    minimumBoundary: function()
    {
        return this._minimumBoundaries;
    },

    boundarySpan: function()
    {
        return this._maximumBoundaries - this._minimumBoundaries;
    }
}

WebInspector.FlameChart.Events = {
    SelectedNode: "SelectedNode"
}

WebInspector.FlameChart.prototype = {
    _onWindowChanged: function(event)
    {
        this._hidePopover();
        this._scheduleUpdate();
    },

    _startCanvasDragging: function(event)
    {
        if (!this._timelineData)
            return false;
        this._isDragging = true;
        this._dragStartPoint = event.pageX;
        this._dragStartWindowLeft = this._windowLeft;
        this._dragStartWindowRight = this._windowRight;
        this._hidePopover();
        return true;
    },

    _canvasDragging: function(event)
    {
        var pixelShift = this._dragStartPoint - event.pageX;
        var windowShift = pixelShift / this._totalPixels;

        var windowLeft = Math.max(0, this._dragStartWindowLeft + windowShift);
        if (windowLeft === this._windowLeft)
            return;
        windowShift = windowLeft - this._dragStartWindowLeft;
        
        var windowRight = Math.min(1, this._dragStartWindowRight + windowShift);
        if (windowRight === this._windowRight)
            return;
        windowShift = windowRight - this._dragStartWindowRight;

        this._overviewGrid.setWindow(this._dragStartWindowLeft + windowShift, this._dragStartWindowRight + windowShift);
    },

    _endCanvasDragging: function()
    {
        this._isDragging = false;
    },

    _calculateTimelineData: function()
    {
        if (this._cpuProfileView.samples)
            return this._calculateTimelineDataForSamples();

        if (this._timelineData)
            return this._timelineData;

        if (!this._cpuProfileView.profileHead)
            return null;

        var functionColorPairs = { };
        var currentColorIndex = 0;

        var index = 0;
        var entries = [];

        function appendReversedArray(toArray, fromArray)
        {
            for (var i = fromArray.length - 1; i >= 0; --i)
                toArray.push(fromArray[i]);
        }

        var stack = [];
        appendReversedArray(stack, this._cpuProfileView.profileHead.children);

        var levelOffsets = /** @type {Array.<!number>} */ ([0]);
        var levelExitIndexes = /** @type {Array.<!number>} */ ([0]);

        while (stack.length) {
            var level = levelOffsets.length - 1;
            var node = stack.pop();
            var offset = levelOffsets[level];

            var functionUID = node.functionName + ":" + node.url + ":" + node.lineNumber;
            var colorPair = functionColorPairs[functionUID];
            if (!colorPair) {
                ++currentColorIndex;
                var hue = (currentColorIndex * 5 + 11 * (currentColorIndex % 2)) % 360;
                functionColorPairs[functionUID] = colorPair = {highlighted: "hsl(" + hue + ", 100%, 33%)", normal: "hsl(" + hue + ", 100%, 66%)"};
            }

            entries.push({
                colorPair: colorPair,
                depth: level,
                duration: node.totalTime,
                startTime: offset,
                node: node
            });


            ++index;

            levelOffsets[level] += node.totalTime;
            if (node.children.length) {
                levelExitIndexes.push(stack.length);
                levelOffsets.push(offset + node.selfTime / 2);
                appendReversedArray(stack, node.children);
            }

            while (stack.length === levelExitIndexes[levelExitIndexes.length - 1]) {
                levelOffsets.pop();
                levelExitIndexes.pop();
            }
        }

        this._timelineData = {
            entries: entries,
            totalTime: this._cpuProfileView.profileHead.totalTime,
        }

        return this._timelineData;
    },

    _calculateTimelineDataForSamples: function()
    {
        if (this._timelineData)
            return this._timelineData;

        if (!this._cpuProfileView.profileHead)
            return null;

        var samples = this._cpuProfileView.samples;
        var idToNode = this._cpuProfileView._idToNode;
        var samplesCount = samples.length;
        var functionColorPairs = { };
        var currentColorIndex = 0;

        var index = 0;
        var entries = [];

        var openIntervals = [];
        var stackTrace = [];
        for (var sampleIndex = 0; sampleIndex < samplesCount; sampleIndex++) {
            var node = idToNode[samples[sampleIndex]];
            stackTrace.length = 0;
            while (node) {
                stackTrace.push(node);
                node = node.parent;
            }
            stackTrace.pop(); // Remove (root) node

            var depth = 0;
            node = stackTrace.pop();
            while (node && depth < openIntervals.length && node === openIntervals[depth].node) {
                var intervalIndex = openIntervals[depth].index;
                entries[intervalIndex].duration += 1;
                node = stackTrace.pop();
                ++depth;
            }
            if (depth < openIntervals.length)
                openIntervals.length = depth;
            if (!node)
                continue;

            while (node) {
                var functionUID = node.functionName + ":" + node.url + ":" + node.lineNumber;
                var colorPair = functionColorPairs[functionUID];
                if (!colorPair) {
                    ++currentColorIndex;
                    var hue = (currentColorIndex * 5 + 11 * (currentColorIndex % 2)) % 360;
                    functionColorPairs[functionUID] = colorPair = {highlighted: "hsl(" + hue + ", 100%, 33%)", normal: "hsl(" + hue + ", 100%, 66%)"};
                }

                entries.push({
                    colorPair: colorPair,
                    depth: depth,
                    duration: 1,
                    startTime: sampleIndex,
                    node: node
                });

                openIntervals.push({node: node, index: index});

                ++index;

                node = stackTrace.pop();
                ++depth;
            }
        }

        this._timelineData = {
            entries: entries,
            totalTime: samplesCount,
        };

        return this._timelineData;
    },

    _getPopoverAnchor: function()
    {
        if (this._highlightedNodeIndex === -1 || this._isDragging)
            return null;
        return this._anchorElement;
    },

    _showPopover: function(anchor, popover)
    {
        if (this._isDragging)
            return;
        var node = this._timelineData.entries[this._highlightedNodeIndex].node;
        if (!node)
            return;
        var contentHelper = new WebInspector.PopoverContentHelper(node.functionName);
        contentHelper.appendTextRow(WebInspector.UIString("Total time"), Number.secondsToString(node.totalTime / 1000, true));
        contentHelper.appendTextRow(WebInspector.UIString("Self time"), Number.secondsToString(node.selfTime / 1000, true));
        if (node.numberOfCalls)
            contentHelper.appendTextRow(WebInspector.UIString("Number of calls"), node.numberOfCalls);
        if (node.url) {
            var link = this._linkifier.linkifyLocation(node.url, node.lineNumber);
            contentHelper.appendElementRow("Location", link);
        }

        popover.show(contentHelper._contentTable, anchor);
    },

    _hidePopover: function()
    {
        this._popoverHelper.hidePopover();
        this._linkifier.reset();
    },

    _onClick: function(e)
    {
        if (this._highlightedNodeIndex === -1)
            return;
        var node = this._timelineData.entries[this._highlightedNodeIndex].node;
        this.dispatchEventToListeners(WebInspector.FlameChart.Events.SelectedNode, node);
    },

    _onMouseMove: function(e)
    {
        if (this._isDragging)
            return;
        var nodeIndex = this._coordinatesToNodeIndex(e.offsetX, e.offsetY);
        if (nodeIndex === this._highlightedNodeIndex)
            return;
        this._highlightedNodeIndex = nodeIndex;
        this.update();

        if (nodeIndex === -1)
            return;

        var timelineEntries = this._timelineData.entries;

        var anchorLeft = Math.floor(timelineEntries[nodeIndex].startTime * this._timeToPixel - this._pixelWindowLeft);
        anchorLeft = Number.constrain(anchorLeft, 0, this._canvas.width);

        var anchorWidth = Math.floor(timelineEntries[nodeIndex].duration * this._timeToPixel);

        anchorWidth = Number.constrain(anchorWidth, 0, this._canvas.width - anchorLeft);

        var style = this._anchorElement.style;
        style.width = anchorWidth + "px";
        style.height = this._barHeight + "px";
        style.left = anchorLeft + "px";
        style.top = Math.floor(this._canvas.height - (timelineEntries[nodeIndex].depth + 1) * this._barHeight) + "px";
    },

    _onMouseWheel: function(e)
    {
        var zoomFactor = (e.wheelDelta > 0) ? 0.9 : 1.1;
        var windowPoint = (this._pixelWindowLeft + e.offsetX) / this._totalPixels;
        var overviewReferencePoint = Math.floor(windowPoint * this._pixelWindowWidth);
        this._overviewGrid.zoom(zoomFactor, overviewReferencePoint);
        this._hidePopover();
    },

    /**
     * @param {!number} x
     * @param {!number} y
     */
    _coordinatesToNodeIndex: function(x, y)
    {
        var timelineData = this._timelineData;
        if (!timelineData)
            return -1;
        var timelineEntries = timelineData.entries;
        var cursorTime = Math.floor((x + this._pixelWindowLeft) * this._pixelToTime);
        var cursorLevel = Math.floor((this._canvas.height - y) / this._barHeight);

        for (var i = 0; i < timelineEntries.length; ++i) {
            if (cursorTime < timelineEntries[i].startTime)
                return -1;
            if (cursorTime < (timelineEntries[i].startTime + timelineEntries[i].duration)
                && cursorLevel === timelineEntries[i].depth)
                return i;
        }
        return -1;
    },

    onResize: function()
    {
        this._updateOverviewCanvas = true;
        this._hidePopover();
        this._scheduleUpdate();
    },

    _drawOverviewCanvas: function(width, height)
    {
        this._overviewCanvas.width = width;
        this._overviewCanvas.height = height;

        if (!this._timelineData)
            return;

        var timelineEntries = this._timelineData.entries;

        var drawData = new Uint8Array(width);
        var scaleFactor = width / this._totalTime;

        for (var nodeIndex = 0; nodeIndex < timelineEntries.length; ++nodeIndex) {
            var entry = timelineEntries[nodeIndex];
            var start = Math.floor(entry.startTime * scaleFactor);
            var finish = Math.floor((entry.startTime + entry.duration) * scaleFactor);
            for (var x = start; x < finish; ++x)
                drawData[x] = Math.max(drawData[x], entry.depth + 1);
        }

        var context = this._overviewCanvas.getContext("2d");
        var yScaleFactor = 2;
        context.lineWidth = 0.5;
        context.strokeStyle = "rgba(20,0,0,0.8)";
        context.fillStyle="rgba(214,225,254, 0.8)";
        context.moveTo(0, height - 1);
        for (var x = 0; x < width; ++x)
            context.lineTo(x, height - drawData[x] * yScaleFactor - 1);
        context.moveTo(width - 1, height - 1);
        context.moveTo(0, height - 1);
        context.fill();
        context.stroke();
        context.closePath();
    },

    /**
     * @param {!number} height
     * @param {!number} width
     */
    draw: function(width, height)
    {
        var timelineData = this._calculateTimelineData();
        if (!timelineData)
            return;
        var timelineEntries = timelineData.entries;
        this._canvas.height = height;
        this._canvas.width = width;
        var barHeight = this._barHeight;

        var context = this._canvas.getContext("2d");
        var paddingLeft = 2;
        context.font = (barHeight - 3) + "px sans-serif";
        context.textBaseline = "top";
        this._dotsWidth = context.measureText("\u2026").width;

        for (var i = 0; i < timelineEntries.length; ++i) {
            var startTime = timelineEntries[i].startTime;
            if (startTime > this._timeWindowRight)
                break;
            if ((startTime + timelineEntries[i].duration) < this._timeWindowLeft)
                continue;
            var x = Math.floor(startTime * this._timeToPixel) - this._pixelWindowLeft;
            var y = height - (timelineEntries[i].depth + 1) * barHeight;
            var barWidth = Math.floor(timelineEntries[i].duration * this._timeToPixel);
            if (barWidth < this._minWidth)
                continue;

            var colorPair = timelineEntries[i].colorPair;
            var color;
            if (this._highlightedNodeIndex === i)
                color =  colorPair.highlighted;
            else
                color = colorPair.normal;

            context.beginPath();
            context.rect(x, y, barWidth - 1, barHeight - 1);
            context.fillStyle = color;
            context.fill();

            var xText = Math.max(0, x);
            var widthText = barWidth - paddingLeft + x - xText;
            var title = this._prepareTitle(context, timelineData.entries[i].node.functionName, barWidth - paddingLeft - xText + x);
            if (title) {
                context.fillStyle = "#333";
                context.fillText(title, xText + paddingLeft, y - 1);
            }
        }
    },

    _prepareTitle: function(context, title, maxSize)
    {
        if (maxSize < this._dotsWidth)
            return null;
        var titleWidth = context.measureText(title).width;
        if (maxSize > titleWidth)
            return title;
        maxSize -= this._dotsWidth;
        var dotRegExp=/[\.\$]/g;
        var match = dotRegExp.exec(title);
        if (!match) {
            var visiblePartSize = maxSize / titleWidth;
            var newTextLength = Math.floor(title.length * visiblePartSize) + 1;
            var minTextLength = 4;
            if (newTextLength < minTextLength)
                return null;
            var substring;
            do {
                --newTextLength;
                substring = title.substring(0, newTextLength);
            } while (context.measureText(substring).width > maxSize);
            return title.substring(0, newTextLength) + "\u2026";
        }
        while (match) {
            var substring = title.substring(match.index + 1);
            var width = context.measureText(substring).width;
            if (maxSize > width)
                return "\u2026" + substring;
            match = dotRegExp.exec(title);
        }
    },

    _scheduleUpdate: function()
    {
        if (this._updateTimerId)
            return;
        this._updateTimerId = setTimeout(this.update.bind(this), 10);
    },
    
    _updateBoundaries: function()
    {
        this._windowLeft = this._overviewGrid.windowLeft();
        this._windowRight = this._overviewGrid.windowRight();
        this._windowWidth = this._windowRight - this._windowLeft;

        this._totalTime = this._timelineData.totalTime;
        this._timeWindowLeft = this._windowLeft * this._totalTime;
        this._timeWindowRight = this._windowRight * this._totalTime;

        this._pixelWindowWidth = this._chartContainer.clientWidth;
        this._totalPixels = Math.floor(this._pixelWindowWidth / this._windowWidth);
        this._pixelWindowLeft = Math.floor(this._totalPixels * this._windowLeft);
        this._pixelWindowRight = Math.floor(this._totalPixels * this._windowRight);

        this._timeToPixel = this._totalPixels / this._totalTime;
        this._pixelToTime = this._totalTime / this._totalPixels;
    },

    update: function()
    {
        this._updateTimerId = 0;
        if (!this._timelineData)
            this._calculateTimelineData();
        if (!this._timelineData)
            return;
        this._updateBoundaries();
        this.draw(this._chartContainer.clientWidth, this._chartContainer.clientHeight);
        this._calculator._updateBoundaries(this);
        this._overviewCalculator._updateBoundaries(this);
        this._timelineGrid.element.style.width = this.element.clientWidth;
        this._timelineGrid.updateDividers(this._calculator);
        this._overviewGrid.updateDividers(this._overviewCalculator);
        if (this._updateOverviewCanvas) {
            this._drawOverviewCanvas(this._overviewContainer.clientWidth, this._overviewContainer.clientHeight);
            this._updateOverviewCanvas = false;
        }
    },

    __proto__: WebInspector.View.prototype
};
