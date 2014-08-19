/*
 * Copyright (C) 2014 Bloomberg Finance L.P.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS," WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <blpwtk2_nativeviewwidget.h>

#include <blpwtk2_nativeviewwidgetdelegate.h>

#include <ui/views/controls/native/native_view_host.h>
#include <ui/views/layout/fill_layout.h>
#include <ui/views/widget/widget.h>
#include <ui/views/win/hwnd_util.h>

namespace blpwtk2 {

NativeViewWidget::NativeViewWidget(gfx::NativeView contents,
                                   blpwtk2::NativeView parent,
                                   NativeViewWidgetDelegate* delegate)
: d_delegate(delegate)
, d_nativeViewHost(new views::NativeViewHost())
, d_impl(new views::Widget())
{
    SetLayoutManager(new views::FillLayout());
    AddChildView(d_nativeViewHost);

    views::Widget::InitParams params;
    params.bounds = gfx::Rect(0, 0, 10, 10);
    params.delegate = this;
    params.type = views::Widget::InitParams::TYPE_WINDOW_FRAMELESS;
    params.opacity = views::Widget::InitParams::OPAQUE_WINDOW;
    d_impl->set_focus_on_creation(false);
    d_impl->Init(params);
    d_nativeViewHost->Attach(contents);

    // Windows-specific code!
    HWND hwnd = views::HWNDForWidget(d_impl);
    LONG style = ::GetWindowLong(hwnd, GWL_STYLE);
    style &= ~WS_POPUP;  // was added due to TYPE_WINDOW_FRAMELESS
    style |= WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    ::SetWindowLong(hwnd, GWL_STYLE, style);
    ::SetParent(hwnd, parent);
}

NativeViewWidget::~NativeViewWidget()
{
    DCHECK(!d_impl);
}

void NativeViewWidget::destroy()
{
    DCHECK(d_impl);
    d_impl->Close();
}

void NativeViewWidget::setDelegate(NativeViewWidgetDelegate* delegate)
{
    d_delegate = delegate;
}

void NativeViewWidget::setParent(blpwtk2::NativeView parent)
{
    DCHECK(d_impl);
    HWND hwnd = views::HWNDForWidget(d_impl);
    ::SetParent(hwnd, parent);
}

void NativeViewWidget::show()
{
    DCHECK(d_impl);
    d_impl->Show();
}

void NativeViewWidget::hide()
{
    DCHECK(d_impl);
    d_impl->Hide();
}

void NativeViewWidget::move(int x, int y, int width, int height)
{
    DCHECK(d_impl);
    d_impl->SetBounds(gfx::Rect(x, y, width, height));
}

void NativeViewWidget::focus()
{
    DCHECK(d_impl);
    HWND hwnd = views::HWNDForWidget(d_impl);
    ::SetFocus(hwnd);
}

blpwtk2::NativeView NativeViewWidget::getNativeWidgetView() const
{
    DCHECK(d_impl);
    return views::HWNDForWidget(d_impl);
}

// views::WidgetDelegate overrides

void NativeViewWidget::WindowClosing()
{
    // WindowClosing() sometimes gets called twice.  This happens when
    // WM_DESTROY is sent to the same HWND twice (!!).
    if (!d_impl)
        return;

    d_impl = 0;
    if (d_delegate)
        d_delegate->onDestroyed(this);
}

views::View* NativeViewWidget::GetContentsView()
{
    return this;
}

bool NativeViewWidget::OnNCHitTest(int* result, const gfx::Point& point)
{
    if (d_delegate)
        return d_delegate->OnNCHitTest(result);
    return false;
}

bool NativeViewWidget::OnNCDragBegin(int hit_test_code)
{
    if (d_delegate)
        return d_delegate->OnNCDragBegin(hit_test_code);
    return false;
}

void NativeViewWidget::OnNCDragMove()
{
    if (d_delegate)
        d_delegate->OnNCDragMove();
}

void NativeViewWidget::OnNCDragEnd()
{
    if (d_delegate)
        d_delegate->OnNCDragEnd();
}

}  // close namespace blpwtk2

