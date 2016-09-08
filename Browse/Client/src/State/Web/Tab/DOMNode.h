//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================
// Class for DOM nodes which are used by Tab.

#ifndef DOMNODE_H_
#define DOMNODE_H_

#include "src/Typedefs.h"
#include "src/Utils/Rect.h"
#include "src/Utils/glmWrapper.h"
#include "src/State/Web/Tab/DOMNodeType.h"
#include <vector>
#include <string>
#include <memory>

class DOMNode
{
    public:
        DOMNodeType GetType() const { return _type; }
        int64 GetFrameID() const { return _frameID; }
        int GetNodeID() const { return _nodeID; };
        std::vector<Rect> GetRects() const { return _rects; };
		std::vector<glm::vec2> GetCenters() const;
		glm::vec2 GetCenter() const;
		bool GetFixed() const { return _fixed; }
		bool GetVisibility() const { return _visible; }

		void AddRect(Rect rect) { _rects.push_back(rect); }

		void SetRects(std::shared_ptr<std::vector<Rect>> rects);
		void SetFixed(bool fixed) { _fixed = fixed; }
		void SetVisibility(bool visible) { _visible = visible; }



		DOMNode(DOMNodeType type, int64 frameID, int nodeID, std::vector<Rect> rects)
		{
			_type = type;
			_frameID = frameID;
			_nodeID = nodeID;
			_rects = rects;
		}

		// Constructor for nodes with single Rect
		DOMNode(DOMNodeType type, int64 frameID, int nodeID, Rect rect)
		{
			_type = type;
			_frameID = frameID;
			_nodeID = nodeID;
			_rects = { rect };
		}

    protected:

        DOMNodeType _type;
        int64 _frameID;
        int _nodeID;			// Node's position in Javascript's list of nodes of the same type
        std::vector<Rect> _rects;				
		bool _fixed = false;
		bool _visible = true;


};

/* LIST OF ALL DOM NODE TYPES */

class DOMTextInput : public DOMNode
{
    public:
        DOMTextInput(	DOMNodeType type,
                        int64 frameID,
                        int nodeID,
                        Rect rect,
                        std::string value		);

        std::string GetValue() const { return _value; }

    private:

        std::string _value = "";
};



class DOMLink : public DOMNode
{
    public:
        DOMLink(	
			DOMNodeType type,
            int64 frameID,
            int nodeID,
			Rect rect,
			std::string text,
            std::string url			
		);

		DOMLink(
			DOMNodeType type,
			int64 frameID,
			int nodeID,
			std::vector<Rect> rects,
			std::string text,
			std::string url
		);

        std::string GetText() const { return _text; }
        std::string GetURL() const { return _url; }

    private:
        std::string _text;
        std::string _url;
};


#endif  // DOMNODE_H_


