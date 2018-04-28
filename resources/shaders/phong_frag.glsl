#version 330 core
// === input ===

in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex_coord;

// === material stuff ===

uniform bool has_diffuse_map;
uniform bool has_specular_map;
uniform bool has_emission_map;
uniform sampler2D diffuse_map;
uniform sampler2D specular_map;
uniform sampler2D emission_map;
uniform float materials_shininess;

// === light stuff ===

#define MAX_DIR_LIGHTS 5
#define MAX_POINT_LIGHTS 15
#define MAX_SPOTLIGHTS 15

struct DirectionalLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;

	vec3 diffuse;
	vec3 specular;

	// for attenuation
	float attenuation_constant;
	float attenuation_linear;
	float attenuation_quadratic;
};

struct Spotlight {
	vec3 position;
	vec3 direction;
	float cutoff;
	float outer_cutoff;

	vec3 diffuse;
	vec3 specular;

	float attenuation_constant;
	float attenuation_linear;
	float attenuation_quadratic;
};

vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 view_dir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 view_dir);
vec3 calcSpotlight(Spotlight light, vec3 normal, vec3 view_dir);

uniform int nbr_dir_lights;
uniform int nbr_point_lights;
uniform int nbr_spotlights;
uniform DirectionalLight dir_lights[MAX_DIR_LIGHTS];
uniform PointLight point_lights[MAX_POINT_LIGHTS];
uniform Spotlight spotlights[MAX_SPOTLIGHTS];

// === output ===

out vec4 frag_color;

// =============

void main() {
	vec3 result = vec3(0.0);

	if (has_diffuse_map || has_specular_map) {
		// view position is always (0, 0, 0) since we're
		// doing lighting in view space.
		vec3 view_dir = normalize(-vertex_pos);
		vec3 norm = normalize(vertex_normal);

		// phase 1: directional light
		for (int i = 0, sz = min(nbr_dir_lights, MAX_DIR_LIGHTS); i < sz; ++i) {
			result += calcDirLight(dir_lights[i], norm, view_dir);
		}

		// phase 2: point lights
		for (int i = min(nbr_point_lights, MAX_POINT_LIGHTS); i-- > 0;) {
			result += calcPointLight(point_lights[i], norm, view_dir);
		}

		// phase 3: spotlight
		for (int i = min(nbr_spotlights, MAX_SPOTLIGHTS); i-- > 0;) {
			result += calcSpotlight(spotlights[i], norm, view_dir);
		}
	}

	// phase 4: emission light
	if (has_emission_map) {
		result += vec3(texture(emission_map, vertex_tex_coord));
	}

	frag_color = vec4(result, 1.0);
}

vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 view_dir) {
	vec3 light_dir = normalize(-light.direction);

	// diffuse shading
	float diff = max(dot(normal, light_dir), 0.0);

	// specular shading
	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), materials_shininess);

	// combine
	vec3 result = vec3(0.0);
	if (has_diffuse_map) {
		result += light.ambient * texture(diffuse_map, vertex_tex_coord).rgb; // ambient
		result += light.diffuse * diff * texture(diffuse_map, vertex_tex_coord).rgb; // diffuse
	}
	if (has_specular_map) {
		result += light.specular * spec * texture(specular_map, vertex_tex_coord).rgb; // specular
	}

	return result;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 view_dir) {
	vec3 light_dir = normalize(vec3(light.position) - vertex_pos);

	// diffuse shading
	float diff = max(dot(normal, light_dir), 0.0);

	// specular shading
	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), materials_shininess);

	// attenuation
	float distance = length(light.position - vertex_pos);
	float attenuation = 1.0 / (light.attenuation_constant + light.attenuation_linear * distance + light.attenuation_quadratic * distance * distance);

	// combine
	vec3 result = vec3(0.0);
	if (has_diffuse_map) {
		result += attenuation * light.diffuse * diff * texture(diffuse_map, vertex_tex_coord).rgb; // diffuse
	}
	if (has_specular_map) {
		result += attenuation * light.specular * spec * texture(specular_map, vertex_tex_coord).rgb; // specular
	}

	return result;
}

vec3 calcSpotlight(Spotlight light, vec3 normal, vec3 view_dir) {
	vec3 light_dir = normalize(vec3(light.position) - vertex_pos);

	float theta = dot(light_dir, normalize(-light.direction));
	float epsilon = light.cutoff - light.outer_cutoff;
	float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0, 1.0);

	// diffuse shading
	float diff = max(dot(normal, light_dir), 0.0);

	// specular shading
	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), materials_shininess);

	// attenuation
	float distance = length(light.position - vertex_pos);
	float attenuation = 1.0 / (light.attenuation_constant + light.attenuation_linear * distance + light.attenuation_quadratic * distance * distance);

	// combine
	vec3 result = vec3(0.0);
	if (has_diffuse_map) {
		result += attenuation * intensity * light.diffuse * diff * texture(diffuse_map, vertex_tex_coord).rgb; // diffuse
	}
	if (has_specular_map) {
		result += attenuation * intensity * light.specular * spec * texture(specular_map, vertex_tex_coord).rgb; // specular
	}

	return result;
}
