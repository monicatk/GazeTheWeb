//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#ifdef TOBII_SUPPORT

#include "Tobii.h"
#include "src/Input/Eyetracker/EyetrackerGlobal.h"
#include "src/Global.h"

// Global variables
TX_CONTEXTHANDLE Context = TX_EMPTY_HANDLE;
TX_HANDLE GlobalInteractorSnapshot = TX_EMPTY_HANDLE;
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
	/*if (result == TX_RESULT_OK || result == TX_RESULT_CANCELLED)
	{
		std::cout << "Ok." << std::endl;
	}*/
}

// Callback function invoked when the status of the connection to the EyeX Engine has changed
void TX_CALLCONVENTION OnEngineConnectionStateChanged(TX_CONNECTIONSTATE connectionState, TX_USERPARAM userParam)
{
	switch (connectionState)
	{
		case TX_CONNECTIONSTATE_CONNECTED:
		{
			bool success;
			//std::cout << "Connected to EyeX Engine" << std::endl;
			success = txCommitSnapshotAsync(GlobalInteractorSnapshot, OnSnapshotCommitted, NULL) == TX_RESULT_OK;
			/*if (!success)
			{
				std::cout << "Failed to initialize the data stream." << std::endl;
			}
			else
			{
				std::cout << "Waiting for gaze data to start streaming..." << std::endl;
			}*/
		}
		break;
	case TX_CONNECTIONSTATE_DISCONNECTED:
		//std::cout << "The connection state is now DISCONNECTED (We are disconnected from the EyeX Engine)" << std::endl;
		break;
	case TX_CONNECTIONSTATE_TRYINGTOCONNECT:
		//std::cout << "The connection state is now TRYINGTOCONNECT (We are trying to connect to the EyeX Engine)" << std::endl;
		break;
	case TX_CONNECTIONSTATE_SERVERVERSIONTOOLOW:
		//std::cout << "The connection state is now SERVER_VERSION_TOO_LOW: this application requires a more recent version of the EyeX Engine to run." << std::endl;
		break;
	case TX_CONNECTIONSTATE_SERVERVERSIONTOOHIGH:
		//std::cout << "The connection state is now SERVER_VERSION_TOO_HIGH: this application requires an older version of the EyeX Engine to run." << std::endl;
		break;
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

bool Tobii::SpecialConnect()
{
	bool success = false;
	TX_TICKET hConnectionStateChangedTicket = TX_INVALID_TICKET;
	TX_TICKET hEventHandlerTicket = TX_INVALID_TICKET;

	// Initialize and enable the context that is the link to the EyeX Engine
	success = txInitializeEyeX(TX_EYEXCOMPONENTOVERRIDEFLAG_NONE, NULL, NULL, NULL, NULL) == TX_RESULT_OK;
	success &= txCreateContext(&Context, TX_FALSE) == TX_RESULT_OK;
	success &= InitializeGlobalInteractorSnapshot(Context);
	success &= txRegisterConnectionStateChangedHandler(Context, &hConnectionStateChangedTicket, OnEngineConnectionStateChanged, NULL) == TX_RESULT_OK;
	success &= txRegisterEventHandler(Context, &hEventHandlerTicket, HandleEvent, NULL) == TX_RESULT_OK;
	success &= txEnableConnection(Context) == TX_RESULT_OK;

	return success;
}

bool Tobii::SpecialDisconnect()
{
	bool success;

	// Disable and delete the context.
	txDisableConnection(Context);
	txReleaseObject(&GlobalInteractorSnapshot);
	success = txShutdownContext(Context, TX_CLEANUPTIMEOUT_DEFAULT, TX_FALSE) == TX_RESULT_OK;
	success &= txReleaseContext(&Context) == TX_RESULT_OK;
	success &= txUninitializeEyeX() == TX_RESULT_OK;
	/*if (!success)
	{
		std::cout << "EyeX could not be shut down cleanly. Did you remember to release all handles?" << std::endl;
	}
	else
	{
		printf("EyeX disconnected!\n");
	}
	*/
	return success;
}

#endif // TOBII_SUPPORT