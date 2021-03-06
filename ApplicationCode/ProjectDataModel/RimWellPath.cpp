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

#include "RimWellPath.h"

#include "RifJsonEncodeDecode.h"
#include "RimProject.h"
#include "RimTools.h"
#include "RimWellPathCollection.h"
#include "RivWellPathPartMgr.h"

#include <QDir>
#include <QFileInfo>

CAF_PDM_SOURCE_INIT(RimWellPath, "WellPath");

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RimWellPath::RimWellPath()
{
    CAF_PDM_InitObject("WellPath", ":/Well.png", "", "");

    CAF_PDM_InitFieldNoDefault(&name,               "WellPathName",                         "Name", "", "", "");
    name.setUiReadOnly(true);
    name.setIOWritable(false);
    name.setIOReadable(false);
    name.setUiHidden(true);
    CAF_PDM_InitFieldNoDefault(&id,                 "WellPathId",                           "Id", "", "", "");
    id.setUiReadOnly(true);
    id.setIOWritable(false);
    id.setIOReadable(false);
    CAF_PDM_InitFieldNoDefault(&sourceSystem,       "SourceSystem",                         "Source System", "", "", "");
    sourceSystem.setUiReadOnly(true);
    sourceSystem.setIOWritable(false);
    sourceSystem.setIOReadable(false);
    CAF_PDM_InitFieldNoDefault(&utmZone,            "UTMZone",                              "UTM Zone", "", "", "");
    utmZone.setUiReadOnly(true);
    utmZone.setIOWritable(false);
    utmZone.setIOReadable(false);
    CAF_PDM_InitFieldNoDefault(&updateDate,         "WellPathUpdateDate",                   "Update Date", "", "", "");
    updateDate.setUiReadOnly(true);
    updateDate.setIOWritable(false);
    updateDate.setIOReadable(false);
    CAF_PDM_InitFieldNoDefault(&updateUser,         "WellPathUpdateUser",                   "Update User", "", "", "");
    updateUser.setUiReadOnly(true);
    updateUser.setIOWritable(false);
    updateUser.setIOReadable(false);
    CAF_PDM_InitFieldNoDefault(&m_surveyType,       "WellPathSurveyType",                   "Survey Type", "", "", "");
    m_surveyType.setUiReadOnly(true);
    m_surveyType.setIOWritable(false);
    m_surveyType.setIOReadable(false);

    CAF_PDM_InitField(&filepath,                    "WellPathFilepath",     QString(""),    "Filepath", "", "", "");
    filepath.setUiReadOnly(true);
    CAF_PDM_InitField(&wellPathIndexInFile,         "WellPathNumberInFile",     -1,    "Well Number in file", "", "", "");
    wellPathIndexInFile.setUiReadOnly(true);

    CAF_PDM_InitField(&showWellPathLabel,           "ShowWellPathLabel",    true,           "Show well path label", "", "", "");

    CAF_PDM_InitField(&showWellPath,                "ShowWellPath",         true,           "Show well path", "", "", "");
    showWellPath.setUiHidden(true);

    CAF_PDM_InitField(&wellPathRadiusScaleFactor,   "WellPathRadiusScale", 1.0,             "Well path radius scale", "", "", "");
    CAF_PDM_InitField(&wellPathColor,               "WellPathColor",       cvf::Color3f(0.999f, 0.333f, 0.999f), "Well path color", "", "", "");
    
    m_wellPath = NULL;
    m_project = NULL;
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RimWellPath::~RimWellPath()
{
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
caf::PdmFieldHandle* RimWellPath::userDescriptionField()
{
    return &name;
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimWellPath::setSurveyType(QString surveyType) 
{ 
    m_surveyType = surveyType; 
    if (m_surveyType == "PLAN")
        wellPathColor = cvf::Color3f(0.999f, 0.333f, 0.0f);
    else if (m_surveyType == "PROTOTYPE")
        wellPathColor = cvf::Color3f(0.0f, 0.333f, 0.999f);
}


RivWellPathPartMgr* RimWellPath::partMgr()
{
    if (m_wellPathPartMgr.isNull()) 
    {
        m_wellPathPartMgr = new RivWellPathPartMgr(m_wellPathCollection, this);
    }

    return m_wellPathPartMgr.p();
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimWellPath::fieldChangedByUi(const caf::PdmFieldHandle* changedField, const QVariant& oldValue, const QVariant& newValue)
{
    partMgr()->scheduleGeometryRegen();
    if (m_project) m_project->createDisplayModelAndRedrawAllViews();
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
caf::PdmFieldHandle* RimWellPath::objectToggleField()
{
    return &showWellPath;
}


//--------------------------------------------------------------------------------------------------
/// Read JSON file containing well path data
//--------------------------------------------------------------------------------------------------
void RimWellPath::readWellPathFile()
{
    QFileInfo fi(filepath());

    if (fi.suffix().compare("json") == 0)
    {
        this->readJsonWellPathFile();
    }
    else
    {
        this->readAsciiWellPathFile();
    }

}

//--------------------------------------------------------------------------------------------------
/// Read JSON file containing well path data
//--------------------------------------------------------------------------------------------------
void RimWellPath::readJsonWellPathFile()
{
    RigWellPath* wellPathGeom = new RigWellPath();
    JsonReader jsonReader;
    QMap<QString, QVariant> jsonMap = jsonReader.decodeFile(filepath);

    // General well info

    name            = jsonMap["name"].toString();
    id              = jsonMap["id"].toString();
    sourceSystem    = jsonMap["sourceSystem"].toString();
    utmZone         = jsonMap["utmZone"].toString();
    updateUser      = jsonMap["updateUser"].toString();

    setSurveyType(jsonMap["surveyType"].toString());

    // Convert updateDate from the following format:
    // "Number of milliseconds elapsed since midnight Coordinated Universal Time (UTC) 
    // of January 1, 1970, not counting leap seconds"

    QString updateDateStr = jsonMap["updateDate"].toString().trimmed();
    uint updateDateUint = updateDateStr.toULongLong() / 1000; // should be within 32 bit, maximum number is 4294967295 which corresponds to year 2106
    QDateTime updateDateTime;
    updateDateTime.setTime_t(updateDateUint);

    updateDate = updateDateTime.toString("d MMMM yyyy");

    // Well path points

    double datumElevation = jsonMap["datumElevation"].toDouble();

    QList<QVariant> pathList = jsonMap["path"].toList();
    foreach (QVariant point, pathList)
    {
        QMap<QString, QVariant> coordinateMap = point.toMap();
        cvf::Vec3d vec3d(coordinateMap["east"].toDouble(), coordinateMap["north"].toDouble(), -(coordinateMap["tvd"].toDouble() - datumElevation));
        wellPathGeom->m_wellPathPoints.push_back(vec3d);
    }

    //jsonReader.dumpToFile(wellPathGeom->m_wellPathPoints, "c:\\temp\\jsonpoints.txt");
    setWellPathGeometry(wellPathGeom);
}

//--------------------------------------------------------------------------------------------------
///
//--------------------------------------------------------------------------------------------------
void RimWellPath::readAsciiWellPathFile()
{
    RimWellPathAsciiFileReader::WellData wpData = m_wellPathCollection->asciiFileReader()->readWellData(filepath(), wellPathIndexInFile());
    this->name = wpData.m_name;

    setWellPathGeometry(wpData.m_wellPathGeometry.p());
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimWellPath::defineUiOrdering(QString uiConfigName, caf::PdmUiOrdering& uiOrdering)
{
    caf::PdmUiGroup* appGroup =  uiOrdering.addNewGroup("Appearance");
    appGroup->add(&showWellPathLabel);
    appGroup->add(&wellPathColor);
    appGroup->add(&wellPathRadiusScaleFactor); 

    caf::PdmUiGroup* fileInfoGroup =   uiOrdering.addNewGroup("File");
    fileInfoGroup->add(&filepath);
    fileInfoGroup->add(&wellPathIndexInFile);

    caf::PdmUiGroup* ssihubGroup =  uiOrdering.addNewGroup("Well Info");
    ssihubGroup->add(&id);
    ssihubGroup->add(&sourceSystem);
    ssihubGroup->add(&utmZone);
    ssihubGroup->add(&updateDate);
    ssihubGroup->add(&updateUser);
    ssihubGroup->add(&m_surveyType);

}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
QString RimWellPath::getCacheDirectoryPath()
{
    QString cacheDirPath = RimTools::getCacheRootDirectoryPathFromProject();
    cacheDirPath += "_wellpaths";
    return cacheDirPath;
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
QString RimWellPath::getCacheFileName()
{
    QString cacheFileName;

    // Make the path correct related to the possibly new project filename
    QString newCacheDirPath = getCacheDirectoryPath();
    QFileInfo oldCacheFile(filepath);

    cacheFileName = newCacheDirPath + "/" + oldCacheFile.fileName();

    return cacheFileName;
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimWellPath::setupBeforeSave()
{
    // SSIHUB is the only source for populating Id, use text in this field to decide if the cache file must be copied to new project cache location
    if (!isStoredInCache())
    {
        return;
    }

    QDir::root().mkpath(getCacheDirectoryPath());

    QString newCacheFileName = getCacheFileName();

    // Use QFileInfo to get same string representation to avoid issues with mix of forward and backward slashes
    QFileInfo prevFileInfo(filepath);
    QFileInfo currentFileInfo(newCacheFileName);

    if (prevFileInfo.absoluteFilePath().compare(currentFileInfo.absoluteFilePath()) != 0)
    {
        QFile::copy(filepath, newCacheFileName);

        filepath = newCacheFileName;
    }
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
bool RimWellPath::isStoredInCache()
{
    // SSIHUB is the only source for populating Id, use text in this field to decide if the cache file must be copied to new project cache location
    return !id().isEmpty();
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimWellPath::updateFilePathsFromProjectPath()
{
    QString newCacheFileName = getCacheFileName();

    if (QFile::exists(newCacheFileName))
    {
        filepath = newCacheFileName;
    }
}

