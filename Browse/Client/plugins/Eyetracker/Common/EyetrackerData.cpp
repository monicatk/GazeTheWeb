//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "EyetrackerData.h"
#include <algorithm>

namespace eyetracker_global
{
    // Global variables for raw gaze data are defined here. Will exist within this translation unit
    double EyetrackerRawGazeX[EYETRACKER_SAMPLE_COLLECTION_COUNT];
    double EyetrackerRawGazeY[EYETRACKER_SAMPLE_COLLECTION_COUNT];
    bool EyetrackerRawGazeValidity[EYETRACKER_SAMPLE_COLLECTION_COUNT];
    int EyetrackerRawGazeIndex = 0;
    int RawGazeEntries = 0; // count of filled raw data fields in array (converges to EYETRACKER_SAMPLE_COLLECTION_COUNT - 1)

    void PushBackRawData(double gazeX, double gazeY, bool valid)
    {
        EyetrackerRawGazeIndex++;
        EyetrackerRawGazeIndex = EyetrackerRawGazeIndex >= EYETRACKER_SAMPLE_COLLECTION_COUNT ? 0 : EyetrackerRawGazeIndex;
        EyetrackerRawGazeX[EyetrackerRawGazeIndex] = gazeX;
        EyetrackerRawGazeY[EyetrackerRawGazeIndex] = gazeY;
        EyetrackerRawGazeValidity[EyetrackerRawGazeIndex] = valid;
        RawGazeEntries++;
        RawGazeEntries = std::min(RawGazeEntries, EYETRACKER_SAMPLE_COLLECTION_COUNT - 1);
    }

    void GetKOrLessValidRawGazeEntries(int k, std::vector<double>& rGazeX, std::vector<double>& rGazeY)
    {
        // Reset output vectors
        rGazeX.clear();
        rGazeY.clear();

        // Calculate actual k
        int clampedK = std::min(k, RawGazeEntries);

        // Go over last k entries and collect valid ones
        for (int i = 0; i < clampedK; i++)
        {
            int index = EyetrackerRawGazeIndex - i;
            if (index < 0)
            {
                // If index less than zero, start at end again
                index += EYETRACKER_SAMPLE_COLLECTION_COUNT;
            }

            // Only add when valid
            if (EyetrackerRawGazeValidity[index])
            {
                rGazeX.push_back(EyetrackerRawGazeX[index]);
                rGazeY.push_back(EyetrackerRawGazeY[index]);
            }
        }
    }
}
