#include "ui_renderer.h"
#include <asset_managers/shader_manager.h>
#include <asset_managers/texture_manager.h>

static UIVertex vertices0[] = {
    UIVertex{ Vector2(-0.5, -0.5), Vector2(0, 0) },
    UIVertex{ Vector2(-0.5,  0.5), Vector2(0, 1) },
    UIVertex{ Vector2(0.5,  0.5), Vector2(1, 1) },
    UIVertex{ Vector2(0.5,  0.5), Vector2(1, 1) },
    UIVertex{ Vector2(0.5, -0.5), Vector2(1, 0) },
    UIVertex{ Vector2(-0.5, -0.5), Vector2(0, 0) }
};

static UIVertex vertices1[] = {
    UIVertex{ Vector2(-0.5, -0.5), Vector2(0, 1) },
    UIVertex{ Vector2(-0.5,  0.5), Vector2(0, 0) },
    UIVertex{ Vector2(0.5,  0.5), Vector2(1, 0) },
    UIVertex{ Vector2(0.5,  0.5), Vector2(1, 0) },
    UIVertex{ Vector2(0.5, -0.5), Vector2(1, 1) },
    UIVertex{ Vector2(-0.5, -0.5), Vector2(0, 1) }
};

void UIRenderer::Init(bool isRenderTarget)
{
    int width, height;
    PlatformClientDimensions(&width, &height);
    matrices.model = Matrix4();
    matrices.view = Matrix4();
    matrices.proj = Matrix4::Ortho(0, width, 0, height, 0, 100);
    uniformBuffer = GraphicsManager::Get()->UniformBufferAlloc(BIND_TO_VS, &matrices, sizeof(UIUbo), 0);
    if (isRenderTarget)
    {
        quad = GraphicsManager::Get()->VertexBufferAlloc(vertices1, ARRAY_LENGTH(vertices1), sizeof(UIVertex));
    }
    else
    {
        quad = GraphicsManager::Get()->VertexBufferAlloc(vertices0, ARRAY_LENGTH(vertices0), sizeof(UIVertex));
    }
}

void UIRenderer::Terminate()
{
    GraphicsManager::Get()->UniformBufferFree(uniformBuffer);
    GraphicsManager::Get()->VertexBufferFree(quad);
}

void UIRenderer::OnResize(const Vector2& extent)
{
    matrices.proj = Matrix4::Ortho(0, extent.x, 0, extent.y, 0, 100);
}

void UIRenderer::DrawQuat(const Vector2& position, const Vector2& size, int zIndex, const char* textureName, bool alphaBlended)
{
    if (alphaBlended)
    {
        GraphicsManager::Get()->SetAlphaBlending();
    }
    else
    {
        GraphicsManager::Get()->SetBlendingOff();
    }


    // Bind the ui shaders
    VertexShaderManager::Get()->Bind("ui_vert");
    FragmentShaderManager::Get()->Bind("ui_frag");

    matrices.model = Matrix4::Scale(size.x, size.y, 1.0f) * Matrix4::Translate(position.x, position.y, zIndex);
    GraphicsManager::Get()->UniformBufferUpdate(uniformBuffer, &matrices);
    GraphicsManager::Get()->UniformBufferBind(uniformBuffer);

    Texture* texture = TextureManager::Get()->Get(textureName);
    GraphicsManager::Get()->TextureBind(texture, 0);
    GraphicsManager::Get()->VertexBufferBind(quad);
    GraphicsManager::Get()->Draw(ARRAY_LENGTH(vertices0));
}

void UIRenderer::DrawQuat(const Vector2& position, const Vector2& size, int zIndex, bool alphaBlended)
{
    if (alphaBlended)
    {
        GraphicsManager::Get()->SetAlphaBlending();
    }
    else
    {
        GraphicsManager::Get()->SetBlendingOff();
    }

    // Bind the ui shaders
    VertexShaderManager::Get()->Bind("ui_vert");
    FragmentShaderManager::Get()->Bind("ui_frag");

    matrices.model = Matrix4::Scale(size.x, size.y, 1.0f) * Matrix4::Translate(position.x, position.y, zIndex);
    GraphicsManager::Get()->UniformBufferUpdate(uniformBuffer, &matrices);
    GraphicsManager::Get()->UniformBufferBind(uniformBuffer);
    GraphicsManager::Get()->VertexBufferBind(quad);
    GraphicsManager::Get()->Draw(ARRAY_LENGTH(vertices0));
}