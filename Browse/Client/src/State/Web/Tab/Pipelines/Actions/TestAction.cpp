//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "TestAction.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"

TestAction::TestAction(TabInteractionInterface *pTab) : Action(pTab)
{
    _dim.setValue(0);
}

bool TestAction::Update(float tpf, TabInput tabInput)
{
    bool finished = _dim.update(0.25f * tpf) >= 1;
    WebViewParameters webViewParameters;
    webViewParameters.dim = _dim.getValue();
    _pTab->SetWebViewParameters(webViewParameters);
    return finished;
}

void TestAction::Draw() const
{

}

void TestAction::Activate()
{

}

void TestAction::Deactivate()
{

}

void TestAction::Abort()
{

}
