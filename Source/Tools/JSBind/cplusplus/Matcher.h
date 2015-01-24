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

#ifndef TYPEMATCHER_H
#define TYPEMATCHER_H

#include "CPlusPlusForwardDeclarations.h"

namespace CPlusPlus {

class CPLUSPLUS_EXPORT Matcher
{
    Matcher(const Matcher &other);
    void operator = (const Matcher &other);

public:
    Matcher();
    virtual ~Matcher();

    static bool match(const Type *type, const Type *otherType, Matcher *matcher = 0);
    static bool match(const Name *name, const Name *otherName, Matcher *matcher = 0);

    virtual bool match(const UndefinedType *type, const UndefinedType *otherType);
    virtual bool match(const VoidType *type, const VoidType *otherType);
    virtual bool match(const IntegerType *type, const IntegerType *otherType);
    virtual bool match(const FloatType *type, const FloatType *otherType);
    virtual bool match(const PointerToMemberType *type, const PointerToMemberType *otherType);
    virtual bool match(const PointerType *type, const PointerType *otherType);
    virtual bool match(const ReferenceType *type, const ReferenceType *otherType);
    virtual bool match(const ArrayType *type, const ArrayType *otherType);
    virtual bool match(const NamedType *type, const NamedType *otherType);

    virtual bool match(const Function *type, const Function *otherType);
    virtual bool match(const Enum *type, const Enum *otherType);
    virtual bool match(const Namespace *type, const Namespace *otherType);
    virtual bool match(const Template *type, const Template *otherType);
    virtual bool match(const ForwardClassDeclaration *type, const ForwardClassDeclaration *otherType);
    virtual bool match(const Class *type, const Class *otherType);
    virtual bool match(const ObjCClass *type, const ObjCClass *otherType);
    virtual bool match(const ObjCProtocol *type, const ObjCProtocol *otherType);
    virtual bool match(const ObjCForwardClassDeclaration *type, const ObjCForwardClassDeclaration *otherType);
    virtual bool match(const ObjCForwardProtocolDeclaration *type, const ObjCForwardProtocolDeclaration *otherType);
    virtual bool match(const ObjCMethod *type, const ObjCMethod *otherType);

    virtual bool match(const Identifier *name, const Identifier *otherName);
    virtual bool match(const AnonymousNameId *name, const AnonymousNameId *otherName);
    virtual bool match(const TemplateNameId *name, const TemplateNameId *otherName);
    virtual bool match(const DestructorNameId *name, const DestructorNameId *otherName);
    virtual bool match(const OperatorNameId *name, const OperatorNameId *otherName);
    virtual bool match(const ConversionNameId *name, const ConversionNameId *otherName);
    virtual bool match(const QualifiedNameId *name, const QualifiedNameId *otherName);
    virtual bool match(const SelectorNameId *name, const SelectorNameId *otherName);
};

} // namespace CPlusPlus

#endif // TYPEMATCHER_H
