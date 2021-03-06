/////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2011-     Statoil ASA
//  Copyright (C) 2013-     Ceetron Solutions AS
//  Copyright (C) 2011-2012 Ceetron AS
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

#include "RimEclipseCase.h"

#include "RiaApplication.h"
#include "RiaPreferences.h"

#include "RigCaseCellResultsData.h"
#include "RigCaseData.h"

#include "RimCaseCollection.h"
#include "RimCellEdgeColors.h"
#include "RimEclipsePropertyFilter.h"
#include "RimEclipsePropertyFilterCollection.h"
#include "RimReservoirCellResultsStorage.h"
#include "RimEclipseView.h"
#include "RimEclipseCellColors.h"

#include "cafPdmDocument.h"
#include "cafProgressInfo.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>


#include "cafPdmAbstractClassSourceInit.h"

CAF_PDM_ABSTRACT_SOURCE_INIT(RimEclipseCase, "RimReservoir"); 

//------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RimEclipseCase::RimEclipseCase()
{
    

    CAF_PDM_InitFieldNoDefault(&reservoirViews, "ReservoirViews", "",  "", "", "");

    CAF_PDM_InitFieldNoDefault(&m_matrixModelResults, "MatrixModelResults", "",  "", "", "");
    m_matrixModelResults.setUiHidden(true);
    CAF_PDM_InitFieldNoDefault(&m_fractureModelResults, "FractureModelResults", "",  "", "", "");
    m_fractureModelResults.setUiHidden(true);

    CAF_PDM_InitField(&flipXAxis, "FlipXAxis", false, "Flip X Axis", "", "", "");
    CAF_PDM_InitField(&flipYAxis, "FlipYAxis", false, "Flip Y Axis", "", "", "");

    CAF_PDM_InitFieldNoDefault(&filesContainingFaults,    "FilesContainingFaults", "", "", "", "");
    filesContainingFaults.setUiHidden(true);

    // Obsolete field
    CAF_PDM_InitField(&caseName, "CaseName",  QString(), "Obsolete", "", "" ,"");
    caseName.setIOWritable(false);
    caseName.setUiHidden(true);

    m_matrixModelResults = new RimReservoirCellResultsStorage;
    m_fractureModelResults = new RimReservoirCellResultsStorage;

    this->setReservoirData( NULL );
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RimEclipseCase::~RimEclipseCase()
{
    reservoirViews.deleteAllChildObjects();

    delete m_matrixModelResults();
    delete m_fractureModelResults();

    if (this->reservoirData())
    {
        // At this point, we assume that memory should be released
        CVF_ASSERT(this->reservoirData()->refCount() == 1);
    }
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RigCaseData* RimEclipseCase::reservoirData()
{
    return m_rigEclipseCase.p();
}

//--------------------------------------------------------------------------------------------------
///
//--------------------------------------------------------------------------------------------------
const RigCaseData* RimEclipseCase::reservoirData() const
{
    return m_rigEclipseCase.p();
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimEclipseCase::initAfterRead()
{
    size_t j;
    for (j = 0; j < reservoirViews().size(); j++)
    {
        RimEclipseView* riv = reservoirViews()[j];
        CVF_ASSERT(riv);

        riv->setEclipseCase(this);
    }

    if (caseUserDescription().isEmpty() && !caseName().isEmpty())
    {
        caseUserDescription = caseName;
    }
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RimEclipseView* RimEclipseCase::createAndAddReservoirView()
{
    RimEclipseView* riv = new RimEclipseView();
    riv->setEclipseCase(this);
    riv->cellEdgeResult()->resultVariable = "MULT";
    riv->cellEdgeResult()->enableCellEdgeColors = false;

    caf::PdmDocument::updateUiIconStateRecursively(riv);

    size_t i = reservoirViews().size();
    riv->name = QString("View %1").arg(i + 1);

    reservoirViews().push_back(riv);

    return riv;
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimEclipseCase::removeResult(const QString& resultName)
{
    size_t i;
    for (i = 0; i < reservoirViews().size(); i++)
    {
        RimEclipseView* reservoirView = reservoirViews()[i];
        CVF_ASSERT(reservoirView);

        RimEclipseCellColors* result = reservoirView->cellResult;
        CVF_ASSERT(result);

        bool rebuildDisplayModel = false;

        // Set cell result variable to none if displaying 
        if (result->resultVariable() == resultName)
        {
            result->setResultVariable(RimDefines::undefinedResultName());
            result->loadResult();

            rebuildDisplayModel = true;
        }

        std::list< caf::PdmPointer< RimEclipsePropertyFilter > >::iterator it;
        RimEclipsePropertyFilterCollection* propFilterCollection = reservoirView->propertyFilterCollection();

        for (size_t filter = 0; filter < propFilterCollection->propertyFilters().size(); filter++)
        {
            RimEclipsePropertyFilter* propertyFilter = propFilterCollection->propertyFilters()[filter];
            if (propertyFilter->resultDefinition->resultVariable() == resultName)
            {
                propertyFilter->resultDefinition->setResultVariable(RimDefines::undefinedResultName());
                propertyFilter->resultDefinition->loadResult();
                propertyFilter->setToDefaultValues();

                rebuildDisplayModel = true;
            }
        }

        if (rebuildDisplayModel)
        {
            reservoirViews()[i]->createDisplayModelAndRedraw();
        }


        // TODO
        // CellEdgeResults are not considered, as they do not support display of input properties yet
    }
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimEclipseCase::fieldChangedByUi(const caf::PdmFieldHandle* changedField, const QVariant& oldValue, const QVariant& newValue)
{
    if (changedField == &releaseResultMemory)
    {
        if (this->reservoirData())
        {
            for (size_t i = 0; i < reservoirViews().size(); i++)
            {
                RimEclipseView* reservoirView = reservoirViews()[i];
                CVF_ASSERT(reservoirView);

                RimEclipseCellColors* result = reservoirView->cellResult;
                CVF_ASSERT(result);

                result->setResultVariable(RimDefines::undefinedResultName());
                result->loadResult();

                RimCellEdgeColors* cellEdgeResult = reservoirView->cellEdgeResult;
                CVF_ASSERT(cellEdgeResult);

                cellEdgeResult->resultVariable.v() = RimDefines::undefinedResultName();
                cellEdgeResult->loadResult();

                reservoirView->createDisplayModelAndRedraw();
            }

            RigCaseCellResultsData* matrixModelResults = reservoirData()->results(RifReaderInterface::MATRIX_RESULTS);
            if (matrixModelResults)
            {
                matrixModelResults->clearAllResults();
            }

            RigCaseCellResultsData* fractureModelResults = reservoirData()->results(RifReaderInterface::FRACTURE_RESULTS);
            if (fractureModelResults)
            {
                fractureModelResults->clearAllResults();
            }
        }

        releaseResultMemory = oldValue.toBool();
    }
    else if (changedField == &flipXAxis || changedField == &flipYAxis)
    {
        RigCaseData* rigEclipseCase = reservoirData();
        if (rigEclipseCase)
        {
            rigEclipseCase->mainGrid()->setFlipAxis(flipXAxis, flipYAxis);

            computeCachedData();

            for (size_t i = 0; i < reservoirViews().size(); i++)
            {
                RimEclipseView* reservoirView = reservoirViews()[i];

                reservoirView->scheduleReservoirGridGeometryRegen();
                reservoirView->schedulePipeGeometryRegen();
                reservoirView->createDisplayModelAndRedraw();
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimEclipseCase::computeCachedData()
{
    RigCaseData* rigEclipseCase = reservoirData();
    if (rigEclipseCase)
    {
        caf::ProgressInfo pInf(30, "");
        pInf.setNextProgressIncrement(1);
        rigEclipseCase->computeActiveCellBoundingBoxes();
        pInf.incrementProgress();

        pInf.setNextProgressIncrement(10);
        rigEclipseCase->mainGrid()->computeCachedData();
        pInf.incrementProgress();

        pInf.setProgressDescription("Calculating faults");
        rigEclipseCase->mainGrid()->calculateFaults();
        pInf.incrementProgress();
    }
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RimCaseCollection* RimEclipseCase::parentCaseCollection()
{
    std::vector<RimCaseCollection*> parentObjects;
    this->parentObjectsOfType(parentObjects);

    if (parentObjects.size() > 0)
    {
        return parentObjects[0];
    }

    return NULL;
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RimIdenticalGridCaseGroup* RimEclipseCase::parentGridCaseGroup()
{
    RimCaseCollection* caseColl = parentCaseCollection();
    if (caseColl) 
    {
        return caseColl->parentCaseGroup();
    }
    else
    {
        return NULL;
    }
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimEclipseCase::setReservoirData(RigCaseData* eclipseCase)
{
    m_rigEclipseCase  = eclipseCase;
    if (this->reservoirData())
    {
        m_fractureModelResults()->setCellResults(reservoirData()->results(RifReaderInterface::FRACTURE_RESULTS));
        m_matrixModelResults()->setCellResults(reservoirData()->results(RifReaderInterface::MATRIX_RESULTS));
        m_fractureModelResults()->setMainGrid(this->reservoirData()->mainGrid());
        m_matrixModelResults()->setMainGrid(this->reservoirData()->mainGrid());
    }
    else
    {
        m_fractureModelResults()->setCellResults(NULL);
        m_matrixModelResults()->setCellResults(NULL);
        m_fractureModelResults()->setMainGrid(NULL);
        m_matrixModelResults()->setMainGrid(NULL);
    }
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RimReservoirCellResultsStorage* RimEclipseCase::results(RifReaderInterface::PorosityModelResultType porosityModel)
{
    if (porosityModel == RifReaderInterface::MATRIX_RESULTS)
    {
        return m_matrixModelResults();
    }

    return m_fractureModelResults();
}



//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
bool RimEclipseCase::openReserviorCase()
{
    // If read already, return

    if (this->reservoirData() != NULL) return true;
    
    if (!openEclipseGridFile())
    {
        return false;
    }

    {
        RimReservoirCellResultsStorage* results = this->results(RifReaderInterface::MATRIX_RESULTS);
        if (results->cellResults())
        {
            results->cellResults()->createPlaceholderResultEntries();
            // After the placeholder result for combined transmissibility is created, 
            // make sure the nnc transmissibilities can be addressed by this scalarResultIndex as well
            size_t combinedTransResIdx = results->cellResults()->findScalarResultIndex(RimDefines::STATIC_NATIVE, RimDefines::combinedTransmissibilityResultName());
            if (combinedTransResIdx != cvf::UNDEFINED_SIZE_T)
            {
                reservoirData()->mainGrid()->nncData()->setCombTransmisibilityScalarResultIndex(combinedTransResIdx);
            }
        }

    }
    {
        RimReservoirCellResultsStorage* results = this->results(RifReaderInterface::FRACTURE_RESULTS);
        if (results->cellResults()) results->cellResults()->createPlaceholderResultEntries();
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
std::vector<RimView*> RimEclipseCase::views()
{
    std::vector<RimView*> views;
    for (size_t vIdx = 0; vIdx < reservoirViews.size(); ++vIdx)
    {
        views.push_back(reservoirViews[vIdx]);
    }
    return views;
}
