#ifndef STUBBLE_SIMPLE_POSITION_GENERATOR_HPP
#define STUBBLE_SIMPLE_POSITION_GENERATOR_HPP

#include "HairShape/Generators/UVPointGenerator.hpp"
#include "HairShape/Mesh/Mesh.hpp"
#include "PositionGenerator.hpp"

namespace Stubble
{

namespace HairShape
{

namespace Interpolation
{

///-------------------------------------------------------------------------------------------------
/// The position generator of interpolated hair used in RenderMan plugin.
///-------------------------------------------------------------------------------------------------
class SimplePositionGenerator : public PositionGenerator
{
public:

	///-------------------------------------------------------------------------------------------------
	/// Constructor. 
	///
	/// \param	aRestPoseMesh				The rest pose mesh. 
	/// \param	aCurrentMesh				The current mesh. 
	/// \param [in,out]	aUVPointGenerator	The uv point generator. 
	/// \param	aHairCount					Number of the hairs. 
	/// \param	aHairStartIndex				Index of first hair.
	///-------------------------------------------------------------------------------------------------
	inline SimplePositionGenerator( const Mesh & aRestPoseMesh, const Mesh & aCurrentMesh,
		UVPointGenerator & aUVPointGenerator, unsigned __int32 aHairCount,
		unsigned __int32 aHairStartIndex );

	///-------------------------------------------------------------------------------------------------
	/// Finaliser. 
	///-------------------------------------------------------------------------------------------------
	inline ~SimplePositionGenerator();
	
	///-------------------------------------------------------------------------------------------------
	/// Generates position of interpolated hair. 
	///
	/// \param [in,out]	aCurrentPosition	The current position of hair on mesh. 
	/// \param [in,out]	aRestPosition		The rest position of hair in 3D space. 
	///-------------------------------------------------------------------------------------------------
	inline void generate( MeshPoint & aCurrentPosition, MeshPoint & aRestPosition );
	
	///-------------------------------------------------------------------------------------------------
	/// Generates position of interpolated hair on displaced mesh. 
	///
	/// \param [in,out]	aCurrentPosition	The current position of hair on displaced mesh. 
	/// \param [in,out]	aRestPosition		The rest position of hair in 3D space. 
	/// \param aDisplacementTexture			The texture defining displacement of mesh.
	///	\param aDisplacementFactor			The displacement texture will be mutliplied by this factor.
	///-------------------------------------------------------------------------------------------------
	inline void generate( MeshPoint & aCurrentPosition, MeshPoint & aRestPosition,
		const Texture & aDisplacementTexture, Real aDisplacementFactor );

	///-------------------------------------------------------------------------------------------------
	/// Gets the number of the hair to be interpolated.
	///
	/// \return	The hair count. 
	///-------------------------------------------------------------------------------------------------
	inline unsigned __int32 getHairCount() const;
	
	///-------------------------------------------------------------------------------------------------
	/// Gets the hair start index. 
	///
	/// \return	The hair start index. 
	///-------------------------------------------------------------------------------------------------
	inline unsigned __int32 getHairStartIndex() const;

private:

	const Mesh & mCurrentMesh;	///< The current mesh

	const Mesh & mRestPoseMesh;   ///< The rest pose mesh

	UVPointGenerator & mUVPointGenerator;   ///< The uv point generator

	unsigned __int32 mHairCount;	///< Number of the interpolated hair.

	unsigned __int32 mHairStartIndex;   ///< The start index of hair
};

// inline functions implementation

inline SimplePositionGenerator::SimplePositionGenerator( const Mesh & aRestPoseMesh, const Mesh & aCurrentMesh,
	UVPointGenerator & aUVPointGenerator, unsigned __int32 aHairCount,
	unsigned __int32 aHairStartIndex ):
	mRestPoseMesh( aRestPoseMesh ),
	mCurrentMesh( aCurrentMesh ),
	mUVPointGenerator( aUVPointGenerator ),
	mHairCount( aHairCount ),
	mHairStartIndex( aHairStartIndex )
{
}

inline SimplePositionGenerator::~SimplePositionGenerator()
{
}

inline void SimplePositionGenerator::generate( MeshPoint & aCurrentPosition, MeshPoint & aRestPosition )
{
	UVPoint uv = mUVPointGenerator.next(); // Generate uv pos
	aCurrentPosition = mCurrentMesh.getMeshPoint( uv );
	aRestPosition = mRestPoseMesh.getMeshPoint( uv );
}

inline void SimplePositionGenerator::generate( MeshPoint & aCurrentPosition, MeshPoint & aRestPosition,
	const Texture & aDisplacementTexture, Real aDisplacementFactor )
{
	UVPoint uv = mUVPointGenerator.next(); // Generate uv pos
	aCurrentPosition = mCurrentMesh.getDisplacedMeshPoint( uv, aDisplacementTexture, aDisplacementFactor );
	aRestPosition = mRestPoseMesh.getMeshPoint( uv );
}

inline unsigned __int32 SimplePositionGenerator::getHairCount() const
{
	return mHairCount;
}

inline unsigned __int32 SimplePositionGenerator::getHairStartIndex() const
{
	return mHairStartIndex;
}

} // namespace Interpolation

} // namespace HairShape

} // namespace Stubble

#endif // STUBBLE_SIMPLE_POSITION_GENERATOR_HPP