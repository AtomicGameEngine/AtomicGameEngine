// This code is in the public domain -- Ignacio Castaño <castano@gmail.com>

#include "Fitting.h"
#include "Vector.inl"
#include "Plane.inl"

#include "nvcore/Utils.h" // max, swap

#include <float.h> // FLT_MAX

using namespace nv;

// @@ Move to EigenSolver.h

// @@ We should be able to do something cheaper...
static Vector3 estimatePrincipleComponent(const float * __restrict matrix)
{
	const Vector3 row0(matrix[0], matrix[1], matrix[2]);
	const Vector3 row1(matrix[1], matrix[3], matrix[4]);
	const Vector3 row2(matrix[2], matrix[4], matrix[5]);

	float r0 = lengthSquared(row0);
	float r1 = lengthSquared(row1);
	float r2 = lengthSquared(row2);

	if (r0 > r1 && r0 > r2) return row0;
	if (r1 > r2) return row1;
	return row2;
}


static inline Vector3 firstEigenVector_PowerMethod(const float *__restrict matrix)
{
    if (matrix[0] == 0 && matrix[3] == 0 && matrix[5] == 0)
    {
        return Vector3(0.0f);
    }

    Vector3 v = estimatePrincipleComponent(matrix);

    const int NUM = 8;
    for (int i = 0; i < NUM; i++)
    {
        float x = v.x * matrix[0] + v.y * matrix[1] + v.z * matrix[2];
        float y = v.x * matrix[1] + v.y * matrix[3] + v.z * matrix[4];
        float z = v.x * matrix[2] + v.y * matrix[4] + v.z * matrix[5];

        float norm = max(max(x, y), z);

        v = Vector3(x, y, z) / norm;
    }

    return v;
}


Vector3 nv::Fit::computeCentroid(int n, const Vector3 *__restrict points)
{
    Vector3 centroid(0.0f);

    for (int i = 0; i < n; i++)
    {
        centroid += points[i];
    }
    centroid /= float(n);

    return centroid;
}

Vector3 nv::Fit::computeCentroid(int n, const Vector3 *__restrict points, const float *__restrict weights, Vector3::Arg metric)
{
    Vector3 centroid(0.0f);
    float total = 0.0f;

    for (int i = 0; i < n; i++)
    {
        total += weights[i];
        centroid += weights[i]*points[i];
    }
    centroid /= total;

    return centroid;
}


Vector3 nv::Fit::computeCovariance(int n, const Vector3 *__restrict points, float *__restrict covariance)
{
    // compute the centroid
    Vector3 centroid = computeCentroid(n, points);

    // compute covariance matrix
    for (int i = 0; i < 6; i++)
    {
        covariance[i] = 0.0f;
    }

    for (int i = 0; i < n; i++)
    {
        Vector3 v = points[i] - centroid;

        covariance[0] += v.x * v.x;
        covariance[1] += v.x * v.y;
        covariance[2] += v.x * v.z;
        covariance[3] += v.y * v.y;
        covariance[4] += v.y * v.z;
        covariance[5] += v.z * v.z;
    }

    return centroid;
}

Vector3 nv::Fit::computeCovariance(int n, const Vector3 *__restrict points, const float *__restrict weights, Vector3::Arg metric, float *__restrict covariance)
{
    // compute the centroid
    Vector3 centroid = computeCentroid(n, points, weights, metric);

    // compute covariance matrix
    for (int i = 0; i < 6; i++)
    {
        covariance[i] = 0.0f;
    }

    for (int i = 0; i < n; i++)
    {
        Vector3 a = (points[i] - centroid) * metric;
        Vector3 b = weights[i]*a;

        covariance[0] += a.x * b.x;
        covariance[1] += a.x * b.y;
        covariance[2] += a.x * b.z;
        covariance[3] += a.y * b.y;
        covariance[4] += a.y * b.z;
        covariance[5] += a.z * b.z;
    }

    return centroid;
}

Vector3 nv::Fit::computePrincipalComponent(int n, const Vector3 *__restrict points)
{
    float matrix[6];
    computeCovariance(n, points, matrix);

    return firstEigenVector_PowerMethod(matrix);
}

Vector3 nv::Fit::computePrincipalComponent(int n, const Vector3 *__restrict points, const float *__restrict weights, Vector3::Arg metric)
{
    float matrix[6];
    computeCovariance(n, points, weights, metric, matrix);

    return firstEigenVector_PowerMethod(matrix);
}


Plane nv::Fit::bestPlane(int n, const Vector3 *__restrict points)
{
    // compute the centroid and covariance
    float matrix[6];
    Vector3 centroid = computeCovariance(n, points, matrix);

    if (matrix[0] == 0 && matrix[3] == 0 && matrix[5] == 0)
    {
        // If no plane defined, then return a horizontal plane.
        return Plane(Vector3(0, 0, 1), centroid);
    }

    float eigenValues[3];
    Vector3 eigenVectors[3];
    if (!eigenSolveSymmetric(matrix, eigenValues, eigenVectors)) {
        // If no plane defined, then return a horizontal plane.
        return Plane(Vector3(0, 0, 1), centroid);
    }

    return Plane(eigenVectors[2], centroid);
}

bool nv::Fit::isPlanar(int n, const Vector3 * points, float epsilon/*=NV_EPSILON*/)
{
    // compute the centroid and covariance
    float matrix[6];
    computeCovariance(n, points, matrix);

    float eigenValues[3];
    Vector3 eigenVectors[3];
    if (!eigenSolveSymmetric(matrix, eigenValues, eigenVectors)) {
        return false;
    }

    return eigenValues[2] < epsilon;
}



// Tridiagonal solver from Charles Bloom. 
// Householder transforms followed by QL decomposition. 
// Seems to be based on the code from Numerical Recipes in C.

static void EigenSolver_Tridiagonal(double mat[3][3],double * diag,double * subd);
static bool EigenSolver_QLAlgorithm(double mat[3][3],double * diag,double * subd);

bool nv::Fit::eigenSolveSymmetric(float matrix[6], float eigenValues[3], Vector3 eigenVectors[3])
{
    nvDebugCheck(matrix != NULL && eigenValues != NULL && eigenVectors != NULL);

    double subd[3];
    double diag[3];
    double work[3][3];

    work[0][0] = matrix[0];
    work[0][1] = work[1][0] = matrix[1];
    work[0][2] = work[2][0] = matrix[2];
    work[1][1] = matrix[3];
    work[1][2] = work[2][1] = matrix[4];
    work[2][2] = matrix[5];

    EigenSolver_Tridiagonal(work, diag, subd);
    if (!EigenSolver_QLAlgorithm(work, diag, subd))
    {
        for (int i = 0; i < 3; i++) {
            eigenValues[i] = 0;
            eigenVectors[i] = Vector3(0);
        }
        return false;
    }

    for (int i = 0; i < 3; i++) {
        eigenValues[i] = (float)diag[i];
    }

    // eigenvectors are the columns; make them the rows :

    for (int i=0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            eigenVectors[j].component[i] = (float) work[i][j];
        }
    }

    // shuffle to sort by singular value :
    if (eigenValues[2] > eigenValues[0] && eigenValues[2] > eigenValues[1])
    {
        swap(eigenValues[0], eigenValues[2]);
        swap(eigenVectors[0], eigenVectors[2]);
    }
    if (eigenValues[1] > eigenValues[0])
    {
        swap(eigenValues[0], eigenValues[1]);
        swap(eigenVectors[0], eigenVectors[1]);
    }
    if (eigenValues[2] > eigenValues[1])
    {
        swap(eigenValues[1], eigenValues[2]);
        swap(eigenVectors[1], eigenVectors[2]);
    }

    nvDebugCheck(eigenValues[0] >= eigenValues[1] && eigenValues[0] >= eigenValues[2]);
    nvDebugCheck(eigenValues[1] >= eigenValues[2]);

    return true;
}

static void EigenSolver_Tridiagonal(double mat[3][3],double * diag,double * subd)
{
    // Householder reduction T = Q^t M Q
    //   Input:   
    //     mat, symmetric 3x3 matrix M
    //   Output:  
    //     mat, orthogonal matrix Q
    //     diag, diagonal entries of T
    //     subd, subdiagonal entries of T (T is symmetric)
    const double epsilon = 1e-08f;

    double a = mat[0][0];
    double b = mat[0][1];
    double c = mat[0][2];
    double d = mat[1][1];
    double e = mat[1][2];
    double f = mat[2][2];

    diag[0] = a;
    subd[2] = 0.f;
    if ( fabs(c) >= epsilon )
    {
        const double ell = sqrt(b*b+c*c);
        b /= ell;
        c /= ell;
        const double q = 2*b*e+c*(f-d);
        diag[1] = d+c*q;
        diag[2] = f-c*q;
        subd[0] = ell;
        subd[1] = e-b*q;
        mat[0][0] = 1; mat[0][1] = 0; mat[0][2] = 0;
        mat[1][0] = 0; mat[1][1] = b; mat[1][2] = c;
        mat[2][0] = 0; mat[2][1] = c; mat[2][2] = -b;
    }
    else
    {
        diag[1] = d;
        diag[2] = f;
        subd[0] = b;
        subd[1] = e;
        mat[0][0] = 1; mat[0][1] = 0; mat[0][2] = 0;
        mat[1][0] = 0; mat[1][1] = 1; mat[1][2] = 0;
        mat[2][0] = 0; mat[2][1] = 0; mat[2][2] = 1;
    }
}

static bool EigenSolver_QLAlgorithm(double mat[3][3],double * diag,double * subd)
{
    // QL iteration with implicit shifting to reduce matrix from tridiagonal
    // to diagonal
    const int maxiter = 32;

    for (int ell = 0; ell < 3; ell++)
    {
        int iter;
        for (iter = 0; iter < maxiter; iter++)
        {
            int m;
            for (m = ell; m <= 1; m++)
            {
                double dd = fabs(diag[m]) + fabs(diag[m+1]);
                if ( fabs(subd[m]) + dd == dd )
                    break;
            }
            if ( m == ell )
                break;

            double g = (diag[ell+1]-diag[ell])/(2*subd[ell]);
            double r = sqrt(g*g+1);
            if ( g < 0 )
                g = diag[m]-diag[ell]+subd[ell]/(g-r);
            else
                g = diag[m]-diag[ell]+subd[ell]/(g+r);
            double s = 1, c = 1, p = 0;
            for (int i = m-1; i >= ell; i--)
            {
                double f = s*subd[i], b = c*subd[i];
                if ( fabs(f) >= fabs(g) )
                {
                    c = g/f;
                    r = sqrt(c*c+1);
                    subd[i+1] = f*r;
                    c *= (s = 1/r);
                }
                else
                {
                    s = f/g;
                    r = sqrt(s*s+1);
                    subd[i+1] = g*r;
                    s *= (c = 1/r);
                }
                g = diag[i+1]-p;
                r = (diag[i]-g)*s+2*b*c;
                p = s*r;
                diag[i+1] = g+p;
                g = c*r-b;

                for (int k = 0; k < 3; k++)
                {
                    f = mat[k][i+1];
                    mat[k][i+1] = s*mat[k][i]+c*f;
                    mat[k][i] = c*mat[k][i]-s*f;
                }
            }
            diag[ell] -= p;
            subd[ell] = g;
            subd[m] = 0;
        }

        if ( iter == maxiter )
            // should not get here under normal circumstances
            return false;
    }

    return true;
}




int nv::Fit::compute4Means(int n, const Vector3 *__restrict points, const float *__restrict weights, Vector3::Arg metric, Vector3 *__restrict cluster)
{
    // Compute principal component.
    float matrix[6];
    Vector3 centroid = computeCovariance(n, points, weights, metric, matrix);
    Vector3 principal = firstEigenVector_PowerMethod(matrix);

    // Pick initial solution.
    int mini, maxi;
    mini = maxi = 0;

    float mindps, maxdps;
    mindps = maxdps = dot(points[0] - centroid, principal);

    for (int i = 1; i < n; ++i)
    {
        float dps = dot(points[i] - centroid, principal);

        if (dps < mindps) {
            mindps = dps;
            mini = i;
        }
        else {
            maxdps = dps;
            maxi = i;
        }
    }

    cluster[0] = centroid + mindps * principal;
    cluster[1] = centroid + maxdps * principal;
    cluster[2] = (2.0f * cluster[0] + cluster[1]) / 3.0f;
    cluster[3] = (2.0f * cluster[1] + cluster[0]) / 3.0f;

    // Now we have to iteratively refine the clusters.
    while (true)
    {
        Vector3 newCluster[4] = { Vector3(0.0f), Vector3(0.0f), Vector3(0.0f), Vector3(0.0f) };
        float total[4] = {0, 0, 0, 0};

        for (int i = 0; i < n; ++i)
        {
            // Find nearest cluster.
            int nearest = 0;
            float mindist = FLT_MAX;
            for (int j = 0; j < 4; j++)
            {
                float dist = lengthSquared((cluster[j] - points[i]) * metric);
                if (dist < mindist)
                {
                    mindist = dist;
                    nearest = j;
                }
            }

            newCluster[nearest] += weights[i] * points[i];
            total[nearest] += weights[i];
        }

        for (int j = 0; j < 4; j++)
        {
            if (total[j] != 0)
                newCluster[j] /= total[j];
        }

        if (equal(cluster[0], newCluster[0]) && equal(cluster[1], newCluster[1]) && 
            equal(cluster[2], newCluster[2]) && equal(cluster[3], newCluster[3]))
        {
            return (total[0] != 0) + (total[1] != 0) + (total[2] != 0) + (total[3] != 0);
        }

        cluster[0] = newCluster[0];
        cluster[1] = newCluster[1];
        cluster[2] = newCluster[2];
        cluster[3] = newCluster[3];

        // Sort clusters by weight.
        for (int i = 0; i < 4; i++)
        {
            for (int j = i; j > 0 && total[j] > total[j - 1]; j--)
            {
                swap( total[j], total[j - 1] );
                swap( cluster[j], cluster[j - 1] );
            }
        }
    }
}

