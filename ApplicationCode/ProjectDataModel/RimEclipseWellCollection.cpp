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

#include "RimEclipseWellCollection.h"

#include "RiaApplication.h"
#include "RiaPreferences.h"
#include "RigSingleWellResultsData.h"
#include "RimEclipseView.h"
#include "RimEclipseWell.h"
#include "RivReservoirViewPartMgr.h"


namespace caf
{
    template<>
    void RimEclipseWellCollection::WellVisibilityEnum::setUp()
    {
        addItem(RimEclipseWellCollection::PIPES_FORCE_ALL_OFF,       "FORCE_ALL_OFF",      "All Off");
        addItem(RimEclipseWellCollection::PIPES_INDIVIDUALLY,        "ALL_ON",             "Individual");
        addItem(RimEclipseWellCollection::PIPES_OPEN_IN_VISIBLE_CELLS,"OPEN_IN_VISIBLE_CELLS", "Visible cells filtered");
        addItem(RimEclipseWellCollection::PIPES_FORCE_ALL_ON,        "FORCE_ALL_ON",       "All On");
    }
}


namespace caf
{
    template<>
    void RimEclipseWellCollection::WellCellsRangeFilterEnum::setUp()
    {
        addItem(RimEclipseWellCollection::RANGE_ADD_NONE,       "FORCE_ALL_OFF",      "All Off");
        addItem(RimEclipseWellCollection::RANGE_ADD_INDIVIDUAL, "ALL_ON",             "Individually");
        addItem(RimEclipseWellCollection::RANGE_ADD_ALL,        "FORCE_ALL_ON",       "All On");
    }
}

namespace caf
{
    template<>
    void RimEclipseWellCollection::WellFenceEnum::setUp()
    {
        addItem(RimEclipseWellCollection::K_DIRECTION, "K_DIRECTION",    "K - Direction");
        addItem(RimEclipseWellCollection::J_DIRECTION, "J_DIRECTION",    "J - Direction");
        addItem(RimEclipseWellCollection::I_DIRECTION, "I_DIRECTION",    "I - Direction");
        setDefault(RimEclipseWellCollection::K_DIRECTION);
    }
}

namespace caf
{
    template<>
    void RimEclipseWellCollection::WellHeadPositionEnum::setUp()
    {
        addItem(RimEclipseWellCollection::WELLHEAD_POS_ACTIVE_CELLS_BB,    "WELLHEAD_POS_ACTIVE_CELLS_BB", "Top of active cells BB");
        addItem(RimEclipseWellCollection::WELLHEAD_POS_TOP_COLUMN,         "WELLHEAD_POS_TOP_COLUMN",      "Top of active cells IJ-column");
        setDefault(RimEclipseWellCollection::WELLHEAD_POS_TOP_COLUMN);
    }
}

CAF_PDM_SOURCE_INIT(RimEclipseWellCollection, "Wells");

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RimEclipseWellCollection::RimEclipseWellCollection()
{
    CAF_PDM_InitObject("Wells", ":/WellCollection.png", "", "");

    CAF_PDM_InitField(&isActive,              "Active",        true,   "Active", "", "", "");
    isActive.setUiHidden(true);

    CAF_PDM_InitField(&showWellHead,        "ShowWellHead",     true,   "Show well heads", "", "", "");
    CAF_PDM_InitField(&showWellLabel,       "ShowWellLabel",    true,   "Show well labels", "", "", "");
    CAF_PDM_InitField(&wellHeadScaleFactor, "WellHeadScale",    1.0,    "Well head scale", "", "", "");
    CAF_PDM_InitField(&wellHeadPosition,    "WellHeadPosition", WellHeadPositionEnum(WELLHEAD_POS_TOP_COLUMN), "Well head position",  "", "", "");
    cvf::Color3f defWellLabelColor = RiaApplication::instance()->preferences()->defaultWellLabelColor();
    CAF_PDM_InitField(&wellLabelColor,      "WellLabelColor",   defWellLabelColor, "Well label color",  "", "", "");

    CAF_PDM_InitField(&wellPipeVisibility,  "GlobalWellPipeVisibility", WellVisibilityEnum(PIPES_OPEN_IN_VISIBLE_CELLS), "Global well pipe visibility",  "", "", "");

    CAF_PDM_InitField(&pipeRadiusScaleFactor,       "WellPipeRadiusScale",    0.1,                        "Pipe radius scale", "", "", "");
    CAF_PDM_InitField(&pipeCrossSectionVertexCount, "WellPipeVertexCount", 12, "Pipe vertex count", "", "", "");
    pipeCrossSectionVertexCount.setUiHidden(true);

    CAF_PDM_InitField(&wellCellsToRangeFilterMode,  "GlobalWellCellVisibility", WellCellsRangeFilterEnum(RANGE_ADD_NONE),  "Add cells to range filter", "", "", "");
    CAF_PDM_InitField(&showWellCellFences,  "ShowWellFences",           false,                              "Use well fence", "", "", "");
    CAF_PDM_InitField(&wellCellFenceType,   "DefaultWellFenceDirection", WellFenceEnum(K_DIRECTION),        "Well Fence direction", "", "", "");

    CAF_PDM_InitField(&wellCellTransparencyLevel, "WellCellTransparency", 0.5, "Well cell transparency", "", "", "");

    CAF_PDM_InitField(&isAutoDetectingBranches, "IsAutoDetectingBranches", true, "Geometry based branch detection", "", "Toggle wether the well pipe visualization will try to detect when a part of the well \nis really a branch, and thus is starting from wellhead", "");

    CAF_PDM_InitFieldNoDefault(&wells, "Wells", "Wells",  "", "", "");

    m_reservoirView = NULL;
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RimEclipseWellCollection::~RimEclipseWellCollection()
{
   wells.deleteAllChildObjects();
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RimEclipseWell* RimEclipseWellCollection::findWell(QString name)
{
    for (size_t i = 0; i < this->wells().size(); ++i)
    {
        if (this->wells()[i]->name() == name)
        {
            return this->wells()[i];
        }
    }
    return NULL;
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
bool RimEclipseWellCollection::hasVisibleWellCells()
{
    if (!this->isActive()) return false;
    if (this->wellCellsToRangeFilterMode() == RANGE_ADD_NONE) return false;
    if (this->wells().size() == 0 ) return false;

    bool hasCells = false;
    for (size_t i = 0 ; !hasCells && i < this->wells().size(); ++i)
    {
        RimEclipseWell* well = this->wells()[i];
        if ( well && well->wellResults() && ((well->showWell() && well->showWellCells()) || this->wellCellsToRangeFilterMode() == RANGE_ADD_ALL) )
        {
            for (size_t tIdx = 0; !hasCells &&  tIdx < well->wellResults()->m_wellCellsTimeSteps.size(); ++tIdx )
            {
                const RigWellResultFrame& wellResultFrame = well->wellResults()->m_wellCellsTimeSteps[tIdx];
                for (size_t wsIdx = 0; !hasCells &&  wsIdx < wellResultFrame.m_wellResultBranches.size(); ++wsIdx)
                {
                    if (wellResultFrame.m_wellResultBranches[wsIdx].m_branchResultPoints.size() > 0  ) hasCells = true; 
                }
            }
        }
    }

    if (!hasCells) return false;

    if (this->wellCellsToRangeFilterMode() == RANGE_ADD_INDIVIDUAL || this->wellCellsToRangeFilterMode() == RANGE_ADD_ALL) return true;

    // Todo: Handle range filter intersection

    return true;
}

//--------------------------------------------------------------------------------------------------
/// Used to know if we need animation of timesteps due to the wells
//--------------------------------------------------------------------------------------------------
bool RimEclipseWellCollection::hasVisibleWellPipes()
{
    if (!this->isActive()) return false;
    if (this->wellPipeVisibility() == PIPES_FORCE_ALL_OFF) return false;
    if (this->wells().size() == 0 ) return false;
    if (this->wellPipeVisibility() == PIPES_FORCE_ALL_ON) return true;

    return true;
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimEclipseWellCollection::fieldChangedByUi(const caf::PdmFieldHandle* changedField, const QVariant& oldValue, const QVariant& newValue)
{
    if (&showWellLabel == changedField || &isActive == changedField)
    {
        this->updateUiIconFromToggleField();

        if (m_reservoirView) 
        {
            m_reservoirView->scheduleGeometryRegen(VISIBLE_WELL_CELLS);
            m_reservoirView->scheduleCreateDisplayModelAndRedraw();
        }
    }
    if (&wellCellsToRangeFilterMode == changedField)
    {
        if (m_reservoirView) 
        {
            m_reservoirView->scheduleGeometryRegen(VISIBLE_WELL_CELLS);
            m_reservoirView->scheduleCreateDisplayModelAndRedraw();
        }
    }
    else if (&showWellCellFences == changedField)
    {
        if (m_reservoirView) 
        {   
            m_reservoirView->scheduleGeometryRegen(VISIBLE_WELL_CELLS);
            m_reservoirView->scheduleCreateDisplayModelAndRedraw();
        }
    }
    else if (&wellCellTransparencyLevel == changedField)
    {
        if (m_reservoirView) 
        {   
            m_reservoirView->scheduleCreateDisplayModelAndRedraw();
        }
    }
    else if (&wellCellFenceType == changedField)
    {
        if (m_reservoirView) 
        {   
            m_reservoirView->scheduleGeometryRegen(VISIBLE_WELL_CELLS);
            m_reservoirView->scheduleCreateDisplayModelAndRedraw();
        }
    }
    else if (&wellPipeVisibility == changedField)
    {
        if (m_reservoirView) 
        {   
            m_reservoirView->scheduleCreateDisplayModelAndRedraw();
        }
    }
    else if (  &pipeCrossSectionVertexCount == changedField 
            || &pipeRadiusScaleFactor == changedField 
            || &wellHeadScaleFactor == changedField 
            || &showWellHead == changedField
            || &isAutoDetectingBranches == changedField
            || &wellHeadPosition == changedField
            || &wellLabelColor == changedField)
    {
        if (m_reservoirView) 
        {
            m_reservoirView->schedulePipeGeometryRegen();
            m_reservoirView->scheduleCreateDisplayModelAndRedraw();
        }
    }
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimEclipseWellCollection::setReservoirView(RimEclipseView* ownerReservoirView)
{
    m_reservoirView = ownerReservoirView;
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimEclipseWellCollection::defineUiOrdering(QString uiConfigName, caf::PdmUiOrdering& uiOrdering)
{
    caf::PdmUiGroup* filterGroup = uiOrdering.addNewGroup("Well range filter");
    filterGroup->add(&wellCellsToRangeFilterMode);
    filterGroup->add(&showWellCellFences);
    filterGroup->add(&wellCellFenceType);

    caf::PdmUiGroup* wellHeadGroup = uiOrdering.addNewGroup("Well head");
    wellHeadGroup->add(&showWellHead);
    wellHeadGroup->add(&wellHeadScaleFactor);
    wellHeadGroup->add(&showWellLabel);
    wellHeadGroup->add(&wellHeadPosition);
    wellHeadGroup->add(&wellLabelColor);

    caf::PdmUiGroup* wellPipe = uiOrdering.addNewGroup("Well pipe");
    wellPipe->add(&wellPipeVisibility);
    wellPipe->add(&pipeRadiusScaleFactor);

    caf::PdmUiGroup* advancedGroup = uiOrdering.addNewGroup("Advanced");
    advancedGroup->add(&wellCellTransparencyLevel);
    advancedGroup->add(&isAutoDetectingBranches);
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
caf::PdmFieldHandle* RimEclipseWellCollection::objectToggleField()
{
    return &isActive;
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
const std::vector<cvf::ubyte>& RimEclipseWellCollection::isWellPipesVisible(size_t frameIndex)
{
    calculateIsWellPipesVisible(frameIndex);
    return m_isWellPipesVisible[frameIndex];
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimEclipseWellCollection::scheduleIsWellPipesVisibleRecalculation()
{
    m_isWellPipesVisible.clear();
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimEclipseWellCollection::calculateIsWellPipesVisible(size_t frameIndex)
{
    if (m_isWellPipesVisible.size() > frameIndex && m_isWellPipesVisible[frameIndex].size()) return;

    if (m_isWellPipesVisible.size() <= frameIndex)
        m_isWellPipesVisible.resize(frameIndex+1);

    if (m_isWellPipesVisible[frameIndex].size() <= wells().size())
        m_isWellPipesVisible[frameIndex].resize(wells().size(), false); 
    
    for (size_t i = 0; i < wells().size(); ++i)
    {
        m_isWellPipesVisible[frameIndex][i] = wells[i]->calculateWellPipeVisibility(frameIndex);
    }
}
