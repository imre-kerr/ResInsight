
# Use this workaround until we're on 2.8.3 on all platforms and can use CMAKE_CURRENT_LIST_DIR directly 
if (${CMAKE_VERSION} VERSION_GREATER "2.8.2")
    set(CEE_CURRENT_LIST_DIR  ${CMAKE_CURRENT_LIST_DIR}/)
endif()

set (SOURCE_GROUP_HEADER_FILES
${CEE_CURRENT_LIST_DIR}RigActiveCellInfo.h
${CEE_CURRENT_LIST_DIR}RigCell.h
${CEE_CURRENT_LIST_DIR}RigCaseData.h
${CEE_CURRENT_LIST_DIR}RigGridBase.h
${CEE_CURRENT_LIST_DIR}RigGridManager.h
${CEE_CURRENT_LIST_DIR}RigResultAccessor.h
${CEE_CURRENT_LIST_DIR}RigResultAccessorFactory.h
${CEE_CURRENT_LIST_DIR}RigAllGridCellsResultAccessor.h
${CEE_CURRENT_LIST_DIR}RigActiveCellsResultAccessor.h
${CEE_CURRENT_LIST_DIR}RigCellEdgeResultAccessor.h
${CEE_CURRENT_LIST_DIR}RigCombTransResultAccessor.h
${CEE_CURRENT_LIST_DIR}RigCombMultResultAccessor.h
${CEE_CURRENT_LIST_DIR}RigResultModifier.h
${CEE_CURRENT_LIST_DIR}RigResultModifierFactory.h
${CEE_CURRENT_LIST_DIR}RigLocalGrid.h
${CEE_CURRENT_LIST_DIR}RigMainGrid.h
${CEE_CURRENT_LIST_DIR}RigReservoirBuilderMock.h
${CEE_CURRENT_LIST_DIR}RigCaseCellResultsData.h
${CEE_CURRENT_LIST_DIR}RigSingleWellResultsData.h
${CEE_CURRENT_LIST_DIR}RigWellPath.h
${CEE_CURRENT_LIST_DIR}RigFault.h
${CEE_CURRENT_LIST_DIR}RigNNCData.h
${CEE_CURRENT_LIST_DIR}cvfGeometryTools.h
${CEE_CURRENT_LIST_DIR}cvfGeometryTools.inl
${CEE_CURRENT_LIST_DIR}RigPipeInCellEvaluator.h
${CEE_CURRENT_LIST_DIR}RigResultAccessor2d.h
${CEE_CURRENT_LIST_DIR}RigTernaryResultAccessor2d.h
${CEE_CURRENT_LIST_DIR}RigNativeStatCalc.h
${CEE_CURRENT_LIST_DIR}RigMultipleDatasetStatCalc.h
)

set (SOURCE_GROUP_SOURCE_FILES
${CEE_CURRENT_LIST_DIR}RigActiveCellInfo.cpp
${CEE_CURRENT_LIST_DIR}RigCell.cpp
${CEE_CURRENT_LIST_DIR}RigCaseData.cpp
${CEE_CURRENT_LIST_DIR}RigGridBase.cpp
${CEE_CURRENT_LIST_DIR}RigGridManager.cpp
${CEE_CURRENT_LIST_DIR}RigResultAccessor.cpp
${CEE_CURRENT_LIST_DIR}RigResultAccessorFactory.cpp
${CEE_CURRENT_LIST_DIR}RigAllGridCellsResultAccessor.cpp
${CEE_CURRENT_LIST_DIR}RigActiveCellsResultAccessor.cpp
${CEE_CURRENT_LIST_DIR}RigCellEdgeResultAccessor.cpp
${CEE_CURRENT_LIST_DIR}RigCombTransResultAccessor.cpp
${CEE_CURRENT_LIST_DIR}RigCombMultResultAccessor.cpp
${CEE_CURRENT_LIST_DIR}RigResultModifierFactory.cpp
${CEE_CURRENT_LIST_DIR}RigLocalGrid.cpp
${CEE_CURRENT_LIST_DIR}RigMainGrid.cpp
${CEE_CURRENT_LIST_DIR}RigReservoirBuilderMock.cpp
${CEE_CURRENT_LIST_DIR}RigCaseCellResultsData.cpp
${CEE_CURRENT_LIST_DIR}RigSingleWellResultsData.cpp
${CEE_CURRENT_LIST_DIR}RigWellPath.cpp
${CEE_CURRENT_LIST_DIR}RigFault.cpp
${CEE_CURRENT_LIST_DIR}RigNNCData.cpp
${CEE_CURRENT_LIST_DIR}cvfGeometryTools.cpp
${CEE_CURRENT_LIST_DIR}RigTernaryResultAccessor2d.cpp
${CEE_CURRENT_LIST_DIR}RigNativeStatCalc.cpp
${CEE_CURRENT_LIST_DIR}RigMultipleDatasetStatCalc.cpp
)

list(APPEND CODE_HEADER_FILES
${SOURCE_GROUP_HEADER_FILES}
)

list(APPEND CODE_SOURCE_FILES
${SOURCE_GROUP_SOURCE_FILES}
)

source_group( "ReservoirDataModel" FILES ${SOURCE_GROUP_HEADER_FILES} ${SOURCE_GROUP_SOURCE_FILES} ${CEE_CURRENT_LIST_DIR}CMakeLists_files.cmake )
