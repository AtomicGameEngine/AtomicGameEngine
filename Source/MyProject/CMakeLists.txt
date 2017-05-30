
file(GLOB_RECURSE SOURCE_FILES *.cpp *.h)

# in order to make our native components available in the Atomic Editor component creation menu, they need to be accessible from TypeScript, so bindings are generated.  This won't be necessary once native components are self registering
file (GLOB JAVASCRIPT_BINDINGS_SOURCE ${ATOMIC_SOURCE_DIR}/Artifacts/Build/Source/Generated/Javascript/Packages/MyProject/*.cpp)

# define our project library
add_library(MyProject ${SOURCE_FILES} ${JAVASCRIPT_BINDINGS_SOURCE})

# and link in Atomic (again, AtomicJS optional)
target_link_libraries(MyProject AtomicJS Atomic)
