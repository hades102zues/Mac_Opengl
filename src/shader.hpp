#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Shader {

public:
	unsigned int ID;

	//
	Shader(const char* vertexPath, const char* fragPath);
	void use();
	
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec3(const std::string uniformNmae, glm::vec3 color);
	void setMat4(const std::string uniformName, glm::mat4 matrix);
};

