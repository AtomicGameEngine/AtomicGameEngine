/*
Portions Copyright (c) 2013 Eugene Solodovnykov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#import <Foundation/Foundation.h>
#import <Security/Security.h>

#include "AtomicEditor.h"
#include "BuildIOSUtils.h"

namespace AtomicEditor {

String GetMobileProvisionData(const char* cfilename)
{
    CMSDecoderRef decoder = NULL;
    CFDataRef dataRef = NULL;

    NSString *plistString = nil;
    //NSDictionary *plist = nil;

    NSString *file = [[NSString alloc]  initWithBytes:cfilename
                                                      length:strlen(cfilename)
                                                      encoding:NSUTF8StringEncoding];
    @try {
        CMSDecoderCreate(&decoder);
        NSData *fileData = [NSData dataWithContentsOfFile:file];
        CMSDecoderUpdateMessage(decoder, fileData.bytes, fileData.length);
        CMSDecoderFinalizeMessage(decoder);
        CMSDecoderCopyContent(decoder, &dataRef);
        plistString = [[[NSString alloc] initWithData:(NSData *)dataRef encoding:NSUTF8StringEncoding] autorelease];
        //plist = [plistString propertyList];
    }
    @catch (NSException *exception) {
        printf("Could not decode file.\n");
    }
    @finally {
        if (decoder) CFRelease(decoder);
        if (dataRef) CFRelease(dataRef);
    }

    String value;

    if (plistString)
    {
         value = [plistString UTF8String];
         [plistString release];
    }

    [file release];

    return value;
}

}




