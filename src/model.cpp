// TODO: fix gImporter architecture
static Assimp::Importer gImporter;

void Mesh::Init(Vertex *vertices, u32 verticesCount,
                u32 *indices, u32 indicesCount,
                Texture *texture)
{
     this->verticesCount = verticesCount;
     this->indicesCount = indicesCount;
     this->vertexBuffer = GraphicsManager::Get()->VertexBufferAlloc((void *)vertices, verticesCount, sizeof(Vertex));
     this->indexBuffer = GraphicsManager::Get()->IndexBufferAlloc(indices, indicesCount);
     if(texture != nullptr)
     {
          this->texture = texture;
     }
}

void Mesh::Terminate()
{
     GraphicsManager::Get()->IndexBufferFree(indexBuffer);
     GraphicsManager::Get()->VertexBufferFree(vertexBuffer);
}

void Mesh::Draw()
{
     GraphicsManager::Get()->VertexBufferBind(vertexBuffer);
     GraphicsManager::Get()->IndexBufferBind(indexBuffer);
     GraphicsManager::Get()->DrawIndexed(indicesCount);
}

// TODO: remove this
static i32 TempTempNextPower2(u32  x)
{
     int  value  =  1 ;
     while  ( value  <=  x)
     {
          value  =  value  <<  1 ;
     }
     return  value;
}

void Model::Init(const char *filepath, i32 memoryType)
{
     const aiScene *scene = gImporter.ReadFile(filepath,
          aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
     if(scene->mNumMeshes > 0)
     {
          meshes.Init(scene->mNumMeshes, memoryType);
     }
     else
     {
          return;
     }

     i32 maxBoneCount = -1;
     for(i32 i = 0; i < scene->mNumMeshes; ++i)
     {
          aiMesh *mesh = scene->mMeshes[i]; 
          if((i32)mesh->mNumBones > maxBoneCount)
          {
               maxBoneCount = mesh->mNumBones;
          }
     }

     if(maxBoneCount > 0)
     {
          if((maxBoneCount & (maxBoneCount - 1)) != 0)
          {
               maxBoneCount = TempTempNextPower2(maxBoneCount);
          }
          bonesInfo.Init(maxBoneCount*2, memoryType);
     }

     for(i32 k = 0; k < scene->mNumMeshes; ++k)
     {
          aiMesh *mesh = scene->mMeshes[k]; 

          Frame frame = MemoryManager::Get()->GetFrame();
          Vertex *vertices = (Vertex *)MemoryManager::Get()->Alloc(mesh->mNumVertices * sizeof(Vertex), FRAME_MEMORY);
          u32 *indices = (u32 *)MemoryManager::Get()->Alloc((mesh->mNumFaces * 3) * sizeof(u32), FRAME_MEMORY);

          for(i32 i = 0; i < mesh->mNumVertices; i++)
          {
               Vertex *vertex = vertices + i;
               vertex->pos = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
               vertex->nor = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
               vertex->uvs = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

               for(u32 j = 0; j < MAX_BONE_INFLUENCE; j++)
               {
                    vertex->boneId[j] = -1;
                    vertex->weights[j] = 0.0f;
               }
          }

          boneCounter = 0;
          for(i32 boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) 
          {

               i32 boneId = -1;
               const char *boneName = mesh->mBones[boneIndex]->mName.C_Str();
               if(bonesInfo.Contains(boneName) == false)
               {
                    BoneInfo boneInfo;
                    boneInfo.id = boneCounter;
                    boneInfo.offset = ai_mat4_to_sd_mat4(mesh->mBones[boneIndex]->mOffsetMatrix);
                    bonesInfo.Add(boneName, boneInfo);
                    boneId = boneCounter;
                    boneCounter++;
               }
               else
               {
                    boneId = bonesInfo.Get(boneName)->id;
               }
               ASSERT(boneId != -1);

               auto weights = mesh->mBones[boneIndex]->mWeights;
               i32 weightsCount = mesh->mBones[boneIndex]->mNumWeights;

               for(i32 weightIndex = 0; weightIndex < weightsCount; ++weightIndex)
               {
                    i32 vertexId = weights[weightIndex].mVertexId;
                    f32 weight = weights[weightIndex].mWeight;
                    ASSERT(vertexId < mesh->mNumVertices);
                    for(i32 i = 0; i < MAX_BONE_INFLUENCE; ++i) 
                    {
                         if(vertices[vertexId].boneId[i] < 0)
                         {
                              vertices[vertexId].weights[i] = weight;
                              vertices[vertexId].boneId[i] = boneId;
                              break;
                         }
                    }
               }
          }

          int indicesCount = 0;
          for(i32 i = 0; i < mesh->mNumFaces; i++)
          {
               aiFace *face = mesh->mFaces + i;
               for(unsigned int j = 0; j < face->mNumIndices; j++)
               {
                    indices[indicesCount++] = face->mIndices[j];
               }
          }

          ASSERT(indicesCount == (mesh->mNumFaces * 3));

          Mesh myMesh;
          myMesh.Init(vertices, mesh->mNumVertices, indices, indicesCount, nullptr);
          meshes.Push(myMesh);

          MemoryManager::Get()->ReleaseFrame(frame);
     }
}

void Model::Init(Vertex *vertices, u32 verticesCount, u32 *indices, u32 indicesCount)
{

}

void Model::Terminate()
{
     for(i32  i = 0; i < meshes.size; ++i)
     {
          meshes[i].Terminate();
     }
}

void Model::Draw()
{
     for(i32  i = 0; i < meshes.size; ++i)
     {
          meshes[i].Draw();
     }
}

HashMap<BoneInfo>& Model::GetBonesInfo()
{
     return bonesInfo;
}