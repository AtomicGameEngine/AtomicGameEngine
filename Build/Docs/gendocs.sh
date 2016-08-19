# Shortcut script that generates docs the same way that the CI script does.  Works on OSX.
mkdir ../../Artifacts/Build/JSDoc
mkdir ../../Artifacts/Build/EditorData
mkdir ../../Artifacts/Build/EditorData/Docs

cp Readme.md ../../Artifacts/Build/JSDoc
cp -R atomic-theme ../../Artifacts/Build/JSDoc/
cd ../../Artifacts/Build/JSDoc
# Create a package.json in this directory so the modules don't get put anywhere else
echo {} > package.json
npm install typedoc
./node_modules/.bin/typedoc --out out ../../../Script/TypeScript/dist/Atomic.d.ts --module commonjs --includeDeclarations --mode file --theme atomic-theme --name 'Atomic Game Engine' --readme ./Readme.md

cp -R out/ ../EditorData/Docs
