//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Abstraction of eyetracker input. Remember to disconnect in the end.

#ifndef EYETRACKER_H_
#define EYETRACKER_H_

class Eyetracker
{
public:

    // Destructor
    virtual ~Eyetracker() = 0;

    // Connect
    bool Connect();

    // Disconnect. Returns whether successful
    bool Disconnect();

    // Update (for filtering input)
    void Update(float tpf);

    // Getter for gaze coordinates
    double GetGazeX() const;
    double GetGazeY() const;

protected:

    // Special connect
    virtual bool SpecialConnect() = 0;

    // Special disconnect. Returns whether successful
    virtual bool SpecialDisconnect() = 0;

    // Bool to check whether connected
    bool _connected = false;

    // Filtered gaze coordinates
    double _gazeX = 0;
    double _gazeY = 0;
};

#endif // EYETRACKER_H_
