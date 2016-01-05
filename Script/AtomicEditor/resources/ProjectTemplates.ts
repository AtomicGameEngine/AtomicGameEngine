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
