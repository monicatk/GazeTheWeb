//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "plugins/Eyetracker/Interface/Eyetracker.h"
#include "plugins/Eyetracker/Common/EyetrackerData.h"
#include "plugins/Eyetracker/TobiiEyeX/TobiiEyeXSDK/include/eyex/EyeX.h"

// Global variables
TX_CONTEXTHANDLE Context = TX_EMPTY_HANDLE;
TX_HANDLE GlobalInteractorSnapshot = TX_EMPTY_HANDLE;
static TX_CONTEXTHANDLE TXContext = TX_EMPTY_HANDLE;
const TX_STRING InteractorId = "GazeTheWeb-Browse";

// Initializes GlobalInteractorSnapshot with an interactor that has the Gaze Point behavior
bool InitializeGlobalInteractorSnapshot(TX_CONTEXTHANDLE hContext)
{
	TX_HANDLE hInteractor = TX_EMPTY_HANDLE;
	TX_GAZEPOINTDATAPARAMS params = { TX_GAZEPOINTDATAMODE_LIGHTLYFILTERED };
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

// Callback function invoked when a snapshot has been committed
void TX_CALLCONVENTION OnSnapshotCommitted(TX_CONSTHANDLE hAsyncData, TX_USERPARAM param)
{
	TX_RESULT result = TX_RESULT_UNKNOWN;
	txGetAsyncDataResultCode(hAsyncData, &result);
}

// Delegate used when state change received
void OnStateReceived(TX_HANDLE hStateBag)
{
	TX_BOOL success;
	TX_INTEGER eyeTrackingState;

	success = (txGetStateValueAsInteger(hStateBag, TX_STATEPATH_EYETRACKINGSTATE, &eyeTrackingState) == TX_RESULT_OK);
	if (success)
	{
		switch (eyeTrackingState)
		{
		case TX_EYETRACKINGDEVICESTATUS_TRACKING:
			// TODO: tracking
			break;

		default:
			// TODO: not tracking
		}
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
	bool success = false;
	switch (connectionState)
	{
	case TX_CONNECTIONSTATE_CONNECTED:
	{
		success = txCommitSnapshotAsync(GlobalInteractorSnapshot, OnSnapshotCommitted, NULL) == TX_RESULT_OK;

		// Async callback for device status
		txGetStateAsync(TXContext, TX_STATEPATH_EYETRACKING, OnEngineStateChanged, NULL);
	}
	break;
	default:
		success = false;
		// TODO: failure
	}
}

// Handles an event from the Gaze Point data stream
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
	if (txGetEventBehavior(hEvent, &hBehavior, TX_BEHAVIORTYPE_GAZEPOINTDATA) == TX_RESULT_OK)
	{
		OnGazeDataEvent(hBehavior);
		txReleaseObject(&hBehavior);
	}
	txReleaseObject(&hEvent);
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

	// Reserve tickets
	TX_TICKET hConnectionStateChangedTicket = TX_INVALID_TICKET;
	TX_TICKET hEventHandlerTicket = TX_INVALID_TICKET;

	// Initialize and enable the context that is the link to the EyeX Engine
	success = txInitializeEyeX(TX_EYEXCOMPONENTOVERRIDEFLAG_NONE, NULL, NULL, NULL, NULL) == TX_RESULT_OK;
	success &= txCreateContext(&Context, TX_FALSE) == TX_RESULT_OK;
	success &= InitializeGlobalInteractorSnapshot(Context);
	success &= txRegisterConnectionStateChangedHandler(Context, &hConnectionStateChangedTicket, OnEngineConnectionStateChanged, NULL) == TX_RESULT_OK;
	success &= txRegisterEventHandler(Context, &hEventHandlerTicket, HandleEvent, NULL) == TX_RESULT_OK;
	success &= txEnableConnection(Context) == TX_RESULT_OK;

	// Return success
	return success;
}

bool Disconnect()
{
	bool success;
	txDisableConnection(Context);
	txReleaseObject(&GlobalInteractorSnapshot);
	success = txShutdownContext(Context, TX_CLEANUPTIMEOUT_DEFAULT, TX_FALSE) == TX_RESULT_OK;
	success &= txReleaseContext(&Context) == TX_RESULT_OK;
	success &= txUninitializeEyeX() == TX_RESULT_OK;
	return success;
}

void FetchGaze(int maxSampleCount, std::vector<double>& rGazeX, std::vector<double>& rGazeY)
{
	eyetracker_global::GetKOrLessValidRawGazeEntries(maxSampleCount, rGazeX, rGazeY);
}