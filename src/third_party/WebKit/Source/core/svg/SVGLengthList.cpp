/*
 * Copyright (C) 2004, 2005, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006 Rob Buis <buis@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "core/svg/SVGLengthList.h"

#include "bindings/v8/ExceptionStatePlaceholder.h"
#include "core/svg/SVGAnimationElement.h"
#include "core/svg/SVGParserUtilities.h"
#include "wtf/text/StringBuilder.h"

namespace WebCore {

inline PassRefPtr<SVGLengthList> toSVGLengthList(PassRefPtr<SVGPropertyBase> passBase)
{
    RefPtr<SVGPropertyBase> base = passBase;
    ASSERT(base->type() == SVGLengthList::classType());
    return static_pointer_cast<SVGLengthList>(base.release());
}

SVGLengthList::SVGLengthList(SVGLengthMode mode)
    : m_mode(mode)
{
}

SVGLengthList::~SVGLengthList()
{
}

PassRefPtr<SVGLengthList> SVGLengthList::clone()
{
    RefPtr<SVGLengthList> ret = SVGLengthList::create(m_mode);
    ret->deepCopy(this);
    return ret.release();
}

PassRefPtr<SVGPropertyBase> SVGLengthList::cloneForAnimation(const String& value) const
{
    RefPtr<SVGLengthList> ret = SVGLengthList::create(m_mode);
    ret->setValueAsString(value, IGNORE_EXCEPTION);
    return ret.release();
}

String SVGLengthList::valueAsString() const
{
    StringBuilder builder;

    ConstIterator it = begin();
    ConstIterator itEnd = end();
    if (it != itEnd) {
        builder.append(it->valueAsString());
        ++it;

        for (; it != itEnd; ++it) {
            builder.append(' ');
            builder.append(it->valueAsString());
        }
    }

    return builder.toString();
}

template <typename CharType>
void SVGLengthList::parseInternal(const CharType*& ptr, const CharType* end, ExceptionState& exceptionState)
{
    clear();
    while (ptr < end) {
        const CharType* start = ptr;
        while (ptr < end && *ptr != ',' && !isSVGSpace(*ptr))
            ptr++;
        if (ptr == start)
            break;

        RefPtr<SVGLength> length = SVGLength::create(m_mode);
        String valueString(start, ptr - start);
        if (valueString.isEmpty())
            return;
        length->setValueAsString(valueString, exceptionState);
        if (exceptionState.hadException())
            return;
        append(length);
        skipOptionalSVGSpacesOrDelimiter(ptr, end);
    }
}

void SVGLengthList::setValueAsString(const String& value, ExceptionState& exceptionState)
{
    if (value.isEmpty())
        return;
    if (value.is8Bit()) {
        const LChar* ptr = value.characters8();
        const LChar* end = ptr + value.length();
        parseInternal(ptr, end, exceptionState);
    } else {
        const UChar* ptr = value.characters16();
        const UChar* end = ptr + value.length();
        parseInternal(ptr, end, exceptionState);
    }
}

void SVGLengthList::add(PassRefPtr<SVGPropertyBase> other, SVGElement* contextElement)
{
    RefPtr<SVGLengthList> otherList = toSVGLengthList(other);

    if (length() != otherList->length())
        return;

    SVGLengthContext lengthContext(contextElement);
    for (size_t i = 0; i < length(); ++i)
        at(i)->setValue(at(i)->value(lengthContext) + otherList->at(i)->value(lengthContext), lengthContext, ASSERT_NO_EXCEPTION);
}

bool SVGLengthList::adjustFromToListValues(PassRefPtr<SVGLengthList> passFromList, PassRefPtr<SVGLengthList> passToList, float percentage, bool isToAnimation, bool resizeAnimatedListIfNeeded)
{
    RefPtr<SVGLengthList> fromList = passFromList;
    RefPtr<SVGLengthList> toList = passToList;

    // If no 'to' value is given, nothing to animate.
    size_t toListSize = toList->length();
    if (!toListSize)
        return false;

    // If the 'from' value is given and it's length doesn't match the 'to' value list length, fallback to a discrete animation.
    size_t fromListSize = fromList->length();
    if (fromListSize != toListSize && fromListSize) {
        if (percentage < 0.5) {
            if (!isToAnimation)
                deepCopy(fromList);
        } else {
            deepCopy(toList);
        }

        return false;
    }

    ASSERT(!fromListSize || fromListSize == toListSize);
    if (resizeAnimatedListIfNeeded && length() < toListSize) {
        size_t paddingCount = toListSize - length();
        for (size_t i = 0; i < paddingCount; ++i)
            append(SVGLength::create(m_mode));
    }

    return true;
}

void SVGLengthList::calculateAnimatedValue(SVGAnimationElement* animationElement, float percentage, unsigned repeatCount, PassRefPtr<SVGPropertyBase> fromValue, PassRefPtr<SVGPropertyBase> toValue, PassRefPtr<SVGPropertyBase> toAtEndOfDurationValue, SVGElement* contextElement)
{
    RefPtr<SVGLengthList> fromList = toSVGLengthList(fromValue);
    RefPtr<SVGLengthList> toList = toSVGLengthList(toValue);
    RefPtr<SVGLengthList> toAtEndOfDurationList = toSVGLengthList(toAtEndOfDurationValue);

    SVGLengthContext lengthContext(contextElement);
    ASSERT(m_mode == SVGLength::lengthModeForAnimatedLengthAttribute(animationElement->attributeName()));

    size_t fromLengthListSize = fromList->length();
    size_t toLengthListSize = toList->length();
    size_t toAtEndOfDurationListSize = toAtEndOfDurationList->length();

    if (!adjustFromToListValues(fromList, toList, percentage, animationElement->animationMode() == ToAnimation, true))
        return;

    for (size_t i = 0; i < toLengthListSize; ++i) {
        float animatedNumber = at(i)->value(lengthContext);
        SVGLengthType unitType = toList->at(i)->unitType();
        float effectiveFrom = 0;
        if (fromLengthListSize) {
            if (percentage < 0.5)
                unitType = fromList->at(i)->unitType();
            effectiveFrom = fromList->at(i)->value(lengthContext);
        }
        float effectiveTo = toList->at(i)->value(lengthContext);
        float effectiveToAtEnd = i < toAtEndOfDurationListSize ? toAtEndOfDurationList->at(i)->value(lengthContext) : 0;

        animationElement->animateAdditiveNumber(percentage, repeatCount, effectiveFrom, effectiveTo, effectiveToAtEnd, animatedNumber);
        at(i)->setUnitType(unitType);
        at(i)->setValue(animatedNumber, lengthContext, ASSERT_NO_EXCEPTION);
    }
}

float SVGLengthList::calculateDistance(PassRefPtr<SVGPropertyBase> to, SVGElement*)
{
    // FIXME: Distance calculation is not possible for SVGLengthList right now. We need the distance for every single value.
    return -1;
}
}
