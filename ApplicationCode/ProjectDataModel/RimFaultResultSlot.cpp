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

#include "RimFaultResultSlot.h"

#include "RimReservoirView.h"
#include "RimResultSlot.h"
#include "RiuMainWindow.h"
#include "RimUiTreeModelPdm.h"


namespace caf
{
    template<>
    void AppEnum< RimFaultResultSlot::FaultVisualizationMode >::setUp()
    {
        addItem(RimFaultResultSlot::FAULT_COLOR,            "FAULT_COLOR",              "Fault Colors");
        addItem(RimFaultResultSlot::CUSTOM_RESULT_MAPPING,  "CUSTOM_RESULT_MAPPING",    "Custom Cell Results");
        setDefault(RimFaultResultSlot::FAULT_COLOR);
    }
}

CAF_PDM_SOURCE_INIT(RimFaultResultSlot, "RimFaultResultSlot");

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RimFaultResultSlot::RimFaultResultSlot()
{
    CAF_PDM_InitObject("Fault Result Slot", ":/draw_style_faults_24x24.png", "", "");

    CAF_PDM_InitField(&showCustomFaultResult,                "ShowCustomFaultResult",                 false,   "Show Custom Fault Result", "", "", "");
    showCustomFaultResult.setUiHidden(true);

    CAF_PDM_InitField(&visualizationMode, "VisualizationMode", caf::AppEnum<RimFaultResultSlot::FaultVisualizationMode>(RimFaultResultSlot::FAULT_COLOR), "Fault Color Mapping", "", "", "");
    CAF_PDM_InitField(&showNNCs,                "ShowNNCs",                 false,   "Show NNCs", "", "", "");

    CAF_PDM_InitFieldNoDefault(&m_customFaultResult, "CustomResultSlot", "Custom Fault Result", ":/CellResult.png", "", "");
    m_customFaultResult = new RimResultSlot();
    m_customFaultResult.setOwnerObject(this);
    m_customFaultResult.setUiHidden(true);

    // Take ownership of the fields in RimResultDefinition to be able to trap fieldChangedByUi in this class
    m_customFaultResult->m_resultTypeUiField.setOwnerObject(this);
    m_customFaultResult->m_porosityModelUiField.setOwnerObject(this);
    m_customFaultResult->m_resultVariableUiField.setOwnerObject(this);

    updateFieldVisibility();
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RimFaultResultSlot::~RimFaultResultSlot()
{
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimFaultResultSlot::setReservoirView(RimReservoirView* ownerReservoirView)
{
    m_reservoirView = ownerReservoirView;
    m_customFaultResult->setReservoirView(ownerReservoirView);
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimFaultResultSlot::fieldChangedByUi(const caf::PdmFieldHandle* changedField, const QVariant& oldValue, const QVariant& newValue)
{
    this->updateUiIconFromToggleField();

    m_customFaultResult->fieldChangedByUi(changedField, oldValue, newValue);

    if (changedField == &visualizationMode)
    {
        updateFieldVisibility();

        RiuMainWindow::instance()->uiPdmModel()->updateUiSubTree(this);
    }

    if (changedField == &m_customFaultResult->m_resultVariableUiField)
    {
        RiuMainWindow::instance()->uiPdmModel()->updateUiSubTree(this);
    }

    if (m_reservoirView) m_reservoirView->scheduleCreateDisplayModelAndRedraw();
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimFaultResultSlot::initAfterRead()
{
    m_customFaultResult->initAfterRead();
    updateFieldVisibility();

    this->updateUiIconFromToggleField();
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimFaultResultSlot::updateFieldVisibility()
{
    m_customFaultResult->updateFieldVisibility();
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RimResultSlot* RimFaultResultSlot::customFaultResult()
{
    if (showCustomFaultResult() && this->visualizationMode() == CUSTOM_RESULT_MAPPING)
    {
        return this->m_customFaultResult();
    }

    return NULL;
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
caf::PdmFieldHandle* RimFaultResultSlot::objectToggleField()
{
    return &showCustomFaultResult;
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimFaultResultSlot::defineUiOrdering(QString uiConfigName, caf::PdmUiOrdering& uiOrdering)
{
    uiOrdering.add(&visualizationMode);
    uiOrdering.add(&showNNCs);

    if (visualizationMode == CUSTOM_RESULT_MAPPING)
    {
        caf::PdmUiGroup* group1 = uiOrdering.addNewGroup("Result");
        group1->add(&(m_customFaultResult->m_resultTypeUiField));
        group1->add(&(m_customFaultResult->m_porosityModelUiField));
        group1->add(&(m_customFaultResult->m_resultVariableUiField));
    }
}   

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
QList<caf::PdmOptionItemInfo> RimFaultResultSlot::calculateValueOptions(const caf::PdmFieldHandle* fieldNeedingOptions, bool * useOptionsOnly)
{
    return m_customFaultResult->calculateValueOptions(fieldNeedingOptions, useOptionsOnly);
}
