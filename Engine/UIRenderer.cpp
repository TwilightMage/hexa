#include "UIRenderer.h"

#include <glad/glad.h>


#include "IRenderable.h"
#include "RendererUtils.h"
#include "Texture.h"
#include "ui/Image.h"

UIRenderer::UIRenderer()
{
    database_ = new render_database<Image>();
}

UIRenderer::~UIRenderer()
{
    delete database_;
}

void UIRenderer::register_object(const Weak<IRenderable>& renderable) const
{
    if (const auto image = cast<Image>(renderable))
    {
        database_->register_object(image);
    }
}

void UIRenderer::unregister_object(const Weak<IRenderable>& renderable) const
{
    if (const auto image = cast<Image>(renderable))
    {
        database_->unregister_object(image);
    }
}

void UIRenderer::change_object_mesh(const Weak<IRenderable>& renderable, const Weak<Mesh>& old_mesh)
{
    if (const auto image = cast<Image>(renderable))
    {
        database_->change_object_mesh(image, old_mesh);
    }
}

void UIRenderer::change_object_shader(const Weak<IRenderable>& renderable, const Weak<Shader>& old_shader)
{
    if (const auto image = cast<Image>(renderable))
    {
        database_->change_object_shader(image, old_shader);
    }
}

std::map<Texture*, uint> UIRenderer::dump_texture_usage() const
{
    return database_->dump_texture_usage();
}

void UIRenderer::render(const glm::mat4& view_projection_matrix) const
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto& db = *database_;
    for (const auto& shader_meshes : db)
    {
        if (shader_meshes.key->get_meta().transparency)
        {
            glEnable(GL_BLEND);
        }
        else
        {
            glDisable(GL_BLEND);
        }
        glUseProgram(shader_meshes.value.gl_shader_id);
        shader_meshes.key->map_params();

        for (const auto& mesh_objects : shader_meshes.value)
        {
            uint rendered_instance_count = 0;
            while (rendered_instance_count < mesh_objects.value.length())
            {
                const auto instance_count = std::min(mesh_objects.value.length() - rendered_instance_count, shader_meshes.key->get_meta().instance_count);

                // declare local buffers
                glm::mat4* mvp_array = new glm::mat4[instance_count];
                Quaternion* uv_array = new Quaternion[instance_count];
                Quaternion* color_array = new Quaternion[instance_count];
                uint64* texture_handle_array = new uint64[instance_count];

                // fill local buffers
                for (uint i = 0; i < instance_count; i++)
                {
                    const auto& object = mesh_objects.value[rendered_instance_count + i];
                    mvp_array[i] = view_projection_matrix * object->get_matrix();
                    uv_array[i] = object->get_uv_rect();
                    color_array[i] = Quaternion(1.0f, 1.0f, 1.0f, 1.0f);
                    texture_handle_array[i] = object->get_texture()->get_handle_arb();
                }

                // put local buffers into uniform arrays
                glUniformMatrix4fv(0, instance_count, GL_FALSE, reinterpret_cast<float*>(mvp_array));
                glUniform4fv(100, instance_count, reinterpret_cast<float*>(uv_array));
                glUniform4fv(200, instance_count, reinterpret_cast<float*>(color_array));
                glUniformHandleui64vARB(300, instance_count, texture_handle_array);

                // clear local buffers
                delete[] mvp_array;
                delete[] uv_array;
                delete[] color_array;
                delete[] texture_handle_array;
            		
                glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instance_count);
                rendered_instance_count += instance_count;
            }
        }
    }
}

void UIRenderer::cleanup() const
{
    database_->cleanup();
}
