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

#pragma once

#include "cafPdmField.h"
#include "cafPdmObject.h"
#include "cafPdmPointer.h"
#include "cafAppEnum.h"

// Include to make Pdm work for cvf::Color
#include "cafPdmFieldCvfColor.h"    

#include <QString>

class RimEclipseView;
class RimEclipseWell;

//==================================================================================================
///  
///  
//==================================================================================================
class RimEclipseWellCollection : public caf::PdmObject
{
    CAF_PDM_HEADER_INIT;
public:

    RimEclipseWellCollection();
    virtual ~RimEclipseWellCollection();

    void                                setReservoirView(RimEclipseView* ownerReservoirView);

    enum WellVisibilityType
    {
        PIPES_FORCE_ALL_OFF,
        PIPES_INDIVIDUALLY,
        PIPES_OPEN_IN_VISIBLE_CELLS,
        PIPES_FORCE_ALL_ON
    };
    typedef caf::AppEnum<RimEclipseWellCollection::WellVisibilityType> WellVisibilityEnum;

    enum WellCellsRangeFilterType
    {
        RANGE_ADD_ALL,
        RANGE_ADD_INDIVIDUAL,
        RANGE_ADD_NONE
    };
    typedef caf::AppEnum<RimEclipseWellCollection::WellCellsRangeFilterType> WellCellsRangeFilterEnum;

    enum WellFenceType
    {
        K_DIRECTION, 
        J_DIRECTION,
        I_DIRECTION
    };
    typedef caf::AppEnum<RimEclipseWellCollection::WellFenceType> WellFenceEnum;

    enum WellHeadPositionType
    {
        WELLHEAD_POS_ACTIVE_CELLS_BB, 
        WELLHEAD_POS_TOP_COLUMN
    };
    typedef caf::AppEnum<RimEclipseWellCollection::WellHeadPositionType> WellHeadPositionEnum;


    caf::PdmField<bool>                 showWellLabel;
    caf::PdmField<cvf::Color3f>         wellLabelColor;

    caf::PdmField<bool>                 isActive;

    caf::PdmField<WellCellsRangeFilterEnum>   wellCellsToRangeFilterMode;
    caf::PdmField<bool>                 showWellCellFences;
    caf::PdmField<WellFenceEnum>        wellCellFenceType;
    caf::PdmField<double>               wellCellTransparencyLevel;

    caf::PdmField<WellVisibilityEnum>   wellPipeVisibility;
    caf::PdmField<double>               pipeRadiusScaleFactor;
    caf::PdmField<int>                  pipeCrossSectionVertexCount;

    caf::PdmField<double>               wellHeadScaleFactor;
    caf::PdmField<bool>                 showWellHead;
    caf::PdmField<WellHeadPositionEnum> wellHeadPosition;


    caf::PdmField<bool>                 isAutoDetectingBranches;

    caf::PdmPointersField<RimEclipseWell*>     wells;

    RimEclipseWell*                            findWell(QString name);
    bool                                hasVisibleWellCells();
    bool                                hasVisibleWellPipes();

    const std::vector<cvf::ubyte>&      isWellPipesVisible(size_t frameIndex);       
    void                                scheduleIsWellPipesVisibleRecalculation();

    virtual void                        fieldChangedByUi(const caf::PdmFieldHandle* changedField, const QVariant& oldValue, const QVariant& newValue);
    virtual void                        defineUiOrdering(QString uiConfigName, caf::PdmUiOrdering& uiOrdering);
    virtual caf::PdmFieldHandle*        objectToggleField();
private:

    void                                calculateIsWellPipesVisible(size_t frameIndex);

    RimEclipseView*   m_reservoirView;
    std::vector< std::vector< cvf::ubyte > >             
                                        m_isWellPipesVisible;  
};
