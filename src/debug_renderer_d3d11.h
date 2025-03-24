struct VertexLine
{
     vec3 pos;
     vec3 col;
};

class DebugRenderer
{
private:
     ID3D11Buffer *gpuBuffer;
     VertexLine   *cpuBuffer;
     u64 bufferSize;
     u32 bufferUsed;

     Shader *shader;
public:
     void Init();
     void Terminate();
     void Present();

     void DrawLine(vec3& a, vec3 &b);
};
