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

bool UIRenderer::register_object(const Shared<IRenderable>& renderable) const
{
    if (const auto image = cast<Image>(renderable))
    {
        return database_->register_object(image);
    }

    return false;
}

bool UIRenderer::unregister_object(const Shared<IRenderable>& renderable) const
{
    if (const auto image = cast<Image>(renderable))
    {
        return database_->unregister_object(image);
    }

    return false;
}

bool UIRenderer::change_object_mesh(const Shared<IRenderable>& renderable, const Shared<Mesh>& old_mesh)
{
    if (const auto image = cast<Image>(renderable))
    {
        return database_->change_object_mesh(image, old_mesh);
    }

    return false;
}

bool UIRenderer::change_object_shader(const Shared<IRenderable>& renderable, const Shared<Shader>& old_shader)
{
    if (const auto image = cast<Image>(renderable))
    {
        return database_->change_object_shader(image, old_shader);
    }

    return false;
}

std::map<Texture*, uint> UIRenderer::dump_texture_usage() const
{
    return database_->dump_texture_usage();
}

void UIRenderer::render(const Matrix4x4& view, const Matrix4x4& projection, const Shared<World>& world) const
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
        shader_meshes.key->map_params();

        for (const auto& mesh_objects : shader_meshes.value)
        {
            uint rendered_instance_count = 0;
            while (rendered_instance_count < mesh_objects.value.length())
            {
                const auto instance_count = std::min(mesh_objects.value.length() - rendered_instance_count, shader_meshes.key->get_meta().instance_count);

                // declare local buffers
                Matrix4x4* mvp_array = new Matrix4x4[instance_count];
                Quaternion* uv_array = new Quaternion[instance_count];
                Quaternion* color_array = new Quaternion[instance_count];
                uint64* texture_handle_array = new uint64[instance_count];

                // fill local buffers
                for (uint i = 0; i < instance_count; i++)
                {
                    const auto& object = mesh_objects.value[rendered_instance_count + i];
                    mvp_array[i] = projection * view * object->get_matrix();
                    uv_array[i] = object->get_uv_rect();
                    color_array[i] = object->get_color().to_quaternion();
                    texture_handle_array[i] = object->get_texture()->get_handle_arb();
                }

                // put local buffers into uniform arrays
                glUniformMatrix4fv(0, instance_count, GL_FALSE, reinterpret_cast<float*>(mvp_array));
                glUniform4fv(146, instance_count, reinterpret_cast<float*>(uv_array));
                glUniform4fv(292, instance_count, reinterpret_cast<float*>(color_array));
                glUniformHandleui64vARB(438, instance_count, texture_handle_array);

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
