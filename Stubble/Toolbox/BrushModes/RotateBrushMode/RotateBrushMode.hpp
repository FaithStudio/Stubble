#ifndef STUBBLE_ROTATE_BRUSH_MODE_HPP
#define STUBBLE_ROTATE_BRUSH_MODE_HPP

#include <maya/MVector.h>
#include "../BrushMode.hpp"

namespace Stubble
{

namespace Toolbox
{

///----------------------------------------------------------------------------------------------------
/// The class implementing the rotate brush transformations.
///----------------------------------------------------------------------------------------------------
class RotateBrushMode :
	public BrushMode
{
public:
	///----------------------------------------------------------------------------------------------------
	/// The method which implements the actual brush transformations.
	///----------------------------------------------------------------------------------------------------
	virtual void doBrush( MVector aDX, float aDT );
};

} // namespace Toolbox

} // namespace Stubble

#endif