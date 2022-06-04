#version 460 core
layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D screen;



struct Ray
{
    vec3 m_origin;
    vec3 m_direction;
};
vec3 at(Ray ray, float t)
{
    return ray.m_origin + t * ray.m_direction;
}

bool hitSphere(const vec3 center, float radius, const Ray r)
{
    vec3 oc = r.m_origin - center;
    float a = dot(r.m_direction, r.m_direction);
    float b = 2.0 * dot(oc, r.m_direction);
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4.0 * a * c;
    return discriminant > 0;
}


vec4 ray_color(const Ray r)
{
    if(hitSphere(vec3(0, 0, -1), 0.5, r))
    {
        return vec4(1, 0, 0, 1);
    }
    vec3 unit_direction = normalize(r.m_direction);
    float t = 0.5 * (unit_direction.y + 1.0);
    return (1.0 - t) * vec4(1.0, 1.0, 1.0, 1.0) + t * vec4(0.5, 0.7, 1.0, 1.0);
}


void main()
{
    ivec2 screen_size = imageSize(screen);
    ivec2 screen_pos = ivec2(gl_GlobalInvocationID.xy);

    // camera 
    float viewport_height = 2.0;
    float viewport_width = viewport_height * screen_size.x / screen_size.y;
    float focal_length = 1.0;

    vec3 origin = vec3(0, 0, 0);
    vec3 horizontal = vec3(viewport_width, 0.0, 0.0);
    vec3 vertical = vec3(0.0, viewport_height, 0.0);
    vec3 lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);


    // render
    float u = float(screen_pos.x) / float(screen_size.x - 1);
    float v = float(screen_pos.y) / float(screen_size.y - 1);
    Ray r = Ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
    vec4 pixel_color = ray_color(r);
    

    imageStore(screen, screen_pos, pixel_color);
}
