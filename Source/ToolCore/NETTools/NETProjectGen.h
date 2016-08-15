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

    class NETProjectGen;

    class NETProjectBase : public Object
    {
        ATOMIC_OBJECT(NETProjectBase, Object)

    public:

        NETProjectBase(Context* context, NETProjectGen* projectGen);
        virtual ~NETProjectBase();

        void ReplacePathStrings(String& path);

        void CopyXMLElementRecursive(XMLElement source, XMLElement dest);

    protected:

        SharedPtr<XMLFile> xmlFile_;
        WeakPtr<NETProjectGen> projectGen_;

    };

    class NETCSProject : public NETProjectBase
    {
        ATOMIC_OBJECT(NETCSProject, NETProjectBase)

    public:

        NETCSProject(Context* context, NETProjectGen* projectGen);
        virtual ~NETCSProject();

        bool Load(const JSONValue& root);

        const String& GetName() { return name_; }
        const String& GetProjectGUID() { return projectGuid_; }

        const Vector<String>& GetReferences() const { return references_; }
        const Vector<String>& GetPackages() const { return packages_; }

        bool Generate();

    private:

        bool CreateProjectFolder(const String& path);

        void CreateCompileItemGroup(XMLElement &projectRoot);
        void CreateReferencesItemGroup(XMLElement &projectRoot);
        void CreateProjectReferencesItemGroup(XMLElement & projectRoot);
        void CreatePackagesItemGroup(XMLElement &projectRoot);
        void CreateMainPropertyGroup(XMLElement &projectRoot);
        void CreateDebugPropertyGroup(XMLElement &projectRoot);
        void CreateReleasePropertyGroup(XMLElement &projectRoot);
        void CreateAssemblyInfo();
        void GetAssemblySearchPaths(String& paths);

        String name_;
        String projectGuid_;
        String outputType_;
        String rootNamespace_;
        String assemblyName_;
        String assemblyOutputPath_;
        String assemblySearchPaths_;

        // project paths
        String projectPath_;

        XMLElement xmlRoot_;

        Vector<String> references_;
        Vector<String> packages_;
        Vector<String> sourceFolders_;
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

        const String& GetScriptPlatform() { return scriptPlatform_; }

        NETSolution* GetSolution() { return solution_; }

        const Vector<SharedPtr<NETCSProject>>& GetCSProjects() { return projects_; }

        Project* GetAtomicProject() const { return atomicProject_;  }

        NETCSProject* GetCSProjectByName(const String& name);

        bool GetCSProjectDependencies(NETCSProject * source, PODVector<NETCSProject*>& depends) const;

        void SetScriptPlatform(const String& platform) { scriptPlatform_ = platform; }

        bool Generate();

        /// Returns true if the generated solution requires NuGet
        bool GetRequiresNuGet();

        String GenerateUUID();

        /// If true, the sln file will rewritten if it exists, default is false
        void SetRewriteSolution(bool rewrite);

        bool LoadProject(const JSONValue& root);
        bool LoadProject(const String& projectPath);
        bool LoadProject(Project* project);

    private:

        // if true, the solution (sln) file will be recreated if it exists
        bool rewriteSolution_;

        String scriptPlatform_;

        SharedPtr<Project> atomicProject_;
        SharedPtr<NETSolution> solution_;
        Vector<SharedPtr<NETCSProject>> projects_;

    };

}
