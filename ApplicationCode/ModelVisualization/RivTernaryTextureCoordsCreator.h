/////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) Statoil ASA, Ceetron Solutions AS
// 
//  ResInsight is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  ResInsight is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.
// 
//  See the GNU General Public License at <http://www.gnu.org/licenses/gpl.html> 
//  for more details.
//
/////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "RigTernaryResultAccessor2d.h"
#include "RivTernaryResultToTextureMapper.h"

#include "cvfBase.h"
#include "cvfObject.h"
#include "cvfArray.h"

class RimResultSlot;
class RimTernaryLegendConfig;

namespace cvf
{
	class StructGridQuadToCellFaceMapper;
}


//==================================================================================================
/// 
//==================================================================================================
class RivTernaryTextureCoordsCreator
{
public:
	RivTernaryTextureCoordsCreator(	RimResultSlot* cellResultSlot,
									RimTernaryLegendConfig* ternaryLegendConfig,
									size_t timeStepIndex,  
									size_t gridIndex, 
									const cvf::StructGridQuadToCellFaceMapper* quadMapper);

    void createTextureCoords(cvf::Vec2fArray* quadTextureCoords);

private:
    static void createTextureCoords(cvf::Vec2fArray* quadTextureCoords,
                                    const cvf::StructGridQuadToCellFaceMapper* quadMapper,  
									const RigTernaryResultAccessor* resultAccessor,
                                    const RivTernaryResultToTextureMapper* texMapper);

private:
    cvf::cref<cvf::StructGridQuadToCellFaceMapper>  m_quadMapper; 
	cvf::ref<RigTernaryResultAccessor>              m_resultAccessor;
    cvf::ref<RivTernaryResultToTextureMapper>       m_texMapper;
};