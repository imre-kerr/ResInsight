/////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2015-     Statoil ASA
//  Copyright (C) 2015-     Ceetron Solutions AS
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

#include "RimGeoMechCase.h"
#include "RimGeoMechView.h"
#include "RiaApplication.h"
#include "RiaPreferences.h"
#include "RifOdbReader.h"
#include "RigGeoMechCaseData.h"
#include <QFile>

CAF_PDM_SOURCE_INIT(RimGeoMechCase, "ResInsightGeoMechCase");
//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RimGeoMechCase::RimGeoMechCase(void)
{
    CAF_PDM_InitObject("Geomechanical Case", ":/GeoMechCase48x48.png", "", "");

    CAF_PDM_InitField(&m_caseFileName, "CaseFileName", QString(), "Case file name", "", "", "");
    m_caseFileName.setUiReadOnly(true);
    CAF_PDM_InitFieldNoDefault(&geoMechViews, "GeoMechViews", "",  "", "", "");

}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RimGeoMechCase::~RimGeoMechCase(void)
{
    geoMechViews.deleteAllChildObjects();
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RimGeoMechView* RimGeoMechCase::createAndAddReservoirView()
{
    RimGeoMechView* gmv = new RimGeoMechView();
    size_t i = geoMechViews().size();
    gmv->name = QString("View %1").arg(i + 1);
    
    gmv->setGeoMechCase(this);

    geoMechViews.push_back(gmv);
    return gmv;
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
bool RimGeoMechCase::openGeoMechCase(std::string* errorMessage)
{
    // If read already, return

    if (this->m_geoMechCaseData.notNull()) return true;


    if (!QFile::exists(m_caseFileName()))
    {
        return false;
    }

    m_geoMechCaseData = new RigGeoMechCaseData(m_caseFileName().toStdString());

    return m_geoMechCaseData->openAndReadFemParts(errorMessage);
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimGeoMechCase::updateFilePathsFromProjectPath(const QString& newProjectPath, const QString& oldProjectPath)
{
    bool foundFile = false;
    std::vector<QString> searchedPaths;

    // Update filename and folder paths when opening project from a different file location
    m_caseFileName = relocateFile(m_caseFileName(), newProjectPath, oldProjectPath, &foundFile, &searchedPaths);
    
#if 0 // Output the search path for debugging
    for (size_t i = 0; i < searchedPaths.size(); ++i)
       qDebug() << searchedPaths[i];
#endif 

}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
std::vector<RimView*> RimGeoMechCase::views()
{
    std::vector<RimView*> views;
    for (size_t vIdx = 0; vIdx < geoMechViews.size(); ++vIdx)
    {
        views.push_back(geoMechViews[vIdx]);
    }
    return views;
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimGeoMechCase::initAfterRead()
{
    size_t j;
    for (j = 0; j < geoMechViews().size(); j++)
    {
        RimGeoMechView* riv = geoMechViews()[j];
        CVF_ASSERT(riv);

        riv->setGeoMechCase(this);
    }

}
