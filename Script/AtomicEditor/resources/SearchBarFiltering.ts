//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
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
//

export class UISearchBar {
    searchPopulate(searchText: string, compareText: string): boolean {

        var equal = false;

        searchText = searchText.toUpperCase();
        compareText = compareText.toUpperCase();

        for (var i = 0; i < compareText.length; i++) {

            if (compareText[i] == searchText[0]) {
                var j = 0;
                var k = i;

                while (k < compareText.length && j < searchText.length) {

                    if (compareText[k] == searchText[j]) {
                        equal = true;
                    } else {
                        equal = false;
                        break;
                    }
                    j++;
                    k++;
                }

                if (j != searchText.length) {
                    equal = false;
                }

                if (equal) {
                    break;
                }
            }
        }

        return equal;
    }
}