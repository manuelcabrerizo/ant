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
    int width, height;
    PlatformClientDimensions(&width, &height);
    matrices.model = Matrix4();
    matrices.view = Matrix4();
    matrices.proj = Matrix4::Ortho(-width*0.5f, width*0.5f, -height*0.5f, height*0.5f, 0, 100);
    uniformBuffer = GraphicsManager::Get()->UniformBufferAlloc(BIND_TO_VS, &matrices, sizeof(UIUbo), 0);
    quad = GraphicsManager::Get()->VertexBufferAlloc(vertices, ARRAY_LENGTH(vertices), sizeof(UIVertex));
}

void UIRenderer::Terminate()
{
    GraphicsManager::Get()->UniformBufferFree(uniformBuffer);
    GraphicsManager::Get()->VertexBufferFree(quad);
}

void UIRenderer::DrawQuat(const Vector2& position, const Vector2& size, int zIndex, const char* textureName)
{
    // Bind the ui shaders
    VertexShaderManager::Get()->Bind("ui_vert");
    FragmentShaderManager::Get()->Bind("ui_frag");

    matrices.model = Matrix4::Translate(position.x, position.y, zIndex) * Matrix4::Scale(size.x, size.y, 1.0f);
    GraphicsManager::Get()->UniformBufferUpdate(uniformBuffer, &matrices);
    GraphicsManager::Get()->UniformBufferBind(uniformBuffer);

    Texture* texture = TextureManager::Get()->Get(textureName);
    GraphicsManager::Get()->TextureBind(texture, 0);
    GraphicsManager::Get()->VertexBufferBind(quad);
    GraphicsManager::Get()->Draw(ARRAY_LENGTH(vertices));
}

void MenuState::Init(GameManager* gameManager)
{
    this->gameManager = gameManager;
    uiRenderer.Init();

    // Load menu assets
    TextureManager::Get()->Load("PlayButton", "data/textures/play_button.png");
    TextureManager::Get()->Load("ExitButton", "data/textures/exit_button.png");
    TextureManager::Get()->Load("BackGround", "data/textures/blood_1.png");
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
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            int width = 512;
            int height = 512;
            uiRenderer.DrawQuat(Vector2(width * j, height * i), Vector2(width, height), 99, "BackGround");
        }
    }
    uiRenderer.DrawQuat(Vector2(0, 60), Vector2(300, 100), 0, "PlayButton");
    uiRenderer.DrawQuat(Vector2(0, 60 - 120), Vector2(300, 100), 0, "ExitButton");
}