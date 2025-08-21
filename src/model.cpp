#include <model.h>
#include <memory_manager.h>
// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <utils.h>
#include <strings.h>
#include <asset_managers/texture_manager.h>
#include <asset_managers/material_manager.h>

void Mesh::Init(Vertex *vertices, u32 verticesCount,
                u32 *indices, u32 indicesCount)
{
    this->material = nullptr;
    this->verticesCount = verticesCount;
    this->indicesCount = indicesCount;
    this->vertexBuffer = GraphicsManager::Get()->VertexBufferAlloc((void *)vertices, verticesCount, sizeof(Vertex));
    this->indexBuffer = GraphicsManager::Get()->IndexBufferAlloc(indices, indicesCount);
}

void Mesh::Terminate()
{
    GraphicsManager::Get()->IndexBufferFree(indexBuffer);
    GraphicsManager::Get()->VertexBufferFree(vertexBuffer);
}

void Mesh::Draw()
{
    material->Bind();
    GraphicsManager::Get()->VertexBufferBind(vertexBuffer);
    GraphicsManager::Get()->IndexBufferBind(indexBuffer);
    GraphicsManager::Get()->DrawIndexed(indicesCount);
}

void Mesh::SetMaterial(Material* material)
{
    this->material = material;
}

void Model::Init(const char *filepath, i32 memoryType)
{
    this->memoryType = memoryType;
     const aiScene *scene = Utils::importer.ReadFile(filepath,
            aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder |
            aiProcess_Triangulate | aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace);

     Frame materialsFrame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
     Array<aiString> materials;
     if (scene->mNumMaterials > 0)
     {
         materials.Init(scene->mNumMaterials, SCRATCH_MEMORY);
         for (int i = 0; i < scene->mNumMaterials; ++i)
         {
             aiMaterial* material = scene->mMaterials[i];
             aiString name;
             material->Get(AI_MATKEY_NAME, name);
             materials.Push(name);

             Frame stringFrame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
                
             bool isTextured = false;
             const char* materialName = name.C_Str();
             const char* diffuseTextureName = "DefaultMaterial_Diffuse";
             const char* normalTextureName = "DefaultMaterial_Normal";
             const char* specularTextureName = "DefaultMaterial_Specular";
             
             if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
             {
                 isTextured = true;
                 aiString path;
                 if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path, 0, 0, 0, 0, 0) == AI_SUCCESS)
                 {
                    i32 prevIndex = Strings::FindPenultimateInstance(filepath, '/');
                    i32 postIndex = Strings::FindFirstInstance(path.C_Str(), '\\');

                    // ERROR: invalid model foulder structure, (we use source/textures standar)
                    ASSERT(prevIndex != -1 && postIndex != -1);

                    const char* texturePath = (const char*)MemoryManager::Get()->Alloc(_MAX_PATH, SCRATCH_MEMORY);
                    const char* tempPath = (const char*)MemoryManager::Get()->Alloc(_MAX_PATH, SCRATCH_MEMORY);
                    memset((void*)texturePath, 0, _MAX_PATH);
                    memset((void*)tempPath, 0, _MAX_PATH);
                    
                    memcpy((void*)texturePath, filepath, prevIndex);
                    memcpy((void*)tempPath, path.C_Str() + postIndex, path.length - postIndex);

                    Strings::ReplaceInstance((char *)tempPath, '\\', '/');

                    memcpy((void *)(texturePath + strlen(texturePath)), tempPath, strlen(tempPath));

                    i32 nameIndex = Strings::FindLastInstance(tempPath, '/');
                    i32 extensionIndex = Strings::FindLastInstance(tempPath, '.');

                    // ERROR: invalid model foulder structure, (we use source/textures standar)
                    ASSERT(nameIndex != -1 && extensionIndex != -1);

                    memset((void*)(tempPath + extensionIndex), 0, strlen(tempPath) - extensionIndex);

                    diffuseTextureName = tempPath + (nameIndex + 1);

                    TextureManager::Get()->Load(diffuseTextureName, texturePath);
                     
                 }
             }

             aiColor3D aiDiffuseColor;
             Vector3 diffuseColor;
             if (material->Get(AI_MATKEY_COLOR_DIFFUSE, aiDiffuseColor) == AI_SUCCESS)
             {
                 diffuseColor.x = aiDiffuseColor.r;
                 diffuseColor.y = aiDiffuseColor.g;
                 diffuseColor.z = aiDiffuseColor.b;
             }

             if (isTextured)
             {
                MaterialManager::Get()->LoadTexture(materialName, "default",
                    diffuseTextureName, normalTextureName, specularTextureName, 64);
             }
             else
             {
                MaterialManager::Get()->LoadSolidColor(materialName, "color", diffuseColor,
                    diffuseColor, Vector3(1.0f, 1.0f, 1.0f), 64);
             }

             MemoryManager::Get()->ReleaseFrame(stringFrame);
         }
     }

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
               maxBoneCount = Utils::NextPower2(maxBoneCount);
          }
          bonesInfo.Init(maxBoneCount*2, memoryType);
     }

     for(i32 k = 0; k < scene->mNumMeshes; ++k)
     {
          aiMesh *mesh = scene->mMeshes[k]; 

          Frame frame = MemoryManager::Get()->GetFrame(SCRATCH_MEMORY);
          Vertex *vertices = (Vertex *)MemoryManager::Get()->Alloc(mesh->mNumVertices * sizeof(Vertex), SCRATCH_MEMORY);
          u32 *indices = (u32 *)MemoryManager::Get()->Alloc((mesh->mNumFaces * 3) * sizeof(u32), SCRATCH_MEMORY);

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
                    boneInfo.offset = Utils::FromAssimp(mesh->mBones[boneIndex]->mOffsetMatrix);
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
          myMesh.Init(vertices, mesh->mNumVertices, indices, indicesCount);

          if(MaterialManager::Get()->Contains(materials[mesh->mMaterialIndex].C_Str()))
          {
              myMesh.SetMaterial(MaterialManager::Get()->Get(materials[mesh->mMaterialIndex].C_Str()));
          }
          else
          {
              myMesh.SetMaterial(MaterialManager::Get()->Get("DefaultMaterial"));
          }
          meshes.Push(myMesh);

          MemoryManager::Get()->ReleaseFrame(frame);
     }

     MemoryManager::Get()->ReleaseFrame(materialsFrame);
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

void Model::SetDefaultMaterial(Material* material)
{
    for (i32 i = 0; i < meshes.size; ++i)
    {
        meshes[i].SetMaterial(material);
    }
}

void Model::SetMaterialAtMeshIndex(i32 index, Material* material)
{
    meshes[index].SetMaterial(material);
}