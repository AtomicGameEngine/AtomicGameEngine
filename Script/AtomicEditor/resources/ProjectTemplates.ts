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

import EditorUI = require("../ui/EditorUI");
/**
 * Interface around a project template
 */
export interface ProjectTemplateDefinition {
    name: string;
    desc: string;
    screenshot: string;
    templates: [ProjectTemplateDetail];
    folder: string;
    module: string;
}

/**
 * Inner details of the project template.
 */
export interface ProjectTemplateDetail {
    language: string;
    folder: string;
}

/**
 * Returns the structured template definition for the provided project type.
 * Note that the path to the templates folder will be fully resolved
 * @param  {string} projectType A template type.  One of project_empty, project_2d, project_3d
 * @return {ProjectTemplateDefinition}              the template definition for the proved project type
 */
export function getNewProjectTemplateDefinition(projectType: string): ProjectTemplateDefinition {
    var env = ToolCore.toolEnvironment;
    var projectTemplateFolder = env.toolDataDir + "ProjectTemplates/";
    var projectTemplateJsonFile = projectTemplateFolder + "ProjectTemplates.json";
    let jsonFile = new Atomic.File(projectTemplateJsonFile, Atomic.FILE_READ);

    if (!jsonFile.isOpen()) {
        return null;
    }
    // Update all the paths to a more fully qualified path
    let json = JSON.parse(jsonFile.readText());
    let projectTemplate = <ProjectTemplateDefinition>json[projectType];
    if (projectTemplate) {
        projectTemplate.templates.forEach(template => {
            template.folder = projectTemplateFolder + template.folder + "/";
        });
    }

    return projectTemplate;
}

/**
 * Return an array of all of the example project definitions.
 * Note that the paths in both screenshot and folder will be fully resolved.
 * @return {[ProjectTemplateDefinition]} Array of example project definitions.
 */
export function getExampleProjectTemplateDefinitions(): [ProjectTemplateDefinition] {
    let env = ToolCore.toolEnvironment;
    let exampleInfoDir = env.toolDataDir + "ExampleInfo/";
    let exampleJsonFile = exampleInfoDir + "Examples.json";
    let jsonFile = new Atomic.File(exampleJsonFile, Atomic.FILE_READ);

    if (!jsonFile.isOpen()) {
        return;
    }

    let exampleJson = JSON.parse(jsonFile.readText());
    let examples = <[ProjectTemplateDefinition]>exampleJson.examples;

    // Update all the paths to a more fully qualified path
    examples.forEach(example => {
        example.screenshot = exampleInfoDir + example.screenshot;
        example.templates.forEach(template => {
            template.folder = env.toolDataDir + "AtomicExamples/" + template.folder + "/";
        });
    });
    return exampleJson.examples;
}

/**
 * Return an array of all of the file type templates for the provided file type
 * @param  {string} fileTemplateType
 * @return {[FileTemplateDefinition]}
 */
export function GetNewFileTemplateDefinitions(fileTemplateType: string) : Editor.Templates.FileTemplateDefinition[] {
    const templateDefinitions = "AtomicEditor/templates/file_template_definitions.json";
    const file = Atomic.cache.getFile(templateDefinitions);

    if (!file) {
        return [];
    }

    const templates = JSON.parse(file.readText());
    return templates[fileTemplateType] || [];
}
