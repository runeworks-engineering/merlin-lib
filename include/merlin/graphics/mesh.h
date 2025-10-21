#pragma once
#include "merlin/core/core.h"
#include "merlin/utils/boundingBox.h"
#include "merlin/shaders/shader.h"
#include "merlin/graphics/material.h"
#include "merlin/memory/vao.h"
#include "merlin/graphics/renderableObject.h"

namespace Merlin {

	// MeshStats.h (or near your solver code)
	struct MeshStats {
		// counts
		size_t numVertices = 0;
		size_t numTriangles = 0;

		// bounds in mm
		glm::vec3 bbMin_mm{ +std::numeric_limits<float>::infinity() };
		glm::vec3 bbMax_mm{ -std::numeric_limits<float>::infinity() };
		glm::vec3 size_mm{ 0 };

		// geometry
		double surfaceArea_mm2 = 0.0;     // mm^2
		double volume_mm3 = 0.0;     // signed (use fabs for magnitude)
		glm::dvec3 centroid_mm{ 0.0 };      // volume-weighted

		// derived
		double mass_g = 0.0;     // if density provided
		double compactness = 0.0;     // 36pi V^2 / A^3  (1.0 = sphere)
		bool   seemsClosed = true;    // heuristic (see note)
	};

	class Mesh : public RenderableObject {
	public:
		Mesh(const std::string& name);
		Mesh(const std::string& name, GLsizei count, shared<VBO<>>, GLuint mode = GL_TRIANGLES);
		Mesh(const std::string& name, GLsizei count, shared<VBO<>>, shared<IBO> = nullptr, GLuint mode = GL_TRIANGLES);
		Mesh(const std::string& name, std::vector<Vertex>& vertices, GLuint mode = GL_TRIANGLES);
		Mesh(const std::string& name, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint mode = GL_TRIANGLES);
		Mesh(const std::string& name, VAO_Ptr, GLuint count, GLuint mode = GL_TRIANGLES);

		void draw() const;
		void drawInstanced(GLsizeiptr instanced) const;

		void voxelize(float size, bool sdf = false);
		void voxelizeSurface(float size, float thickness);

		glm::vec4 sdf(glm::vec3);
		void computeSDF(int threadCount = 1);

		MeshStats computeMeshStats(float unit_to_mm = 1.0f, double density_g_per_cm3 = 0.0);
		void computeBoundingBox();
		void swapNormals();
		void flipFace();
		void computeNormals();
		void smoothNormals();
		void calculateIndices();
		void removeUnusedVertices();
		void applyMeshTransform();
		void centerMeshOrigin();
		void updateVAO();

		inline void setDrawMode(GLuint mode) { m_drawMode = mode; }
		inline void setShader(shared<Shader> shader) { m_shader = shader; }
		inline void setMaterial(shared<MaterialBase> material) { m_material = material; }
		inline void setShader(std::string shaderName) { m_shaderName = shaderName; }
		inline void setMaterial(std::string materialName) { m_materialName = materialName; m_material = nullptr; }

		inline bool hasIndices() const { return m_indices.size() > 0; }
		inline bool hasShader() const { return m_shader != nullptr; }
		inline bool hasMaterial() const { return m_material != nullptr; }
		inline bool hasInvertedNormals() const { return m_swap_normals; }

		inline GLuint getDrawMode() const { return m_drawMode; }
		inline const std::vector<int>& getVoxels() const { return m_voxels;  }
		inline const std::vector<Vertex>& getVertices() const { return m_vertices;  }
		inline const std::vector<GLuint>& getIndices() const{ return m_indices; }
		inline const glm::mat4& getTransform() const { return m_transform; }
		inline const shared<Shader> getShader() const { return m_shader; }
		inline const std::string& getShaderName() const { return m_shaderName; }
		inline const shared<MaterialBase> getMaterial() const { return m_material; }
		inline const std::string& getMaterialName() const { return m_materialName; }
		inline const std::vector<glm::vec3>& getVoxelsPosition() const { return m_voxels_position; }
		inline const std::vector<glm::vec4>& getVoxelsSDF() const { return m_voxels_sdf; }


		inline bool hasBoundingBox() const { return m_hasBoundingBox;  }
		inline BoundingBox getBoundingBox() const { return m_bbox; }

		static shared<Mesh> create(std::string name);
		static shared<Mesh> create(std::string name, std::vector<Vertex>& vertices, GLuint mode = GL_TRIANGLES);
		static shared<Mesh> create(std::string name, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint mode = GL_TRIANGLES);

	private:
		VAO_Ptr m_vao;
		GLuint m_drawMode;
		bool m_swap_normals = false;

		GLuint m_elementCount = 0;
		std::vector<Vertex> m_vertices;
		std::vector<GLuint> m_indices;

		std::vector<int> m_voxels;
		std::vector<glm::vec3> m_voxels_position;
		std::vector<glm::vec4> m_voxels_sdf;

		bool m_hasBoundingBox = false;
		BoundingBox m_bbox = { glm::vec3(), glm::vec3() };

		std::string m_materialName = "default";
		std::string m_shaderName = "default";

		shared<MaterialBase> m_material = nullptr;
		shared<Shader> m_shader = nullptr;

		

		bool debugnormal = true;
	};

	typedef shared<Mesh> Mesh_Ptr;
}