#ifndef STUBBLE_INTERPOLATED_HAIR_HPP
#define STUBBLE_INTERPOLATED_HAIR_HPP

#include "HairShape/Generators/UVPointGenerator.hpp"

#include "HairGenerator.tmpl.hpp"
#include "HairProperties.hpp"
#include "MayaOutputGenerator.hpp"
#include "MayaPositionGenerator.hpp"

#include <vector>
#include <omp.h>

namespace Stubble
{

namespace HairShape
{

namespace Interpolation
{

///-------------------------------------------------------------------------------------------------
/// Class for generating and displaying interpolated hair inside Maya
///-------------------------------------------------------------------------------------------------
class InterpolatedHair
{
public:

	///-------------------------------------------------------------------------------------------------
	/// Default constructor. 
	///-------------------------------------------------------------------------------------------------
	inline InterpolatedHair();

	///-------------------------------------------------------------------------------------------------
	/// Finaliser. 
	///-------------------------------------------------------------------------------------------------
	inline ~InterpolatedHair();

	///-------------------------------------------------------------------------------------------------
	/// Generates interpolated hair. 
	///
	/// \param [in,out]	aUVPointGenerator	The uv point generator. 
	/// \param	aCurrentMesh				The current mesh. 
	/// \param	aRestPoseMesh				The rest pose mesh. 
	/// \param	aHairProperties				The hair properties. 
	/// \param	aCount						Number of the hair. 
	///-------------------------------------------------------------------------------------------------
	inline void generate( UVPointGenerator & aUVPointGenerator, const MayaMesh & aCurrentMesh,
		const Mesh & aRestPoseMesh, const HairProperties & aHairProperties, unsigned __int32 aCount );

	///-------------------------------------------------------------------------------------------------
	/// Updates hair positions and local space due to mesh update. This method can not handle 
	/// mesh topology update (generate should be used instead).
	///
	/// \param	aCurrentMesh	The current mesh. 
	/// \param	aHairProperties	The hair properties. 
	///-------------------------------------------------------------------------------------------------
	inline void meshUpdate( const MayaMesh & aCurrentMesh, const HairProperties & aHairProperties );

	///-------------------------------------------------------------------------------------------------
	/// Updates hair properties.
	///
	/// \param	aHairProperties	The hair properties. 
	///-------------------------------------------------------------------------------------------------
	inline void propertiesUpdate( const HairProperties & aHairProperties );

	///-------------------------------------------------------------------------------------------------
	/// Draws interpolated hair. 
	///-------------------------------------------------------------------------------------------------
	inline void draw();

	static const unsigned __int32 MAX_HAIR_COUNT = 10000;   ///< Number of maximum hair

private:

	///-------------------------------------------------------------------------------------------------
	/// Hair interpolated by one thread.
	///-------------------------------------------------------------------------------------------------
	class ThreadData
	{
	public:

		///-------------------------------------------------------------------------------------------------
		/// Default constructor. 
		///-------------------------------------------------------------------------------------------------
		inline ThreadData();

		MayaPositionGenerator::GeneratedPosition * mGeneratedPositions; ///< The generated positions for this thread
		
		unsigned __int32 mHairCount; ///< The hair count for this thread

		MayaPositionGenerator mPositionGenerator;   ///< The hair position generator

		MayaOutputGenerator mOutputGenerator;   ///< The output hair generator

		HairGenerator< MayaPositionGenerator, MayaOutputGenerator > mHairGenerator; ///< The hair generator
	};

	ThreadData * mThreads;  ///< The threads

	ThreadData * mThreadsEnd;   ///< The threads end

	unsigned __int32 mThreadsCount; ///< Number of threads

	MayaPositionGenerator::GeneratedPosition * mGeneratedPositions; ///< The generated positions of hair

	unsigned __int32 mHairCount;	///< Number of hair

	unsigned __int32 mAllocatedHairCount;   ///< Number of allocated and generated hair
};

// inline functions implementation

inline InterpolatedHair::InterpolatedHair():
	mGeneratedPositions( 0 ),
	mAllocatedHairCount( 0 ),
	mThreadsCount( 
	#ifdef _OPENMP
		omp_get_max_threads()
	#else
		1
	#endif
	)
{
	mThreads = new ThreadData[ mThreadsCount ];
	mThreadsEnd = mThreads + mThreadsCount;
}

inline InterpolatedHair::~InterpolatedHair()
{
	delete [] mThreads;
	delete [] mGeneratedPositions;
}

inline void InterpolatedHair::generate( UVPointGenerator & aUVPointGenerator, const MayaMesh & aCurrentMesh,
	const Mesh & aRestPoseMesh, const HairProperties & aHairProperties, unsigned __int32 aCount )
{
	if ( aCount > mAllocatedHairCount )
	{
		// Clear old memory
		delete [] mGeneratedPositions;
		mGeneratedPositions = 0;
		// Sets new size
		mAllocatedHairCount = aCount << 1;
		mAllocatedHairCount = mAllocatedHairCount > MAX_HAIR_COUNT ? MAX_HAIR_COUNT : mAllocatedHairCount;
		// Allocate memory for generated positions
		mGeneratedPositions = new MayaPositionGenerator::GeneratedPosition[ mAllocatedHairCount ];
		// Iterates over all positions
		MayaPositionGenerator::GeneratedPosition * endIteration = mGeneratedPositions + mAllocatedHairCount;
		aUVPointGenerator.reset();
		for ( MayaPositionGenerator::GeneratedPosition * it = mGeneratedPositions; it != endIteration; ++it )
		{
			it->mUVPoint = aUVPointGenerator.next(); // Generate uv pos
			// Calculate positions
			it->mCurrentPosition = aCurrentMesh.getMeshPoint( it->mUVPoint );
			it->mRestPosition = aRestPoseMesh.getMeshPoint( it->mUVPoint );
		}
	}
	// Copy new hair count
	mHairCount = aCount;
	// Calculate number of hair per thread
	unsigned __int32 aCountPerThread = ( mHairCount / mThreadsCount ) + ( mHairCount % mThreadsCount );
	MayaPositionGenerator::GeneratedPosition * current = mGeneratedPositions, * end = mGeneratedPositions + aCount;
	// For every thread - single threaded
	for ( ThreadData * it = mThreads; it != mThreadsEnd; ++it )
	{
		// Sets hair positions start
		it->mGeneratedPositions = current;
		// Sets hair count
		current += aCountPerThread;
		current = current > end ? end : current;
		it->mHairCount = static_cast< unsigned __int32 >( current - it->mGeneratedPositions );
		it->mPositionGenerator.set( it->mGeneratedPositions, it->mHairCount, 
			static_cast< unsigned __int32 >( it->mGeneratedPositions - mGeneratedPositions ) );
	}
	// For every thread - multi threaded
	#pragma omp parallel for
	for ( int i = 0; i < static_cast< int >( mThreadsCount ); ++i )
	{
		ThreadData * it = mThreads + i;
		if ( it->mHairCount > 0 )
		{
			it->mHairGenerator.generate( aHairProperties );
		}
	}
}

inline void InterpolatedHair::meshUpdate( const MayaMesh & aCurrentMesh, const HairProperties & aHairProperties )
{
	// Iterates over all used positions
	const MayaPositionGenerator::GeneratedPosition * endIteration = mGeneratedPositions + mHairCount;
	for ( MayaPositionGenerator::GeneratedPosition * it = mGeneratedPositions; it != endIteration; ++it )
	{
		// Alter hair points local space
		it->mCurrentPosition = aCurrentMesh.getMeshPoint( it->mUVPoint );
	}
	propertiesUpdate( aHairProperties );
}

inline void InterpolatedHair::propertiesUpdate( const HairProperties & aHairProperties )
{
	// For every thread - multi threaded
	#pragma omp parallel for
	for ( int i = 0; i < static_cast< int >( mThreadsCount ); ++i )
	{
		ThreadData * it = mThreads + i;
		if ( it->mHairCount > 0 )
		{
			it->mPositionGenerator.reset();
			it->mHairGenerator.generate( aHairProperties );
		}
	}
}

inline void InterpolatedHair::draw()
{
	// For every thread - single threaded
	for ( ThreadData * it = mThreads; it != mThreadsEnd; ++it )
	{
		if ( it->mHairCount > 0 )
		{
			it->mOutputGenerator.draw();
		}
	}
}

inline InterpolatedHair::ThreadData::ThreadData():
	mHairGenerator( mPositionGenerator, mOutputGenerator )
{
}

} // namespace Interpolation

} // namespace HairShape

} // namespace Stubble

#endif // STUBBLE_INTERPOLATED_HAIR_HPP