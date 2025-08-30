#include "batch_renderer.h"    
#include <graphics_manager.h>

void BatchRenderer::Init(VertexShader* vertShader, FragmentShader* fragShader, Texture* texture)
{
    this->vertexShader = vertShader;
    this->fragmentShader = fragShader;
    this->texture = texture;
}

void BatchRenderer::BindResources()
{
    GraphicsManager::Get()->VertexShaderBind(vertexShader);
    GraphicsManager::Get()->FragmentShaderBind(fragmentShader);
    GraphicsManager::Get()->TextureBind(texture, 0);
}

void BatchRenderer::SetTexture(Texture* texture)
{
    this->texture = texture;
}
