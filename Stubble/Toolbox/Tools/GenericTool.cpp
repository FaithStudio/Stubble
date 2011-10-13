#include "GenericTool.hpp"
#include "../ToolShapes/CircleToolShape/CircleToolShape.hpp"

const char *toolScaleFlag = "-ts";
const char *toolScaleLongFlag = "-toolScale";
const char *brushModeChoiceFlag = "-bmc";
const char *brushModeChoiceLongFlag = "-brushModeChoice";
const char *brushSensitivityFlag = "-bse"; // :)
const char *brushSensitivityLongFlag = "-brushSensitivity";

namespace Stubble
{

namespace Toolbox
{

GenericTool::GenericTool() :
	mShape(0), mScale(1.0)
{
}

GenericTool::GenericTool(ToolShape *aToolShape) :
	mShape(aToolShape), mScale(1.0)
{
}

M3dView* GenericTool::getActiveView()
{
	return &mView;
}

void GenericTool::drawToolShape( short aScreenCoords[ 2 ], QEvent::Type aEventType )
{
	mShape->draw( &mView, aScreenCoords, aEventType );
}

ToolShape *GenericTool::getToolShape()
{
	return mShape;
}

double GenericTool::getToolScale()
{
	return mScale;
}


GenericToolCommand::GenericToolCommand()
{
}

GenericToolCommand::~GenericToolCommand()
{
}

MStatus	GenericToolCommand::appendSyntax()
{
	MSyntax syn = syntax();
	
	syn.addFlag( brushModeChoiceFlag, brushModeChoiceLongFlag, MSyntax::kLong );

	syn.addFlag( toolScaleFlag, toolScaleLongFlag, MSyntax::kLong );

	syn.addFlag( brushSensitivityFlag, brushSensitivityLongFlag, MSyntax::kLong );

	return MS::kSuccess;
}

} // namespace Toolbox

} // namespace Stubble