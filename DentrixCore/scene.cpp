#include <cstddef>
#include <iostream>
#include <fstream>
#include <sstream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "scene.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//toDO: refactor later into a utils class
static void printMat4(const glm::mat4& mat) {
    for (int row = 0; row < 4; ++row) {
        std::cout << "[ ";
        for (int col = 0; col < 4; ++col) {
            std::cout << mat[col][row] << " ";
        }
        std::cout << "]\n";
    }
}

Scene::Scene() {

}

Scene::Scene(std::vector<Mesh*> meshes) {
    this->meshes = meshes;
}

void Scene::Draw(Shader* shader, Mesh* selectedMesh) {
    for (int i=0; i<meshes.size(); i++) {
        if (selectedMesh && meshes[i]->name == selectedMesh->name) {
            shader->setFloat("color", 0.8f);
        } else {
            shader->setFloat("color", 0.5f);
        }

        // Scaling needs to happen at 0,0
        // Translate the mesh to origin using its center, scale it, translate it back
        glm::mat4 meshfinalTransform = glm::translate(glm::mat4(1.0f), meshes[i]->center) * meshes[i]->scaleTransform * meshes[i]->directionalScaleTransform * glm::translate(glm::mat4(1.0f), -1.0f * meshes[i]->center);

        // Center the scene
        // In mainScene, this has no effect since the vertices are already centered on first load
        // In editScene, this centers the new mesh array
        meshfinalTransform = glm::translate(glm::mat4(1.0f), -1.0f * center) * meshfinalTransform;

        // Send per-mesh model matrix to the shader
        shader->setMatrix4("model", glm::value_ptr(meshfinalTransform));

        meshes[i]->Draw();
    }
}

glm::vec3 Scene::getMeshesCenter() {
    int numMeshes = meshes.size();
    glm::vec3 center(0.0f);
    for (int i=0; i < numMeshes; i++) {
        center += 0.5f * (meshes[i]->aabb_max + meshes[i]->aabb_min);
    }
    return center / (float)numMeshes;
}

// PMP doesn't support loading multiple separate objects from .obj files
// Objects are loaded using Assimp and PMP meshes are constructed manually
std::vector<Mesh*> Scene::loadScene(std::string path, QOpenGLFunctions_3_3_Core* gl) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_GenBoundingBoxes);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
    }

    // Get scene center
    unsigned int numMeshes = scene->mNumMeshes;
    glm::vec3 center(0.0f);
    for (int i=0; i < numMeshes; i++) {
        const aiAABB &aabb = scene->mMeshes[i]->mAABB;
        glm::vec3 aabb_min = glm::vec3(aabb.mMin.x,aabb.mMin.y,aabb.mMin.z);
        glm::vec3 aabb_max = glm::vec3(aabb.mMax.x,aabb.mMax.y,aabb.mMax.z);
        center += 0.5f * (aabb_min + aabb_max);
    }
    center = center / (float)numMeshes;

    std::vector<Mesh*> meshes;
    for (int i=0; i<numMeshes; i++) {
        meshes.push_back(new Mesh(scene->mMeshes[i], center, gl));
    }
    return meshes;
}

bool Scene::TestRayOBBIntersection(
    glm::vec3 ray_origin,        // Ray origin, in world space
    glm::vec3 ray_direction,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.
    glm::vec3 aabb_min,          // Minimum X,Y,Z coords of the mesh when not transformed at all.
    glm::vec3 aabb_max,          // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.
    glm::mat4 ModelMatrix,       // Transformation applied to the mesh (which will thus be also applied to its bounding box)
    float& intersection_distance // Output : distance between ray_origin and the intersection with the OBB
    ){

    // Intersection method from Real-Time Rendering and Essential Mathematics for Games

    float tMin = 0.0f;
    float tMax = 100000.0f;

    glm::vec3 OBBposition_worldspace(ModelMatrix[3].x, ModelMatrix[3].y, ModelMatrix[3].z);

    glm::vec3 delta = OBBposition_worldspace - ray_origin;

    // Test intersection with the 2 planes perpendicular to the OBB's X axis
    {
        glm::vec3 xaxis(ModelMatrix[0].x, ModelMatrix[0].y, ModelMatrix[0].z);
        float e = glm::dot(xaxis, delta);
        float f = glm::dot(ray_direction, xaxis);

        if ( fabs(f) > 0.001f ){ // Standard case

            float t1 = (e+aabb_min.x)/f; // Intersection with the "left" plane
            float t2 = (e+aabb_max.x)/f; // Intersection with the "right" plane
            // t1 and t2 now contain distances betwen ray origin and ray-plane intersections

            // We want t1 to represent the nearest intersection,
            // so if it's not the case, invert t1 and t2
            if (t1>t2){
                float w=t1;t1=t2;t2=w; // swap t1 and t2
            }

            // tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
            if ( t2 < tMax )
                tMax = t2;
            // tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
            if ( t1 > tMin )
                tMin = t1;

            // And here's the trick :
            // If "far" is closer than "near", then there is NO intersection.
            // See the images in the tutorials for the visual explanation.
            if (tMax < tMin )
                return false;

        }else{ // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
            if(-e+aabb_min.x > 0.0f || -e+aabb_max.x < 0.0f)
                return false;
        }
    }

    // Test intersection with the 2 planes perpendicular to the OBB's Y axis
    // Exactly the same thing than above.
    {
        glm::vec3 yaxis(ModelMatrix[1].x, ModelMatrix[1].y, ModelMatrix[1].z);
        float e = glm::dot(yaxis, delta);
        float f = glm::dot(ray_direction, yaxis);

        if ( fabs(f) > 0.001f ){

            float t1 = (e+aabb_min.y)/f;
            float t2 = (e+aabb_max.y)/f;

            if (t1>t2){float w=t1;t1=t2;t2=w;}

            if ( t2 < tMax )
                tMax = t2;
            if ( t1 > tMin )
                tMin = t1;
            if (tMin > tMax)
                return false;

        }else{
            if(-e+aabb_min.y > 0.0f || -e+aabb_max.y < 0.0f)
                return false;
        }
    }

    // Test intersection with the 2 planes perpendicular to the OBB's Z axis
    // Exactly the same thing than above.
    {
        glm::vec3 zaxis(ModelMatrix[2].x, ModelMatrix[2].y, ModelMatrix[2].z);
        float e = glm::dot(zaxis, delta);
        float f = glm::dot(ray_direction, zaxis);

        if ( fabs(f) > 0.001f ){

            float t1 = (e+aabb_min.z)/f;
            float t2 = (e+aabb_max.z)/f;

            if (t1>t2){float w=t1;t1=t2;t2=w;}

            if ( t2 < tMax )
                tMax = t2;
            if ( t1 > tMin )
                tMin = t1;
            if (tMin > tMax)
                return false;

        }else{
            if(-e+aabb_min.z > 0.0f || -e+aabb_max.z < 0.0f)
                return false;
        }
    }

    intersection_distance = tMin;
    return true;
}

bool Scene::RayTriangleIntersect(const glm::vec3 &rayOrigin, const glm::vec3 &rayDirection, const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, float &outIntersectionDistance)
{
    const float EPSILON = 0.000001f;
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 h = glm::cross(rayDirection, edge2);
    float a = glm::dot(edge1, h);

    if (a > -EPSILON && a < EPSILON)
        return false; // Ray is parallel to the triangle.

    float f = 1.0f / a;
    glm::vec3 s = rayOrigin - v0;
    float u = f * glm::dot(s, h);

    if (u < 0.0f || (u > 1.0f))
        return false;

    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(rayDirection, q);

    if (v < 0.0f || (u + v > 1.0f))
        return false;

    // At this stage we can compute t to find out where the intersection point is on the ray.
    float t = f * glm::dot(edge2, q);

    if (t > EPSILON) // Ray intersection
    {
        outIntersectionDistance = t;
        return true;
    } else // This means that there is a line intersection but not a ray intersection.
        return false;
}

bool Scene::Intersect(glm::vec3 ray_origin, glm::vec3 ray_direction, glm::mat4 ModelMatrix, Mesh*& intersectedMesh) {
    std::string meshName = "none";
    bool intersectionFound = false;
    float distance;
    float minDistance = 0.0f;
    for (int i=0; i < meshes.size(); i++) {
        // Skip gingiv mesh
        if (meshes[i]->name == "tooth0") {
            continue;
        }
        if (TestRayOBBIntersection(ray_origin, ray_direction, meshes[i]->aabb_min, meshes[i]->aabb_max, ModelMatrix, distance)) {
            std::cout << meshes[i]->name << std::endl;
            if (!intersectionFound) { // First intersection found
                intersectionFound = true;
                minDistance = distance;
                meshName = meshes[i]->name;
                intersectedMesh = meshes[i];
            } else { // Check if new intersection is closer
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

bool Scene::IntersectTriangles(
    glm::vec3 ray_origin_world,
    glm::vec3 ray_direction_world,
    glm::mat4 sceneMatrix, // Base scene transform
    Mesh*& outHitMesh,
    unsigned int& outHitVertexIndex,
    glm::vec3& outIntersectionPointWorld
    ) {
    float closest_t = std::numeric_limits<float>::max();
    outHitMesh = nullptr;
    Mesh* closest_mesh = nullptr;
    unsigned int closest_vertex_index = 0; // Store the index within the mesh->vertices array
    glm::vec3 closest_intersection_point_world;

    // Ensure ray direction is normalized
    ray_direction_world = glm::normalize(ray_direction_world);

    for (Mesh* mesh : meshes) {
        if (mesh->name == "tooth0") {
            continue;
        }

        // --- Construct the full Model Matrix for this mesh ---
        // This depends on how you handle transformations.
        // Example: Base scene transform * mesh-specific transforms
        // Adjust this according to your transformation hierarchy!
        glm::mat4 modelMatrix = sceneMatrix * mesh->directionalScaleTransform * mesh->scaleTransform; // Example order

        // --- 1. Optional but recommended: Coarse Bounding Box Check ---
        float obb_intersection_distance;
        if (!TestRayOBBIntersection(ray_origin_world, ray_direction_world, mesh->aabb_min, mesh->aabb_max, modelMatrix, obb_intersection_distance)) {
            continue; // Skip this mesh if ray doesn't hit its OBB
        }
        // Optional refinement: If obb_intersection_distance > closest_t, we can also skip.
        // Be careful if the ray origin is inside the OBB. TestRayOBBIntersection might need adjustment.


        // --- 2. Transform Ray into Model Space ---
        glm::mat4 invModelMatrix = glm::inverse(modelMatrix);
        glm::vec4 ray_origin_model_h = invModelMatrix * glm::vec4(ray_origin_world, 1.0f);
        glm::vec4 ray_direction_model_h = invModelMatrix * glm::vec4(ray_direction_world, 0.0f); // Direction needs 0.0

        glm::vec3 ray_origin_model = glm::vec3(ray_origin_model_h);
        // IMPORTANT: Re-normalize direction after non-uniform scaling/shear in inverse transform
        glm::vec3 ray_direction_model = glm::normalize(glm::vec3(ray_direction_model_h));


        // --- 3. Iterate through Triangles ---
        bool mesh_hit = false;
        float mesh_closest_t = std::numeric_limits<float>::max();
        unsigned int mesh_hit_v0_idx = 0, mesh_hit_v1_idx = 0, mesh_hit_v2_idx = 0;
        glm::vec3 hit_v0, hit_v1, hit_v2; // Keep track of hit triangle vertices

        for (size_t i = 0; i < mesh->indices.size(); i += 3) {
            unsigned int idx0 = mesh->indices[i];
            unsigned int idx1 = mesh->indices[i + 1];
            unsigned int idx2 = mesh->indices[i + 2];

            const glm::vec3 v0 = glm::vec3(mesh->vertices[idx0*3], mesh->vertices[idx0*3+1], mesh->vertices[idx0*3+2]);
            const glm::vec3 v1 = glm::vec3(mesh->vertices[idx1*3], mesh->vertices[idx1*3+1], mesh->vertices[idx1*3+2]);
            const glm::vec3 v2 = glm::vec3(mesh->vertices[idx2*3], mesh->vertices[idx2*3+1], mesh->vertices[idx2*3+2]);

            float t;
            // Use the MODEL SPACE ray and vertices
            if (RayTriangleIntersect(ray_origin_model, ray_direction_model, v0, v1, v2, t)) {
                // We need the distance 't' relative to the *original world ray* to compare across different meshes
                // Transform the intersection point found in model space back to world space
                glm::vec3 intersectionPointModel = ray_origin_model + ray_direction_model * t;
                glm::vec3 intersectionPointWorldTemp = glm::vec3(modelMatrix * glm::vec4(intersectionPointModel, 1.0f));

                // Calculate distance from the world ray origin
                // Use distance squared for comparison to avoid sqrt, only do sqrt at the end if needed
                // Or more simply, recalculate 't' in world space (though less robust if model matrix has non-uniform scale)
                // A simpler approximation (often good enough if transforms are rigid or uniform scale): Use model-space 't' for comparison.
                // Let's recalculate world distance for accuracy:
                float world_t = glm::distance(ray_origin_world, intersectionPointWorldTemp);

                // Check if this intersection is closer than previous hits *for this mesh*
                // and closer than the overall closest hit found so far
                if (world_t < closest_t) { // Found a new overall closest hit
                    closest_t = world_t;
                    closest_mesh = mesh;
                    // Store the vertices and indices of this triangle
                    mesh_hit_v0_idx = idx0;
                    mesh_hit_v1_idx = idx1;
                    mesh_hit_v2_idx = idx2;
                    hit_v0 = v0; // Model space vertices
                    hit_v1 = v1;
                    hit_v2 = v2;
                    closest_intersection_point_world = intersectionPointWorldTemp; // Store the exact world intersection point
                    mesh_hit = true; // Mark that this mesh was hit
                }
            }
        } // End triangle loop

        // --- 4. If this mesh contained the closest hit so far, find the closest vertex ---
        if (mesh_hit && closest_mesh == mesh) {
            // We already stored closest_intersection_point_world when updating closest_t

            // Transform the vertices of the *hit triangle* to world space
            glm::vec3 world_v0 = glm::vec3(modelMatrix * glm::vec4(hit_v0, 1.0f));
            glm::vec3 world_v1 = glm::vec3(modelMatrix * glm::vec4(hit_v1, 1.0f));
            glm::vec3 world_v2 = glm::vec3(modelMatrix * glm::vec4(hit_v2, 1.0f));

            // Calculate squared distances from the intersection point to each vertex
            float dist_sq_0 = glm::dot(world_v0 - closest_intersection_point_world, world_v0 - closest_intersection_point_world);
            float dist_sq_1 = glm::dot(world_v1 - closest_intersection_point_world, world_v1 - closest_intersection_point_world);
            float dist_sq_2 = glm::dot(world_v2 - closest_intersection_point_world, world_v2 - closest_intersection_point_world);

            // Find the minimum distance and store the corresponding original vertex index
            if (dist_sq_0 <= dist_sq_1 && dist_sq_0 <= dist_sq_2) {
                closest_vertex_index = mesh_hit_v0_idx;
            } else if (dist_sq_1 <= dist_sq_0 && dist_sq_1 <= dist_sq_2) {
                closest_vertex_index = mesh_hit_v1_idx;
            } else {
                closest_vertex_index = mesh_hit_v2_idx;
            }
        }
    } // End mesh loop

    // --- 5. Set output parameters and return ---
    if (closest_mesh != nullptr) {
        outHitMesh = closest_mesh;
        outHitVertexIndex = closest_vertex_index;
        outIntersectionPointWorld = closest_intersection_point_world; // Set the final intersection point
        // Debug output (optional)
        // std::cout << "Hit Mesh: " << outHitMesh->name
        //           << ", Vertex Index: " << outHitVertexIndex
        //           << ", Intersection Point: (" << outIntersectionPointWorld.x << ", "
        //           << outIntersectionPointWorld.y << ", " << outIntersectionPointWorld.z << ")" << std::endl;
        return true;
    }

    return false; // No intersection found
}

void Scene::GetNeighboringMeshNames(std::string meshName, std::string &neighbor1Name, std::string &neighbor2Name)
{
    // Upper jaw teeth ordering in local mapping (not universal mapping)
    std::string upperMeshNames[16] = {"tooth16", "tooth15", "tooth14", "tooth13", "tooth12", "tooth11", "tooth10", "tooth9", "tooth1", "tooth2", "tooth3", "tooth4", "tooth5", "tooth6", "tooth7", "tooth8"};

    int upperMeshIndex;
    for (int i=0; i<16; i++) {
        if (upperMeshNames[i] == meshName)
            upperMeshIndex = i;
    }

    if (upperMeshIndex == 0) {
        neighbor2Name = upperMeshNames[upperMeshIndex+1];
    } else if (upperMeshIndex == 15) {
        neighbor1Name = upperMeshNames[upperMeshIndex-1];
    } else {
        neighbor1Name = upperMeshNames[upperMeshIndex-1];
        neighbor2Name = upperMeshNames[upperMeshIndex+1];
    }
}








