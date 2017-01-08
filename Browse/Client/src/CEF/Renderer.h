//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================

#ifndef CEF_RENDERER_H_
#define CEF_RENDERER_H_

#include "include/cef_client.h"
#include "include/cef_render_handler.h"

// Forward declaration
class Texture;
class Mediator;

// Does offscreen rendering
class Renderer : public CefRenderHandler
{
public:

    Renderer(Mediator* pMediator);

    // Tell CEF3 the size of texture to render to
    bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) OVERRIDE;

    // Called when paint happens, copy pixels over RAM to texture
    // maybe in future CEF3 versions: directly GPU mapping possible?)
    void OnPaint(
        CefRefPtr<CefBrowser> browser,
        PaintElementType type,
        const RectList &dirtyRects,
        const void *buffer,
        int width,
        int height) OVERRIDE;

    // Called when scrolling offset changes
    void OnScrollOffsetChanged(CefRefPtr< CefBrowser > browser, double x, double y) OVERRIDE;

private:

    /* MEMBERS */

    // Pointer to CefMediator in order to send rendering relevant information to Tabs etc.
    Mediator* _mediator;

    // Include CEF'S default reference counting implementation
    IMPLEMENT_REFCOUNTING(Renderer);
};

#endif // CEF_RENDERER_H_
