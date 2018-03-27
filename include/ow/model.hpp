#pragma once

#include <vector>
#include <string>

#include <assimp/scene.h>
#include <ow/shader_program.hpp>
#include <ow/mesh.hpp>
#include <ow/texture.hpp>

namespace ow {

class model {
public:
    model(std::string path);
    void draw(shader_program prog) const;

private:
    std::vector<mesh> m_meshes;
    std::string m_directory;

    void load_model(std::string path);
    void process_node(aiNode* node, const aiScene* scene);
    void process_mesh(aiMesh* mesh, const aiScene* scene);
    std::vector<std::shared_ptr<texture>> load_material_textures(aiMaterial* mat, aiTextureType type, std::string type_name);
};

}

