#include <stdbool.h>
#include <stdio.h>

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#include "common.h"
#include "gl_glue.h"
#include "platform_types.h"

#define PI 3.1415926535

typedef struct {
    Vec_3 pos;
    float pitch, yaw;
} Camera;

typedef struct {
    GLuint prog;
    GLuint vao, vbo;
    float w, h;
    float rot_angle;
    bool is_rotating;
    Camera camera;
    bool move_f, move_b, move_l, move_r, move_u, move_d;
    bool is_captured;
    GLFWwindow *window;
} Game_State;

void on_init(Game_State *state, GLFWwindow *window, float window_w, float window_h, float window_px_w, float window_px_h, bool is_live_scene, GLuint fbo, int argc, char **argv)
{
    if (!is_live_scene) glfwSetWindowTitle(window, "minecraf2");

    state->w = window_px_w;
    state->h = window_px_h;
    state->window = window;

    state->prog = gl_create_shader_program(
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "out vec3 Color;\n"
        "uniform mat4 u_mvp;\n"
        "void main()\n"
        "{\n"
        "  gl_Position = u_mvp * vec4(aPos, 1.0);\n"
        "  Color = aColor;\n"
        "}\n",

        "#version 330 core\n"
        "in vec3 Color;\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "  FragColor = vec4(Color, 1.0);\n"
        "}\n");
        
    float tri_verts[] =
    {
          0,    0.5,  0, 1, 0, 0,
         -0.5, -0.5, 0, 0, 1, 0,
          0.5, -0.5, 0, 0, 0, 1,
    };
    
    glGenVertexArrays(1, &state->vao);
    glBindVertexArray(state->vao);
    glGenBuffers(1, &state->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, state->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tri_verts), tri_verts, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    state->camera.pos = (Vec_3){0,0,-5};
    state->camera.yaw = PI * 1.5f;
}

void on_reload(Game_State *state)
{
}

static inline Vec_3 camera_forward(Camera camera)
{
    float cos_pitch = cosf(camera.pitch);
    Vec_3 result = {
        .x = cosf(camera.yaw) * cos_pitch,
        .y = sinf(camera.pitch),
        .z = -sinf(camera.yaw) * cos_pitch
    };
    return result;
}

static inline Mat_4 camera_view(Camera camera)
{
    Vec_3 f = camera_forward(camera);
    Vec_3 eye = camera.pos;
    Vec_3 center = vec3_add(eye, f);
    Vec_3 up =  {0, 1, 0};
    Mat_4 view = mat4_look_at(eye, center, up);
    return view;
}

void on_frame(Game_State *state, const Platform_Timing *t)
{
    glViewport(0, 0, (GLsizei)state->w, (GLsizei)state->h);

    glClearColor(0.9f, 0.4f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(state->prog);
    glBindVertexArray(state->vao);
    
    //state->rot_angle += t->prev_delta_time * 3.0f;
    //state->rot_angle = 0;
    
    //printf("Camera yaw: %0.3f\n", state->camera.yaw);
    
    const float move_speed = 3.0f;
    
    Vec_3 forward = camera_forward(state->camera);
    Vec_3 right = vec3_normalize(vec3_cross(forward, (Vec_3){0, 1, 0}));
    Vec_3 up = {0, 1, 0};
    
    Vec_3 move = {0};
    
    if (state->move_f) move = vec3_add(move, forward);
    if (state->move_b) move = vec3_sub(move, forward);
    if (state->move_l) move = vec3_sub(move, right);
    if (state->move_r) move = vec3_add(move, right);
    if (state->move_u) move = vec3_add(move, up);
    if (state->move_d) move = vec3_sub(move, up);
    
    if (move.x != 0 || move.y != 0 || move.z != 0)
    {
        move = vec3_normalize(move);
        move = vec3_scale(move, move_speed * t->prev_delta_time);
        state->camera.pos = vec3_add(state->camera.pos, move);
    }
    
    Mat_4 proj = mat4_proj_perspective(PI / 3, state->w / state->h, 0.1f, 100.0f);
    
    Mat_4 view = camera_view(state->camera);

    Mat_4 model = mat4_rotate_axis(0, 0, 1, state->rot_angle);
    Mat_4 mvp = mat4_mul(mat4_mul(proj, view), model);

    glUniformMatrix4fv(glGetUniformLocation(state->prog, "u_mvp"), 1, GL_FALSE, mvp.m);
    
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void on_platform_event(Game_State *state, const Platform_Event *e)
{
    switch (e->kind)
    {
        case PLATFORM_EVENT_KEY:
        {
            switch (e->key.key)
            {
                case GLFW_KEY_W:
                {
                    if (e->key.action == GLFW_PRESS) state->move_f = true;
                    else if (e->key.action == GLFW_RELEASE) state->move_f = false;
                } break;
                
                case GLFW_KEY_S:
                {
                    if (e->key.action == GLFW_PRESS) state->move_b = true;
                    else if (e->key.action == GLFW_RELEASE) state->move_b = false;
                } break;

                case GLFW_KEY_A:
                {
                    if (e->key.action == GLFW_PRESS) state->move_l = true;
                    else if (e->key.action == GLFW_RELEASE) state->move_l = false;
                } break;

                case GLFW_KEY_D:
                {
                    if (e->key.action == GLFW_PRESS) state->move_r = true;
                    else if (e->key.action == GLFW_RELEASE) state->move_r = false;
                } break;

                case GLFW_KEY_SPACE:
                {
                    if (e->key.action == GLFW_PRESS) state->move_u = true;
                    else if (e->key.action == GLFW_RELEASE) state->move_u = false;
                } break;
                
                case GLFW_KEY_LEFT_CONTROL:
                {
                    if (e->key.action == GLFW_PRESS) state->move_d = true;
                    else if (e->key.action == GLFW_RELEASE) state->move_d = false;
                } break;

            }
        } break;

        case PLATFORM_EVENT_MOUSE_MOTION:
        {
            if (state->is_captured)
            {
                state->camera.yaw -= 0.01f * e->mouse_motion.delta.x;
                state->camera.pitch -= 0.01f * e->mouse_motion.delta.y;
            }
        } break;
        
        case PLATFORM_EVENT_INPUT_CAPTURED:
        {
            if (e->input_captured.captured)
            {
                glfwSetInputMode(state->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            else
            {
                glfwSetInputMode(state->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            state->is_captured = e->input_captured.captured;
        } break;
        
        default: break;
    }
}

void on_destroy(Game_State *state)
{
    glDeleteBuffers(1, &state->vbo);
    glDeleteVertexArrays(1, &state->vao);
    glDeleteProgram(state->prog);
}
