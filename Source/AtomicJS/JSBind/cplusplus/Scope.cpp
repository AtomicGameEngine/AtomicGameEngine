// Copyright (c) 2008 Roberto Raggi <roberto.raggi@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "Scope.h"
#include "Symbols.h"
#include "Names.h"
#include "Literals.h"
#include "Templates.h"

#include "cppassert.h"

#include <cstring>

using namespace CPlusPlus;

class CPlusPlus::SymbolTable
{
    SymbolTable(const SymbolTable &other);
    void operator =(const SymbolTable &other);

public:
    typedef Symbol **iterator;

public:
    /// Constructs an empty Scope.
    SymbolTable(Scope *owner = 0);

    /// Destroy this scope.
    ~SymbolTable();

    /// Returns this scope's owner Symbol.
    Scope *owner() const;

    /// Sets this scope's owner Symbol.
    void setOwner(Scope *owner); // ### remove me

    /// Adds a Symbol to this Scope.
    void enterSymbol(Symbol *symbol);

    /// Returns true if this Scope is empty; otherwise returns false.
    bool isEmpty() const;

    /// Returns the number of symbols is in the scope.
    unsigned symbolCount() const;

    /// Returns the Symbol at the given position.
    Symbol *symbolAt(unsigned index) const;

    /// Returns the first Symbol in the scope.
    iterator firstSymbol() const;

    /// Returns the last Symbol in the scope.
    iterator lastSymbol() const;

    Symbol *lookat(const Identifier *id) const;
    Symbol *lookat(OperatorNameId::Kind operatorId) const;

private:
    /// Returns the hash value for the given Symbol.
    unsigned hashValue(Symbol *symbol) const;

    /// Updates the hash table.
    void rehash();

private:
    enum { DefaultInitialSize = 4 };

    Scope *_owner;
    Symbol **_symbols;
    Symbol **_hash;
    int _allocatedSymbols;
    int _symbolCount;
    int _hashSize;
};

SymbolTable::SymbolTable(Scope *owner)
    : _owner(owner),
      _symbols(0),
      _hash(0),
      _allocatedSymbols(0),
      _symbolCount(-1),
      _hashSize(0)
{ }

SymbolTable::~SymbolTable()
{
    if (_symbols)
        free(_symbols);
    if (_hash)
        free(_hash);
}

void SymbolTable::enterSymbol(Symbol *symbol)
{
    CPP_ASSERT(! symbol->_enclosingScope || symbol->enclosingScope() == _owner, return);

    if (++_symbolCount == _allocatedSymbols) {
        _allocatedSymbols <<= 1;
        if (! _allocatedSymbols)
            _allocatedSymbols = DefaultInitialSize;

        _symbols = reinterpret_cast<Symbol **>(realloc(_symbols, sizeof(Symbol *) * _allocatedSymbols));
    }

    symbol->_index = _symbolCount;
    symbol->_enclosingScope = _owner;
    _symbols[_symbolCount] = symbol;

    if (_symbolCount * 5 >= _hashSize * 3)
        rehash();
    else {
        const unsigned h = hashValue(symbol);
        symbol->_next = _hash[h];
        _hash[h] = symbol;
    }
}

Symbol *SymbolTable::lookat(const Identifier *id) const
{
    if (! _hash || ! id)
        return 0;

    const unsigned h = id->hashCode() % _hashSize;
    Symbol *symbol = _hash[h];
    for (; symbol; symbol = symbol->_next) {
        const Name *identity = symbol->unqualifiedName();
        if (! identity) {
            continue;
        } else if (const Identifier *nameId = identity->asNameId()) {
            if (nameId->identifier()->match(id))
                break;
        } else if (const TemplateNameId *t = identity->asTemplateNameId()) {
            if (t->identifier()->match(id))
                break;
        } else if (const DestructorNameId *d = identity->asDestructorNameId()) {
            if (d->identifier()->match(id))
                break;
        } else if (identity->isQualifiedNameId()) {
            return 0;
        } else if (const SelectorNameId *selectorNameId = identity->asSelectorNameId()) {
            if (selectorNameId->identifier()->match(id))
                break;
        }
    }
    return symbol;
}

Symbol *SymbolTable::lookat(OperatorNameId::Kind operatorId) const
{
    if (! _hash)
        return 0;

    const unsigned h = operatorId % _hashSize;
    Symbol *symbol = _hash[h];
    for (; symbol; symbol = symbol->_next) {
        if (const Name *identity = symbol->unqualifiedName()) {
            if (const OperatorNameId *op = identity->asOperatorNameId()) {
                if (op->kind() == operatorId)
                    break;
            }
        }
    }
    return symbol;
}

void SymbolTable::rehash()
{
    _hashSize <<= 1;

    if (! _hashSize)
        _hashSize = DefaultInitialSize;

    _hash = reinterpret_cast<Symbol **>(realloc(_hash, sizeof(Symbol *) * _hashSize));
    std::memset(_hash, 0, sizeof(Symbol *) * _hashSize);

    for (int index = 0; index < _symbolCount + 1; ++index) {
        Symbol *symbol = _symbols[index];
        const unsigned h = hashValue(symbol);
        symbol->_next = _hash[h];
        _hash[h] = symbol;
    }
}

unsigned SymbolTable::hashValue(Symbol *symbol) const
{
    if (! symbol)
        return 0;

    return symbol->hashCode() % _hashSize;
}

bool SymbolTable::isEmpty() const
{ return _symbolCount == -1; }

unsigned SymbolTable::symbolCount() const
{ return _symbolCount + 1; }

Symbol *SymbolTable::symbolAt(unsigned index) const
{
    if (! _symbols)
        return 0;
    return _symbols[index];
}

SymbolTable::iterator SymbolTable::firstSymbol() const
{ return _symbols; }

SymbolTable::iterator SymbolTable::lastSymbol() const
{ return _symbols + _symbolCount + 1; }

Scope::Scope(TranslationUnit *translationUnit, unsigned sourceLocation, const Name *name)
    : Symbol(translationUnit, sourceLocation, name),
      _members(0),
      _startOffset(0),
      _endOffset(0)
{ }

Scope::Scope(Clone *clone, Subst *subst, Scope *original)
    : Symbol(clone, subst, original)
    , _members(0)
    , _startOffset(original->_startOffset)
    , _endOffset(original->_endOffset)
{
    for (iterator it = original->firstMember(), end = original->lastMember(); it != end; ++it)
        addMember(clone->symbol(*it, subst));
}

Scope::~Scope()
{ delete _members; }

/// Adds a Symbol to this Scope.
void Scope::addMember(Symbol *symbol)
{
    if (! _members)
        _members = new SymbolTable(this);

    _members->enterSymbol(symbol);
}

/// Returns true if this Scope is empty; otherwise returns false.
bool Scope::isEmpty() const
{ return _members ? _members->isEmpty() : true; }

/// Returns the number of symbols is in the scope.
unsigned Scope::memberCount() const
{ return _members ? _members->symbolCount() : 0; }

/// Returns the Symbol at the given position.
Symbol *Scope::memberAt(unsigned index) const
{ return _members ? _members->symbolAt(index) : 0; }

/// Returns the first Symbol in the scope.
Scope::iterator Scope::firstMember() const
{ return _members ? _members->firstSymbol() : 0; }

/// Returns the last Symbol in the scope.
Scope::iterator Scope::lastMember() const
{ return _members ? _members->lastSymbol() : 0; }

Symbol *Scope::find(const Identifier *id) const
{ return _members ? _members->lookat(id) : 0; }

Symbol *Scope::find(OperatorNameId::Kind operatorId) const
{ return _members ? _members->lookat(operatorId) : 0; }

/// Set the start offset of the scope
unsigned Scope::startOffset() const
{ return _startOffset; }

void Scope::setStartOffset(unsigned offset)
{ _startOffset = offset; }

/// Set the end offset of the scope
unsigned Scope::endOffset() const
{ return _endOffset; }

void Scope::setEndOffset(unsigned offset)
{ _endOffset = offset; }
