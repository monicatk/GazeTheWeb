// -----------------------------------------------------------------------
//
// (c) Copyright 1997-2013, SensoMotoric Instruments GmbH
// 
// Permission  is  hereby granted,  free  of  charge,  to any  person  or
// organization  obtaining  a  copy  of  the  software  and  accompanying
// documentation  covered  by  this  license  (the  "Software")  to  use,
// reproduce,  display, distribute, execute,  and transmit  the Software,
// and  to  prepare derivative  works  of  the  Software, and  to  permit
// third-parties to whom the Software  is furnished to do so, all subject
// to the following:
// 
// The  copyright notices  in  the Software  and  this entire  statement,
// including the above license  grant, this restriction and the following
// disclaimer, must be  included in all copies of  the Software, in whole
// or  in part, and  all derivative  works of  the Software,  unless such
// copies   or   derivative   works   are   solely   in   the   form   of
// machine-executable  object   code  generated  by   a  source  language
// processor.
// 
// THE  SOFTWARE IS  PROVIDED  "AS  IS", WITHOUT  WARRANTY  OF ANY  KIND,
// EXPRESS OR  IMPLIED, INCLUDING  BUT NOT LIMITED  TO THE  WARRANTIES OF
// MERCHANTABILITY,   FITNESS  FOR  A   PARTICULAR  PURPOSE,   TITLE  AND
// NON-INFRINGEMENT. IN  NO EVENT SHALL  THE COPYRIGHT HOLDERS  OR ANYONE
// DISTRIBUTING  THE  SOFTWARE  BE   LIABLE  FOR  ANY  DAMAGES  OR  OTHER
// LIABILITY, WHETHER  IN CONTRACT, TORT OR OTHERWISE,  ARISING FROM, OUT
// OF OR IN CONNECTION WITH THE  SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// -----------------------------------------------------------------------
//  Modified for "GazeTheWeb - Tweet" application (05/02/2016)

#include "src/Eyetracker/SMI.h"

#ifdef USEEYETRACKER_IVIEW
/**
* callback function being called from the eye tracker
*/
int __stdcall SampleCallbackFunction(SampleStruct sampleData)
{
    //std::cout << "From Sample Callback X: " << sampleData.leftEye.gazeX << " Y: " << sampleData.leftEye.gazeY << std::endl;
    eye_tracker_x = sampleData.leftEye.gazeX;
    eye_tracker_y = sampleData.leftEye.gazeY;

    return 1;
}

double eye_tracker_x = 0;
double eye_tracker_y = 0;

/**
* Setup connection to IView Eye Tracker
*/
void iview_setup()
{
    SystemInfoStruct systemInfoData;
    int ret_connect = 0;

    // connect to iViewX
    ret_connect = iV_Connect("127.0.0.1", 4444, "127.0.0.1", 5555);

    switch (ret_connect)
    {
    case RET_SUCCESS:
        std::cout << "Connection was established successfully" << std::endl;

        // read out meta data from iViewX
        std::cout << "GetSystemInfo: " << iV_GetSystemInfo(&systemInfoData) << std::endl;
        std::cout << "SystemInfo ETSystem: " << systemInfoData.iV_ETDevice << std::endl;
        std::cout << "SystemInfo iV_Version: " << systemInfoData.iV_MajorVersion << "." << systemInfoData.iV_MinorVersion << "." << systemInfoData.iV_Buildnumber << std::endl;
        std::cout << "SystemInfo API_Version: " << systemInfoData.API_MajorVersion << "." << systemInfoData.API_MinorVersion << "." << systemInfoData.API_Buildnumber << std::endl;
        std::cout << "SystemInfo samplerate: " << systemInfoData.samplerate << std::endl;

        break;
    case ERR_COULD_NOT_CONNECT:
        std::cout << "Connection could not be established" << std::endl;
        break;
    case ERR_WRONG_PARAMETER:
        std::cout << "Wrong Parameter used" << std::endl;
        break;
    default:
        std::cout << "Any other error appeared" << std::endl;
        return;
    }

    if (ret_connect == RET_SUCCESS)
    {
        // start data output via callback function
        // define a callback function for receiving samples
        iV_SetSampleCallback(SampleCallbackFunction);
    }
    return;
}

/**
* Close connection to IView Eye Tracker
*/
void iview_disconnect() {

    // disable callbacks
    iV_SetSampleCallback(NULL);
    iV_SetTrackingMonitorCallback(NULL);

    // disconnect
    std::cout << "iV_Disconnect: " << iV_Disconnect() << std::endl;
}
#endif
