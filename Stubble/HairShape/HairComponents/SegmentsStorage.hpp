#ifndef STUBBLE_SEGMENTS_STORAGE_HPP
#define STUBBLE_SEGMENTS_STORAGE_HPP

#include "Primitives\BoundingBox.hpp"
#include "HairShape\HairComponents\GuidePosition.hpp"
#include "HairShape\HairComponents\RestPositionsUG.hpp"
#include "HairShape\HairComponents\Segments.hpp"
#include "HairShape\HairComponents\SelectedGuides.hpp"
#include "HairShape\Interpolation\InterpolationGroups.hpp"

namespace Stubble
{

namespace HairShape
{

namespace HairComponents
{

///-------------------------------------------------------------------------------------------------
/// Class for storing all guides segments .
///-------------------------------------------------------------------------------------------------
class SegmentsStorage
{
public:

	///-------------------------------------------------------------------------------------------------
	/// Constructor, inits segments to point along surface normal. 
	///
	/// \param	aRestPositions			the rest positions of guides. 
	/// \param	aInterpolationGroups	Interpolation groups.
	///-------------------------------------------------------------------------------------------------
	SegmentsStorage( const GuidesRestPositions & aRestPositions, 
		const Interpolation::InterpolationGroups & aInterpolationGroups );

	///-------------------------------------------------------------------------------------------------
	/// Constructor, interpolates segments from old segments in aOldStorage
	///
	/// \param	aOldStorage							the old segments storage. 
	/// \param	aOldRestPositionsUG					the old rest positions uniform grid. 
	/// \param	aRestPositions						the rest positions of guides. 
	/// \param	aInterpolationGroups				Interpolation groups.
	/// \param	aNumberOfGuidesToInterpolateFrom	Number of guides to interpolate from
	///-------------------------------------------------------------------------------------------------
	SegmentsStorage( const SegmentsStorage & aOldStorage, const RestPositionsUG & aOldRestPositionsUG,
		const GuidesRestPositions & aRestPositions, 
		const Interpolation::InterpolationGroups & aInterpolationGroups,
		unsigned __int32 aNumberOfGuidesToInterpolateFrom );

	///-------------------------------------------------------------------------------------------------
	/// Constructor, copies only selected segments from aOldStorage
	///
	/// \param	aOldStorage			the old segments storage. 
	/// \param	aRemainingGuides	the remaining guides. 
	///-------------------------------------------------------------------------------------------------
	SegmentsStorage( const SegmentsStorage & aOldStorage, const GuidesIds & aRemainingGuides );

	///----------------------------------------------------------------------------------------------------
	/// Finalizer
	///----------------------------------------------------------------------------------------------------
	inline ~SegmentsStorage();

	///-------------------------------------------------------------------------------------------------
	/// Sets a time frame. 
	///
	/// \param	aTime	Time. 
	///-------------------------------------------------------------------------------------------------
	void setFrame( Time aTime );

	///-------------------------------------------------------------------------------------------------
	/// Replaces the old segments of some guides with aSegmentsChange (used only for undo, redo) 
	///
	/// \param	aSegmentsChange	the segments change. 
	/// 						
	/// \return	segments change in PartialStorage for undo stack 
	///-------------------------------------------------------------------------------------------------
	PartialStorage * replace( const PartialStorage & aSegmentsChange );

	///-------------------------------------------------------------------------------------------------
	/// Propagate changes of selected guides through all time frames. 
	///
	/// \param	aSelectedGuides	the selected guides. 
	///
	/// \return	segments change in PartialStorage for undo stack 
	///-------------------------------------------------------------------------------------------------
	PartialStorage * propagateChanges( const SelectedGuides & aSelectedGuides );

	///-------------------------------------------------------------------------------------------------
	/// Import segments in selected time frame. 
	///
	/// \param	aFrameSegments	the segments in time frame. 
	///-------------------------------------------------------------------------------------------------
	inline void importSegments( const FrameSegments & aFrameSegments );

	///-------------------------------------------------------------------------------------------------
	/// Gets the segments from current time frame. 
	///
	/// \return	The current segments. 
	///-------------------------------------------------------------------------------------------------
	inline const FrameSegments & getCurrentSegments() const;

	///-------------------------------------------------------------------------------------------------
	/// Returns true, if any segments were imported (using importSegments)
	///
	/// \return	true if it succeeds, false if it fails. 
	///-------------------------------------------------------------------------------------------------
	inline bool imported() const;

	///-------------------------------------------------------------------------------------------------
	/// Sets the segments count
	///
	/// \param	aRestPositions			the rest positions of guides. 
	/// \param	aInterpolationGroups	Interpolation groups.
	///-------------------------------------------------------------------------------------------------
	void setSegmentsCount( const GuidesRestPositions & aRestPositions, 
		const Interpolation::InterpolationGroups & aInterpolationGroups );

	///----------------------------------------------------------------------------------------------------
	/// Gets a bounding box. 
	///
	/// \param	aCurrentPositions	the current positions of guides. 
	///
	/// \return	The bounding box. 
		///----------------------------------------------------------------------------------------------------
	BoundingBox getBoundingBox( const GuidesCurrentPositions & aCurrentPositions ) const;

	///-------------------------------------------------------------------------------------------------
	/// Uniformly reposition segments with newly set count. 
	///
	/// \param [in,out]	aGuideSegments	The guide segments. 
	/// \param aCount					The new number of segments;
	///-------------------------------------------------------------------------------------------------
	static void uniformlyRepositionSegments( OneGuideSegments & aGuideSegments, unsigned __int32 aCount );

private:

	/// ------------------------------------------------------------------------------------------
	///  Interpolates segments from old segments
	///
	/// \param	aOldSegments						the old segments. 
	/// \param	aOldRestPositionsUG					the old rest positions uniform grid. 
	/// \param	aRestPositions						the rest positions of guides. 
	/// \param	aInterpolationGroups				Interpolation groups. 
	/// \param	aNumberOfGuidesToInterpolateFrom	Number of guides to interpolate from
	/// \param [in,out]	aOutputSegments				the output segments. 
	///----------------------------------------------------------------------------------------------------
	void InterpolateFrame( const FrameSegments & aOldSegments, const RestPositionsUG & aOldRestPositionsUG,
		const GuidesRestPositions & aRestPositions, 
		const Interpolation::InterpolationGroups & aInterpolationGroups,
		unsigned __int32 aNumberOfGuidesToInterpolateFrom, 
		FrameSegments & aOutputSegments ) const;

	///-------------------------------------------------------------------------------------------------
	/// Calculates how much change in one frame affects other frame.
	///
	/// \param	aTimeDifference	Distance between frames in time.
	///
	/// \return	Affect factor. 
	///-------------------------------------------------------------------------------------------------
	Real timeAffectFactor( Time aTimeDifference );

	///-------------------------------------------------------------------------------------------------
	/// Propagate changes from current frame to other frames.
	/// Must be used before segments are exported.
	///-------------------------------------------------------------------------------------------------
	void propagateChangesThroughTime();

	///-------------------------------------------------------------------------------------------------
	/// Propagete changes from current frame to selected frame . 
	///
	/// \param [in,out]	aGuides	The guides of selected frame. 
	/// \param	aFactor			The affect factor ( see timeAffectFactor ). 
	///-------------------------------------------------------------------------------------------------
	void propageteChangesToFrame( GuidesSegments & aGuides, Real aFactor );

	AllFramesSegments mSegments;  ///< The all segments (exists after the import!)

	FrameSegments mCurrent; ///< The current frame segments

	bool mAreCurrentSegmentsDirty;  ///< true if are current segments dirty and need to be propagated
};

// inline functions implementation

inline SegmentsStorage::~SegmentsStorage()
{
}

inline void SegmentsStorage::importSegments( const FrameSegments & aFrameSegments )
{
	mSegments.insert( std::make_pair( aFrameSegments.mFrame, aFrameSegments ) );
	mAreCurrentSegmentsDirty = false; // Signal that current frame is useless
}

inline const FrameSegments & SegmentsStorage::getCurrentSegments() const
{
	return mCurrent;
}

inline bool SegmentsStorage::imported() const
{
	return !mSegments.empty();
}

} // namespace HairComponents

} // namespace HairShape

} // namespace Stubble

#endif // STUBBLE_SEGMENTS_STORAGE_HPP