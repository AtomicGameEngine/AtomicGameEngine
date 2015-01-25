
include_directories(src)

# Define source files
file (GLOB CPP_FILES src/BulletCollision/BroadphaseCollision/*.cpp 
    src/BulletCollision/CollisionDispatch/*.cpp src/BulletCollision/CollisionShapes/*.cpp 
    src/BulletCollision/Gimpact/*.cpp src/BulletCollision/NarrowPhaseCollision/*.cpp 
    src/BulletDynamics/Character/*.cpp src/BulletDynamics/ConstraintSolver/*.cpp
    src/BulletDynamics/Dynamics/*.cpp src/BulletDynamics/Vehicle/*.cpp src/BulletSoftBody/*.cpp
    src/BulletDynamics/Featherstone/*.cpp src/BulletDynamics/MLCPSolvers/*.cpp
    src/LinearMath/*.cpp)
file (GLOB H_FILES *.h src/BulletCollision/BroadphaseCollision/*.h
    src/BulletCollision/CollisionDispatch/*.h src/BulletCollision/CollisionShapes/*.h
    src/BulletCollision/Gimpact/*.h src/BulletCollision/NarrowPhaseCollision/*.h
    src/BulletDynamics/Character/*.h src/BulletDynamics/ConstraintSolver/*.h
    src/BulletDynamics/Dynamics/*.h src/BulletDynamics/Vehicle/*.h src/BulletSoftBody/*.h 
    src/BulletDynamics/Featherstone/*.h src/BulletDynamics/MLCPSolvers/*.h
    src/LinearMath/*.h)

set (SOURCE_FILES ${CPP_FILES} ${H_FILES})

add_library(Bullet ${SOURCE_FILES})


