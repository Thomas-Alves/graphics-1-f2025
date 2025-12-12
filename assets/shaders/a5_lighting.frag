#version 430

in vec3 frag_position;
in vec3 frag_normal;
in vec2 uv;

out vec4 fragColor;

uniform vec3 u_view_pos;
uniform sampler2D u_sampler0;

// Directional light
uniform vec3 u_dir_light_direction;
uniform vec3 u_dir_light_color;

// Spotlight
uniform vec3 u_spot_position;
uniform vec3 u_spot_direction;
uniform vec3 u_spot_color;
uniform float u_spot_cutoff;

// Point light
uniform vec3 u_point_position;
uniform vec3 u_point_color;
uniform float u_point_constant;
uniform float u_point_linear;
uniform float u_point_quadratic;

vec3 CalcDirectionalLight(vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-u_dir_light_direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    vec3 ambient = 0.05 * u_dir_light_color;
    vec3 diffuse = diff * u_dir_light_color;
    vec3 specular = spec * u_dir_light_color;
    return ambient + diffuse + specular;
}

vec3 CalcSpotLight(vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(u_spot_position - fragPos);
    float theta = dot(lightDir, normalize(-u_spot_direction));
    if (theta < u_spot_cutoff) return vec3(0.0);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    vec3 ambient = 0.05 * u_spot_color;
    vec3 diffuse = diff * u_spot_color;
    vec3 specular = spec * u_spot_color;
    return ambient + diffuse + specular;
}

vec3 CalcPointLight(vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(u_point_position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    float distance = length(u_point_position - fragPos);
    float attenuation = 1.0 / (u_point_constant + u_point_linear * distance + u_point_quadratic * (distance * distance));

    vec3 ambient = 0.05 * u_point_color * attenuation;
    vec3 diffuse = diff * u_point_color * attenuation;
    vec3 specular = spec * u_point_color * attenuation;
    return ambient + diffuse + specular;
}

void main()
{
    vec3 normal = normalize(frag_normal);
    vec3 viewDir = normalize(u_view_pos - frag_position);

    vec3 tex_col = texture(u_sampler0, uv).rgb;

    vec3 result = vec3(0.0);
    result += CalcDirectionalLight(normal, viewDir);
    result += CalcSpotLight(normal, frag_position, viewDir);
    result += CalcPointLight(normal, frag_position, viewDir);

    result *= tex_col;

    fragColor = vec4(result, 1.0);
}

