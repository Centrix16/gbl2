#include <stdio.h>
#include <cglm/cglm.h>

int main(void) {
	vec4 vec_in = {1.0, 0.0, 0.0, 1.0};	
	vec4 vec_out;
	mat4 trans;

	glm_translate(trans, (vec3){1.0, 1.0, 0.0});
	glm_mat4_mulv(trans, vec_in, vec_out);
	glm_vec4_print(vec_out, stdout);
	return 0;	
}
