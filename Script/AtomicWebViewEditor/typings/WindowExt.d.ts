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

/**
 * Defines the interface to what is available for the host to call or for the client to call on the window object
 */
interface Window {
    atomicQuery: (message: {
        request: any,
        persistent: boolean,
        onSuccess: () => void,
        onFailure: (error_code, error_message) => void
    }) => void;

    /**
     * Used to call into the host and provide messages
     * @param {string} messageType
     * @param {object} data
     * @return {Promise}
     */
    atomicQueryPromise: (messageType: string, data?: {}) => Promise<{}>;

    HOST_loadCode: (codeUrl) => void;
    HOST_saveCode: () => void;

    HOST_resourceRenamed: (path: string, newPath: string) => void;
    HOST_resourceDeleted: (path: string) => void;
    HOST_preferencesChanged: (jsonProjectPrefs: string, jsonApplicationPrefs: string) => void;

    /**
     * Preferences set by the host.  Each preference category is a JSON string of all the prefs
     */
    HOST_Preferences: {
        ApplicationPreferences: string,
        ProjectPreferences: string
    };
}
