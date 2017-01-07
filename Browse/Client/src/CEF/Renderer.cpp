//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================

#include "Renderer.h"
#include "src/CEF/CefMediator.h"
#include "src/Utils/Texture.h"
#include "src/Utils/Logger.h"
#include "include/wrapper/cef_helpers.h"

Renderer::Renderer(CefRefPtr<CefMediator> mediator)
{
    _mediator = mediator;
}

bool Renderer::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
{
    int width = 0, height = 0;
    _mediator->GetResolution(browser, width, height);
    rect = CefRect(0, 0, width, height);
    if (width == 0 || height == 0)
    {
        LogDebug("Renderer: GetViewRect size equal zero!");
    }
    return true;
}

void Renderer::OnPaint(
    CefRefPtr<CefBrowser> browser,
    PaintElementType type,
    const RectList &dirtyRects,
    const void *buffer,
    int width,
    int height)
{
    // Look up corresponding texture
    if (auto spTexture = _mediator->GetTexture(browser).lock())
    {
        // Fill texture with rendered website
        spTexture->Fill(width, height, GL_BGRA, (const unsigned char*)buffer);
        //std::cout << "INFO(Renderer): Page rendered and copied to texture." << std::endl;
    }
    else
    {
        LogDebug("Renderer: OnPaint couldn't fill texture...");
    }
}

void Renderer::OnScrollOffsetChanged(CefRefPtr<CefBrowser> browser, double x, double y)
{
    // Call CefMediator to set offset in corresponding Tab
    _mediator->OnScrollOffsetChanged(browser, x, y);
}
