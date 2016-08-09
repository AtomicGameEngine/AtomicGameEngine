PROJECT(nvmath)

SET(MATH_SRCS
	nvmath.h
	Vector.h
	Matrix.h
	Quaternion.h
	ConvexHull.h ConvexHull.cpp
	Plane.h Plane.cpp
	Box.h
	Basis.h Basis.cpp
	TypeSerialization.h TypeSerialization.cpp
	Random.h Random.cpp
	Sparse.h Sparse.cpp
	Solver.h Solver.cpp
	KahanSum.h
	Fitting.h Fitting.cpp)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# targets
ADD_DEFINITIONS(-DNVMATH_EXPORTS)

IF(NVMATH_SHARED)
    ADD_DEFINITIONS(-DNVMATH_SHARED=1)
    ADD_LIBRARY(nvmath SHARED ${MATH_SRCS})
ELSE(NVMATH_SHARED)
    ADD_LIBRARY(nvmath ${MATH_SRCS})
ENDIF(NVMATH_SHARED)

TARGET_LINK_LIBRARIES(nvmath ${LIBS} nvcore)

INSTALL(TARGETS nvmath
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static)
