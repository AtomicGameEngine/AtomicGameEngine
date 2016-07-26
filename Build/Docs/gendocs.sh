# Shortcut script that generates docs the same way that the CI script does.  Works on OSX.
cp Readme.md ../../Artifacts/Build/JSDoc
cp -R atomic-theme ../../Artifacts/Build/JSDoc/
cd ../../Artifacts/Build/JSDoc
npm install typedoc@0.3.12
./node_modules/.bin/typedoc --out out ../../../Script/TypeScript/dist/Atomic.d.ts --module commonjs --includeDeclarations --mode file --theme atomic-theme --name 'Atomic Game Engine' --readme ./Readme.md

cp -R out/ ../EditorData/Docs
