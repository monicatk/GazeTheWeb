/**
* This is an example that demonstrates how to connect to the EyeX Engine and subscribe to the lightly filtered gaze data stream.
*
* Copyright 2013-2014 Tobii Technology AB. All rights reserved.
*
* Modified for "GazeTheWeb - Tweet" application (05/02/2016)
*/

#ifdef USEEYETRACKER_TOBII

#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <assert.h>
#include "eyex/EyeX.h"

#pragma comment (lib, "Tobii.EyeX.Client.lib")

extern double eye_tracker_x, eye_tracker_y;

/**
* Initializes g_hGlobalInteractorSnapshot with an interactor that has the Gaze Point behavior.
*/
BOOL InitializeGlobalInteractorSnapshot(TX_CONTEXTHANDLE hContext);

/**
* Callback function invoked when a snapshot has been committed.
*/
void TX_CALLCONVENTION OnSnapshotCommitted(TX_CONSTHANDLE hAsyncData, TX_USERPARAM param);

/**
* Callback function invoked when the status of the connection to the EyeX Engine has changed.
*/
void TX_CALLCONVENTION OnEngineConnectionStateChanged(TX_CONNECTIONSTATE connectionState, TX_USERPARAM userParam);

/**
* Handles an event from the Gaze Point data stream.
*/
void OnGazeDataEvent(TX_HANDLE hGazeDataBehavior);

/**
* Callback function invoked when an event has been received from the EyeX Engine.
*/
void TX_CALLCONVENTION HandleEvent(TX_CONSTHANDLE hAsyncData, TX_USERPARAM userParam);

/**
* Setup and initialize connection to Tobii Eye Tracker
*/
void tobii_setup();

/**
* Close connection to Tobii Eye Tracker
*/
void tobii_disconnect();

#endif
