/////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) Statoil ASA
//  Copyright (C) Ceetron Solutions AS
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

#include "RivTernaryTextureCoordsCreator.h"

#include "RigCaseCellResultsData.h"
#include "RigCaseData.h"
#include "RigPipeInCellEvaluator.h"
#include "RigResultAccessorFactory.h"
#include "RigTernaryResultAccessor2d.h"

#include "RimEclipseCase.h"
#include "RimEclipseView.h"
#include "RimEclipseCellColors.h"
#include "RimTernaryLegendConfig.h"
#include "RimEclipseWellCollection.h"

#include "RivTernaryResultToTextureMapper.h"
#include "RivTernaryScalarMapper.h"

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RivTernaryTextureCoordsCreator::RivTernaryTextureCoordsCreator(
	RimEclipseCellColors* cellResultColors, 
	RimTernaryLegendConfig* ternaryLegendConfig,
	size_t timeStepIndex, 
	size_t gridIndex, 
	const cvf::StructGridQuadToCellFaceMapper* quadMapper)
{
    RigCaseData* eclipseCase = cellResultColors->reservoirView()->eclipseCase()->reservoirData();

    m_quadMapper = quadMapper;
    CVF_ASSERT(quadMapper && eclipseCase );

    size_t resTimeStepIdx = timeStepIndex;

    if (cellResultColors->hasStaticResult()) resTimeStepIdx = 0;

    RifReaderInterface::PorosityModelResultType porosityModel = RigCaseCellResultsData::convertFromProjectModelPorosityModel(cellResultColors->porosityModel());

	cvf::ref<RigResultAccessor> soil = RigResultAccessorFactory::createResultAccessor(eclipseCase, gridIndex, porosityModel, resTimeStepIdx, "SOIL");
	cvf::ref<RigResultAccessor> sgas = RigResultAccessorFactory::createResultAccessor(eclipseCase, gridIndex, porosityModel, resTimeStepIdx, "SGAS");
	cvf::ref<RigResultAccessor> swat = RigResultAccessorFactory::createResultAccessor(eclipseCase, gridIndex, porosityModel, resTimeStepIdx, "SWAT");
	
	m_resultAccessor = new RigTernaryResultAccessor();
	m_resultAccessor->setTernaryResultAccessors(soil.p(), sgas.p(), swat.p());

    cvf::ref<RigPipeInCellEvaluator> pipeInCellEval = new RigPipeInCellEvaluator( cellResultColors->reservoirView()->wellCollection()->isWellPipesVisible(timeStepIndex),
        eclipseCase->gridCellToWellIndex(gridIndex));

    const RivTernaryScalarMapper* mapper = ternaryLegendConfig->scalarMapper();

    m_texMapper = new RivTernaryResultToTextureMapper(mapper, pipeInCellEval.p());
    CVF_ASSERT(m_texMapper.notNull());
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RivTernaryTextureCoordsCreator::createTextureCoords(cvf::Vec2fArray* quadTextureCoords)
{
	createTextureCoords(quadTextureCoords, m_quadMapper.p(), m_resultAccessor.p(), m_texMapper.p());
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RivTernaryTextureCoordsCreator::createTextureCoords(
	cvf::Vec2fArray* quadTextureCoords,
	const cvf::StructGridQuadToCellFaceMapper* quadMapper,
	const RigTernaryResultAccessor* resultAccessor,
	const RivTernaryResultToTextureMapper* texMapper)
{
    CVF_ASSERT(quadTextureCoords && quadMapper && resultAccessor && texMapper);

    size_t numVertices = quadMapper->quadCount()*4;
    quadTextureCoords->resize(numVertices);
    cvf::Vec2f* rawPtr = quadTextureCoords->ptr();

	cvf::Vec2d resultValue;
    cvf::Vec2f texCoord;

#pragma omp parallel for private(texCoord, resultValue)
    for (int i = 0; i < static_cast<int>(quadMapper->quadCount()); i++)
    {
        cvf::StructGridInterface::FaceType faceId = quadMapper->cellFace(i);
        size_t cellIdx = quadMapper->cellIndex(i);

        resultValue = resultAccessor->cellFaceScalar(cellIdx, faceId);
        texCoord = texMapper->getTexCoord(resultValue.x(), resultValue.y(), cellIdx);

        size_t j;
        for (j = 0; j < 4; j++)
        {   
            rawPtr[i*4 + j] = texCoord;
        }
    }
}
