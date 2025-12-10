#include "Window.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"

#include <imgui/imgui.h>
#include <cstddef>
#include <cstdlib>
#include <cstdio>
#include <ctime>

enum ShaderType
{
    SHADER_LIGHTING,
    SHADER_FLAT,

    SHADER_SAMPLE_TEXTURE,
    SHADER_POSITION_COLOR,
    SHADER_TCOORD_COLOR,
    SHADER_NORMAL_COLOR,
    SHADER_TYPE_COUNT
};

enum MeshType
{
    // Parametric surfaces
    MESH_PLANE,
    MESH_SPHERE,
    MESH_HEMISPHERE,

    // Obj files
    MESH_HEAD,

    MESH_TYPE_COUNT
};

enum TextureType
{
    TEXTURE_CT4,
    TEXTURE_WHITE,
    TEXTURE_GRADIENT_WARM,
    TEXTURE_GRADIENT_COOL,
    TEXTURE_TYPE_COUNT
};

void LoadTextures(Texture textures[TEXTURE_TYPE_COUNT])
{
    Image ct4, white, warm, cool;

    LoadImage(&ct4, "./assets/textures/ct4_orange.bmp");
    LoadImage(&white, 1, 1);
    LoadImage(&warm, 512, 512);
    LoadImage(&cool, 512, 512);

    white.pixels[0] = { 0xFF, 0xFF, 0xFF, 0xFF };
    LoadImageGradient(&warm, Vector3Zeros, Vector3UnitX, Vector3UnitY, Vector3UnitX + Vector3UnitY);
    LoadImageGradient(&cool, Vector3UnitZ, Vector3UnitZ + Vector3UnitX, Vector3UnitY + Vector3UnitZ, Vector3Ones);

    LoadTexture(&textures[TEXTURE_CT4], ct4);
    LoadTexture(&textures[TEXTURE_WHITE], white);
    LoadTexture(&textures[TEXTURE_GRADIENT_WARM], warm);
    LoadTexture(&textures[TEXTURE_GRADIENT_COOL], cool);
}

struct Camera
{
    float pitch = 0.0f;
    float yaw = 0.0f;
    Vector3 position = Vector3Zeros;
};

int main()
{
    CreateWindow(800, 800, "Graphics 1");

    Mesh meshes[MESH_TYPE_COUNT];
    LoadMeshPlane(&meshes[MESH_PLANE]);
    LoadMeshSphere(&meshes[MESH_SPHERE]);
    LoadMeshHemisphere(&meshes[MESH_HEMISPHERE]);

    LoadMeshObj(&meshes[MESH_HEAD], "./assets/meshes/head.obj");
    
    GLuint position_color_vert = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/position_color.vert");
    GLuint tcoord_color_vert = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/tcoord_color.vert");
    GLuint normal_color_vert = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/normal_color.vert");
    GLuint vertex_color_frag = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/vertex_color.frag");
    GLuint a4_texture_vert = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/a4_texture.vert");
    GLuint a4_texture_frag = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/a4_texture.frag");
    GLuint a5_lighting_vert = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/a5_lighting.vert");
    GLuint a5_lighting_frag = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/a5_lighting.frag");
    GLuint pass_through_vert = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/pass_through.vert");
    GLuint pass_through_frag = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/pass_through.frag");

    GLuint shaders[SHADER_TYPE_COUNT];
    shaders[SHADER_LIGHTING] = CreateProgram(a5_lighting_vert, a5_lighting_frag);
    shaders[SHADER_FLAT] = CreateProgram(pass_through_vert, pass_through_frag);
    shaders[SHADER_SAMPLE_TEXTURE] = CreateProgram(a4_texture_vert, a4_texture_frag);
    shaders[SHADER_POSITION_COLOR] = CreateProgram(position_color_vert, vertex_color_frag);
    shaders[SHADER_TCOORD_COLOR] = CreateProgram(tcoord_color_vert, vertex_color_frag);
    shaders[SHADER_NORMAL_COLOR] = CreateProgram(normal_color_vert, vertex_color_frag);

    Texture textures[TEXTURE_TYPE_COUNT];
    LoadTextures(textures);

    Camera camera;
    camera.position = { 0.0f, 0.0f, 5.0f };

    Vector3 light_position = Vector3UnitZ * 5.0f;
    Vector3 light_color = Vector3Ones;

    int shader_index = SHADER_LIGHTING;
    int mesh_index = MESH_HEAD;
    int texture_index = TEXTURE_WHITE;
    while (!WindowShouldClose())
    {
        BeginFrame();
        float dt = FrameTime();

        if (IsKeyPressed(KEY_ESCAPE))
            SetWindowShouldClose(true);

        if (IsKeyPressed(KEY_GRAVE_ACCENT))
            ++shader_index %= SHADER_TYPE_COUNT;

        if (IsKeyPressed(KEY_TAB))
            mesh_index = MESH_PLANE + ((mesh_index + 1) % MESH_TYPE_COUNT);

        if (IsKeyPressed(KEY_T))
            ++texture_index %= TEXTURE_TYPE_COUNT;

        float tt = Time();
        float nsin = sinf(tt) * 0.5f + 0.5f;

        if (IsKeyDown(KEY_1))
            camera.yaw -= 100.0f * dt * DEG2RAD;
        
        if (IsKeyDown(KEY_2))
            camera.yaw += 100.0f * dt * DEG2RAD;
        
        if (IsKeyDown(KEY_3))
            camera.pitch -= 100.0f * dt * DEG2RAD;
        
        if (IsKeyDown(KEY_4))
            camera.pitch += 100.0f * dt * DEG2RAD;

        Matrix camera_rotation = MatrixRotateY(camera.yaw) * MatrixRotateX(camera.pitch);
        Vector3 camera_direction_z = { camera_rotation.m8, camera_rotation.m9, camera_rotation.m10 };
        Vector3 camera_direction_x = { camera_rotation.m0, camera_rotation.m1, camera_rotation.m2 };
        Vector3 camera_direction_y = { camera_rotation.m4, camera_rotation.m5, camera_rotation.m6 };

        if (IsKeyDown(KEY_W))
            camera.position -= camera_direction_z * 10.0f * dt;
        
        if (IsKeyDown(KEY_S))
            camera.position += camera_direction_z * 10.0f * dt;
        
        if (IsKeyDown(KEY_D))
            camera.position += camera_direction_x * 10.0f * dt;
        
        if (IsKeyDown(KEY_A))
            camera.position -= camera_direction_x * 10.0f * dt;
         
        if (IsKeyDown(KEY_SPACE))
            camera.position += camera_direction_y * 10.0f * dt;
        
        if (IsKeyDown(KEY_LEFT_SHIFT))
            camera.position -= camera_direction_y * 10.0f * dt;

        Matrix proj = MatrixPerspective(75.0f * DEG2RAD, WindowWidth() / (float)WindowHeight(), 0.01f, 100.0f);
        Matrix view = MatrixInvert(camera_rotation * MatrixTranslate(camera.position.x, camera.position.y, camera.position.z));
        Matrix world = MatrixIdentity();
        Matrix mvp = world * view * proj;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render scene
        BeginTexture(textures[texture_index]);
            BeginShader(shaders[shader_index]);
                SendVec3(light_position, "u_light_position");
                SendVec3(light_color, "u_light_color");
                SendMat4(world, "u_world");
                SendMat4(mvp, "u_mvp");
                DrawMesh(meshes[mesh_index]);
            EndShader();
        EndTexture();

        // Render light
        world = MatrixTranslate(light_position.x, light_position.y, light_position.z);
        mvp = world * view * proj;
        BeginShader(shaders[SHADER_FLAT]);
            SendVec3(light_color, "u_color");
            SendMat4(mvp, "u_mvp");
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                DrawMesh(meshes[MESH_SPHERE]);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        EndShader();

        BeginGui();
        ImGui::SliderFloat3("Light Position", &light_position.x, -10.0f, 10.0f);
        ImGui::ColorPicker3("Light Color", &light_color.x);
        //ImGui::ShowDemoWindow(nullptr);
        EndGui();

        Loop();
        EndFrame();
    }

    DestroyShader(&position_color_vert);
    DestroyShader(&tcoord_color_vert);
    DestroyShader(&normal_color_vert);
    DestroyShader(&vertex_color_frag);

    for (int i = 0; i < TEXTURE_TYPE_COUNT; i++)
        UnloadTexture(&textures[i]);

    for (int i = 0; i < SHADER_TYPE_COUNT; i++)
        DestroyProgram(&shaders[i]);

    for (int i = 0; i < MESH_TYPE_COUNT; i++)
        UnloadMesh(&meshes[i]);

    DestroyWindow();
    return 0;
}
