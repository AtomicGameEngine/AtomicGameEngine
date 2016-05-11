#!/usr/bin/env bash
set -e
cd "$(dirname $0)/.."
echo "Linting files..."
node_modules/.bin/tslint --config tslint.json index.ts bin/dts-generator.ts tests/intern.ts tests/unit/all.ts
echo "Building modules..."
node_modules/.bin/tsc --module umd --target es5 --moduleResolution node node_modules/intern/typings/intern/intern.d.ts typings/tsd.d.ts tests/typings/dts-generator/dts-generator.d.ts tests/intern.ts tests/unit/all.ts
node_modules/.bin/tsc --module commonjs --target es5 --sourcemap --moduleResolution node typings/tsd.d.ts index.ts bin/dts-generator.ts
echo "Running intern..."
node_modules/.bin/intern-client config=tests/intern reporters=Console
echo "Cleanup..."
rm -rf tmp
