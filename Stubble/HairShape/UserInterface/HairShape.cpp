#include "HairShape.hpp"

namespace Stubble
{

namespace HairShape
{

const MTypeId HairShape::typeId( 0x80002 );
const MString HairShape::typeName( "HairShape" );

/// attributes for the locator
MObject HairShape::countAttr;
MObject HairShape::genCountAttr;
MObject HairShape::lengthAttr;
MObject HairShape::surfaceAttr;
MObject HairShape::randomXAttr;
MObject HairShape::randomYAttr;
MObject HairShape::randomZAttr;
MObject HairShape::genRandomXAttr;
MObject HairShape::genRandomYAttr;
MObject HairShape::genRandomZAttr;
MObject HairShape::redrawAttr;
MObject HairShape::segmentsAttr;
MObject HairShape::textureAttr;
MObject HairShape::displayNormalsAndTangentsAttr;

HairShape::HairShape()
{
}

bool HairShape::isBounded() const
{
	return true;
}

MBoundingBox HairShape::boundingBox() const
{
	MObject thisNode = thisMObject();
	int sent = 0;

	// get sentinel plug, causes update
	MPlug RedrawPlug(thisNode, HairShape::redrawAttr );
	RedrawPlug.getValue( sent );

	// returns updated bounding box
	return mBoundingBox;
}

MStatus HairShape::compute(const MPlug &plug, MDataBlock &dataBlock)
{
	MStatus status;

	//we only care about the sentinelAttr, ignore stuff like localScale
	if(plug == redrawAttr)
	{
		dataBlock.setClean(redrawAttr);

		MObject meshObj = dataBlock.inputValue(surfaceAttr).asMesh(); //get input mesh
		if(meshObj == MObject::kNullObj)
		{
			return MS::kFailure;
		}

		// TODO
		/*
		if ( !callbackIsRegistered )
		{
			MStatus status = RegisterTopologyCallback();
			if ( status != MStatus::kSuccess )
			{
				return status;
			}
		}
		*/

		// getting hair attributes
		/*
		const int count = dataBlock.inputValue(countAttr).asInt();
		length = dataBlock.inputValue(lengthAttr).asFloat();
		genCount = dataBlock.inputValue(genCountAttr).asInt();
		const float randomX = dataBlock.inputValue(randomXAttr).asFloat();
		const float randomY = dataBlock.inputValue(randomYAttr).asFloat();
		const float randomZ = dataBlock.inputValue(randomZAttr).asFloat();
		genRandom.x = dataBlock.inputValue(genRandomXAttr).asFloat();
		genRandom.y = dataBlock.inputValue(genRandomYAttr).asFloat();
		genRandom.z = dataBlock.inputValue(genRandomZAttr).asFloat();
		const int segments = dataBlock.inputValue(segmentsAttr).asInt();
		MString texture = dataBlock.inputValue(textureAttr).asString();
		displayNormalsAndTangents = dataBlock.inputValue(displayNormalsAndTangentsAttr).asBool();
		*/
	}

	return MS::kSuccess;
}

void* HairShape::creator()
{
	return new HairShape();
}

void HairShape::draw()
{
	// TODO
}

} // namespace HairShape

} // namespace Stubble