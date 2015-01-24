/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://www.qt.io/licensing.  For further information
** use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file.  Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "CoreTypes.h"
#include "Names.h"
#include "SafeMatcher.h"

using namespace std;
using namespace CPlusPlus;

namespace {
template<typename T>
class Blocker
{
    vector<const T *> &v;

public:
    Blocker(vector<const T *> &v, const T *el1, const T *el2)
        : v(v)
    { v.push_back(el1); v.push_back(el2); }
    ~Blocker() { v.pop_back(); v.pop_back(); }
};

template<typename T, typename U>
bool isBlocked(const vector<const T *> &v, const U *t1, const U *t2)
{
    for (size_t i = v.size(); i > 0; ) {
        const T *t = v[--i];
        if (t == t1 || t == t2)
            return true;
    }

    return false;
}
} // anonymous namespace

SafeMatcher::SafeMatcher()
{
    _blockedTypes.reserve(8);
    _blockedNames.reserve(8);
}

SafeMatcher::~SafeMatcher()
{}

bool SafeMatcher::match(const PointerToMemberType *type, const PointerToMemberType *otherType)
{
    if (isBlocked(_blockedTypes, type, otherType))
        return true;

    Blocker<Type> b(_blockedTypes, type, otherType);
    return Matcher::match(type, otherType);
}

bool SafeMatcher::match(const PointerType *type, const PointerType *otherType)
{
    if (isBlocked(_blockedTypes, type, otherType))
        return true;

    Blocker<Type> b(_blockedTypes, type, otherType);
    return Matcher::match(type, otherType);
}

bool SafeMatcher::match(const ReferenceType *type, const ReferenceType *otherType)
{
    if (isBlocked(_blockedTypes, type, otherType))
        return true;

    Blocker<Type> b(_blockedTypes, type, otherType);
    return Matcher::match(type, otherType);
}

bool SafeMatcher::match(const ArrayType *type, const ArrayType *otherType)
{
    if (isBlocked(_blockedTypes, type, otherType))
        return true;

    Blocker<Type> b(_blockedTypes, type, otherType);
    return Matcher::match(type, otherType);
}

bool SafeMatcher::match(const NamedType *type, const NamedType *otherType)
{
    if (isBlocked(_blockedTypes, type, otherType))
        return true;

    Blocker<Type> b(_blockedTypes, type, otherType);
    return Matcher::match(type, otherType);
}

bool SafeMatcher::match(const TemplateNameId *name, const TemplateNameId *otherName)
{
    if (isBlocked(_blockedNames, name, otherName))
        return true;

    Blocker<Name> b(_blockedNames, name, otherName);
    return Matcher::match(name, otherName);
}

bool SafeMatcher::match(const DestructorNameId *name, const DestructorNameId *otherName)
{
    if (isBlocked(_blockedNames, name, otherName))
        return true;

    Blocker<Name> b(_blockedNames, name, otherName);
    return Matcher::match(name, otherName);
}

bool SafeMatcher::match(const ConversionNameId *name, const ConversionNameId *otherName)
{
    if (isBlocked(_blockedNames, name, otherName))
        return true;

    Blocker<Name> b(_blockedNames, name, otherName);
    return Matcher::match(name, otherName);
}

bool SafeMatcher::match(const QualifiedNameId *name, const QualifiedNameId *otherName)
{
    if (isBlocked(_blockedNames, name, otherName))
        return true;

    Blocker<Name> b(_blockedNames, name, otherName);
    return Matcher::match(name, otherName);
}

bool SafeMatcher::match(const SelectorNameId *name, const SelectorNameId *otherName)
{
    if (isBlocked(_blockedNames, name, otherName))
        return true;

    Blocker<Name> b(_blockedNames, name, otherName);
    return Matcher::match(name, otherName);
}
