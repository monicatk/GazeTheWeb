//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Settings State.

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "src/State/State.h"

class Settings : public State
{
public:

    // Constructor
    Settings(Master* pMaster);

    // Destructor
    virtual ~Settings();

    // #############
    // ### STATE ###
    // #############

    // Update. Returns which state should be active in next time step
    virtual StateType Update(float tpf, Input& rInput);

    // Draw
    virtual void Draw() const;

    // Activate
    virtual void Activate();

    // Deactivate
    virtual void Deactivate();

private:

    // Give listener full access
    friend class SettingsButtonListener;

    // Listener for GUI
    class SettingsButtonListener : public eyegui::ButtonListener
    {
    public:

        SettingsButtonListener(Settings* pSettings) { _pSettings = pSettings; }
        virtual void hit(eyegui::Layout* pLayout, std::string id) {}
        virtual void down(eyegui::Layout* pLayout, std::string id);
        virtual void up(eyegui::Layout* pLayout, std::string id) {}

    private:

        Settings* _pSettings;
    };

    // Instance of listener
    std::shared_ptr<SettingsButtonListener> _spSettingsButtonListener;

    // Setup
    class Setup
    {
    public:

        bool _showDescriptions = true;
        bool _showGazeVisualization = false;
    };

    // Layouts
    eyegui::Layout* _pSettingsLayout;
    eyegui::Layout* _pGeneralLayout;
    eyegui::Layout* _pInfoLayout;

    // Bool to remember whether to switch to Web in next frame
    bool _goToWeb = false;

    // Setup
    Setup _setup;
};

#endif // SETTINGS_H_
