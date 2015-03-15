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

#include "TranslationUnit.h"
#include "Control.h"
#include "Parser.h"
#include "Lexer.h"
#include "MemoryPool.h"
#include "AST.h"
#include "Literals.h"
#include "DiagnosticClient.h"
#include <stack>
#include <vector>
#include <cstdarg>
#include <algorithm>
#include <utility>

#if defined(_MSC_VER) && (_MSC_VER < 1800)
#    define va_copy(dst, src) ((dst) = (src))
#elif defined(__INTEL_COMPILER) && !defined(va_copy)
#    define va_copy __va_copy
#endif

using namespace CPlusPlus;

const Token TranslationUnit::nullToken;

TranslationUnit::TranslationUnit(Control *control, const StringLiteral *fileId)
    : _control(control),
      _fileId(fileId),
      _firstSourceChar(0),
      _lastSourceChar(0),
      _pool(0),
      _ast(0),
      _flags(0)
{
    _tokens = new std::vector<Token>();
    _comments = new std::vector<Token>();
    _previousTranslationUnit = control->switchTranslationUnit(this);
    _pool = new MemoryPool();
}

TranslationUnit::~TranslationUnit()
{
    (void) _control->switchTranslationUnit(_previousTranslationUnit);
    release();
}

Control *TranslationUnit::control() const
{ return _control; }

const StringLiteral *TranslationUnit::fileId() const
{ return _fileId; }

const char *TranslationUnit::fileName() const
{ return _fileId->chars(); }

unsigned TranslationUnit::fileNameLength() const
{ return _fileId->size(); }

const char *TranslationUnit::firstSourceChar() const
{ return _firstSourceChar; }

const char *TranslationUnit::lastSourceChar() const
{ return _lastSourceChar; }

unsigned TranslationUnit::sourceLength() const
{ return _lastSourceChar - _firstSourceChar; }

void TranslationUnit::setSource(const char *source, unsigned size)
{
    _firstSourceChar = source;
    _lastSourceChar = source + size;
}

const char *TranslationUnit::spell(unsigned index) const
{
    if (! index)
        return 0;

    return tokenAt(index).spell();
}

unsigned TranslationUnit::commentCount() const
{ return unsigned(_comments->size()); }

const Token &TranslationUnit::commentAt(unsigned index) const
{ return _comments->at(index); }

const Identifier *TranslationUnit::identifier(unsigned index) const
{ return tokenAt(index).identifier; }

const Literal *TranslationUnit::literal(unsigned index) const
{ return tokenAt(index).literal; }

const StringLiteral *TranslationUnit::stringLiteral(unsigned index) const
{ return tokenAt(index).string; }

const NumericLiteral *TranslationUnit::numericLiteral(unsigned index) const
{ return tokenAt(index).number; }

unsigned TranslationUnit::matchingBrace(unsigned index) const
{ return tokenAt(index).close_brace; }

MemoryPool *TranslationUnit::memoryPool() const
{ return _pool; }

AST *TranslationUnit::ast() const
{ return _ast; }

bool TranslationUnit::isTokenized() const
{ return f._tokenized; }

bool TranslationUnit::isParsed() const
{ return f._parsed; }

void TranslationUnit::tokenize()
{
    if (isTokenized())
        return;

    f._tokenized = true;

    Lexer lex(this);
    lex.setLanguageFeatures(_languageFeatures);
    lex.setScanCommentTokens(true);

    std::stack<unsigned> braces;
    _tokens->push_back(nullToken); // the first token needs to be invalid!

    pushLineOffset(0);
    pushPreprocessorLine(0, 1, fileId());

    const Identifier *lineId   = control()->identifier("line");
    const Identifier *expansionId = control()->identifier("expansion");
    const Identifier *beginId = control()->identifier("begin");
    const Identifier *endId = control()->identifier("end");

    // We need to track information about the expanded tokens. A vector with an addition
    // explicit index control is used instead of queue mainly for performance reasons.
    std::vector<std::pair<unsigned, unsigned> > lineColumn;
    unsigned lineColumnIdx = 0;

    Token tk;
    do {
        lex(&tk);

recognize:
        if (tk.is(T_POUND) && tk.newline()) {
            const unsigned utf16CharOffset = tk.utf16charOffset;
            lex(&tk);

            if (! tk.newline() && tk.is(T_IDENTIFIER) && tk.identifier == expansionId) {
                // It's an expansion mark.
                lex(&tk);

                if (!tk.newline() && tk.is(T_IDENTIFIER)) {
                    if (tk.identifier == beginId) {
                        // Start of a macro expansion section.
                        lex(&tk);

                        // Gather where the expansion happens and its length.
                        //unsigned macroOffset = static_cast<unsigned>(strtoul(tk.spell(), 0, 0));
                        lex(&tk);
                        lex(&tk); // Skip the separating comma
                        //unsigned macroLength = static_cast<unsigned>(strtoul(tk.spell(), 0, 0));
                        lex(&tk);

                        // NOTE: We are currently not using the macro offset and length. They
                        // are kept here for now because of future use.
                        //Q_UNUSED(macroOffset)
                        //Q_UNUSED(macroLength)

                        // Now we need to gather the real line and columns from the upcoming
                        // tokens. But notice this is only relevant for tokens which are expanded
                        // but not generated.
                        while (tk.isNot(T_EOF_SYMBOL) && !tk.newline()) {
                            // When we get a ~ it means there's a number of generated tokens
                            // following. Otherwise, we have actual data.
                            if (tk.is(T_TILDE)) {
                                lex(&tk);

                                // Get the total number of generated tokens and specify "null"
                                // information for them.
                                unsigned totalGenerated =
                                        static_cast<unsigned>(strtoul(tk.spell(), 0, 0));
                                const std::size_t previousSize = lineColumn.size();
                                lineColumn.resize(previousSize + totalGenerated);
                                std::fill(lineColumn.begin() + previousSize,
                                          lineColumn.end(),
                                          std::make_pair(0, 0));

                                lex(&tk);
                            } else if (tk.is(T_NUMERIC_LITERAL)) {
                                unsigned line = static_cast<unsigned>(strtoul(tk.spell(), 0, 0));
                                lex(&tk);
                                lex(&tk); // Skip the separating colon
                                unsigned column = static_cast<unsigned>(strtoul(tk.spell(), 0, 0));

                                // Store line and column for this non-generated token.
                                lineColumn.push_back(std::make_pair(line, column));

                                lex(&tk);
                            }
                        }
                    } else if (tk.identifier == endId) {
                        // End of a macro expansion.
                        lineColumn.clear();
                        lineColumnIdx = 0;

                        lex(&tk);
                    }
                }
            } else {
                if (! tk.newline() && tk.is(T_IDENTIFIER) && tk.identifier == lineId)
                    lex(&tk);
                if (! tk.newline() && tk.is(T_NUMERIC_LITERAL)) {
                    unsigned line = (unsigned) strtoul(tk.spell(), 0, 0);
                    lex(&tk);
                    if (! tk.newline() && tk.is(T_STRING_LITERAL)) {
                        const StringLiteral *fileName =
                                control()->stringLiteral(tk.string->chars(), tk.string->size());
                        pushPreprocessorLine(utf16CharOffset, line, fileName);
                        lex(&tk);
                    }
                }
                while (tk.isNot(T_EOF_SYMBOL) && ! tk.newline())
                    lex(&tk);
            }
            goto recognize;
        } else if (tk.kind() == T_LBRACE) {
            braces.push(unsigned(_tokens->size()));
        } else if (tk.kind() == T_RBRACE && ! braces.empty()) {
            const unsigned open_brace_index = braces.top();
            braces.pop();
            if (open_brace_index < tokenCount())
                (*_tokens)[open_brace_index].close_brace = unsigned(_tokens->size());
        } else if (tk.isComment()) {
            _comments->push_back(tk);
            continue; // comments are not in the regular token stream
        }

        bool currentExpanded = false;
        bool currentGenerated = false;

        if (!lineColumn.empty() && lineColumnIdx < lineColumn.size()) {
            currentExpanded = true;
            const std::pair<unsigned, unsigned> &p = lineColumn[lineColumnIdx];
            if (p.first)
                _expandedLineColumn.insert(std::make_pair(tk.utf16charsBegin(), p));
            else
                currentGenerated = true;

            ++lineColumnIdx;
        }

        tk.f.expanded = currentExpanded;
        tk.f.generated = currentGenerated;

        _tokens->push_back(tk);
    } while (tk.kind());

    for (; ! braces.empty(); braces.pop()) {
        unsigned open_brace_index = braces.top();
        (*_tokens)[open_brace_index].close_brace = unsigned(_tokens->size());
    }
}

bool TranslationUnit::skipFunctionBody() const
{ return f._skipFunctionBody; }

void TranslationUnit::setSkipFunctionBody(bool skipFunctionBody)
{ f._skipFunctionBody = skipFunctionBody; }

bool TranslationUnit::parse(ParseMode mode)
{
    if (isParsed())
        return false;

    if (! isTokenized())
        tokenize();

    f._parsed = true;

    Parser parser(this);
    bool parsed = false;

    switch (mode) {
    case ParseTranlationUnit: {
        TranslationUnitAST *node = 0;
        parsed = parser.parseTranslationUnit(node);
        _ast = node;
    } break;

    case ParseDeclaration: {
        DeclarationAST *node = 0;
        parsed = parser.parseDeclaration(node);
        _ast = node;
    } break;

    case ParseExpression: {
        ExpressionAST *node = 0;
        parsed = parser.parseExpression(node);
        _ast = node;
    } break;

    case ParseDeclarator: {
        DeclaratorAST *node = 0;
        parsed = parser.parseDeclarator(node, /*decl_specifier_list =*/ 0);
        _ast = node;
    } break;

    case ParseStatement: {
        StatementAST *node = 0;
        parsed = parser.parseStatement(node);
        _ast = node;
    } break;

    default:
        break;
    } // switch

    return parsed;
}

void TranslationUnit::pushLineOffset(unsigned offset)
{ _lineOffsets.push_back(offset); }

void TranslationUnit::pushPreprocessorLine(unsigned utf16charOffset,
                                           unsigned line,
                                           const StringLiteral *fileName)
{ _ppLines.push_back(PPLine(utf16charOffset, line, fileName)); }

unsigned TranslationUnit::findLineNumber(unsigned utf16charOffset) const
{
    std::vector<unsigned>::const_iterator it =
        std::lower_bound(_lineOffsets.begin(), _lineOffsets.end(), utf16charOffset);

    if (it != _lineOffsets.begin())
        --it;

    return it - _lineOffsets.begin();
}

TranslationUnit::PPLine TranslationUnit::findPreprocessorLine(unsigned utf16charOffset) const
{
    std::vector<PPLine>::const_iterator it =
        std::lower_bound(_ppLines.begin(), _ppLines.end(), PPLine(utf16charOffset));

    if (it != _ppLines.begin())
        --it;

    return *it;
}

unsigned TranslationUnit::findColumnNumber(unsigned utf16CharOffset, unsigned lineNumber) const
{
    if (! utf16CharOffset)
        return 0;

    return utf16CharOffset - _lineOffsets[lineNumber];
}

void TranslationUnit::getTokenPosition(unsigned index,
                                       unsigned *line,
                                       unsigned *column,
                                       const StringLiteral **fileName) const
{ return getPosition(tokenAt(index).utf16charsBegin(), line, column, fileName); }

void TranslationUnit::getTokenStartPosition(unsigned index, unsigned *line,
                                            unsigned *column,
                                            const StringLiteral **fileName) const
{ return getPosition(tokenAt(index).utf16charsBegin(), line, column, fileName); }

void TranslationUnit::getTokenEndPosition(unsigned index, unsigned *line,
                                          unsigned *column,
                                          const StringLiteral **fileName) const
{ return getPosition(tokenAt(index).utf16charsEnd(), line, column, fileName); }

void TranslationUnit::getPosition(unsigned utf16charOffset,
                                  unsigned *line,
                                  unsigned *column,
                                  const StringLiteral **fileName) const
{
    unsigned lineNumber = 0;
    unsigned columnNumber = 0;
    const StringLiteral *file = 0;

    // If this token is expanded we already have the information directly from the expansion
    // section header. Otherwise, we need to calculate it.
    TokenLineColumn::const_iterator it = _expandedLineColumn.find(utf16charOffset);
    if (it != _expandedLineColumn.end()) {
        lineNumber = it->second.first;
        columnNumber = it->second.second + 1;
        file = _fileId;
    } else {
        // Identify line within the entire translation unit.
        lineNumber = findLineNumber(utf16charOffset);

        // Identify column.
        columnNumber = findColumnNumber(utf16charOffset, lineNumber);

        // Adjust the line in regards to the preprocessing markers.
        const PPLine ppLine = findPreprocessorLine(utf16charOffset);
        lineNumber -= findLineNumber(ppLine.utf16charOffset) + 1;
        lineNumber += ppLine.line;

        file = ppLine.fileName;
    }

    if (line)
        *line = lineNumber;

    if (column)
        *column = columnNumber;

    if (fileName)
       *fileName = file;
}

void TranslationUnit::message(DiagnosticClient::Level level, unsigned index, const char *format, va_list args)
{
    if (f._blockErrors)
        return;

    index = std::min(index, tokenCount() - 1);

    unsigned line = 0, column = 0;
    const StringLiteral *fileName = 0;
    getTokenPosition(index, &line, &column, &fileName);

    if (DiagnosticClient *client = control()->diagnosticClient()) {
        client->report(level, fileName, line, column, format, args);
    } else {
        fprintf(stderr, "%s:%u: ", fileName->chars(), line);
        const char *l = "error";
        if (level == DiagnosticClient::Warning)
            l = "warning";
        else if (level == DiagnosticClient::Fatal)
            l = "fatal";
        fprintf(stderr, "%s: ", l);

        vfprintf(stderr, format, args);
        fputc('\n', stderr);

        showErrorLine(index, column, stderr);
    }

    if (level == DiagnosticClient::Fatal)
        exit(EXIT_FAILURE);
}

void TranslationUnit::warning(unsigned index, const char *format, ...)
{
    if (f._blockErrors)
        return;

    va_list args, ap;
    va_start(args, format);
    va_copy(ap, args);
    message(DiagnosticClient::Warning, index, format, args);
    va_end(ap);
    va_end(args);
}

void TranslationUnit::error(unsigned index, const char *format, ...)
{
    if (f._blockErrors)
        return;

    va_list args, ap;
    va_start(args, format);
    va_copy(ap, args);
    message(DiagnosticClient::Error, index, format, args);
    va_end(ap);
    va_end(args);
}

void TranslationUnit::fatal(unsigned index, const char *format, ...)
{
    if (f._blockErrors)
        return;

    va_list args, ap;
    va_start(args, format);
    va_copy(ap, args);
    message(DiagnosticClient::Fatal, index, format, args);
    va_end(ap);
    va_end(args);
}

unsigned TranslationUnit::findPreviousLineOffset(unsigned tokenIndex) const
{
    unsigned lineOffset = _lineOffsets[findLineNumber(tokenAt(tokenIndex).utf16charsBegin())];
    return lineOffset;
}

bool TranslationUnit::maybeSplitGreaterGreaterToken(unsigned tokenIndex)
{
    if (tokenIndex >= tokenCount())
        return false;
    Token &tok = (*_tokens)[tokenIndex];
    if (tok.kind() != T_GREATER_GREATER)
        return false;

    tok.f.kind = T_GREATER;
    tok.f.bytes = 1;
    tok.f.utf16chars = 1;

    Token newGreater;
    newGreater.f.kind = T_GREATER;
    newGreater.f.expanded = tok.expanded();
    newGreater.f.generated = tok.generated();
    newGreater.f.bytes = 1;
    newGreater.f.utf16chars = 1;
    newGreater.byteOffset = tok.byteOffset + 1;
    newGreater.utf16charOffset = tok.utf16charOffset + 1;

    _tokens->insert(_tokens->begin() + tokenIndex + 1, newGreater);

    TokenLineColumn::const_iterator it = _expandedLineColumn.find(tok.bytesBegin());
    if (it != _expandedLineColumn.end()) {
        const std::pair<unsigned, unsigned> newPosition(it->second.first, it->second.second + 1);
        _expandedLineColumn.insert(std::make_pair(newGreater.bytesBegin(), newPosition));
    }

    return true;
}

void TranslationUnit::releaseTokensAndComments()
{
    delete _tokens;
    _tokens = 0;
    delete _comments;
    _comments = 0;
}

void TranslationUnit::showErrorLine(unsigned index, unsigned column, FILE *out)
{
    unsigned lineOffset = _lineOffsets[findLineNumber(tokenAt(index).utf16charsBegin())];
    for (const char *cp = _firstSourceChar + lineOffset + 1; *cp && *cp != '\n'; ++cp) {
        fputc(*cp, out);
    }
    fputc('\n', out);

    const char *end = _firstSourceChar + lineOffset + 1 + column - 1;
    for (const char *cp = _firstSourceChar + lineOffset + 1; cp != end; ++cp) {
        if (*cp != '\t')
            fputc(' ', out);
        else
            fputc('\t', out);
    }
    fputc('^', out);
    fputc('\n', out);
}

void TranslationUnit::resetAST()
{
    delete _pool;
    _pool = 0;
    _ast = 0;
}

void TranslationUnit::release()
{
    resetAST();
    releaseTokensAndComments();
}
