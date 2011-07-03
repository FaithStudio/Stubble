#ifndef STUBBLE_GENERIC_TOOL_HPP
#define STUBBLE_GENERIC_TOOL_HPP

#include <maya/M3dView.h>
#include <maya/MPxContext.h>
#include "../ToolShapes/ToolShape.hpp"

namespace Stubble
{

namespace Toolbox
{

class GenericTool
	: public MPxContext
{
public:
    
	GenericTool();
	
	///----------------------------------------------------------------------------------------------------
	/// The method which returns the current 3D view.
	/// \return	The current 3D view. 
	///----------------------------------------------------------------------------------------------------
	M3dView* getActiveView();

	///----------------------------------------------------------------------------------------------------
	/// Draws the current tool shape on the screen by delegating the request to its ToolShape object.
	///----------------------------------------------------------------------------------------------------
	void drawToolShape( short aScreenCoords[ 2 ], QEvent::Type aEventType );
		
	///----------------------------------------------------------------------------------------------------
	/// Changes the current tool shape. Called every time the user changes
	/// his preference in the user interface. 
	///----------------------------------------------------------------------------------------------------
	virtual void changeToolShape() = 0; 

	int getCircleRadius();

protected:

	ToolShape *mShape; ///< Current brush shape (circle, texture shape...).

	M3dView mView; ///< The view in which we are currently operating.

	int mCircleRadius; ///< The radius of the visual aid
};

} // namespace Toolbox

} // namespace Stubble

#endif  // STUBBLE_GENERIC_TOOL_HPP