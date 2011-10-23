#include "HairGuides.hpp"

#include <cassert>

namespace Stubble
{

namespace HairShape
{
	
namespace HairComponents
{

HairGuides::HairGuides():
	mSegmentsStorage( 0 ),
	mBoundingBoxDirtyFlag( false ),
	mNumberOfGuidesToInterpolateFrom( 3 )
{
}

HairGuides::~HairGuides()
{
	clearSelectedGuides();
	delete mSegmentsStorage;
}

void HairGuides::applySelection( MSelectInfo &aSelectInfo, MSelectionList &aSelectionList,  MPointArray &aWorldSpaceSelectPts )
{
	if ( mAllSegmentsUG.isDirty() ) // Is UG for selection up-to-date ?
	{
		mAllSegmentsUG.build( mCurrentPositions, mSegmentsStorage->getCurrentSegments() );
	}
	// Hide old selection
	//mDisplayedGuides.selectionRebuild( mSelectedGuides, false );
	// Rebuild selected segments UG
	clearSelectedGuides();
	mSelectedSegmentsUG.build(mCurrentPositions, mSegmentsStorage->getCurrentSegments(), aSelectInfo, aSelectionList, aWorldSpaceSelectPts, mSelectedGuides);
	// Display selection
	mDisplayedGuides.selectionRebuild( mSelectedGuides, true );
}

BoundingBox HairGuides::getBoundingBox()
{
	if ( mBoundingBoxDirtyFlag && mSegmentsStorage )
	{
		mBoundingBox = mSegmentsStorage->getBoundingBox( mCurrentPositions );
		mBoundingBoxDirtyFlag = false;
	}
	return mBoundingBox;
}

const SegmentsUG & HairGuides::getSelectedGuidesUG()
{
	if ( mAllSegmentsUG.isDirty() ) // Is UG for selection up-to-date ?
	{
		mSelectedSegmentsUG.build( mSelectedGuides, true );
	}
	return mSelectedSegmentsUG;
}

void HairGuides::updateGuides( bool aStoreUpdate )
{
	// Update selection
	mDisplayedGuides.selectionRebuild( mSelectedGuides, true );
	// Update bounding box
	// For every selected guide
	for( SelectedGuides::const_iterator it = mSelectedGuides.begin(); it != mSelectedGuides.end(); ++it )
	{
		if ( (*it)->mDirtyFlag )
		{
			// For every segment
			for ( Segments::const_iterator segIt = (*it)->mGuideSegments.mSegments.begin();
				segIt != (*it)->mGuideSegments.mSegments.end(); ++segIt )
			{
				// Transform to world and expand bbox
				mBoundingBox.expand( mCurrentPositions[ (*it)->mGuideId].mPosition.toWorld( *segIt ) );
			}
		}
	}
	if ( aStoreUpdate ) // Final update ? ( user has stop using brush, etc. )
	{
		// Propagate changes to all frames and update undo stack
		mUndoStack.add( mSegmentsStorage->propagateChanges( mSelectedGuides ) );
		// Set guides segments dirty
		mAllSegmentsUG.setDirty();
		mSelectedSegmentsUG.setDirty();
		// Set selected guides as non dirty
		// For every selected guide
		for( SelectedGuides::iterator it = mSelectedGuides.begin(); it != mSelectedGuides.end(); ++it )
		{
			(*it)->mDirtyFlag = false;
		}
	}
}

const RestPositionsUG & HairGuides::getGuidesPositionsUG( const Interpolation::InterpolationGroups & aInterpolationGroups )
{
	if ( mRestPositionsUG.isDirty() ) // Is segments UG up-to-date ?
	{
		mRestPositionsUG.build( mRestPositions, aInterpolationGroups );
	}
	return mRestPositionsUG;
}

void HairGuides::draw()
{
	if ( mDisplayedGuides.isDirty() ) // Is display list up-to-date ?
	{
		mDisplayedGuides.build( mCurrentPositions, mSegmentsStorage->getCurrentSegments(), mSelectedGuides );
	}
	mDisplayedGuides.draw();
}

void HairGuides::importNURBS()
{
	MSelectionList selection;
	MGlobal::getActiveSelectionList( selection );		
	for (MTime time = MAnimControl::minTime(); time <= MAnimControl::maxTime(); time++ )
	{
		MAnimControl::setCurrentTime( time );		
		FrameSegments frameSegments;
		frameSegments.mFrame = time.as( MTime::uiUnit() );
		MItSelectionList curveIt( selection, MFn::kNurbsCurve );
		for ( ; !curveIt.isDone(); curveIt.next() ) // over all curves
		{
			MDagPath path;
			curveIt.getDagPath( path );
			MFnNurbsCurve curve( path );		
			MPointArray pointArray;
			curve.getCVs( pointArray );

			OneGuideSegments guideSegments;		
			/* TODO compute segment length (from curve CV distances?) */
			for ( unsigned __int32 i = 0; i < pointArray.length(); i++ ) // over all segments
			{
				guideSegments.mSegments.push_back( Vector3D< Real >( pointArray[ i ] ) );
			}		
			frameSegments.mSegments.push_back( guideSegments );
		}
		mSegmentsStorage->importSegments( frameSegments );		
	}
	// All nurbs has been imported, we have to recalculate for current time	
	setCurrentTime( MAnimControl::currentTime().as( MTime::uiUnit() ) ); // XXX is this correct unit?	
}

void HairGuides::exportToNURBS()
{
	if ( mSegmentsStorage->imported() )
	{
		throw StubbleException( " HairGuides::exportToNURBS : already used import command ! " );
	}
		
	for ( SelectedGuides::const_iterator hairIt = mSelectedGuides.begin(); 
		hairIt != mSelectedGuides.end(); hairIt++ )
	{		
		MPointArray pointArray;			
		for ( Segments::const_iterator segmentIt = ( *hairIt )->mGuideSegments.mSegments.begin()
			; segmentIt != ( *hairIt )->mGuideSegments.mSegments.end()
			; segmentIt++ )
		{			
			// Transform to world coordinates and append
			pointArray.append( ( *hairIt )->mPosition.mPosition.toWorld( *segmentIt ).toMayaPoint() );
		}
		MFnNurbsCurve nurbsCurve;
		MStatus status;

		nurbsCurve.createWithEditPoints( pointArray, 1, MFnNurbsCurve::kOpen, 
			false, false, true, MObject::kNullObj, &status );
		if ( status != MStatus::kSuccess )
		{
			throw StubbleException( " HairGuides::exportNURBS : Failed to create NURBS curve. " );
		}
	}	
}

void HairGuides::setCurrentTime( Time aTime )
{
	if ( !mSegmentsStorage->imported() ) // Same guides for all frames ?
	{
		return; // Time change has no meaning
	}
	mSegmentsStorage->setFrame( aTime );
	// Everything has changed...
	mDisplayedGuides.setDirty();
	mRestPositionsUG.setDirty();
	mAllSegmentsUG.setDirty();
	mUndoStack.clear();
	updateSelectedGuides();
	mBoundingBoxDirtyFlag = true;
}

GuideId HairGuides::meshUpdate( const MayaMesh & aMayaMesh, bool aTopologyChanged )
{
	if ( aTopologyChanged )
	{
		const MeshUVCoordUG & positionConverter = aMayaMesh.getMeshUVCoordUG();
		// Temporary hair guides
		SegmentsStorage * tmpSegmentsStorage;
		GuidesRestPositions tmpRestPositions;
		tmpRestPositions.reserve( mRestPositions.size() ); // Optimalization
		mCurrentPositions.clear();
		mCurrentPositions.reserve( mRestPositions.size() ); // Optimalization
		GuidesIds remainingGuides; // Stores ids of remaining guides
		remainingGuides.reserve( mRestPositions.size() ); // Optimalization
		// Calculate new positions
		for( GuidesRestPositions::iterator restPosIt = mRestPositions.begin(); restPosIt != mRestPositions.end(); 
			++restPosIt )
		{
			GuideRestPosition restPos;
			restPos.mUVPoint = positionConverter.getUVPoint( restPosIt->mPosition );
			if ( restPos.mUVPoint.getTriangleID() >= 0 ) // Topology change did not destroy the guide
			{
				// Handle rest position
				restPos.mPosition = aMayaMesh.getRestPose().getIncompleteMeshPoint( restPos.mUVPoint );
				tmpRestPositions.push_back( restPos );
				// Handle current position
				GuideCurrentPosition currPos;
				currPos.mPosition = aMayaMesh.getMeshPoint( restPosIt->mUVPoint );
				currPos.mPosition.getLocalTransformMatrix( currPos.mLocalTransformMatrix );
				currPos.mPosition.getWorldTransformMatrix( currPos.mWorldTransformMatrix );
				mCurrentPositions.push_back( currPos );
				// Remember id
				remainingGuides.push_back( static_cast< GuideId >( restPosIt - mRestPositions.begin() ) );
			}
		}
		// Copy old segments
		tmpSegmentsStorage = new SegmentsStorage( *mSegmentsStorage, remainingGuides );
		// Now we can throw away old data
		std::swap( tmpRestPositions, mRestPositions );
		std::swap( tmpSegmentsStorage, mSegmentsStorage );
		delete tmpSegmentsStorage;
		// Rest positions has changed...
		mRestPositionsUG.setDirty();
		clearSelectedGuides();
	}
	else
	{
		// Update current positions
		GuidesCurrentPositions::iterator currPosIt = mCurrentPositions.begin();
		for( GuidesRestPositions::iterator restPosIt = mRestPositions.begin(); restPosIt != mRestPositions.end(); 
			++restPosIt, ++currPosIt )
		{
			currPosIt->mPosition = aMayaMesh.getMeshPoint( restPosIt->mUVPoint );
			currPosIt->mPosition.getLocalTransformMatrix( currPosIt->mLocalTransformMatrix );
			currPosIt->mPosition.getWorldTransformMatrix( currPosIt->mWorldTransformMatrix );
		}
		updateSelectedGuides();
	}
	// Current positions has changed...
	mDisplayedGuides.setDirty();
	mAllSegmentsUG.setDirty();
	mUndoStack.clear();
	mBoundingBoxDirtyFlag = true;
	return static_cast< GuideId >( mRestPositions.size() );
}

void HairGuides::undo()
{
	bool noRedo = mUndoStack.canRedo();
	if ( !mUndoStack.canUndo() )
	{
		return;
	}
	mUndoStack.updateAfterUndo( mSegmentsStorage->replace( mUndoStack.undo() ) );
	// Segments has changed...
	mDisplayedGuides.setDirty();
	mAllSegmentsUG.setDirty();
	updateSelectedGuides();
	mBoundingBoxDirtyFlag = true;
}

void HairGuides::redo()
{
	if ( !mUndoStack.canRedo() )
	{
		return;
	}
	mUndoStack.updateAfterRedo( mSegmentsStorage->replace( mUndoStack.redo() ) );
	// Segments has changed...
	mDisplayedGuides.setDirty();
	mAllSegmentsUG.setDirty();
	updateSelectedGuides();
	mBoundingBoxDirtyFlag = true;
}

void HairGuides::emptyHistoryStack()
{
	mUndoStack.clear();
}

void HairGuides::generate( UVPointGenerator & aUVPointGenerator, const MayaMesh & aMayaMesh, 
	const Interpolation::InterpolationGroups & aInterpolationGroups, unsigned __int32 aCount, 
	bool aInterpolateFromPrevious )
{
	// Temporary hair guides
	SegmentsStorage * tmpSegmentsStorage;
	GuidesRestPositions tmpRestPositions;
	tmpRestPositions.resize( aCount );
	mCurrentPositions.clear();
	mCurrentPositions.resize( aCount );
	// Start generating positions
	GuidesRestPositions::iterator restPosIt = tmpRestPositions.begin();
	GuidesCurrentPositions::iterator currPosIt = mCurrentPositions.begin();
	aUVPointGenerator.reset();
	for ( unsigned __int32 i = 0; i < aCount; ++i, ++restPosIt, ++currPosIt )
	{
		// Generate rest pose position
		restPosIt->mUVPoint = aUVPointGenerator.next();
		restPosIt->mPosition = aMayaMesh.getRestPose().getIncompleteMeshPoint( restPosIt->mUVPoint );
		// Calculate current position
		currPosIt->mPosition = aMayaMesh.getMeshPoint( restPosIt->mUVPoint );
		currPosIt->mPosition.getLocalTransformMatrix( currPosIt->mLocalTransformMatrix );
		currPosIt->mPosition.getWorldTransformMatrix( currPosIt->mWorldTransformMatrix );
	}
	// Now we can create new segments
	if ( aInterpolateFromPrevious )
	{
		if ( mSegmentsStorage == 0 )
		{
			throw StubbleException(" HairGuides::generate : No old segments to interpolate from ! ");
		}
		tmpSegmentsStorage = new SegmentsStorage( *mSegmentsStorage, getGuidesPositionsUG( aInterpolationGroups ), 
			tmpRestPositions, aInterpolationGroups, mNumberOfGuidesToInterpolateFrom );
	}
	else
	{
		tmpSegmentsStorage = new SegmentsStorage( tmpRestPositions, aInterpolationGroups );
	}
	// Now we can throw away old data
	std::swap( tmpRestPositions, mRestPositions );
	std::swap( tmpSegmentsStorage, mSegmentsStorage );
	delete tmpSegmentsStorage;
	// Everything has changed...
	mUndoStack.clear();
	mDisplayedGuides.setDirty();
	mRestPositionsUG.setDirty();
	mAllSegmentsUG.setDirty();
	clearSelectedGuides();
	mBoundingBoxDirtyFlag = true;
}

void HairGuides::updateSegmentsCount( const Interpolation::InterpolationGroups & aInterpolationGroups )
{
	mSegmentsStorage->setSegmentsCount( mRestPositions, aInterpolationGroups );
	// Segments has changed...
	mUndoStack.clear();
	mDisplayedGuides.setDirty();
	mAllSegmentsUG.setDirty();
	updateSelectedGuides();
	mBoundingBoxDirtyFlag = true;
}

void HairGuides::clearSelectedGuides()
{
	for ( SelectedGuides::iterator guideIt = mSelectedGuides.begin();
		guideIt != mSelectedGuides.end(); ++guideIt )
	{
		delete *guideIt;
	}
	mSelectedGuides.clear();
	mSelectedSegmentsUG.setDirty();
}

void HairGuides::updateSelectedGuides()
{
	const GuidesSegments & guideSegments = mSegmentsStorage->getCurrentSegments().mSegments;
	for ( SelectedGuides::iterator guideIt = mSelectedGuides.begin();
		guideIt != mSelectedGuides.end(); ++guideIt )
	{
		SelectedGuide & guide = **guideIt;
		// Updates segments info
		guide.mGuideSegments = guideSegments[ guide.mGuideId ];
		// Update position info
		const GuideCurrentPosition & pos = mCurrentPositions[ guide.mGuideId ];
		guide.mNormal = pos.mPosition.getNormal();
		guide.mPosition = pos;
		// Update flags
		guide.mDirtyFlag = true;
		guide.mDirtyRedrawFlag = true;
	} // for each guide
	// We will not update all segments ug, it will be updated when necessary
	// for each guide, we will only set selected segments as dirty
	mSelectedSegmentsUG.setDirty();
}

} // namespace HairComponents

} // namespace HairShape

} // namespace Stubble