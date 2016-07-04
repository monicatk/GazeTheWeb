//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Tab interface for methods that may be accessed by CEFMediator.

#ifndef TABCEFINTERFACE_H_
#define TABCEFINTERFACE_H_

#include <memory>
#include <vector>
#include "submodules/glm/glm/glm.hpp"

// Forward declaration
class Texture;
class DOMNode;

class TabCEFInterface
{
public:

     // Tell CEF callback which resolution web view texture should have
    virtual void GetWebRenderResolution(int& rWidth, int& rHeight) const = 0;

    // Getter and setter for favicon URL
    virtual std::string GetFavIconURL() const = 0;
    virtual void SetFavIconURL(std::string url) = 0;

    // Setter of URL. Does not load it. Should be called by CefMediator only
    virtual void SetURL(std::string URL) = 0;

    // Setter for can go back / go forward
    virtual void SetCanGoBack(bool canGoBack) = 0;
    virtual void SetCanGoForward(bool canGoForward) = 0;

    // Receive favicon bytes as char vector ordered in RGBA
    virtual void ReceiveFaviconBytes(std::unique_ptr< std::vector<unsigned char> > upData, int width, int height) = 0;
	virtual void ResetFaviconBytes() = 0;

    // Get weak pointer to texture of web view
    virtual std::weak_ptr<Texture> GetWebViewTexture() = 0;

    // Used by DOMMapping interface
    virtual void AddDOMNode(std::shared_ptr<DOMNode> spNode) = 0;
    virtual void ClearDOMNodes() = 0;

    // Receive callbacks from CefMediator upon scrolling offset changes
    virtual void SetScrollingOffset(double x, double y) = 0;

    // Getter for URL
    virtual std::string GetURL() const = 0;

    // Getter for current zoom level of corresponding browser
    virtual double GetZoomLevel() const = 0;

    // Receive size of current page for scrolling purposes
    virtual void SetPageResolution(double width, double height) = 0;

    // Fixed elements' coordinates
    virtual void SetFixedElementsCoordinates(std::vector<glm::vec4> elements) = 0;
    virtual bool GetFixedElementsLoadedAfterScrolling() = 0;
};

#endif // TABCEFINTERFACE_H_
