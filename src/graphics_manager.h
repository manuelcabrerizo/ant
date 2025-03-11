#pragma once

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#define GRAPHICS_MANAGER_MEMORY_SIZE KB(100)
#define MAX_VERTEX_BUFFER_COUNT  10
#define MAX_INDEX_BUFFER_COUNT   10
#define MAX_UNIFORM_BUFFER_COUNT 10
#define MAX_FRAME_BUFFER_COUNT   10
#define MAX_SHADER_COUNT         10
#define MAX_TEXTURE_COUNT        10

enum GraphicsTypeID
{
     VERTEX_BUFFER_TYPE_ID,
     INDEX_BUFFER_TYPE_ID,
     UNIFORM_BUFFER_TYPE_ID,
     FRAME_BUFFER_TYPE_ID,
     SHADER_TYPE_ID,
     TEXTURE_TYPE_ID
};

enum UniformBufferBind
{
     BIND_TO_VS = (1 << 0),
     BIND_TO_PS = (1 << 1),
     BIND_TO_GS = (1 << 2)
};

struct DirectXVertexBuffer;
struct DirectXIndexBuffer;
struct DirectXUniformBuffer;
struct DirectXFrameBuffer;
struct DirectXShader;
struct DirectXTexture;

typedef SlotmapKey<DirectXVertexBuffer> VertexBuffer;
typedef SlotmapKey<DirectXIndexBuffer> IndexBuffer;
typedef SlotmapKey<DirectXUniformBuffer> UniformBuffer;
typedef SlotmapKey<DirectXFrameBuffer> FrameBuffer;
typedef SlotmapKey<DirectXShader> Shader;
typedef SlotmapKey<DirectXTexture> Texture;

class GraphicsManager
{
private:
     GraphicsManager() {}
     static GraphicsManager instance;
     static bool initialize;
public:

     static void Init(void *osWindow, i32 width, i32 height, i32 stackNum);
     static void Terminate();
     static GraphicsManager *Get();
     
     void BeginFrame(f32 r, f32 g, f32 b);
     void EndFrame(i32 vsync);

     void Draw(u32 verticesCount);
     void DrawIndexed(u32 indexCount);

     void SetRasterizerStateCullBack();
     void SetRasterizerStateCullFront();
     void SetRasterizerStateCullNone();
     void SetRasterizerStateWireframe();
     
     VertexBuffer VertexBufferAlloc(void *vertices, u32 count, u32 stride);
     void VertexBufferFree(VertexBuffer vertexBuffer);
     void VertexBufferBind(VertexBuffer vertexBuffer);

     IndexBuffer IndexBufferAlloc(u32 *indices, u32 count);
     void IndexBufferFree(IndexBuffer indexBuffer);
     void IndexBufferBind(IndexBuffer indexBuffer);

     UniformBuffer UniformBufferAlloc(u32 bindTo, void *data, u32 dataSize, u32 slot);
     void UniformBufferFree(UniformBuffer uniformBuffer);
     void UniformBufferBind(UniformBuffer uniformBuffer);
     void UniformBufferUpdate(UniformBuffer uniformBuffer, void *data);

     Shader ShaderAlloc(File vertFilepath, File fragFilepath);
     void ShaderFree(Shader shader);
     void ShaderBind(Shader shader);

     Texture TextureAlloc(const char *filepath);
     void TextureFree(Texture texture);
     void TextureBind(Texture texture, i32 slot);
     i32 TextureWidth(Texture texture);
     i32 TextureHeight(Texture texture);
};
