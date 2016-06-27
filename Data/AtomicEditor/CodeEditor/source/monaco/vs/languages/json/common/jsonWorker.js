/*!-----------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Version: 0.5.2(d49899a916fd99840f6f9178f2dd06e0e7013646)
 * Released under the MIT license
 * https://github.com/Microsoft/vscode/blob/master/LICENSE.txt
 *-----------------------------------------------------------*/

(function() {
var __m = ["exports","require","vs/nls","vs/nls!vs/languages/json/common/jsonWorker","vs/base/common/strings","vs/platform/request/common/request","vs/base/common/types","vs/base/common/json","vs/languages/json/common/parser/jsonParser","vs/base/common/winjs.base","vs/nls!vs/languages/json/common/contributions/globPatternContribution","vs/languages/json/common/contributions/globPatternContribution","vs/nls!vs/base/common/http","vs/nls!vs/languages/json/common/contributions/packageJSONContribution","vs/nls!vs/languages/json/common/contributions/projectJSONContribution","vs/nls!vs/languages/json/common/jsonIntellisense","vs/nls!vs/languages/json/common/jsonSchemaService","vs/nls!vs/languages/json/common/parser/jsonParser","vs/base/common/http","vs/platform/workspace/common/workspace","vs/base/common/jsonFormatter","vs/languages/json/common/jsonIntellisense","vs/languages/json/common/parser/jsonLocation","vs/base/common/errors","vs/languages/json/common/contributions/bowerJSONContribution","vs/nls!vs/languages/json/common/contributions/bowerJSONContribution","vs/languages/json/common/contributions/packageJSONContribution","vs/languages/json/common/contributions/projectJSONContribution","vs/languages/json/common/jsonSchemaService","vs/editor/common/services/resourceService","vs/base/common/severity","vs/base/common/uri","vs/base/common/arrays","vs/base/common/lifecycle","vs/languages/json/common/jsonWorker","vs/base/common/objects","vs/editor/common/modes","vs/platform/markers/common/markers","vs/platform/instantiation/common/instantiation","vs/editor/common/modes/supports/suggestSupport","vs/editor/common/worker/validationHelper"];
var __M = function(deps) {
  var result = [];
  for (var i = 0, len = deps.length; i < len; i++) {
    result[i] = __m[deps[i]];
  }
  return result;
};
define(__m[22], __M([1,0]), function (require, exports) {
    /*---------------------------------------------------------------------------------------------
     *  Copyright (c) Microsoft Corporation. All rights reserved.
     *  Licensed under the MIT License. See License.txt in the project root for license information.
     *--------------------------------------------------------------------------------------------*/
    'use strict';
    var JSONLocation = (function () {
        function JSONLocation(segments) {
            this.segments = segments;
        }
        JSONLocation.prototype.append = function (segment) {
            return new JSONLocation(this.segments.concat(segment));
        };
        JSONLocation.prototype.getSegments = function () {
            return this.segments;
        };
        JSONLocation.prototype.matches = function (segments) {
            var k = 0;
            for (var i = 0; k < segments.length && i < this.segments.length; i++) {
                if (segments[k] === this.segments[i] || segments[k] === '*') {
                    k++;
                }
                else if (segments[k] !== '**') {
                    return false;
                }
            }
            return k === segments.length;
        };
        JSONLocation.prototype.toString = function () {
            return '[' + this.segments.join('][') + ']';
        };
        return JSONLocation;
    }());
    exports.JSONLocation = JSONLocation;
});

define(__m[12], __M([2,3]), function(nls, data) { return nls.create("vs/base/common/http", data); });
define(__m[18], __M([1,0,12]), function (require, exports, nls) {
    /*---------------------------------------------------------------------------------------------
     *  Copyright (c) Microsoft Corporation. All rights reserved.
     *  Licensed under the MIT License. See License.txt in the project root for license information.
     *--------------------------------------------------------------------------------------------*/
    'use strict';
    function getErrorStatusDescription(status) {
        if (status < 400) {
            return void 0;
        }
        switch (status) {
            case 400: return nls.localize(0, null);
            case 401: return nls.localize(1, null);
            case 403: return nls.localize(2, null);
            case 404: return nls.localize(3, null);
            case 405: return nls.localize(4, null);
            case 406: return nls.localize(5, null);
            case 407: return nls.localize(6, null);
            case 408: return nls.localize(7, null);
            case 409: return nls.localize(8, null);
            case 410: return nls.localize(9, null);
            case 411: return nls.localize(10, null);
            case 412: return nls.localize(11, null);
            case 413: return nls.localize(12, null);
            case 414: return nls.localize(13, null);
            case 415: return nls.localize(14, null);
            case 500: return nls.localize(15, null);
            case 501: return nls.localize(16, null);
            case 503: return nls.localize(17, null);
            default: return nls.localize(18, null, status);
        }
    }
    exports.getErrorStatusDescription = getErrorStatusDescription;
});

define(__m[20], __M([1,0,7]), function (require, exports, Json) {
    /*---------------------------------------------------------------------------------------------
     *  Copyright (c) Microsoft Corporation. All rights reserved.
     *  Licensed under the MIT License. See License.txt in the project root for license information.
     *--------------------------------------------------------------------------------------------*/
    'use strict';
    function applyEdit(text, edit) {
        return text.substring(0, edit.offset) + edit.content + text.substring(edit.offset + edit.length);
    }
    exports.applyEdit = applyEdit;
    function applyEdits(text, edits) {
        for (var i = edits.length - 1; i >= 0; i--) {
            text = applyEdit(text, edits[i]);
        }
        return text;
    }
    exports.applyEdits = applyEdits;
    function format(documentText, range, options) {
        var initialIndentLevel;
        var value;
        var rangeStart;
        var rangeEnd;
        if (range) {
            rangeStart = range.offset;
            rangeEnd = rangeStart + range.length;
            while (rangeStart > 0 && !isEOL(documentText, rangeStart - 1)) {
                rangeStart--;
            }
            var scanner_1 = Json.createScanner(documentText, true);
            scanner_1.setPosition(rangeEnd);
            scanner_1.scan();
            rangeEnd = scanner_1.getPosition();
            value = documentText.substring(rangeStart, rangeEnd);
            initialIndentLevel = computeIndentLevel(value, 0, options);
        }
        else {
            value = documentText;
            rangeStart = 0;
            rangeEnd = documentText.length;
            initialIndentLevel = 0;
        }
        var eol = getEOL(options, documentText);
        var lineBreak = false;
        var indentLevel = 0;
        var indentValue;
        if (options.insertSpaces) {
            indentValue = repeat(' ', options.tabSize);
        }
        else {
            indentValue = '\t';
        }
        var scanner = Json.createScanner(value, false);
        function newLineAndIndent() {
            return eol + repeat(indentValue, initialIndentLevel + indentLevel);
        }
        function scanNext() {
            var token = scanner.scan();
            lineBreak = false;
            while (token === Json.SyntaxKind.Trivia || token === Json.SyntaxKind.LineBreakTrivia) {
                lineBreak = lineBreak || (token === Json.SyntaxKind.LineBreakTrivia);
                token = scanner.scan();
            }
            return token;
        }
        var editOperations = [];
        function addEdit(text, startOffset, endOffset) {
            if (documentText.substring(startOffset, endOffset) !== text) {
                editOperations.push({ offset: startOffset, length: endOffset - startOffset, content: text });
            }
        }
        var firstToken = scanNext();
        if (firstToken !== Json.SyntaxKind.EOF) {
            var firstTokenStart = scanner.getTokenOffset() + rangeStart;
            var initialIndent = repeat(indentValue, initialIndentLevel);
            addEdit(initialIndent, rangeStart, firstTokenStart);
        }
        while (firstToken !== Json.SyntaxKind.EOF) {
            var firstTokenEnd = scanner.getTokenOffset() + scanner.getTokenLength() + rangeStart;
            var secondToken = scanNext();
            var replaceContent = '';
            while (!lineBreak && (secondToken === Json.SyntaxKind.LineCommentTrivia || secondToken === Json.SyntaxKind.BlockCommentTrivia)) {
                // comments on the same line: keep them on the same line, but ignore them otherwise
                var commentTokenStart = scanner.getTokenOffset() + rangeStart;
                addEdit(' ', firstTokenEnd, commentTokenStart);
                firstTokenEnd = scanner.getTokenOffset() + scanner.getTokenLength() + rangeStart;
                replaceContent = secondToken === Json.SyntaxKind.LineCommentTrivia ? newLineAndIndent() : '';
                secondToken = scanNext();
            }
            if (secondToken === Json.SyntaxKind.CloseBraceToken) {
                if (firstToken !== Json.SyntaxKind.OpenBraceToken) {
                    indentLevel--;
                    replaceContent = newLineAndIndent();
                }
            }
            else if (secondToken === Json.SyntaxKind.CloseBracketToken) {
                if (firstToken !== Json.SyntaxKind.OpenBracketToken) {
                    indentLevel--;
                    replaceContent = newLineAndIndent();
                }
            }
            else if (secondToken !== Json.SyntaxKind.EOF) {
                switch (firstToken) {
                    case Json.SyntaxKind.OpenBracketToken:
                    case Json.SyntaxKind.OpenBraceToken:
                        indentLevel++;
                        replaceContent = newLineAndIndent();
                        break;
                    case Json.SyntaxKind.CommaToken:
                    case Json.SyntaxKind.LineCommentTrivia:
                        replaceContent = newLineAndIndent();
                        break;
                    case Json.SyntaxKind.BlockCommentTrivia:
                        if (lineBreak) {
                            replaceContent = newLineAndIndent();
                        }
                        else {
                            // symbol following comment on the same line: keep on same line, separate with ' '
                            replaceContent = ' ';
                        }
                        break;
                    case Json.SyntaxKind.ColonToken:
                        replaceContent = ' ';
                        break;
                    case Json.SyntaxKind.NullKeyword:
                    case Json.SyntaxKind.TrueKeyword:
                    case Json.SyntaxKind.FalseKeyword:
                    case Json.SyntaxKind.NumericLiteral:
                        if (secondToken === Json.SyntaxKind.NullKeyword || secondToken === Json.SyntaxKind.FalseKeyword || secondToken === Json.SyntaxKind.NumericLiteral) {
                            replaceContent = ' ';
                        }
                        break;
                }
                if (lineBreak && (secondToken === Json.SyntaxKind.LineCommentTrivia || secondToken === Json.SyntaxKind.BlockCommentTrivia)) {
                    replaceContent = newLineAndIndent();
                }
            }
            var secondTokenStart = scanner.getTokenOffset() + rangeStart;
            addEdit(replaceContent, firstTokenEnd, secondTokenStart);
            firstToken = secondToken;
        }
        return editOperations;
    }
    exports.format = format;
    function repeat(s, count) {
        var result = '';
        for (var i = 0; i < count; i++) {
            result += s;
        }
        return result;
    }
    function computeIndentLevel(content, offset, options) {
        var i = 0;
        var nChars = 0;
        var tabSize = options.tabSize || 4;
        while (i < content.length) {
            var ch = content.charAt(i);
            if (ch === ' ') {
                nChars++;
            }
            else if (ch === '\t') {
                nChars += tabSize;
            }
            else {
                break;
            }
            i++;
        }
        return Math.floor(nChars / tabSize);
    }
    function getEOL(options, text) {
        for (var i = 0; i < text.length; i++) {
            var ch = text.charAt(i);
            if (ch === '\r') {
                if (i + 1 < text.length && text.charAt(i + 1) === '\n') {
                    return '\r\n';
                }
                return '\r';
            }
            else if (ch === '\n') {
                return '\n';
            }
        }
        return (options && options.eol) || '\n';
    }
    function isEOL(text, offset) {
        return '\r\n'.indexOf(text.charAt(offset)) !== -1;
    }
});

define(__m[25], __M([2,3]), function(nls, data) { return nls.create("vs/languages/json/common/contributions/bowerJSONContribution", data); });
define(__m[10], __M([2,3]), function(nls, data) { return nls.create("vs/languages/json/common/contributions/globPatternContribution", data); });
define(__m[11], __M([1,0,4,10]), function (require, exports, Strings, nls) {
    /*---------------------------------------------------------------------------------------------
     *  Copyright (c) Microsoft Corporation. All rights reserved.
     *  Licensed under the MIT License. See License.txt in the project root for license information.
     *--------------------------------------------------------------------------------------------*/
    'use strict';
    var globProperties = [
        { type: 'value', label: nls.localize(0, null), codeSnippet: '"**/*.{{extension}}": true', documentationLabel: nls.localize(1, null) },
        { type: 'value', label: nls.localize(2, null), codeSnippet: '"**/*.{ext1,ext2,ext3}": true', documentationLabel: nls.localize(3, null) },
        { type: 'value', label: nls.localize(4, null), codeSnippet: '"**/*.{{source-extension}}": { "when": "$(basename).{{target-extension}}" }', documentationLabel: nls.localize(5, null) },
        { type: 'value', label: nls.localize(6, null), codeSnippet: '"{{name}}": true', documentationLabel: nls.localize(7, null) },
        { type: 'value', label: nls.localize(8, null), codeSnippet: '"{folder1,folder2,folder3}": true', documentationLabel: nls.localize(9, null) },
        { type: 'value', label: nls.localize(10, null), codeSnippet: '"**/{{name}}": true', documentationLabel: nls.localize(11, null) },
    ];
    var globValues = [
        { type: 'value', label: nls.localize(12, null), codeSnippet: 'true', documentationLabel: nls.localize(13, null) },
        { type: 'value', label: nls.localize(14, null), codeSnippet: 'false', documentationLabel: nls.localize(15, null) },
        { type: 'value', label: nls.localize(16, null), codeSnippet: '{ "when": "$(basename).{{extension}}" }', documentationLabel: nls.localize(17, null) }
    ];
    var GlobPatternContribution = (function () {
        function GlobPatternContribution() {
        }
        GlobPatternContribution.prototype.isSettingsFile = function (resource) {
            var path = resource.path;
            return Strings.endsWith(path, '/settings.json');
        };
        GlobPatternContribution.prototype.collectDefaultSuggestions = function (resource, result) {
            return null;
        };
        GlobPatternContribution.prototype.collectPropertySuggestions = function (resource, location, currentWord, addValue, isLast, result) {
            if (this.isSettingsFile(resource) && (location.matches(['files.exclude']) || location.matches(['search.exclude']))) {
                globProperties.forEach(function (e) { return result.add(e); });
            }
            return null;
        };
        GlobPatternContribution.prototype.collectValueSuggestions = function (resource, location, currentKey, result) {
            if (this.isSettingsFile(resource) && (location.matches(['files.exclude']) || location.matches(['search.exclude']))) {
                globValues.forEach(function (e) { return result.add(e); });
            }
            return null;
        };
        GlobPatternContribution.prototype.getInfoContribution = function (resource, location) {
            return null;
        };
        return GlobPatternContribution;
    }());
    exports.GlobPatternContribution = GlobPatternContribution;
});

define(__m[13], __M([2,3]), function(nls, data) { return nls.create("vs/languages/json/common/contributions/packageJSONContribution", data); });
define(__m[14], __M([2,3]), function(nls, data) { return nls.create("vs/languages/json/common/contributions/projectJSONContribution", data); });
define(__m[15], __M([2,3]), function(nls, data) { return nls.create("vs/languages/json/common/jsonIntellisense", data); });
define(__m[16], __M([2,3]), function(nls, data) { return nls.create("vs/languages/json/common/jsonSchemaService", data); });
define(__m[17], __M([2,3]), function(nls, data) { return nls.create("vs/languages/json/common/parser/jsonParser", data); });
var __extends = (this && this.__extends) || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
};
define(__m[8], __M([1,0,17,32,6,7,22]), function (require, exports, nls, Arrays, Types, Json, jsonLocation_1) {
    /*---------------------------------------------------------------------------------------------
     *  Copyright (c) Microsoft Corporation. All rights reserved.
     *  Licensed under the MIT License. See License.txt in the project root for license information.
     *--------------------------------------------------------------------------------------------*/
    'use strict';
    var ASTNode = (function () {
        function ASTNode(parent, type, name, start, end) {
            this.type = type;
            this.name = name;
            this.start = start;
            this.end = end;
            this.parent = parent;
        }
        ASTNode.prototype.getNodeLocation = function () {
            var path = this.parent ? this.parent.getNodeLocation() : new jsonLocation_1.JSONLocation([]);
            if (this.name) {
                path = path.append(this.name);
            }
            return path;
        };
        ASTNode.prototype.getChildNodes = function () {
            return [];
        };
        ASTNode.prototype.getValue = function () {
            // override in children
            return;
        };
        ASTNode.prototype.contains = function (offset, includeRightBound) {
            if (includeRightBound === void 0) { includeRightBound = false; }
            return offset >= this.start && offset < this.end || includeRightBound && offset === this.end;
        };
        ASTNode.prototype.visit = function (visitor) {
            return visitor(this);
        };
        ASTNode.prototype.getNodeFromOffset = function (offset) {
            var findNode = function (node) {
                if (offset >= node.start && offset < node.end) {
                    var children = node.getChildNodes();
                    for (var i = 0; i < children.length && children[i].start <= offset; i++) {
                        var item = findNode(children[i]);
                        if (item) {
                            return item;
                        }
                    }
                    return node;
                }
                return null;
            };
            return findNode(this);
        };
        ASTNode.prototype.getNodeFromOffsetEndInclusive = function (offset) {
            var findNode = function (node) {
                if (offset >= node.start && offset <= node.end) {
                    var children = node.getChildNodes();
                    for (var i = 0; i < children.length && children[i].start <= offset; i++) {
                        var item = findNode(children[i]);
                        if (item) {
                            return item;
                        }
                    }
                    return node;
                }
                return null;
            };
            return findNode(this);
        };
        ASTNode.prototype.validate = function (schema, validationResult, matchingSchemas, offset) {
            var _this = this;
            if (offset === void 0) { offset = -1; }
            if (offset !== -1 && !this.contains(offset)) {
                return;
            }
            if (Array.isArray(schema.type)) {
                if (Arrays.contains(schema.type, this.type) === false) {
                    validationResult.warnings.push({
                        location: { start: this.start, end: this.end },
                        message: schema.errorMessage || nls.localize(0, null, schema.type.join())
                    });
                }
            }
            else if (schema.type) {
                if (this.type !== schema.type) {
                    validationResult.warnings.push({
                        location: { start: this.start, end: this.end },
                        message: schema.errorMessage || nls.localize(1, null, schema.type)
                    });
                }
            }
            if (Array.isArray(schema.allOf)) {
                schema.allOf.forEach(function (subSchema) {
                    _this.validate(subSchema, validationResult, matchingSchemas, offset);
                });
            }
            if (schema.not) {
                var subValidationResult = new ValidationResult();
                var subMatchingSchemas = [];
                this.validate(schema.not, subValidationResult, subMatchingSchemas, offset);
                if (!subValidationResult.hasErrors()) {
                    validationResult.warnings.push({
                        location: { start: this.start, end: this.end },
                        message: nls.localize(2, null)
                    });
                }
                if (matchingSchemas) {
                    subMatchingSchemas.forEach(function (ms) {
                        ms.inverted = !ms.inverted;
                        matchingSchemas.push(ms);
                    });
                }
            }
            var testAlternatives = function (alternatives, maxOneMatch) {
                var matches = [];
                // remember the best match that is used for error messages
                var bestMatch = null;
                alternatives.forEach(function (subSchema) {
                    var subValidationResult = new ValidationResult();
                    var subMatchingSchemas = [];
                    _this.validate(subSchema, subValidationResult, subMatchingSchemas);
                    if (!subValidationResult.hasErrors()) {
                        matches.push(subSchema);
                    }
                    if (!bestMatch) {
                        bestMatch = { schema: subSchema, validationResult: subValidationResult, matchingSchemas: subMatchingSchemas };
                    }
                    else {
                        if (!maxOneMatch && !subValidationResult.hasErrors() && !bestMatch.validationResult.hasErrors()) {
                            // no errors, both are equally good matches
                            bestMatch.matchingSchemas.push.apply(bestMatch.matchingSchemas, subMatchingSchemas);
                            bestMatch.validationResult.propertiesMatches += subValidationResult.propertiesMatches;
                            bestMatch.validationResult.propertiesValueMatches += subValidationResult.propertiesValueMatches;
                        }
                        else {
                            var compareResult = subValidationResult.compare(bestMatch.validationResult);
                            if (compareResult > 0) {
                                // our node is the best matching so far
                                bestMatch = { schema: subSchema, validationResult: subValidationResult, matchingSchemas: subMatchingSchemas };
                            }
                            else if (compareResult === 0) {
                                // there's already a best matching but we are as good
                                bestMatch.matchingSchemas.push.apply(bestMatch.matchingSchemas, subMatchingSchemas);
                            }
                        }
                    }
                });
                if (matches.length > 1 && maxOneMatch) {
                    validationResult.warnings.push({
                        location: { start: _this.start, end: _this.start + 1 },
                        message: nls.localize(3, null)
                    });
                }
                if (bestMatch !== null) {
                    validationResult.merge(bestMatch.validationResult);
                    validationResult.propertiesMatches += bestMatch.validationResult.propertiesMatches;
                    validationResult.propertiesValueMatches += bestMatch.validationResult.propertiesValueMatches;
                    if (matchingSchemas) {
                        matchingSchemas.push.apply(matchingSchemas, bestMatch.matchingSchemas);
                    }
                }
                return matches.length;
            };
            if (Array.isArray(schema.anyOf)) {
                testAlternatives(schema.anyOf, false);
            }
            if (Array.isArray(schema.oneOf)) {
                testAlternatives(schema.oneOf, true);
            }
            if (Array.isArray(schema.enum)) {
                if (Arrays.contains(schema.enum, this.getValue()) === false) {
                    validationResult.warnings.push({
                        location: { start: this.start, end: this.end },
                        message: nls.localize(4, null, JSON.stringify(schema.enum))
                    });
                }
                else {
                    validationResult.enumValueMatch = true;
                }
            }
            if (matchingSchemas !== null) {
                matchingSchemas.push({ node: this, schema: schema });
            }
        };
        return ASTNode;
    }());
    exports.ASTNode = ASTNode;
    var NullASTNode = (function (_super) {
        __extends(NullASTNode, _super);
        function NullASTNode(parent, name, start, end) {
            _super.call(this, parent, 'null', name, start, end);
        }
        NullASTNode.prototype.getValue = function () {
            return null;
        };
        return NullASTNode;
    }(ASTNode));
    exports.NullASTNode = NullASTNode;
    var BooleanASTNode = (function (_super) {
        __extends(BooleanASTNode, _super);
        function BooleanASTNode(parent, name, value, start, end) {
            _super.call(this, parent, 'boolean', name, start, end);
            this.value = value;
        }
        BooleanASTNode.prototype.getValue = function () {
            return this.value;
        };
        return BooleanASTNode;
    }(ASTNode));
    exports.BooleanASTNode = BooleanASTNode;
    var ArrayASTNode = (function (_super) {
        __extends(ArrayASTNode, _super);
        function ArrayASTNode(parent, name, start, end) {
            _super.call(this, parent, 'array', name, start, end);
            this.items = [];
        }
        ArrayASTNode.prototype.getChildNodes = function () {
            return this.items;
        };
        ArrayASTNode.prototype.getValue = function () {
            return this.items.map(function (v) { return v.getValue(); });
        };
        ArrayASTNode.prototype.addItem = function (item) {
            if (item) {
                this.items.push(item);
                return true;
            }
            return false;
        };
        ArrayASTNode.prototype.visit = function (visitor) {
            var ctn = visitor(this);
            for (var i = 0; i < this.items.length && ctn; i++) {
                ctn = this.items[i].visit(visitor);
            }
            return ctn;
        };
        ArrayASTNode.prototype.validate = function (schema, validationResult, matchingSchemas, offset) {
            var _this = this;
            if (offset === void 0) { offset = -1; }
            if (offset !== -1 && !this.contains(offset)) {
                return;
            }
            _super.prototype.validate.call(this, schema, validationResult, matchingSchemas, offset);
            if (Array.isArray(schema.items)) {
                var subSchemas = schema.items;
                subSchemas.forEach(function (subSchema, index) {
                    var itemValidationResult = new ValidationResult();
                    var item = _this.items[index];
                    if (item) {
                        item.validate(subSchema, itemValidationResult, matchingSchemas, offset);
                        validationResult.mergePropertyMatch(itemValidationResult);
                    }
                    else if (_this.items.length >= subSchemas.length) {
                        validationResult.propertiesValueMatches++;
                    }
                });
                if (schema.additionalItems === false && this.items.length > subSchemas.length) {
                    validationResult.warnings.push({
                        location: { start: this.start, end: this.end },
                        message: nls.localize(5, null, subSchemas.length)
                    });
                }
                else if (this.items.length >= subSchemas.length) {
                    validationResult.propertiesValueMatches += (this.items.length - subSchemas.length);
                }
            }
            else if (schema.items) {
                this.items.forEach(function (item) {
                    var itemValidationResult = new ValidationResult();
                    item.validate(schema.items, itemValidationResult, matchingSchemas, offset);
                    validationResult.mergePropertyMatch(itemValidationResult);
                });
            }
            if (schema.minItems && this.items.length < schema.minItems) {
                validationResult.warnings.push({
                    location: { start: this.start, end: this.end },
                    message: nls.localize(6, null, schema.minItems)
                });
            }
            if (schema.maxItems && this.items.length > schema.maxItems) {
                validationResult.warnings.push({
                    location: { start: this.start, end: this.end },
                    message: nls.localize(7, null, schema.minItems)
                });
            }
            if (schema.uniqueItems === true) {
                var values = this.items.map(function (node) {
                    return node.getValue();
                });
                var duplicates = values.some(function (value, index) {
                    return index !== values.lastIndexOf(value);
                });
                if (duplicates) {
                    validationResult.warnings.push({
                        location: { start: this.start, end: this.end },
                        message: nls.localize(8, null)
                    });
                }
            }
        };
        return ArrayASTNode;
    }(ASTNode));
    exports.ArrayASTNode = ArrayASTNode;
    var NumberASTNode = (function (_super) {
        __extends(NumberASTNode, _super);
        function NumberASTNode(parent, name, start, end) {
            _super.call(this, parent, 'number', name, start, end);
            this.isInteger = true;
            this.value = Number.NaN;
        }
        NumberASTNode.prototype.getValue = function () {
            return this.value;
        };
        NumberASTNode.prototype.validate = function (schema, validationResult, matchingSchemas, offset) {
            if (offset === void 0) { offset = -1; }
            if (offset !== -1 && !this.contains(offset)) {
                return;
            }
            // work around type validation in the base class
            var typeIsInteger = false;
            if (schema.type === 'integer' || (Array.isArray(schema.type) && Arrays.contains(schema.type, 'integer'))) {
                typeIsInteger = true;
            }
            if (typeIsInteger && this.isInteger === true) {
                this.type = 'integer';
            }
            _super.prototype.validate.call(this, schema, validationResult, matchingSchemas, offset);
            this.type = 'number';
            var val = this.getValue();
            if (Types.isNumber(schema.multipleOf)) {
                if (val % schema.multipleOf !== 0) {
                    validationResult.warnings.push({
                        location: { start: this.start, end: this.end },
                        message: nls.localize(9, null, schema.multipleOf)
                    });
                }
            }
            if (!Types.isUndefined(schema.minimum)) {
                if (schema.exclusiveMinimum && val <= schema.minimum) {
                    validationResult.warnings.push({
                        location: { start: this.start, end: this.end },
                        message: nls.localize(10, null, schema.minimum)
                    });
                }
                if (!schema.exclusiveMinimum && val < schema.minimum) {
                    validationResult.warnings.push({
                        location: { start: this.start, end: this.end },
                        message: nls.localize(11, null, schema.minimum)
                    });
                }
            }
            if (!Types.isUndefined(schema.maximum)) {
                if (schema.exclusiveMaximum && val >= schema.maximum) {
                    validationResult.warnings.push({
                        location: { start: this.start, end: this.end },
                        message: nls.localize(12, null, schema.maximum)
                    });
                }
                if (!schema.exclusiveMaximum && val > schema.maximum) {
                    validationResult.warnings.push({
                        location: { start: this.start, end: this.end },
                        message: nls.localize(13, null, schema.maximum)
                    });
                }
            }
        };
        return NumberASTNode;
    }(ASTNode));
    exports.NumberASTNode = NumberASTNode;
    var StringASTNode = (function (_super) {
        __extends(StringASTNode, _super);
        function StringASTNode(parent, name, isKey, start, end) {
            _super.call(this, parent, 'string', name, start, end);
            this.isKey = isKey;
            this.value = '';
        }
        StringASTNode.prototype.getValue = function () {
            return this.value;
        };
        StringASTNode.prototype.validate = function (schema, validationResult, matchingSchemas, offset) {
            if (offset === void 0) { offset = -1; }
            if (offset !== -1 && !this.contains(offset)) {
                return;
            }
            _super.prototype.validate.call(this, schema, validationResult, matchingSchemas, offset);
            if (schema.minLength && this.value.length < schema.minLength) {
                validationResult.warnings.push({
                    location: { start: this.start, end: this.end },
                    message: nls.localize(14, null, schema.minLength)
                });
            }
            if (schema.maxLength && this.value.length > schema.maxLength) {
                validationResult.warnings.push({
                    location: { start: this.start, end: this.end },
                    message: nls.localize(15, null, schema.maxLength)
                });
            }
            if (schema.pattern) {
                var regex = new RegExp(schema.pattern);
                if (!regex.test(this.value)) {
                    validationResult.warnings.push({
                        location: { start: this.start, end: this.end },
                        message: schema.errorMessage || nls.localize(16, null, schema.pattern)
                    });
                }
            }
        };
        return StringASTNode;
    }(ASTNode));
    exports.StringASTNode = StringASTNode;
    var PropertyASTNode = (function (_super) {
        __extends(PropertyASTNode, _super);
        function PropertyASTNode(parent, key) {
            _super.call(this, parent, 'property', null, key.start);
            this.key = key;
            key.parent = this;
            key.name = key.value;
            this.colonOffset = -1;
        }
        PropertyASTNode.prototype.getChildNodes = function () {
            return this.value ? [this.key, this.value] : [this.key];
        };
        PropertyASTNode.prototype.setValue = function (value) {
            this.value = value;
            return value !== null;
        };
        PropertyASTNode.prototype.visit = function (visitor) {
            return visitor(this) && this.key.visit(visitor) && this.value && this.value.visit(visitor);
        };
        PropertyASTNode.prototype.validate = function (schema, validationResult, matchingSchemas, offset) {
            if (offset === void 0) { offset = -1; }
            if (offset !== -1 && !this.contains(offset)) {
                return;
            }
            if (this.value) {
                this.value.validate(schema, validationResult, matchingSchemas, offset);
            }
        };
        return PropertyASTNode;
    }(ASTNode));
    exports.PropertyASTNode = PropertyASTNode;
    var ObjectASTNode = (function (_super) {
        __extends(ObjectASTNode, _super);
        function ObjectASTNode(parent, name, start, end) {
            _super.call(this, parent, 'object', name, start, end);
            this.properties = [];
        }
        ObjectASTNode.prototype.getChildNodes = function () {
            return this.properties;
        };
        ObjectASTNode.prototype.addProperty = function (node) {
            if (!node) {
                return false;
            }
            this.properties.push(node);
            return true;
        };
        ObjectASTNode.prototype.getFirstProperty = function (key) {
            for (var i = 0; i < this.properties.length; i++) {
                if (this.properties[i].key.value === key) {
                    return this.properties[i];
                }
            }
            return null;
        };
        ObjectASTNode.prototype.getKeyList = function () {
            return this.properties.map(function (p) { return p.key.getValue(); });
        };
        ObjectASTNode.prototype.getValue = function () {
            var value = {};
            this.properties.forEach(function (p) {
                var v = p.value && p.value.getValue();
                if (v) {
                    value[p.key.getValue()] = v;
                }
            });
            return value;
        };
        ObjectASTNode.prototype.visit = function (visitor) {
            var ctn = visitor(this);
            for (var i = 0; i < this.properties.length && ctn; i++) {
                ctn = this.properties[i].visit(visitor);
            }
            return ctn;
        };
        ObjectASTNode.prototype.validate = function (schema, validationResult, matchingSchemas, offset) {
            var _this = this;
            if (offset === void 0) { offset = -1; }
            if (offset !== -1 && !this.contains(offset)) {
                return;
            }
            _super.prototype.validate.call(this, schema, validationResult, matchingSchemas, offset);
            var seenKeys = {};
            var unprocessedProperties = [];
            this.properties.forEach(function (node) {
                var key = node.key.value;
                seenKeys[key] = node.value;
                unprocessedProperties.push(key);
            });
            if (Array.isArray(schema.required)) {
                schema.required.forEach(function (propertyName) {
                    if (!seenKeys[propertyName]) {
                        var key = _this.parent && _this.parent && _this.parent.key;
                        var location = key ? { start: key.start, end: key.end } : { start: _this.start, end: _this.start + 1 };
                        validationResult.warnings.push({
                            location: location,
                            message: nls.localize(17, null, propertyName)
                        });
                    }
                });
            }
            var propertyProcessed = function (prop) {
                var index = unprocessedProperties.indexOf(prop);
                while (index >= 0) {
                    unprocessedProperties.splice(index, 1);
                    index = unprocessedProperties.indexOf(prop);
                }
            };
            if (schema.properties) {
                Object.keys(schema.properties).forEach(function (propertyName) {
                    propertyProcessed(propertyName);
                    var prop = schema.properties[propertyName];
                    var child = seenKeys[propertyName];
                    if (child) {
                        var propertyvalidationResult = new ValidationResult();
                        child.validate(prop, propertyvalidationResult, matchingSchemas, offset);
                        validationResult.mergePropertyMatch(propertyvalidationResult);
                    }
                });
            }
            if (schema.patternProperties) {
                Object.keys(schema.patternProperties).forEach(function (propertyPattern) {
                    var regex = new RegExp(propertyPattern);
                    unprocessedProperties.slice(0).forEach(function (propertyName) {
                        if (regex.test(propertyName)) {
                            propertyProcessed(propertyName);
                            var child = seenKeys[propertyName];
                            if (child) {
                                var propertyvalidationResult = new ValidationResult();
                                child.validate(schema.patternProperties[propertyPattern], propertyvalidationResult, matchingSchemas, offset);
                                validationResult.mergePropertyMatch(propertyvalidationResult);
                            }
                        }
                    });
                });
            }
            if (Types.isObject(schema.additionalProperties)) {
                unprocessedProperties.forEach(function (propertyName) {
                    var child = seenKeys[propertyName];
                    if (child) {
                        var propertyvalidationResult = new ValidationResult();
                        child.validate(schema.additionalProperties, propertyvalidationResult, matchingSchemas, offset);
                        validationResult.mergePropertyMatch(propertyvalidationResult);
                    }
                });
            }
            else if (schema.additionalProperties === false) {
                if (unprocessedProperties.length > 0) {
                    unprocessedProperties.forEach(function (propertyName) {
                        var child = seenKeys[propertyName];
                        if (child) {
                            var propertyNode = child.parent;
                            validationResult.warnings.push({
                                location: { start: propertyNode.key.start, end: propertyNode.key.end },
                                message: nls.localize(18, null, propertyName)
                            });
                        }
                    });
                }
            }
            if (schema.maxProperties) {
                if (this.properties.length > schema.maxProperties) {
                    validationResult.warnings.push({
                        location: { start: this.start, end: this.end },
                        message: nls.localize(19, null, schema.maxProperties)
                    });
                }
            }
            if (schema.minProperties) {
                if (this.properties.length < schema.minProperties) {
                    validationResult.warnings.push({
                        location: { start: this.start, end: this.end },
                        message: nls.localize(20, null, schema.minProperties)
                    });
                }
            }
            if (Types.isObject(schema.dependencies)) {
                Object.keys(schema.dependencies).forEach(function (key) {
                    var prop = seenKeys[key];
                    if (prop) {
                        if (Array.isArray(schema.dependencies[key])) {
                            var valueAsArray = schema.dependencies[key];
                            valueAsArray.forEach(function (requiredProp) {
                                if (!seenKeys[requiredProp]) {
                                    validationResult.warnings.push({
                                        location: { start: _this.start, end: _this.end },
                                        message: nls.localize(21, null, requiredProp, key)
                                    });
                                }
                                else {
                                    validationResult.propertiesValueMatches++;
                                }
                            });
                        }
                        else if (Types.isObject(schema.dependencies[key])) {
                            var valueAsSchema = schema.dependencies[key];
                            var propertyvalidationResult = new ValidationResult();
                            _this.validate(valueAsSchema, propertyvalidationResult, matchingSchemas, offset);
                            validationResult.mergePropertyMatch(propertyvalidationResult);
                        }
                    }
                });
            }
        };
        return ObjectASTNode;
    }(ASTNode));
    exports.ObjectASTNode = ObjectASTNode;
    var JSONDocumentConfig = (function () {
        function JSONDocumentConfig() {
            this.ignoreDanglingComma = false;
        }
        return JSONDocumentConfig;
    }());
    exports.JSONDocumentConfig = JSONDocumentConfig;
    var ValidationResult = (function () {
        function ValidationResult() {
            this.errors = [];
            this.warnings = [];
            this.propertiesMatches = 0;
            this.propertiesValueMatches = 0;
            this.enumValueMatch = false;
        }
        ValidationResult.prototype.hasErrors = function () {
            return !!this.errors.length || !!this.warnings.length;
        };
        ValidationResult.prototype.mergeAll = function (validationResults) {
            var _this = this;
            validationResults.forEach(function (validationResult) {
                _this.merge(validationResult);
            });
        };
        ValidationResult.prototype.merge = function (validationResult) {
            this.errors = this.errors.concat(validationResult.errors);
            this.warnings = this.warnings.concat(validationResult.warnings);
        };
        ValidationResult.prototype.mergePropertyMatch = function (propertyValidationResult) {
            this.merge(propertyValidationResult);
            this.propertiesMatches++;
            if (propertyValidationResult.enumValueMatch || !propertyValidationResult.hasErrors() && propertyValidationResult.propertiesMatches) {
                this.propertiesValueMatches++;
            }
        };
        ValidationResult.prototype.compare = function (other) {
            var hasErrors = this.hasErrors();
            if (hasErrors !== other.hasErrors()) {
                return hasErrors ? -1 : 1;
            }
            if (this.enumValueMatch !== other.enumValueMatch) {
                return other.enumValueMatch ? -1 : 1;
            }
            if (this.propertiesValueMatches !== other.propertiesValueMatches) {
                return this.propertiesValueMatches - other.propertiesValueMatches;
            }
            return this.propertiesMatches - other.propertiesMatches;
        };
        return ValidationResult;
    }());
    exports.ValidationResult = ValidationResult;
    var JSONDocument = (function () {
        function JSONDocument(config) {
            this.config = config;
            this.validationResult = new ValidationResult();
        }
        Object.defineProperty(JSONDocument.prototype, "errors", {
            get: function () {
                return this.validationResult.errors;
            },
            enumerable: true,
            configurable: true
        });
        Object.defineProperty(JSONDocument.prototype, "warnings", {
            get: function () {
                return this.validationResult.warnings;
            },
            enumerable: true,
            configurable: true
        });
        JSONDocument.prototype.getNodeFromOffset = function (offset) {
            return this.root && this.root.getNodeFromOffset(offset);
        };
        JSONDocument.prototype.getNodeFromOffsetEndInclusive = function (offset) {
            return this.root && this.root.getNodeFromOffsetEndInclusive(offset);
        };
        JSONDocument.prototype.visit = function (visitor) {
            if (this.root) {
                this.root.visit(visitor);
            }
        };
        JSONDocument.prototype.validate = function (schema, matchingSchemas, offset) {
            if (matchingSchemas === void 0) { matchingSchemas = null; }
            if (offset === void 0) { offset = -1; }
            if (this.root) {
                this.root.validate(schema, this.validationResult, matchingSchemas, offset);
            }
        };
        return JSONDocument;
    }());
    exports.JSONDocument = JSONDocument;
    var JSONParser = (function () {
        function JSONParser() {
        }
        JSONParser.prototype.parse = function (text, config) {
            if (config === void 0) { config = new JSONDocumentConfig(); }
            var _doc = new JSONDocument(config);
            var _scanner = Json.createScanner(text, true);
            function _accept(token) {
                if (_scanner.getToken() === token) {
                    _scanner.scan();
                    return true;
                }
                return false;
            }
            function _error(message, node, skipUntilAfter, skipUntil) {
                if (node === void 0) { node = null; }
                if (skipUntilAfter === void 0) { skipUntilAfter = []; }
                if (skipUntil === void 0) { skipUntil = []; }
                if (_doc.errors.length === 0 || _doc.errors[0].location.start !== _scanner.getTokenOffset()) {
                    // ignore multiple errors on the same offset
                    var error = { message: message, location: { start: _scanner.getTokenOffset(), end: _scanner.getTokenOffset() + _scanner.getTokenLength() } };
                    _doc.errors.push(error);
                }
                if (node) {
                    _finalize(node, false);
                }
                if (skipUntilAfter.length + skipUntil.length > 0) {
                    var token = _scanner.getToken();
                    while (token !== Json.SyntaxKind.EOF) {
                        if (skipUntilAfter.indexOf(token) !== -1) {
                            _scanner.scan();
                            break;
                        }
                        else if (skipUntil.indexOf(token) !== -1) {
                            break;
                        }
                        token = _scanner.scan();
                    }
                }
                return node;
            }
            function _checkScanError() {
                switch (_scanner.getTokenError()) {
                    case Json.ScanError.InvalidUnicode:
                        _error(nls.localize(22, null));
                        return true;
                    case Json.ScanError.InvalidEscapeCharacter:
                        _error(nls.localize(23, null));
                        return true;
                    case Json.ScanError.UnexpectedEndOfNumber:
                        _error(nls.localize(24, null));
                        return true;
                    case Json.ScanError.UnexpectedEndOfComment:
                        _error(nls.localize(25, null));
                        return true;
                    case Json.ScanError.UnexpectedEndOfString:
                        _error(nls.localize(26, null));
                        return true;
                }
                return false;
            }
            function _finalize(node, scanNext) {
                node.end = _scanner.getTokenOffset() + _scanner.getTokenLength();
                if (scanNext) {
                    _scanner.scan();
                }
                return node;
            }
            function _parseArray(parent, name) {
                if (_scanner.getToken() !== Json.SyntaxKind.OpenBracketToken) {
                    return null;
                }
                var node = new ArrayASTNode(parent, name, _scanner.getTokenOffset());
                _scanner.scan(); // consume OpenBracketToken
                var count = 0;
                if (node.addItem(_parseValue(node, '' + count++))) {
                    while (_accept(Json.SyntaxKind.CommaToken)) {
                        if (!node.addItem(_parseValue(node, '' + count++)) && !_doc.config.ignoreDanglingComma) {
                            _error(nls.localize(27, null));
                        }
                    }
                }
                if (_scanner.getToken() !== Json.SyntaxKind.CloseBracketToken) {
                    return _error(nls.localize(28, null), node);
                }
                return _finalize(node, true);
            }
            function _parseProperty(parent, keysSeen) {
                var key = _parseString(null, null, true);
                if (!key) {
                    if (_scanner.getToken() === Json.SyntaxKind.Unknown) {
                        // give a more helpful error message
                        var value = _scanner.getTokenValue();
                        if (value.match(/^['\w]/)) {
                            _error(nls.localize(29, null));
                        }
                    }
                    return null;
                }
                var node = new PropertyASTNode(parent, key);
                if (keysSeen[key.value]) {
                    _doc.warnings.push({ location: { start: node.key.start, end: node.key.end }, message: nls.localize(30, null) });
                }
                keysSeen[key.value] = true;
                if (_scanner.getToken() === Json.SyntaxKind.ColonToken) {
                    node.colonOffset = _scanner.getTokenOffset();
                }
                else {
                    return _error(nls.localize(31, null), node, [], [Json.SyntaxKind.CloseBraceToken, Json.SyntaxKind.CommaToken]);
                }
                _scanner.scan(); // consume ColonToken
                if (!node.setValue(_parseValue(node, key.value))) {
                    return _error(nls.localize(32, null), node, [], [Json.SyntaxKind.CloseBraceToken, Json.SyntaxKind.CommaToken]);
                }
                node.end = node.value.end;
                return node;
            }
            function _parseObject(parent, name) {
                if (_scanner.getToken() !== Json.SyntaxKind.OpenBraceToken) {
                    return null;
                }
                var node = new ObjectASTNode(parent, name, _scanner.getTokenOffset());
                _scanner.scan(); // consume OpenBraceToken
                var keysSeen = {};
                if (node.addProperty(_parseProperty(node, keysSeen))) {
                    while (_accept(Json.SyntaxKind.CommaToken)) {
                        if (!node.addProperty(_parseProperty(node, keysSeen)) && !_doc.config.ignoreDanglingComma) {
                            _error(nls.localize(33, null));
                        }
                    }
                }
                if (_scanner.getToken() !== Json.SyntaxKind.CloseBraceToken) {
                    return _error(nls.localize(34, null), node);
                }
                return _finalize(node, true);
            }
            function _parseString(parent, name, isKey) {
                if (_scanner.getToken() !== Json.SyntaxKind.StringLiteral) {
                    return null;
                }
                var node = new StringASTNode(parent, name, isKey, _scanner.getTokenOffset());
                node.value = _scanner.getTokenValue();
                _checkScanError();
                return _finalize(node, true);
            }
            function _parseNumber(parent, name) {
                if (_scanner.getToken() !== Json.SyntaxKind.NumericLiteral) {
                    return null;
                }
                var node = new NumberASTNode(parent, name, _scanner.getTokenOffset());
                if (!_checkScanError()) {
                    var tokenValue = _scanner.getTokenValue();
                    try {
                        var numberValue = JSON.parse(tokenValue);
                        if (typeof numberValue !== 'number') {
                            return _error(nls.localize(35, null), node);
                        }
                        node.value = numberValue;
                    }
                    catch (e) {
                        return _error(nls.localize(36, null), node);
                    }
                    node.isInteger = tokenValue.indexOf('.') === -1;
                }
                return _finalize(node, true);
            }
            function _parseLiteral(parent, name) {
                var node;
                switch (_scanner.getToken()) {
                    case Json.SyntaxKind.NullKeyword:
                        node = new NullASTNode(parent, name, _scanner.getTokenOffset());
                        break;
                    case Json.SyntaxKind.TrueKeyword:
                        node = new BooleanASTNode(parent, name, true, _scanner.getTokenOffset());
                        break;
                    case Json.SyntaxKind.FalseKeyword:
                        node = new BooleanASTNode(parent, name, false, _scanner.getTokenOffset());
                        break;
                    default:
                        return null;
                }
                return _finalize(node, true);
            }
            function _parseValue(parent, name) {
                return _parseArray(parent, name) || _parseObject(parent, name) || _parseString(parent, name, false) || _parseNumber(parent, name) || _parseLiteral(parent, name);
            }
            _scanner.scan();
            _doc.root = _parseValue(null, null);
            if (!_doc.root) {
                _error(nls.localize(37, null));
            }
            else if (_scanner.getToken() !== Json.SyntaxKind.EOF) {
                _error(nls.localize(38, null));
            }
            return _doc;
        };
        return JSONParser;
    }());
    exports.JSONParser = JSONParser;
});

define(__m[21], __M([1,0,8,6,9,15,23]), function (require, exports, Parser, Types, WinJS, nls, errors) {
    /*---------------------------------------------------------------------------------------------
     *  Copyright (c) Microsoft Corporation. All rights reserved.
     *  Licensed under the MIT License. See License.txt in the project root for license information.
     *--------------------------------------------------------------------------------------------*/
    'use strict';
    var JSONIntellisense = (function () {
        function JSONIntellisense(schemaService, requestService, contributions) {
            this.schemaService = schemaService;
            this.requestService = requestService;
            this.contributions = contributions;
        }
        JSONIntellisense.prototype.doSuggest = function (resource, modelMirror, position) {
            var _this = this;
            var currentWord = modelMirror.getWordUntilPosition(position).word;
            var parser = new Parser.JSONParser();
            var config = new Parser.JSONDocumentConfig();
            // so you can invoke suggest after the comma in an object literal
            config.ignoreDanglingComma = true;
            var doc = parser.parse(modelMirror.getValue(), config);
            var result = {
                currentWord: currentWord,
                incomplete: false,
                suggestions: []
            };
            var overwriteBefore = void 0;
            var overwriteAfter = void 0;
            var proposed = {};
            var collector = {
                add: function (suggestion) {
                    if (!proposed[suggestion.label]) {
                        proposed[suggestion.label] = true;
                        suggestion.overwriteBefore = overwriteBefore;
                        suggestion.overwriteAfter = overwriteAfter;
                        result.suggestions.push(suggestion);
                    }
                },
                setAsIncomplete: function () {
                    result.incomplete = true;
                },
                error: function (message) {
                    errors.onUnexpectedError(message);
                }
            };
            return this.schemaService.getSchemaForResource(resource.toString(), doc).then(function (schema) {
                var collectionPromises = [];
                var offset = modelMirror.getOffsetFromPosition(position);
                var node = doc.getNodeFromOffsetEndInclusive(offset);
                var addValue = true;
                var currentKey = currentWord;
                var currentProperty = null;
                if (node) {
                    if (node.type === 'string') {
                        var stringNode = node;
                        if (stringNode.isKey) {
                            var nodeRange = modelMirror.getRangeFromOffsetAndLength(node.start, node.end - node.start);
                            overwriteBefore = position.column - nodeRange.startColumn;
                            overwriteAfter = nodeRange.endColumn - position.column;
                            addValue = !(node.parent && (node.parent.value));
                            currentProperty = node.parent ? node.parent : null;
                            currentKey = modelMirror.getValueInRange({ startColumn: nodeRange.startColumn + 1, startLineNumber: nodeRange.startLineNumber, endColumn: position.column, endLineNumber: position.lineNumber });
                            if (node.parent) {
                                node = node.parent.parent;
                            }
                        }
                    }
                }
                // proposals for properties
                if (node && node.type === 'object') {
                    // don't suggest keys when the cursor is just before the opening curly brace
                    if (node.start === offset) {
                        return result;
                    }
                    // don't suggest properties that are already present
                    var properties = node.properties;
                    properties.forEach(function (p) {
                        if (!currentProperty || currentProperty !== p) {
                            proposed[p.key.value] = true;
                        }
                    });
                    if (schema) {
                        // property proposals with schema
                        var isLast = properties.length === 0 || offset >= properties[properties.length - 1].start;
                        _this.getPropertySuggestions(resource, schema, doc, node, currentKey, addValue, isLast, collector);
                    }
                    else if (node.parent) {
                        // property proposals without schema
                        _this.getSchemaLessPropertySuggestions(doc, node, collector);
                    }
                    var location = node.getNodeLocation();
                    _this.contributions.forEach(function (contribution) {
                        var collectPromise = contribution.collectPropertySuggestions(resource, location, currentWord, addValue, isLast, collector);
                        if (collectPromise) {
                            collectionPromises.push(collectPromise);
                        }
                    });
                }
                // proposals for values
                if (node && (node.type === 'string' || node.type === 'number' || node.type === 'integer' || node.type === 'boolean' || node.type === 'null')) {
                    var nodeRange = modelMirror.getRangeFromOffsetAndLength(node.start, node.end - node.start);
                    overwriteBefore = position.column - nodeRange.startColumn;
                    overwriteAfter = nodeRange.endColumn - position.column;
                    node = node.parent;
                }
                if (schema) {
                    // value proposals with schema
                    _this.getValueSuggestions(resource, schema, doc, node, offset, collector);
                }
                else {
                    // value proposals without schema
                    _this.getSchemaLessValueSuggestions(doc, node, offset, modelMirror, collector);
                }
                if (!node) {
                    _this.contributions.forEach(function (contribution) {
                        var collectPromise = contribution.collectDefaultSuggestions(resource, collector);
                        if (collectPromise) {
                            collectionPromises.push(collectPromise);
                        }
                    });
                }
                else {
                    if ((node.type === 'property') && offset > node.colonOffset) {
                        var parentKey = node.key.value;
                        var valueNode = node.value;
                        if (!valueNode || offset <= valueNode.end) {
                            var location = node.parent.getNodeLocation();
                            _this.contributions.forEach(function (contribution) {
                                var collectPromise = contribution.collectValueSuggestions(resource, location, parentKey, collector);
                                if (collectPromise) {
                                    collectionPromises.push(collectPromise);
                                }
                            });
                        }
                    }
                }
                return WinJS.Promise.join(collectionPromises).then(function () { return result; });
            });
        };
        JSONIntellisense.prototype.getPropertySuggestions = function (resource, schema, doc, node, currentWord, addValue, isLast, collector) {
            var _this = this;
            var matchingSchemas = [];
            doc.validate(schema.schema, matchingSchemas, node.start);
            matchingSchemas.forEach(function (s) {
                if (s.node === node && !s.inverted) {
                    var schemaProperties = s.schema.properties;
                    if (schemaProperties) {
                        Object.keys(schemaProperties).forEach(function (key) {
                            var propertySchema = schemaProperties[key];
                            collector.add({ type: 'property', label: key, codeSnippet: _this.getTextForProperty(key, propertySchema, addValue, isLast), documentationLabel: propertySchema.description || '' });
                        });
                    }
                }
            });
        };
        JSONIntellisense.prototype.getSchemaLessPropertySuggestions = function (doc, node, collector) {
            var _this = this;
            var collectSuggestionsForSimilarObject = function (obj) {
                obj.properties.forEach(function (p) {
                    var key = p.key.value;
                    collector.add({ type: 'property', label: key, codeSnippet: _this.getTextForSimilarProperty(key, p.value), documentationLabel: '' });
                });
            };
            if (node.parent.type === 'property') {
                // if the object is a property value, check the tree for other objects that hang under a property of the same name
                var parentKey = node.parent.key.value;
                doc.visit(function (n) {
                    if (n.type === 'property' && n.key.value === parentKey && n.value && n.value.type === 'object') {
                        collectSuggestionsForSimilarObject(n.value);
                    }
                    return true;
                });
            }
            else if (node.parent.type === 'array') {
                // if the object is in an array, use all other array elements as similar objects
                node.parent.items.forEach(function (n) {
                    if (n.type === 'object' && n !== node) {
                        collectSuggestionsForSimilarObject(n);
                    }
                });
            }
        };
        JSONIntellisense.prototype.getSchemaLessValueSuggestions = function (doc, node, offset, modelMirror, collector) {
            var _this = this;
            var collectSuggestionsForValues = function (value) {
                var content = _this.getTextForMatchingNode(value, modelMirror);
                collector.add({ type: _this.getSuggestionType(value.type), label: content, codeSnippet: content, documentationLabel: '' });
                if (value.type === 'boolean') {
                    _this.addBooleanSuggestion(!value.getValue(), collector);
                }
            };
            if (!node) {
                collector.add({ type: this.getSuggestionType('object'), label: 'Empty object', codeSnippet: '{\n\t{{}}\n}', documentationLabel: '' });
                collector.add({ type: this.getSuggestionType('array'), label: 'Empty array', codeSnippet: '[\n\t{{}}\n]', documentationLabel: '' });
            }
            else {
                if (node.type === 'property' && offset > node.colonOffset) {
                    var valueNode = node.value;
                    if (valueNode && offset > valueNode.end) {
                        return;
                    }
                    // suggest values at the same key
                    var parentKey = node.key.value;
                    doc.visit(function (n) {
                        if (n.type === 'property' && n.key.value === parentKey && n.value) {
                            collectSuggestionsForValues(n.value);
                        }
                        return true;
                    });
                }
                if (node.type === 'array') {
                    if (node.parent && node.parent.type === 'property') {
                        // suggest items of an array at the same key
                        var parentKey = node.parent.key.value;
                        doc.visit(function (n) {
                            if (n.type === 'property' && n.key.value === parentKey && n.value && n.value.type === 'array') {
                                (n.value.items).forEach(function (n) {
                                    collectSuggestionsForValues(n);
                                });
                            }
                            return true;
                        });
                    }
                    else {
                        // suggest items in the same array
                        node.items.forEach(function (n) {
                            collectSuggestionsForValues(n);
                        });
                    }
                }
            }
        };
        JSONIntellisense.prototype.getValueSuggestions = function (resource, schema, doc, node, offset, collector) {
            var _this = this;
            if (!node) {
                this.addDefaultSuggestion(schema.schema, collector);
            }
            else {
                var parentKey = null;
                if (node && (node.type === 'property') && offset > node.colonOffset) {
                    var valueNode = node.value;
                    if (valueNode && offset > valueNode.end) {
                        return; // we are past the value node
                    }
                    parentKey = node.key.value;
                    node = node.parent;
                }
                if (node && (parentKey !== null || node.type === 'array')) {
                    var matchingSchemas = [];
                    doc.validate(schema.schema, matchingSchemas, node.start);
                    matchingSchemas.forEach(function (s) {
                        if (s.node === node && !s.inverted && s.schema) {
                            if (s.schema.items) {
                                _this.addDefaultSuggestion(s.schema.items, collector);
                                _this.addEnumSuggestion(s.schema.items, collector);
                            }
                            if (s.schema.properties) {
                                var propertySchema = s.schema.properties[parentKey];
                                if (propertySchema) {
                                    _this.addDefaultSuggestion(propertySchema, collector);
                                    _this.addEnumSuggestion(propertySchema, collector);
                                }
                            }
                        }
                    });
                }
            }
        };
        JSONIntellisense.prototype.addBooleanSuggestion = function (value, collector) {
            collector.add({ type: this.getSuggestionType('boolean'), label: value ? 'true' : 'false', codeSnippet: this.getTextForEnumValue(value), documentationLabel: '' });
        };
        JSONIntellisense.prototype.addEnumSuggestion = function (schema, collector) {
            var _this = this;
            if (Array.isArray(schema.enum)) {
                schema.enum.forEach(function (enm) { return collector.add({ type: _this.getSuggestionType(schema.type), label: _this.getLabelForValue(enm), codeSnippet: _this.getTextForEnumValue(enm), documentationLabel: '' }); });
            }
            else if (schema.type === 'boolean') {
                this.addBooleanSuggestion(true, collector);
                this.addBooleanSuggestion(false, collector);
            }
            if (Array.isArray(schema.allOf)) {
                schema.allOf.forEach(function (s) { return _this.addEnumSuggestion(s, collector); });
            }
            if (Array.isArray(schema.anyOf)) {
                schema.anyOf.forEach(function (s) { return _this.addEnumSuggestion(s, collector); });
            }
            if (Array.isArray(schema.oneOf)) {
                schema.oneOf.forEach(function (s) { return _this.addEnumSuggestion(s, collector); });
            }
        };
        JSONIntellisense.prototype.addDefaultSuggestion = function (schema, collector) {
            var _this = this;
            if (schema.default) {
                collector.add({
                    type: this.getSuggestionType(schema.type),
                    label: this.getLabelForValue(schema.default),
                    codeSnippet: this.getTextForValue(schema.default),
                    typeLabel: nls.localize(0, null),
                });
            }
            if (Array.isArray(schema.defaultSnippets)) {
                schema.defaultSnippets.forEach(function (s) {
                    collector.add({
                        type: 'snippet',
                        label: _this.getLabelForSnippetValue(s.body),
                        codeSnippet: _this.getTextForSnippetValue(s.body)
                    });
                });
            }
            if (Array.isArray(schema.allOf)) {
                schema.allOf.forEach(function (s) { return _this.addDefaultSuggestion(s, collector); });
            }
            if (Array.isArray(schema.anyOf)) {
                schema.anyOf.forEach(function (s) { return _this.addDefaultSuggestion(s, collector); });
            }
            if (Array.isArray(schema.oneOf)) {
                schema.oneOf.forEach(function (s) { return _this.addDefaultSuggestion(s, collector); });
            }
        };
        JSONIntellisense.prototype.getLabelForValue = function (value) {
            var label = JSON.stringify(value);
            label = label.replace('{{', '').replace('}}', '');
            if (label.length > 57) {
                return label.substr(0, 57).trim() + '...';
            }
            return label;
        };
        JSONIntellisense.prototype.getLabelForSnippetValue = function (value) {
            var label = JSON.stringify(value);
            label = label.replace(/\{\{|\}\}/g, '');
            if (label.length > 57) {
                return label.substr(0, 57).trim() + '...';
            }
            return label;
        };
        JSONIntellisense.prototype.getTextForValue = function (value) {
            var text = JSON.stringify(value, null, '\t');
            text = text.replace(/[\\\{\}]/g, '\\$&');
            return text;
        };
        JSONIntellisense.prototype.getTextForSnippetValue = function (value) {
            return JSON.stringify(value, null, '\t');
        };
        JSONIntellisense.prototype.getTextForEnumValue = function (value) {
            var snippet = JSON.stringify(value, null, '\t');
            switch (typeof value) {
                case 'object':
                    if (value === null) {
                        return '{{null}}';
                    }
                    return snippet;
                case 'string':
                    return '"{{' + snippet.substr(1, snippet.length - 2) + '}}"';
                case 'number':
                case 'integer':
                case 'boolean':
                    return '{{' + snippet + '}}';
            }
            return snippet;
        };
        JSONIntellisense.prototype.getSuggestionType = function (type) {
            if (Array.isArray(type)) {
                var array = type;
                type = array.length > 0 ? array[0] : null;
            }
            if (!type) {
                return 'text';
            }
            switch (type) {
                case 'string': return 'text';
                case 'object': return 'module';
                case 'property': return 'property';
                default: return 'value';
            }
        };
        JSONIntellisense.prototype.getTextForMatchingNode = function (node, modelMirror) {
            switch (node.type) {
                case 'array':
                    return '[]';
                case 'object':
                    return '{}';
                default:
                    var content = modelMirror.getValueInRange(modelMirror.getRangeFromOffsetAndLength(node.start, node.end - node.start));
                    return content;
            }
        };
        JSONIntellisense.prototype.getTextForProperty = function (key, propertySchema, addValue, isLast) {
            var result = this.getTextForValue(key);
            if (!addValue) {
                return result;
            }
            result += ': ';
            var defaultVal = propertySchema.default;
            if (!Types.isUndefined(defaultVal)) {
                result = result + this.getTextForEnumValue(defaultVal);
            }
            else if (propertySchema.enum && propertySchema.enum.length > 0) {
                result = result + this.getTextForEnumValue(propertySchema.enum[0]);
            }
            else {
                switch (propertySchema.type) {
                    case 'boolean':
                        result += '{{false}}';
                        break;
                    case 'string':
                        result += '"{{}}"';
                        break;
                    case 'object':
                        result += '{\n\t{{}}\n}';
                        break;
                    case 'array':
                        result += '[\n\t{{}}\n]';
                        break;
                    case 'number':
                    case 'integer':
                        result += '{{0}}';
                        break;
                    case 'null':
                        result += '{{null}}';
                        break;
                    default:
                        return result;
                }
            }
            if (!isLast) {
                result += ',';
            }
            return result;
        };
        JSONIntellisense.prototype.getTextForSimilarProperty = function (key, templateValue) {
            return this.getTextForValue(key);
        };
        return JSONIntellisense;
    }());
    exports.JSONIntellisense = JSONIntellisense;
});

var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
var __param = (this && this.__param) || function (paramIndex, decorator) {
    return function (target, key) { decorator(target, key, paramIndex); }
};
define(__m[24], __M([1,0,4,25,5]), function (require, exports, Strings, nls, request_1) {
    /*---------------------------------------------------------------------------------------------
     *  Copyright (c) Microsoft Corporation. All rights reserved.
     *  Licensed under the MIT License. See License.txt in the project root for license information.
     *--------------------------------------------------------------------------------------------*/
    'use strict';
    var BowerJSONContribution = (function () {
        function BowerJSONContribution(requestService) {
            this.topRanked = ['twitter', 'bootstrap', 'angular-1.1.6', 'angular-latest', 'angulerjs', 'd3', 'myjquery', 'jq', 'abcdef1234567890', 'jQuery', 'jquery-1.11.1', 'jquery',
                'sushi-vanilla-x-data', 'font-awsome', 'Font-Awesome', 'font-awesome', 'fontawesome', 'html5-boilerplate', 'impress.js', 'homebrew',
                'backbone', 'moment1', 'momentjs', 'moment', 'linux', 'animate.css', 'animate-css', 'reveal.js', 'jquery-file-upload', 'blueimp-file-upload', 'threejs', 'express', 'chosen',
                'normalize-css', 'normalize.css', 'semantic', 'semantic-ui', 'Semantic-UI', 'modernizr', 'underscore', 'underscore1',
                'material-design-icons', 'ionic', 'chartjs', 'Chart.js', 'nnnick-chartjs', 'select2-ng', 'select2-dist', 'phantom', 'skrollr', 'scrollr', 'less.js', 'leancss', 'parser-lib',
                'hui', 'bootstrap-languages', 'async', 'gulp', 'jquery-pjax', 'coffeescript', 'hammer.js', 'ace', 'leaflet', 'jquery-mobile', 'sweetalert', 'typeahead.js', 'soup', 'typehead.js',
                'sails', 'codeigniter2'];
            this.requestService = requestService;
        }
        BowerJSONContribution.prototype.isBowerFile = function (resource) {
            var path = resource.path;
            return Strings.endsWith(path, '/bower.json') || Strings.endsWith(path, '/.bower.json');
        };
        BowerJSONContribution.prototype.collectDefaultSuggestions = function (resource, result) {
            if (this.isBowerFile(resource)) {
                var defaultValue = {
                    'name': '{{name}}',
                    'description': '{{description}}',
                    'authors': ['{{author}}'],
                    'version': '{{1.0.0}}',
                    'main': '{{pathToMain}}',
                    'dependencies': {}
                };
                result.add({ type: 'snippet', label: nls.localize(0, null), codeSnippet: JSON.stringify(defaultValue, null, '\t'), documentationLabel: '' });
            }
            return null;
        };
        BowerJSONContribution.prototype.collectPropertySuggestions = function (resource, location, currentWord, addValue, isLast, result) {
            if (this.isBowerFile(resource) && (location.matches(['dependencies']) || location.matches(['devDependencies']))) {
                if (currentWord.length > 0) {
                    var queryUrl = 'https://bower.herokuapp.com/packages/search/' + encodeURIComponent(currentWord);
                    return this.requestService.makeRequest({
                        url: queryUrl
                    }).then(function (success) {
                        if (success.status === 200) {
                            try {
                                var obj = JSON.parse(success.responseText);
                                if (Array.isArray(obj)) {
                                    var results = obj;
                                    for (var i = 0; i < results.length; i++) {
                                        var name = results[i].name;
                                        var description = results[i].description || '';
                                        var codeSnippet = JSON.stringify(name);
                                        if (addValue) {
                                            codeSnippet += ': "{{*}}"';
                                            if (!isLast) {
                                                codeSnippet += ',';
                                            }
                                        }
                                        result.add({ type: 'property', label: name, codeSnippet: codeSnippet, documentationLabel: description });
                                    }
                                    result.setAsIncomplete();
                                }
                            }
                            catch (e) {
                            }
                        }
                        else {
                            result.error(nls.localize(1, null, success.responseText));
                            return 0;
                        }
                    }, function (error) {
                        result.error(nls.localize(2, null, error.responseText));
                        return 0;
                    });
                }
                else {
                    this.topRanked.forEach(function (name) {
                        var codeSnippet = JSON.stringify(name);
                        if (addValue) {
                            codeSnippet += ': "{{*}}"';
                            if (!isLast) {
                                codeSnippet += ',';
                            }
                        }
                        result.add({ type: 'property', label: name, codeSnippet: codeSnippet, documentationLabel: '' });
                    });
                    result.setAsIncomplete();
                }
            }
            return null;
        };
        BowerJSONContribution.prototype.collectValueSuggestions = function (resource, location, currentKey, result) {
            // not implemented. Could be do done calling the bower command. Waiting for web API: https://github.com/bower/registry/issues/26
            return null;
        };
        BowerJSONContribution.prototype.getInfoContribution = function (resource, location) {
            if (this.isBowerFile(resource) && (location.matches(['dependencies', '*']) || location.matches(['devDependencies', '*']))) {
                var pack = location.getSegments()[location.getSegments().length - 1];
                var htmlContent = [];
                htmlContent.push(nls.localize(3, null, pack));
                var queryUrl = 'https://bower.herokuapp.com/packages/' + encodeURIComponent(pack);
                return this.requestService.makeRequest({
                    url: queryUrl
                }).then(function (success) {
                    try {
                        var obj = JSON.parse(success.responseText);
                        if (obj && obj.url) {
                            var url = obj.url;
                            if (Strings.startsWith(url, 'git://')) {
                                url = url.substring(6);
                            }
                            if (Strings.endsWith(url, '.git')) {
                                url = url.substring(0, url.length - 4);
                            }
                            htmlContent.push(url);
                        }
                    }
                    catch (e) {
                    }
                    return htmlContent;
                }, function (error) {
                    return htmlContent;
                });
            }
            return null;
        };
        BowerJSONContribution = __decorate([
            __param(0, request_1.IRequestService)
        ], BowerJSONContribution);
        return BowerJSONContribution;
    }());
    exports.BowerJSONContribution = BowerJSONContribution;
});










define(__m[26], __M([1,0,4,13,5]), function (require, exports, Strings, nls, request_1) {
    /*---------------------------------------------------------------------------------------------
     *  Copyright (c) Microsoft Corporation. All rights reserved.
     *  Licensed under the MIT License. See License.txt in the project root for license information.
     *--------------------------------------------------------------------------------------------*/
    'use strict';
    var LIMIT = 40;
    var PackageJSONContribution = (function () {
        function PackageJSONContribution(requestService) {
            this.mostDependedOn = ['lodash', 'async', 'underscore', 'request', 'commander', 'express', 'debug', 'chalk', 'colors', 'q', 'coffee-script',
                'mkdirp', 'optimist', 'through2', 'yeoman-generator', 'moment', 'bluebird', 'glob', 'gulp-util', 'minimist', 'cheerio', 'jade', 'redis', 'node-uuid',
                'socket', 'io', 'uglify-js', 'winston', 'through', 'fs-extra', 'handlebars', 'body-parser', 'rimraf', 'mime', 'semver', 'mongodb', 'jquery',
                'grunt', 'connect', 'yosay', 'underscore', 'string', 'xml2js', 'ejs', 'mongoose', 'marked', 'extend', 'mocha', 'superagent', 'js-yaml', 'xtend',
                'shelljs', 'gulp', 'yargs', 'browserify', 'minimatch', 'react', 'less', 'prompt', 'inquirer', 'ws', 'event-stream', 'inherits', 'mysql', 'esprima',
                'jsdom', 'stylus', 'when', 'readable-stream', 'aws-sdk', 'concat-stream', 'chai', 'promise', 'wrench'];
            this.requestService = requestService;
        }
        PackageJSONContribution.prototype.isPackageJSONFile = function (resource) {
            var path = resource.path;
            return Strings.endsWith(path, '/package.json');
        };
        PackageJSONContribution.prototype.collectDefaultSuggestions = function (resource, result) {
            if (this.isPackageJSONFile(resource)) {
                var defaultValue = {
                    'name': '{{name}}',
                    'description': '{{description}}',
                    'author': '{{author}}',
                    'version': '{{1.0.0}}',
                    'main': '{{pathToMain}}',
                    'dependencies': {}
                };
                result.add({ type: 'module', label: nls.localize(0, null), codeSnippet: JSON.stringify(defaultValue, null, '\t'), documentationLabel: '' });
            }
            return null;
        };
        PackageJSONContribution.prototype.collectPropertySuggestions = function (resource, location, currentWord, addValue, isLast, result) {
            if (this.isPackageJSONFile(resource) && (location.matches(['dependencies']) || location.matches(['devDependencies']) || location.matches(['optionalDependencies']) || location.matches(['peerDependencies']))) {
                var queryUrl;
                if (currentWord.length > 0) {
                    queryUrl = 'https://skimdb.npmjs.com/registry/_design/app/_view/browseAll?group_level=1&limit=' + LIMIT + '&start_key=%5B%22' + encodeURIComponent(currentWord) + '%22%5D&end_key=%5B%22' + encodeURIComponent(currentWord + 'z') + '%22,%7B%7D%5D';
                    return this.requestService.makeRequest({
                        url: queryUrl
                    }).then(function (success) {
                        if (success.status === 200) {
                            try {
                                var obj = JSON.parse(success.responseText);
                                if (obj && Array.isArray(obj.rows)) {
                                    var results = obj.rows;
                                    for (var i = 0; i < results.length; i++) {
                                        var keys = results[i].key;
                                        if (Array.isArray(keys) && keys.length > 0) {
                                            var name = keys[0];
                                            var codeSnippet = JSON.stringify(name);
                                            if (addValue) {
                                                codeSnippet += ': "{{*}}"';
                                                if (!isLast) {
                                                    codeSnippet += ',';
                                                }
                                            }
                                            result.add({ type: 'property', label: name, codeSnippet: codeSnippet, documentationLabel: '' });
                                        }
                                    }
                                    if (results.length === LIMIT) {
                                        result.setAsIncomplete();
                                    }
                                }
                            }
                            catch (e) {
                            }
                        }
                        else {
                            result.error(nls.localize(1, null, success.responseText));
                            return 0;
                        }
                    }, function (error) {
                        result.error(nls.localize(2, null, error.responseText));
                        return 0;
                    });
                }
                else {
                    this.mostDependedOn.forEach(function (name) {
                        var codeSnippet = JSON.stringify(name);
                        if (addValue) {
                            codeSnippet += ': "{{*}}"';
                            if (!isLast) {
                                codeSnippet += ',';
                            }
                        }
                        result.add({ type: 'property', label: name, codeSnippet: codeSnippet, documentationLabel: '' });
                    });
                    result.setAsIncomplete();
                }
            }
            return null;
        };
        PackageJSONContribution.prototype.collectValueSuggestions = function (resource, location, currentKey, result) {
            if (this.isPackageJSONFile(resource) && (location.matches(['dependencies']) || location.matches(['devDependencies']) || location.matches(['optionalDependencies']) || location.matches(['peerDependencies']))) {
                var queryUrl = 'http://registry.npmjs.org/' + encodeURIComponent(currentKey) + '/latest';
                return this.requestService.makeRequest({
                    url: queryUrl
                }).then(function (success) {
                    try {
                        var obj = JSON.parse(success.responseText);
                        if (obj && obj.version) {
                            var version = obj.version;
                            var name = JSON.stringify(version);
                            result.add({ type: 'class', label: name, codeSnippet: name, documentationLabel: nls.localize(3, null) });
                            name = JSON.stringify('^' + version);
                            result.add({ type: 'class', label: name, codeSnippet: name, documentationLabel: nls.localize(4, null) });
                            name = JSON.stringify('~' + version);
                            result.add({ type: 'class', label: name, codeSnippet: name, documentationLabel: nls.localize(5, null) });
                        }
                    }
                    catch (e) {
                    }
                    return 0;
                }, function (error) {
                    return 0;
                });
            }
            return null;
        };
        PackageJSONContribution.prototype.getInfoContribution = function (resource, location) {
            if (this.isPackageJSONFile(resource) && (location.matches(['dependencies', '*']) || location.matches(['devDependencies', '*']) || location.matches(['optionalDependencies', '*']) || location.matches(['peerDependencies', '*']))) {
                var pack = location.getSegments()[location.getSegments().length - 1];
                var htmlContent = [];
                htmlContent.push(nls.localize(6, null, pack));
                var queryUrl = 'http://registry.npmjs.org/' + encodeURIComponent(pack) + '/latest';
                return this.requestService.makeRequest({
                    url: queryUrl
                }).then(function (success) {
                    try {
                        var obj = JSON.parse(success.responseText);
                        if (obj) {
                            if (obj.description) {
                                htmlContent.push(obj.description);
                            }
                            if (obj.version) {
                                htmlContent.push(nls.localize(7, null, obj.version));
                            }
                        }
                    }
                    catch (e) {
                    }
                    return htmlContent;
                }, function (error) {
                    return htmlContent;
                });
            }
            return null;
        };
        PackageJSONContribution = __decorate([
            __param(0, request_1.IRequestService)
        ], PackageJSONContribution);
        return PackageJSONContribution;
    }());
    exports.PackageJSONContribution = PackageJSONContribution;
});










define(__m[27], __M([1,0,4,14,5]), function (require, exports, Strings, nls, request_1) {
    /*---------------------------------------------------------------------------------------------
     *  Copyright (c) Microsoft Corporation. All rights reserved.
     *  Licensed under the MIT License. See License.txt in the project root for license information.
     *--------------------------------------------------------------------------------------------*/
    'use strict';
    var LIMIT = 40;
    var ProjectJSONContribution = (function () {
        function ProjectJSONContribution(requestService) {
            this.requestService = requestService;
        }
        ProjectJSONContribution.prototype.isProjectJSONFile = function (resource) {
            var path = resource.path;
            return Strings.endsWith(path, '/project.json');
        };
        ProjectJSONContribution.prototype.collectDefaultSuggestions = function (resource, result) {
            if (this.isProjectJSONFile(resource)) {
                var defaultValue = {
                    'version': '{{1.0.0-*}}',
                    'dependencies': {},
                    'frameworks': {
                        'dnx451': {},
                        'dnxcore50': {}
                    }
                };
                result.add({ type: 'snippet', label: nls.localize(0, null), codeSnippet: JSON.stringify(defaultValue, null, '\t'), documentationLabel: '' });
            }
            return null;
        };
        ProjectJSONContribution.prototype.collectPropertySuggestions = function (resource, location, currentWord, addValue, isLast, result) {
            if (this.isProjectJSONFile(resource) && (location.matches(['dependencies']) || location.matches(['frameworks', '*', 'dependencies']) || location.matches(['frameworks', '*', 'frameworkAssemblies']))) {
                var queryUrl;
                if (currentWord.length > 0) {
                    queryUrl = 'https://www.nuget.org/api/v2/Packages?'
                        + '$filter=Id%20ge%20\''
                        + encodeURIComponent(currentWord)
                        + '\'%20and%20Id%20lt%20\''
                        + encodeURIComponent(currentWord + 'z')
                        + '\'%20and%20IsAbsoluteLatestVersion%20eq%20true'
                        + '&$select=Id,Version,Description&$format=json&$top=' + LIMIT;
                }
                else {
                    queryUrl = 'https://www.nuget.org/api/v2/Packages?'
                        + '$filter=IsAbsoluteLatestVersion%20eq%20true'
                        + '&$orderby=DownloadCount%20desc&$top=' + LIMIT
                        + '&$select=Id,Version,DownloadCount,Description&$format=json';
                }
                return this.requestService.makeRequest({
                    url: queryUrl
                }).then(function (success) {
                    if (success.status === 200) {
                        try {
                            var obj = JSON.parse(success.responseText);
                            if (Array.isArray(obj.d)) {
                                var results = obj.d;
                                for (var i = 0; i < results.length; i++) {
                                    var curr = results[i];
                                    var name = curr.Id;
                                    var version = curr.Version;
                                    if (name) {
                                        var documentation = curr.Description;
                                        var typeLabel = curr.Version;
                                        var codeSnippet = JSON.stringify(name);
                                        if (addValue) {
                                            codeSnippet += ': "{{' + version + '}}"';
                                            if (!isLast) {
                                                codeSnippet += ',';
                                            }
                                        }
                                        result.add({ type: 'property', label: name, codeSnippet: codeSnippet, typeLabel: typeLabel, documentationLabel: documentation });
                                    }
                                }
                                if (results.length === LIMIT) {
                                    result.setAsIncomplete();
                                }
                            }
                        }
                        catch (e) {
                        }
                    }
                    else {
                        result.error(nls.localize(1, null, success.responseText));
                        return 0;
                    }
                }, function (error) {
                    result.error(nls.localize(2, null, error.responseText));
                    return 0;
                });
            }
            return null;
        };
        ProjectJSONContribution.prototype.collectValueSuggestions = function (resource, location, currentKey, result) {
            if (this.isProjectJSONFile(resource) && (location.matches(['dependencies']) || location.matches(['frameworks', '*', 'dependencies']) || location.matches(['frameworks', '*', 'frameworkAssemblies']))) {
                var queryUrl = 'https://www.myget.org/F/aspnetrelease/api/v2/Packages?'
                    + '$filter=Id%20eq%20\''
                    + encodeURIComponent(currentKey)
                    + '\'&$select=Version,IsAbsoluteLatestVersion&$format=json&$top=' + LIMIT;
                return this.requestService.makeRequest({
                    url: queryUrl
                }).then(function (success) {
                    try {
                        var obj = JSON.parse(success.responseText);
                        if (Array.isArray(obj.d)) {
                            var results = obj.d;
                            for (var i = 0; i < results.length; i++) {
                                var curr = results[i];
                                var version = curr.Version;
                                if (version) {
                                    var name = JSON.stringify(version);
                                    var isLatest = curr.IsAbsoluteLatestVersion === 'true';
                                    var label = name;
                                    var documentationLabel = '';
                                    if (isLatest) {
                                        documentationLabel = nls.localize(3, null);
                                    }
                                    result.add({ type: 'class', label: label, codeSnippet: name, documentationLabel: documentationLabel });
                                }
                            }
                            if (results.length === LIMIT) {
                                result.setAsIncomplete();
                            }
                        }
                    }
                    catch (e) {
                    }
                    return 0;
                }, function (error) {
                    return 0;
                });
            }
            return null;
        };
        ProjectJSONContribution.prototype.getInfoContribution = function (resource, location) {
            if (this.isProjectJSONFile(resource) && (location.matches(['dependencies', '*']) || location.matches(['frameworks', '*', 'dependencies', '*']) || location.matches(['frameworks', '*', 'frameworkAssemblies', '*']))) {
                var pack = location.getSegments()[location.getSegments().length - 1];
                var htmlContent = [];
                htmlContent.push(nls.localize(4, null, pack));
                var queryUrl = 'https://www.myget.org/F/aspnetrelease/api/v2/Packages?'
                    + '$filter=Id%20eq%20\''
                    + encodeURIComponent(pack)
                    + '\'%20and%20IsAbsoluteLatestVersion%20eq%20true'
                    + '&$select=Version,Description&$format=json&$top=5';
                return this.requestService.makeRequest({
                    url: queryUrl
                }).then(function (success) {
                    var content = success.responseText;
                    if (content) {
                        try {
                            var obj = JSON.parse(content);
                            if (obj.d && obj.d[0]) {
                                var res = obj.d[0];
                                if (res.Description) {
                                    htmlContent.push(res.Description);
                                }
                                if (res.Version) {
                                    htmlContent.push(nls.localize(5, null, res.Version));
                                }
                            }
                        }
                        catch (e) {
                        }
                    }
                    return htmlContent;
                }, function (error) {
                    return htmlContent;
                });
            }
            return null;
        };
        ProjectJSONContribution = __decorate([
            __param(0, request_1.IRequestService)
        ], ProjectJSONContribution);
        return ProjectJSONContribution;
    }());
    exports.ProjectJSONContribution = ProjectJSONContribution;
});

/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/









define(__m[28], __M([1,0,16,35,7,18,4,31,6,9,29,5,19,33]), function (require, exports, nls, Objects, Json, http, Strings, uri_1, Types, WinJS, resourceService_1, request_1, workspace_1, lifecycle_1) {
    'use strict';
    var FilePatternAssociation = (function () {
        function FilePatternAssociation(pattern) {
            this.combinedSchemaId = 'local://combinedSchema/' + encodeURIComponent(pattern);
            try {
                this.patternRegExp = new RegExp(Strings.convertSimple2RegExpPattern(pattern) + '$');
            }
            catch (e) {
                // invalid pattern
                this.patternRegExp = null;
            }
            this.schemas = [];
            this.combinedSchema = null;
        }
        FilePatternAssociation.prototype.addSchema = function (id) {
            this.schemas.push(id);
            this.combinedSchema = null;
        };
        FilePatternAssociation.prototype.matchesPattern = function (fileName) {
            return this.patternRegExp && this.patternRegExp.test(fileName);
        };
        FilePatternAssociation.prototype.getCombinedSchema = function (service) {
            if (!this.combinedSchema) {
                this.combinedSchema = service.createCombinedSchema(this.combinedSchemaId, this.schemas);
            }
            return this.combinedSchema;
        };
        return FilePatternAssociation;
    }());
    var SchemaHandle = (function () {
        function SchemaHandle(service, url, unresolvedSchemaContent) {
            this.service = service;
            this.url = url;
            if (unresolvedSchemaContent) {
                this.unresolvedSchema = WinJS.TPromise.as(new UnresolvedSchema(unresolvedSchemaContent));
            }
        }
        SchemaHandle.prototype.getUnresolvedSchema = function () {
            if (!this.unresolvedSchema) {
                this.unresolvedSchema = this.service.loadSchema(this.url);
            }
            return this.unresolvedSchema;
        };
        SchemaHandle.prototype.getResolvedSchema = function () {
            var _this = this;
            if (!this.resolvedSchema) {
                this.resolvedSchema = this.getUnresolvedSchema().then(function (unresolved) {
                    return _this.service.resolveSchemaContent(unresolved);
                });
            }
            return this.resolvedSchema;
        };
        SchemaHandle.prototype.clearSchema = function () {
            this.resolvedSchema = null;
            this.unresolvedSchema = null;
        };
        return SchemaHandle;
    }());
    var UnresolvedSchema = (function () {
        function UnresolvedSchema(schema, errors) {
            if (errors === void 0) { errors = []; }
            this.schema = schema;
            this.errors = errors;
        }
        return UnresolvedSchema;
    }());
    exports.UnresolvedSchema = UnresolvedSchema;
    var ResolvedSchema = (function () {
        function ResolvedSchema(schema, errors) {
            if (errors === void 0) { errors = []; }
            this.schema = schema;
            this.errors = errors;
        }
        ResolvedSchema.prototype.getSection = function (path) {
            return this.getSectionRecursive(path, this.schema);
        };
        ResolvedSchema.prototype.getSectionRecursive = function (path, schema) {
            var _this = this;
            if (!schema || path.length === 0) {
                return schema;
            }
            var next = path.shift();
            if (schema.properties && schema.properties[next]) {
                return this.getSectionRecursive(path, schema.properties[next]);
            }
            else if (Types.isObject(schema.patternProperties)) {
                Object.keys(schema.patternProperties).forEach(function (pattern) {
                    var regex = new RegExp(pattern);
                    if (regex.test(next)) {
                        return _this.getSectionRecursive(path, schema.patternProperties[pattern]);
                    }
                });
            }
            else if (Types.isObject(schema.additionalProperties)) {
                return this.getSectionRecursive(path, schema.additionalProperties);
            }
            else if (next.match('[0-9]+')) {
                if (Types.isObject(schema.items)) {
                    return this.getSectionRecursive(path, schema.items);
                }
                else if (Array.isArray(schema.items)) {
                    try {
                        var index = parseInt(next, 10);
                        if (schema.items[index]) {
                            return this.getSectionRecursive(path, schema.items[index]);
                        }
                        return null;
                    }
                    catch (e) {
                        return null;
                    }
                }
            }
            return null;
        };
        return ResolvedSchema;
    }());
    exports.ResolvedSchema = ResolvedSchema;
    var JSONSchemaService = (function () {
        function JSONSchemaService(requestService, contextService, resourceService) {
            var _this = this;
            this.requestService = requestService;
            this.contextService = contextService;
            this.callOnDispose = [];
            if (resourceService) {
                this.callOnDispose.push(resourceService.addListener2_(resourceService_1.ResourceEvents.CHANGED, function (e) { return _this.onResourceChange(e); }));
            }
            this.contributionSchemas = {};
            this.contributionAssociations = {};
            this.schemasById = {};
            this.filePatternAssociations = [];
            this.filePatternAssociationById = {};
        }
        JSONSchemaService.prototype.dispose = function () {
            this.callOnDispose = lifecycle_1.dispose(this.callOnDispose);
        };
        JSONSchemaService.prototype.onResourceChange = function (e) {
            var url = e.url.toString();
            var schemaFile = this.schemasById[url];
            if (schemaFile) {
                schemaFile.clearSchema();
            }
        };
        JSONSchemaService.prototype.normalizeId = function (id) {
            if (id.length > 0 && id.charAt(id.length - 1) === '#') {
                return id.substring(0, id.length - 1);
            }
            return id;
        };
        JSONSchemaService.prototype.setSchemaContributions = function (schemaContributions) {
            if (schemaContributions.schemas) {
                var schemas = schemaContributions.schemas;
                for (var id in schemas) {
                    id = this.normalizeId(id);
                    this.contributionSchemas[id] = this.addSchemaHandle(id, schemas[id]);
                }
            }
        };
        JSONSchemaService.prototype.addSchemaHandle = function (id, unresolvedSchemaContent) {
            var schemaHandle = new SchemaHandle(this, id, unresolvedSchemaContent);
            this.schemasById[id] = schemaHandle;
            return schemaHandle;
        };
        JSONSchemaService.prototype.getOrAddSchemaHandle = function (id, unresolvedSchemaContent) {
            return this.schemasById[id] || this.addSchemaHandle(id, unresolvedSchemaContent);
        };
        JSONSchemaService.prototype.getOrAddFilePatternAssociation = function (pattern) {
            var fpa = this.filePatternAssociationById[pattern];
            if (!fpa) {
                fpa = new FilePatternAssociation(pattern);
                this.filePatternAssociationById[pattern] = fpa;
                this.filePatternAssociations.push(fpa);
            }
            return fpa;
        };
        JSONSchemaService.prototype.registerExternalSchema = function (uri, filePatterns, unresolvedSchemaContent) {
            var _this = this;
            if (filePatterns === void 0) { filePatterns = null; }
            var id = this.normalizeId(uri);
            if (filePatterns) {
                filePatterns.forEach(function (pattern) {
                    _this.getOrAddFilePatternAssociation(pattern).addSchema(uri);
                });
            }
            return unresolvedSchemaContent ? this.addSchemaHandle(id, unresolvedSchemaContent) : this.getOrAddSchemaHandle(id);
        };
        JSONSchemaService.prototype.clearExternalSchemas = function () {
            var _this = this;
            this.schemasById = {};
            this.filePatternAssociations = [];
            this.filePatternAssociationById = {};
            for (var id in this.contributionSchemas) {
                this.schemasById[id] = this.contributionSchemas[id];
            }
            for (var pattern in this.contributionAssociations) {
                var fpa = this.getOrAddFilePatternAssociation(pattern);
                this.contributionAssociations[pattern].forEach(function (schemaId) {
                    var id = _this.normalizeId(schemaId);
                    fpa.addSchema(id);
                });
            }
        };
        JSONSchemaService.prototype.getResolvedSchema = function (schemaId) {
            var id = this.normalizeId(schemaId);
            var schemaHandle = this.schemasById[id];
            if (schemaHandle) {
                return schemaHandle.getResolvedSchema();
            }
            return WinJS.TPromise.as(null);
        };
        JSONSchemaService.prototype.loadSchema = function (url) {
            return this.requestService.makeRequest({ url: url }).then(function (request) {
                var content = request.responseText;
                if (!content) {
                    var errorMessage = nls.localize(0, null, toDisplayString(url));
                    return new UnresolvedSchema({}, [errorMessage]);
                }
                var schemaContent = {};
                var jsonErrors = [];
                schemaContent = Json.parse(content, jsonErrors);
                var errors = jsonErrors.length ? [nls.localize(1, null, toDisplayString(url), Json.getParseErrorMessage(jsonErrors[0].error))] : [];
                return new UnresolvedSchema(schemaContent, errors);
            }, function (error) {
                var errorMessage = nls.localize(2, null, toDisplayString(url), error.responseText || http.getErrorStatusDescription(error.status) || error.toString());
                return new UnresolvedSchema({}, [errorMessage]);
            });
        };
        JSONSchemaService.prototype.resolveSchemaContent = function (schemaToResolve) {
            var _this = this;
            var resolveErrors = schemaToResolve.errors.slice(0);
            var schema = schemaToResolve.schema;
            var findSection = function (schema, path) {
                if (!path) {
                    return schema;
                }
                var current = schema;
                path.substr(1).split('/').some(function (part) {
                    current = current[part];
                    return !current;
                });
                return current;
            };
            var resolveLink = function (node, linkedSchema, linkPath) {
                var section = findSection(linkedSchema, linkPath);
                if (typeof section === 'object') {
                    Objects.mixin(node, section, false);
                }
                else {
                    resolveErrors.push(nls.localize(3, null, linkPath, linkedSchema.id));
                }
                delete node.$ref;
            };
            var resolveExternalLink = function (node, uri, linkPath) {
                return _this.getOrAddSchemaHandle(uri).getUnresolvedSchema().then(function (unresolvedSchema) {
                    if (unresolvedSchema.errors.length) {
                        var loc = linkPath ? uri + '#' + linkPath : uri;
                        resolveErrors.push(nls.localize(4, null, loc, unresolvedSchema.errors[0]));
                    }
                    resolveLink(node, unresolvedSchema.schema, linkPath);
                    return resolveRefs(node, unresolvedSchema.schema);
                });
            };
            var resolveRefs = function (node, parentSchema) {
                var toWalk = [node];
                var seen = [];
                var openPromises = [];
                var collectEntries = function () {
                    var entries = [];
                    for (var _i = 0; _i < arguments.length; _i++) {
                        entries[_i - 0] = arguments[_i];
                    }
                    for (var _a = 0, entries_1 = entries; _a < entries_1.length; _a++) {
                        var entry = entries_1[_a];
                        if (typeof entry === 'object') {
                            toWalk.push(entry);
                        }
                    }
                };
                var collectMapEntries = function () {
                    var maps = [];
                    for (var _i = 0; _i < arguments.length; _i++) {
                        maps[_i - 0] = arguments[_i];
                    }
                    for (var _a = 0, maps_1 = maps; _a < maps_1.length; _a++) {
                        var map = maps_1[_a];
                        if (typeof map === 'object') {
                            for (var key in map) {
                                var entry = map[key];
                                toWalk.push(entry);
                            }
                        }
                    }
                };
                var collectArrayEntries = function () {
                    var arrays = [];
                    for (var _i = 0; _i < arguments.length; _i++) {
                        arrays[_i - 0] = arguments[_i];
                    }
                    for (var _a = 0, arrays_1 = arrays; _a < arrays_1.length; _a++) {
                        var array = arrays_1[_a];
                        if (Array.isArray(array)) {
                            toWalk.push.apply(toWalk, array);
                        }
                    }
                };
                while (toWalk.length) {
                    var next = toWalk.pop();
                    if (seen.indexOf(next) >= 0) {
                        continue;
                    }
                    seen.push(next);
                    if (next.$ref) {
                        var segments = next.$ref.split('#', 2);
                        if (segments[0].length > 0) {
                            openPromises.push(resolveExternalLink(next, segments[0], segments[1]));
                            continue;
                        }
                        else {
                            resolveLink(next, parentSchema, segments[1]);
                        }
                    }
                    collectEntries(next.items, next.additionalProperties, next.not);
                    collectMapEntries(next.definitions, next.properties, next.patternProperties, next.dependencies);
                    collectArrayEntries(next.anyOf, next.allOf, next.oneOf, next.items);
                }
                return WinJS.Promise.join(openPromises);
            };
            return resolveRefs(schema, schema).then(function (_) { return new ResolvedSchema(schema, resolveErrors); });
        };
        JSONSchemaService.prototype.getSchemaForResource = function (resource, document) {
            // first use $schema if present
            if (document && document.root && document.root.type === 'object') {
                var schemaProperties = document.root.properties.filter(function (p) { return (p.key.value === '$schema') && !!p.value; });
                if (schemaProperties.length > 0) {
                    var schemeId = schemaProperties[0].value.getValue();
                    if (!Strings.startsWith(schemeId, 'http://') && !Strings.startsWith(schemeId, 'https://') && !Strings.startsWith(schemeId, 'file://')) {
                        var resourceURL = this.contextService.toResource(schemeId);
                        if (resourceURL) {
                            schemeId = resourceURL.toString();
                        }
                    }
                    if (schemeId) {
                        var id = this.normalizeId(schemeId);
                        return this.getOrAddSchemaHandle(id).getResolvedSchema();
                    }
                }
            }
            // then check for matching file names, last to first
            for (var i = this.filePatternAssociations.length - 1; i >= 0; i--) {
                var entry = this.filePatternAssociations[i];
                if (entry.matchesPattern(resource)) {
                    return entry.getCombinedSchema(this).getResolvedSchema();
                }
            }
            return WinJS.TPromise.as(null);
        };
        JSONSchemaService.prototype.createCombinedSchema = function (combinedSchemaId, schemaIds) {
            if (schemaIds.length === 1) {
                return this.getOrAddSchemaHandle(schemaIds[0]);
            }
            else {
                var combinedSchema = {
                    allOf: schemaIds.map(function (schemaId) { return ({ $ref: schemaId }); })
                };
                return this.addSchemaHandle(combinedSchemaId, combinedSchema);
            }
        };
        JSONSchemaService = __decorate([
            __param(0, request_1.IRequestService),
            __param(1, workspace_1.IWorkspaceContextService),
            __param(2, resourceService_1.IResourceService)
        ], JSONSchemaService);
        return JSONSchemaService;
    }());
    exports.JSONSchemaService = JSONSchemaService;
    function toDisplayString(url) {
        try {
            var uri = uri_1.default.parse(url);
            if (uri.scheme === 'file') {
                return uri.fsPath;
            }
        }
        catch (e) {
        }
        return url;
    }
});










define(__m[34], __M([1,0,30,36,8,20,28,21,9,4,27,26,24,11,23,37,5,19,29,38,39,40]), function (require, exports, severity_1, Modes, Parser, JSONFormatter, SchemaService, JSONIntellisense, WinJS, Strings, ProjectJSONContribution, PackageJSONContribution, BowerJSONContribution, GlobPatternContribution, errors, markers_1, request_1, workspace_1, resourceService_1, instantiation_1, suggestSupport_1, validationHelper_1) {
    /*---------------------------------------------------------------------------------------------
     *  Copyright (c) Microsoft Corporation. All rights reserved.
     *  Licensed under the MIT License. See License.txt in the project root for license information.
     *--------------------------------------------------------------------------------------------*/
    'use strict';
    var JSONWorker = (function () {
        function JSONWorker(modeId, resourceService, markerService, requestService, contextService, instantiationService) {
            var _this = this;
            this._modeId = modeId;
            this.resourceService = resourceService;
            this.markerService = markerService;
            this._validationHelper = new validationHelper_1.ValidationHelper(this.resourceService, this._modeId, function (toValidate) { return _this.doValidate(toValidate); });
            this.requestService = requestService;
            this.contextService = contextService;
            this.schemaService = instantiationService.createInstance(SchemaService.JSONSchemaService);
            this.contributions = [
                instantiationService.createInstance(ProjectJSONContribution.ProjectJSONContribution),
                instantiationService.createInstance(PackageJSONContribution.PackageJSONContribution),
                instantiationService.createInstance(BowerJSONContribution.BowerJSONContribution),
                instantiationService.createInstance(GlobPatternContribution.GlobPatternContribution)
            ];
            this.jsonIntellisense = new JSONIntellisense.JSONIntellisense(this.schemaService, this.requestService, this.contributions);
        }
        JSONWorker.prototype.navigateValueSet = function (resource, range, up) {
            var _this = this;
            var modelMirror = this.resourceService.get(resource);
            var offset = modelMirror.getOffsetFromPosition({ lineNumber: range.startLineNumber, column: range.startColumn });
            var parser = new Parser.JSONParser();
            var config = new Parser.JSONDocumentConfig();
            config.ignoreDanglingComma = true;
            var doc = parser.parse(modelMirror.getValue(), config);
            var node = doc.getNodeFromOffsetEndInclusive(offset);
            if (node && (node.type === 'string' || node.type === 'number' || node.type === 'boolean' || node.type === 'null')) {
                return this.schemaService.getSchemaForResource(resource.toString(), doc).then(function (schema) {
                    if (schema) {
                        var proposals = [];
                        var proposed = {};
                        var collector = {
                            add: function (suggestion) {
                                if (!proposed[suggestion.label]) {
                                    proposed[suggestion.label] = true;
                                    proposals.push(suggestion);
                                }
                            },
                            setAsIncomplete: function () { },
                            error: function (message) {
                                errors.onUnexpectedError(message);
                            }
                        };
                        _this.jsonIntellisense.getValueSuggestions(resource, schema, doc, node.parent, node.start, collector);
                        var range = modelMirror.getRangeFromOffsetAndLength(node.start, node.end - node.start);
                        var text = modelMirror.getValueInRange(range);
                        for (var i = 0, len = proposals.length; i < len; i++) {
                            if (Strings.equalsIgnoreCase(proposals[i].label, text)) {
                                var nextIdx = i;
                                if (up) {
                                    nextIdx = (i + 1) % len;
                                }
                                else {
                                    nextIdx = i - 1;
                                    if (nextIdx < 0) {
                                        nextIdx = len - 1;
                                    }
                                }
                                return {
                                    value: proposals[nextIdx].label,
                                    range: range
                                };
                            }
                        }
                        return null;
                    }
                });
            }
            return null;
        };
        /**
         * @return true if you want to revalidate your models
         */
        JSONWorker.prototype._doConfigure = function (options) {
            var _this = this;
            if (options && options.schemas) {
                this.schemaService.clearExternalSchemas();
                options.schemas.forEach(function (schema) {
                    if (schema.url && (schema.fileMatch || schema.schema)) {
                        var url = schema.url;
                        if (!Strings.startsWith(url, 'http://') && !Strings.startsWith(url, 'https://') && !Strings.startsWith(url, 'file://')) {
                            var resourceURL = _this.contextService.toResource(url);
                            if (resourceURL) {
                                url = resourceURL.toString();
                            }
                        }
                        if (url) {
                            _this.schemaService.registerExternalSchema(url, schema.fileMatch, schema.schema);
                        }
                    }
                    else if (schema.filePattern && schema.schemaPath) {
                        var url = _this.contextService.toResource(schema.schemaPath).toString();
                        var patterns = schema.filePattern ? [schema.filePattern] : [];
                        _this.schemaService.registerExternalSchema(url, patterns);
                    }
                });
            }
            this._validationHelper.triggerDueToConfigurationChange();
            return WinJS.TPromise.as(void 0);
        };
        JSONWorker.prototype.setSchemaContributions = function (contributions) {
            this.schemaService.setSchemaContributions(contributions);
            return WinJS.TPromise.as(true);
        };
        JSONWorker.prototype.enableValidator = function () {
            this._validationHelper.enable();
            return WinJS.TPromise.as(null);
        };
        JSONWorker.prototype.doValidate = function (resources) {
            for (var i = 0; i < resources.length; i++) {
                this.doValidate1(resources[i]);
            }
        };
        JSONWorker.prototype.doValidate1 = function (resource) {
            var _this = this;
            var modelMirror = this.resourceService.get(resource);
            var parser = new Parser.JSONParser();
            var content = modelMirror.getValue();
            if (content.length === 0) {
                // ignore empty content, no marker can be set anyway
                return;
            }
            var result = parser.parse(content);
            this.schemaService.getSchemaForResource(resource.toString(), result).then(function (schema) {
                if (schema) {
                    if (schema.errors.length && result.root) {
                        var property = result.root.type === 'object' ? result.root.getFirstProperty('$schema') : null;
                        if (property) {
                            var node = property.value || property;
                            result.warnings.push({ location: { start: node.start, end: node.end }, message: schema.errors[0] });
                        }
                        else {
                            result.warnings.push({ location: { start: result.root.start, end: result.root.start + 1 }, message: schema.errors[0] });
                        }
                    }
                    else {
                        result.validate(schema.schema);
                    }
                }
                var added = {};
                var markerData = [];
                result.errors.concat(result.warnings).forEach(function (error, idx) {
                    // remove duplicated messages
                    var signature = error.location.start + ' ' + error.location.end + ' ' + error.message;
                    if (!added[signature]) {
                        added[signature] = true;
                        var startPosition = modelMirror.getPositionFromOffset(error.location.start);
                        var endPosition = modelMirror.getPositionFromOffset(error.location.end);
                        markerData.push({
                            message: error.message,
                            severity: idx >= result.errors.length ? severity_1.default.Warning : severity_1.default.Error,
                            startLineNumber: startPosition.lineNumber,
                            startColumn: startPosition.column,
                            endLineNumber: endPosition.lineNumber,
                            endColumn: endPosition.column
                        });
                    }
                });
                _this.markerService.changeOne(_this._modeId, resource, markerData);
            });
        };
        JSONWorker.prototype.provideCompletionItems = function (resource, position) {
            return this.doSuggest(resource, position).then(function (value) { return suggestSupport_1.filterSuggestions(value); });
        };
        JSONWorker.prototype.doSuggest = function (resource, position) {
            var modelMirror = this.resourceService.get(resource);
            return this.jsonIntellisense.doSuggest(resource, modelMirror, position);
        };
        JSONWorker.prototype.provideHover = function (resource, position) {
            var _this = this;
            var modelMirror = this.resourceService.get(resource);
            var parser = new Parser.JSONParser();
            var doc = parser.parse(modelMirror.getValue());
            var offset = modelMirror.getOffsetFromPosition(position);
            var node = doc.getNodeFromOffset(offset);
            var originalNode = node;
            // use the property description when hovering over an object key
            if (node && node.type === 'string') {
                var stringNode = node;
                if (stringNode.isKey) {
                    var propertyNode = node.parent;
                    node = propertyNode.value;
                }
            }
            if (!node) {
                return WinJS.TPromise.as(null);
            }
            return this.schemaService.getSchemaForResource(resource.toString(), doc).then(function (schema) {
                if (schema) {
                    var matchingSchemas = [];
                    doc.validate(schema.schema, matchingSchemas, node.start);
                    var description = null;
                    var contributonId = null;
                    matchingSchemas.every(function (s) {
                        if (s.node === node && !s.inverted && s.schema) {
                            description = description || s.schema.description;
                            contributonId = contributonId || s.schema.id;
                        }
                        return true;
                    });
                    var location = node.getNodeLocation();
                    for (var i = _this.contributions.length - 1; i >= 0; i--) {
                        var contribution = _this.contributions[i];
                        var promise = contribution.getInfoContribution(resource, location);
                        if (promise) {
                            return promise.then(function (htmlContent) { return _this.createInfoResult(htmlContent, originalNode, modelMirror); });
                        }
                    }
                    if (description) {
                        var htmlContent = [description];
                        return _this.createInfoResult(htmlContent, originalNode, modelMirror);
                    }
                }
                return null;
            });
        };
        JSONWorker.prototype.createInfoResult = function (htmlContent, node, modelMirror) {
            var range = modelMirror.getRangeFromOffsetAndLength(node.start, node.end - node.start);
            var result = {
                contents: htmlContent,
                range: range
            };
            return result;
        };
        JSONWorker.prototype.provideDocumentSymbols = function (resource) {
            var modelMirror = this.resourceService.get(resource);
            var parser = new Parser.JSONParser();
            var doc = parser.parse(modelMirror.getValue());
            var root = doc.root;
            if (!root) {
                return WinJS.TPromise.as(null);
            }
            // special handling for key bindings
            var resourceString = resource.toString();
            if ((resourceString === 'vscode://defaultsettings/keybindings.json') || Strings.endsWith(resourceString.toLowerCase(), '/user/keybindings.json')) {
                if (root.type === 'array') {
                    var result = [];
                    root.items.forEach(function (item) {
                        if (item.type === 'object') {
                            var property = item.getFirstProperty('key');
                            if (property && property.value) {
                                var range = modelMirror.getRangeFromOffsetAndLength(item.start, item.end - item.start);
                                result.push({
                                    name: property.value.getValue(),
                                    kind: Modes.SymbolKind.String,
                                    location: {
                                        uri: resource,
                                        range: range
                                    }
                                });
                            }
                        }
                    });
                    return WinJS.TPromise.as(result);
                }
            }
            function collectOutlineEntries(result, node, containerName) {
                if (node.type === 'array') {
                    node.items.forEach(function (node) {
                        collectOutlineEntries(result, node, containerName);
                    });
                }
                else if (node.type === 'object') {
                    var objectNode = node;
                    objectNode.properties.forEach(function (property) {
                        var range = modelMirror.getRangeFromOffsetAndLength(property.start, property.end - property.start);
                        var valueNode = property.value;
                        if (valueNode) {
                            var childContainerName = containerName ? containerName + '.' + property.key.name : property.key.name;
                            result.push({
                                name: property.key.getValue(),
                                kind: getSymbolKind(valueNode.type),
                                location: {
                                    uri: resource,
                                    range: range,
                                },
                                containerName: containerName
                            });
                            collectOutlineEntries(result, valueNode, childContainerName);
                        }
                    });
                }
                return result;
            }
            var result = collectOutlineEntries([], root, void 0);
            return WinJS.TPromise.as(result);
        };
        JSONWorker.prototype.format = function (resource, range, options) {
            var model = this.resourceService.get(resource);
            var formatRange = range ? model.getOffsetAndLengthFromRange(range) : void 0;
            var edits = JSONFormatter.format(model.getValue(), formatRange, { insertSpaces: options.insertSpaces, tabSize: options.tabSize, eol: model.getEOL() });
            var operations = edits.map(function (e) { return ({ range: model.getRangeFromOffsetAndLength(e.offset, e.length), text: e.content }); });
            return WinJS.TPromise.as(operations);
        };
        JSONWorker = __decorate([
            __param(1, resourceService_1.IResourceService),
            __param(2, markers_1.IMarkerService),
            __param(3, request_1.IRequestService),
            __param(4, workspace_1.IWorkspaceContextService),
            __param(5, instantiation_1.IInstantiationService)
        ], JSONWorker);
        return JSONWorker;
    }());
    exports.JSONWorker = JSONWorker;
    function getSymbolKind(nodeType) {
        switch (nodeType) {
            case 'object':
                return Modes.SymbolKind.Module;
            case 'string':
                return Modes.SymbolKind.String;
            case 'number':
                return Modes.SymbolKind.Number;
            case 'array':
                return Modes.SymbolKind.Array;
            case 'boolean':
                return Modes.SymbolKind.Boolean;
            default:
                return Modes.SymbolKind.Variable;
        }
    }
});

}).call(this);
//# sourceMappingURL=jsonWorker.js.map
