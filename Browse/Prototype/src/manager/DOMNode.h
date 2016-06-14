//============================================================================
// Distributed under the MIT License.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================

#ifndef DOMNODE_H_
#define DOMNODE_H_

#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "src/events/Event.h"

class DOMNode{

public:
    std::pair<glm::vec2, glm::vec2>* getCoordinates() { return &m_coordinates; };

protected:
    std::pair<glm::vec2, glm::vec2> m_coordinates;	// top-left & bottom right vertices
    std::string m_id;
};


/* LIST OF ALL DOM NODE TYPES*/

class TextInputField : private DOMNode{
public:
    TextInputField(std::string id, std::pair<glm::vec2, glm::vec2> coordinates);

    std::string getValue() { return m_value; }

private:
    std::string m_value = "";
};


#endif  // DOMNODE_H_


