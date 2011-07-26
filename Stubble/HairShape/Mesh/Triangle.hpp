#ifndef STUBBLE_TRIANGLE_HPP
#define STUBBLE_TRIANGLE_HPP

#include "HairShape\Mesh\MeshPoint.hpp"

#include <vector>

namespace Stubble
{

namespace HairShape
{

///----------------------------------------------------------------------------------------------------
/// Triangle. 
///----------------------------------------------------------------------------------------------------
class Triangle
{
public:

	///----------------------------------------------------------------------------------------------------
	/// Gets the point 1. 
	///
	/// \return	The point 1. 
	///----------------------------------------------------------------------------------------------------
	inline const MeshPoint & getPoint1() const;

	///----------------------------------------------------------------------------------------------------
	/// Gets the point 2. 
	///
	/// \return	The point 2. 
	///----------------------------------------------------------------------------------------------------
	inline const MeshPoint & getPoint2() const;

	///----------------------------------------------------------------------------------------------------
	/// Gets the point 3. 
	///
	/// \return	The point 3. 
	///----------------------------------------------------------------------------------------------------
	inline const MeshPoint & getPoint3() const;

private:

	MeshPoint mPoints[ 3 ]; ///< The points
};

///----------------------------------------------------------------------------------------------------
/// Defines an alias representing the triangles .
///----------------------------------------------------------------------------------------------------
typedef std::vector< Triangle > Triangles;

///----------------------------------------------------------------------------------------------------
/// Gets the point 1. 
///
/// \return	The point 1. 
///----------------------------------------------------------------------------------------------------
inline const MeshPoint & Triangle::getPoint1() const
{
	return mPoints[ 0 ];
}

///----------------------------------------------------------------------------------------------------
/// Gets the point 2. 
///
/// \return	The point 2. 
///----------------------------------------------------------------------------------------------------
inline const MeshPoint & Triangle::getPoint2() const
{
	return mPoints[ 1 ];
}

///----------------------------------------------------------------------------------------------------
/// Gets the point 3. 
///
/// \return	The point 3. 
///----------------------------------------------------------------------------------------------------
inline const MeshPoint & Triangle::getPoint3() const
{
	return mPoints[ 2 ];
}

} // namespace HairShape

} // namespace Stubble

#endif // STUBBLE_TRIANGLE_HPP