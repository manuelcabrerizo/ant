static Assimp::Importer gImporter;

void Model::Init(const char *filepath)
{
/*
     aiProcess_MakeLeftHanded |
     aiProcess_FlipUVs | 
     aiProcess_FlipWindingOrder*/
     const aiScene *scene = gImporter.ReadFile(filepath, /*aiProcess_ConvertToLeftHanded*/0);
     i32 totalVerticesCount = 0;
     i32 totalIndicesCount = 0;
     for(i32 i = 0; i < scene->mNumMeshes; i++)
     {
          aiMesh *mesh = scene->mMeshes[i];
          totalVerticesCount += mesh->mNumVertices;
          totalIndicesCount += mesh->mNumFaces * 3;
     }

     Frame frame = MemoryManager::Get()->GetFrame(FRAME_MEMORY);
     Vertex *vertices = (Vertex *)MemoryManager::Get()->Alloc(totalVerticesCount * sizeof(Vertex), FRAME_MEMORY);
     u32 *indices = (u32 *)MemoryManager::Get()->Alloc(totalIndicesCount * sizeof(u32), FRAME_MEMORY);

     verticesCount = 0;
     indicesCount = 0;
     i32 indicesOffset = 0;
     for(i32 k = 0; k < scene->mNumMeshes; k++)
     {
          aiMesh *mesh = scene->mMeshes[k];
          for(i32 i = 0; i < mesh->mNumVertices; i++)
          {
               Vertex *vertex = vertices + verticesCount++;
               vertex->pos = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z*-1.0f };
               vertex->nor = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z*-1.0f };
               vertex->uvs = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
          }
     
          for(i32 i = 0; i < mesh->mNumFaces; i++)
          {
               aiFace *face = mesh->mFaces + i;
               for(unsigned int j = 0; j < face->mNumIndices; j++)
               {
                    indices[indicesCount++] = indicesOffset + face->mIndices[j];
               }
          }
          indicesOffset += indicesCount;
     }

     ASSERT(verticesCount == totalVerticesCount);
     ASSERT(indicesCount == totalIndicesCount);
     
     vertexBuffer = GraphicsManager::Get()->VertexBufferAlloc((void *)vertices, verticesCount, sizeof(Vertex));
     indexBuffer = GraphicsManager::Get()->IndexBufferAlloc(indices, indicesCount);

     MemoryManager::Get()->ReleaseFrame(frame);
}

void Model::Init(Vertex *vertices, u32 verticesCount_,
                 u32 *indices, u32 indicesCount_)
{
     verticesCount = verticesCount_;
     indicesCount = indicesCount_;
     vertexBuffer = GraphicsManager::Get()->VertexBufferAlloc((void *)vertices, verticesCount, sizeof(Vertex));
     indexBuffer = GraphicsManager::Get()->IndexBufferAlloc(indices, indicesCount);
}

void Model::Terminate()
{
     GraphicsManager::Get()->IndexBufferFree(indexBuffer);
     GraphicsManager::Get()->VertexBufferFree(vertexBuffer);
}

void Model::Draw()
{
     GraphicsManager::Get()->VertexBufferBind(vertexBuffer);
     GraphicsManager::Get()->IndexBufferBind(indexBuffer);
     GraphicsManager::Get()->Draw(verticesCount);
}
