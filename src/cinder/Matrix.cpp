/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#include "cinder/Matrix.h"

#include "glm/gtx/matrix_operation.hpp"
#include "glm/gtc/matrix_inverse.hpp"

namespace cinder {

//
//  firstFrame - Compute the first reference frame along a curve.
//
//  This function returns the transformation matrix to the reference frame
//  defined by the three points 'firstPoint', 'secondPoint' and 'thirdPoint'. Note that if the two
//  vectors <firstPoint,secondPoint> and <firstPoint,thirdPoint> are co-linears, an arbitrary twist value will
//  be chosen.
//
//  Throw 'NullVecExc' if 'firstPoint' and 'secondPoint' are equals.
#if 0
template<typename T>
Matrix44<T> firstFrame( 
	const Vec3<T> &firstPoint, 
	const Vec3<T> &secondPoint,
	const Vec3<T> &thirdPoint 
)
{
    Vec3<T> t = ( secondPoint - firstPoint ).normalized();

    Vec3<T> n = t.cross( thirdPoint - firstPoint ).normalized();
    if( n.length() == 0.0f ) {
        int i = fabs( t[0] ) < fabs( t[1] ) ? 0 : 1;
        if( fabs( t[2] ) < fabs( t[i] ) ) i = 2;

        Vec3<T> v( (T)0.0, (T)0.0, (T)0.0 ); v[i] = (T)1.0;
        n = t.cross( v ).normalized();
    }

    Vec3<T> b = t.cross( n );

    Matrix44<T> M;

    M.m[0] =  b[0]; M.m[1] =  b[1]; M.m[2]  =  b[2]; M.m[3]  = (T)0.0,
    M.m[4] =  n[0]; M.m[5] =  n[1]; M.m[6]  =  n[2]; M.m[7]  = (T)0.0,
    M.m[8] =  t[0]; M.m[9] =  t[1]; M.m[10] =  t[2]; M.m[11] = (T)0.0,
    M.m[12] = firstPoint[0]; M.m[13] = firstPoint[1]; M.m[14] = firstPoint[2]; M.m[15] = (T)1.0;
    //M.m[0] =  t[0]; M.m[1] =  t[1]; M.m[2] =  t[2]; M.m[3] = (T)0.0,
    //M.m[4] =  n[0]; M.m[5] =  n[1]; M.m[6] =  n[2]; M.m[7] = (T)0.0,
    //M.m[8] =  b[0]; M.m[9] =  b[1]; M.m[10] =  b[2]; M.m[11] = (T)0.0,
    //M.m[12] = firstPoint[0]; M.m[13] = firstPoint[1]; M.m[14] = firstPoint[2]; M.m[15] = (T)1.0;

    return M;

}

//  nextFrame - Compute the next reference frame along a curve.
//
//  This function returns the transformation matrix to the next reference 
//  frame defined by the previously computed transformation matrix and the
//  new point and tangent vector along the curve.
template<typename T>
Matrix44<T> nextFrame( 
	const Matrix44<T> &prevMatrix, 
	const Vec3<T> &prevPoint, 
	const Vec3<T> &curPoint,
	Vec3<T> &prevTangent, 
	Vec3<T> &curTangent 
)
{
    Vec3<T> a = vec3( 0 );	// Rotation axis.
    T r = 0;						// Rotation angle.

    if( ( prevTangent.length() != 0.0 ) && ( curTangent.length() != 0.0 ) ) {
        prevTangent.normalize();
        curTangent.normalize();
        T dot = prevTangent.dot( curTangent ); 

        if( dot > (T)1.0 ) dot = (T)1.0; 
        else if( dot < (T)-1.0 ) dot = (T)-1.0;

        r = math<T>::acos( dot );
        a = prevTangent.cross( curTangent );
    }

    if( ( a.length() != 0.0 ) && ( r != 0.0 ) ) {
        Matrix44<T> R  = Matrix44<T>::createRotation( a, r );		
        Matrix44<T> Tj = Matrix44<T>::createTranslation( curPoint );
        Matrix44<T> Ti = Matrix44<T>::createTranslation( -prevPoint );

		// Original order of operation:
        //return prevMatrix * Ti * R * Tj;
		//
		// Cinder's order of operation
        return Tj*R*Ti*prevMatrix;
    }
    else {
        Matrix44<T> Tr = Matrix44<T>::createTranslation( curPoint - prevPoint );
		// Original order of operation:
        //return prevMatrix*Tr;
		//
		// Cinder's order of operation
        return Tr*prevMatrix;
    }
}

//  lastFrame - Compute the last reference frame along a curve.
//
//  This function returns the transformation matrix to the last reference 
//  frame defined by the previously computed transformation matrix and the
//  last point along the curve.
template<typename T>
Matrix44<T> lastFrame( 
	const Matrix44<T> &prevMatrix, 
	const Vec3<T> &prevPoint, 
	const Vec3<T> &lastPoint 
)
{
	// Original order of operation:
	//return prevMatrix * Matrix44<T>::createTranslation( lastPoint - prevPoint );
	//
	// Cinder's order of operation
    return Matrix44<T>::createTranslation( lastPoint - prevPoint )*prevMatrix;
}

// Explicitly declare the functions
template mat4 firstFrame( const vec3 &firstPoint, const vec3 &secondPoint, const vec3 &thirdPoint );
template mat4 nextFrame( const mat4 &prevMatrix, const vec3 &prevPoint, const vec3 &curPoint, vec3 &prevTangent, vec3 &curTangent );
template mat4 lastFrame( const mat4 &prevMatrix, const vec3 &prevPoint, 	const vec3 &lastPoint );
template dmat4 firstFrame( const dvec3 &firstPoint, const dvec3 &secondPoint, const dvec3 &thirdPoint );
template dmat4 nextFrame( const dmat4 &prevMatrix, const dvec3 &prevPoint, const dvec3 &curPoint, dvec3 &prevTangent, dvec3 &curTangent );
template dmat4 lastFrame( const dmat4 &prevMatrix, const dvec3 &prevPoint, 	const dvec3 &lastPoint );

#endif

glm::mat4 alignZAxisWithTarget( vec3 targetDir, vec3 upDir )
{
    // Ensure that the target direction is non-zero.
    if( length2( targetDir ) == 0 )
		targetDir = vec3( 0, 0, 1 );

    // Ensure that the up direction is non-zero.
    if( length2( upDir ) == 0 )
		upDir = vec3( 0, 1, 0 );

    // Check for degeneracies.  If the upDir and targetDir are parallel 
    // or opposite, then compute a new, arbitrary up direction that is
    // not parallel or opposite to the targetDir.
    if( length2( cross( upDir, targetDir ) ) == 0 ) {
		upDir = cross( targetDir, vec3( 1, 0, 0 ) );
	if( length2( upDir ) == 0 )
	    upDir = cross( targetDir, vec3( 0, 0, 1 ) );
    }

    // Compute the x-, y-, and z-axis vectors of the new coordinate system.
	vec3 targetPerpDir = cross( upDir, targetDir );
	vec3 targetUpDir = cross( targetDir, targetPerpDir );

    // Rotate the x-axis into targetPerpDir (row 0),
    // rotate the y-axis into targetUpDir   (row 1),
    // rotate the z-axis into targetDir     (row 2).
    vec3 row[3];
    row[0] = normalize( targetPerpDir );
	row[1] = normalize( targetUpDir );
	row[2] = normalize( targetDir );

    const float v[16] = {	row[0].x,  row[0].y,  row[0].z,  0,
							row[1].x,  row[1].y,  row[1].z,  0,
							row[2].x,  row[2].y,  row[2].z,  0,
					        0,         0,         0,		 1 };
	
    
    return glm::make_mat4( v );
}

} // namespace cinder
