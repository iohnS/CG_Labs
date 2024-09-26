#include "interpolation.hpp"

glm::vec3
interpolation::evalLERP(glm::vec3 const& p0, glm::vec3 const& p1, float const x)
{
	glm::vec3 result = (1-x)*p0 + x*p1;
	return result;
}

glm::vec3
interpolation::evalCatmullRom(glm::vec3 const& p0, glm::vec3 const& p1,
                              glm::vec3 const& p2, glm::vec3 const& p3,
                              float const t, float const x)
{	
	glm::vec4 x_vec = glm::vec4(1, x, x*x, x*x*x);

	glm::mat4 t_mat = glm::mat4(glm::vec4(0, 1, 0, 0), 
								glm::vec4(-t, 0, t, 0), 
								glm::vec4(2*t, t-3, 3-2*t, -t), 
								glm::vec4(-t, 2-t, t-2, t));
								
	glm::mat4 p_vec = glm::mat4(glm::vec4(p0, 0), glm::vec4(p1, 0), glm::vec4(p2, 0), glm::vec4(p3, 0));

	return p_vec * t_mat * x_vec;
}
