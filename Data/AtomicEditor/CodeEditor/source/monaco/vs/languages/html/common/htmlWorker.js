/*!-----------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Version: 0.5.2(d49899a916fd99840f6f9178f2dd06e0e7013646)
 * Released under the MIT license
 * https://github.com/Microsoft/vscode/blob/master/LICENSE.txt
 *-----------------------------------------------------------*/

(function() {
var __m = ["require","exports","vs/base/common/errors","vs/base/common/winjs.base","vs/editor/common/editorCommonExtensions","vs/editor/common/modes","vs/platform/keybinding/common/keybindingService","vs/platform/instantiation/common/instantiation","vs/base/common/async","vs/base/common/keyCodes","vs/platform/keybinding/common/keybindingsRegistry","vs/base/common/strings","vs/editor/common/services/codeEditorService","vs/languages/html/common/htmlTokenTypes","vs/platform/editor/common/editor","vs/nls!vs/base/common/keyCodes","vs/nls","vs/editor/contrib/suggest/common/suggest","vs/platform/platform","vs/editor/common/config/config","vs/editor/common/editorCommon","vs/nls!vs/languages/html/common/htmlWorker","vs/languages/html/common/htmlScanner","vs/base/common/uri","vs/editor/contrib/referenceSearch/common/referenceSearch","vs/base/common/platform","vs/editor/common/core/position","vs/nls!vs/languages/html/common/htmlTags","vs/editor/contrib/hover/common/hover","vs/base/common/arrays","vs/languages/html/common/htmlTags","vs/base/common/network","vs/editor/common/services/modelService","vs/platform/instantiation/common/descriptors","vs/base/common/types","vs/editor/common/modes/supports","vs/languages/html/common/htmlWorker","vs/languages/lib/common/beautify-html","vs/platform/telemetry/common/telemetry","vs/platform/workspace/common/workspace","vs/platform/markers/common/markers","vs/editor/common/services/resourceService","vs/languages/html/common/htmlEmptyTagsShared","vs/editor/common/modes/supports/suggestSupport","vs/base/common/paths"];
var __M = function(deps) {
  var result = [];
  for (var i = 0, len = deps.length; i < len; i++) {
    result[i] = __m[deps[i]];
  }
  return result;
};
define(__m[22], __M([0,1,13]), function (require, exports, htmlTokenTypes_1) {
    /*---------------------------------------------------------------------------------------------
     *  Copyright (c) Microsoft Corporation. All rights reserved.
     *  Licensed under the MIT License. See License.txt in the project root for license information.
     *--------------------------------------------------------------------------------------------*/
    'use strict';
    function isDelimiter(tokenType) {
        switch (tokenType) {
            case htmlTokenTypes_1.DELIM_START:
            case htmlTokenTypes_1.DELIM_END:
            case htmlTokenTypes_1.DELIM_ASSIGN:
                return true;
        }
        return false;
    }
    function isInterestingToken(tokenType) {
        switch (tokenType) {
            case htmlTokenTypes_1.DELIM_START:
            case htmlTokenTypes_1.DELIM_END:
            case htmlTokenTypes_1.DELIM_ASSIGN:
            case htmlTokenTypes_1.ATTRIB_NAME:
            case htmlTokenTypes_1.ATTRIB_VALUE:
                return true;
        }
        return htmlTokenTypes_1.isTag(tokenType);
    }
    function getScanner(model, position) {
        var lineOffset = position.column - 1;
        var currentLine = position.lineNumber;
        var tokens = model.getLineTokens(currentLine);
        var lineContent = model.getLineContent(currentLine);
        var tokenIndex = tokens.findIndexOfOffset(lineOffset);
        var tokensOnLine = tokens.getTokenCount();
        var tokenType = tokens.getTokenType(tokenIndex);
        var tokenStart = tokens.getTokenStartIndex(tokenIndex);
        var tokenEnd = tokens.getTokenEndIndex(tokenIndex, lineContent.length);
        if ((tokenType === '' || isDelimiter(tokenType)) && tokenStart === lineOffset) {
            tokenIndex--;
            if (tokenIndex >= 0) {
                // we are at the end of a different token
                tokenType = tokens.getTokenType(tokenIndex);
                tokenStart = tokens.getTokenStartIndex(tokenIndex);
                tokenEnd = tokens.getTokenEndIndex(tokenIndex, lineContent.length);
            }
            else {
                tokenType = '';
                tokenStart = tokenEnd = 0;
            }
        }
        return {
            getTokenType: function () { return tokenType; },
            isAtTokenEnd: function () { return lineOffset === tokenEnd; },
            isAtTokenStart: function () { return lineOffset === tokenStart; },
            getTokenContent: function () { return lineContent.substring(tokenStart, tokenEnd); },
            isOpenBrace: function () { return tokenStart < tokenEnd && lineContent.charAt(tokenStart) === '<'; },
            getTokenPosition: function () { return { lineNumber: currentLine, column: tokenStart + 1 }; },
            getTokenRange: function () { return { startLineNumber: currentLine, startColumn: tokenStart + 1, endLineNumber: currentLine, endColumn: tokenEnd + 1 }; },
            getModel: function () { return model; },
            scanBack: function () {
                if (currentLine <= 0) {
                    return false;
                }
                tokenIndex--;
                do {
                    while (tokenIndex >= 0) {
                        tokenType = tokens.getTokenType(tokenIndex);
                        tokenStart = tokens.getTokenStartIndex(tokenIndex);
                        tokenEnd = tokens.getTokenEndIndex(tokenIndex, lineContent.length);
                        if (isInterestingToken(tokenType)) {
                            return true;
                        }
                        tokenIndex--;
                    }
                    currentLine--;
                    if (currentLine > 0) {
                        tokens = model.getLineTokens(currentLine);
                        lineContent = model.getLineContent(currentLine);
                        tokensOnLine = tokens.getTokenCount();
                        tokenIndex = tokensOnLine - 1;
                    }
                } while (currentLine > 0);
                tokens = null;
                tokenType = lineContent = '';
                tokenStart = tokenEnd = tokensOnLine = 0;
                return false;
            },
            scanForward: function () {
                if (currentLine > model.getLineCount()) {
                    return false;
                }
                tokenIndex++;
                do {
                    while (tokenIndex < tokensOnLine) {
                        tokenType = tokens.getTokenType(tokenIndex);
                        tokenStart = tokens.getTokenStartIndex(tokenIndex);
                        tokenEnd = tokens.getTokenEndIndex(tokenIndex, lineContent.length);
                        if (isInterestingToken(tokenType)) {
                            return true;
                        }
                        tokenIndex++;
                    }
                    currentLine++;
                    tokenIndex = 0;
                    if (currentLine <= model.getLineCount()) {
                        tokens = model.getLineTokens(currentLine);
                        lineContent = model.getLineContent(currentLine);
                        tokensOnLine = tokens.getTokenCount();
                    }
                } while (currentLine <= model.getLineCount());
                tokenType = lineContent = '';
                tokenStart = tokenEnd = tokensOnLine = 0;
                return false;
            }
        };
    }
    exports.getScanner = getScanner;
});

define(__m[15], __M([16,21]), function(nls, data) { return nls.create("vs/base/common/keyCodes", data); });
/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
define(__m[9], __M([0,1,15,25]), function (require, exports, nls, defaultPlatform) {
    'use strict';
    /**
     * Virtual Key Codes, the value does not hold any inherent meaning.
     * Inspired somewhat from https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
     * But these are "more general", as they should work across browsers & OS`s.
     */
    (function (KeyCode) {
        /**
         * Placed first to cover the 0 value of the enum.
         */
        KeyCode[KeyCode["Unknown"] = 0] = "Unknown";
        KeyCode[KeyCode["Backspace"] = 1] = "Backspace";
        KeyCode[KeyCode["Tab"] = 2] = "Tab";
        KeyCode[KeyCode["Enter"] = 3] = "Enter";
        KeyCode[KeyCode["Shift"] = 4] = "Shift";
        KeyCode[KeyCode["Ctrl"] = 5] = "Ctrl";
        KeyCode[KeyCode["Alt"] = 6] = "Alt";
        KeyCode[KeyCode["PauseBreak"] = 7] = "PauseBreak";
        KeyCode[KeyCode["CapsLock"] = 8] = "CapsLock";
        KeyCode[KeyCode["Escape"] = 9] = "Escape";
        KeyCode[KeyCode["Space"] = 10] = "Space";
        KeyCode[KeyCode["PageUp"] = 11] = "PageUp";
        KeyCode[KeyCode["PageDown"] = 12] = "PageDown";
        KeyCode[KeyCode["End"] = 13] = "End";
        KeyCode[KeyCode["Home"] = 14] = "Home";
        KeyCode[KeyCode["LeftArrow"] = 15] = "LeftArrow";
        KeyCode[KeyCode["UpArrow"] = 16] = "UpArrow";
        KeyCode[KeyCode["RightArrow"] = 17] = "RightArrow";
        KeyCode[KeyCode["DownArrow"] = 18] = "DownArrow";
        KeyCode[KeyCode["Insert"] = 19] = "Insert";
        KeyCode[KeyCode["Delete"] = 20] = "Delete";
        KeyCode[KeyCode["KEY_0"] = 21] = "KEY_0";
        KeyCode[KeyCode["KEY_1"] = 22] = "KEY_1";
        KeyCode[KeyCode["KEY_2"] = 23] = "KEY_2";
        KeyCode[KeyCode["KEY_3"] = 24] = "KEY_3";
        KeyCode[KeyCode["KEY_4"] = 25] = "KEY_4";
        KeyCode[KeyCode["KEY_5"] = 26] = "KEY_5";
        KeyCode[KeyCode["KEY_6"] = 27] = "KEY_6";
        KeyCode[KeyCode["KEY_7"] = 28] = "KEY_7";
        KeyCode[KeyCode["KEY_8"] = 29] = "KEY_8";
        KeyCode[KeyCode["KEY_9"] = 30] = "KEY_9";
        KeyCode[KeyCode["KEY_A"] = 31] = "KEY_A";
        KeyCode[KeyCode["KEY_B"] = 32] = "KEY_B";
        KeyCode[KeyCode["KEY_C"] = 33] = "KEY_C";
        KeyCode[KeyCode["KEY_D"] = 34] = "KEY_D";
        KeyCode[KeyCode["KEY_E"] = 35] = "KEY_E";
        KeyCode[KeyCode["KEY_F"] = 36] = "KEY_F";
        KeyCode[KeyCode["KEY_G"] = 37] = "KEY_G";
        KeyCode[KeyCode["KEY_H"] = 38] = "KEY_H";
        KeyCode[KeyCode["KEY_I"] = 39] = "KEY_I";
        KeyCode[KeyCode["KEY_J"] = 40] = "KEY_J";
        KeyCode[KeyCode["KEY_K"] = 41] = "KEY_K";
        KeyCode[KeyCode["KEY_L"] = 42] = "KEY_L";
        KeyCode[KeyCode["KEY_M"] = 43] = "KEY_M";
        KeyCode[KeyCode["KEY_N"] = 44] = "KEY_N";
        KeyCode[KeyCode["KEY_O"] = 45] = "KEY_O";
        KeyCode[KeyCode["KEY_P"] = 46] = "KEY_P";
        KeyCode[KeyCode["KEY_Q"] = 47] = "KEY_Q";
        KeyCode[KeyCode["KEY_R"] = 48] = "KEY_R";
        KeyCode[KeyCode["KEY_S"] = 49] = "KEY_S";
        KeyCode[KeyCode["KEY_T"] = 50] = "KEY_T";
        KeyCode[KeyCode["KEY_U"] = 51] = "KEY_U";
        KeyCode[KeyCode["KEY_V"] = 52] = "KEY_V";
        KeyCode[KeyCode["KEY_W"] = 53] = "KEY_W";
        KeyCode[KeyCode["KEY_X"] = 54] = "KEY_X";
        KeyCode[KeyCode["KEY_Y"] = 55] = "KEY_Y";
        KeyCode[KeyCode["KEY_Z"] = 56] = "KEY_Z";
        KeyCode[KeyCode["Meta"] = 57] = "Meta";
        KeyCode[KeyCode["ContextMenu"] = 58] = "ContextMenu";
        KeyCode[KeyCode["F1"] = 59] = "F1";
        KeyCode[KeyCode["F2"] = 60] = "F2";
        KeyCode[KeyCode["F3"] = 61] = "F3";
        KeyCode[KeyCode["F4"] = 62] = "F4";
        KeyCode[KeyCode["F5"] = 63] = "F5";
        KeyCode[KeyCode["F6"] = 64] = "F6";
        KeyCode[KeyCode["F7"] = 65] = "F7";
        KeyCode[KeyCode["F8"] = 66] = "F8";
        KeyCode[KeyCode["F9"] = 67] = "F9";
        KeyCode[KeyCode["F10"] = 68] = "F10";
        KeyCode[KeyCode["F11"] = 69] = "F11";
        KeyCode[KeyCode["F12"] = 70] = "F12";
        KeyCode[KeyCode["F13"] = 71] = "F13";
        KeyCode[KeyCode["F14"] = 72] = "F14";
        KeyCode[KeyCode["F15"] = 73] = "F15";
        KeyCode[KeyCode["F16"] = 74] = "F16";
        KeyCode[KeyCode["F17"] = 75] = "F17";
        KeyCode[KeyCode["F18"] = 76] = "F18";
        KeyCode[KeyCode["F19"] = 77] = "F19";
        KeyCode[KeyCode["NumLock"] = 78] = "NumLock";
        KeyCode[KeyCode["ScrollLock"] = 79] = "ScrollLock";
        /**
         * Used for miscellaneous characters; it can vary by keyboard.
         * For the US standard keyboard, the ';:' key
         */
        KeyCode[KeyCode["US_SEMICOLON"] = 80] = "US_SEMICOLON";
        /**
         * For any country/region, the '+' key
         * For the US standard keyboard, the '=+' key
         */
        KeyCode[KeyCode["US_EQUAL"] = 81] = "US_EQUAL";
        /**
         * For any country/region, the ',' key
         * For the US standard keyboard, the ',<' key
         */
        KeyCode[KeyCode["US_COMMA"] = 82] = "US_COMMA";
        /**
         * For any country/region, the '-' key
         * For the US standard keyboard, the '-_' key
         */
        KeyCode[KeyCode["US_MINUS"] = 83] = "US_MINUS";
        /**
         * For any country/region, the '.' key
         * For the US standard keyboard, the '.>' key
         */
        KeyCode[KeyCode["US_DOT"] = 84] = "US_DOT";
        /**
         * Used for miscellaneous characters; it can vary by keyboard.
         * For the US standard keyboard, the '/?' key
         */
        KeyCode[KeyCode["US_SLASH"] = 85] = "US_SLASH";
        /**
         * Used for miscellaneous characters; it can vary by keyboard.
         * For the US standard keyboard, the '`~' key
         */
        KeyCode[KeyCode["US_BACKTICK"] = 86] = "US_BACKTICK";
        /**
         * Used for miscellaneous characters; it can vary by keyboard.
         * For the US standard keyboard, the '[{' key
         */
        KeyCode[KeyCode["US_OPEN_SQUARE_BRACKET"] = 87] = "US_OPEN_SQUARE_BRACKET";
        /**
         * Used for miscellaneous characters; it can vary by keyboard.
         * For the US standard keyboard, the '\|' key
         */
        KeyCode[KeyCode["US_BACKSLASH"] = 88] = "US_BACKSLASH";
        /**
         * Used for miscellaneous characters; it can vary by keyboard.
         * For the US standard keyboard, the ']}' key
         */
        KeyCode[KeyCode["US_CLOSE_SQUARE_BRACKET"] = 89] = "US_CLOSE_SQUARE_BRACKET";
        /**
         * Used for miscellaneous characters; it can vary by keyboard.
         * For the US standard keyboard, the ''"' key
         */
        KeyCode[KeyCode["US_QUOTE"] = 90] = "US_QUOTE";
        /**
         * Used for miscellaneous characters; it can vary by keyboard.
         */
        KeyCode[KeyCode["OEM_8"] = 91] = "OEM_8";
        /**
         * Either the angle bracket key or the backslash key on the RT 102-key keyboard.
         */
        KeyCode[KeyCode["OEM_102"] = 92] = "OEM_102";
        KeyCode[KeyCode["NUMPAD_0"] = 93] = "NUMPAD_0";
        KeyCode[KeyCode["NUMPAD_1"] = 94] = "NUMPAD_1";
        KeyCode[KeyCode["NUMPAD_2"] = 95] = "NUMPAD_2";
        KeyCode[KeyCode["NUMPAD_3"] = 96] = "NUMPAD_3";
        KeyCode[KeyCode["NUMPAD_4"] = 97] = "NUMPAD_4";
        KeyCode[KeyCode["NUMPAD_5"] = 98] = "NUMPAD_5";
        KeyCode[KeyCode["NUMPAD_6"] = 99] = "NUMPAD_6";
        KeyCode[KeyCode["NUMPAD_7"] = 100] = "NUMPAD_7";
        KeyCode[KeyCode["NUMPAD_8"] = 101] = "NUMPAD_8";
        KeyCode[KeyCode["NUMPAD_9"] = 102] = "NUMPAD_9";
        KeyCode[KeyCode["NUMPAD_MULTIPLY"] = 103] = "NUMPAD_MULTIPLY";
        KeyCode[KeyCode["NUMPAD_ADD"] = 104] = "NUMPAD_ADD";
        KeyCode[KeyCode["NUMPAD_SEPARATOR"] = 105] = "NUMPAD_SEPARATOR";
        KeyCode[KeyCode["NUMPAD_SUBTRACT"] = 106] = "NUMPAD_SUBTRACT";
        KeyCode[KeyCode["NUMPAD_DECIMAL"] = 107] = "NUMPAD_DECIMAL";
        KeyCode[KeyCode["NUMPAD_DIVIDE"] = 108] = "NUMPAD_DIVIDE";
        /**
         * Placed last to cover the length of the enum.
         */
        KeyCode[KeyCode["MAX_VALUE"] = 109] = "MAX_VALUE";
    })(exports.KeyCode || (exports.KeyCode = {}));
    var KeyCode = exports.KeyCode;
    var Mapping = (function () {
        function Mapping(fromKeyCode, toKeyCode) {
            this._fromKeyCode = fromKeyCode;
            this._toKeyCode = toKeyCode;
        }
        Mapping.prototype.fromKeyCode = function (keyCode) {
            return this._fromKeyCode[keyCode];
        };
        Mapping.prototype.toKeyCode = function (str) {
            if (this._toKeyCode.hasOwnProperty(str)) {
                return this._toKeyCode[str];
            }
            return KeyCode.Unknown;
        };
        return Mapping;
    }());
    function createMapping(fill1, fill2) {
        var MAP = [];
        fill1(MAP);
        var REVERSE_MAP = {};
        for (var i = 0, len = MAP.length; i < len; i++) {
            if (!MAP[i]) {
                continue;
            }
            REVERSE_MAP[MAP[i]] = i;
        }
        fill2(REVERSE_MAP);
        var FINAL_REVERSE_MAP = {};
        for (var entry in REVERSE_MAP) {
            if (REVERSE_MAP.hasOwnProperty(entry)) {
                FINAL_REVERSE_MAP[entry] = REVERSE_MAP[entry];
                FINAL_REVERSE_MAP[entry.toLowerCase()] = REVERSE_MAP[entry];
            }
        }
        return new Mapping(MAP, FINAL_REVERSE_MAP);
    }
    var STRING = createMapping(function (TO_STRING_MAP) {
        TO_STRING_MAP[KeyCode.Unknown] = 'unknown';
        TO_STRING_MAP[KeyCode.Backspace] = 'Backspace';
        TO_STRING_MAP[KeyCode.Tab] = 'Tab';
        TO_STRING_MAP[KeyCode.Enter] = 'Enter';
        TO_STRING_MAP[KeyCode.Shift] = 'Shift';
        TO_STRING_MAP[KeyCode.Ctrl] = 'Ctrl';
        TO_STRING_MAP[KeyCode.Alt] = 'Alt';
        TO_STRING_MAP[KeyCode.PauseBreak] = 'PauseBreak';
        TO_STRING_MAP[KeyCode.CapsLock] = 'CapsLock';
        TO_STRING_MAP[KeyCode.Escape] = 'Escape';
        TO_STRING_MAP[KeyCode.Space] = 'Space';
        TO_STRING_MAP[KeyCode.PageUp] = 'PageUp';
        TO_STRING_MAP[KeyCode.PageDown] = 'PageDown';
        TO_STRING_MAP[KeyCode.End] = 'End';
        TO_STRING_MAP[KeyCode.Home] = 'Home';
        TO_STRING_MAP[KeyCode.LeftArrow] = 'LeftArrow';
        TO_STRING_MAP[KeyCode.UpArrow] = 'UpArrow';
        TO_STRING_MAP[KeyCode.RightArrow] = 'RightArrow';
        TO_STRING_MAP[KeyCode.DownArrow] = 'DownArrow';
        TO_STRING_MAP[KeyCode.Insert] = 'Insert';
        TO_STRING_MAP[KeyCode.Delete] = 'Delete';
        TO_STRING_MAP[KeyCode.KEY_0] = '0';
        TO_STRING_MAP[KeyCode.KEY_1] = '1';
        TO_STRING_MAP[KeyCode.KEY_2] = '2';
        TO_STRING_MAP[KeyCode.KEY_3] = '3';
        TO_STRING_MAP[KeyCode.KEY_4] = '4';
        TO_STRING_MAP[KeyCode.KEY_5] = '5';
        TO_STRING_MAP[KeyCode.KEY_6] = '6';
        TO_STRING_MAP[KeyCode.KEY_7] = '7';
        TO_STRING_MAP[KeyCode.KEY_8] = '8';
        TO_STRING_MAP[KeyCode.KEY_9] = '9';
        TO_STRING_MAP[KeyCode.KEY_A] = 'A';
        TO_STRING_MAP[KeyCode.KEY_B] = 'B';
        TO_STRING_MAP[KeyCode.KEY_C] = 'C';
        TO_STRING_MAP[KeyCode.KEY_D] = 'D';
        TO_STRING_MAP[KeyCode.KEY_E] = 'E';
        TO_STRING_MAP[KeyCode.KEY_F] = 'F';
        TO_STRING_MAP[KeyCode.KEY_G] = 'G';
        TO_STRING_MAP[KeyCode.KEY_H] = 'H';
        TO_STRING_MAP[KeyCode.KEY_I] = 'I';
        TO_STRING_MAP[KeyCode.KEY_J] = 'J';
        TO_STRING_MAP[KeyCode.KEY_K] = 'K';
        TO_STRING_MAP[KeyCode.KEY_L] = 'L';
        TO_STRING_MAP[KeyCode.KEY_M] = 'M';
        TO_STRING_MAP[KeyCode.KEY_N] = 'N';
        TO_STRING_MAP[KeyCode.KEY_O] = 'O';
        TO_STRING_MAP[KeyCode.KEY_P] = 'P';
        TO_STRING_MAP[KeyCode.KEY_Q] = 'Q';
        TO_STRING_MAP[KeyCode.KEY_R] = 'R';
        TO_STRING_MAP[KeyCode.KEY_S] = 'S';
        TO_STRING_MAP[KeyCode.KEY_T] = 'T';
        TO_STRING_MAP[KeyCode.KEY_U] = 'U';
        TO_STRING_MAP[KeyCode.KEY_V] = 'V';
        TO_STRING_MAP[KeyCode.KEY_W] = 'W';
        TO_STRING_MAP[KeyCode.KEY_X] = 'X';
        TO_STRING_MAP[KeyCode.KEY_Y] = 'Y';
        TO_STRING_MAP[KeyCode.KEY_Z] = 'Z';
        TO_STRING_MAP[KeyCode.ContextMenu] = 'ContextMenu';
        TO_STRING_MAP[KeyCode.F1] = 'F1';
        TO_STRING_MAP[KeyCode.F2] = 'F2';
        TO_STRING_MAP[KeyCode.F3] = 'F3';
        TO_STRING_MAP[KeyCode.F4] = 'F4';
        TO_STRING_MAP[KeyCode.F5] = 'F5';
        TO_STRING_MAP[KeyCode.F6] = 'F6';
        TO_STRING_MAP[KeyCode.F7] = 'F7';
        TO_STRING_MAP[KeyCode.F8] = 'F8';
        TO_STRING_MAP[KeyCode.F9] = 'F9';
        TO_STRING_MAP[KeyCode.F10] = 'F10';
        TO_STRING_MAP[KeyCode.F11] = 'F11';
        TO_STRING_MAP[KeyCode.F12] = 'F12';
        TO_STRING_MAP[KeyCode.F13] = 'F13';
        TO_STRING_MAP[KeyCode.F14] = 'F14';
        TO_STRING_MAP[KeyCode.F15] = 'F15';
        TO_STRING_MAP[KeyCode.F16] = 'F16';
        TO_STRING_MAP[KeyCode.F17] = 'F17';
        TO_STRING_MAP[KeyCode.F18] = 'F18';
        TO_STRING_MAP[KeyCode.F19] = 'F19';
        TO_STRING_MAP[KeyCode.NumLock] = 'NumLock';
        TO_STRING_MAP[KeyCode.ScrollLock] = 'ScrollLock';
        TO_STRING_MAP[KeyCode.US_SEMICOLON] = ';';
        TO_STRING_MAP[KeyCode.US_EQUAL] = '=';
        TO_STRING_MAP[KeyCode.US_COMMA] = ',';
        TO_STRING_MAP[KeyCode.US_MINUS] = '-';
        TO_STRING_MAP[KeyCode.US_DOT] = '.';
        TO_STRING_MAP[KeyCode.US_SLASH] = '/';
        TO_STRING_MAP[KeyCode.US_BACKTICK] = '`';
        TO_STRING_MAP[KeyCode.US_OPEN_SQUARE_BRACKET] = '[';
        TO_STRING_MAP[KeyCode.US_BACKSLASH] = '\\';
        TO_STRING_MAP[KeyCode.US_CLOSE_SQUARE_BRACKET] = ']';
        TO_STRING_MAP[KeyCode.US_QUOTE] = '\'';
        TO_STRING_MAP[KeyCode.OEM_8] = 'OEM_8';
        TO_STRING_MAP[KeyCode.OEM_102] = 'OEM_102';
        TO_STRING_MAP[KeyCode.NUMPAD_0] = 'NumPad0';
        TO_STRING_MAP[KeyCode.NUMPAD_1] = 'NumPad1';
        TO_STRING_MAP[KeyCode.NUMPAD_2] = 'NumPad2';
        TO_STRING_MAP[KeyCode.NUMPAD_3] = 'NumPad3';
        TO_STRING_MAP[KeyCode.NUMPAD_4] = 'NumPad4';
        TO_STRING_MAP[KeyCode.NUMPAD_5] = 'NumPad5';
        TO_STRING_MAP[KeyCode.NUMPAD_6] = 'NumPad6';
        TO_STRING_MAP[KeyCode.NUMPAD_7] = 'NumPad7';
        TO_STRING_MAP[KeyCode.NUMPAD_8] = 'NumPad8';
        TO_STRING_MAP[KeyCode.NUMPAD_9] = 'NumPad9';
        TO_STRING_MAP[KeyCode.NUMPAD_MULTIPLY] = 'NumPad_Multiply';
        TO_STRING_MAP[KeyCode.NUMPAD_ADD] = 'NumPad_Add';
        TO_STRING_MAP[KeyCode.NUMPAD_SEPARATOR] = 'NumPad_Separator';
        TO_STRING_MAP[KeyCode.NUMPAD_SUBTRACT] = 'NumPad_Subtract';
        TO_STRING_MAP[KeyCode.NUMPAD_DECIMAL] = 'NumPad_Decimal';
        TO_STRING_MAP[KeyCode.NUMPAD_DIVIDE] = 'NumPad_Divide';
        // for (let i = 0; i < KeyCode.MAX_VALUE; i++) {
        // 	if (!TO_STRING_MAP[i]) {
        // 		console.warn('Missing string representation for ' + KeyCode[i]);
        // 	}
        // }
    }, function (FROM_STRING_MAP) {
        FROM_STRING_MAP['\r'] = KeyCode.Enter;
    });
    var USER_SETTINGS = createMapping(function (TO_USER_SETTINGS_MAP) {
        for (var i = 0, len = STRING._fromKeyCode.length; i < len; i++) {
            TO_USER_SETTINGS_MAP[i] = STRING._fromKeyCode[i];
        }
        TO_USER_SETTINGS_MAP[KeyCode.LeftArrow] = 'Left';
        TO_USER_SETTINGS_MAP[KeyCode.UpArrow] = 'Up';
        TO_USER_SETTINGS_MAP[KeyCode.RightArrow] = 'Right';
        TO_USER_SETTINGS_MAP[KeyCode.DownArrow] = 'Down';
    }, function (FROM_USER_SETTINGS_MAP) {
        FROM_USER_SETTINGS_MAP['OEM_1'] = KeyCode.US_SEMICOLON;
        FROM_USER_SETTINGS_MAP['OEM_PLUS'] = KeyCode.US_EQUAL;
        FROM_USER_SETTINGS_MAP['OEM_COMMA'] = KeyCode.US_COMMA;
        FROM_USER_SETTINGS_MAP['OEM_MINUS'] = KeyCode.US_MINUS;
        FROM_USER_SETTINGS_MAP['OEM_PERIOD'] = KeyCode.US_DOT;
        FROM_USER_SETTINGS_MAP['OEM_2'] = KeyCode.US_SLASH;
        FROM_USER_SETTINGS_MAP['OEM_3'] = KeyCode.US_BACKTICK;
        FROM_USER_SETTINGS_MAP['OEM_4'] = KeyCode.US_OPEN_SQUARE_BRACKET;
        FROM_USER_SETTINGS_MAP['OEM_5'] = KeyCode.US_BACKSLASH;
        FROM_USER_SETTINGS_MAP['OEM_6'] = KeyCode.US_CLOSE_SQUARE_BRACKET;
        FROM_USER_SETTINGS_MAP['OEM_7'] = KeyCode.US_QUOTE;
        FROM_USER_SETTINGS_MAP['OEM_8'] = KeyCode.OEM_8;
        FROM_USER_SETTINGS_MAP['OEM_102'] = KeyCode.OEM_102;
    });
    var KeyCode;
    (function (KeyCode) {
        function toString(key) {
            return STRING.fromKeyCode(key);
        }
        KeyCode.toString = toString;
        function fromString(key) {
            return STRING.toKeyCode(key);
        }
        KeyCode.fromString = fromString;
    })(KeyCode = exports.KeyCode || (exports.KeyCode = {}));
    // Binary encoding strategy:
    // 15:  1 bit for ctrlCmd
    // 14:  1 bit for shift
    // 13:  1 bit for alt
    // 12:  1 bit for winCtrl
    //  0: 12 bits for keyCode (up to a maximum keyCode of 4096. Given we have 83 at this point thats good enough)
    var BIN_CTRLCMD_MASK = 1 << 15;
    var BIN_SHIFT_MASK = 1 << 14;
    var BIN_ALT_MASK = 1 << 13;
    var BIN_WINCTRL_MASK = 1 << 12;
    var BIN_KEYCODE_MASK = 0x00000fff;
    var BinaryKeybindings = (function () {
        function BinaryKeybindings() {
        }
        BinaryKeybindings.extractFirstPart = function (keybinding) {
            return keybinding & 0x0000ffff;
        };
        BinaryKeybindings.extractChordPart = function (keybinding) {
            return (keybinding >> 16) & 0x0000ffff;
        };
        BinaryKeybindings.hasChord = function (keybinding) {
            return (this.extractChordPart(keybinding) !== 0);
        };
        BinaryKeybindings.hasCtrlCmd = function (keybinding) {
            return (keybinding & BIN_CTRLCMD_MASK ? true : false);
        };
        BinaryKeybindings.hasShift = function (keybinding) {
            return (keybinding & BIN_SHIFT_MASK ? true : false);
        };
        BinaryKeybindings.hasAlt = function (keybinding) {
            return (keybinding & BIN_ALT_MASK ? true : false);
        };
        BinaryKeybindings.hasWinCtrl = function (keybinding) {
            return (keybinding & BIN_WINCTRL_MASK ? true : false);
        };
        BinaryKeybindings.extractKeyCode = function (keybinding) {
            return (keybinding & BIN_KEYCODE_MASK);
        };
        return BinaryKeybindings;
    }());
    exports.BinaryKeybindings = BinaryKeybindings;
    var KeyMod = (function () {
        function KeyMod() {
        }
        KeyMod.chord = function (firstPart, secondPart) {
            return firstPart | ((secondPart & 0x0000ffff) << 16);
        };
        KeyMod.CtrlCmd = BIN_CTRLCMD_MASK;
        KeyMod.Shift = BIN_SHIFT_MASK;
        KeyMod.Alt = BIN_ALT_MASK;
        KeyMod.WinCtrl = BIN_WINCTRL_MASK;
        return KeyMod;
    }());
    exports.KeyMod = KeyMod;
    /**
     * A set of usual keybindings that can be reused in code
     */
    var CommonKeybindings = (function () {
        function CommonKeybindings() {
        }
        CommonKeybindings.ENTER = KeyCode.Enter;
        CommonKeybindings.SHIFT_ENTER = KeyMod.Shift | KeyCode.Enter;
        CommonKeybindings.CTRLCMD_ENTER = KeyMod.CtrlCmd | KeyCode.Enter;
        CommonKeybindings.WINCTRL_ENTER = KeyMod.WinCtrl | KeyCode.Enter;
        CommonKeybindings.TAB = KeyCode.Tab;
        CommonKeybindings.SHIFT_TAB = KeyMod.Shift | KeyCode.Tab;
        CommonKeybindings.ESCAPE = KeyCode.Escape;
        CommonKeybindings.SPACE = KeyCode.Space;
        CommonKeybindings.DELETE = KeyCode.Delete;
        CommonKeybindings.SHIFT_DELETE = KeyMod.Shift | KeyCode.Delete;
        CommonKeybindings.CTRLCMD_BACKSPACE = KeyMod.CtrlCmd | KeyCode.Backspace;
        CommonKeybindings.UP_ARROW = KeyCode.UpArrow;
        CommonKeybindings.SHIFT_UP_ARROW = KeyMod.Shift | KeyCode.UpArrow;
        CommonKeybindings.CTRLCMD_UP_ARROW = KeyMod.CtrlCmd | KeyCode.UpArrow;
        CommonKeybindings.DOWN_ARROW = KeyCode.DownArrow;
        CommonKeybindings.SHIFT_DOWN_ARROW = KeyMod.Shift | KeyCode.DownArrow;
        CommonKeybindings.CTRLCMD_DOWN_ARROW = KeyMod.CtrlCmd | KeyCode.DownArrow;
        CommonKeybindings.LEFT_ARROW = KeyCode.LeftArrow;
        CommonKeybindings.RIGHT_ARROW = KeyCode.RightArrow;
        CommonKeybindings.HOME = KeyCode.Home;
        CommonKeybindings.END = KeyCode.End;
        CommonKeybindings.PAGE_UP = KeyCode.PageUp;
        CommonKeybindings.SHIFT_PAGE_UP = KeyMod.Shift | KeyCode.PageUp;
        CommonKeybindings.PAGE_DOWN = KeyCode.PageDown;
        CommonKeybindings.SHIFT_PAGE_DOWN = KeyMod.Shift | KeyCode.PageDown;
        CommonKeybindings.F2 = KeyCode.F2;
        CommonKeybindings.CTRLCMD_S = KeyMod.CtrlCmd | KeyCode.KEY_S;
        CommonKeybindings.CTRLCMD_C = KeyMod.CtrlCmd | KeyCode.KEY_C;
        CommonKeybindings.CTRLCMD_V = KeyMod.CtrlCmd | KeyCode.KEY_V;
        return CommonKeybindings;
    }());
    exports.CommonKeybindings = CommonKeybindings;
    var Keybinding = (function () {
        function Keybinding(keybinding) {
            this.value = keybinding;
        }
        /**
         * Format the binding to a format appropiate for rendering in the UI
         */
        Keybinding._toUSLabel = function (value, Platform) {
            return _asString(value, (Platform.isMacintosh ? MacUIKeyLabelProvider.INSTANCE : ClassicUIKeyLabelProvider.INSTANCE), Platform);
        };
        /**
         * Format the binding to a format appropiate for placing in an aria-label.
         */
        Keybinding._toUSAriaLabel = function (value, Platform) {
            return _asString(value, AriaKeyLabelProvider.INSTANCE, Platform);
        };
        /**
         * Format the binding to a format appropiate for rendering in the UI
         */
        Keybinding._toUSHTMLLabel = function (value, Platform) {
            return _asHTML(value, (Platform.isMacintosh ? MacUIKeyLabelProvider.INSTANCE : ClassicUIKeyLabelProvider.INSTANCE), Platform);
        };
        /**
         * Format the binding to a format appropiate for rendering in the UI
         */
        Keybinding._toCustomLabel = function (value, labelProvider, Platform) {
            return _asString(value, labelProvider, Platform);
        };
        /**
         * Format the binding to a format appropiate for rendering in the UI
         */
        Keybinding._toCustomHTMLLabel = function (value, labelProvider, Platform) {
            return _asHTML(value, labelProvider, Platform);
        };
        /**
         * This prints the binding in a format suitable for electron's accelerators.
         * See https://github.com/electron/electron/blob/master/docs/api/accelerator.md
         */
        Keybinding._toElectronAccelerator = function (value, Platform) {
            if (BinaryKeybindings.hasChord(value)) {
                // Electron cannot handle chords
                return null;
            }
            return _asString(value, ElectronAcceleratorLabelProvider.INSTANCE, Platform);
        };
        Keybinding.getUserSettingsKeybindingRegex = function () {
            if (!this._cachedKeybindingRegex) {
                var numpadKey = 'numpad(0|1|2|3|4|5|6|7|8|9|_multiply|_add|_subtract|_decimal|_divide|_separator)';
                var oemKey = '`|\\-|=|\\[|\\]|\\\\\\\\|;|\'|,|\\.|\\/|oem_8|oem_102';
                var specialKey = 'left|up|right|down|pageup|pagedown|end|home|tab|enter|escape|space|backspace|delete|pausebreak|capslock|insert|contextmenu|numlock|scrolllock';
                var casualKey = '[a-z]|[0-9]|f(1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19)';
                var key = '((' + [numpadKey, oemKey, specialKey, casualKey].join(')|(') + '))';
                var mod = '((ctrl|shift|alt|cmd|win|meta)\\+)*';
                var keybinding = '(' + mod + key + ')';
                this._cachedKeybindingRegex = '"\\s*(' + keybinding + '(\\s+' + keybinding + ')?' + ')\\s*"';
            }
            return this._cachedKeybindingRegex;
        };
        /**
         * Format the binding to a format appropiate for the user settings file.
         */
        Keybinding.toUserSettingsLabel = function (value, Platform) {
            if (Platform === void 0) { Platform = defaultPlatform; }
            var result = _asString(value, UserSettingsKeyLabelProvider.INSTANCE, Platform);
            result = result.toLowerCase();
            if (Platform.isMacintosh) {
                result = result.replace(/meta/g, 'cmd');
            }
            else if (Platform.isWindows) {
                result = result.replace(/meta/g, 'win');
            }
            return result;
        };
        Keybinding.fromUserSettingsLabel = function (input, Platform) {
            if (Platform === void 0) { Platform = defaultPlatform; }
            if (!input) {
                return null;
            }
            input = input.toLowerCase().trim();
            var ctrlCmd = false, shift = false, alt = false, winCtrl = false, key = '';
            while (/^(ctrl|shift|alt|meta|win|cmd)(\+|\-)/.test(input)) {
                if (/^ctrl(\+|\-)/.test(input)) {
                    if (Platform.isMacintosh) {
                        winCtrl = true;
                    }
                    else {
                        ctrlCmd = true;
                    }
                    input = input.substr('ctrl-'.length);
                }
                if (/^shift(\+|\-)/.test(input)) {
                    shift = true;
                    input = input.substr('shift-'.length);
                }
                if (/^alt(\+|\-)/.test(input)) {
                    alt = true;
                    input = input.substr('alt-'.length);
                }
                if (/^meta(\+|\-)/.test(input)) {
                    if (Platform.isMacintosh) {
                        ctrlCmd = true;
                    }
                    else {
                        winCtrl = true;
                    }
                    input = input.substr('meta-'.length);
                }
                if (/^win(\+|\-)/.test(input)) {
                    if (Platform.isMacintosh) {
                        ctrlCmd = true;
                    }
                    else {
                        winCtrl = true;
                    }
                    input = input.substr('win-'.length);
                }
                if (/^cmd(\+|\-)/.test(input)) {
                    if (Platform.isMacintosh) {
                        ctrlCmd = true;
                    }
                    else {
                        winCtrl = true;
                    }
                    input = input.substr('cmd-'.length);
                }
            }
            var chord = 0;
            var firstSpaceIdx = input.indexOf(' ');
            if (firstSpaceIdx > 0) {
                key = input.substring(0, firstSpaceIdx);
                chord = Keybinding.fromUserSettingsLabel(input.substring(firstSpaceIdx), Platform);
            }
            else {
                key = input;
            }
            var keyCode = USER_SETTINGS.toKeyCode(key);
            var result = 0;
            if (ctrlCmd) {
                result |= KeyMod.CtrlCmd;
            }
            if (shift) {
                result |= KeyMod.Shift;
            }
            if (alt) {
                result |= KeyMod.Alt;
            }
            if (winCtrl) {
                result |= KeyMod.WinCtrl;
            }
            result |= keyCode;
            return KeyMod.chord(result, chord);
        };
        Keybinding.prototype.hasCtrlCmd = function () {
            return BinaryKeybindings.hasCtrlCmd(this.value);
        };
        Keybinding.prototype.hasShift = function () {
            return BinaryKeybindings.hasShift(this.value);
        };
        Keybinding.prototype.hasAlt = function () {
            return BinaryKeybindings.hasAlt(this.value);
        };
        Keybinding.prototype.hasWinCtrl = function () {
            return BinaryKeybindings.hasWinCtrl(this.value);
        };
        Keybinding.prototype.extractKeyCode = function () {
            return BinaryKeybindings.extractKeyCode(this.value);
        };
        /**
         * Format the binding to a format appropiate for rendering in the UI
         */
        Keybinding.prototype._toUSLabel = function (Platform) {
            if (Platform === void 0) { Platform = defaultPlatform; }
            return Keybinding._toUSLabel(this.value, Platform);
        };
        /**
         * Format the binding to a format appropiate for placing in an aria-label.
         */
        Keybinding.prototype._toUSAriaLabel = function (Platform) {
            if (Platform === void 0) { Platform = defaultPlatform; }
            return Keybinding._toUSAriaLabel(this.value, Platform);
        };
        /**
         * Format the binding to a format appropiate for rendering in the UI
         */
        Keybinding.prototype._toUSHTMLLabel = function (Platform) {
            if (Platform === void 0) { Platform = defaultPlatform; }
            return Keybinding._toUSHTMLLabel(this.value, Platform);
        };
        /**
         * Format the binding to a format appropiate for rendering in the UI
         */
        Keybinding.prototype.toCustomLabel = function (labelProvider, Platform) {
            if (Platform === void 0) { Platform = defaultPlatform; }
            return Keybinding._toCustomLabel(this.value, labelProvider, Platform);
        };
        /**
         * Format the binding to a format appropiate for rendering in the UI
         */
        Keybinding.prototype.toCustomHTMLLabel = function (labelProvider, Platform) {
            if (Platform === void 0) { Platform = defaultPlatform; }
            return Keybinding._toCustomHTMLLabel(this.value, labelProvider, Platform);
        };
        /**
         * This prints the binding in a format suitable for electron's accelerators.
         * See https://github.com/electron/electron/blob/master/docs/api/accelerator.md
         */
        Keybinding.prototype._toElectronAccelerator = function (Platform) {
            if (Platform === void 0) { Platform = defaultPlatform; }
            return Keybinding._toElectronAccelerator(this.value, Platform);
        };
        /**
         * Format the binding to a format appropiate for the user settings file.
         */
        Keybinding.prototype.toUserSettingsLabel = function (Platform) {
            if (Platform === void 0) { Platform = defaultPlatform; }
            return Keybinding.toUserSettingsLabel(this.value, Platform);
        };
        Keybinding._cachedKeybindingRegex = null;
        return Keybinding;
    }());
    exports.Keybinding = Keybinding;
    /**
     * Print for Electron
     */
    var ElectronAcceleratorLabelProvider = (function () {
        function ElectronAcceleratorLabelProvider() {
            this.ctrlKeyLabel = 'Ctrl';
            this.shiftKeyLabel = 'Shift';
            this.altKeyLabel = 'Alt';
            this.cmdKeyLabel = 'Cmd';
            this.windowsKeyLabel = 'Super';
            this.modifierSeparator = '+';
        }
        ElectronAcceleratorLabelProvider.prototype.getLabelForKey = function (keyCode) {
            switch (keyCode) {
                case KeyCode.UpArrow:
                    return 'Up';
                case KeyCode.DownArrow:
                    return 'Down';
                case KeyCode.LeftArrow:
                    return 'Left';
                case KeyCode.RightArrow:
                    return 'Right';
            }
            return KeyCode.toString(keyCode);
        };
        ElectronAcceleratorLabelProvider.INSTANCE = new ElectronAcceleratorLabelProvider();
        return ElectronAcceleratorLabelProvider;
    }());
    exports.ElectronAcceleratorLabelProvider = ElectronAcceleratorLabelProvider;
    /**
     * Print for Mac UI
     */
    var MacUIKeyLabelProvider = (function () {
        function MacUIKeyLabelProvider() {
            this.ctrlKeyLabel = '\u2303';
            this.shiftKeyLabel = '\u21E7';
            this.altKeyLabel = '\u2325';
            this.cmdKeyLabel = '\u2318';
            this.windowsKeyLabel = nls.localize(0, null);
            this.modifierSeparator = '';
        }
        MacUIKeyLabelProvider.prototype.getLabelForKey = function (keyCode) {
            switch (keyCode) {
                case KeyCode.LeftArrow:
                    return MacUIKeyLabelProvider.leftArrowUnicodeLabel;
                case KeyCode.UpArrow:
                    return MacUIKeyLabelProvider.upArrowUnicodeLabel;
                case KeyCode.RightArrow:
                    return MacUIKeyLabelProvider.rightArrowUnicodeLabel;
                case KeyCode.DownArrow:
                    return MacUIKeyLabelProvider.downArrowUnicodeLabel;
            }
            return KeyCode.toString(keyCode);
        };
        MacUIKeyLabelProvider.INSTANCE = new MacUIKeyLabelProvider();
        MacUIKeyLabelProvider.leftArrowUnicodeLabel = String.fromCharCode(8592);
        MacUIKeyLabelProvider.upArrowUnicodeLabel = String.fromCharCode(8593);
        MacUIKeyLabelProvider.rightArrowUnicodeLabel = String.fromCharCode(8594);
        MacUIKeyLabelProvider.downArrowUnicodeLabel = String.fromCharCode(8595);
        return MacUIKeyLabelProvider;
    }());
    exports.MacUIKeyLabelProvider = MacUIKeyLabelProvider;
    /**
     * Aria label provider for Mac.
     */
    var AriaKeyLabelProvider = (function () {
        function AriaKeyLabelProvider() {
            this.ctrlKeyLabel = nls.localize(1, null);
            this.shiftKeyLabel = nls.localize(2, null);
            this.altKeyLabel = nls.localize(3, null);
            this.cmdKeyLabel = nls.localize(4, null);
            this.windowsKeyLabel = nls.localize(5, null);
            this.modifierSeparator = '+';
        }
        AriaKeyLabelProvider.prototype.getLabelForKey = function (keyCode) {
            return KeyCode.toString(keyCode);
        };
        AriaKeyLabelProvider.INSTANCE = new MacUIKeyLabelProvider();
        return AriaKeyLabelProvider;
    }());
    exports.AriaKeyLabelProvider = AriaKeyLabelProvider;
    /**
     * Print for Windows, Linux UI
     */
    var ClassicUIKeyLabelProvider = (function () {
        function ClassicUIKeyLabelProvider() {
            this.ctrlKeyLabel = nls.localize(6, null);
            this.shiftKeyLabel = nls.localize(7, null);
            this.altKeyLabel = nls.localize(8, null);
            this.cmdKeyLabel = nls.localize(9, null);
            this.windowsKeyLabel = nls.localize(10, null);
            this.modifierSeparator = '+';
        }
        ClassicUIKeyLabelProvider.prototype.getLabelForKey = function (keyCode) {
            return KeyCode.toString(keyCode);
        };
        ClassicUIKeyLabelProvider.INSTANCE = new ClassicUIKeyLabelProvider();
        return ClassicUIKeyLabelProvider;
    }());
    exports.ClassicUIKeyLabelProvider = ClassicUIKeyLabelProvider;
    /**
     * Print for the user settings file.
     */
    var UserSettingsKeyLabelProvider = (function () {
        function UserSettingsKeyLabelProvider() {
            this.ctrlKeyLabel = 'Ctrl';
            this.shiftKeyLabel = 'Shift';
            this.altKeyLabel = 'Alt';
            this.cmdKeyLabel = 'Meta';
            this.windowsKeyLabel = 'Meta';
            this.modifierSeparator = '+';
        }
        UserSettingsKeyLabelProvider.prototype.getLabelForKey = function (keyCode) {
            return USER_SETTINGS.fromKeyCode(keyCode);
        };
        UserSettingsKeyLabelProvider.INSTANCE = new UserSettingsKeyLabelProvider();
        return UserSettingsKeyLabelProvider;
    }());
    function _asString(keybinding, labelProvider, Platform) {
        var result = [], ctrlCmd = BinaryKeybindings.hasCtrlCmd(keybinding), shift = BinaryKeybindings.hasShift(keybinding), alt = BinaryKeybindings.hasAlt(keybinding), winCtrl = BinaryKeybindings.hasWinCtrl(keybinding), keyCode = BinaryKeybindings.extractKeyCode(keybinding);
        var keyLabel = labelProvider.getLabelForKey(keyCode);
        if (!keyLabel) {
            // cannot trigger this key code under this kb layout
            return '';
        }
        // translate modifier keys: Ctrl-Shift-Alt-Meta
        if ((ctrlCmd && !Platform.isMacintosh) || (winCtrl && Platform.isMacintosh)) {
            result.push(labelProvider.ctrlKeyLabel);
        }
        if (shift) {
            result.push(labelProvider.shiftKeyLabel);
        }
        if (alt) {
            result.push(labelProvider.altKeyLabel);
        }
        if (ctrlCmd && Platform.isMacintosh) {
            result.push(labelProvider.cmdKeyLabel);
        }
        if (winCtrl && !Platform.isMacintosh) {
            result.push(labelProvider.windowsKeyLabel);
        }
        // the actual key
        result.push(keyLabel);
        var actualResult = result.join(labelProvider.modifierSeparator);
        if (BinaryKeybindings.hasChord(keybinding)) {
            return actualResult + ' ' + _asString(BinaryKeybindings.extractChordPart(keybinding), labelProvider, Platform);
        }
        return actualResult;
    }
    function _pushKey(result, str) {
        if (result.length > 0) {
            result.push({
                tagName: 'span',
                text: '+'
            });
        }
        result.push({
            tagName: 'span',
            className: 'monaco-kbkey',
            text: str
        });
    }
    function _asHTML(keybinding, labelProvider, Platform, isChord) {
        if (isChord === void 0) { isChord = false; }
        var result = [], ctrlCmd = BinaryKeybindings.hasCtrlCmd(keybinding), shift = BinaryKeybindings.hasShift(keybinding), alt = BinaryKeybindings.hasAlt(keybinding), winCtrl = BinaryKeybindings.hasWinCtrl(keybinding), keyCode = BinaryKeybindings.extractKeyCode(keybinding);
        var keyLabel = labelProvider.getLabelForKey(keyCode);
        if (!keyLabel) {
            // cannot trigger this key code under this kb layout
            return [];
        }
        // translate modifier keys: Ctrl-Shift-Alt-Meta
        if ((ctrlCmd && !Platform.isMacintosh) || (winCtrl && Platform.isMacintosh)) {
            _pushKey(result, labelProvider.ctrlKeyLabel);
        }
        if (shift) {
            _pushKey(result, labelProvider.shiftKeyLabel);
        }
        if (alt) {
            _pushKey(result, labelProvider.altKeyLabel);
        }
        if (ctrlCmd && Platform.isMacintosh) {
            _pushKey(result, labelProvider.cmdKeyLabel);
        }
        if (winCtrl && !Platform.isMacintosh) {
            _pushKey(result, labelProvider.windowsKeyLabel);
        }
        // the actual key
        _pushKey(result, keyLabel);
        var chordTo = null;
        if (BinaryKeybindings.hasChord(keybinding)) {
            chordTo = _asHTML(BinaryKeybindings.extractChordPart(keybinding), labelProvider, Platform, true);
            result.push({
                tagName: 'span',
                text: ' '
            });
            result = result.concat(chordTo);
        }
        if (isChord) {
            return result;
        }
        return [{
                tagName: 'span',
                className: 'monaco-kb',
                children: result
            }];
    }
});

define(__m[27], __M([16,21]), function(nls, data) { return nls.create("vs/languages/html/common/htmlTags", data); });
/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
/*!
BEGIN THIRD PARTY
*/
/*--------------------------------------------------------------------------------------------
 *  This file is based on or incorporates material from the projects listed below (Third Party IP).
 *  The original copyright notice and the license under which Microsoft received such Third Party IP,
 *  are set forth below. Such licenses and notices are provided for informational purposes only.
 *  Microsoft licenses the Third Party IP to you under the licensing terms for the Microsoft product.
 *  Microsoft reserves all other rights not expressly granted under this agreement, whether by implication,
 *  estoppel or otherwise.
 *--------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------
 *  Copyright © 2015 W3C® (MIT, ERCIM, Keio, Beihang). This software or document includes includes material copied
 *  from or derived from HTML 5.1 W3C Working Draft (http://www.w3.org/TR/2015/WD-html51-20151008/.)"
 *--------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------
 *  Ionic Main Site (https://github.com/driftyco/ionic-site).
 *  Copyright Drifty Co. http://drifty.com/.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 *  except in compliance with the License. You may obtain a copy of the License at
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  THIS CODE IS PROVIDED ON AN *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
 *  WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
 *  MERCHANTABLITY OR NON-INFRINGEMENT.
 *
 *  See the Apache Version 2.0 License for specific language governing permissions
 *  and limitations under the License.
 *--------------------------------------------------------------------------------------------*/
define(__m[30], __M([0,1,11,27]), function (require, exports, strings, nls) {
    "use strict";
    var HTMLTagSpecification = (function () {
        function HTMLTagSpecification(label, attributes) {
            if (attributes === void 0) { attributes = []; }
            this.label = label;
            this.attributes = attributes;
        }
        return HTMLTagSpecification;
    }());
    exports.HTMLTagSpecification = HTMLTagSpecification;
    // HTML tag information sourced from http://www.w3.org/TR/2015/WD-html51-20151008/
    exports.HTML_TAGS = {
        // The root element
        html: new HTMLTagSpecification(nls.localize(0, null), ['manifest']),
        // Document metadata
        head: new HTMLTagSpecification(nls.localize(1, null)),
        title: new HTMLTagSpecification(nls.localize(2, null)),
        base: new HTMLTagSpecification(nls.localize(3, null), ['href', 'target']),
        link: new HTMLTagSpecification(nls.localize(4, null), ['href', 'crossorigin:xo', 'rel', 'media', 'hreflang', 'type', 'sizes']),
        meta: new HTMLTagSpecification(nls.localize(5, null), ['name', 'http-equiv', 'content', 'charset']),
        style: new HTMLTagSpecification(nls.localize(6, null), ['media', 'nonce', 'type', 'scoped:v']),
        // Sections
        body: new HTMLTagSpecification(nls.localize(7, null), ['onafterprint', 'onbeforeprint', 'onbeforeunload', 'onhashchange', 'onlanguagechange', 'onmessage', 'onoffline', 'ononline', 'onpagehide', 'onpageshow', 'onpopstate', 'onstorage', 'onunload']),
        article: new HTMLTagSpecification(nls.localize(8, null)),
        section: new HTMLTagSpecification(nls.localize(9, null)),
        nav: new HTMLTagSpecification(nls.localize(10, null)),
        aside: new HTMLTagSpecification(nls.localize(11, null)),
        h1: new HTMLTagSpecification(nls.localize(12, null)),
        h2: new HTMLTagSpecification(nls.localize(13, null)),
        h3: new HTMLTagSpecification(nls.localize(14, null)),
        h4: new HTMLTagSpecification(nls.localize(15, null)),
        h5: new HTMLTagSpecification(nls.localize(16, null)),
        h6: new HTMLTagSpecification(nls.localize(17, null)),
        header: new HTMLTagSpecification(nls.localize(18, null)),
        footer: new HTMLTagSpecification(nls.localize(19, null)),
        address: new HTMLTagSpecification(nls.localize(20, null)),
        // Grouping content
        p: new HTMLTagSpecification(nls.localize(21, null)),
        hr: new HTMLTagSpecification(nls.localize(22, null)),
        pre: new HTMLTagSpecification(nls.localize(23, null)),
        blockquote: new HTMLTagSpecification(nls.localize(24, null), ['cite']),
        ol: new HTMLTagSpecification(nls.localize(25, null), ['reversed:v', 'start', 'type:lt']),
        ul: new HTMLTagSpecification(nls.localize(26, null)),
        li: new HTMLTagSpecification(nls.localize(27, null), ['value']),
        dl: new HTMLTagSpecification(nls.localize(28, null)),
        dt: new HTMLTagSpecification(nls.localize(29, null)),
        dd: new HTMLTagSpecification(nls.localize(30, null)),
        figure: new HTMLTagSpecification(nls.localize(31, null)),
        figcaption: new HTMLTagSpecification(nls.localize(32, null)),
        main: new HTMLTagSpecification(nls.localize(33, null)),
        div: new HTMLTagSpecification(nls.localize(34, null)),
        // Text-level semantics
        a: new HTMLTagSpecification(nls.localize(35, null), ['href', 'target', 'download', 'ping', 'rel', 'hreflang', 'type']),
        em: new HTMLTagSpecification(nls.localize(36, null)),
        strong: new HTMLTagSpecification(nls.localize(37, null)),
        small: new HTMLTagSpecification(nls.localize(38, null)),
        s: new HTMLTagSpecification(nls.localize(39, null)),
        cite: new HTMLTagSpecification(nls.localize(40, null)),
        q: new HTMLTagSpecification(nls.localize(41, null), ['cite']),
        dfn: new HTMLTagSpecification(nls.localize(42, null)),
        abbr: new HTMLTagSpecification(nls.localize(43, null)),
        ruby: new HTMLTagSpecification(nls.localize(44, null)),
        rb: new HTMLTagSpecification(nls.localize(45, null)),
        rt: new HTMLTagSpecification(nls.localize(46, null)),
        // <rtc> is not yet supported by 2+ browsers
        //rtc: new HTMLTagSpecification(
        //	nls.localize('tags.rtc', 'The rtc element marks a ruby text container for ruby text components in a ruby annotation. When it is the child of a ruby element it doesn\'t represent anything itself, but its parent ruby element uses it as part of determining what it represents.')),
        rp: new HTMLTagSpecification(nls.localize(47, null)),
        // <data> is not yet supported by 2+ browsers
        //data: new HTMLTagSpecification(
        //	nls.localize('tags.data', 'The data element represents its contents, along with a machine-readable form of those contents in the value attribute.')),
        time: new HTMLTagSpecification(nls.localize(48, null), ['datetime']),
        code: new HTMLTagSpecification(nls.localize(49, null)),
        var: new HTMLTagSpecification(nls.localize(50, null)),
        samp: new HTMLTagSpecification(nls.localize(51, null)),
        kbd: new HTMLTagSpecification(nls.localize(52, null)),
        sub: new HTMLTagSpecification(nls.localize(53, null)),
        sup: new HTMLTagSpecification(nls.localize(54, null)),
        i: new HTMLTagSpecification(nls.localize(55, null)),
        b: new HTMLTagSpecification(nls.localize(56, null)),
        u: new HTMLTagSpecification(nls.localize(57, null)),
        mark: new HTMLTagSpecification(nls.localize(58, null)),
        bdi: new HTMLTagSpecification(nls.localize(59, null)),
        bdo: new HTMLTagSpecification(nls.localize(60, null)),
        span: new HTMLTagSpecification(nls.localize(61, null)),
        br: new HTMLTagSpecification(nls.localize(62, null)),
        wbr: new HTMLTagSpecification(nls.localize(63, null)),
        // Edits
        ins: new HTMLTagSpecification(nls.localize(64, null)),
        del: new HTMLTagSpecification(nls.localize(65, null), ['cite', 'datetime']),
        // Embedded content
        picture: new HTMLTagSpecification(nls.localize(66, null)),
        img: new HTMLTagSpecification(nls.localize(67, null), ['alt', 'src', 'srcset', 'crossorigin:xo', 'usemap', 'ismap:v', 'width', 'height']),
        iframe: new HTMLTagSpecification(nls.localize(68, null), ['src', 'srcdoc', 'name', 'sandbox:sb', 'seamless:v', 'allowfullscreen:v', 'width', 'height']),
        embed: new HTMLTagSpecification(nls.localize(69, null), ['src', 'type', 'width', 'height']),
        object: new HTMLTagSpecification(nls.localize(70, null), ['data', 'type', 'typemustmatch:v', 'name', 'usemap', 'form', 'width', 'height']),
        param: new HTMLTagSpecification(nls.localize(71, null), ['name', 'value']),
        video: new HTMLTagSpecification(nls.localize(72, null), ['src', 'crossorigin:xo', 'poster', 'preload:pl', 'autoplay:v', 'mediagroup', 'loop:v', 'muted:v', 'controls:v', 'width', 'height']),
        audio: new HTMLTagSpecification(nls.localize(73, null), ['src', 'crossorigin:xo', 'preload:pl', 'autoplay:v', 'mediagroup', 'loop:v', 'muted:v', 'controls:v']),
        source: new HTMLTagSpecification(nls.localize(74, null), 
        // 'When the source element has a parent that is a picture element, the source element allows authors to specify multiple alternative source sets for img elements.'
        ['src', 'type']),
        track: new HTMLTagSpecification(nls.localize(75, null), ['default:v', 'kind:tk', 'label', 'src', 'srclang']),
        map: new HTMLTagSpecification(nls.localize(76, null), ['name']),
        area: new HTMLTagSpecification(nls.localize(77, null), ['alt', 'coords', 'shape:sh', 'href', 'target', 'download', 'ping', 'rel', 'hreflang', 'type']),
        // Tabular data
        table: new HTMLTagSpecification(nls.localize(78, null), ['sortable:v', 'border']),
        caption: new HTMLTagSpecification(nls.localize(79, null)),
        colgroup: new HTMLTagSpecification(nls.localize(80, null), ['span']),
        col: new HTMLTagSpecification(nls.localize(81, null), ['span']),
        tbody: new HTMLTagSpecification(nls.localize(82, null)),
        thead: new HTMLTagSpecification(nls.localize(83, null)),
        tfoot: new HTMLTagSpecification(nls.localize(84, null)),
        tr: new HTMLTagSpecification(nls.localize(85, null)),
        td: new HTMLTagSpecification(nls.localize(86, null), ['colspan', 'rowspan', 'headers']),
        th: new HTMLTagSpecification(nls.localize(87, null), ['colspan', 'rowspan', 'headers', 'scope:s', 'sorted', 'abbr']),
        // Forms
        form: new HTMLTagSpecification(nls.localize(88, null), ['accept-charset', 'action', 'autocomplete:o', 'enctype:et', 'method:m', 'name', 'novalidate:v', 'target']),
        label: new HTMLTagSpecification(nls.localize(89, null), ['form', 'for']),
        input: new HTMLTagSpecification(nls.localize(90, null), ['accept', 'alt', 'autocomplete:inputautocomplete', 'autofocus:v', 'checked:v', 'dirname', 'disabled:v', 'form', 'formaction', 'formenctype:et', 'formmethod:fm', 'formnovalidate:v', 'formtarget', 'height', 'inputmode:im', 'list', 'max', 'maxlength', 'min', 'minlength', 'multiple:v', 'name', 'pattern', 'placeholder', 'readonly:v', 'required:v', 'size', 'src', 'step', 'type:t', 'value', 'width']),
        button: new HTMLTagSpecification(nls.localize(91, null), ['autofocus:v', 'disabled:v', 'form', 'formaction', 'formenctype:et', 'formmethod:fm', 'formnovalidate:v', 'formtarget', 'name', 'type:bt', 'value']),
        select: new HTMLTagSpecification(nls.localize(92, null), ['autocomplete:inputautocomplete', 'autofocus:v', 'disabled:v', 'form', 'multiple:v', 'name', 'required:v', 'size']),
        datalist: new HTMLTagSpecification(nls.localize(93, null)),
        optgroup: new HTMLTagSpecification(nls.localize(94, null), ['disabled:v', 'label']),
        option: new HTMLTagSpecification(nls.localize(95, null), ['disabled:v', 'label', 'selected:v', 'value']),
        textarea: new HTMLTagSpecification(nls.localize(96, null), ['autocomplete:inputautocomplete', 'autofocus:v', 'cols', 'dirname', 'disabled:v', 'form', 'inputmode:im', 'maxlength', 'minlength', 'name', 'placeholder', 'readonly:v', 'required:v', 'rows', 'wrap:w']),
        output: new HTMLTagSpecification(nls.localize(97, null), ['for', 'form', 'name']),
        progress: new HTMLTagSpecification(nls.localize(98, null), ['value', 'max']),
        meter: new HTMLTagSpecification(nls.localize(99, null), ['value', 'min', 'max', 'low', 'high', 'optimum']),
        fieldset: new HTMLTagSpecification(nls.localize(100, null), ['disabled:v', 'form', 'name']),
        legend: new HTMLTagSpecification(nls.localize(101, null)),
        // Interactive elements
        details: new HTMLTagSpecification(nls.localize(102, null), ['open:v']),
        summary: new HTMLTagSpecification(nls.localize(103, null)),
        // <menu> and <menuitem> are not yet supported by 2+ browsers
        //menu: new HTMLTagSpecification(
        //	nls.localize('tags.menu', 'The menu element represents a list of commands.'),
        //	['type:mt', 'label']),
        //menuitem: new HTMLTagSpecification(
        //	nls.localize('tags.menuitem', 'The menuitem element represents a command that the user can invoke from a popup menu (either a context menu or the menu of a menu button).')),
        dialog: new HTMLTagSpecification(nls.localize(104, null)),
        // Scripting
        script: new HTMLTagSpecification(nls.localize(105, null), ['src', 'type', 'charset', 'async:v', 'defer:v', 'crossorigin:xo', 'nonce']),
        noscript: new HTMLTagSpecification(nls.localize(106, null)),
        template: new HTMLTagSpecification(nls.localize(107, null)),
        canvas: new HTMLTagSpecification(nls.localize(108, null), ['width', 'height'])
    };
    // Ionic tag information sourced from Ionic main website (https://github.com/driftyco/ionic-site)
    exports.IONIC_TAGS = {
        'ion-checkbox': new HTMLTagSpecification(nls.localize(109, null), ['name', 'ng-false-value', 'ng-model', 'ng-true-value']),
        'ion-content': new HTMLTagSpecification(nls.localize(110, null), ['delegate-handle', 'direction:scrolldir', 'has-bouncing:b', 'locking:b', 'on-scroll', 'on-scroll-complete', 'overflow-scroll:b', 'padding:b', 'scroll:b', 'scrollbar-x:b', 'scrollbar-y:b', 'start-x', 'start-y']),
        'ion-delete-button': new HTMLTagSpecification(nls.localize(111, null), []),
        'ion-footer-bar': new HTMLTagSpecification(nls.localize(112, null), ['align-title:align', 'keyboard-attach:v']),
        'ion-header-bar': new HTMLTagSpecification(nls.localize(113, null), ['align-title:align', 'no-tap-scroll:b']),
        'ion-infinite-scroll': new HTMLTagSpecification(nls.localize(114, null), ['distance', 'icon', 'immediate-check:b', 'on-infinite', 'spinner']),
        'ion-input': new HTMLTagSpecification(nls.localize(115, null), ['type:inputtype', 'clearInput:v']),
        'ion-item': new HTMLTagSpecification(nls.localize(116, null), []),
        'ion-list': new HTMLTagSpecification(nls.localize(117, null), ['can-swipe:b', 'delegate-handle', 'show-delete:b', 'show-reorder:b', 'type:listtype']),
        'ion-modal-view': new HTMLTagSpecification(nls.localize(118, null), []),
        'ion-nav-back-button': new HTMLTagSpecification(nls.localize(119, null), []),
        'ion-nav-bar': new HTMLTagSpecification(nls.localize(120, null), ['align-title:align', 'delegate-handle', 'no-tap-scroll:b']),
        'ion-nav-buttons': new HTMLTagSpecification(nls.localize(121, null), ['side:navsides']),
        'ion-nav-title': new HTMLTagSpecification(nls.localize(122, null), []),
        'ion-nav-view': new HTMLTagSpecification(nls.localize(123, null), ['name']),
        'ion-option-button': new HTMLTagSpecification(nls.localize(124, null), []),
        'ion-pane': new HTMLTagSpecification(nls.localize(125, null), []),
        'ion-popover-view': new HTMLTagSpecification(nls.localize(126, null), []),
        'ion-radio': new HTMLTagSpecification(nls.localize(127, null), ['disabled:b', 'icon', 'name', 'ng-disabled:b', 'ng-model', 'ng-value', 'value']),
        'ion-refresher': new HTMLTagSpecification(nls.localize(128, null), ['disable-pulling-rotation:b', 'on-pulling', 'on-refresh', 'pulling-icon', 'pulling-text', 'refreshing-icon', 'spinner']),
        'ion-reorder-button': new HTMLTagSpecification(nls.localize(129, null), ['on-reorder']),
        'ion-scroll': new HTMLTagSpecification(nls.localize(130, null), ['delegate-handle', 'direction:scrolldir', 'has-bouncing:b', 'locking:b', 'max-zoom', 'min-zoom', 'on-refresh', 'on-scroll', 'paging:b', 'scrollbar-x:b', 'scrollbar-y:b', 'zooming:b']),
        'ion-side-menu': new HTMLTagSpecification(nls.localize(131, null), ['is-enabled:b', 'expose-aside-when', 'side:navsides', 'width']),
        'ion-side-menu-content': new HTMLTagSpecification(nls.localize(132, null), ['drag-content:b', 'edge-drag-threshold']),
        'ion-side-menus': new HTMLTagSpecification(nls.localize(133, null), ['delegate-handle', 'enable-menu-with-back-views:b']),
        'ion-slide': new HTMLTagSpecification(nls.localize(134, null), []),
        'ion-slide-box': new HTMLTagSpecification(nls.localize(135, null), ['active-slide', 'auto-play:b', 'delegate-handle', 'does-continue:b', 'on-slide-changed', 'pager-click', 'show-pager:b', 'slide-interval']),
        'ion-spinner': new HTMLTagSpecification(nls.localize(136, null), ['icon']),
        'ion-tab': new HTMLTagSpecification(nls.localize(137, null), ['badge', 'badge-style', 'disabled', 'hidden', 'href', 'icon', 'icon-off', 'icon-on', 'ng-click', 'on-deselect', 'on-select', 'title']),
        'ion-tabs': new HTMLTagSpecification(nls.localize(138, null), ['delegate-handle']),
        'ion-title': new HTMLTagSpecification(nls.localize(139, null), []),
        'ion-toggle': new HTMLTagSpecification(nls.localize(140, null), ['name', 'ng-false-value', 'ng-model', 'ng-true-value', 'toggle-class']),
        'ion-view ': new HTMLTagSpecification(nls.localize(141, null), ['cache-view:b', 'can-swipe-back:b', 'hide-back-button:b', 'hide-nav-bar:b', 'view-title'])
    };
    function getHTML5TagProvider() {
        var globalAttributes = [
            'aria-activedescendant', 'aria-atomic:b', 'aria-autocomplete:autocomplete', 'aria-busy:b', 'aria-checked:tristate', 'aria-colcount', 'aria-colindex', 'aria-colspan', 'aria-controls', 'aria-current:current', 'aria-describedat',
            'aria-describedby', 'aria-disabled:b', 'aria-dropeffect:dropeffect', 'aria-errormessage', 'aria-expanded:u', 'aria-flowto', 'aria-grabbed:u', 'aria-haspopup:b', 'aria-hidden:b', 'aria-invalid:invalid', 'aria-kbdshortcuts',
            'aria-label', 'aria-labelledby', 'aria-level', 'aria-live:live', 'aria-modal:b', 'aria-multiline:b', 'aria-multiselectable:b', 'aria-orientation:orientation', 'aria-owns', 'aria-placeholder', 'aria-posinset', 'aria-pressed:tristate',
            'aria-readonly:b', 'aria-relevant:relevant', 'aria-required:b', 'aria-roledescription', 'aria-rowcount', 'aria-rowindex', 'aria-rowspan', 'aria-selected:u', 'aria-setsize', 'aria-sort:sort', 'aria-valuemax', 'aria-valuemin', 'aria-valuenow', 'aria-valuetext',
            'accesskey', 'class', 'contenteditable:b', 'contextmenu', 'dir:d', 'draggable:b', 'dropzone', 'hidden:v', 'id', 'itemid', 'itemprop', 'itemref', 'itemscope:v', 'itemtype', 'lang', 'role:roles', 'spellcheck:b', 'style', 'tabindex',
            'title', 'translate:y'];
        var eventHandlers = ['onabort', 'onblur', 'oncanplay', 'oncanplaythrough', 'onchange', 'onclick', 'oncontextmenu', 'ondblclick', 'ondrag', 'ondragend', 'ondragenter', 'ondragleave', 'ondragover', 'ondragstart',
            'ondrop', 'ondurationchange', 'onemptied', 'onended', 'onerror', 'onfocus', 'onformchange', 'onforminput', 'oninput', 'oninvalid', 'onkeydown', 'onkeypress', 'onkeyup', 'onload', 'onloadeddata', 'onloadedmetadata',
            'onloadstart', 'onmousedown', 'onmousemove', 'onmouseout', 'onmouseover', 'onmouseup', 'onmousewheel', 'onpause', 'onplay', 'onplaying', 'onprogress', 'onratechange', 'onreset', 'onresize', 'onreadystatechange', 'onscroll',
            'onseeked', 'onseeking', 'onselect', 'onshow', 'onstalled', 'onsubmit', 'onsuspend', 'ontimeupdate', 'onvolumechange', 'onwaiting'];
        var valueSets = {
            b: ['true', 'false'],
            u: ['true', 'false', 'undefined'],
            o: ['on', 'off'],
            y: ['yes', 'no'],
            w: ['soft', 'hard'],
            d: ['ltr', 'rtl', 'auto'],
            m: ['GET', 'POST', 'dialog'],
            fm: ['GET', 'POST'],
            s: ['row', 'col', 'rowgroup', 'colgroup'],
            t: ['hidden', 'text', 'search', 'tel', 'url', 'email', 'password', 'datetime', 'date', 'month', 'week', 'time', 'datetime-local', 'number', 'range', 'color', 'checkbox', 'radio', 'file', 'submit', 'image', 'reset', 'button'],
            im: ['verbatim', 'latin', 'latin-name', 'latin-prose', 'full-width-latin', 'kana', 'kana-name', 'katakana', 'numeric', 'tel', 'email', 'url'],
            bt: ['button', 'submit', 'reset', 'menu'],
            lt: ['1', 'a', 'A', 'i', 'I'],
            mt: ['context', 'toolbar'],
            mit: ['command', 'checkbox', 'radio'],
            et: ['application/x-www-form-urlencoded', 'multipart/form-data', 'text/plain'],
            tk: ['subtitles', 'captions', 'descriptions', 'chapters', 'metadata'],
            pl: ['none', 'metadata', 'auto'],
            sh: ['circle', 'default', 'poly', 'rect'],
            xo: ['anonymous', 'use-credentials'],
            sb: ['allow-forms', 'allow-modals', 'allow-pointer-lock', 'allow-popups', 'allow-popups-to-escape-sandbox', 'allow-same-origin', 'allow-scripts', 'allow-top-navigation'],
            tristate: ['true', 'false', 'mixed', 'undefined'],
            inputautocomplete: ['additional-name', 'address-level1', 'address-level2', 'address-level3', 'address-level4', 'address-line1', 'address-line2', 'address-line3', 'bday', 'bday-year', 'bday-day', 'bday-month', 'billing', 'cc-additional-name', 'cc-csc', 'cc-exp', 'cc-exp-month', 'cc-exp-year', 'cc-family-name', 'cc-given-name', 'cc-name', 'cc-number', 'cc-type', 'country', 'country-name', 'current-password', 'email', 'family-name', 'fax', 'given-name', 'home', 'honorific-prefix', 'honorific-suffix', 'impp', 'language', 'mobile', 'name', 'new-password', 'nickname', 'organization', 'organization-title', 'pager', 'photo', 'postal-code', 'sex', 'shipping', 'street-address', 'tel-area-code', 'tel', 'tel-country-code', 'tel-extension', 'tel-local', 'tel-local-prefix', 'tel-local-suffix', 'tel-national', 'transaction-amount', 'transaction-currency', 'url', 'username', 'work'],
            autocomplete: ['inline', 'list', 'both', 'none'],
            current: ['page', 'step', 'location', 'date', 'time', 'true', 'false'],
            dropeffect: ['copy', 'move', 'link', 'execute', 'popup', 'none'],
            invalid: ['grammar', 'false', 'spelling', 'true'],
            live: ['off', 'polite', 'assertive'],
            orientation: ['vertical', 'horizontal', 'undefined'],
            relevant: ['additions', 'removals', 'text', 'all', 'additions text'],
            sort: ['ascending', 'descending', 'none', 'other'],
            roles: ['alert', 'alertdialog', 'button', 'checkbox', 'dialog', 'gridcell', 'link', 'log', 'marquee', 'menuitem', 'menuitemcheckbox', 'menuitemradio', 'option', 'progressbar', 'radio', 'scrollbar', 'searchbox', 'slider',
                'spinbutton', 'status', 'switch', 'tab', 'tabpanel', 'textbox', 'timer', 'tooltip', 'treeitem', 'combobox', 'grid', 'listbox', 'menu', 'menubar', 'radiogroup', 'tablist', 'tree', 'treegrid',
                'application', 'article', 'cell', 'columnheader', 'definition', 'directory', 'document', 'feed', 'figure', 'group', 'heading', 'img', 'list', 'listitem', 'math', 'none', 'note', 'presentation', 'region', 'row', 'rowgroup',
                'rowheader', 'separator', 'table', 'term', 'text', 'toolbar',
                'banner', 'complementary', 'contentinfo', 'form', 'main', 'navigation', 'region', 'search']
        };
        return {
            collectTags: function (collector) { return collectTagsDefault(collector, exports.HTML_TAGS); },
            collectAttributes: function (tag, collector) {
                collectAttributesDefault(tag, collector, exports.HTML_TAGS, globalAttributes);
                eventHandlers.forEach(function (handler) {
                    collector(handler, 'event');
                });
            },
            collectValues: function (tag, attribute, collector) { return collectValuesDefault(tag, attribute, collector, exports.HTML_TAGS, globalAttributes, valueSets); }
        };
    }
    exports.getHTML5TagProvider = getHTML5TagProvider;
    function getAngularTagProvider() {
        var customTags = {
            input: ['ng-model', 'ng-required', 'ng-minlength', 'ng-maxlength', 'ng-pattern', 'ng-trim'],
            select: ['ng-model'],
            textarea: ['ng-model', 'ng-required', 'ng-minlength', 'ng-maxlength', 'ng-pattern', 'ng-trim']
        };
        var globalAttributes = ['ng-app', 'ng-bind', 'ng-bind-html', 'ng-bind-template', 'ng-blur', 'ng-change', 'ng-checked', 'ng-class', 'ng-class-even', 'ng-class-odd',
            'ng-click', 'ng-cloak', 'ng-controller', 'ng-copy', 'ng-csp', 'ng-cut', 'ng-dblclick', 'ng-disabled', 'ng-focus', 'ng-form', 'ng-hide', 'ng-href', 'ng-if',
            'ng-include', 'ng-init', 'ng-jq', 'ng-keydown', 'ng-keypress', 'ng-keyup', 'ng-list', 'ng-model-options', 'ng-mousedown', 'ng-mouseenter', 'ng-mouseleave',
            'ng-mousemove', 'ng-mouseover', 'ng-mouseup', 'ng-non-bindable', 'ng-open', 'ng-options', 'ng-paste', 'ng-pluralize', 'ng-readonly', 'ng-repeat', 'ng-selected',
            'ng-show', 'ng-src', 'ng-srcset', 'ng-style', 'ng-submit', 'ng-switch', 'ng-transclude', 'ng-value'
        ];
        return {
            collectTags: function (collector) {
                // no extra tags
            },
            collectAttributes: function (tag, collector) {
                if (tag) {
                    var attributes = customTags[tag];
                    if (attributes) {
                        attributes.forEach(function (a) {
                            collector(a, null);
                            collector('data-' + a, null);
                        });
                    }
                }
                globalAttributes.forEach(function (a) {
                    collector(a, null);
                    collector('data-' + a, null);
                });
            },
            collectValues: function (tag, attribute, collector) {
                // no values
            }
        };
    }
    exports.getAngularTagProvider = getAngularTagProvider;
    function getIonicTagProvider() {
        var customTags = {
            a: ['nav-direction:navdir', 'nav-transition:trans'],
            button: ['menu-toggle:menusides']
        };
        var globalAttributes = ['collection-repeat', 'force-refresh-images:b', 'ion-stop-event', 'item-height', 'item-render-buffer', 'item-width', 'menu-close:v',
            'on-double-tap', 'on-drag', 'on-drag-down', 'on-drag-left', 'on-drag-right', 'on-drag-up', 'on-hold', 'on-release', 'on-swipe', 'on-swipe-down', 'on-swipe-left',
            'on-swipe-right', 'on-swipe-up', 'on-tap', 'on-touch'];
        var valueSets = {
            align: ['center', 'left', 'right'],
            b: ['true', 'false'],
            inputtype: ['email', 'number', 'password', 'search', 'tel', 'text', 'url'],
            listtype: ['card', 'list-inset'],
            menusides: ['left', 'right'],
            navdir: ['back', 'enter', 'exit', 'forward', 'swap'],
            navsides: ['left', 'primary', 'right', 'secondary'],
            scrolldir: ['x', 'xy', 'y'],
            trans: ['android', 'ios', 'none']
        };
        return {
            collectTags: function (collector) { return collectTagsDefault(collector, exports.IONIC_TAGS); },
            collectAttributes: function (tag, collector) {
                collectAttributesDefault(tag, collector, exports.IONIC_TAGS, globalAttributes);
                if (tag) {
                    var attributes = customTags[tag];
                    if (attributes) {
                        attributes.forEach(function (a) {
                            var segments = a.split(':');
                            collector(segments[0], segments[1]);
                        });
                    }
                }
            },
            collectValues: function (tag, attribute, collector) { return collectValuesDefault(tag, attribute, collector, exports.IONIC_TAGS, globalAttributes, valueSets, customTags); }
        };
    }
    exports.getIonicTagProvider = getIonicTagProvider;
    function collectTagsDefault(collector, tagSet) {
        for (var tag in tagSet) {
            collector(tag, tagSet[tag].label);
        }
    }
    function collectAttributesDefault(tag, collector, tagSet, globalAttributes) {
        globalAttributes.forEach(function (attr) {
            var segments = attr.split(':');
            collector(segments[0], segments[1]);
        });
        if (tag) {
            var tags = tagSet[tag];
            if (tags) {
                var attributes = tags.attributes;
                if (attributes) {
                    attributes.forEach(function (attr) {
                        var segments = attr.split(':');
                        collector(segments[0], segments[1]);
                    });
                }
            }
        }
    }
    function collectValuesDefault(tag, attribute, collector, tagSet, globalAttributes, valueSets, customTags) {
        var prefix = attribute + ':';
        var processAttributes = function (attributes) {
            attributes.forEach(function (attr) {
                if (attr.length > prefix.length && strings.startsWith(attr, prefix)) {
                    var typeInfo = attr.substr(prefix.length);
                    if (typeInfo === 'v') {
                        collector(attribute);
                    }
                    else {
                        var values = valueSets[typeInfo];
                        if (values) {
                            values.forEach(collector);
                        }
                    }
                }
            });
        };
        if (tag) {
            var tags = tagSet[tag];
            if (tags) {
                var attributes = tags.attributes;
                if (attributes) {
                    processAttributes(attributes);
                }
            }
        }
        processAttributes(globalAttributes);
        if (customTags) {
            var customTagAttributes = customTags[tag];
            if (customTagAttributes) {
                processAttributes(customTagAttributes);
            }
        }
    }
});
/*!
END THIRD PARTY
*/ 

define(__m[12], __M([0,1,7]), function (require, exports, instantiation_1) {
    /*---------------------------------------------------------------------------------------------
     *  Copyright (c) Microsoft Corporation. All rights reserved.
     *  Licensed under the MIT License. See License.txt in the project root for license information.
     *--------------------------------------------------------------------------------------------*/
    'use strict';
    exports.ID_CODE_EDITOR_SERVICE = 'codeEditorService';
    exports.ICodeEditorService = instantiation_1.createDecorator(exports.ID_CODE_EDITOR_SERVICE);
});

define(__m[14], __M([0,1,7]), function (require, exports, instantiation_1) {
    /*---------------------------------------------------------------------------------------------
     *  Copyright (c) Microsoft Corporation. All rights reserved.
     *  Licensed under the MIT License. See License.txt in the project root for license information.
     *--------------------------------------------------------------------------------------------*/
    'use strict';
    exports.IEditorService = instantiation_1.createDecorator('editorService');
    /**
     * Possible locations for opening an editor.
     */
    (function (Position) {
        /** Opens the editor in the LEFT most position replacing the input currently showing */
        Position[Position["LEFT"] = 0] = "LEFT";
        /** Opens the editor in the CENTER position replacing the input currently showing */
        Position[Position["CENTER"] = 1] = "CENTER";
        /** Opens the editor in the RIGHT most position replacing the input currently showing */
        Position[Position["RIGHT"] = 2] = "RIGHT";
    })(exports.Position || (exports.Position = {}));
    var Position = exports.Position;
    exports.POSITIONS = [Position.LEFT, Position.CENTER, Position.RIGHT];
    (function (Direction) {
        Direction[Direction["LEFT"] = 0] = "LEFT";
        Direction[Direction["RIGHT"] = 1] = "RIGHT";
    })(exports.Direction || (exports.Direction = {}));
    var Direction = exports.Direction;
});

define(__m[6], __M([0,1,7]), function (require, exports, instantiation_1) {
    /*---------------------------------------------------------------------------------------------
     *  Copyright (c) Microsoft Corporation. All rights reserved.
     *  Licensed under the MIT License. See License.txt in the project root for license information.
     *--------------------------------------------------------------------------------------------*/
    'use strict';
    (function (KbExprType) {
        KbExprType[KbExprType["KbDefinedExpression"] = 1] = "KbDefinedExpression";
        KbExprType[KbExprType["KbNotExpression"] = 2] = "KbNotExpression";
        KbExprType[KbExprType["KbEqualsExpression"] = 3] = "KbEqualsExpression";
        KbExprType[KbExprType["KbNotEqualsExpression"] = 4] = "KbNotEqualsExpression";
        KbExprType[KbExprType["KbAndExpression"] = 5] = "KbAndExpression";
    })(exports.KbExprType || (exports.KbExprType = {}));
    var KbExprType = exports.KbExprType;
    function cmp(a, b) {
        var aType = a.getType();
        var bType = b.getType();
        if (aType !== bType) {
            return aType - bType;
        }
        switch (aType) {
            case KbExprType.KbDefinedExpression:
                return a.cmp(b);
            case KbExprType.KbNotExpression:
                return a.cmp(b);
            case KbExprType.KbEqualsExpression:
                return a.cmp(b);
            case KbExprType.KbNotEqualsExpression:
                return a.cmp(b);
            default:
                throw new Error('Unknown KbExpr!');
        }
    }
    var KbDefinedExpression = (function () {
        function KbDefinedExpression(key) {
            this.key = key;
        }
        KbDefinedExpression.prototype.getType = function () {
            return KbExprType.KbDefinedExpression;
        };
        KbDefinedExpression.prototype.cmp = function (other) {
            if (this.key < other.key) {
                return -1;
            }
            if (this.key > other.key) {
                return 1;
            }
            return 0;
        };
        KbDefinedExpression.prototype.equals = function (other) {
            if (other instanceof KbDefinedExpression) {
                return (this.key === other.key);
            }
            return false;
        };
        KbDefinedExpression.prototype.evaluate = function (context) {
            return (!!context[this.key]);
        };
        KbDefinedExpression.prototype.normalize = function () {
            return this;
        };
        KbDefinedExpression.prototype.serialize = function () {
            return this.key;
        };
        return KbDefinedExpression;
    }());
    exports.KbDefinedExpression = KbDefinedExpression;
    var KbEqualsExpression = (function () {
        function KbEqualsExpression(key, value) {
            this.key = key;
            this.value = value;
        }
        KbEqualsExpression.prototype.getType = function () {
            return KbExprType.KbEqualsExpression;
        };
        KbEqualsExpression.prototype.cmp = function (other) {
            if (this.key < other.key) {
                return -1;
            }
            if (this.key > other.key) {
                return 1;
            }
            if (this.value < other.value) {
                return -1;
            }
            if (this.value > other.value) {
                return 1;
            }
            return 0;
        };
        KbEqualsExpression.prototype.equals = function (other) {
            if (other instanceof KbEqualsExpression) {
                return (this.key === other.key && this.value === other.value);
            }
            return false;
        };
        KbEqualsExpression.prototype.evaluate = function (context) {
            /* tslint:disable:triple-equals */
            // Intentional ==
            return (context[this.key] == this.value);
            /* tslint:enable:triple-equals */
        };
        KbEqualsExpression.prototype.normalize = function () {
            if (typeof this.value === 'boolean') {
                if (this.value) {
                    return new KbDefinedExpression(this.key);
                }
                return new KbNotExpression(this.key);
            }
            return this;
        };
        KbEqualsExpression.prototype.serialize = function () {
            if (typeof this.value === 'boolean') {
                return this.normalize().serialize();
            }
            return this.key + ' == \'' + this.value + '\'';
        };
        return KbEqualsExpression;
    }());
    exports.KbEqualsExpression = KbEqualsExpression;
    var KbNotEqualsExpression = (function () {
        function KbNotEqualsExpression(key, value) {
            this.key = key;
            this.value = value;
        }
        KbNotEqualsExpression.prototype.getType = function () {
            return KbExprType.KbNotEqualsExpression;
        };
        KbNotEqualsExpression.prototype.cmp = function (other) {
            if (this.key < other.key) {
                return -1;
            }
            if (this.key > other.key) {
                return 1;
            }
            if (this.value < other.value) {
                return -1;
            }
            if (this.value > other.value) {
                return 1;
            }
            return 0;
        };
        KbNotEqualsExpression.prototype.equals = function (other) {
            if (other instanceof KbNotEqualsExpression) {
                return (this.key === other.key && this.value === other.value);
            }
            return false;
        };
        KbNotEqualsExpression.prototype.evaluate = function (context) {
            /* tslint:disable:triple-equals */
            // Intentional !=
            return (context[this.key] != this.value);
            /* tslint:enable:triple-equals */
        };
        KbNotEqualsExpression.prototype.normalize = function () {
            if (typeof this.value === 'boolean') {
                if (this.value) {
                    return new KbNotExpression(this.key);
                }
                return new KbDefinedExpression(this.key);
            }
            return this;
        };
        KbNotEqualsExpression.prototype.serialize = function () {
            if (typeof this.value === 'boolean') {
                return this.normalize().serialize();
            }
            return this.key + ' != \'' + this.value + '\'';
        };
        return KbNotEqualsExpression;
    }());
    exports.KbNotEqualsExpression = KbNotEqualsExpression;
    var KbNotExpression = (function () {
        function KbNotExpression(key) {
            this.key = key;
        }
        KbNotExpression.prototype.getType = function () {
            return KbExprType.KbNotExpression;
        };
        KbNotExpression.prototype.cmp = function (other) {
            if (this.key < other.key) {
                return -1;
            }
            if (this.key > other.key) {
                return 1;
            }
            return 0;
        };
        KbNotExpression.prototype.equals = function (other) {
            if (other instanceof KbNotExpression) {
                return (this.key === other.key);
            }
            return false;
        };
        KbNotExpression.prototype.evaluate = function (context) {
            return (!context[this.key]);
        };
        KbNotExpression.prototype.normalize = function () {
            return this;
        };
        KbNotExpression.prototype.serialize = function () {
            return '!' + this.key;
        };
        return KbNotExpression;
    }());
    exports.KbNotExpression = KbNotExpression;
    var KbAndExpression = (function () {
        function KbAndExpression(expr) {
            this.expr = KbAndExpression._normalizeArr(expr);
        }
        KbAndExpression.prototype.getType = function () {
            return KbExprType.KbAndExpression;
        };
        KbAndExpression.prototype.equals = function (other) {
            if (other instanceof KbAndExpression) {
                if (this.expr.length !== other.expr.length) {
                    return false;
                }
                for (var i = 0, len = this.expr.length; i < len; i++) {
                    if (!this.expr[i].equals(other.expr[i])) {
                        return false;
                    }
                }
                return true;
            }
        };
        KbAndExpression.prototype.evaluate = function (context) {
            for (var i = 0, len = this.expr.length; i < len; i++) {
                if (!this.expr[i].evaluate(context)) {
                    return false;
                }
            }
            return true;
        };
        KbAndExpression._normalizeArr = function (arr) {
            var expr = [];
            if (arr) {
                for (var i = 0, len = arr.length; i < len; i++) {
                    var e = arr[i];
                    if (!e) {
                        continue;
                    }
                    e = e.normalize();
                    if (!e) {
                        continue;
                    }
                    if (e instanceof KbAndExpression) {
                        expr = expr.concat(e.expr);
                        continue;
                    }
                    expr.push(e);
                }
                expr.sort(cmp);
            }
            return expr;
        };
        KbAndExpression.prototype.normalize = function () {
            if (this.expr.length === 0) {
                return null;
            }
            if (this.expr.length === 1) {
                return this.expr[0];
            }
            return this;
        };
        KbAndExpression.prototype.serialize = function () {
            if (this.expr.length === 0) {
                return '';
            }
            if (this.expr.length === 1) {
                return this.normalize().serialize();
            }
            return this.expr.map(function (e) { return e.serialize(); }).join(' && ');
        };
        return KbAndExpression;
    }());
    exports.KbAndExpression = KbAndExpression;
    exports.KbExpr = {
        has: function (key) { return new KbDefinedExpression(key); },
        equals: function (key, value) { return new KbEqualsExpression(key, value); },
        notEquals: function (key, value) { return new KbNotEqualsExpression(key, value); },
        not: function (key) { return new KbNotExpression(key); },
        and: function () {
            var expr = [];
            for (var _i = 0; _i < arguments.length; _i++) {
                expr[_i - 0] = arguments[_i];
            }
            return new KbAndExpression(expr);
        },
        deserialize: function (serialized) {
            if (!serialized) {
                return null;
            }
            var pieces = serialized.split('&&');
            var result = new KbAndExpression(pieces.map(function (p) { return exports.KbExpr._deserializeOne(p); }));
            return result.normalize();
        },
        _deserializeOne: function (serializedOne) {
            serializedOne = serializedOne.trim();
            if (serializedOne.indexOf('!=') >= 0) {
                var pieces = serializedOne.split('!=');
                return new KbNotEqualsExpression(pieces[0].trim(), exports.KbExpr._deserializeValue(pieces[1]));
            }
            if (serializedOne.indexOf('==') >= 0) {
                var pieces = serializedOne.split('==');
                return new KbEqualsExpression(pieces[0].trim(), exports.KbExpr._deserializeValue(pieces[1]));
            }
            if (/^\!\s*/.test(serializedOne)) {
                return new KbNotExpression(serializedOne.substr(1).trim());
            }
            return new KbDefinedExpression(serializedOne);
        },
        _deserializeValue: function (serializedValue) {
            serializedValue = serializedValue.trim();
            if (serializedValue === 'true') {
                return true;
            }
            if (serializedValue === 'false') {
                return false;
            }
            var m = /^'([^']*)'$/.exec(serializedValue);
            if (m) {
                return m[1].trim();
            }
            return serializedValue;
        }
    };
    exports.IKeybindingService = instantiation_1.createDecorator('keybindingService');
    exports.SET_CONTEXT_COMMAND_ID = 'setContext';
});

define(__m[10], __M([0,1,9,25,34,18]), function (require, exports, keyCodes_1, platform, types_1, platform_1) {
    /*---------------------------------------------------------------------------------------------
     *  Copyright (c) Microsoft Corporation. All rights reserved.
     *  Licensed under the MIT License. See License.txt in the project root for license information.
     *--------------------------------------------------------------------------------------------*/
    'use strict';
    var KeybindingsRegistryImpl = (function () {
        function KeybindingsRegistryImpl() {
            this.WEIGHT = {
                editorCore: function (importance) {
                    if (importance === void 0) { importance = 0; }
                    return 0 + importance;
                },
                editorContrib: function (importance) {
                    if (importance === void 0) { importance = 0; }
                    return 100 + importance;
                },
                workbenchContrib: function (importance) {
                    if (importance === void 0) { importance = 0; }
                    return 200 + importance;
                },
                builtinExtension: function (importance) {
                    if (importance === void 0) { importance = 0; }
                    return 300 + importance;
                },
                externalExtension: function (importance) {
                    if (importance === void 0) { importance = 0; }
                    return 400 + importance;
                }
            };
            this._keybindings = [];
            this._commands = Object.create(null);
        }
        /**
         * Take current platform into account and reduce to primary & secondary.
         */
        KeybindingsRegistryImpl.bindToCurrentPlatform = function (kb) {
            if (platform.isWindows) {
                if (kb && kb.win) {
                    return kb.win;
                }
            }
            else if (platform.isMacintosh) {
                if (kb && kb.mac) {
                    return kb.mac;
                }
            }
            else {
                if (kb && kb.linux) {
                    return kb.linux;
                }
            }
            return kb;
        };
        KeybindingsRegistryImpl.prototype.registerCommandRule = function (rule) {
            var _this = this;
            var actualKb = KeybindingsRegistryImpl.bindToCurrentPlatform(rule);
            // here
            if (actualKb && actualKb.primary) {
                this.registerDefaultKeybinding(actualKb.primary, rule.id, rule.weight, 0, rule.when);
            }
            // here
            if (actualKb && Array.isArray(actualKb.secondary)) {
                actualKb.secondary.forEach(function (k, i) { return _this.registerDefaultKeybinding(k, rule.id, rule.weight, -i - 1, rule.when); });
            }
        };
        KeybindingsRegistryImpl.prototype.registerCommandDesc = function (desc) {
            this.registerCommandRule(desc);
            // if (_commands[desc.id]) {
            // 	console.warn('Duplicate handler for command: ' + desc.id);
            // }
            // this._commands[desc.id] = desc.handler;
            var handler = desc.handler;
            var description = desc.description || handler.description;
            // add argument validation if rich command metadata is provided
            if (typeof description === 'object') {
                var constraints_1 = [];
                for (var _i = 0, _a = description.args; _i < _a.length; _i++) {
                    var arg = _a[_i];
                    constraints_1.push(arg.constraint);
                }
                handler = function (accesor) {
                    var args = [];
                    for (var _i = 1; _i < arguments.length; _i++) {
                        args[_i - 1] = arguments[_i];
                    }
                    types_1.validateConstraints(args, constraints_1);
                    return desc.handler.apply(desc, [accesor].concat(args));
                };
            }
            // make sure description is there
            handler.description = description;
            // register handler
            this._commands[desc.id] = handler;
        };
        KeybindingsRegistryImpl.prototype.getCommands = function () {
            return this._commands;
        };
        KeybindingsRegistryImpl.prototype.registerDefaultKeybinding = function (keybinding, commandId, weight1, weight2, when) {
            if (platform.isWindows) {
                if (keyCodes_1.BinaryKeybindings.hasCtrlCmd(keybinding) && !keyCodes_1.BinaryKeybindings.hasShift(keybinding) && keyCodes_1.BinaryKeybindings.hasAlt(keybinding) && !keyCodes_1.BinaryKeybindings.hasWinCtrl(keybinding)) {
                    if (/^[A-Z0-9\[\]\|\;\'\,\.\/\`]$/.test(keyCodes_1.KeyCode.toString(keyCodes_1.BinaryKeybindings.extractKeyCode(keybinding)))) {
                        console.warn('Ctrl+Alt+ keybindings should not be used by default under Windows. Offender: ', keybinding, ' for ', commandId);
                    }
                }
            }
            this._keybindings.push({
                keybinding: keybinding,
                command: commandId,
                when: when,
                weight1: weight1,
                weight2: weight2
            });
        };
        KeybindingsRegistryImpl.prototype.getDefaultKeybindings = function () {
            return this._keybindings;
        };
        return KeybindingsRegistryImpl;
    }());
    exports.KeybindingsRegistry = new KeybindingsRegistryImpl();
    // Define extension point ids
    exports.Extensions = {
        EditorModes: 'platform.keybindingsRegistry'
    };
    platform_1.Registry.add(exports.Extensions.EditorModes, exports.KeybindingsRegistry);
});

define(__m[19], __M([0,1,9,14,6,10,20,12]), function (require, exports, keyCodes_1, editor_1, keybindingService_1, keybindingsRegistry_1, editorCommon, codeEditorService_1) {
    /*---------------------------------------------------------------------------------------------
     *  Copyright (c) Microsoft Corporation. All rights reserved.
     *  Licensed under the MIT License. See License.txt in the project root for license information.
     *--------------------------------------------------------------------------------------------*/
    'use strict';
    var H = editorCommon.Handler;
    function findFocusedEditor(commandId, accessor, complain) {
        var editor = accessor.get(codeEditorService_1.ICodeEditorService).getFocusedCodeEditor();
        if (!editor) {
            if (complain) {
                console.warn('Cannot execute ' + commandId + ' because no code editor is focused.');
            }
            return null;
        }
        return editor;
    }
    exports.findFocusedEditor = findFocusedEditor;
    function withCodeEditorFromCommandHandler(commandId, accessor, callback) {
        var editor = findFocusedEditor(commandId, accessor, true);
        if (editor) {
            callback(editor);
        }
    }
    exports.withCodeEditorFromCommandHandler = withCodeEditorFromCommandHandler;
    function getActiveEditor(accessor) {
        var editorService = accessor.get(editor_1.IEditorService);
        var activeEditor = editorService.getActiveEditor && editorService.getActiveEditor();
        if (activeEditor) {
            var editor = activeEditor.getControl();
            // Substitute for (editor instanceof ICodeEditor)
            if (editor && typeof editor.getEditorType === 'function') {
                var codeEditor = editor;
                return codeEditor;
            }
        }
        return null;
    }
    exports.getActiveEditor = getActiveEditor;
    function triggerEditorHandler(handlerId, accessor, args) {
        withCodeEditorFromCommandHandler(handlerId, accessor, function (editor) {
            editor.trigger('keyboard', handlerId, args);
        });
    }
    function registerCoreCommand(handlerId, kb, weight, when) {
        if (weight === void 0) { weight = keybindingsRegistry_1.KeybindingsRegistry.WEIGHT.editorCore(); }
        var desc = {
            id: handlerId,
            handler: triggerEditorHandler.bind(null, handlerId),
            weight: weight,
            when: (when ? when : keybindingService_1.KbExpr.has(editorCommon.KEYBINDING_CONTEXT_EDITOR_TEXT_FOCUS)),
            primary: kb.primary,
            secondary: kb.secondary,
            win: kb.win,
            mac: kb.mac,
            linux: kb.linux
        };
        keybindingsRegistry_1.KeybindingsRegistry.registerCommandDesc(desc);
    }
    function registerCoreDispatchCommand2(handlerId) {
        var desc = {
            id: handlerId,
            handler: triggerEditorHandler.bind(null, handlerId),
            weight: keybindingsRegistry_1.KeybindingsRegistry.WEIGHT.editorCore(),
            when: null,
            primary: 0
        };
        keybindingsRegistry_1.KeybindingsRegistry.registerCommandDesc(desc);
        var desc2 = {
            id: 'default:' + handlerId,
            handler: function (accessor, args) {
                withCodeEditorFromCommandHandler(handlerId, accessor, function (editor) {
                    editor.trigger('keyboard', handlerId, args);
                });
            },
            weight: keybindingsRegistry_1.KeybindingsRegistry.WEIGHT.editorCore(),
            when: null,
            primary: 0
        };
        keybindingsRegistry_1.KeybindingsRegistry.registerCommandDesc(desc2);
    }
    registerCoreDispatchCommand2(H.Type);
    registerCoreDispatchCommand2(H.ReplacePreviousChar);
    registerCoreDispatchCommand2(H.Paste);
    registerCoreDispatchCommand2(H.Cut);
    function getMacWordNavigationKB(shift, key) {
        // For macs, word navigation is based on the alt modifier
        if (shift) {
            return keyCodes_1.KeyMod.Shift | keyCodes_1.KeyMod.Alt | key;
        }
        else {
            return keyCodes_1.KeyMod.Alt | key;
        }
    }
    function getWordNavigationKB(shift, key) {
        // Normally word navigation is based on the ctrl modifier
        if (shift) {
            return keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyMod.Shift | key;
        }
        else {
            return keyCodes_1.KeyMod.CtrlCmd | key;
        }
    }
    // https://support.apple.com/en-gb/HT201236
    // [ADDED] Control-H					Delete the character to the left of the insertion point. Or use Delete.
    // [ADDED] Control-D					Delete the character to the right of the insertion point. Or use Fn-Delete.
    // [ADDED] Control-K					Delete the text between the insertion point and the end of the line or paragraph.
    // [ADDED] Command–Up Arrow				Move the insertion point to the beginning of the document.
    // [ADDED] Command–Down Arrow			Move the insertion point to the end of the document.
    // [ADDED] Command–Left Arrow			Move the insertion point to the beginning of the current line.
    // [ADDED] Command–Right Arrow			Move the insertion point to the end of the current line.
    // [ADDED] Option–Left Arrow			Move the insertion point to the beginning of the previous word.
    // [ADDED] Option–Right Arrow			Move the insertion point to the end of the next word.
    // [ADDED] Command–Shift–Up Arrow		Select the text between the insertion point and the beginning of the document.
    // [ADDED] Command–Shift–Down Arrow		Select the text between the insertion point and the end of the document.
    // [ADDED] Command–Shift–Left Arrow		Select the text between the insertion point and the beginning of the current line.
    // [ADDED] Command–Shift–Right Arrow	Select the text between the insertion point and the end of the current line.
    // [USED BY DUPLICATE LINES] Shift–Option–Up Arrow		Extend text selection to the beginning of the current paragraph, then to the beginning of the following paragraph if pressed again.
    // [USED BY DUPLICATE LINES] Shift–Option–Down Arrow	Extend text selection to the end of the current paragraph, then to the end of the following paragraph if pressed again.
    // [ADDED] Shift–Option–Left Arrow		Extend text selection to the beginning of the current word, then to the beginning of the following word if pressed again.
    // [ADDED] Shift–Option–Right Arrow		Extend text selection to the end of the current word, then to the end of the following word if pressed again.
    // [ADDED] Control-A					Move to the beginning of the line or paragraph.
    // [ADDED] Control-E					Move to the end of a line or paragraph.
    // [ADDED] Control-F					Move one character forward.
    // [ADDED] Control-B					Move one character backward.
    //Control-L								Center the cursor or selection in the visible area.
    // [ADDED] Control-P					Move up one line.
    // [ADDED] Control-N					Move down one line.
    // [ADDED] Control-O					Insert a new line after the insertion point.
    //Control-T								Swap the character behind the insertion point with the character in front of the insertion point.
    // Unconfirmed????
    //	Config.addKeyBinding(editorCommon.Handler.CursorPageDown,		KeyMod.WinCtrl | KeyCode.KEY_V);
    // OS X built in commands
    // Control+y => yank
    // [ADDED] Command+backspace => Delete to Hard BOL
    // [ADDED] Command+delete => Delete to Hard EOL
    // [ADDED] Control+k => Delete to Hard EOL
    // Control+l => show_at_center
    // Control+Command+d => noop
    // Control+Command+shift+d => noop
    registerCoreCommand(H.CursorLeft, {
        primary: keyCodes_1.KeyCode.LeftArrow,
        mac: { primary: keyCodes_1.KeyCode.LeftArrow, secondary: [keyCodes_1.KeyMod.WinCtrl | keyCodes_1.KeyCode.KEY_B] }
    });
    registerCoreCommand(H.CursorLeftSelect, {
        primary: keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.LeftArrow
    });
    registerCoreCommand(H.CursorRight, {
        primary: keyCodes_1.KeyCode.RightArrow,
        mac: { primary: keyCodes_1.KeyCode.RightArrow, secondary: [keyCodes_1.KeyMod.WinCtrl | keyCodes_1.KeyCode.KEY_F] }
    });
    registerCoreCommand(H.CursorRightSelect, {
        primary: keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.RightArrow
    });
    registerCoreCommand(H.CursorUp, {
        primary: keyCodes_1.KeyCode.UpArrow,
        mac: { primary: keyCodes_1.KeyCode.UpArrow, secondary: [keyCodes_1.KeyMod.WinCtrl | keyCodes_1.KeyCode.KEY_P] }
    });
    registerCoreCommand(H.CursorUpSelect, {
        primary: keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.UpArrow,
        secondary: [getWordNavigationKB(true, keyCodes_1.KeyCode.UpArrow)],
        mac: { primary: keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.UpArrow },
        linux: { primary: keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.UpArrow }
    });
    registerCoreCommand(H.CursorDown, {
        primary: keyCodes_1.KeyCode.DownArrow,
        mac: { primary: keyCodes_1.KeyCode.DownArrow, secondary: [keyCodes_1.KeyMod.WinCtrl | keyCodes_1.KeyCode.KEY_N] }
    });
    registerCoreCommand(H.CursorDownSelect, {
        primary: keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.DownArrow,
        secondary: [getWordNavigationKB(true, keyCodes_1.KeyCode.DownArrow)],
        mac: { primary: keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.DownArrow },
        linux: { primary: keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.DownArrow }
    });
    registerCoreCommand(H.CursorPageUp, {
        primary: keyCodes_1.KeyCode.PageUp
    });
    registerCoreCommand(H.CursorPageUpSelect, {
        primary: keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.PageUp
    });
    registerCoreCommand(H.CursorPageDown, {
        primary: keyCodes_1.KeyCode.PageDown
    });
    registerCoreCommand(H.CursorPageDownSelect, {
        primary: keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.PageDown
    });
    registerCoreCommand(H.CursorHome, {
        primary: keyCodes_1.KeyCode.Home,
        mac: { primary: keyCodes_1.KeyCode.Home, secondary: [keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyCode.LeftArrow, keyCodes_1.KeyMod.WinCtrl | keyCodes_1.KeyCode.KEY_A] }
    });
    registerCoreCommand(H.CursorHomeSelect, {
        primary: keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.Home,
        mac: { primary: keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.Home, secondary: [keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.LeftArrow] }
    });
    registerCoreCommand(H.CursorEnd, {
        primary: keyCodes_1.KeyCode.End,
        mac: { primary: keyCodes_1.KeyCode.End, secondary: [keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyCode.RightArrow, keyCodes_1.KeyMod.WinCtrl | keyCodes_1.KeyCode.KEY_E] }
    });
    registerCoreCommand(H.CursorEndSelect, {
        primary: keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.End,
        mac: { primary: keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.End, secondary: [keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.RightArrow] }
    });
    registerCoreCommand(H.ExpandLineSelection, {
        primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyCode.KEY_I
    });
    registerCoreCommand(H.ScrollLineUp, {
        primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyCode.UpArrow,
        mac: { primary: keyCodes_1.KeyMod.WinCtrl | keyCodes_1.KeyCode.PageUp }
    });
    registerCoreCommand(H.ScrollLineDown, {
        primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyCode.DownArrow,
        mac: { primary: keyCodes_1.KeyMod.WinCtrl | keyCodes_1.KeyCode.PageDown }
    });
    registerCoreCommand(H.ScrollPageUp, {
        primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyCode.PageUp,
        win: { primary: keyCodes_1.KeyMod.Alt | keyCodes_1.KeyCode.PageUp },
        linux: { primary: keyCodes_1.KeyMod.Alt | keyCodes_1.KeyCode.PageUp }
    });
    registerCoreCommand(H.ScrollPageDown, {
        primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyCode.PageDown,
        win: { primary: keyCodes_1.KeyMod.Alt | keyCodes_1.KeyCode.PageDown },
        linux: { primary: keyCodes_1.KeyMod.Alt | keyCodes_1.KeyCode.PageDown }
    });
    registerCoreCommand(H.CursorColumnSelectLeft, {
        primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyMod.Shift | keyCodes_1.KeyMod.Alt | keyCodes_1.KeyCode.LeftArrow,
        linux: { primary: 0 }
    });
    registerCoreCommand(H.CursorColumnSelectRight, {
        primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyMod.Shift | keyCodes_1.KeyMod.Alt | keyCodes_1.KeyCode.RightArrow,
        linux: { primary: 0 }
    });
    registerCoreCommand(H.CursorColumnSelectUp, {
        primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyMod.Shift | keyCodes_1.KeyMod.Alt | keyCodes_1.KeyCode.UpArrow,
        linux: { primary: 0 }
    });
    registerCoreCommand(H.CursorColumnSelectPageUp, {
        primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyMod.Shift | keyCodes_1.KeyMod.Alt | keyCodes_1.KeyCode.PageUp,
        linux: { primary: 0 }
    });
    registerCoreCommand(H.CursorColumnSelectDown, {
        primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyMod.Shift | keyCodes_1.KeyMod.Alt | keyCodes_1.KeyCode.DownArrow,
        linux: { primary: 0 }
    });
    registerCoreCommand(H.CursorColumnSelectPageDown, {
        primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyMod.Shift | keyCodes_1.KeyMod.Alt | keyCodes_1.KeyCode.PageDown,
        linux: { primary: 0 }
    });
    registerCoreCommand(H.Tab, {
        primary: keyCodes_1.KeyCode.Tab
    }, keybindingsRegistry_1.KeybindingsRegistry.WEIGHT.editorCore(), keybindingService_1.KbExpr.and(keybindingService_1.KbExpr.has(editorCommon.KEYBINDING_CONTEXT_EDITOR_TEXT_FOCUS), keybindingService_1.KbExpr.not(editorCommon.KEYBINDING_CONTEXT_EDITOR_TAB_MOVES_FOCUS)));
    registerCoreCommand(H.Outdent, {
        primary: keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.Tab
    }, keybindingsRegistry_1.KeybindingsRegistry.WEIGHT.editorCore(), keybindingService_1.KbExpr.and(keybindingService_1.KbExpr.has(editorCommon.KEYBINDING_CONTEXT_EDITOR_TEXT_FOCUS), keybindingService_1.KbExpr.not(editorCommon.KEYBINDING_CONTEXT_EDITOR_TAB_MOVES_FOCUS)));
    registerCoreCommand(H.DeleteLeft, {
        primary: keyCodes_1.KeyCode.Backspace,
        secondary: [keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.Backspace],
        mac: { primary: keyCodes_1.KeyCode.Backspace, secondary: [keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.Backspace, keyCodes_1.KeyMod.WinCtrl | keyCodes_1.KeyCode.KEY_H, keyCodes_1.KeyMod.WinCtrl | keyCodes_1.KeyCode.Backspace] }
    });
    registerCoreCommand(H.DeleteRight, {
        primary: keyCodes_1.KeyCode.Delete,
        mac: { primary: keyCodes_1.KeyCode.Delete, secondary: [keyCodes_1.KeyMod.WinCtrl | keyCodes_1.KeyCode.KEY_D, keyCodes_1.KeyMod.WinCtrl | keyCodes_1.KeyCode.Delete] }
    });
    registerCoreCommand(H.DeleteAllLeft, {
        primary: null,
        mac: { primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyCode.Backspace }
    });
    registerCoreCommand(H.DeleteAllRight, {
        primary: null,
        mac: { primary: keyCodes_1.KeyMod.WinCtrl | keyCodes_1.KeyCode.KEY_K, secondary: [keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyCode.Delete] }
    });
    function registerWordCommand(handlerId, shift, key) {
        registerCoreCommand(handlerId, {
            primary: getWordNavigationKB(shift, key),
            mac: { primary: getMacWordNavigationKB(shift, key) }
        });
    }
    registerWordCommand(H.CursorWordStartLeft, false, keyCodes_1.KeyCode.LeftArrow);
    registerCoreCommand(H.CursorWordEndLeft, { primary: 0 });
    registerCoreCommand(H.CursorWordLeft, { primary: 0 });
    registerWordCommand(H.CursorWordStartLeftSelect, true, keyCodes_1.KeyCode.LeftArrow);
    registerCoreCommand(H.CursorWordEndLeftSelect, { primary: 0 });
    registerCoreCommand(H.CursorWordLeftSelect, { primary: 0 });
    registerWordCommand(H.CursorWordEndRight, false, keyCodes_1.KeyCode.RightArrow);
    registerCoreCommand(H.CursorWordStartRight, { primary: 0 });
    registerCoreCommand(H.CursorWordRight, { primary: 0 });
    registerWordCommand(H.CursorWordEndRightSelect, true, keyCodes_1.KeyCode.RightArrow);
    registerCoreCommand(H.CursorWordStartRightSelect, { primary: 0 });
    registerCoreCommand(H.CursorWordRightSelect, { primary: 0 });
    registerWordCommand(H.DeleteWordLeft, false, keyCodes_1.KeyCode.Backspace);
    registerCoreCommand(H.DeleteWordStartLeft, { primary: 0 });
    registerCoreCommand(H.DeleteWordEndLeft, { primary: 0 });
    registerWordCommand(H.DeleteWordRight, false, keyCodes_1.KeyCode.Delete);
    registerCoreCommand(H.DeleteWordStartRight, { primary: 0 });
    registerCoreCommand(H.DeleteWordEndRight, { primary: 0 });
    registerCoreCommand(H.CancelSelection, {
        primary: keyCodes_1.KeyCode.Escape,
        secondary: [keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.Escape]
    }, keybindingsRegistry_1.KeybindingsRegistry.WEIGHT.editorCore(), keybindingService_1.KbExpr.and(keybindingService_1.KbExpr.has(editorCommon.KEYBINDING_CONTEXT_EDITOR_TEXT_FOCUS), keybindingService_1.KbExpr.has(editorCommon.KEYBINDING_CONTEXT_EDITOR_HAS_NON_EMPTY_SELECTION)));
    registerCoreCommand(H.RemoveSecondaryCursors, {
        primary: keyCodes_1.KeyCode.Escape,
        secondary: [keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.Escape]
    }, keybindingsRegistry_1.KeybindingsRegistry.WEIGHT.editorCore(1), keybindingService_1.KbExpr.and(keybindingService_1.KbExpr.has(editorCommon.KEYBINDING_CONTEXT_EDITOR_TEXT_FOCUS), keybindingService_1.KbExpr.has(editorCommon.KEYBINDING_CONTEXT_EDITOR_HAS_MULTIPLE_SELECTIONS)));
    registerCoreCommand(H.CursorTop, {
        primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyCode.Home,
        mac: { primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyCode.UpArrow }
    });
    registerCoreCommand(H.CursorTopSelect, {
        primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.Home,
        mac: { primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.UpArrow }
    });
    registerCoreCommand(H.CursorBottom, {
        primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyCode.End,
        mac: { primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyCode.DownArrow }
    });
    registerCoreCommand(H.CursorBottomSelect, {
        primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.End,
        mac: { primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.DownArrow }
    });
    registerCoreCommand(H.LineBreakInsert, {
        primary: null,
        mac: { primary: keyCodes_1.KeyMod.WinCtrl | keyCodes_1.KeyCode.KEY_O }
    });
    registerCoreCommand(H.Undo, {
        primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyCode.KEY_Z
    });
    registerCoreCommand(H.CursorUndo, {
        primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyCode.KEY_U
    });
    registerCoreCommand(H.Redo, {
        primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyCode.KEY_Y,
        secondary: [keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.KEY_Z],
        mac: { primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyMod.Shift | keyCodes_1.KeyCode.KEY_Z }
    });
    function selectAll(accessor, args) {
        var HANDLER = editorCommon.Handler.SelectAll;
        var focusedEditor = findFocusedEditor(HANDLER, accessor, false);
        // Only if editor text focus (i.e. not if editor has widget focus).
        if (focusedEditor && focusedEditor.isFocused()) {
            focusedEditor.trigger('keyboard', HANDLER, args);
            return;
        }
        // Ignore this action when user is focussed on an element that allows for entering text
        var activeElement = document.activeElement;
        if (activeElement && ['input', 'textarea'].indexOf(activeElement.tagName.toLowerCase()) >= 0) {
            activeElement.select();
            return;
        }
        // Redirecting to last active editor
        var activeEditor = getActiveEditor(accessor);
        if (activeEditor) {
            activeEditor.trigger('keyboard', HANDLER, args);
            return;
        }
    }
    keybindingsRegistry_1.KeybindingsRegistry.registerCommandDesc({
        id: 'editor.action.selectAll',
        handler: selectAll,
        weight: keybindingsRegistry_1.KeybindingsRegistry.WEIGHT.editorCore(),
        when: null,
        primary: keyCodes_1.KeyMod.CtrlCmd | keyCodes_1.KeyCode.KEY_A
    });
});

define(__m[4], __M([0,1,2,23,33,6,10,18,38,19,26,20,32]), function (require, exports, errors_1, uri_1, descriptors_1, keybindingService_1, keybindingsRegistry_1, platform_1, telemetry_1, config_1, position_1, editorCommon, modelService_1) {
    /*---------------------------------------------------------------------------------------------
     *  Copyright (c) Microsoft Corporation. All rights reserved.
     *  Licensed under the MIT License. See License.txt in the project root for license information.
     *--------------------------------------------------------------------------------------------*/
    'use strict';
    // --- Keybinding extensions to make it more concise to express keybindings conditions
    (function (ContextKey) {
        ContextKey[ContextKey["None"] = 0] = "None";
        ContextKey[ContextKey["EditorTextFocus"] = 1] = "EditorTextFocus";
        ContextKey[ContextKey["EditorFocus"] = 2] = "EditorFocus";
    })(exports.ContextKey || (exports.ContextKey = {}));
    var ContextKey = exports.ContextKey;
    // --- Editor Actions
    var EditorActionDescriptor = (function () {
        function EditorActionDescriptor(ctor, id, label, kbOpts, alias) {
            if (kbOpts === void 0) { kbOpts = defaultEditorActionKeybindingOptions; }
            this.ctor = ctor;
            this.id = id;
            this.label = label;
            this.alias = alias;
            this.kbOpts = kbOpts;
        }
        return EditorActionDescriptor;
    }());
    exports.EditorActionDescriptor = EditorActionDescriptor;
    var CommonEditorRegistry;
    (function (CommonEditorRegistry) {
        function registerEditorAction(desc) {
            platform_1.Registry.as(Extensions.EditorCommonContributions).registerEditorAction(desc);
        }
        CommonEditorRegistry.registerEditorAction = registerEditorAction;
        // --- Editor Contributions
        function registerEditorContribution(ctor) {
            platform_1.Registry.as(Extensions.EditorCommonContributions).registerEditorContribution2(ctor);
        }
        CommonEditorRegistry.registerEditorContribution = registerEditorContribution;
        function getEditorContributions() {
            return platform_1.Registry.as(Extensions.EditorCommonContributions).getEditorContributions2();
        }
        CommonEditorRegistry.getEditorContributions = getEditorContributions;
        // --- Editor Commands
        function commandWeight(importance) {
            if (importance === void 0) { importance = 0; }
            return keybindingsRegistry_1.KeybindingsRegistry.WEIGHT.editorContrib(importance);
        }
        CommonEditorRegistry.commandWeight = commandWeight;
        function registerEditorCommand(commandId, weight, keybinding, needsTextFocus, needsKey, handler) {
            var commandDesc = {
                id: commandId,
                handler: createCommandHandler(commandId, handler),
                weight: weight,
                when: whenRule(needsTextFocus, needsKey),
                primary: keybinding.primary,
                secondary: keybinding.secondary,
                win: keybinding.win,
                linux: keybinding.linux,
                mac: keybinding.mac,
            };
            keybindingsRegistry_1.KeybindingsRegistry.registerCommandDesc(commandDesc);
        }
        CommonEditorRegistry.registerEditorCommand = registerEditorCommand;
        function registerLanguageCommand(id, handler) {
            keybindingsRegistry_1.KeybindingsRegistry.registerCommandDesc({
                id: id,
                handler: function (accessor, args) {
                    return handler(accessor, args || {});
                },
                weight: keybindingsRegistry_1.KeybindingsRegistry.WEIGHT.editorContrib(),
                primary: undefined,
                when: undefined,
            });
        }
        CommonEditorRegistry.registerLanguageCommand = registerLanguageCommand;
        function registerDefaultLanguageCommand(id, handler) {
            registerLanguageCommand(id, function (accessor, args) {
                var resource = args.resource, position = args.position;
                if (!(resource instanceof uri_1.default) || !position_1.Position.isIPosition(position)) {
                    throw errors_1.illegalArgument();
                }
                var model = accessor.get(modelService_1.IModelService).getModel(resource);
                if (!model) {
                    throw errors_1.illegalArgument();
                }
                var editorPosition = position_1.Position.lift(position);
                return handler(model, editorPosition, args);
            });
        }
        CommonEditorRegistry.registerDefaultLanguageCommand = registerDefaultLanguageCommand;
    })(CommonEditorRegistry = exports.CommonEditorRegistry || (exports.CommonEditorRegistry = {}));
    var SimpleEditorContributionDescriptor = (function () {
        function SimpleEditorContributionDescriptor(ctor) {
            this._ctor = ctor;
        }
        SimpleEditorContributionDescriptor.prototype.createInstance = function (instantiationService, editor) {
            return instantiationService.createInstance(this._ctor, editor);
        };
        return SimpleEditorContributionDescriptor;
    }());
    var InternalEditorActionDescriptor = (function () {
        function InternalEditorActionDescriptor(ctor, id, label, alias) {
            this._descriptor = descriptors_1.createSyncDescriptor(ctor, {
                id: id,
                label: label,
                alias: alias
            });
        }
        InternalEditorActionDescriptor.prototype.createInstance = function (instService, editor) {
            return instService.createInstance(this._descriptor, editor);
        };
        return InternalEditorActionDescriptor;
    }());
    // Editor extension points
    var Extensions = {
        EditorCommonContributions: 'editor.commonContributions'
    };
    var EditorContributionRegistry = (function () {
        function EditorContributionRegistry() {
            this.editorContributions = [];
        }
        EditorContributionRegistry.prototype.registerEditorContribution2 = function (ctor) {
            this.editorContributions.push(new SimpleEditorContributionDescriptor(ctor));
        };
        EditorContributionRegistry.prototype.registerEditorAction = function (desc) {
            var handler = desc.kbOpts.handler;
            if (!handler) {
                // here
                if (desc.kbOpts.context === ContextKey.EditorTextFocus || desc.kbOpts.context === ContextKey.EditorFocus) {
                    handler = triggerEditorAction.bind(null, desc.id);
                }
                else {
                    handler = triggerEditorActionGlobal.bind(null, desc.id);
                }
            }
            var when = null;
            if (typeof desc.kbOpts.kbExpr === 'undefined') {
                // here
                if (desc.kbOpts.context === ContextKey.EditorTextFocus) {
                    when = keybindingService_1.KbExpr.has(editorCommon.KEYBINDING_CONTEXT_EDITOR_TEXT_FOCUS);
                }
                else if (desc.kbOpts.context === ContextKey.EditorFocus) {
                    when = keybindingService_1.KbExpr.has(editorCommon.KEYBINDING_CONTEXT_EDITOR_FOCUS);
                }
            }
            else {
                when = desc.kbOpts.kbExpr;
            }
            var commandDesc = {
                id: desc.id,
                handler: handler,
                weight: keybindingsRegistry_1.KeybindingsRegistry.WEIGHT.editorContrib(),
                when: when,
                primary: desc.kbOpts.primary,
                secondary: desc.kbOpts.secondary,
                win: desc.kbOpts.win,
                linux: desc.kbOpts.linux,
                mac: desc.kbOpts.mac,
            };
            keybindingsRegistry_1.KeybindingsRegistry.registerCommandDesc(commandDesc);
            this.editorContributions.push(new InternalEditorActionDescriptor(desc.ctor, desc.id, desc.label, desc.alias));
        };
        EditorContributionRegistry.prototype.getEditorContributions2 = function () {
            return this.editorContributions.slice(0);
        };
        return EditorContributionRegistry;
    }());
    platform_1.Registry.add(Extensions.EditorCommonContributions, new EditorContributionRegistry());
    function triggerEditorAction(actionId, accessor, args) {
        config_1.withCodeEditorFromCommandHandler(actionId, accessor, function (editor) {
            editor.trigger('keyboard', actionId, args);
        });
    }
    function triggerEditorActionGlobal(actionId, accessor, args) {
        // TODO: this is not necessarily keyboard
        var focusedEditor = config_1.findFocusedEditor(actionId, accessor, false);
        if (focusedEditor) {
            focusedEditor.trigger('keyboard', actionId, args);
            return;
        }
        var activeEditor = config_1.getActiveEditor(accessor);
        if (activeEditor) {
            var action = activeEditor.getAction(actionId);
            if (action) {
                accessor.get(telemetry_1.ITelemetryService).publicLog('editorActionInvoked', { name: action.label });
                action.run().done(null, errors_1.onUnexpectedError);
            }
            return;
        }
    }
    var defaultEditorActionKeybindingOptions = { primary: null, context: ContextKey.EditorTextFocus };
    function whenRule(needsTextFocus, needsKey) {
        var base = keybindingService_1.KbExpr.has(needsTextFocus ? editorCommon.KEYBINDING_CONTEXT_EDITOR_TEXT_FOCUS : editorCommon.KEYBINDING_CONTEXT_EDITOR_FOCUS);
        if (needsKey) {
            return keybindingService_1.KbExpr.and(base, keybindingService_1.KbExpr.has(needsKey));
        }
        return base;
    }
    function createCommandHandler(commandId, handler) {
        return function (accessor, args) {
            config_1.withCodeEditorFromCommandHandler(commandId, accessor, function (editor) {
                handler(accessor, editor, args || {});
            });
        };
    }
});

/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
define(__m[28], __M([0,1,29,2,3,4,5,8]), function (require, exports, arrays_1, errors_1, winjs_base_1, editorCommonExtensions_1, modes_1, async_1) {
    'use strict';
    function getHover(model, position) {
        var supports = modes_1.HoverProviderRegistry.ordered(model);
        var values = [];
        var promises = supports.map(function (support, idx) {
            return async_1.asWinJsPromise(function (token) {
                return support.provideHover(model, position, token);
            }).then(function (result) {
                if (result) {
                    var hasRange = (typeof result.range !== 'undefined');
                    var hasHtmlContent = (typeof result.contents !== 'undefined' && result.contents && result.contents.length > 0);
                    if (hasRange && hasHtmlContent) {
                        values[idx] = result;
                    }
                }
            }, function (err) {
                errors_1.onUnexpectedError(err);
            });
        });
        return winjs_base_1.TPromise.join(promises).then(function () { return arrays_1.coalesce(values); });
    }
    exports.getHover = getHover;
    editorCommonExtensions_1.CommonEditorRegistry.registerDefaultLanguageCommand('_executeHoverProvider', getHover);
});

/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
define(__m[24], __M([0,1,2,3,4,5,8]), function (require, exports, errors_1, winjs_base_1, editorCommonExtensions_1, modes_1, async_1) {
    'use strict';
    function provideReferences(model, position) {
        // collect references from all providers
        var promises = modes_1.ReferenceProviderRegistry.ordered(model).map(function (provider) {
            return async_1.asWinJsPromise(function (token) {
                return provider.provideReferences(model, position, { includeDeclaration: true }, token);
            }).then(function (result) {
                if (Array.isArray(result)) {
                    return result;
                }
            }, function (err) {
                errors_1.onUnexpectedError(err);
            });
        });
        return winjs_base_1.TPromise.join(promises).then(function (references) {
            var result = [];
            for (var _i = 0, references_1 = references; _i < references_1.length; _i++) {
                var ref = references_1[_i];
                if (ref) {
                    result.push.apply(result, ref);
                }
            }
            return result;
        });
    }
    exports.provideReferences = provideReferences;
    editorCommonExtensions_1.CommonEditorRegistry.registerDefaultLanguageCommand('_executeReferenceProvider', provideReferences);
});

define(__m[17], __M([0,1,8,29,2,3,4,5,35]), function (require, exports, async_1, arrays_1, errors_1, winjs_base_1, editorCommonExtensions_1, modes_1, supports_1) {
    /*---------------------------------------------------------------------------------------------
     *  Copyright (c) Microsoft Corporation. All rights reserved.
     *  Licensed under the MIT License. See License.txt in the project root for license information.
     *--------------------------------------------------------------------------------------------*/
    'use strict';
    exports.Context = {
        Visible: 'suggestWidgetVisible',
        MultipleSuggestions: 'suggestWidgetMultipleSuggestions',
        AcceptOnKey: 'suggestionSupportsAcceptOnKey'
    };
    function provideCompletionItems(model, position, groups) {
        if (!groups) {
            groups = modes_1.SuggestRegistry.orderedGroups(model);
        }
        var result = [];
        var factory = groups.map(function (supports, index) {
            return function () {
                // stop as soon as a group produced a result
                if (result.length > 0) {
                    return;
                }
                // for each support in the group ask for suggestions
                return winjs_base_1.TPromise.join(supports.map(function (support) {
                    return async_1.asWinJsPromise(function (token) {
                        return support.provideCompletionItems(model, position, token);
                    }).then(function (values) {
                        if (!values) {
                            return;
                        }
                        for (var _i = 0, values_1 = values; _i < values_1.length; _i++) {
                            var suggestResult = values_1[_i];
                            if (!suggestResult || arrays_1.isFalsyOrEmpty(suggestResult.suggestions)) {
                                continue;
                            }
                            result.push({
                                support: support,
                                currentWord: suggestResult.currentWord,
                                incomplete: suggestResult.incomplete,
                                suggestions: suggestResult.suggestions
                            });
                        }
                    }, errors_1.onUnexpectedError);
                }));
            };
        });
        return async_1.sequence(factory).then(function () {
            // add snippets to the first group
            var snippets = supports_1.SnippetsRegistry.getSnippets(model, position);
            result.push(snippets);
            return result;
        });
    }
    exports.provideCompletionItems = provideCompletionItems;
    editorCommonExtensions_1.CommonEditorRegistry.registerDefaultLanguageCommand('_executeCompletionItemProvider', function (model, position, args) {
        return provideCompletionItems(model, position);
    });
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
define(__m[36], __M([0,1,23,3,37,30,31,5,11,26,39,40,41,22,13,42,43,44,28,24,17]), function (require, exports, uri_1, winjs, beautifyHTML, htmlTags, network, modes, strings, position_1, workspace_1, markers_1, resourceService_1, htmlScanner_1, htmlTokenTypes_1, htmlEmptyTagsShared_1, suggestSupport_1, paths, hover_1, referenceSearch_1, suggest_1) {
    /*---------------------------------------------------------------------------------------------
     *  Copyright (c) Microsoft Corporation. All rights reserved.
     *  Licensed under the MIT License. See License.txt in the project root for license information.
     *--------------------------------------------------------------------------------------------*/
    'use strict';
    var LinkDetectionState;
    (function (LinkDetectionState) {
        LinkDetectionState[LinkDetectionState["LOOKING_FOR_HREF_OR_SRC"] = 1] = "LOOKING_FOR_HREF_OR_SRC";
        LinkDetectionState[LinkDetectionState["AFTER_HREF_OR_SRC"] = 2] = "AFTER_HREF_OR_SRC";
    })(LinkDetectionState || (LinkDetectionState = {}));
    var HTMLWorker = (function () {
        function HTMLWorker(modeId, resourceService, markerService, contextService) {
            this._modeId = modeId;
            this.resourceService = resourceService;
            this.markerService = markerService;
            this._contextService = contextService;
            this._tagProviders = [];
            this._tagProviders.push(htmlTags.getHTML5TagProvider());
            this.addCustomTagProviders(this._tagProviders);
        }
        HTMLWorker.prototype.addCustomTagProviders = function (providers) {
            providers.push(htmlTags.getAngularTagProvider());
            providers.push(htmlTags.getIonicTagProvider());
        };
        HTMLWorker.prototype.provideDocumentRangeFormattingEdits = function (resource, range, options) {
            return this.formatHTML(resource, range, options);
        };
        HTMLWorker.prototype.formatHTML = function (resource, range, options) {
            var model = this.resourceService.get(resource);
            var value = range ? model.getValueInRange(range) : model.getValue();
            var htmlOptions = {
                indent_size: options.insertSpaces ? options.tabSize : 1,
                indent_char: options.insertSpaces ? ' ' : '\t',
                wrap_line_length: this.getFormatOption('wrapLineLength', 120),
                unformatted: this.getTagsFormatOption('unformatted', void 0),
                indent_inner_html: this.getFormatOption('indentInnerHtml', false),
                preserve_newlines: this.getFormatOption('preserveNewLines', false),
                max_preserve_newlines: this.getFormatOption('maxPreserveNewLines', void 0),
                indent_handlebars: this.getFormatOption('indentHandlebars', false),
                end_with_newline: this.getFormatOption('endWithNewline', false),
                extra_liners: this.getTagsFormatOption('extraLiners', void 0),
            };
            var result = beautifyHTML.html_beautify(value, htmlOptions);
            return winjs.TPromise.as([{
                    range: range,
                    text: result
                }]);
        };
        HTMLWorker.prototype.getFormatOption = function (key, dflt) {
            if (this.formatSettings && this.formatSettings.hasOwnProperty(key)) {
                var value = this.formatSettings[key];
                if (value !== null) {
                    return value;
                }
            }
            return dflt;
        };
        HTMLWorker.prototype.getTagsFormatOption = function (key, dflt) {
            var list = this.getFormatOption(key, null);
            if (typeof list === 'string') {
                if (list.length > 0) {
                    return list.split(',').map(function (t) { return t.trim().toLowerCase(); });
                }
                return [];
            }
            return dflt;
        };
        HTMLWorker.prototype._doConfigure = function (options) {
            this.formatSettings = options && options.format;
            return winjs.TPromise.as(null);
        };
        HTMLWorker.prototype._delegateToModeAtPosition = function (resource, position, callback) {
            var model = this.resourceService.get(resource);
            if (!model) {
                return null;
            }
            var modelAtPosition = model.getEmbeddedAtPosition(position);
            if (!modelAtPosition) {
                return callback(false, model);
            }
            var modeAtPosition = modelAtPosition.getMode();
            return callback(modeAtPosition.getId() !== this._modeId, modelAtPosition);
        };
        HTMLWorker.prototype._delegateToAllModes = function (resource, callback) {
            var model = this.resourceService.get(resource);
            if (!model) {
                return null;
            }
            return callback(model.getAllEmbedded());
        };
        HTMLWorker.prototype.provideHover = function (resource, position) {
            return this._delegateToModeAtPosition(resource, position, function (isEmbeddedMode, model) {
                if (isEmbeddedMode) {
                    return hover_1.getHover(model, position_1.Position.lift(position)).then(function (r) {
                        return (r.length > 0 ? r[0] : null);
                    });
                }
            });
        };
        HTMLWorker.prototype.provideReferences = function (resource, position) {
            return this._delegateToModeAtPosition(resource, position, function (isEmbeddedMode, model) {
                if (isEmbeddedMode) {
                    return referenceSearch_1.provideReferences(model, position_1.Position.lift(position));
                }
            });
        };
        HTMLWorker.prototype.findMatchingOpenTag = function (scanner) {
            var closedTags = {};
            var tagClosed = false;
            while (scanner.scanBack()) {
                if (htmlTokenTypes_1.isTag(scanner.getTokenType()) && !tagClosed) {
                    var tag = scanner.getTokenContent();
                    scanner.scanBack();
                    if (scanner.getTokenType() === htmlTokenTypes_1.DELIM_END) {
                        closedTags[tag] = (closedTags[tag] || 0) + 1;
                    }
                    else if (!htmlEmptyTagsShared_1.isEmptyElement(tag)) {
                        if (closedTags[tag]) {
                            closedTags[tag]--;
                        }
                        else {
                            return tag;
                        }
                    }
                }
                else if (scanner.getTokenType() === htmlTokenTypes_1.DELIM_START) {
                    tagClosed = scanner.getTokenContent() === '/>';
                }
            }
            return null;
        };
        HTMLWorker.prototype.collectTagSuggestions = function (scanner, position, suggestions) {
            var _this = this;
            var model = scanner.getModel();
            var currentLine = model.getLineContent(position.lineNumber);
            var contentAfter = currentLine.substr(position.column - 1);
            var closeTag = isWhiteSpace(contentAfter) || strings.startsWith(contentAfter, '<') ? '>' : '';
            var collectClosingTagSuggestion = function (overwriteBefore) {
                var endPosition = scanner.getTokenPosition();
                var matchingTag = _this.findMatchingOpenTag(scanner);
                if (matchingTag) {
                    var suggestion = {
                        label: '/' + matchingTag,
                        codeSnippet: '/' + matchingTag + closeTag,
                        overwriteBefore: overwriteBefore,
                        type: 'property'
                    };
                    suggestions.suggestions.push(suggestion);
                    // use indent from start tag
                    var startPosition = scanner.getTokenPosition();
                    if (endPosition.lineNumber !== startPosition.lineNumber) {
                        var startIndent = model.getLineContent(startPosition.lineNumber).substring(0, startPosition.column - 1);
                        var endIndent = model.getLineContent(endPosition.lineNumber).substring(0, endPosition.column - 1);
                        if (isWhiteSpace(startIndent) && isWhiteSpace(endIndent)) {
                            suggestion.overwriteBefore = position.column - 1; // replace from start of line
                            suggestion.codeSnippet = startIndent + '</' + matchingTag + closeTag;
                            suggestion.filterText = currentLine.substring(0, position.column - 1);
                        }
                    }
                    return true;
                }
                return false;
            };
            if (scanner.getTokenType() === htmlTokenTypes_1.DELIM_END) {
                var hasClose = collectClosingTagSuggestion(suggestions.currentWord.length + 1);
                if (!hasClose) {
                    this._tagProviders.forEach(function (provider) {
                        provider.collectTags(function (tag, label) {
                            suggestions.suggestions.push({
                                label: '/' + tag,
                                overwriteBefore: suggestions.currentWord.length + 1,
                                codeSnippet: '/' + tag + closeTag,
                                type: 'property',
                                documentationLabel: label
                            });
                        });
                    });
                }
            }
            else {
                collectClosingTagSuggestion(suggestions.currentWord.length);
                this._tagProviders.forEach(function (provider) {
                    provider.collectTags(function (tag, label) {
                        suggestions.suggestions.push({
                            label: tag,
                            codeSnippet: tag,
                            type: 'property',
                            documentationLabel: label
                        });
                    });
                });
            }
        };
        HTMLWorker.prototype.collectContentSuggestions = function (suggestions) {
            // disable the simple snippets in favor of the emmet templates
        };
        HTMLWorker.prototype.collectAttributeSuggestions = function (scanner, suggestions) {
            var parentTag = null;
            do {
                if (htmlTokenTypes_1.isTag(scanner.getTokenType())) {
                    parentTag = scanner.getTokenContent();
                    break;
                }
                if (scanner.getTokenType() === htmlTokenTypes_1.DELIM_START) {
                    break;
                }
            } while (scanner.scanBack());
            this._tagProviders.forEach(function (provider) {
                provider.collectAttributes(parentTag, function (attribute, type) {
                    var codeSnippet = attribute;
                    if (type !== 'v') {
                        codeSnippet = codeSnippet + '="{{}}"';
                    }
                    suggestions.suggestions.push({
                        label: attribute,
                        codeSnippet: codeSnippet,
                        type: type === 'handler' ? 'function' : 'value'
                    });
                });
            });
        };
        HTMLWorker.prototype.collectAttributeValueSuggestions = function (scanner, suggestions) {
            var needsQuotes = scanner.getTokenType() === htmlTokenTypes_1.DELIM_ASSIGN;
            var attribute = null;
            var parentTag = null;
            while (scanner.scanBack()) {
                if (scanner.getTokenType() === htmlTokenTypes_1.ATTRIB_NAME) {
                    attribute = scanner.getTokenContent();
                    break;
                }
            }
            while (scanner.scanBack()) {
                if (htmlTokenTypes_1.isTag(scanner.getTokenType())) {
                    parentTag = scanner.getTokenContent();
                    break;
                }
                if (scanner.getTokenType() === htmlTokenTypes_1.DELIM_START) {
                    return;
                }
            }
            this._tagProviders.forEach(function (provider) {
                provider.collectValues(parentTag, attribute, function (value) {
                    suggestions.suggestions.push({
                        label: value,
                        codeSnippet: needsQuotes ? '"' + value + '"' : value,
                        type: 'unit'
                    });
                });
            });
        };
        HTMLWorker.prototype.provideCompletionItems = function (resource, position) {
            var _this = this;
            return this._delegateToModeAtPosition(resource, position, function (isEmbeddedMode, model) {
                if (isEmbeddedMode) {
                    return suggest_1.provideCompletionItems(model, position_1.Position.lift(position));
                }
                return _this.suggestHTML(resource, position);
            });
        };
        HTMLWorker.prototype.suggestHTML = function (resource, position) {
            return this.doSuggest(resource, position).then(function (value) { return suggestSupport_1.filterSuggestions(value); });
        };
        HTMLWorker.prototype.doSuggest = function (resource, position) {
            var model = this.resourceService.get(resource), currentWord = model.getWordUntilPosition(position).word;
            var suggestions = {
                currentWord: currentWord,
                suggestions: [],
            };
            var scanner = htmlScanner_1.getScanner(model, position);
            switch (scanner.getTokenType()) {
                case htmlTokenTypes_1.DELIM_START:
                case htmlTokenTypes_1.DELIM_END:
                    if (scanner.isOpenBrace()) {
                        this.collectTagSuggestions(scanner, position, suggestions);
                    }
                    else {
                        this.collectContentSuggestions(suggestions);
                    }
                    break;
                case htmlTokenTypes_1.ATTRIB_NAME:
                    this.collectAttributeSuggestions(scanner, suggestions);
                    break;
                case htmlTokenTypes_1.ATTRIB_VALUE:
                    this.collectAttributeValueSuggestions(scanner, suggestions);
                    break;
                case htmlTokenTypes_1.DELIM_ASSIGN:
                    if (scanner.isAtTokenEnd()) {
                        this.collectAttributeValueSuggestions(scanner, suggestions);
                    }
                    break;
                case '':
                    if (isWhiteSpace(scanner.getTokenContent()) && scanner.scanBack()) {
                        switch (scanner.getTokenType()) {
                            case htmlTokenTypes_1.ATTRIB_VALUE:
                            case htmlTokenTypes_1.ATTRIB_NAME:
                                this.collectAttributeSuggestions(scanner, suggestions);
                                break;
                            case htmlTokenTypes_1.DELIM_ASSIGN:
                                this.collectAttributeValueSuggestions(scanner, suggestions);
                                break;
                            case htmlTokenTypes_1.DELIM_START:
                            case htmlTokenTypes_1.DELIM_END:
                                if (scanner.isOpenBrace()) {
                                    this.collectTagSuggestions(scanner, position, suggestions);
                                }
                                else {
                                    this.collectContentSuggestions(suggestions);
                                }
                                break;
                            default:
                                if (htmlTokenTypes_1.isTag(scanner.getTokenType())) {
                                    this.collectAttributeSuggestions(scanner, suggestions);
                                }
                        }
                    }
                    else {
                        this.collectContentSuggestions(suggestions);
                    }
                    break;
                default:
                    if (htmlTokenTypes_1.isTag(scanner.getTokenType())) {
                        scanner.scanBack(); // one back to the end/start bracket
                        this.collectTagSuggestions(scanner, position, suggestions);
                    }
            }
            return winjs.TPromise.as(suggestions);
        };
        HTMLWorker.prototype.findMatchingBracket = function (tagname, scanner) {
            if (htmlEmptyTagsShared_1.isEmptyElement(tagname)) {
                return null;
            }
            var tagCount = 0;
            scanner.scanBack(); // one back to the end/start bracket
            if (scanner.getTokenType() === htmlTokenTypes_1.DELIM_END) {
                // find the opening tag
                var tagClosed = false;
                while (scanner.scanBack()) {
                    if (htmlTokenTypes_1.isTag(scanner.getTokenType()) && scanner.getTokenContent() === tagname && !tagClosed) {
                        var range = scanner.getTokenRange();
                        scanner.scanBack(); // one back to the end/start bracket
                        if (scanner.getTokenType() === htmlTokenTypes_1.DELIM_START) {
                            if (tagCount === 0) {
                                return range;
                            }
                            else {
                                tagCount--;
                            }
                        }
                        else {
                            tagCount++;
                        }
                    }
                    else if (scanner.getTokenType() === htmlTokenTypes_1.DELIM_START) {
                        tagClosed = scanner.getTokenContent() === '/>';
                    }
                }
            }
            else {
                var isTagEnd = false;
                while (scanner.scanForward()) {
                    if (htmlTokenTypes_1.isTag(scanner.getTokenType()) && scanner.getTokenContent() === tagname) {
                        if (!isTagEnd) {
                            scanner.scanForward();
                            if (scanner.getTokenType() === htmlTokenTypes_1.DELIM_START && scanner.getTokenContent() === '/>') {
                                if (tagCount <= 0) {
                                    return null;
                                }
                            }
                            else {
                                tagCount++;
                            }
                        }
                        else {
                            tagCount--;
                            if (tagCount <= 0) {
                                return scanner.getTokenRange();
                            }
                        }
                    }
                    else if (scanner.getTokenType() === htmlTokenTypes_1.DELIM_START) {
                        isTagEnd = false;
                    }
                    else if (scanner.getTokenType() === htmlTokenTypes_1.DELIM_END) {
                        isTagEnd = true;
                    }
                }
            }
            return null;
        };
        HTMLWorker.prototype.provideDocumentHighlights = function (resource, position, strict) {
            if (strict === void 0) { strict = false; }
            var model = this.resourceService.get(resource), wordAtPosition = model.getWordAtPosition(position), currentWord = (wordAtPosition ? wordAtPosition.word : ''), result = [];
            var scanner = htmlScanner_1.getScanner(model, position);
            if (htmlTokenTypes_1.isTag(scanner.getTokenType())) {
                var tagname = scanner.getTokenContent();
                result.push({
                    range: scanner.getTokenRange(),
                    kind: modes.DocumentHighlightKind.Read
                });
                var range = this.findMatchingBracket(tagname, scanner);
                if (range) {
                    result.push({
                        range: range,
                        kind: modes.DocumentHighlightKind.Read
                    });
                }
            }
            else {
                var words = model.getAllWordsWithRange(), upperBound = Math.min(1000, words.length); // Limit find occurences to 1000 occurences
                for (var i = 0; i < upperBound; i++) {
                    if (words[i].text === currentWord) {
                        result.push({
                            range: words[i].range,
                            kind: modes.DocumentHighlightKind.Read
                        });
                    }
                }
            }
            return winjs.TPromise.as(result);
        };
        HTMLWorker._stripQuotes = function (url) {
            return url
                .replace(/^'([^']+)'$/, function (substr, match1) { return match1; })
                .replace(/^"([^"]+)"$/, function (substr, match1) { return match1; });
        };
        HTMLWorker._getWorkspaceUrl = function (modelAbsoluteUri, rootAbsoluteUri, tokenContent) {
            tokenContent = HTMLWorker._stripQuotes(tokenContent);
            if (/^\s*javascript\:/i.test(tokenContent) || /^\s*\#/i.test(tokenContent)) {
                return null;
            }
            if (/^\s*https?:\/\//i.test(tokenContent) || /^\s*file:\/\//i.test(tokenContent)) {
                // Absolute link that needs no treatment
                return tokenContent.replace(/^\s*/g, '');
            }
            if (/^\s*\/\//i.test(tokenContent)) {
                // Absolute link (that does not name the protocol)
                var pickedScheme = network.Schemas.http;
                if (modelAbsoluteUri.scheme === network.Schemas.https) {
                    pickedScheme = network.Schemas.https;
                }
                return pickedScheme + ':' + tokenContent.replace(/^\s*/g, '');
            }
            var modelPath = paths.dirname(modelAbsoluteUri.path);
            var alternativeResultPath = null;
            if (tokenContent.length > 0 && tokenContent.charAt(0) === '/') {
                alternativeResultPath = tokenContent;
            }
            else {
                alternativeResultPath = paths.join(modelPath, tokenContent);
                alternativeResultPath = alternativeResultPath.replace(/^(\/\.\.)+/, '');
            }
            var potentialResult = modelAbsoluteUri.with({ path: alternativeResultPath }).toString();
            var rootAbsoluteUrlStr = (rootAbsoluteUri ? rootAbsoluteUri.toString() : null);
            if (rootAbsoluteUrlStr && strings.startsWith(modelAbsoluteUri.toString(), rootAbsoluteUrlStr)) {
                // The `rootAbsoluteUrl` is set and matches our current model
                // We need to ensure that this `potentialResult` does not escape `rootAbsoluteUrl`
                var commonPrefixLength = strings.commonPrefixLength(rootAbsoluteUrlStr, potentialResult);
                if (strings.endsWith(rootAbsoluteUrlStr, '/')) {
                    commonPrefixLength = potentialResult.lastIndexOf('/', commonPrefixLength) + 1;
                }
                return rootAbsoluteUrlStr + potentialResult.substr(commonPrefixLength);
            }
            return potentialResult;
        };
        HTMLWorker.prototype.createLink = function (modelAbsoluteUrl, rootAbsoluteUrl, tokenContent, lineNumber, startColumn, endColumn) {
            var workspaceUrl = HTMLWorker._getWorkspaceUrl(modelAbsoluteUrl, rootAbsoluteUrl, tokenContent);
            if (!workspaceUrl) {
                return null;
            }
            //		console.info('workspaceUrl: ' + workspaceUrl);
            return {
                range: {
                    startLineNumber: lineNumber,
                    startColumn: startColumn,
                    endLineNumber: lineNumber,
                    endColumn: endColumn
                },
                url: workspaceUrl
            };
        };
        HTMLWorker.prototype._computeHTMLLinks = function (model) {
            var lineCount = model.getLineCount(), newLinks = [], state = LinkDetectionState.LOOKING_FOR_HREF_OR_SRC, modelAbsoluteUrl = model.uri, lineNumber, lineContent, lineContentLength, tokens, tokenType, tokensLength, i, nextTokenEndIndex, tokenContent, link;
            var rootAbsoluteUrl = null;
            var workspace = this._contextService.getWorkspace();
            if (workspace) {
                // The workspace can be null in the no folder opened case
                var strRootAbsoluteUrl = String(workspace.resource);
                if (strRootAbsoluteUrl.charAt(strRootAbsoluteUrl.length - 1) === '/') {
                    rootAbsoluteUrl = uri_1.default.parse(strRootAbsoluteUrl);
                }
                else {
                    rootAbsoluteUrl = uri_1.default.parse(strRootAbsoluteUrl + '/');
                }
            }
            for (lineNumber = 1; lineNumber <= lineCount; lineNumber++) {
                lineContent = model.getLineContent(lineNumber);
                lineContentLength = lineContent.length;
                tokens = model.getLineTokens(lineNumber);
                for (i = 0, tokensLength = tokens.getTokenCount(); i < tokensLength; i++) {
                    tokenType = tokens.getTokenType(i);
                    switch (tokenType) {
                        case htmlTokenTypes_1.DELIM_ASSIGN:
                        case '':
                            break;
                        case htmlTokenTypes_1.ATTRIB_NAME:
                            nextTokenEndIndex = tokens.getTokenEndIndex(i, lineContentLength);
                            tokenContent = lineContent.substring(tokens.getTokenStartIndex(i), nextTokenEndIndex).toLowerCase();
                            if (tokenContent === 'src' || tokenContent === 'href') {
                                state = LinkDetectionState.AFTER_HREF_OR_SRC;
                            }
                            else {
                                state = LinkDetectionState.LOOKING_FOR_HREF_OR_SRC;
                            }
                            break;
                        case htmlTokenTypes_1.ATTRIB_VALUE:
                            if (state === LinkDetectionState.AFTER_HREF_OR_SRC) {
                                nextTokenEndIndex = tokens.getTokenEndIndex(i, lineContentLength);
                                tokenContent = lineContent.substring(tokens.getTokenStartIndex(i), nextTokenEndIndex);
                                link = this.createLink(modelAbsoluteUrl, rootAbsoluteUrl, tokenContent, lineNumber, tokens.getTokenStartIndex(i) + 2, nextTokenEndIndex);
                                if (link) {
                                    newLinks.push(link);
                                }
                                state = LinkDetectionState.LOOKING_FOR_HREF_OR_SRC;
                            }
                        default:
                            if (htmlTokenTypes_1.isTag(tokenType)) {
                                state = LinkDetectionState.LOOKING_FOR_HREF_OR_SRC;
                            }
                            else if (state === LinkDetectionState.AFTER_HREF_OR_SRC) {
                                state = LinkDetectionState.LOOKING_FOR_HREF_OR_SRC;
                            }
                    }
                }
            }
            return newLinks;
        };
        HTMLWorker.prototype.provideLinks = function (resource) {
            var model = this.resourceService.get(resource);
            return winjs.TPromise.as(this._computeHTMLLinks(model));
        };
        HTMLWorker = __decorate([
            __param(1, resourceService_1.IResourceService),
            __param(2, markers_1.IMarkerService),
            __param(3, workspace_1.IWorkspaceContextService)
        ], HTMLWorker);
        return HTMLWorker;
    }());
    exports.HTMLWorker = HTMLWorker;
    function isWhiteSpace(s) {
        return /^\s*$/.test(s);
    }
});

}).call(this);
//# sourceMappingURL=htmlWorker.js.map
