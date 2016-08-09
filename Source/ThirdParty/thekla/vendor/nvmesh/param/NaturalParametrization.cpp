/*============================================================================
	Title: NaturalParametrization.cpp
	Module: Pi/MeshLib
	Author: Ignacio Castaño
	Date: 22/07/2003.
============================================================================*/

/*----------------------------------------------------------------------------
	Doc:
----------------------------------------------------------------------------*/

/** @file NaturalParametrization.cpp
 * @brief Natural parametrization.
 *
 * Computes a natural parametrization of a TriMesh object.
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



/*----------------------------------------------------------------------------
	Definitions
----------------------------------------------------------------------------*/

/** @internal */
#define X(a)	(a)

/** @internal */
#define Y(a)	((a)+vertex_num)



/*----------------------------------------------------------------------------
	Functors
----------------------------------------------------------------------------*/

/** */
struct FillInteriorVertex : public IUnaryConstProcedure<PiMeshAdjacency::Edge> {
	FillInteriorVertex( PiTriMesh * m, const PiMeshEdgeInfo * e, SparseMatrix & d, uint ch, uint v, uint n ) : mesh(m), edg(e), D(d) {
		atlas = (PiMeshAtlas *) mesh->GetDecorator( "PiMeshAtlas" );
		c = ch;
		v0 = v;
		vertex_num = n;
		sum = 0;
	}

	void operator()( const PiMeshAdjacency::Edge & edge ) {

		REAL dirichlet = edg->GetEdgeInfo( edge.id ).dirichlet_energy;

		uint v1 = edge.GetOppositeVertex( v0 );

		const PiTriMesh::Vertex & vertex0 = mesh->GetVertex( v0 );
		const PiTriMesh::Vertex & vertex1 = mesh->GetVertex( mesh->GetVertexOnChart( v1, c ) );

		// Get vertex rows.
		uint r0 = atlas->MapMeshVertexToChartVertex( vertex0.id );
		uint r1 = atlas->MapMeshVertexToChartVertex( vertex1.id );

		D.SetElem( X(r0), X(r1), -dirichlet );
		D.SetElem( Y(r0), Y(r1), -dirichlet );

		sum += dirichlet;
	}

	PiTriMesh * mesh;
	PiMeshAtlas * atlas;
	const PiMeshEdgeInfo * edg;
	SparseMatrix & D;
	uint c;
	uint v0, vertex_num;
	REAL sum;
};


/** */
struct FillNeumannVertex : public IUnaryConstProcedure<PiMeshAdjacency::Edge> {
	FillNeumannVertex( PiTriMesh * m, const PiMeshEdgeInfo * e, SparseMatrix & d, uint ch, uint v, uint n ) : mesh(m), edg(e), D(d) {
		atlas = (PiMeshAtlas *) mesh->GetDecorator( "PiMeshAtlas" );
		c = ch;
		v0 = v;
		vertex_num = n;
		sum = 0;
	}

	void operator()( const PiMeshAdjacency::Edge & edge ) {

		REAL dirichlet = edg->GetEdgeInfo( edge.id ).dirichlet_energy;

		uint v1 = edge.GetOppositeVertex( v0 );
		v1 = mesh->GetVertexOnChart( v1, c );

		if( v1 == NULL_INDEX ) {
			// Skip edges that are on a different chart.
			return;
		}

		const PiTriMesh::Vertex & vertex0 = mesh->GetVertex( mesh->GetVertexOnChart( v0, c ) );
		const PiTriMesh::Vertex & vertex1 = mesh->GetVertex( v1 );

		// Get vertex rows.
		uint r0 = atlas->MapMeshVertexToChartVertex( vertex0.id );
		uint r1 = atlas->MapMeshVertexToChartVertex( vertex1.id );

		D.SetElem( X(r0), X(r1), -dirichlet );
		D.SetElem( Y(r0), Y(r1), -dirichlet );

		sum += dirichlet;

		if( edge.IsSeam() ) {
			if( c == mesh->GetFace( edge.f0 ).chart ) {
				if( v0 == edge.v0 ) {
					D.SetElem( X(r0), Y(r1), -2 );
					D.SetElem( Y(r0), X(r1), 2 );
				}
				else {
					D.SetElem( X(r0), Y(r1), 2 );
					D.SetElem( Y(r0), X(r1), -2 );
				}
			}
			else {
				if( v0 != edge.v0 ) {
					D.SetElem( X(r0), Y(r1), -2 );
					D.SetElem( Y(r0), X(r1), 2 );
				}
				else {
					D.SetElem( X(r0), Y(r1), 2 );
					D.SetElem( Y(r0), X(r1), -2 );
				}
			}
		}
	}

	PiTriMesh * mesh;
	PiMeshAtlas * atlas;
	const PiMeshEdgeInfo * edg;
	SparseMatrix & D;
	uint c;
	uint v0, vertex_num;
	REAL sum;
};


/** */
struct FillDirichletVertex : public PiFunctor {
	FillDirichletVertex( SparseMatrix & d, uint v, uint n ) : D(d) { 
		this->v = v;
		vertex_num = n;
	}

	void operator()( void ) {
		D.SetIdentityRow( X(v) );
		D.SetIdentityRow( Y(v) );
	}

	SparseMatrix & D;
	uint v, vertex_num;
};




/*----------------------------------------------------------------------------
	Methods
----------------------------------------------------------------------------*/

/*
bool PiMeshParametrizationOp::ComputeNaturalMap( void ) {
	const PiMeshAdjacency * adj = (PiMeshAdjacency *)mesh->GetDecorator( "PiMeshAdjacency" );
	const PiMeshEdgeInfo * edg = (PiMeshEdgeInfo *)mesh->GetDecorator( "PiMeshEdgeInfo" );
	const PiMeshAtlas * atlas = (PiMeshAtlas *)mesh->GetDecorator( "PiMeshAtlas" );


	piDebug( "--- Computing natural map:\n" );

	GTimer.Refresh();
	int seconds = GTimer.GetTime();

	uint c, chart_num = atlas->GetChartNum();
	for( c = 0; c < chart_num; c++ ) {

		const PiMeshAtlas::Chart & chart = atlas->GetChart( c );

		uint vertex_num = chart.vertex_array.Size();
		uint edge_num = chart.edge_array.Size();
		uint boundary_num = chart.boundary_array.Size();

		SparseMatrix D( 2*vertex_num );
		DenseVector a( 2*vertex_num );
		DenseVector x( 2*vertex_num );


		// Fill matrix.
		uint i;
		for( i = 0; i < vertex_num; i++ ) {
			uint v = chart.vertex_array[i];

			const PiTriMesh::Vertex & vertex = mesh->GetVertex( v );

			if( vertex.IsSeam() ) {
				FillNeumannVertex filler( mesh, edg, D, c, v, vertex_num );
				adj->ForEachEdgeAround( v, filler );

				// Get vertex rows.
				uint r = atlas->MapMeshVertexToChartVertex( v );

				// Fill the diagonal
				D.SetElem( X(r), X(r), filler.sum );
				D.SetElem( Y(r), Y(r), filler.sum );
			}
			else {
				FillInteriorVertex filler( mesh, edg, D, c, v, vertex_num );
				adj->ForEachEdgeAround( v, filler );

				// Get vertex rows.
				uint r = atlas->MapMeshVertexToChartVertex( v );

				// Fill the diagonal
				D.SetElem( X(r), X(r), filler.sum );
				D.SetElem( Y(r), Y(r), filler.sum );
			}
		}


		// Compute boundary length.
		REAL boundary_length = 0;
		for( i = 0; i < boundary_num; i++ ) {
			const PiMeshAtlas::ChartEdge & edge = chart.boundary_array[i];
			const PiMeshEdgeInfo::EdgeInfo & edgeinfo = edg->GetEdgeInfo( edge.id );
			boundary_length += edgeinfo.length;
		}



		// Set number of fixed vertices.
		options.natural.fixed = clamp( options.natural.fixed, 2, boundary_num );
		if( boundary_num / options.natural.free_max > options.natural.fixed ) {
			options.natural.fixed = boundary_num / options.natural.free_max;
		}

		piDebug( "---   Fixing %d points.\n", options.natural.fixed );


		// Set fixed vertices.
		PiArray<uint> fixed_array;
		fixed_array.Resize( options.natural.fixed );

		uint fixed_num = 1;
		fixed_array[0] = chart.boundary_array[0].v[0];

		REAL l = 0.0f;
		i = 0;
		while( fixed_num < options.natural.fixed ) {
			REAL stop = REAL(fixed_num) * boundary_length / REAL(options.natural.fixed);

			// @fixme This works in most, but not all cases.
			uint v;
			do {
				v = chart.boundary_array[i].v[0];
				l += edg->GetEdgeInfo( chart.boundary_array[i].id ).length;
				i++;
			} while( l < stop && i < boundary_num );

			fixed_array[fixed_num++] = v;
		}


		a.Zero();
		x.Zero();

		// Setup initial guess.
		float angle = 0.0f;
		for( i = 0; i < boundary_num; i++ ) {

			float s = sin(angle);
			float t = cos(angle);

			const PiMeshAtlas::ChartEdge & edge = chart.boundary_array[i];

			uint v = edge.v[0];
			uint r = atlas->MapMeshVertexToChartVertex( v );

			x[X(r)] = s;
			x[Y(r)] = t;

			angle += edg->GetEdgeInfo( edge.id ).length * 2.0f * PI / boundary_length;
		}


		// Set Dirichlet boundaries.
		for( i = 0; i < fixed_array.Size(); i++ ) {
			uint v = fixed_array[i];
			uint r = atlas->MapMeshVertexToChartVertex( v );

			a[X(r)] = x[X(r)];
			a[Y(r)] = x[Y(r)];

			D.SetIdentityRow( X(r) );
			D.SetIdentityRow( Y(r) );
		}

		int iter;
		JacobiPreconditioner M(D);
		iter = BiCGSTABPrecondSolve( D, a, x, M, 0.000001 );
	//	iter = GMRESSolve( D, a, x, 1e-5 );
		piDebug( "---   CG solved in %d iterations.\n", iter );


		// Generate texcoords
		for( i = 0; i < vertex_num; i++ ) {
			uint v = chart.vertex_array[i];
			uint cv = chart.owner->MapMeshVertexToChartVertex( v );
			mesh->GetVertex( v ).tex.Set( 0.5f * x[X(cv)] + 0.5f, 0.5f * x[Y(cv)] + 0.5f, 0 );
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

/*/


bool PiMeshParametrizationOp::ComputeNaturalMap( void ) {

	piDebug( "--- Computing conformal map with natural boundaries:\n" );

	// @todo use a decent profiler.
	GTimer.Refresh();
	int seconds = GTimer.GetTime();

	uint c, chart_num = atlas->GetChartNum();
	for( c = 0; c < chart_num; c++ ) {

		const PiMeshAtlas::Chart & chart = atlas->GetChart( c );

		uint vertex_num = chart.vertex_array.Size();
		uint edge_num = chart.edge_array.Size();
		uint boundary_num = chart.boundary_array.Size();


		SparseMatrix D( 2*vertex_num );
		DenseVector a( 2*vertex_num );
		DenseVector x( 2*vertex_num );


		// For each edge, fill matrix.
		uint i;
		for( i = 0; i < edge_num; i++ ) {
			const PiMeshAtlas::ChartEdge & edge = chart.edge_array[i];
			const PiMeshEdgeInfo::EdgeInfo & edgeinfo = edg->GetEdgeInfo( edge.id );

			PiTriMesh::Vertex & vertex0 = mesh->GetVertex( edge.v[0] );
			PiTriMesh::Vertex & vertex1 = mesh->GetVertex( edge.v[1] );

			// Get vertex rows.
			uint r0 = atlas->MapMeshVertexToChartVertex( edge.v[0] );
			uint r1 = atlas->MapMeshVertexToChartVertex( edge.v[1] );

			REAL Dpq = - edgeinfo.dirichlet_energy;

			D.SetElem( X(r0), X(r1), Dpq );
			D.SetElem( Y(r0), Y(r1), Dpq );
			D.SetElem( X(r1), X(r0), Dpq );
			D.SetElem( Y(r1), Y(r0), Dpq );
		}
		for( i = 0; i < boundary_num; i++ ) {
			const PiMeshAtlas::ChartEdge & edge = chart.boundary_array[i];
			const PiMeshEdgeInfo::EdgeInfo & edgeinfo = edg->GetEdgeInfo( edge.id );

			// Get vertex rows.
			uint r0 = atlas->MapMeshVertexToChartVertex( edge.v[0] );
			uint r1 = atlas->MapMeshVertexToChartVertex( edge.v[1] );

			REAL Dpq = - edgeinfo.dirichlet_energy;

			D.SetElem( X(r0), X(r1), Dpq );
			D.SetElem( Y(r0), Y(r1), Dpq );
			D.SetElem( X(r1), X(r0), Dpq );
			D.SetElem( Y(r1), Y(r0), Dpq );
		}

		// Set elastic boundaries.
		/*for( i = 0; i < boundary_num; i++ ) {
			const PiMeshAtlas::ChartEdge & edge = chart.boundary_array[i];
			const PiMeshEdgeInfo::EdgeInfo & edgeinfo = edg->GetEdgeInfo( edge.id );

			uint v0 = edge.v[0];
			uint v1 = edge.v[1];
			uint r0 = chart.owner->MapMeshVertexToChartVertex( v0 );
			uint r1 = chart.owner->MapMeshVertexToChartVertex( v1 );

			REAL Dpq = 1.0f / SQ(edgeinfo.length);

			D.SetElem( X(r0), X(r1), Dpq );
			D.SetElem( Y(r0), Y(r1), Dpq );
			D.SetElem( X(r1), X(r0), Dpq );
			D.SetElem( Y(r1), Y(r0), Dpq );
		}*/


		// Fill the diagonal
		for( i = 0; i < vertex_num; i++ ) {
			REAL Dpp = - D.AddRow( i );
			D.SetElem( X(i), X(i), Dpp );
			D.SetElem( Y(i), Y(i), Dpp );
		}


		// Set Neumann boundaries.
		for( i = 0; i < boundary_num; i++ ) {
			const PiMeshAtlas::ChartEdge & edge = chart.boundary_array[i];

			uint v0 = edge.v[0];
			uint v1 = edge.v[1];
			uint r0 = chart.owner->MapMeshVertexToChartVertex( v0 );
			uint r1 = chart.owner->MapMeshVertexToChartVertex( v1 );

			D.SetElem( X(r0), Y(r1), -2 );
			D.SetElem( Y(r0), X(r1), 2 );
			D.SetElem( X(r1), Y(r0), 2 );
			D.SetElem( Y(r1), X(r0), -2 );
		}


		// Compute boundary length.
		REAL boundary_length = 0;
		for( i = 0; i < boundary_num; i++ ) {
			const PiMeshAtlas::ChartEdge & edge = chart.boundary_array[i];
			const PiMeshEdgeInfo::EdgeInfo & edgeinfo = edg->GetEdgeInfo( edge.id );
			boundary_length += edgeinfo.length;
		}



		// Set number of fixed vertices.
		options.natural.fixed = piClamp( options.natural.fixed, uint(2), boundary_num );
		if( boundary_num / options.natural.free_max > options.natural.fixed ) {
			options.natural.fixed = boundary_num / options.natural.free_max;
		}

		piDebug( "---   Fixing %d points.\n", options.natural.fixed );


		// Set fixed vertices.
		PiArray<uint> fixed_array;
		fixed_array.Resize( options.natural.fixed );

		uint fixed_num = 1;
		fixed_array[0] = chart.boundary_array[0].v[0];

		REAL l = 0.0f;
		i = 0;
		while( fixed_num < options.natural.fixed ) {
			REAL stop = REAL(fixed_num) * boundary_length / REAL(options.natural.fixed);

			// @fixme This works in most, but not all cases.
			uint v;
			do {
				const PiMeshAtlas::ChartEdge & edge = chart.boundary_array[i];
				v = edge.v[0];
				l += edg->GetEdgeInfo( edge.id ).length;
				i++;
			} while( l < stop && i < boundary_num );

			fixed_array[fixed_num++] = v;
		}

		/*fixed_array.Resize( 3 );
		uint fixed_num = 0;
		for( fixed_num = 0, i = 0; i < 4; i++ ) {
			uint v = chart.boundary_array[i].v[0];
			fixed_array[fixed_num++] = v;
		}*/


		a.Zero();
		x.Zero();

		// Setup initial guess.
		float angle = 0.0f;
		for( i = 0; i < boundary_num; i++ ) {

			float s = sin(angle);
			float t = cos(angle);

			const PiMeshAtlas::ChartEdge & edge = chart.boundary_array[i];

			uint v = edge.v[0];
			uint r = atlas->MapMeshVertexToChartVertex( v );

			x[X(r)] = s;
			x[Y(r)] = t;

			angle += edg->GetEdgeInfo( edge.id ).length * 2.0f * PI / boundary_length;
		}


		// Set Dirichlet boundaries.
		for( i = 0; i < fixed_array.Size(); i++ ) {
			uint v = fixed_array[i];
			uint r = atlas->MapMeshVertexToChartVertex( v );

			a[X(r)] = x[X(r)];
			a[Y(r)] = x[Y(r)];

			D.SetIdentityRow( X(r) );
		//	D.SetIdentityCol( X(r) );
			D.SetIdentityRow( Y(r) );
		//	D.SetIdentityCol( Y(r) );
		}



		int iter;
		JacobiPreconditioner M(D);
		iter = BiCGSTABPrecondSolve( D, a, x, M, 1e-5 );
	//	iter = GMRESSolve( D, a, x, 1e-5 );
		piDebug( "---   CG solved in %d iterations.\n", iter );


		// Generate texcoords
		for( i = 0; i < vertex_num; i++ ) {
			uint v = chart.vertex_array[i];
			uint cv = chart.owner->MapMeshVertexToChartVertex( v );
			mesh->GetVertex( v ).tex.Set( 0.5f * x[X(cv)] + 0.5f, 0.5f * x[Y(cv)] + 0.5f, 0 );
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

//*/