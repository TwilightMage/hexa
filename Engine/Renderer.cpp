#include "Renderer.h"

#include <glad/glad.h>

#include "IRenderable.h"
#include "RendererUtils.h"
#include "Texture.h"

Renderer::Renderer()
{
    database_ = new render_database<IRenderable>();
}

Renderer::~Renderer()
{
    delete database_;
}

bool Renderer::register_object(const Weak<IRenderable>& renderable) const
{
    return database_->register_object(renderable);
}

bool Renderer::unregister_object(const Weak<IRenderable>& renderable) const
{
    return database_->unregister_object(renderable);
}

bool Renderer::change_object_mesh(const Weak<IRenderable>& renderable, const Weak<Mesh>& old_mesh)
{
    return database_->change_object_mesh(renderable, old_mesh);
}

bool Renderer::change_object_shader(const Weak<IRenderable>& renderable, const Weak<Shader>& old_shader)
{
    return database_->change_object_shader(renderable, old_shader);
}

std::map<Texture*, uint> Renderer::dump_texture_usage() const
{
    return database_->dump_texture_usage();
}

void Renderer::render(const Matrix4x4& view_projection_matrix) const
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    auto& db = *database_;
    for (const auto& shader_meshes : db)
    {
        if (shader_meshes.key->get_meta().transparency)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
        {
            glDisable(GL_BLEND);
        }
        glUseProgram(shader_meshes.value.gl_shader_id);
        glBindBuffer(GL_ARRAY_BUFFER, shader_meshes.value.gl_vertex_buffer_id);
        shader_meshes.key->map_params();

        for (const auto& mesh_objects : shader_meshes.value)
        {
            uint rendered_instance_count = 0;
            while (rendered_instance_count < mesh_objects.value.length())
            {
                const auto instance_count = std::min(mesh_objects.value.length() - rendered_instance_count, shader_meshes.key->get_meta().instance_count);
                put_params_for_intances(view_projection_matrix, &mesh_objects.value[rendered_instance_count], instance_count);
            		
                glDrawArraysInstanced(GL_TRIANGLES, mesh_objects.value.vertex_buffer_offset, mesh_objects.value.size_in_vertex_buffer, instance_count);
                rendered_instance_count += instance_count;
            }
        }
    }
}

void Renderer::cleanup() const
{
    database_->cleanup();
}

void Renderer::put_params_for_intances(const Matrix4x4& view_projection_matrix, const Shared<IRenderable>* objects, uint instance_count) const
{
    // declare local buffers
    Matrix4x4* mvp_array = new Matrix4x4[instance_count];
    uint64* texture_handle_array = new uint64[instance_count];

    // fill local buffers
    for (uint i = 0; i < instance_count; i++)
    {
        const auto& object = objects[i];
        mvp_array[i] = view_projection_matrix * object->get_matrix();
        texture_handle_array[i] = object->get_texture()->get_handle_arb();
    }

    // put local buffers into uniform arrays
    glUniformMatrix4fv(0, instance_count, GL_FALSE, reinterpret_cast<float*>(mvp_array));
    glUniformHandleui64vARB(230, instance_count, texture_handle_array);

    // clear local buffers
    delete[] mvp_array;
    delete[] texture_handle_array;
}
