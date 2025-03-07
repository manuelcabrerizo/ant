#pragma once

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

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
public:
     static void BeginFrame(f32 r, f32 g, f32 b);
     static void EndFrame(i32 vsync);

     static void Draw(u32 verticesCount);
     static void DrawIndexed(u32 indexCount);
     
     static VertexBuffer VertexBufferAlloc(void *vertices, u32 count, u32 stride);
     static void VertexBufferFree(VertexBuffer vertexBuffer);
     static void VertexBufferBind(VertexBuffer vertexBuffer);

     static IndexBuffer IndexBufferAlloc(u32 *indices, u32 count);
     static void IndexBufferFree(IndexBuffer indexBuffer);
     static void IndexBufferBind(IndexBuffer indexBuffer);

     static UniformBuffer UniformBufferAlloc(u32 bindTo, void *data, u32 dataSize, u32 slot);
     static void UniformBufferFree(UniformBuffer uniformBuffer);
     static void UniformBufferBind(UniformBuffer uniformBuffer);
     static void UniformBufferUpdate(UniformBuffer uniformBuffer, void *data);

     static Shader ShaderAlloc(File vertFilepath, File fragFilepath);
     static void ShaderFree(Shader shader);
     static void ShaderBind(Shader shader);

     static Texture TextureAlloc(const char *filepath);
     static void TextureFree(Texture texture);
     static void TextureBind(Texture texture, i32 slot);
     static i32 TextureWidth(Texture texture);
     static i32 TextureHeight(Texture texture);
};
