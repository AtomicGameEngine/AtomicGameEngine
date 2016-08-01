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

import JavascriptResourceEditorBuilder from "./resourceEditors/JavascriptResourceEditorBuilder";
import JsonResourceEditorBuilder from "./resourceEditors/JsonResourceEditorBuilder";
import Scene3dResourceEditorBuilder from "./resourceEditors/Scene3dResourceEditorBuilder";
import TextFileResourceEditorBuilder from "./resourceEditors/TextFileResourceEditorBuilder";
import TurboBadgerResourceEditorBuilder from "./resourceEditors/TurboBadgerResourceEditorBuilder";
import TypescriptResourceEditorBuilder from "./resourceEditors/TypescriptResourceEditorBuilder";
import CSharpResourceEditorBuilder from "./resourceEditors/CSharpResourceEditorBuilder";
import XMLResourceEditorBuilder from "./resourceEditors/XMLResourceEditorBuilder";
import ShaderResourceEditorBuilder from "./resourceEditors/ShaderResourceEditorBuilder";
import VisualStudioResourceEditorBuilder from "./resourceEditors/VisualStudioResourceEditorBuilder";

export default class ResourceEditorProvider {
    private standardEditorRegistry: Editor.Extensions.ResourceEditorBuilder[] = [];
    private customEditorRegistry: Editor.Extensions.ResourceEditorBuilder[] = [];
    private resourceFrame: Atomic.UIWidget;

    constructor(resourceFrame: Atomic.UIWidget) {
        this.resourceFrame = resourceFrame;
    }

    /**
     * Register an internal core editor.
     */
    registerStandardEditor(editorBuilder: Editor.Extensions.ResourceEditorBuilder) {
        this.standardEditorRegistry.push(editorBuilder);
    }

    /**
     * Register a custom editor.  These editors will override editors in the standard editor list if
     * they both resolve the ```canHandleResource``` call.
     */
    registerCustomEditor(editorBuilder: Editor.Extensions.ResourceEditorBuilder) {
        this.customEditorRegistry.push(editorBuilder);
    }

    /**
     * Will unregister a previously registered editor builder
     * @param  {Editor.Extensions.ResourceEditorBuilder} editorBuilder
     */
    unregisterCustomEditor(editorBuilder: Editor.Extensions.ResourceEditorBuilder) {
        var index = this.customEditorRegistry.indexOf(editorBuilder, 0);
        if (index > -1) {
            this.customEditorRegistry.splice(index, 1);
        }
    }

    /**
     * Returns an editor for the provided resource type or null
     */
    getEditor(resourcePath: string, tabContainer) {
        let editorBuilder: Editor.Extensions.ResourceEditorBuilder;
        this.customEditorRegistry.forEach((builder) => {
            if (builder.canHandleResource(resourcePath)) {
                editorBuilder = builder;
                return;
            }
        });

        if (!editorBuilder) {
            this.standardEditorRegistry.forEach((builder) => {
                if (builder.canHandleResource(resourcePath)) {
                    editorBuilder = builder;
                    return;
                }
            });
        }

        if (editorBuilder) {
            return editorBuilder.getEditor(this.resourceFrame, resourcePath, tabContainer);
        } else {
            return null;
        }
    }

    /**
     * Loads the built-in standard editors
     */
    loadStandardEditors() {
        this.registerStandardEditor(new TextFileResourceEditorBuilder());
        this.registerStandardEditor(new JavascriptResourceEditorBuilder());
        this.registerStandardEditor(new JsonResourceEditorBuilder());
        this.registerStandardEditor(new CSharpResourceEditorBuilder());
        this.registerStandardEditor(new TypescriptResourceEditorBuilder());
        this.registerStandardEditor(new Scene3dResourceEditorBuilder());
        this.registerStandardEditor(new XMLResourceEditorBuilder());
        this.registerStandardEditor(new ShaderResourceEditorBuilder());

        // this overrides the test resource editor so need to put it in the custom bucket
        this.registerCustomEditor(new TurboBadgerResourceEditorBuilder());

        this.registerCustomEditor(new VisualStudioResourceEditorBuilder());
    }
}
