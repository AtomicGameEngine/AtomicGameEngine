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

class WindowExt {

    /**
     * Used to send a notification message to the host
     * @param {string} messageType
     * @param {object} data
     * @return {Promise} will resolve when the host has handled the message
     */
    static atomicHostEvent(messageType: string, data?: {}): Promise<void> {
        let queryMessage: HostMessage;

        // if we have a data element, then we need to structure the message so that the host understands it
        // by adding the message to the object and then stringify-ing the whole thing
        if (data) {
            // stringify and reparse since we need to modify the data, but don't want to modify the passed in object
            queryMessage = JSON.parse(JSON.stringify(data));
            queryMessage.message = messageType;
        } else {
            queryMessage = {
                message: messageType
            };
        }

        return new Promise<void>(function(resolve, reject) {
            window.atomicQuery({
                request: JSON.stringify(queryMessage),
                persistent: false,
                onSuccess: (result?: string) => resolve(),
                onFailure: (error_code, error_message) => reject({ error_code: error_code, error_message: error_message })
            });
        });
    }

    /**
     * Used to send a request to the server.  The server will send back the results in the promise
     * @param  {string} messageType
     * @param  {object} data
     * @return {Promise}
     */
    static atomicHostRequest<T>(messageType: string, data?: {}): Promise<T> {
        let queryMessage: HostMessage;

        // if we have a data element, then we need to structure the message so that the host understands it
        // by adding the message to the object and then stringify-ing the whole thing
        if (data) {
            // stringify and reparse since we need to modify the data, but don't want to modify the passed in object
            queryMessage = JSON.parse(JSON.stringify(data));
            queryMessage.message = messageType;
        } else {
            queryMessage = {
                message: messageType
            };
        }

        return new Promise<T>(function(resolve, reject) {
            window.atomicQuery({
                request: JSON.stringify(queryMessage),
                persistent: false,
                onSuccess: (s) => {
                    // unwrap the message that was returned
                    let o = JSON.parse(s) as WebView.WebMessageEventResponse<T>;
                    resolve(o.response);
                },
                onFailure: (error_code, error_message) => reject({ error_code: error_code, error_message: error_message })
            });
        });
    }
}

// set up the window
window.atomicQueryPromise = WindowExt.atomicHostEvent; // deprecated
window.atomicHostEvent = WindowExt.atomicHostEvent;
window.atomicHostRequest = WindowExt.atomicHostRequest;
