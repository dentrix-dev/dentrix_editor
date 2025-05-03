#include "scene.h"

#include <pmp/io/io.h>
#include <pmp/surface_mesh.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "filehandler.h"
#include "utils.h"

// toDO: refactor later into a utils class
static void printMat4(const glm::mat4 &mat)
{
	for (int row = 0; row < 4; ++row) {
		std::cout << "[ ";
		for (int col = 0; col < 4; ++col) {
			std::cout << mat[col][row] << " ";
		}
		std::cout << "]\n";
	}
}

Scene::Scene() {}

Scene::Scene(std::vector<Mesh *> meshes)
{
	this->meshes = meshes;
}

void Scene::Draw(Shader *shader, Mesh *selectedMesh)
{
	for (int i = 0; i < meshes.size(); i++) {
		if (selectedMesh && meshes[i]->name == selectedMesh->name) {
			shader->setFloat("color", 0.8f);
		} else {
			shader->setFloat("color", 0.5f);
		}

		// Scaling needs to happen at 0,0
		// Translate the mesh to origin using its center, scale it, translate it back
		glm::mat4 meshfinalTransform = glm::translate(glm::mat4(1.0f), meshes[i]->center) *
		                               meshes[i]->scaleTransform *
		                               meshes[i]->directionalScaleTransform *
		                               glm::translate(glm::mat4(1.0f), -1.0f * meshes[i]->center);

		// Center the scene
		// In mainScene, this has no effect since the vertices are already centered on first load
		// In editScene, this centers the new mesh array
		meshfinalTransform = glm::translate(glm::mat4(1.0f), -1.0f * center) * meshfinalTransform;

		// Send per-mesh model matrix to the shader
		shader->setMatrix4("model", glm::value_ptr(meshfinalTransform));

		meshes[i]->draw();
	}
}

glm::vec3 Scene::getMeshesCenter()
{
	int numMeshes = meshes.size();
	glm::vec3 center(0.0f);
	for (int i = 0; i < numMeshes; i++) {
		center += 0.5f * (meshes[i]->aabb_max + meshes[i]->aabb_min);
	}
	return center / (float)numMeshes;
}

// PMP doesn't support loading multiple separate objects from .obj files
// Objects are loaded using a custom file loader and PMP meshes are constructed manually
std::vector<Mesh *> Scene::loadScene(std::string path, QOpenGLFunctions_3_3_Core *gl)
{
	std::vector<Mesh *> fileMeshes = FileHandler::readOBJ(path, gl);
	std::cout << fileMeshes.size() << std::endl;

	// Translate all meshes to the center of the scene
	glm::vec3 sceneCenter(0.0f);
	for (int i = 0; i < fileMeshes.size(); i++) {
		sceneCenter += fileMeshes[i]->center;
	}
	sceneCenter /= (float)fileMeshes.size();
	for (int i = 0; i < fileMeshes.size(); i++) {
		for (auto v : fileMeshes[i]->mesh.vertices()) {
			fileMeshes[i]->mesh.position(v) -= Utils::glmToPmpPoint(sceneCenter);
		}
		fileMeshes[i]->center -= sceneCenter;
		fileMeshes[i]->aabb_max -= sceneCenter;
		fileMeshes[i]->aabb_min -= sceneCenter;
		fileMeshes[i]->updateBoundingBoxBuffers();
		fileMeshes[i]->updateBuffers();
	}
	return fileMeshes;
}

bool Scene::RayTriangleIntersect(const glm::vec3 &rayOrigin, const glm::vec3 &rayDirection,
                                 const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2,
                                 float &outIntersectionDistance)
{
	const float EPSILON = 0.000001f;
	glm::vec3 edge1 = v1 - v0;
	glm::vec3 edge2 = v2 - v0;
	glm::vec3 h = glm::cross(rayDirection, edge2);
	float a = glm::dot(edge1, h);

	if (a > -EPSILON && a < EPSILON) return false;  // Ray is parallel to the triangle.

	float f = 1.0f / a;
	glm::vec3 s = rayOrigin - v0;
	float u = f * glm::dot(s, h);

	if (u < 0.0f || (u > 1.0f)) return false;

	glm::vec3 q = glm::cross(s, edge1);
	float v = f * glm::dot(rayDirection, q);

	if (v < 0.0f || (u + v > 1.0f)) return false;

	// At this stage we can compute t to find out where the intersection point is
	// on the ray.
	float t = f * glm::dot(edge2, q);

	// Ray intersection
	if (t > EPSILON) {
		outIntersectionDistance = t;
		return true;
	} else  // This means that there is a line intersection but not a ray intersection.
		return false;
}

bool Scene::Intersect(glm::vec3 ray_origin, glm::vec3 ray_direction, glm::mat4 ModelMatrix,
                      Mesh *&intersectedMesh)
{
	std::string meshName = "none";
	bool intersectionFound = false;
	float distance;
	float minDistance = 0.0f;
	for (int i = 0; i < meshes.size(); i++) {
		// Skip gingiv mesh
		if (meshes[i]->name == "tooth0") {
			continue;
		}
		if (meshes[i]->testRayOBBIntersection(ray_origin, ray_direction, ModelMatrix, distance)) {
			std::cout << meshes[i]->name << std::endl;
			if (!intersectionFound) {  // First intersection found
				intersectionFound = true;
				minDistance = distance;
				meshName = meshes[i]->name;
				intersectedMesh = meshes[i];
			} else {  // Check if new intersection is closer
				if (distance < minDistance) {
					minDistance = distance;
					meshName = meshes[i]->name;
					intersectedMesh = meshes[i];
				}
			}
		}
	}
	std::cout << "Closest mesh: " << meshName << std::endl;
	return intersectionFound;
}

bool Scene::IntersectTriangles(glm::vec3 ray_origin_world, glm::vec3 ray_direction_world,
                               glm::mat4 sceneMatrix,  // Base scene transform
                               Mesh *&outHitMesh, pmp::Vertex &outHitVertexIndex,
                               glm::vec3 &outIntersectionPointWorld)
{
	float closest_t = std::numeric_limits<float>::max();
	outHitMesh = nullptr;
	Mesh *closest_mesh = nullptr;
	pmp::Vertex closest_vertex_index;  // Store the index within the mesh->vertices array
	glm::vec3 closest_intersection_point_world;

	// Ensure ray direction is normalized
	ray_direction_world = glm::normalize(ray_direction_world);

	for (Mesh *mesh : meshes) {
		if (mesh->name == "tooth0") {
			continue;
		}

		// --- Construct the full Model Matrix for this mesh ---
		// This depends on how you handle transformations.
		// Example: Base scene transform * mesh-specific transforms
		// Adjust this according to your transformation hierarchy!
		glm::mat4 modelMatrix =
		    sceneMatrix * mesh->directionalScaleTransform * mesh->scaleTransform;  // Example order

		// --- 1. Optional but recommended: Coarse Bounding Box Check ---
		float obb_intersection_distance;
		if (!mesh->testRayOBBIntersection(ray_origin_world, ray_direction_world, modelMatrix,
		                                  obb_intersection_distance)) {
			continue;  // Skip this mesh if ray doesn't hit its OBB
		}
		// Optional refinement: If obb_intersection_distance > closest_t, we can
		// also skip. Be careful if the ray origin is inside the OBB.
		// TestRayOBBIntersection might need adjustment.

		// --- 2. Transform Ray into Model Space ---
		glm::mat4 invModelMatrix = glm::inverse(modelMatrix);
		glm::vec4 ray_origin_model_h = invModelMatrix * glm::vec4(ray_origin_world, 1.0f);
		glm::vec4 ray_direction_model_h =
		    invModelMatrix * glm::vec4(ray_direction_world, 0.0f);  // Direction needs 0.0

		glm::vec3 ray_origin_model = glm::vec3(ray_origin_model_h);
		// IMPORTANT: Re-normalize direction after non-uniform scaling/shear in
		// inverse transform
		glm::vec3 ray_direction_model = glm::normalize(glm::vec3(ray_direction_model_h));

		// --- 3. Iterate through Triangles ---
		bool mesh_hit = false;
		std::vector<glm::vec3> faceVertices = {};
		std::vector<pmp::Vertex> faceVerticesIndices = {};
		pmp::Vertex mesh_hit_v0_idx, mesh_hit_v1_idx, mesh_hit_v2_idx;
		float mesh_closest_t = std::numeric_limits<float>::max();
		glm::vec3 hit_v0, hit_v1, hit_v2;  // Keep track of hit triangle vertices

		for (auto f : mesh->mesh.faces()) {
			faceVertices.clear();
			faceVerticesIndices.clear();
			for (auto v : mesh->mesh.vertices(f)) {
				faceVertices.push_back(Utils::pmpPointToGlm(mesh->mesh.position(v)));
				faceVerticesIndices.push_back(v);
			}

			float t;
			// Use the MODEL SPACE ray and vertices
			if (RayTriangleIntersect(ray_origin_model, ray_direction_model, faceVertices[0],
			                         faceVertices[1], faceVertices[2], t)) {
				// We need the distance 't' relative to the *original world ray* to
				// compare across different meshes Transform the intersection point
				// found in model space back to world space
				glm::vec3 intersectionPointModel = ray_origin_model + ray_direction_model * t;
				glm::vec3 intersectionPointWorldTemp =
				    glm::vec3(modelMatrix * glm::vec4(intersectionPointModel, 1.0f));

				// Calculate distance from the world ray origin
				// Use distance squared for comparison to avoid sqrt, only do sqrt at
				// the end if needed Or more simply, recalculate 't' in world space
				// (though less robust if model matrix has non-uniform scale) A simpler
				// approximation (often good enough if transforms are rigid or uniform
				// scale): Use model-space 't' for comparison. Let's recalculate world
				// distance for accuracy:
				float world_t = glm::distance(ray_origin_world, intersectionPointWorldTemp);

				// Check if this intersection is closer than previous hits *for this
				// mesh* and closer than the overall closest hit found so far
				if (world_t < closest_t) {  // Found a new overall closest hit
					closest_t = world_t;
					closest_mesh = mesh;
					// Store the vertices and indices of this triangle
					mesh_hit_v0_idx = faceVerticesIndices[0];
					mesh_hit_v1_idx = faceVerticesIndices[1];
					mesh_hit_v2_idx = faceVerticesIndices[2];
					hit_v0 = faceVertices[0];  // Model space vertices
					hit_v1 = faceVertices[1];
					hit_v2 = faceVertices[2];
					closest_intersection_point_world =
					    intersectionPointWorldTemp;  // Store the exact world intersection
					                                 // point
					mesh_hit = true;                 // Mark that this mesh was hit
				}
			}
		}  // End triangle loop

		// --- 4. If this mesh contained the closest hit so far, find the closest
		// vertex ---
		if (mesh_hit && closest_mesh == mesh) {
			// We already stored closest_intersection_point_world when updating
			// closest_t

			// Transform the vertices of the *hit triangle* to world space
			glm::vec3 world_v0 = glm::vec3(modelMatrix * glm::vec4(hit_v0, 1.0f));
			glm::vec3 world_v1 = glm::vec3(modelMatrix * glm::vec4(hit_v1, 1.0f));
			glm::vec3 world_v2 = glm::vec3(modelMatrix * glm::vec4(hit_v2, 1.0f));

			// Calculate squared distances from the intersection point to each vertex
			float dist_sq_0 = glm::dot(world_v0 - closest_intersection_point_world,
			                           world_v0 - closest_intersection_point_world);
			float dist_sq_1 = glm::dot(world_v1 - closest_intersection_point_world,
			                           world_v1 - closest_intersection_point_world);
			float dist_sq_2 = glm::dot(world_v2 - closest_intersection_point_world,
			                           world_v2 - closest_intersection_point_world);

			// Find the minimum distance and store the corresponding original vertex
			// index
			if (dist_sq_0 <= dist_sq_1 && dist_sq_0 <= dist_sq_2) {
				closest_vertex_index = mesh_hit_v0_idx;
			} else if (dist_sq_1 <= dist_sq_0 && dist_sq_1 <= dist_sq_2) {
				closest_vertex_index = mesh_hit_v1_idx;
			} else {
				closest_vertex_index = mesh_hit_v2_idx;
			}
		}
	}  // End mesh loop

	// --- 5. Set output parameters and return ---
	if (closest_mesh != nullptr) {
		outHitMesh = closest_mesh;
		outHitVertexIndex = closest_vertex_index;
		outIntersectionPointWorld =
		    closest_intersection_point_world;  // Set the final intersection point
		return true;
	}

	return false;  // No intersection found
}

void Scene::GetNeighboringMeshNames(std::string meshName, std::string &neighbor1Name,
                                    std::string &neighbor2Name)
{
	// Upper jaw teeth ordering in local mapping (not universal mapping)
	std::string upperMeshNames[16] = {
	    "tooth16", "tooth15", "tooth14", "tooth13", "tooth12", "tooth11", "tooth10", "tooth9",
	    "tooth1",  "tooth2",  "tooth3",  "tooth4",  "tooth5",  "tooth6",  "tooth7",  "tooth8"};

	int upperMeshIndex;
	for (int i = 0; i < 16; i++) {
		if (upperMeshNames[i] == meshName) upperMeshIndex = i;
	}

	if (upperMeshIndex == 0) {
		neighbor2Name = upperMeshNames[upperMeshIndex + 1];
	} else if (upperMeshIndex == 15) {
		neighbor1Name = upperMeshNames[upperMeshIndex - 1];
	} else {
		neighbor1Name = upperMeshNames[upperMeshIndex - 1];
		neighbor2Name = upperMeshNames[upperMeshIndex + 1];
	}
}
