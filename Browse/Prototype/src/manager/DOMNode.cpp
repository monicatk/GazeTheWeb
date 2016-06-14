//============================================================================
// Distributed under the MIT License.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================

#include "DOMNode.h"

TextInputField::TextInputField(std::string id, std::pair<glm::vec2, glm::vec2> coordinates)
{
	m_id = id;
	m_coordinates = coordinates;
}