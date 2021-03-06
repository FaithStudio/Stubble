#ifndef STUBBLE_MESH_POINT_HPP
#define STUBBLE_MESH_POINT_HPP

#include "Common\CommonTypes.hpp"
#include "Common\CommonConstants.hpp"
#include "Common\CommonFunctions.hpp"
#include "Primitives\Matrix.hpp"
#include "Primitives\Vector3D.hpp"

#include <ostream>
#include <istream>
#include <sstream>
#include <string>

namespace Stubble
{

namespace HairShape
{

///----------------------------------------------------------------------------------------------------
/// Stores all information needed about single point on mesh.
/// Position and vectors are stored in world coordinate system. 
/// This class can be imported from/exported to binary file stream.
/// It has methods for transform matrix creation ( from/to local coordinate system defined by point on 
/// mesh ) and direct from local-to-world position transformation method.
///----------------------------------------------------------------------------------------------------
class MeshPoint
{
	friend inline std::ostream & operator<<( std::ostream & aStreamOut, const MeshPoint & aPointOnMesh );
	friend inline std::istream & operator>>( std::istream & aStreamIn, MeshPoint & aPointOnMesh );

public:

	///----------------------------------------------------------------------------------------------------
	/// Default empty constructor. 
	///----------------------------------------------------------------------------------------------------
	inline MeshPoint(); 

	///----------------------------------------------------------------------------------------------------
	/// Creates MeshPoint class. 
	///
	/// \param	aPosition		the point position. 
	/// \param	aNormal			the normal in selected position. 
	/// \param	aTangent		the tangent in selected position. 
	/// \param	aUCoordinate	the texture u coordinate. 
	/// \param	aVCoordinate	the texture v coordinate. 
	///----------------------------------------------------------------------------------------------------
	inline MeshPoint( const Vector3D< Real > &aPosition, const Vector3D< Real > &aNormal, 
		const Vector3D< Real > &aTangent, Real aUCoordinate, Real aVCoordinate );

	///----------------------------------------------------------------------------------------------------
	/// Creates MeshPoint class without normal and tangent specification. 
	/// Tangent and normal will have default value which is all zeros.
	///
	/// \param	aPosition		the point position. 
	/// \param	aUCoordinate	the texture u coordinate. 
	/// \param	aVCoordinate	the texture v coordinate. 
	///----------------------------------------------------------------------------------------------------
	inline MeshPoint( const Vector3D< Real > &aPosition, Real aUCoordinate, Real aVCoordinate );

	///----------------------------------------------------------------------------------------------------
	/// Gets the position of point. 
	///
	/// \return	The position of point. 
	///----------------------------------------------------------------------------------------------------
	inline const Vector3D< Real > & getPosition() const;

	///----------------------------------------------------------------------------------------------------
	/// Gets the normal. 
	///
	/// \return	The normal. 
	///----------------------------------------------------------------------------------------------------
	inline const Vector3D< Real > & getNormal() const;
	
	///----------------------------------------------------------------------------------------------------
	/// Gets the tangent. 
	///
	/// \return	The tangent. 
	///----------------------------------------------------------------------------------------------------
	inline const Vector3D< Real > & getTangent() const;

	///----------------------------------------------------------------------------------------------------
	/// Gets the binormal. 
	///
	/// \return	The binormal. 
	///----------------------------------------------------------------------------------------------------
	inline const Vector3D< Real > & getBinormal() const;
		
	///----------------------------------------------------------------------------------------------------
	/// Gets the texture u coordinate. 
	///
	/// \return	The texture u coordinate. 
	///----------------------------------------------------------------------------------------------------
	inline Real getUCoordinate() const;
	
	///----------------------------------------------------------------------------------------------------
	/// Gets the texture v coordinate. 
	///
	/// \return	The texture v coordinate. 
	///----------------------------------------------------------------------------------------------------
	inline Real getVCoordinate() const; 

	///----------------------------------------------------------------------------------------------------
	/// Converts vector in local coordinate system to a vector in world coordinate system. 
	/// Local coordinate system is defined by this point on mesh (binormal, tangent, normal; position).
	///
	/// \param	aLocalVector	The vector in local coordinate system. 
	///
	/// \return	Vector in world coordinate system.
	///----------------------------------------------------------------------------------------------------
	template< typename tType >
	Vector3D< tType > toWorld( const Vector3D< tType > aLocalVector ) const;

	///-------------------------------------------------------------------------------------------------
	/// Gets a transform matrix from local to world coordinate system. 
	/// Local coordinate system is defined by this point on mesh (binormal, tangent, normal; position).
	///
	/// \param [in,out]	aLocalTransformMatrix	The transform matrix to local coordinates.
	///-------------------------------------------------------------------------------------------------
	template< typename tMatrixType >
	inline void getLocalTransformMatrix( Matrix< tMatrixType > & aLocalTransformMatrix ) const;

	///----------------------------------------------------------------------------------------------------
	/// Gets a transform matrix from world to local coordinate system. 
	/// Local coordinate system is defined by this point on mesh (binormal, tangent, normal; position).
	///
	/// \param [out]	aWorldTransformMatrix	The transform matrix to world coordinates.
	///----------------------------------------------------------------------------------------------------
	template< typename tMatrixType >
	inline void getWorldTransformMatrix( Matrix< tMatrixType > & aWorldTransformMatrix ) const;

	///----------------------------------------------------------------------------------------------------
	/// Import only 3D position from stream. 
	/// Other properties are left as they were.
	///
	/// \param	aInStream	Input file stream
	///----------------------------------------------------------------------------------------------------
	inline void importPosition( std::istream & aStreamIn );

private:

	Vector3D< Real > mPosition; ///< The point position 

	Vector3D< Real > mNormal; ///< The normal in selected position

	Vector3D< Real > mTangent; ///< The tangent in selected position

	Vector3D< Real > mBinormal; ///< The binormal in selected position
	
	Real mUCoordinate; ///< The texture u coordinate

	Real mVCoordinate; ///< The texture v coordinate
};

///----------------------------------------------------------------------------------------------------
/// Put point on mesh into stream 
///
/// \param [in,out]	aStreamOut	The stream out. 
/// \param aPointOnMesh			The point on mesh. 
///
/// \return	The updated stream. 
///----------------------------------------------------------------------------------------------------
inline std::ostream & operator<<( std::ostream & aStreamOut, const MeshPoint & aPointOnMesh );

///----------------------------------------------------------------------------------------------------
/// Pull point on mesh from stream 
///
/// \param [in,out]	aStreamIn		The stream in. 
/// \param [in,out]	aPointOnMesh	The point on mesh. 
///
/// \return	The updated stream. 
///----------------------------------------------------------------------------------------------------
inline std::istream & operator>>( std::istream & aStreamIn, MeshPoint & aPointOnMesh );

// inline functions implementation
inline MeshPoint::MeshPoint()
{
}

inline MeshPoint::MeshPoint( const Vector3D< Real > &aPosition, const Vector3D< Real > &aNormal, 
	const Vector3D< Real > &aTangent, Real aUCoordinate, Real aVCoordinate ):
	mPosition( aPosition ),
	mNormal( aNormal ),
	mTangent( aTangent ),
	mUCoordinate( aUCoordinate ),
	mVCoordinate( aVCoordinate ),
	// Calculates binormal as aTangent x aNormal ( already normalized )
	mBinormal( Vector3D< Real >::crossProduct( aTangent, aNormal ) )
{
}

inline MeshPoint::MeshPoint( const Vector3D< Real > &aPosition, Real aUCoordinate, Real aVCoordinate ):
mPosition( aPosition ),
mUCoordinate( aUCoordinate ),
mVCoordinate( aVCoordinate )
{
}

template< typename tType >
inline Vector3D< tType > MeshPoint::toWorld( const Vector3D< tType > aLocalVector ) const
{
	return Vector3D< tType >( 
		static_cast< tType >(
		aLocalVector.x * mBinormal.x +
		aLocalVector.y * mTangent.x +
		aLocalVector.z * mNormal.x +
		mPosition.x ),
		static_cast< tType >(
		aLocalVector.x * mBinormal.y +
		aLocalVector.y * mTangent.y +
		aLocalVector.z * mNormal.y +
		mPosition.y ),
		static_cast< tType >(
		aLocalVector.x * mBinormal.z +
		aLocalVector.y * mTangent.z +
		aLocalVector.z * mNormal.z +
		mPosition.z ) );
}

template<>
inline Vector3D< Real > MeshPoint::toWorld( const Vector3D< Real > aLocalVector ) const
{
	return Vector3D< Real >( 
		aLocalVector.x * mBinormal.x +
		aLocalVector.y * mTangent.x +
		aLocalVector.z * mNormal.x +
		mPosition.x,
		aLocalVector.x * mBinormal.y +
		aLocalVector.y * mTangent.y +
		aLocalVector.z * mNormal.y +
		mPosition.y,
		aLocalVector.x * mBinormal.z +
		aLocalVector.y * mTangent.z +
		aLocalVector.z * mNormal.z +
		mPosition.z );
}

inline const Vector3D< Real > & MeshPoint::getPosition() const 
{
	return mPosition;
}

inline const Vector3D< Real > & MeshPoint::getNormal() const 
{
	return mNormal;
}

inline const Vector3D< Real > & MeshPoint::getTangent() const 
{
	return mTangent;
}

inline const Vector3D< Real > & MeshPoint::getBinormal() const 
{
	return mBinormal;
}
	
inline Real MeshPoint::getUCoordinate() const 
{
	return mUCoordinate;
}

inline Real MeshPoint::getVCoordinate() const 
{
	return mVCoordinate;
}

template< typename tMatrixType >
inline void MeshPoint::getLocalTransformMatrix( Matrix< tMatrixType > & aLocalTransformMatrix ) const
{
	aLocalTransformMatrix[ 0 ] = static_cast< tMatrixType >( mBinormal.x );
	aLocalTransformMatrix[ 1 ] = static_cast< tMatrixType >( mBinormal.y );
	aLocalTransformMatrix[ 2 ] = static_cast< tMatrixType >( mBinormal.z );
	aLocalTransformMatrix[ 3 ] = static_cast< tMatrixType >( - Vector3D< Real >::dotProduct( mPosition, mBinormal ) );
	aLocalTransformMatrix[ 4 ] = static_cast< tMatrixType >( mTangent.x );
	aLocalTransformMatrix[ 5 ] = static_cast< tMatrixType >( mTangent.y );
	aLocalTransformMatrix[ 6 ] = static_cast< tMatrixType >( mTangent.z );
	aLocalTransformMatrix[ 7 ] = static_cast< tMatrixType >( - Vector3D< Real >::dotProduct( mPosition, mTangent ) );
	aLocalTransformMatrix[ 8 ] = static_cast< tMatrixType >( mNormal.x );
	aLocalTransformMatrix[ 9 ] = static_cast< tMatrixType >( mNormal.y );
	aLocalTransformMatrix[ 10 ] = static_cast< tMatrixType >( mNormal.z );
	aLocalTransformMatrix[ 11 ] = static_cast< tMatrixType >( - Vector3D< Real >::dotProduct( mPosition, mNormal ) );
	aLocalTransformMatrix[ 12 ] = 0;
	aLocalTransformMatrix[ 13 ] = 0;
	aLocalTransformMatrix[ 14 ] = 0;
	aLocalTransformMatrix[ 15 ] = 1;
}

template< typename tMatrixType >
inline void MeshPoint::getWorldTransformMatrix( Matrix< tMatrixType > & aWorldTransformMatrix ) const
{
	aWorldTransformMatrix[ 0 ] = static_cast< tMatrixType >( mBinormal.x );
	aWorldTransformMatrix[ 4 ] = static_cast< tMatrixType >( mBinormal.y );
	aWorldTransformMatrix[ 8 ] = static_cast< tMatrixType >( mBinormal.z );
	aWorldTransformMatrix[ 12 ] = 0;
	aWorldTransformMatrix[ 1 ] = static_cast< tMatrixType >( mTangent.x );
	aWorldTransformMatrix[ 5 ] = static_cast< tMatrixType >( mTangent.y );
	aWorldTransformMatrix[ 9 ] = static_cast< tMatrixType >( mTangent.z );
	aWorldTransformMatrix[ 13 ] = 0;
	aWorldTransformMatrix[ 2 ] = static_cast< tMatrixType >( mNormal.x );
	aWorldTransformMatrix[ 6 ] = static_cast< tMatrixType >( mNormal.y );
	aWorldTransformMatrix[ 10 ] = static_cast< tMatrixType >( mNormal.z );
	aWorldTransformMatrix[ 14 ] = 0;
	aWorldTransformMatrix[ 3 ] = static_cast< tMatrixType >( mPosition.x );
	aWorldTransformMatrix[ 7 ] = static_cast< tMatrixType >( mPosition.y );
	aWorldTransformMatrix[ 11 ] = static_cast< tMatrixType >( mPosition.z );
	aWorldTransformMatrix[ 15 ] = 1;
}

inline void MeshPoint::importPosition( std::istream & aStreamIn )
{
	aStreamIn >> mPosition;
}

inline std::ostream & operator<<( std::ostream &aStreamOut, const MeshPoint &aPointOnMesh )
{
	aStreamOut << aPointOnMesh.mPosition << aPointOnMesh.mNormal << aPointOnMesh.mTangent;
	// Binormal is stored, it will be recalculated
	aStreamOut.write( reinterpret_cast< const char * >( &aPointOnMesh.mUCoordinate ), sizeof( Real ) );
	aStreamOut.write( reinterpret_cast< const char * >( &aPointOnMesh.mVCoordinate ), sizeof( Real ) );
	return aStreamOut;
}

inline std::istream & operator>>( std::istream & aStreamIn, MeshPoint & aPointOnMesh )
{
	aStreamIn >> aPointOnMesh.mPosition >> aPointOnMesh.mNormal >> aPointOnMesh.mTangent; 
	// Recalculates binormal
	aPointOnMesh.mBinormal = Vector3D< Real >::crossProduct( aPointOnMesh.mTangent, aPointOnMesh.mNormal );
	aStreamIn.read( reinterpret_cast< char * >( &aPointOnMesh.mUCoordinate ), sizeof( Real ) );
	aStreamIn.read( reinterpret_cast< char * >( &aPointOnMesh.mVCoordinate ), sizeof( Real ) );
	return aStreamIn;
}

} // namespace HairShape

} // namespace Stubble

#endif // STUBBLE_MESH_POINT_HPP