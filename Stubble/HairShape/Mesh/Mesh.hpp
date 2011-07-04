#ifndef STUBBLE_MESH_HPP
#define STUBBLE_MESH_HPP

#include "Triangle.hpp"
#include "TriangleConstIterator.hpp"
#include "UVPoint.hpp"

namespace Stubble
{

namespace HairShape
{

///----------------------------------------------------------------------------------------------------
/// Stores mesh in inner format.
///----------------------------------------------------------------------------------------------------
class Mesh
{
public:
	///----------------------------------------------------------------------------------------------------
	/// Gets const triangle iterator.
	///----------------------------------------------------------------------------------------------------
	TriangleConstIterator getTriangleConstIterator() const;

	///----------------------------------------------------------------------------------------------------
	/// Gets point on mesh interpolated from 3 vertices of given triangle
	///----------------------------------------------------------------------------------------------------
	MeshPoint getMeshPoint(const UVPoint &aPoint) const;

	///----------------------------------------------------------------------------------------------------
	/// Gets triangle as 3 vertices.
	///----------------------------------------------------------------------------------------------------
	// TODO

	///----------------------------------------------------------------------------------------------------
	/// Gets number of mesh's triangles.
	///----------------------------------------------------------------------------------------------------
	unsigned int getTriangleCount() const;

	///----------------------------------------------------------------------------------------------------
	/// meshUpdate TODO
	///----------------------------------------------------------------------------------------------------

	///----------------------------------------------------------------------------------------------------
	/// getMesh UG
	///----------------------------------------------------------------------------------------------------

protected:
	Mesh();

private:
	Triangles mTriangles; ///< Triangles of mesh
};

///----------------------------------------------------------------------------------------------------
/// Getting const triangle iterator.
///----------------------------------------------------------------------------------------------------
TriangleConstIterator Mesh::getTriangleConstIterator() const
{
	return TriangleConstIterator(mTriangles.begin(), mTriangles.end());
}

///----------------------------------------------------------------------------------------------------
/// Gets number of mesh's triangles.
///----------------------------------------------------------------------------------------------------
unsigned int Mesh::getTriangleCount() const
{
	return mTriangles.size();
}

} // namespace HairShape

} // namespace Stubble

#endif // STUBBLE_MESH_HPP