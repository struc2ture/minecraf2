#include <stdbool.h>
#include <stdio.h>

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#include "common.h"
#include "gl_glue.h"
#include "platform_types.h"

typedef struct {
    GLuint prog;
    GLuint vao, vbo;
    float w, h;
    float rot_angle;
    bool is_rotating;
} Game_State;

void on_init(Game_State *state, GLFWwindow *window, float window_w, float window_h, float window_px_w, float window_px_h, bool is_live_scene, GLuint fbo, int argc, char **argv)
{
    state->w = window_px_w;
    state->h = window_px_h;

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
          0,    100,  0, 1, 0, 0,
         -100, -100, 0, 0, 1, 0,
          100, -100, 0, 0, 0, 1,
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
}

void on_reload(Game_State *state)
{
}

void on_frame(Game_State *state, const Platform_Timing *t)
{
    glViewport(0, 0, (GLsizei)state->w, (GLsizei)state->h);

    glClearColor(0.9f, 0.4f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(state->prog);
    glBindVertexArray(state->vao);
    
    //state->rot_angle += t->prev_delta_time * 3.0f;
    //state->rot_angle = ;
    
    Mat_4 proj = mat4_proj_ortho(0, state->w, state->h, 0, -1, 1);
    Mat_4 translation = mat4_translate(state->w * 0.5f, state->h * 0.5f, 0);
    Mat_4 rot_mat = mat4_rotate_axis(0, 0, 1, state->rot_angle);
    Mat_4 mvp = mat4_mul(mat4_mul(proj, translation), rot_mat);

    glUniformMatrix4fv(glGetUniformLocation(state->prog, "u_mvp"), 1, GL_FALSE, mvp.m);
    
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void on_platform_event(Game_State *state, const Platform_Event *e)
{
    switch (e->kind)
    {
        case PLATFORM_EVENT_MOUSE_BUTTON:
        {
            state->is_rotating = e->mouse_button.button == GLFW_MOUSE_BUTTON_LEFT && e->mouse_button.action == GLFW_PRESS;
        } break;

        case PLATFORM_EVENT_MOUSE_MOTION:
        {
            if (state->is_rotating)
            {
                state->rot_angle += 0.1f * e->mouse_motion.delta.x;
            }
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
