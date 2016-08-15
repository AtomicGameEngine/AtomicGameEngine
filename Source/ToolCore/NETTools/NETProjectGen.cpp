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

    NETProjectBase::NETProjectBase(Context* context, NETProjectGen* projectGen) :
        Object(context), xmlFile_(new XMLFile(context)), projectGen_(projectGen)
    {

    }

    NETProjectBase::~NETProjectBase()
    {

    }

    void NETProjectBase::ReplacePathStrings(String& path)
    {
        ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();

        String atomicRoot = tenv->GetRootSourceDir();
        atomicRoot = RemoveTrailingSlash(atomicRoot);
        
        const String& scriptPlatform = projectGen_->GetScriptPlatform();

        path.Replace("$ATOMIC_ROOT$", atomicRoot, false);
        path.Replace("$SCRIPT_PLATFORM$", scriptPlatform, false);

    }

    NETCSProject::NETCSProject(Context* context, NETProjectGen* projectGen) : NETProjectBase(context, projectGen)
    {

    }

    NETCSProject::~NETCSProject()
    {

    }

    bool NETCSProject::CreateProjectFolder(const String& path)
    {
        FileSystem* fileSystem = GetSubsystem<FileSystem>();

        if (fileSystem->DirExists(path))
            return true;

        fileSystem->CreateDirsRecursive(path);

        if (!fileSystem->DirExists(path))
        {
            ATOMIC_LOGERRORF("Unable to create dir: %s", path.CString());
            return false;
        }

        return true;
    }


    void NETCSProject::CreateCompileItemGroup(XMLElement &projectRoot)
    {
        FileSystem* fs = GetSubsystem<FileSystem>();

        XMLElement igroup = projectRoot.CreateChild("ItemGroup");

        // Compile AssemblyInfo.cs
        igroup.CreateChild("Compile").SetAttribute("Include", "Properties\\AssemblyInfo.cs");

        for (unsigned i = 0; i < sourceFolders_.Size(); i++)
        {
            const String& sourceFolder = sourceFolders_[i];

            Vector<String> result;
            fs->ScanDir(result, sourceFolder, "*.cs", SCAN_FILES, true);

            for (unsigned j = 0; j < result.Size(); j++)
            {
                XMLElement compile = igroup.CreateChild("Compile");
                
                String path = sourceFolder + result[j];

                String relativePath;
                
                if (GetRelativePath(projectPath_, GetPath(path), relativePath))
                {
                    path = relativePath + GetFileName(path) + GetExtension(path);
                }

                // IMPORTANT: / Slash direction breaks intellisense :/
                path.Replace('/', '\\');

                compile.SetAttribute("Include", path.CString());

                // put generated files into generated folder
                if (sourceFolder.Contains("Generated") && sourceFolder.Contains("CSharp") && sourceFolder.Contains("Packages"))
                {
                    compile.CreateChild("Link").SetValue("Generated\\" + result[j]);
                }
                else
                {
                    compile.CreateChild("Link").SetValue(result[j]);
                }

            }

        }

    }

    void NETProjectBase::CopyXMLElementRecursive(XMLElement source, XMLElement dest)
    {
        Vector<String> attrNames = source.GetAttributeNames();

        for (unsigned i = 0; i < attrNames.Size(); i++)
        {
            String value = source.GetAttribute(attrNames[i]);
            dest.SetAttribute(attrNames[i], value);
        }

        dest.SetValue(source.GetValue());

        XMLElement child = source.GetChild();

        while (child.NotNull() && child.GetName().Length())
        {
            XMLElement childDest = dest.CreateChild(child.GetName());
            CopyXMLElementRecursive(child, childDest);
            child = child.GetNext();
        }
    }

    void NETCSProject::CreateReferencesItemGroup(XMLElement &projectRoot)
    {

        XMLElement xref;
        XMLElement igroup = projectRoot.CreateChild("ItemGroup");

        for (unsigned i = 0; i < references_.Size(); i++)
        {
            String ref = references_[i];

            // project reference
            if (projectGen_->GetCSProjectByName(ref))
                continue;

            // NuGet project
            if (ref.StartsWith("<"))
            {
                XMLFile xmlFile(context_);

                if (!xmlFile.FromString(ref))
                {
                    ATOMIC_LOGERROR("NETCSProject::CreateReferencesItemGroup - Unable to parse reference XML");
                }
                

                xref = igroup.CreateChild("Reference");
                CopyXMLElementRecursive(xmlFile.GetRoot(), xref);
                continue;
            }

            xref = igroup.CreateChild("Reference");
            xref.SetAttribute("Include", ref);

        }

        Project* project = projectGen_->GetAtomicProject();

        if (project)
        {
            Vector<String> result;
            GetSubsystem<FileSystem>()->ScanDir(result, project->GetResourcePath(), "*.dll", SCAN_FILES, true);

            for (unsigned j = 0; j < result.Size(); j++)
            {
                String path = project->GetResourcePath() + result[j];

                String relativePath;

                if (GetRelativePath(projectPath_, GetPath(path), relativePath))
                {
                    if (projectGen_->GetCSProjectByName(GetFileName(path)))
                        continue;

                    path = relativePath + GetFileName(path) + GetExtension(path);
                }

                xref = igroup.CreateChild("Reference");
                xref.SetAttribute("Include", path);
            }

        }

    }

    void NETCSProject::CreateProjectReferencesItemGroup(XMLElement &projectRoot)
    {

        XMLElement igroup = projectRoot.CreateChild("ItemGroup");

        for (unsigned i = 0; i < references_.Size(); i++)
        {
            const String& ref = references_[i];
            NETCSProject* project = projectGen_->GetCSProjectByName(ref);

            if (!project)
                continue;


            XMLElement projectRef = igroup.CreateChild("ProjectReference");
            projectRef.SetAttribute("Include", ToString("..\\%s\\%s.csproj", ref.CString(), ref.CString()));

            XMLElement xproject = projectRef.CreateChild("Project");
            xproject.SetValue(ToString("{%s}", project->GetProjectGUID().ToLower().CString()));

            XMLElement xname = projectRef.CreateChild("Name");
            xname.SetValue(project->GetName());
        }
    }


    void NETCSProject::CreatePackagesItemGroup(XMLElement &projectRoot)
    {
        if (!packages_.Size())
            return;

        XMLElement xref;
        XMLElement igroup = projectRoot.CreateChild("ItemGroup");
        xref = igroup.CreateChild("None");
        xref.SetAttribute("Include", "packages.config");

        XMLFile packageConfig(context_);

        XMLElement packageRoot = packageConfig.CreateRoot("packages");

        for (unsigned i = 0; i < packages_.Size(); i++)
        {
            XMLFile xmlFile(context_);
            if (!xmlFile.FromString(packages_[i]))
            {
                ATOMIC_LOGERROR("NETCSProject::CreatePackagesItemGroup - Unable to parse package xml");
            }

            xref = packageRoot.CreateChild("package");

            CopyXMLElementRecursive(xmlFile.GetRoot(), xref);
        }

        SharedPtr<File> output(new File(context_, projectPath_ + "packages.config", FILE_WRITE));
        String source = packageConfig.ToString();
        output->Write(source.CString(), source.Length());

    }

    void NETCSProject::GetAssemblySearchPaths(String& paths)
    {
        paths.Clear();

        ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();

        Vector<String> searchPaths;

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
        pgroup.CreateChild("OutputPath").SetValue(assemblyOutputPath_ + "Release\\");

        pgroup.CreateChild("DefineConstants").SetValue("TRACE");
        pgroup.CreateChild("ErrorReport").SetValue("prompt");
        pgroup.CreateChild("WarningLevel").SetValue("4");
        pgroup.CreateChild("ConsolePause").SetValue("false");
        pgroup.CreateChild("AllowUnsafeBlocks").SetValue("true");
        pgroup.CreateChild("PlatformTarget").SetValue("x64");

    }

    void NETCSProject::CreateDebugPropertyGroup(XMLElement &projectRoot)
    {
        XMLElement pgroup = projectRoot.CreateChild("PropertyGroup");
        pgroup.SetAttribute("Condition", " '$(Configuration)|$(Platform)' == 'Debug|AnyCPU' ");

        pgroup.CreateChild("DebugSymbols").SetValue("true");
        pgroup.CreateChild("DebugType").SetValue("full");
        pgroup.CreateChild("Optimize").SetValue("false");
        pgroup.CreateChild("OutputPath").SetValue(assemblyOutputPath_ + "Debug\\");
        pgroup.CreateChild("DefineConstants").SetValue("DEBUG;TRACE");
        pgroup.CreateChild("ErrorReport").SetValue("prompt");
        pgroup.CreateChild("WarningLevel").SetValue("4");
        pgroup.CreateChild("ConsolePause").SetValue("false");
        pgroup.CreateChild("AllowUnsafeBlocks").SetValue("true");
        pgroup.CreateChild("PlatformTarget").SetValue("x64");

    }

    void NETCSProject::CreateAssemblyInfo()
    {

        String info = "using System.Reflection;\nusing System.Runtime.CompilerServices;\nusing System.Runtime.InteropServices;\n\n\n";
        info += ToString("[assembly:AssemblyTitle(\"%s\")]\n", name_.CString());
        info += "[assembly:AssemblyDescription(\"\")]\n";
        info += "[assembly:AssemblyConfiguration(\"\")]\n";
        info += "[assembly:AssemblyCompany(\"\")]\n";
        info += ToString("[assembly:AssemblyProduct(\"%s\")]\n", name_.CString());

        info += "\n\n\n";

        info += "[assembly:ComVisible(false)]\n";

        info += "\n\n";

        info += ToString("[assembly:Guid(\"%s\")]\n", projectGuid_.CString());

        info += "\n\n";

        info += "[assembly:AssemblyVersion(\"1.0.0.0\")]\n";
        info += "[assembly:AssemblyFileVersion(\"1.0.0.0\")]\n";

        SharedPtr<File> output(new File(context_, projectPath_ + "Properties/AssemblyInfo.cs", FILE_WRITE));
        output->Write(info.CString(), info.Length());

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
        guid.SetValue("{" + projectGuid_ + "}");

        // OutputType
        XMLElement outputType = pgroup.CreateChild("OutputType");
        outputType.SetValue(outputType_);

        pgroup.CreateChild("AppDesignerFolder").SetValue("Properties");

        // RootNamespace
        XMLElement rootNamespace = pgroup.CreateChild("RootNamespace");
        rootNamespace.SetValue(rootNamespace_);

        // AssemblyName
        XMLElement assemblyName = pgroup.CreateChild("AssemblyName");
        assemblyName.SetValue(assemblyName_);

        // TargetFrameworkVersion
        XMLElement targetFrameWork = pgroup.CreateChild("TargetFrameworkVersion");
        targetFrameWork.SetValue("v4.6");

        pgroup.CreateChild("FileAlignment").SetValue("512");

    }

    bool NETCSProject::Generate()
    {
        FileSystem* fileSystem = GetSubsystem<FileSystem>();
        NETSolution* solution = projectGen_->GetSolution();
        ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();

        projectPath_ = solution->GetOutputPath() + name_ + "/";

        if (!CreateProjectFolder(projectPath_))
            return false;

        if (!CreateProjectFolder(projectPath_ + "Properties"))
            return false;

        XMLElement project = xmlFile_->CreateRoot("Project");

        project.SetAttribute("DefaultTargets", "Build");
        project.SetAttribute("ToolsVersion", "14.0");
        project.SetAttribute("DefaultTargets", "Build");
        project.SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");

        XMLElement import = project.CreateChild("Import");
        import.SetAttribute("Project", "$(MSBuildExtensionsPath)\\$(MSBuildToolsVersion)\\Microsoft.Common.props");
        import.SetAttribute("Condition", "Exists('$(MSBuildExtensionsPath)\\$(MSBuildToolsVersion)\\Microsoft.Common.props')");

        CreateMainPropertyGroup(project);
        CreateDebugPropertyGroup(project);
        CreateReleasePropertyGroup(project);
        CreateReferencesItemGroup(project);
        CreateProjectReferencesItemGroup(project);
        CreateCompileItemGroup(project);
        CreatePackagesItemGroup(project);
        CreateAssemblyInfo();

        project.CreateChild("Import").SetAttribute("Project", "$(MSBuildToolsPath)\\Microsoft.CSharp.targets");

        Project* atomicProject = projectGen_->GetAtomicProject();

        if (atomicProject)
        {
            XMLElement afterBuild = project.CreateChild("Target");
            afterBuild.SetAttribute("Name", "AfterBuild");
            XMLElement copy = afterBuild.CreateChild("Copy");
            copy.SetAttribute("SourceFiles", "$(TargetPath)");

            String destPath = projectPath_ + "../../../Resources/";
            String relativePath;            

            if (GetRelativePath(projectPath_, atomicProject->GetResourcePath(), relativePath))
            {
                destPath = AddTrailingSlash(relativePath);
            }

            copy.SetAttribute("DestinationFolder", destPath);

            // Create the AtomicProject.csproj.user file if it doesn't exist
            String userSettingsFilename = projectPath_ + name_ + ".csproj.user";
            if (!fileSystem->FileExists(userSettingsFilename))
            {
                SharedPtr<XMLFile> userSettings(new XMLFile(context_));

                XMLElement project = userSettings->CreateRoot("Project");
                
                //XMLElement xml = userRoot.CreateChild("?xml");
                //xml.SetAttribute("version", "1.0");
                //xml.SetAttribute("encoding", "utf-8");

                project.SetAttribute("ToolsVersion", "14.0");
                project.SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");

                StringVector configs;
                configs.Push("Debug");
                configs.Push("Release");

                for (unsigned i = 0; i < configs.Size(); i++)
                {
                    String cfg = configs[i];

                    XMLElement propertyGroup = project.CreateChild("PropertyGroup");
                    propertyGroup.SetAttribute("Condition", ToString("'$(Configuration)|$(Platform)' == '%s|AnyCPU'", cfg.CString()));

                    String startArguments;

#ifdef ATOMIC_DEV_BUILD
                    String playerBin = tenv->GetAtomicNETRootDir() + cfg + "/AtomicIPCPlayer.exe";
#else
                    String playerBin = tenv->GetAtomicNETRootDir() + "Release/AtomicIPCPlayer.exe";

                    startArguments += ToString("--resourcePrefix \"%s\" ", (fileSystem->GetProgramDir() + "Resources/").CString());

#endif

                    propertyGroup.CreateChild("StartAction").SetValue("Program");                    
                    propertyGroup.CreateChild("StartProgram").SetValue(playerBin );

                    startArguments += ToString("--project \"%s\"", atomicProject->GetProjectPath().CString());

                    propertyGroup.CreateChild("StartArguments").SetValue(startArguments);
                    
                }

                String userSettingsSource = userSettings->ToString();
                SharedPtr<File> output(new File(context_, userSettingsFilename, FILE_WRITE));
                output->Write(userSettingsSource.CString(), userSettingsSource.Length());
                output->Close();

            }
            
        }

        String projectSource = xmlFile_->ToString();

        SharedPtr<File> output(new File(context_, projectPath_ + name_ + ".csproj", FILE_WRITE));
        output->Write(projectSource.CString(), projectSource.Length());

        return true;
    }

    bool NETCSProject::Load(const JSONValue& root)
    {
        name_ = root["name"].GetString();

        projectGuid_ = projectGen_->GenerateUUID();

        outputType_ = root["outputType"].GetString();

        rootNamespace_ = root["rootNamespace"].GetString();
        assemblyName_ = root["assemblyName"].GetString();
        assemblyOutputPath_ = root["assemblyOutputPath"].GetString();
        ReplacePathStrings(assemblyOutputPath_);

        assemblySearchPaths_ = root["assemblySearchPaths"].GetString();

        ReplacePathStrings(assemblySearchPaths_);

        const JSONArray& references = root["references"].GetArray();

        for (unsigned i = 0; i < references.Size(); i++)
        {
            String reference = references[i].GetString();
            ReplacePathStrings(reference);
            references_.Push(reference);
        }

        const JSONArray& packages = root["packages"].GetArray();

        for (unsigned i = 0; i < packages.Size(); i++)
        {
            String package = packages[i].GetString();

            if (packages_.Find(package) != packages_.End())
            {
                ATOMIC_LOGERRORF("Duplicate package found %s", package.CString());
                continue;
            }

            projectGen_->GetSolution()->RegisterPackage(package);

            packages_.Push(package);
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

    NETSolution::NETSolution(Context* context, NETProjectGen* projectGen, bool rewrite) : NETProjectBase(context, projectGen),
       rewriteSolution_(rewrite)
    {

    }

    NETSolution::~NETSolution()
    {

    }

    bool NETSolution::Generate()
    {

        String slnPath = outputPath_ + name_ + ".sln";

        GenerateSolution(slnPath);

        return true;
    }

    void NETSolution::GenerateSolution(const String &slnPath)
    {
        String source = "Microsoft Visual Studio Solution File, Format Version 12.00\n";
        source += "# Visual Studio 14\n";
        source += "VisualStudioVersion = 14.0.25420.1\n";
        source += "MinimumVisualStudioVersion = 10.0.40219.1\n";

        solutionGUID_ = projectGen_->GenerateUUID();

        PODVector<NETCSProject*> depends;
        const Vector<SharedPtr<NETCSProject>>& projects = projectGen_->GetCSProjects();

        for (unsigned i = 0; i < projects.Size(); i++)
        {
            NETCSProject* p = projects.At(i);

            const String& projectName = p->GetName();
            const String& projectGUID = p->GetProjectGUID();

            source += ToString("Project(\"{%s}\") = \"%s\", \"%s\\%s.csproj\", \"{%s}\"\n",
                solutionGUID_.CString(), projectName.CString(), projectName.CString(),
                projectName.CString(), projectGUID.CString());

            projectGen_->GetCSProjectDependencies(p, depends);

            if (depends.Size())
            {
                source += "\tProjectSection(ProjectDependencies) = postProject\n";

                for (unsigned j = 0; j < depends.Size(); j++)
                {
                    source += ToString("\t{%s} = {%s}\n",
                        depends[j]->GetProjectGUID().CString(), depends[j]->GetProjectGUID().CString());
                }

                source += "\tEndProjectSection\n";
            }

            source += "\tEndProject\n";
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

            source += ToString("        {%s}.Debug|Any CPU.ActiveCfg = Debug|Any CPU\n", p->GetProjectGUID().CString());
            source += ToString("        {%s}.Debug|Any CPU.Build.0 = Debug|Any CPU\n", p->GetProjectGUID().CString());
            source += ToString("        {%s}.Release|Any CPU.ActiveCfg = Release|Any CPU\n", p->GetProjectGUID().CString());
            source += ToString("        {%s}.Release|Any CPU.Build.0 = Release|Any CPU\n", p->GetProjectGUID().CString());
        }

        source += "    EndGlobalSection\n";

        source += "EndGlobal\n";

        if (!rewriteSolution_)
        { 
            FileSystem* fileSystem = GetSubsystem<FileSystem>();
            if (fileSystem->Exists(slnPath))
                return;
        }
        
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
            fs->CreateDirsRecursive(outputPath_);

        return true;
    }

    bool NETSolution::RegisterPackage(const String& package)
    {
        if (packages_.Find(package) != packages_.End())
            return false;
        
        packages_.Push(package);

        return true;
    }

    NETProjectGen::NETProjectGen(Context* context) : Object(context),
        rewriteSolution_(false)
    {

    }

    NETProjectGen::~NETProjectGen()
    {

    }

    NETCSProject* NETProjectGen::GetCSProjectByName(const String & name)
    {

        for (unsigned i = 0; i < projects_.Size(); i++)
        {
            if (projects_[i]->GetName() == name)
                return projects_[i];
        }

        return nullptr;
    }

    bool NETProjectGen::GetCSProjectDependencies(NETCSProject* source, PODVector<NETCSProject*>& depends) const
    {
        depends.Clear();

        const Vector<String>& references = source->GetReferences();

        for (unsigned i = 0; i < projects_.Size(); i++)
        {
            NETCSProject* pdepend = projects_.At(i);

            if (source == pdepend)
                continue;

            for (unsigned j = 0; j < references.Size(); j++)
            {
                if (pdepend->GetName() == references[j])
                {
                    depends.Push(pdepend);
                }
            }
        }

        return depends.Size() != 0;

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

    void NETProjectGen::SetRewriteSolution(bool rewrite)
    {
        rewriteSolution_ = rewrite;

        if (solution_.NotNull())
            solution_->SetRewriteSolution(rewrite);
    }

    bool NETProjectGen::LoadProject(const JSONValue &root)
    {

        solution_ = new NETSolution(context_, this, rewriteSolution_);

        solution_->Load(root["solution"]);

        const JSONValue& jprojects = root["projects"];

        if (!jprojects.IsArray() || !jprojects.Size())
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

    bool NETProjectGen::LoadProject(const String& projectPath)
    {
        SharedPtr<File> file(new File(context_));

        if (!file->Open(projectPath))
            return false;

        String json;
        file->ReadText(json);

        JSONValue jvalue;

        if (!JSONFile::ParseJSON(json, jvalue))
            return false;

        return LoadProject(jvalue);
    }

    bool NETProjectGen::LoadProject(Project* project)
    {
        FileSystem* fileSystem = GetSubsystem<FileSystem>();
        ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();

        atomicProject_ = project;

        JSONValue root;
        JSONValue solution;

        solution["name"] = "AtomicProject";
        solution["outputPath"] = AddTrailingSlash(project->GetProjectPath()) + "AtomicNET/Solution/";

        JSONArray projects;

        JSONObject jproject;
        jproject["name"] = "AtomicProject";
        jproject["outputType"] = "Library";
        jproject["assemblyName"] = "AtomicProject";
        jproject["assemblyOutputPath"] = AddTrailingSlash(project->GetProjectPath()) + "AtomicNET/Bin/";

        JSONArray references;
        references.Push(JSONValue("System"));
        references.Push(JSONValue("System.Core"));
        references.Push(JSONValue("System.Xml.Linq"));
        references.Push(JSONValue("System.XML"));

        String atomicNETAssembly = tenv->GetAtomicNETCoreAssemblyDir() + "AtomicNET.dll";

        if (!fileSystem->FileExists(atomicNETAssembly))
        {
            ATOMIC_LOGERRORF("NETProjectGen::LoadProject - AtomicNET assembly does not exist: %s", atomicNETAssembly.CString());
            return false;
        }

        references.Push(JSONValue(atomicNETAssembly));        

        jproject["references"] = references;

        JSONArray sources;
        sources.Push(JSONValue(ToString("%s", project->GetResourcePath().CString())));

        jproject["sources"] = sources;

        projects.Push(jproject);

        root["projects"] = projects;
        root["solution"] = solution;
        
        return LoadProject(root);
    }

    bool NETProjectGen::GetRequiresNuGet()
    {
        if (solution_.Null())
        {
            ATOMIC_LOGERROR("NETProjectGen::GetRequiresNuGet() - called without a solution loaded");
            return false;
        }

        return solution_->GetPackages().Size() != 0;

    }


    String NETProjectGen::GenerateUUID()
    {
        Poco::UUIDGenerator& generator = Poco::UUIDGenerator::defaultGenerator();
        Poco::UUID uuid(generator.create()); // time based
        return String(uuid.toString().c_str()).ToUpper();
    }

}
