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

#pragma once

#include <Atomic/Core/Object.h>

#include <Atomic/Resource/XMLFile.h>
#include <Atomic/Resource/JSONFile.h>

using namespace Atomic;

namespace ToolCore
{

    class ProjectSettings;
    class NETProjectGen;

    class NETProjectBase : public Object
    {
        ATOMIC_OBJECT(NETProjectBase, Object)

    public:

        NETProjectBase(Context* context, NETProjectGen* projectGen);
        virtual ~NETProjectBase();

        void ReplacePathStrings(String& path) const;

        void CopyXMLElementRecursive(XMLElement source, XMLElement dest);

    protected:

        SharedPtr<XMLFile> xmlFile_;
        WeakPtr<NETProjectGen> projectGen_;

    };

    class NETCSProject : public NETProjectBase
    {
        friend class NETSolution;

        ATOMIC_OBJECT(NETCSProject, NETProjectBase)

    public:

        NETCSProject(Context* context, NETProjectGen* projectGen);
        virtual ~NETCSProject();

        bool Load(const JSONValue& root);

        const String& GetName() { return name_; }
        const String& GetProjectGUID() { return projectGuid_; }

        const Vector<String>& GetReferences() const { return references_; }
        const Vector<String>& GetPackages() const { return packages_; }

        bool GetIsPCL() const { return projectTypeGuids_.Contains("{786C830F-07A1-408B-BD7F-6EE04809D6DB}"); }

        bool GetIsPlayerApp() const { return playerApplication_; }
        bool SupportsDesktop() const;
        bool SupportsPlatform(const String& platform, bool explicitCheck = true) const;

        /// Returns true if this project is part of core AtomicNET
        bool GetAtomicNETProject() const { return atomicNETProject_;  }

        bool Generate();

    private:

        /// Returns true if this project is part of core AtomicNET
        void SetAtomicNETProject(bool value) { atomicNETProject_ = value; }

        // Portable Class Library
        bool GenerateShared();

        bool GenerateStandard();

        bool GetRelativeProjectPath(const String& fromPath, const String& toPath, String& output) const;

        bool CreateProjectFolder(const String& path);

        void CreateCompileItemGroup(XMLElement &projectRoot);
        void CreateReferencesItemGroup(XMLElement &projectRoot);
        void CreatePackagesItemGroup(XMLElement &projectRoot);
        void CreateMainPropertyGroup(XMLElement &projectRoot);
        void CreateDebugPropertyGroup(XMLElement &projectRoot);
        void CreateReleasePropertyGroup(XMLElement &projectRoot);

        void CreateApplicationItems(XMLElement &projectRoot);
        void CreateAndroidItems(XMLElement &projectRoot);
        void CreateIOSItems(XMLElement &projectRoot);

        void CreateAssemblyInfo();
        void GetAssemblySearchPaths(String& paths);

        void ProcessDefineConstants(StringVector& constants);

        /// Return a relative path to the output folder, config can be Release/Debug/Lib
        String GetRelativeOutputPath(const String& config) const;

        String name_;
        String projectGuid_;
        String outputType_;
        String rootNamespace_;
        String assemblyName_;
        String assemblyOutputPath_;
        String assemblySearchPaths_;
        bool atomicNETProject_;

        // project paths
        String projectPath_;

        XMLElement xmlRoot_;

        Vector<String> platforms_;
        Vector<String> references_;
        Vector<String> packages_;
        Vector<String> sourceFolders_;

        Vector<String> defineConstants_;

        Vector<String> projectTypeGuids_;
        Vector<String> importProjects_;
        Vector<String> libraryProjectZips_;
        Vector<String> transformFiles_;

        String targetFrameworkProfile_;
        Vector<String> sharedReferences_;

        bool genAssemblyDocFile_;

        bool playerApplication_;

        // Android
        bool androidApplication_;

        // iOS
        String objcBindingApiDefinition_;
        String codesignEntitlements_;
        String infoPList_;

        
    };

    class NETSolution : public NETProjectBase
    {
        ATOMIC_OBJECT(NETSolution, NETProjectBase)

    public:

        NETSolution(Context* context, NETProjectGen* projectGen, bool rewrite = false);
        virtual ~NETSolution();

        bool Load(const JSONValue& root);

        bool Generate();

        const String& GetOutputPath() { return outputPath_; }
        String GetOutputFilename() { return outputPath_ + name_ + ".sln"; }

        Vector<String>& GetPackages() { return packages_;  }

        // Registers a NuGet package, returns true if the package hasn't been previously registered
        bool RegisterPackage(const String& package);

        /// If true, the sln file will rewritten if it exists, default is false
        void SetRewriteSolution(bool rewrite) { rewriteSolution_ = rewrite; }


    private:

        void GenerateSolution(const String& slnPath);

        String name_;
        String outputPath_;
        String solutionGUID_;
        Vector<String> packages_;
        bool rewriteSolution_;

    };

    class NETProjectGen : public Object
    {
        ATOMIC_OBJECT(NETProjectGen, Object)

    public:

        NETProjectGen(Context* context);
        virtual ~NETProjectGen();

        NETSolution* GetSolution() { return solution_; }

        const Vector<SharedPtr<NETCSProject>>& GetCSProjects() { return projects_; }

        NETCSProject* GetCSProjectByName(const String& name);

        bool GetCSProjectDependencies(NETCSProject * source, PODVector<NETCSProject*>& depends) const;

        const String& GetAtomicProjectPath() const { return atomicProjectPath_; }

        bool Generate();

        /// Returns true if the generated solution requires NuGet
        bool GetRequiresNuGet();

        String GenerateUUID();

        /// If true, the sln file will rewritten if it exists, default is false
        void SetRewriteSolution(bool rewrite);

        bool LoadJSONProject(const String& jsonProjectPath);
        bool LoadAtomicProject(const String& atomicProjectPath);

        void AddGlobalDefineConstant(const String& constant) { globalDefineConstants_.Push(constant); }
        const Vector<String>& GetGlobalDefineConstants() const { return globalDefineConstants_; }

        void SetSupportedPlatforms(const StringVector& platforms);
        bool GetSupportsPlatform(const String& platform) const;

        ProjectSettings* GetProjectSettings() { return projectSettings_; }

    private:

        bool LoadProject(const JSONValue& root);

        /// Returns true if a project is included on the specifed platform
        bool IncludeProjectOnPlatform(const JSONValue& projectRoot, const String& platform);

        // if true, the solution (sln) file will be recreated if it exists
        bool rewriteSolution_;

        String atomicProjectPath_;
        SharedPtr<NETSolution> solution_;
        Vector<String> globalDefineConstants_;
        Vector<SharedPtr<NETCSProject>> projects_;

        SharedPtr<ProjectSettings> projectSettings_;

    };

}
