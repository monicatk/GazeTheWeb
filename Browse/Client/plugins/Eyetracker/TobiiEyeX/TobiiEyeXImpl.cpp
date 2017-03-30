//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "plugins/Eyetracker/Interface/Eyetracker.h"
#include "plugins/Eyetracker/Common/EyetrackerData.h"
#include "plugins/Eyetracker/TobiiEyeX/TobiiEyeXSDK/include/eyex/EyeX.h"

// Global variables
TX_CONTEXTHANDLE Context = TX_EMPTY_HANDLE;
TX_HANDLE GlobalInteractorSnapshot = TX_EMPTY_HANDLE; // snapshot containing interactor
const TX_STRING InteractorId = "GazeTheWeb-Browse"; // id or interactor which covers complete screen and we just listen to gaze point behavior
TX_TICKET hConnectionStateChangedTicket = TX_INVALID_TICKET;
TX_TICKET hGazeTrackingStateChangedTicket = TX_INVALID_TICKET;
TX_TICKET hEventHandlerTicket = TX_INVALID_TICKET;
bool Tracking = true;

// Delegate used when state change received
void OnStateReceived(TX_HANDLE hStateBag)
{
	TX_BOOL success;
	TX_INTEGER gazeTracking;
	success = (txGetStateValueAsInteger(hStateBag, TX_STATEPATH_GAZETRACKING, &gazeTracking) == TX_RESULT_OK);
	if (success)
	{
		Tracking = gazeTracking == TX_GAZETRACKING_GAZETRACKED;
	}
}

// On engine state changed callback
void TX_CALLCONVENTION OnEngineStateChanged(TX_CONSTHANDLE hAsyncData, TX_USERPARAM userParam)
{
	TX_RESULT result = TX_RESULT_UNKNOWN;
	TX_HANDLE hStateBag = TX_EMPTY_HANDLE;

	if (txGetAsyncDataResultCode(hAsyncData, &result) == TX_RESULT_OK &&
		txGetAsyncDataContent(hAsyncData, &hStateBag) == TX_RESULT_OK)
	{
		OnStateReceived(hStateBag);
		txReleaseObject(&hStateBag);
	}
}

// Callback function invoked when the status of the connection to the EyeX Engine has changed
void TX_CALLCONVENTION OnEngineConnectionStateChanged(TX_CONNECTIONSTATE connectionState, TX_USERPARAM userParam)
{
	if (connectionState == TX_CONNECTIONSTATE_CONNECTED)
	{
		txGetStateAsync(Context, TX_STATEPATH_EYETRACKING, OnEngineStateChanged, NULL);
	}
}

// Handles a gaze data behavior from the Gaze Point data stream
void OnGazeDataEvent(TX_HANDLE hGazeDataBehavior)
{
	TX_GAZEPOINTDATAEVENTPARAMS eventParams;
	if (txGetGazePointDataEventParams(hGazeDataBehavior, &eventParams) == TX_RESULT_OK)
	{
		// Push back to array
		eyetracker_global::PushBackRawData(eventParams.X, eventParams.Y, true);
	}
}

// Callback function invoked when an event has been received from the EyeX Engine
void TX_CALLCONVENTION HandleEvent(TX_CONSTHANDLE hAsyncData, TX_USERPARAM userParam)
{
	TX_HANDLE hEvent = TX_EMPTY_HANDLE;
	TX_HANDLE hBehavior = TX_EMPTY_HANDLE;
	txGetAsyncDataContent(hAsyncData, &hEvent);

	// Extract behavior of event (we are only interested in raw gaze points)
	if (txGetEventBehavior(hEvent, &hBehavior, TX_BEHAVIORTYPE_GAZEPOINTDATA) == TX_RESULT_OK)
	{
		OnGazeDataEvent(hBehavior);
		txReleaseObject(&hBehavior);
	}
	txReleaseObject(&hEvent);
}

// Intialization of global interactor snapshot with unfiltered gaze
bool InitializeGlobalInteractorSnapshot(TX_CONTEXTHANDLE hContext)
{
	TX_HANDLE hInteractor = TX_EMPTY_HANDLE;
	TX_GAZEPOINTDATAPARAMS params = { TX_GAZEPOINTDATAMODE_UNFILTERED };
	bool success;
	success = txCreateGlobalInteractorSnapshot(
		hContext,
		InteractorId,
		&GlobalInteractorSnapshot,
		&hInteractor) == TX_RESULT_OK;
	success &= txCreateGazePointDataBehavior(hInteractor, &params) == TX_RESULT_OK;
	txReleaseObject(&hInteractor);
	return success;
}


bool Connect()
{
	bool success = false;

	// Check for EyeX engine
	TX_EYEXAVAILABILITY availability;
	if (txGetEyeXAvailability(&availability) == TX_RESULT_OK)
	{
		if (availability == TX_EYEXAVAILABILITY_NOTAVAILABLE)
		{
			return success;
		}
		else if (availability == TX_EYEXAVAILABILITY_NOTRUNNING)
		{
			return success;
		}
	}

	// Initialize and enable the context that is the link to the EyeX Engine
	success = txInitializeEyeX(TX_EYEXCOMPONENTOVERRIDEFLAG_NONE, NULL, NULL, NULL, NULL) == TX_RESULT_OK; // initialize library
	success &= txCreateContext(&Context, TX_FALSE) == TX_RESULT_OK; // context used over complete lifetime
	success &= InitializeGlobalInteractorSnapshot(Context); // initialize snapshot and handle it over at connection
	success &= txRegisterConnectionStateChangedHandler(Context, &hConnectionStateChangedTicket, OnEngineConnectionStateChanged, NULL) == TX_RESULT_OK; // register handler for engine connection state change
	success &= txRegisterStateChangedHandler(Context, &hGazeTrackingStateChangedTicket, TX_STATEPATH_GAZETRACKING, OnEngineStateChanged, NULL) == TX_RESULT_OK; // register handler for gaze tracking state change
	success &= txRegisterEventHandler(Context, &hEventHandlerTicket, HandleEvent, NULL) == TX_RESULT_OK; // register handler for events
	success &= txEnableConnection(Context) == TX_RESULT_OK; // do connect to EyeX engine

	// Return success
	return success;
}

bool IsTracking()
{
	return Tracking;
}

bool Disconnect()
{
	bool success;
	txUnregisterConnectionStateChangedHandler(Context, hConnectionStateChangedTicket); // unregister connection state change handler
	txUnregisterStateChangedHandler(Context, hGazeTrackingStateChangedTicket); // unregister gaze tracking state change handler
	txUnregisterEventHandler(Context, hEventHandlerTicket); // unregister event handler (which receives behavior)
	txDisableConnection(Context); // disable connection to EyeX engine
	txReleaseObject(&GlobalInteractorSnapshot); // release snapshot containing interactor
	success = txShutdownContext(Context, TX_CLEANUPTIMEOUT_DEFAULT, TX_FALSE) == TX_RESULT_OK; // shutdown context
	success &= txReleaseContext(&Context) == TX_RESULT_OK; // release context object
	success &= txUninitializeEyeX() == TX_RESULT_OK; // uninitialize library
	return success;
}

void FetchGaze(int maxSampleCount, std::vector<double>& rGazeX, std::vector<double>& rGazeY)
{
	eyetracker_global::GetKOrLessValidRawGazeEntries(maxSampleCount, rGazeX, rGazeY);
}