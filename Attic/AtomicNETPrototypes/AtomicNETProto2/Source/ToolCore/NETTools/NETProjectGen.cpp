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

#include <Poco/UUID.h>
#include <Poco/UUIDGenerator.h>

#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>

#include "../ToolEnvironment.h"
#include "../ToolSystem.h"
#include "../Project/Project.h"
#include "NETProjectGen.h"

namespace ToolCore
{

NETProjectBase::NETProjectBase(Context* context, NETProjectGen* projectGen):
    Object(context), xmlFile_(new XMLFile(context)), projectGen_(projectGen)
{

}

NETProjectBase::~NETProjectBase()
{

}

void NETProjectBase::ReplacePathStrings(String& path)
{
    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();
    ToolSystem* tsys = GetSubsystem<ToolSystem>();

    const String& atomicRoot = tenv->GetRootSourceDir();
    const String& scriptPlatform = projectGen_->GetScriptPlatform();

    path.Replace("$ATOMIC_ROOT$", atomicRoot, false);
    path.Replace("$SCRIPT_PLATFORM$", scriptPlatform, false);

    Project* project = tsys->GetProject();
    if (project)
    {
        path.Replace("$PROJECT_ROOT$", project->GetProjectPath(), false);
    }

}

NETCSProject::NETCSProject(Context* context, NETProjectGen* projectGen) : NETProjectBase(context, projectGen)
{

}

NETCSProject::~NETCSProject()
{

}

void NETCSProject::CreateCompileItemGroup(XMLElement &projectRoot)
{
    FileSystem* fs = GetSubsystem<FileSystem>();

    XMLElement igroup = projectRoot.CreateChild("ItemGroup");

    for (unsigned i = 0; i < sourceFolders_.Size(); i++)
    {
        const String& sourceFolder = sourceFolders_[i];

        Vector<String> result;
        fs->ScanDir(result, sourceFolder, "*.cs", SCAN_FILES, true);

        for (unsigned j = 0; j < result.Size(); j++)
        {
            XMLElement compile = igroup.CreateChild("Compile");

            compile.SetAttribute("Include", sourceFolder + result[j]);

            // put generated files into generated folder
            if (sourceFolder.Contains("Generated") && sourceFolder.Contains("CSharp") && sourceFolder.Contains("Packages") )
            {
                compile.CreateChild("Link").SetValue("Generated\\" + result[j]);
            }

        }

    }

}

void NETCSProject::CreateReferencesItemGroup(XMLElement &projectRoot)
{
    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();
    FileSystem* fs = GetSubsystem<FileSystem>();

    const String& coreCLRAbsPath = tenv->GetNETCoreCLRAbsPath();

    XMLElement igroup = projectRoot.CreateChild("ItemGroup");

    XMLElement mscorlibref = igroup.CreateChild("Reference");
    mscorlibref.SetAttribute("Include", "mscorlib");
    mscorlibref.CreateChild("HintPath").SetValue(coreCLRAbsPath + "mscorlib.dll");
    mscorlibref.CreateChild("Private").SetValue("False");

    for (unsigned i = 0; i < references_.Size(); i++)
    {
        String ref = references_[i];
        String refpath = ref + ".dll";

        // we explicitly add mscorlib
        if (ref == "mscorlib")
            continue;

        XMLElement xref = igroup.CreateChild("Reference");
        xref.SetAttribute("Include", ref);

        // if we're a coreclr assembly, qualify it
        if (fs->FileExists(coreCLRAbsPath + refpath))
        {
            refpath = coreCLRAbsPath + refpath;
        }

        xref.CreateChild("HintPath").SetValue(refpath);
        xref.CreateChild("Private").SetValue("False");

    }
}

void NETCSProject::GetAssemblySearchPaths(String& paths)
{
    paths.Clear();

    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();
    const String& coreCLRAbsPath = tenv->GetNETCoreCLRAbsPath();

    Vector<String> searchPaths;
    searchPaths.Push(coreCLRAbsPath);

    if (assemblySearchPaths_.Length())
        searchPaths.Push(assemblySearchPaths_);

    paths.Join(searchPaths, ";");
}

void NETCSProject::CreateReleasePropertyGroup(XMLElement &projectRoot)
{
    XMLElement pgroup = projectRoot.CreateChild("PropertyGroup");
    pgroup.SetAttribute("Condition", " '$(Configuration)|$(Platform)' == 'Release|AnyCPU' ");

    pgroup.CreateChild("DebugType").SetValue("full");
    pgroup.CreateChild("Optimize").SetValue("true");
    pgroup.CreateChild("OutputPath").SetValue(assemblyOutputPath_);
    pgroup.CreateChild("ErrorReport").SetValue("prompt");
    pgroup.CreateChild("WarningLevel").SetValue("4");
    pgroup.CreateChild("ConsolePause").SetValue("false");
    pgroup.CreateChild("AllowUnsafeBlocks").SetValue("true");
    pgroup.CreateChild("NoStdLib").SetValue("true");
    pgroup.CreateChild("NoConfig").SetValue("true");
    pgroup.CreateChild("NoCompilerStandardLib").SetValue("true");

    String assemblySearchPaths;
    GetAssemblySearchPaths(assemblySearchPaths);
    pgroup.CreateChild("AssemblySearchPaths").SetValue(assemblySearchPaths);

    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();
    const String& editorBinary = tenv->GetEditorBinary();

    if (!projectGen_->GetGameBuild())
    {
        XMLElement command = pgroup.CreateChild("CustomCommands").CreateChild("CustomCommands").CreateChild("Command");
        command.SetAttribute("type", "Execute");
        command.SetAttribute("command", editorBinary.CString());
    }
}

void NETCSProject::CreateDebugPropertyGroup(XMLElement &projectRoot)
{
    XMLElement pgroup = projectRoot.CreateChild("PropertyGroup");
    pgroup.SetAttribute("Condition", " '$(Configuration)|$(Platform)' == 'Debug|AnyCPU' ");

    pgroup.CreateChild("DebugSymbols").SetValue("true");
    pgroup.CreateChild("DebugType").SetValue("full");
    pgroup.CreateChild("Optimize").SetValue("false");
    pgroup.CreateChild("OutputPath").SetValue(assemblyOutputPath_);
    pgroup.CreateChild("DefineConstants").SetValue("DEBUG;");
    pgroup.CreateChild("ErrorReport").SetValue("prompt");
    pgroup.CreateChild("WarningLevel").SetValue("4");
    pgroup.CreateChild("ConsolePause").SetValue("false");
    pgroup.CreateChild("AllowUnsafeBlocks").SetValue("true");
    pgroup.CreateChild("NoStdLib").SetValue("true");
    pgroup.CreateChild("NoConfig").SetValue("true");
    pgroup.CreateChild("NoCompilerStandardLib").SetValue("true");

    String assemblySearchPaths;
    GetAssemblySearchPaths(assemblySearchPaths);
    pgroup.CreateChild("AssemblySearchPaths").SetValue(assemblySearchPaths);

    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();
    const String& editorBinary = tenv->GetEditorBinary();

    if (!projectGen_->GetGameBuild())
    {
        XMLElement command = pgroup.CreateChild("CustomCommands").CreateChild("CustomCommands").CreateChild("Command");
        command.SetAttribute("type", "Execute");
        command.SetAttribute("command", editorBinary.CString());
    }

}

void NETCSProject::CreateMainPropertyGroup(XMLElement& projectRoot)
{
    XMLElement pgroup = projectRoot.CreateChild("PropertyGroup");

    // Configuration
    XMLElement config = pgroup.CreateChild("Configuration");
    config.SetAttribute("Condition", " '$(Configuration)' == '' ");
    config.SetValue("Debug");

    // Platform
    XMLElement platform = pgroup.CreateChild("Platform");
    platform.SetAttribute("Condition", " '$(Platform)' == '' ");
    platform.SetValue("AnyCPU");

    // ProjectGuid
    XMLElement guid = pgroup.CreateChild("ProjectGuid");
    guid.SetValue(projectGuid_);

    // OutputType
    XMLElement outputType = pgroup.CreateChild("OutputType");
    outputType.SetValue(outputType_);

    // RootNamespace
    XMLElement rootNamespace = pgroup.CreateChild("RootNamespace");
    rootNamespace.SetValue(rootNamespace_);

    // AssemblyName
    XMLElement assemblyName = pgroup.CreateChild("AssemblyName");
    assemblyName.SetValue(assemblyName_);

    // TargetFrameworkVersion
    XMLElement targetFrameWork = pgroup.CreateChild("TargetFrameworkVersion");
    targetFrameWork.SetValue("4.5");

}

bool NETCSProject::Generate()
{
    XMLElement project = xmlFile_->CreateRoot("Project");

    project.SetAttribute("DefaultTargets", "Build");
    project.SetAttribute("ToolsVersion", "4.0");
    project.SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");

    CreateMainPropertyGroup(project);
    CreateDebugPropertyGroup(project);
    CreateReleasePropertyGroup(project);
    CreateReferencesItemGroup(project);
    CreateCompileItemGroup(project);

    project.CreateChild("Import").SetAttribute("Project", "$(MSBuildBinPath)\\Microsoft.CSharp.targets");

    // on msbuild this seems to stop referencing of framework assemblies
    project.CreateChild("Target").SetAttribute("Name", "GetReferenceAssemblyPaths");
    project.CreateChild("Target").SetAttribute("Name", "GetFrameworkPaths");

    String projectSource = xmlFile_->ToString();

    NETSolution* solution = projectGen_->GetSolution();

    String projectPath = solution->GetOutputPath() + name_ + ".csproj";

    SharedPtr<File> output(new File(context_, projectPath, FILE_WRITE));
    output->Write(projectSource.CString(), projectSource.Length());

    return true;
}

bool NETCSProject::Load(const JSONValue& root)
{
    bool gameBuild = projectGen_->GetGameBuild();

    name_ = root["name"].GetString();

    projectGuid_ = projectGen_->GenerateUUID();

    if (gameBuild)
        outputType_ = "Library";
    else
        outputType_ = root["outputType"].GetString();

    rootNamespace_ = root["rootNamespace"].GetString();
    assemblyName_ = root["assemblyName"].GetString();
    assemblyOutputPath_ = root["assemblyOutputPath"].GetString();
    ReplacePathStrings(assemblyOutputPath_);

    assemblySearchPaths_ = root["assemblySearchPaths"].GetString();

    if (gameBuild)
    {
        ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();
        const String& engineAssemblyPath = tenv->GetAtomicNETEngineAssemblyPath();
        if (assemblySearchPaths_.Length())
        {
            assemblySearchPaths_ += ";";
            assemblySearchPaths_ += engineAssemblyPath;
        }
        else
        {
            assemblySearchPaths_ = engineAssemblyPath;
        }

    }

    ReplacePathStrings(assemblySearchPaths_);

    const JSONArray& references = root["references"].GetArray();

    for (unsigned i = 0; i < references.Size(); i++)
    {
        String reference = references[i].GetString();
        ReplacePathStrings(reference);
        references_.Push(reference);
    }

    if (gameBuild)
    {
        references_.Push("AtomicNETEngine");
    }

    // msvc doesn't like including these
    if (projectGen_->GetMonoBuild())
    {
        references_.Push("System.Console");
        references_.Push("System.IO");
        references_.Push("System.IO.FileSystem");
    }

    const JSONArray& sources = root["sources"].GetArray();

    for (unsigned i = 0; i < sources.Size(); i++)
    {
        String source = sources[i].GetString();
        ReplacePathStrings(source);
        sourceFolders_.Push(AddTrailingSlash(source));
    }

    return true;
}

NETSolution::NETSolution(Context* context, NETProjectGen* projectGen) : NETProjectBase(context, projectGen)
{

}

NETSolution::~NETSolution()
{

}

bool NETSolution::Generate()
{

    String slnPath = outputPath_ + name_ + ".sln";

    GenerateXamarinStudio(slnPath);

    return true;
}

void NETSolution::GenerateXamarinStudio(const String &slnPath)
{
    String source = "Microsoft Visual Studio Solution File, Format Version 12.00\n";
    source += "# Visual Studio 2012\n";

    const Vector<SharedPtr<NETCSProject>>& projects = projectGen_->GetCSProjects();
    for (unsigned i = 0; i < projects.Size(); i++)
    {
        NETCSProject* p = projects.At(i);

        source += ToString("Project(\"{%s}\") = \"%s\", \"%s.csproj\", \"{%s}\"\n",
                           p->GetProjectGUID().CString(), p->GetName().CString(), p->GetName().CString(),
                           p->GetProjectGUID().CString());

        source += "EndProject\n";
    }

    source += "Global\n";
    source += "    GlobalSection(SolutionConfigurationPlatforms) = preSolution\n";
    source += "        Debug|Any CPU = Debug|Any CPU\n";
    source += "        Release|Any CPU = Release|Any CPU\n";
    source += "    EndGlobalSection\n";
    source += "    GlobalSection(ProjectConfigurationPlatforms) = postSolution\n";

    for (unsigned i = 0; i < projects.Size(); i++)
    {
        NETCSProject* p = projects.At(i);

        source += ToString("        {%s}.Debug|Any CPU.ActiveCfg = Debug|Any CPU\n",  p->GetProjectGUID().CString());
        source += ToString("        {%s}.Debug|Any CPU.Build.0 = Debug|Any CPU\n",  p->GetProjectGUID().CString());
        source += ToString("        {%s}.Release|Any CPU.ActiveCfg = Release|Any CPU\n",  p->GetProjectGUID().CString());
        source += ToString("        {%s}.Release|Any CPU.Build.0 = Release|Any CPU\n",  p->GetProjectGUID().CString());
    }

    source += "    EndGlobalSection\n";

    source += "EndGlobal\n";

    SharedPtr<File> output(new File(context_, slnPath, FILE_WRITE));
    output->Write(source.CString(), source.Length());
    output->Close();
}

bool NETSolution::Load(const JSONValue& root)
{
    FileSystem* fs = GetSubsystem<FileSystem>();

    name_ = root["name"].GetString();

    outputPath_ = AddTrailingSlash(root["outputPath"].GetString());
    ReplacePathStrings(outputPath_);

    // TODO: use poco mkdirs
    if (!fs->DirExists(outputPath_))
        fs->CreateDir(outputPath_);

    return true;
}

NETProjectGen::NETProjectGen(Context* context) : Object(context),
    monoBuild_(false), gameBuild_(false)
{

#ifndef ATOMIC_PLATFORM_WINDOWS
    monoBuild_ = true;
#endif

}

NETProjectGen::~NETProjectGen()
{

}

bool NETProjectGen::Generate()
{
    solution_->Generate();

    for (unsigned i = 0; i < projects_.Size(); i++)
    {
        if (!projects_[i]->Generate())
            return false;
    }
    return true;
}

bool NETProjectGen::LoadProject(const JSONValue &root, bool gameBuild)
{

    gameBuild_ = gameBuild;
    solution_ = new NETSolution(context_, this);

    solution_->Load(root["solution"]);

    const JSONValue& jprojects = root["projects"];

    if (!jprojects.IsArray() || ! jprojects.Size())
        return false;

    for (unsigned i = 0; i < jprojects.Size(); i++)
    {
        const JSONValue& jproject = jprojects[i];

        if (!jproject.IsObject())
            return false;

        SharedPtr<NETCSProject> csProject(new NETCSProject(context_, this));

        if (!csProject->Load(jproject))
            return false;

        projects_.Push(csProject);

    }

    return true;
}

bool NETProjectGen::LoadProject(const String& projectPath, bool gameBuild)
{
    SharedPtr<File> file(new File(context_));

    if (!file->Open(projectPath))
        return false;

    String json;
    file->ReadText(json);

    JSONValue jvalue;

    if (!JSONFile::ParseJSON(json, jvalue))
        return false;

    return LoadProject(jvalue, gameBuild);
}

String NETProjectGen::GenerateUUID()
{
    Poco::UUIDGenerator& generator = Poco::UUIDGenerator::defaultGenerator();
    Poco::UUID uuid(generator.create()); // time based
    return String(uuid.toString().c_str()).ToUpper();
}

}
