
// 54 Departure tables

// Registries
#include "DisplayType.h"
#include "DisplayScreen.h"
#include "DisplayMonitoringStatus.h"
#include "DisplayScreenCPU.h"

// Factories
#include "DeparturesTableModule.h"

#include "DeparturesTableInterfaceElement.h"
#include "DeparturesTableRoutePlanningInterfaceElement.h"
#include "DisplayScreenHasAlarmValueInterfaceElement.h"
#include "DisplayScreenAlarmContentValueInterfaceElement.h"
#include "DeparturesTableLineContentInterfaceElement.h"
#include "DeparturesTableDestinationContentInterfaceElement.h"

#include "DepartureTableRowInterfacePage.h"
#include "DeparturesTableRoutePlanningRowInterfacePage.h"
#include "DeparturesTableRoutePlanningRowKeyInterfacePage.h"
#include "ParseDisplayReturnInterfacePage.h"
#include "DeparturesTableInterfacePage.h"
#include "DeparturesTableRoutePlanningInterfacePage.h"
#include "DeparturesTableDestinationInterfacepage.h"
#include "DeparturesTableTransferDestinationInterfacePage.h"

#include "DisplaySearchAdmin.h"
#include "DisplayTypesAdmin.h"
#include "BroadcastPointsAdmin.h"
#include "DisplayAdmin.h"
#include "DisplayTypeAdmin.h"
#include "DisplayScreenCPUAdmin.h"
#include "DeparturesTableBenchmarkAdmin.h"

#include "DisplayTypeTableSync.h"
#include "DisplayScreenTableSync.h"
#include "DisplayMonitoringStatusTableSync.h"
#include "DisplayScreenCPUTableSync.h"

#include "CreateDisplayTypeAction.h"
#include "UpdateDisplayTypeAction.h"
#include "UpdateDisplayScreenAction.h"
#include "CreateDisplayScreenAction.h"
#include "DisplayScreenAppearanceUpdateAction.h"
#include "AddPreselectionPlaceToDisplayScreenAction.h"
#include "UpdateDisplayPreselectionParametersAction.h"
#include "RemovePreselectionPlaceFromDisplayScreenAction.h"
#include "UpdateAllStopsDisplayScreenAction.h"
#include "AddDepartureStopToDisplayScreenAction.h"
#include "AddForbiddenPlaceToDisplayScreenAction.h"
#include "UpdateDisplayPreselectionParametersAction.h"
#include "DisplayScreenRemovePhysicalStopAction.h"
#include "DisplayScreenRemoveDisplayedPlaceAction.h"
#include "DisplayScreenRemoveForbiddenPlaceAction.h"
#include "UpdateDisplayMaintenanceAction.h"
#include "DisplayScreenAddDisplayedPlaceAction.h"
#include "DisplayScreenCPUCreateAction.h"
#include "DisplayScreenCPUUpdateAction.h"
#include "DisplayScreenCPUMaintenanceUpdateAction.h"
#include "DisplayScreenTransferDestinationAddAction.h"
#include "DisplayScreenTransferDestinationRemoveAction.h"
#include "DisplayScreenUpdateDisplayedStopAreaAction.hpp"

#include "ArrivalDepartureTableRight.h"
#include "DisplayMaintenanceRight.h"

#include "ArrivalDepartureTableLog.h"
#include "DisplayMaintenanceLog.h"

#include "DisplayScreenAlarmRecipient.h"

#include "DisplayScreenSupervisionFunction.h"
#include "DisplayScreenContentFunction.h"
#include "AlarmTestOnDisplayScreenFunction.h"
#include "DisplayGetNagiosStatusFunction.h"
#include "CentreonConfigExportFunction.h"
#include "CPUGetWiredScreensFunction.h"
