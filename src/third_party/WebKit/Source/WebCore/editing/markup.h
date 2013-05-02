/*
 * Copyright (C) 2004 Apple Computer, Inc.  All rights reserved.
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

#ifndef markup_h
#define markup_h

#include "FragmentScriptingPermission.h"
#include "HTMLInterchange.h"
#include <wtf/Forward.h>
#include <wtf/Vector.h>

namespace WebCore {

class ContainerNode;
class Document;
class DocumentFragment;
class Element;
class HTMLElement;
class KURL;
class Node;
class QualifiedName;
class Range;

typedef int ExceptionCode;

enum EChildrenOnly { IncludeNode, ChildrenOnly };
enum EAbsoluteURLs { DoNotResolveURLs, ResolveAllURLs, ResolveNonLocalURLs };

PassRefPtr<DocumentFragment> createFragmentFromText(Range* context, const String& text);
PassRefPtr<DocumentFragment> createFragmentFromMarkup(Document*, const String& markup, const String& baseURL, ParserContentPolicy = AllowScriptingContent);
PassRefPtr<DocumentFragment> createFragmentFromMarkupWithContext(Document*, const String& markup, unsigned fragmentStart, unsigned fragmentEnd, const String& baseURL, ParserContentPolicy);
PassRefPtr<DocumentFragment> createFragmentFromNodes(Document*, const Vector<Node*>&);
PassRefPtr<DocumentFragment> createFragmentForInnerOuterHTML(const String&, Element*, ParserContentPolicy, ExceptionCode&);
PassRefPtr<DocumentFragment> createFragmentForTransformToFragment(const String&, const String& sourceMIMEType, Document* outputDoc);
PassRefPtr<DocumentFragment> createContextualFragment(const String&, HTMLElement*, ParserContentPolicy, ExceptionCode&);

bool isPlainTextMarkup(Node*);

// These methods are used by HTMLElement & ShadowRoot to replace the
// children with respected fragment/text.
void replaceChildrenWithFragment(ContainerNode*, PassRefPtr<DocumentFragment>, ExceptionCode&);
void replaceChildrenWithText(ContainerNode*, const String&, ExceptionCode&);

String createMarkup(const Range*, Vector<Node*>* = 0, EAnnotateForInterchange = DoNotAnnotateForInterchange, bool convertBlocksToInlines = false, EAbsoluteURLs = DoNotResolveURLs, Node* constrainingAncestor = 0);
String createMarkup(const Node*, EChildrenOnly = IncludeNode, Vector<Node*>* = 0, EAbsoluteURLs = DoNotResolveURLs, Vector<QualifiedName>* tagNamesToSkip = 0);

String createFullMarkup(const Node*);
String createFullMarkup(const Range*);

String urlToMarkup(const KURL&, const String& title);

}

#endif // markup_h
