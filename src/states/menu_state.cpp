#include "menu_state.h"
#include <game_manager.h>
#include <input_manager.h>

#include <asset_managers/texture_manager.h>
#include <asset_managers/shader_manager.h>

static UIVertex vertices[] = {
    UIVertex{ Vector2(-0.5, -0.5), Vector2(0, 0) },
    UIVertex{ Vector2(-0.5,  0.5), Vector2(0, 1) },
    UIVertex{ Vector2( 0.5,  0.5), Vector2(1, 1) },
    UIVertex{ Vector2( 0.5,  0.5), Vector2(1, 1) },
    UIVertex{ Vector2( 0.5, -0.5), Vector2(1, 0) },
    UIVertex{ Vector2(-0.5, -0.5), Vector2(0, 0) }
};

void UIRenderer::Init()
{
    quad = GraphicsManager::Get()->VertexBufferAlloc(vertices, ARRAY_LENGTH(vertices), sizeof(UIVertex));

    int width, height;
    PlatformClientDimensions(&width, &height);

    matrices.model = Matrix4();
    matrices.view = Matrix4();
    matrices.proj = Matrix4::Ortho(-width*0.5f, width*0.5f, -height*0.5f, height*0.5f, 0.001f, 100);

    uniformBuffer = GraphicsManager::Get()->UniformBufferAlloc(BIND_TO_VS, &matrices, sizeof(UIUbo), 0);
    GraphicsManager::Get()->UniformBufferBind(uniformBuffer);
}

void UIRenderer::Terminate()
{
    GraphicsManager::Get()->UniformBufferFree(uniformBuffer);
    GraphicsManager::Get()->VertexBufferFree(quad);
}

void UIRenderer::DrawQuat(const char* textureName, const Vector2& position, const Vector2& size)
{
    // Bind the ui shaders
    VertexShaderManager::Get()->Bind("ui_vert");
    FragmentShaderManager::Get()->Bind("ui_frag");

    Texture* texture = TextureManager::Get()->Get(textureName);
    GraphicsManager::Get()->TextureBind(texture, 0);
    GraphicsManager::Get()->VertexBufferBind(quad);
    GraphicsManager::Get()->Draw(ARRAY_LENGTH(vertices));
}

void MenuState::Init(GameManager* gameManager)
{
    this->gameManager = gameManager;
    uiRenderer.Init();
}

void MenuState::OnEnter()
{

}

void MenuState::OnExit()
{
}

void MenuState::OnUpdate(float deltaTime)
{
    if (InputManager::Get()->KeyJustDown(KEY_RETURN))
    {
        gameManager->ChangeToGameState();
    }
}

void MenuState::OnRender()
{
    uiRenderer.DrawQuat("DefaultMaterial_Diffuse", Vector2(), Vector2());
}