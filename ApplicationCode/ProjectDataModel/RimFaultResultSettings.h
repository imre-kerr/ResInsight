/////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2011-2012 Statoil ASA, Ceetron AS
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

#include "cafAppEnum.h"
#include "cafPdmField.h"
#include "cafPdmObject.h"

class RimResultSlot;
class RimReservoirView;

//==================================================================================================
///  
///  
//==================================================================================================
class RimFaultResultSettings : public caf::PdmObject
{
    CAF_PDM_HEADER_INIT;
public:
    enum FaultVisualizationMode
    {
        FAULT_COLOR,
        CUSTOM_RESULT_MAPPING
    };

public:
    RimFaultResultSettings();
    virtual ~RimFaultResultSettings();
    
    void setReservoirView(RimReservoirView* ownerReservoirView);

    caf::PdmField< caf::AppEnum< FaultVisualizationMode > >     visualizationMode;
    caf::PdmField<bool>     showNNCs;
    caf::PdmField<bool>     showCustomFaultResult;


    RimResultSlot*  customFaultResult();
    void            updateVisibility();

protected:
    virtual void                    initAfterRead();
    virtual caf::PdmFieldHandle*    objectToggleField();
    virtual void                    fieldChangedByUi(const caf::PdmFieldHandle* changedField, const QVariant& oldValue, const QVariant& newValue);

private:
    caf::PdmField<RimResultSlot*>       m_customFaultResult;
    caf::PdmPointer<RimReservoirView>   m_reservoirView;
};
