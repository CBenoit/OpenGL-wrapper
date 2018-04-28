#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <ow/model.hpp>
#include <ow/utils.hpp>

ow::model::model(const std::string& path)
		: m_meshes{}
		, m_directory{}
{
	load_model(path);
}

void ow::model::draw(const shader_program& prog) const {
	for (auto& mesh : m_meshes) {
		mesh.draw(prog);
	}
}

void ow::model::load_model(const std::string& path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		logger << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	m_directory = path.substr(0, path.find_last_of('/'));

	process_node(scene->mRootNode, scene);
}

void ow::model::process_node(aiNode* node, const aiScene *scene) {
	// process all the node's meshes (if any)
	for(unsigned int i = 0; i < node->mNumMeshes; ++i) {
		process_mesh(scene->mMeshes[node->mMeshes[i]], scene);
	}
	// then do the same for each of its children
	for(unsigned int i = 0; i < node->mNumChildren; ++i) {
		process_node(node->mChildren[i], scene);
	}
}

void ow::model::process_mesh(aiMesh* mesh, const aiScene *scene) {
	std::vector<vertex> vertices;
	std::vector<unsigned int> indices;

	// vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		glm::vec3 pos{
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
		};

		glm::vec3 normals{
			mesh->mNormals[i].x,
			mesh->mNormals[i].y,
			mesh->mNormals[i].z
		};

		glm::vec2 tex_coords{0.0f};
		if (mesh->mTextureCoords[0]) { // does the mesh contain texture coordinates?
			tex_coords.x = mesh->mTextureCoords[0][i].x;
			tex_coords.y = mesh->mTextureCoords[0][i].y;
		}

		vertices.emplace_back(pos, normals, tex_coords);
	}

	// indices
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j) {
			indices.push_back(face.mIndices[j]);
		}
	}

	// materials
	//if (mesh->mMaterialIndex >= 0) { // if (true)
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	std::vector<std::shared_ptr<texture>> diffuse_maps = load_material_textures(material, aiTextureType_DIFFUSE, texture_type::diffuse);
	std::vector<std::shared_ptr<texture>> specular_maps = load_material_textures(material, aiTextureType_SPECULAR, texture_type::specular);
	std::vector<std::shared_ptr<texture>> emission_maps = load_material_textures(material, aiTextureType_EMISSIVE, texture_type::emission);
	//}

	m_meshes.emplace_back(std::move(vertices), std::move(indices), std::move(diffuse_maps), std::move(specular_maps), std::move(emission_maps));
}

std::vector<std::shared_ptr<ow::texture>> ow::model::load_material_textures(aiMaterial* mat, aiTextureType type, const texture_type& type_name) {
	std::vector<std::shared_ptr<texture>> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		std::string filename = m_directory + '/' + std::string(str.C_Str());
		textures.push_back(std::make_shared<texture>(filename, type_name));
	}
	return textures;
}
