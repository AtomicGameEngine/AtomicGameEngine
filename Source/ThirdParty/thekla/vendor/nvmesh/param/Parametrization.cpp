/*============================================================================
	Title: Parametrization.cpp
	Module: Pi/MeshLib
	Author: Ignacio Castaño
	Date: 22/07/2003.
============================================================================*/

/*----------------------------------------------------------------------------
	Doc:
----------------------------------------------------------------------------*/

/** @file Parametrization.cpp
 * @brief Mesh parametrization.
 *
 * Computes a parametrization of a TriMesh object.
 * 
**/


/*----------------------------------------------------------------------------
	Includes
----------------------------------------------------------------------------*/

// Core
#include "App.h"
#include "Timer.h"
#include "Radix.h"

// MathLib
#include "Sparse.h"
#include "Color.h"
#include "Real.h"

// Parametrization
#include "Parametrization.h"
#include "Adjacency.h"
#include "EdgeInfo.h"
#include "Topology.h"
#include "Atlas.h"



/*----------------------------------------------------------------------------
	Implementation
----------------------------------------------------------------------------*/

PI_IMPLEMENT_CLASS(PiMeshParametrizationOp, PiMeshOperator);



/*----------------------------------------------------------------------------
	Methods
----------------------------------------------------------------------------*/

/** Compute the desired parametrization. */
bool PiMeshParametrizationOp::Compute( PiTriMesh * m, void * data ) {
	piCheck( m != NULL );
	mesh = m;

	options = * (Options *) data;

	// Get atlas.
	atlas = (PiMeshAtlas *)mesh->GetDecorator( "PiMeshAtlas" );
	if( !atlas->IsDone() ) {
		return false;
	}

	if( !atlas->HasEdges() ) {
		atlas->ComputeEdges();
	}

	if( !atlas->HasVertices() ) {
		atlas->ComputeVertices();
	}

	// Get adjacency.
	adj = (PiMeshAdjacency *)mesh->GetDecorator( "PiMeshAdjacency" );

	// Get edge info.
	edg = (PiMeshEdgeInfo *)mesh->GetDecorator( "PiMeshEdgeInfo" );


	switch( options.method ) {
		case NULL_MAP:
			return ComputeNullMap();
		case TUTTE_MAP:
			return ComputeTutteMap();
		case CONFORMAL_MAP:
			return ComputeConformalMap();
		case INTRINSIC_MAP:
			return ComputeIntrinsicMap();
		case NATURAL_MAP:
			return ComputeNaturalMap();
		case MEAN_VALUE_MAP:
			return ComputeMeanValueMap();
		case CHORD_LENGTH_MAP:
			return ComputeChordLengthMap();
		case FLOATER_MAP:
			return ComputeFloaterMap();
		case LEAST_SQUARES_MAP:
			return ComputeLeastSquaresConformalMap();
		case SPHERICAL_MAP:
			return ComputeSphericalConformalMap();
		default:
			return false;
	}
}


bool PiMeshParametrizationOp::ComputeNullMap( void ) {

	piDebug( "--- Computing Null map.\n" );

	uint v, vertex_num = mesh->GetVertexNum();
	for( v = 0; v < vertex_num; v++ ) {
		mesh->GetVertex( v ).tex.Set( Vec3::Origin );
	}

	return true;
}


/**
 * Compute a Tutte map. The mush must have disk topology, otherwise returns false.
**/
bool PiMeshParametrizationOp::ComputeTutteMap( void ) {

	piDebug( "--- Computing Tutte map:\n" );

	// @todo use a decent profiler.
	GTimer.Refresh();
	int seconds = GTimer.GetTime();

	uint c, chart_num = atlas->GetChartNum();
	for( c = 0; c < chart_num; c++ ) {

		const PiMeshAtlas::Chart & chart = atlas->GetChart( c );

		uint vertex_num = chart.vertex_array.Size();
		uint edge_num = chart.edge_array.Size();


		SparseMatrix D( vertex_num );
		DenseVector a( vertex_num );
		DenseVector b( vertex_num );
		DenseVector x( vertex_num );
		DenseVector y( vertex_num );


		// For each interior edge, fill matrix.
		uint i;
		for( i = 0; i < edge_num; i++ ) {
			const PiMeshAtlas::ChartEdge & chart_edge = chart.edge_array[i];

			PiTriMesh::Vertex & vertex0 = mesh->GetVertex( chart_edge.v[0] );
			PiTriMesh::Vertex & vertex1 = mesh->GetVertex( chart_edge.v[1] );

			// Get vertex rows.
			uint r0 = atlas->MapMeshVertexToChartVertex( chart_edge.v[0] );
			uint r1 = atlas->MapMeshVertexToChartVertex( chart_edge.v[1] );

			// Insert value.
			if( !vertex1.IsSeam() ) {
				D.SetElem( r0, r1, 1 );
			}
			if( !vertex0.IsSeam() ) {
				D.SetElem( r1, r0, 1 );
			}
		}


		// Fill the diagonal
		for( i = 0; i < vertex_num; i++ ) {
			uint v = chart.vertex_array[i];
			if( !mesh->GetVertex( v ).IsSeam() ) {
				REAL Dpp = -D.AddRow( i );
				D.SetElem( i, i, Dpp );
			}
			else {
				D.SetElem( i, i, 1.0f );
			}
		}

		SetBoundary( options.tutte.boundary, chart, a, b );

		// Initial guess
		x.Set( a );
		y.Set( b );


		// Solve the system
		int iter;
		if( options.tutte.solver == CG_JACOBI ) {
			JacobiSymmetricPreconditioner M(D);
			iter = ConjugateGradientPrecondSolve( D, a, x, M, 1e-5 );
			piDebug( "---   CG solved in %d iterations.\n", iter );
			iter = ConjugateGradientPrecondSolve( D, b, y, M, 1e-5 );
			piDebug( "---   CG solved in %d iterations.\n", iter );
		}
		else {
			iter = GMRESSolve( D, a, x, 1e-5 );
			piDebug( "---   GMRES solved in %d iterations.\n", iter );
			iter = GMRESSolve( D, b, y, 1e-5 );
			piDebug( "---   GMRES solved in %d iterations.\n", iter );
		}


		// Generate texcoords
		for( i = 0; i < vertex_num; i++ ) {
			uint v = chart.vertex_array[i];
			uint cv = chart.owner->MapMeshVertexToChartVertex( v );
			mesh->GetVertex( v ).tex.Set( 0.5f * x[cv] + 0.5f, 0.5f * y[cv] + 0.5f, 0 );
		}

	}

	// @todo use a decent profiler.
	GTimer.Refresh();
	seconds = (GTimer.GetTime() - seconds)/1000;

	piDebug( "---   Parameterization solved in %d seconds.\n", seconds );

	// Set vertex format.
	mesh->SetVertexFormat( mesh->GetVertexFormat() & PiTriMesh::HAS_TEXCOORDS );

	return true;
}


/**
 * Compute a conformal or harmonic map. 
 * The mush must have disk topology, otherwise returns false.
**/
bool PiMeshParametrizationOp::ComputeConformalMap( void ) {

	piDebug( "--- Computing conformal map:\n" );

	// @todo use a decent profiler.
	GTimer.Refresh();
	int seconds = GTimer.GetTime();

	uint c, chart_num = atlas->GetChartNum();
	for( c = 0; c < chart_num; c++ ) {

		const PiMeshAtlas::Chart & chart = atlas->GetChart( c );

		uint vertex_num = chart.vertex_array.Size();
		uint edge_num = chart.edge_array.Size();


		SparseMatrix D( vertex_num );
		DenseVector a( vertex_num );
		DenseVector b( vertex_num );
		DenseVector x( vertex_num );
		DenseVector y( vertex_num );


		// For each interior edge, fill matrix.
		uint i;
		for( i = 0; i < edge_num; i++ ) {
			const PiMeshAtlas::ChartEdge & chart_edge = chart.edge_array[i];

			PiTriMesh::Vertex & vertex0 = mesh->GetVertex( chart_edge.v[0] );
			PiTriMesh::Vertex & vertex1 = mesh->GetVertex( chart_edge.v[1] );

			// Get vertex rows.
			uint r0 = atlas->MapMeshVertexToChartVertex( chart_edge.v[0] );
			uint r1 = atlas->MapMeshVertexToChartVertex( chart_edge.v[1] );

			const PiMeshEdgeInfo::EdgeInfo & edgeinfo = edg->GetEdgeInfo( chart_edge.id );
			REAL Dpq = edgeinfo.dirichlet_energy;

			// Insert value.
			if( !vertex1.IsSeam() ) {
				D.SetElem( r0, r1, Dpq );
			}
			if( !vertex0.IsSeam() ) {
				D.SetElem( r1, r0, Dpq );
			}
		}


		// Fill the diagonal
		for( i = 0; i < vertex_num; i++ ) {
			uint v = chart.vertex_array[i];
			if( !mesh->GetVertex( v ).IsSeam() ) {
				REAL Dpp = -D.AddRow( i );
				D.SetElem( i, i, Dpp );
			}
			else {
				D.SetElem( i, i, 1.0f );
			}
		}

		SetBoundary( options.conformal.boundary, chart, a, b );

		// Initial guess
		x.Set( a );
		y.Set( b );


		// Solve the system
		int iter;
		if( options.conformal.solver == CG_JACOBI ) {
			JacobiSymmetricPreconditioner M(D);
			iter = ConjugateGradientPrecondSolve( D, a, x, M, 1e-5 );
			piDebug( "---   CG solved in %d iterations.\n", iter );
			iter = ConjugateGradientPrecondSolve( D, b, y, M, 1e-5 );
			piDebug( "---   CG solved in %d iterations.\n", iter );
		}
		else {
			iter = GMRESSolve( D, a, x, 1e-5 );
			piDebug( "---   GMRES solved in %d iterations.\n", iter );
			iter = GMRESSolve( D, b, y, 1e-5 );
			piDebug( "---   GMRES solved in %d iterations.\n", iter );
		}


		// Generate texcoords
		for( i = 0; i < vertex_num; i++ ) {
			uint v = chart.vertex_array[i];
			uint cv = chart.owner->MapMeshVertexToChartVertex( v );
			mesh->GetVertex( v ).tex.Set( 0.5f * x[cv] + 0.5f, 0.5f * y[cv] + 0.5f, 0 );
		}

	}

	// @todo use a decent profiler.
	GTimer.Refresh();
	seconds = (GTimer.GetTime() - seconds)/1000;

	piDebug( "---   Parameterization solved in %d seconds.\n", seconds );

	// Set vertex format.
	mesh->SetVertexFormat( mesh->GetVertexFormat() & PiTriMesh::HAS_TEXCOORDS );

	return true;
}


/**
 * Compute a discrete intrinsic (conformal-authalic) parameterization of
 * this mesh. The mesh must have disk topology, otherwise returns false.
**/
bool PiMeshParametrizationOp::ComputeIntrinsicMap( void ) {

	piDebug( "--- Computing intrinsic map:\n" );

	// @todo use a decent profiler.
	GTimer.Refresh();
	int seconds = GTimer.GetTime();

	uint c, chart_num = atlas->GetChartNum();
	for( c = 0; c < chart_num; c++ ) {

		const PiMeshAtlas::Chart & chart = atlas->GetChart( c );

		uint vertex_num = chart.vertex_array.Size();
		uint edge_num = chart.edge_array.Size();


		SparseMatrix D( vertex_num );
		DenseVector a( vertex_num );
		DenseVector b( vertex_num );
		DenseVector x( vertex_num );
		DenseVector y( vertex_num );


		// For each interior edge, fill matrix.
		uint i;
		for( i = 0; i < edge_num; i++ ) {
			const PiMeshAtlas::ChartEdge & chart_edge = chart.edge_array[i];
			const PiMeshEdgeInfo::EdgeInfo & edgeinfo = edg->GetEdgeInfo( chart_edge.id );

			PiTriMesh::Vertex & vertex0 = mesh->GetVertex( chart_edge.v[0] );
			PiTriMesh::Vertex & vertex1 = mesh->GetVertex( chart_edge.v[1] );

			// Get vertex rows.
			uint r0 = atlas->MapMeshVertexToChartVertex( chart_edge.v[0] );
			uint r1 = atlas->MapMeshVertexToChartVertex( chart_edge.v[1] );

			REAL chi0, chi1;
			GetWachpressCoordinates( chart_edge.id, chi0, chi1 );

			// Insert value.
			if( !vertex1.IsSeam() ) {
				REAL Dpq = edgeinfo.dirichlet_energy * options.intrinsic.dirichlet + chi0 * options.intrinsic.chi;
				D.SetElem( r0, r1, Dpq );
			}
			if( !vertex0.IsSeam() ) {
				REAL Dpq = edgeinfo.dirichlet_energy * options.intrinsic.dirichlet + chi1 * options.intrinsic.chi;
				D.SetElem( r1, r0, Dpq );
			}
		}


		// Fill the diagonal
		for( i = 0; i < vertex_num; i++ ) {
			uint v = chart.vertex_array[i];
			if( !mesh->GetVertex( v ).IsSeam() ) {
				REAL Dpp = -D.AddRow( i );
				D.SetElem( i, i, Dpp );
			}
			else {
				D.SetElem( i, i, 1.0f );
			}
		}

		SetBoundary( options.intrinsic.boundary, chart, a, b );

		// Initial guess
		x.Set( a );
		y.Set( b );


		// Solve the system
		int iter;
		if( options.intrinsic.solver == CG_JACOBI ) {
			JacobiPreconditioner M(D);
			iter = BiCGSTABPrecondSolve( D, a, x, M, 1e-5 );
			piDebug( "---   CG solved in %d iterations.\n", iter );
			iter = BiCGSTABPrecondSolve( D, b, y, M, 1e-5 );
			piDebug( "---   CG solved in %d iterations.\n", iter );
		}
		else {
			iter = GMRESSolve( D, a, x, 1e-5 );
			piDebug( "---   GMRES solved in %d iterations.\n", iter );
			iter = GMRESSolve( D, b, y, 1e-5 );
			piDebug( "---   GMRES solved in %d iterations.\n", iter );
		}


		// Generate texcoords
		for( i = 0; i < vertex_num; i++ ) {
			uint v = chart.vertex_array[i];
			uint cv = chart.owner->MapMeshVertexToChartVertex( v );
			mesh->GetVertex( v ).tex.Set( 0.5f * x[cv] + 0.5f, 0.5f * y[cv] + 0.5f, 0 );
		}

	}

	// @todo use a decent profiler.
	GTimer.Refresh();
	seconds = (GTimer.GetTime() - seconds)/1000;

	piDebug( "---   Parameterization solved in %d seconds.\n", seconds );

	// Set vertex format.
	mesh->SetVertexFormat( mesh->GetVertexFormat() & PiTriMesh::HAS_TEXCOORDS );

	return true;
}




bool PiMeshParametrizationOp::ComputeMeanValueMap( void ) {

	piDebug( "--- Computing mean value map:\n" );

	// @todo use a decent profiler.
	GTimer.Refresh();
	int seconds = GTimer.GetTime();

	uint c, chart_num = atlas->GetChartNum();
	for( c = 0; c < chart_num; c++ ) {

		const PiMeshAtlas::Chart & chart = atlas->GetChart( c );

		uint vertex_num = chart.vertex_array.Size();
		uint edge_num = chart.edge_array.Size();


		SparseMatrix D( vertex_num );
		DenseVector a( vertex_num );
		DenseVector b( vertex_num );
		DenseVector x( vertex_num );
		DenseVector y( vertex_num );


		// For each interior edge, fill matrix.
		uint i;
		for( i = 0; i < edge_num; i++ ) {
			const PiMeshAtlas::ChartEdge & chart_edge = chart.edge_array[i];

			PiTriMesh::Vertex & vertex0 = mesh->GetVertex( chart_edge.v[0] );
			PiTriMesh::Vertex & vertex1 = mesh->GetVertex( chart_edge.v[1] );

			// Get vertex rows.
			uint r0 = atlas->MapMeshVertexToChartVertex( chart_edge.v[0] );
			uint r1 = atlas->MapMeshVertexToChartVertex( chart_edge.v[1] );

			REAL mv0, mv1;
			GetMeanValueCoordinates( chart_edge.id, mv0, mv1 );


			// Insert value.
			if( !vertex1.IsSeam() ) {
				D.SetElem( r0, r1, mv0 );
			}
			if( !vertex0.IsSeam() ) {
				D.SetElem( r1, r0, mv1 );
			}
		}


		// Fill the diagonal
		for( i = 0; i < vertex_num; i++ ) {
			uint v = chart.vertex_array[i];
			if( !mesh->GetVertex( v ).IsSeam() ) {
				REAL Dpp = -D.AddRow( i );
				D.SetElem( i, i, Dpp );
			}
			else {
				D.SetElem( i, i, 1.0f );
			}
		}

		SetBoundary( options.meanvalue.boundary, chart, a, b );

		// Initial guess
		x.Set( a );
		y.Set( b );


		// Solve the system
		int iter;
		if( options.meanvalue.solver == CG_JACOBI ) {
			JacobiPreconditioner M(D);
			iter = BiCGSTABPrecondSolve( D, a, x, M, 1e-5 );
			piDebug( "---   CG solved in %d iterations.\n", iter );
			iter = BiCGSTABPrecondSolve( D, b, y, M, 1e-5 );
			piDebug( "---   CG solved in %d iterations.\n", iter );
		}
		else {
			iter = GMRESSolve( D, a, x, 1e-5 );
			piDebug( "---   GMRES solved in %d iterations.\n", iter );
			iter = GMRESSolve( D, b, y, 1e-5 );
			piDebug( "---   GMRES solved in %d iterations.\n", iter );
		}


		// Generate texcoords
		for( i = 0; i < vertex_num; i++ ) {
			uint v = chart.vertex_array[i];
			uint cv = chart.owner->MapMeshVertexToChartVertex( v );
			mesh->GetVertex( v ).tex.Set( 0.5f * x[cv] + 0.5f, 0.5f * y[cv] + 0.5f, 0 );
		}

	}

	// @todo use a decent profiler.
	GTimer.Refresh();
	seconds = (GTimer.GetTime() - seconds)/1000;

	piDebug( "---   Parameterization solved in %d seconds.\n", seconds );

	// Set vertex format.
	mesh->SetVertexFormat( mesh->GetVertexFormat() & PiTriMesh::HAS_TEXCOORDS );

	return true;
}



/** 
 * A chord length map of a surface onto the given boundary is the result
 * of minimizing an energy functional that depends on the length of the
 * edges of the surface. The value @a q controls the exponent of the 
 * length factor.
**/
bool PiMeshParametrizationOp::ComputeChordLengthMap( void ) {

	piDebug( "--- Computing chord length map: (q = %.2f)\n", options.chordlength.q );

	// @todo use a decent profiler.
	GTimer.Refresh();
	int seconds = GTimer.GetTime();

	uint c, chart_num = atlas->GetChartNum();
	for( c = 0; c < chart_num; c++ ) {

		const PiMeshAtlas::Chart & chart = atlas->GetChart( c );

		uint vertex_num = chart.vertex_array.Size();
		uint edge_num = chart.edge_array.Size();


		SparseMatrix D( vertex_num );
		DenseVector a( vertex_num );
		DenseVector b( vertex_num );
		DenseVector x( vertex_num );
		DenseVector y( vertex_num );


		// For each interior edge, fill matrix.
		uint i;
		for( i = 0; i < edge_num; i++ ) {
			const PiMeshAtlas::ChartEdge & chart_edge = chart.edge_array[i];
			const PiMeshEdgeInfo::EdgeInfo & edgeinfo = edg->GetEdgeInfo( chart_edge.id );

			PiTriMesh::Vertex & vertex0 = mesh->GetVertex( chart_edge.v[0] );
			PiTriMesh::Vertex & vertex1 = mesh->GetVertex( chart_edge.v[1] );

			// Get vertex rows.
			uint r0 = atlas->MapMeshVertexToChartVertex( chart_edge.v[0] );
			uint r1 = atlas->MapMeshVertexToChartVertex( chart_edge.v[1] );

			REAL Dpq = 1.0f / pow( edgeinfo.length, options.chordlength.q );

			// Insert value.
			if( !vertex0.IsSeam() ) {
				D.SetElem( r1, r0, Dpq );
			}
			if( !vertex1.IsSeam() ) {
				D.SetElem( r0, r1, Dpq );
			}
		}


		// Fill the diagonal
		for( i = 0; i < vertex_num; i++ ) {
			uint v = chart.vertex_array[i];
			if( !mesh->GetVertex( v ).IsSeam() ) {
				REAL Dpp = -D.AddRow( i );
				D.SetElem( i, i, Dpp );
			}
			else {
				D.SetElem( i, i, 1.0f );
			}
		}

		SetBoundary( options.chordlength.boundary, chart, a, b );

		// Initial guess
		x.Set( a );
		y.Set( b );


		// Solve the system
		int iter;
		if( options.chordlength.solver == CG_JACOBI ) {
			JacobiSymmetricPreconditioner M(D);
			iter = ConjugateGradientPrecondSolve( D, a, x, M );
			piDebug( "---   CG solved in %d iterations.\n", iter );
			iter = ConjugateGradientPrecondSolve( D, b, y, M );
			piDebug( "---   CG solved in %d iterations.\n", iter );
		}
		else {
			iter = GMRESSolve( D, a, x, 1e-5 );
			piDebug( "---   GMRES solved in %d iterations.\n", iter );
			iter = GMRESSolve( D, b, y, 1e-5 );
			piDebug( "---   GMRES solved in %d iterations.\n", iter );
		}

		// Generate texcoords
		for( i = 0; i < vertex_num; i++ ) {
			uint v = chart.vertex_array[i];
			uint cv = chart.owner->MapMeshVertexToChartVertex( v );
			mesh->GetVertex( v ).tex.Set( 0.5f * x[cv] + 0.5f, 0.5f * y[cv] + 0.5f, 0 );
		}
	}

	// @todo use a decent profiler.
	GTimer.Refresh();
	seconds = (GTimer.GetTime() - seconds)/1000;

	piDebug( "---   Parameterization solved in %d seconds.\n", seconds );

	// Set vertex format.
	mesh->SetVertexFormat( mesh->GetVertexFormat() & PiTriMesh::HAS_TEXCOORDS );

	return true;
}


bool PiMeshParametrizationOp::ComputeFloaterMap( void ) {

	return false;
}


bool PiMeshParametrizationOp::ComputeLeastSquaresConformalMap( void ) {

	return false;
}


/** */
bool PiMeshParametrizationOp::ComputeSphericalConformalMap( void ) {

	return false;
}


/** Set chart boundary in the equation system. */
void PiMeshParametrizationOp::SetBoundary( boundary_t boundary, const PiMeshAtlas::Chart & chart, DenseVector &a, DenseVector &b ) {

	REAL boundary_length = 0;
	uint edge_num = chart.edge_array.Size();
	uint boundary_num = chart.boundary_array.Size();

	// Compute boundary length.
	uint i;
	for( i = 0; i < boundary_num; i++ ) {
		const PiMeshAtlas::ChartEdge & edge = chart.boundary_array[i];
		const PiMeshEdgeInfo::EdgeInfo & edgeinfo = edg->GetEdgeInfo( edge.id );
		boundary_length += edgeinfo.length;
	}

	// Clear the boundary vectors.
	a.Zero();
	b.Zero();


	if( boundary == BOUNDARY_CIRCLE ) {

		// place the boundaries on a circle by chord length.
		float angle = 0.0f;

		for( i = 0; i < boundary_num; i++ ) {

			float s = sin(angle);
			float t = cos(angle);

			const PiMeshAtlas::ChartEdge & edge = chart.boundary_array[i];

			uint v = edge.v[1];
			uint r = chart.owner->MapMeshVertexToChartVertex( v );

			a[r] = s;
			b[r] = t;

			angle += edg->GetEdgeInfo( edge.id ).length * 2.0f * PI / boundary_length;
		}
	}
	else if( boundary == BOUNDARY_QUAD ) {

		// compute quad edge lengths
		/*	
		REAL length = 0.0f;
		uint e = edge_seed;
		do {
			length += edg->GetEdgeInfo(e).length;
			e = adj->GetEdge(e).GetNextBoundary();
		} while( length < boundary_length * 0.25f );
		const REAL length0 = length;

		do {
			length += edg->GetEdgeInfo(e).length;
			e = adj->GetEdge(e).GetNextBoundary();
		} while( length < boundary_length * 0.5f );
		const REAL length1 = length - length0;

		do {
			length += edg->GetEdgeInfo(e).length;
			e = adj->GetEdge(e).GetNextBoundary();
		} while( length < boundary_length * 0.75f );
		const REAL length2 = length - length1 - length0;

		do {
			length += edg->GetEdgeInfo(e).length;
			e = adj->GetEdge(e).GetNextBoundary();
		} while( e != edge_seed );
		const REAL length3 = length - length2 - length1 - length0;
		*/



		// map vertices to the edges of the quad by chord length.
		/*length = 0.0f;
		e = edge_seed;
		do {
			a[adj->GetEdge(e).v0] = -1;
			b[adj->GetEdge(e).v0] = 2 * length / length0 - 1;

			length += edg->GetEdgeInfo(e).length;
			e = adj->GetEdge(e).GetNextBoundary();
		} while( length < boundary_length * 0.25f );

		do {
			a[adj->GetEdge(e).v0] = 2 * (length-length0) / length1 - 1;
			b[adj->GetEdge(e).v0] = 1;

			length += edg->GetEdgeInfo(e).length;
			e = adj->GetEdge(e).GetNextBoundary();
		} while( length < boundary_length * 0.5f );

		do {
			a[adj->GetEdge(e).v0] = 1;
			b[adj->GetEdge(e).v0] = 1 - 2 * (length-length1-length0) / length2;

			length += edg->GetEdgeInfo(e).length;
			e = adj->GetEdge(e).GetNextBoundary();
		} while( length < boundary_length * 0.75f );

		do {
			a[adj->GetEdge(e).v0] = 1 - 2 * (length-length2-length1-length0) / length3;
			b[adj->GetEdge(e).v0] = -1;

			length += edg->GetEdgeInfo(e).length;
			e = adj->GetEdge(e).GetNextBoundary();
		} while( e != edge_seed );*/

	}
	else if( boundary == BOUNDARY_PRESERVE ) {
		// Copy previous texture coordinates.
		for( i = 0; i < boundary_num; i++ ) {

			uint v = chart.boundary_array[i].v[0];
			uint r = chart.owner->MapMeshVertexToChartVertex( v );

			a[r] = 2.0f * mesh->GetVertex( v ).tex.x - 1.0f;
			b[r] = 2.0f * mesh->GetVertex( v ).tex.y - 1.0f;
		}
	}
	else if( boundary == BOUNDARY_MODEL_XY ) {
		
		// Copy xy model coordinates.
		for( i = 0; i < boundary_num; i++ ) {

			uint v = chart.boundary_array[i].v[0];
			uint r = chart.owner->MapMeshVertexToChartVertex( v );

			a[r] = 2.0f * mesh->GetVertex( v ).pos.x - 1.0f;
			b[r] = 2.0f * mesh->GetVertex( v ).pos.y - 1.0f;
		}
	}

}


/** Set the boundary for the given edge loop. 
void PiMeshParametrizationOp::SetBoundary( PiMeshParametrizationOp::boundary_t boundary, uint edge_seed, float boundary_length, DenseVector &a, DenseVector &b ) {

	a.Zero();
	b.Zero();

	if( boundary == BOUNDARY_CIRCLE ) {

		// place the boundaries on a circle by chord length.
		float angle = 0.0f;

		uint e = edge_seed;
		do {
			float s = sin(angle);
			float t = cos(angle);

			a[adj->GetEdge(e).v0] = s;
			b[adj->GetEdge(e).v0] = t;

			angle += edg->GetEdgeInfo(e).length * 2.0f * PI / boundary_length;
			//angle += 2.0f * PI / float(boundary_num);

			e = adj->GetEdge(e).GetNextBoundary();
		} while( e != edge_seed );

	}
	else if( boundary == BOUNDARY_QUAD ) {

		// compute quad edge lengths
		REAL length = 0.0f;
		uint e = edge_seed;
		do {
			length += edg->GetEdgeInfo(e).length;
			e = adj->GetEdge(e).GetNextBoundary();
		} while( length < boundary_length * 0.25f );
		const REAL length0 = length;

		do {
			length += edg->GetEdgeInfo(e).length;
			e = adj->GetEdge(e).GetNextBoundary();
		} while( length < boundary_length * 0.5f );
		const REAL length1 = length - length0;

		do {
			length += edg->GetEdgeInfo(e).length;
			e = adj->GetEdge(e).GetNextBoundary();
		} while( length < boundary_length * 0.75f );
		const REAL length2 = length - length1 - length0;

		do {
			length += edg->GetEdgeInfo(e).length;
			e = adj->GetEdge(e).GetNextBoundary();
		} while( e != edge_seed );
		const REAL length3 = length - length2 - length1 - length0;



		// map vertices to the edges of the quad by chord length.
		length = 0.0f;
		e = edge_seed;
		do {
			a[adj->GetEdge(e).v0] = -1;
			b[adj->GetEdge(e).v0] = 2 * length / length0 - 1;

			length += edg->GetEdgeInfo(e).length;
			e = adj->GetEdge(e).GetNextBoundary();
		} while( length < boundary_length * 0.25f );

		do {
			a[adj->GetEdge(e).v0] = 2 * (length-length0) / length1 - 1;
			b[adj->GetEdge(e).v0] = 1;

			length += edg->GetEdgeInfo(e).length;
			e = adj->GetEdge(e).GetNextBoundary();
		} while( length < boundary_length * 0.5f );

		do {
			a[adj->GetEdge(e).v0] = 1;
			b[adj->GetEdge(e).v0] = 1 - 2 * (length-length1-length0) / length2;

			length += edg->GetEdgeInfo(e).length;
			e = adj->GetEdge(e).GetNextBoundary();
		} while( length < boundary_length * 0.75f );

		do {
			a[adj->GetEdge(e).v0] = 1 - 2 * (length-length2-length1-length0) / length3;
			b[adj->GetEdge(e).v0] = -1;

			length += edg->GetEdgeInfo(e).length;
			e = adj->GetEdge(e).GetNextBoundary();
		} while( e != edge_seed );

	}
	else if( boundary == BOUNDARY_PRESERVE ) {

		uint e = edge_seed;
		do {
			const uint v = adj->GetEdge(e).v0;
			a[v] = 2.0f * mesh->GetVertex(v).tex.x - 1.0f;
			b[v] = 2.0f * mesh->GetVertex(v).tex.y - 1.0f;

			e = adj->GetEdge(e).GetNextBoundary();
		} while( e != edge_seed );

	}
}*/


/** Compute the chi energy of the given edge. */
void PiMeshParametrizationOp::GetWachpressCoordinates( uint e, REAL &chi0, REAL &chi1 ) {
	const PiMeshAdjacency::Edge & edge = adj->GetEdge( e );

	piCheck( !edge.IsBoundary() );
	piDebugCheck( edge.v0 != NULL_INDEX );
	piDebugCheck( edge.v1 != NULL_INDEX );

	const Vec3 & P = mesh->GetVertex( edge.v0 ).pos;
	const Vec3 & Q = mesh->GetVertex( edge.v1 ).pos;

	uint r = NULL_INDEX, s = NULL_INDEX;	// opposite indices.

	if( edge.f0 != NULL_INDEX ) {
		r = adj->GetOppositeFaceVertex( edge.f0, e );
	}

	if( edge.f1 != NULL_INDEX ) {
		s = adj->GetOppositeFaceVertex( edge.f1, e );
	}

	piCheck( r != NULL_INDEX );
	piCheck( s != NULL_INDEX );

	const Vec3 & R = mesh->GetVertex( r ).pos;
	const Vec3 & S = mesh->GetVertex( s ).pos;

	Vec3 QR, QS, QP, PR, PS, PQ;

	QR.Sub( R, Q );
	QS.Sub( S, Q );
	QP.Sub( P, Q );

	PR.Sub( R, P );
	PS.Sub( S, P );
	PQ.Sub( Q, P );

	REAL area_PRQ = Vec3Area( PQ, PR );
	REAL area_PSQ = Vec3Area( PQ, PS );

	REAL area_QRS = Vec3Area( QS, QR );
	REAL area_PSR = Vec3Area( PR, PS );

	chi0 = area_PSR / (area_PSQ * area_PRQ);
	chi1 = area_QRS / (area_PSQ * area_PRQ);
}


/** Compute the mean value coordinates for the given edge. */
void PiMeshParametrizationOp::GetMeanValueCoordinates( uint e, REAL &mv0, REAL &mv1 ) {
	const PiMeshAdjacency::Edge & edge = adj->GetEdge( e );

	piCheck( !edge.IsBoundary() );
	piDebugCheck( edge.v0 != NULL_INDEX );
	piDebugCheck( edge.v1 != NULL_INDEX );

	const Vec3 & P = mesh->GetVertex( edge.v0 ).pos;
	const Vec3 & Q = mesh->GetVertex( edge.v1 ).pos;

	uint r = NULL_INDEX, s = NULL_INDEX;	// opposite indices.

	if( edge.f0 != NULL_INDEX ) {
		r = adj->GetOppositeFaceVertex( edge.f0, e );
	}

	if( edge.f1 != NULL_INDEX ) {
		s = adj->GetOppositeFaceVertex( edge.f1, e );
	}

	piCheck( r != NULL_INDEX );
	piCheck( s != NULL_INDEX );

	const Vec3 & R = mesh->GetVertex( r ).pos;
	const Vec3 & S = mesh->GetVertex( s ).pos;

	Vec3 QR, QS, QP, PR, PS, PQ;

	QR.Sub( R, Q );
	QS.Sub( S, Q );
	QP.Sub( P, Q );

	PR.Sub( R, P );
	PS.Sub( S, P );
	PQ.Sub( Q, P );

	// tan(alpha/2) = (1 - cos(alpha)) / sin(alpha)

	REAL tanS0 = (PQ.Length() * PS.Length() - Vec3DotProduct( PQ, PS )) / Vec3Area( PQ, PS );
	REAL tanR0 = (PQ.Length() * PR.Length() - Vec3DotProduct( PQ, PR )) / Vec3Area( PQ, PR );

	REAL tanS1 = (QP.Length() * QS.Length() - Vec3DotProduct( QP, QS )) / Vec3Area( QP, QS );
	REAL tanR1 = (QP.Length() * QR.Length() - Vec3DotProduct( QP, QR )) / Vec3Area( QP, QR );

	mv1 = (tanS0 + tanR0) / PQ.Length();
	mv0 = (tanS1 + tanR1) / PQ.Length();

}
