#pragma once

#include <math/vector3.h>
#include <math/quaternion.h>

struct VertexShader;
struct FragmentShader;
struct Texture;

class BatchRenderer
{
private:
    VertexShader* vertexShader = nullptr;
    FragmentShader* fragmentShader = nullptr;
    Texture* texture = nullptr;
protected:
    void BindResources();
    void Init(VertexShader* vertShader, FragmentShader* fragShader, Texture* texture);
public:
    virtual ~BatchRenderer() {}
    virtual void Terminate() = 0;
    virtual void Present() = 0;
    virtual void DrawQuad(const Vector3& position, const Vector3& scale,
        const Quaternion& rotation, const Vector3& color) = 0;
    void SetTexture(Texture* texture);
};

