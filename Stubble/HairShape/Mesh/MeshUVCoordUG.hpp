#ifndef STUBBLE_MESH_UV_COORD_UG_HPP
#define STUBBLE_MESH_UV_COORD_UG_HPP

#include "HairShape\Mesh\Triangle.hpp"
#include "HairShape\Mesh\UVPoint.hpp"

namespace Stubble
{

namespace HairShape
{

///-------------------------------------------------------------------------------------------------
/// Mesh uv coordinates uniform grid used after topology change
///-------------------------------------------------------------------------------------------------
class MeshUVCoordUG
{
public:

	///-------------------------------------------------------------------------------------------------
	/// Default constructor. 
	///-------------------------------------------------------------------------------------------------
	MeshUVCoordUG();

	///-------------------------------------------------------------------------------------------------
	/// Finaliser. 
	///-------------------------------------------------------------------------------------------------
	~MeshUVCoordUG();

	///-------------------------------------------------------------------------------------------------
	///  Builds the uniform grid. 
	///
	/// \param [in,out]	aTriangles	the mesh triangles ( aTriangles are destroyed in process ). 
	///-------------------------------------------------------------------------------------------------
	void build( Triangles & aTriangles );

	///-------------------------------------------------------------------------------------------------
	/// Gets a uv point on current mesh. 
	///
	/// \param	aPoint	a point on old mesh ( only u,v coordinates ared used )
	///
	/// \return	The uv point ( triangles id == -1 if point does not lie on mesh). 
	///-------------------------------------------------------------------------------------------------
	UVPoint getUVPoint( const MeshPoint &aPoint ) const;

	///-------------------------------------------------------------------------------------------------
	/// Sets the dirty bit. 
	///-------------------------------------------------------------------------------------------------
	inline void setDirty();

	///-------------------------------------------------------------------------------------------------
	/// Query if this object is dirty. 
	///
	/// \return	true if dirty, false if not. 
	///-------------------------------------------------------------------------------------------------
	inline bool isDirty() const;

private:
	bool mDirtyBit; ///< true to dirty bit
};

// inline functions implementation

inline void MeshUVCoordUG::setDirty()
{
	mDirtyBit = true;
}

inline bool MeshUVCoordUG::isDirty() const
{
	return mDirtyBit;
}

} // namespace HairShape

} // namespace Stubble

#endif // STUBBLE_MESH_UV_COORD_UG_HPP