#include "mesh.h"

#include <pmp/algorithms/differential_geometry.h>
#include <pmp/algorithms/normals.h>
#include <pmp/algorithms/utilities.h>
#include <pmp/bounding_box.h>
#include <pmp/surface_mesh.h>
#include <utils.h>

#include <vector>

#include "pmp/algorithms/fairing.h"

bool Mesh::drawBoundingBox = true;

Mesh::Mesh(pmp::SurfaceMesh& input_mesh, std::string name, QOpenGLFunctions_3_3_Core* gl)
{
	this->gl = gl;
	this->name = name;
	mesh = input_mesh;

	// Calculate per-face normals for vertices
	// (vertex has a normal for each face it's a part of)
	pmp::face_normals(mesh);

	// Calculate mesh bounding box and center
	pmp::BoundingBox aabb = pmp::bounds(mesh);
	aabb_min = Utils::pmpPointToGlm(aabb.min());
	aabb_max = Utils::pmpPointToGlm(aabb.max());
	center = 0.5f * (aabb_min + aabb_max);

	// Update bounding box
	// aabb_min -= center;
	// aabb_max -= center;

	setup();
	setupBoundingBox();
}

Mesh::~Mesh() {}

void Mesh::setup()
{
	auto vpos = mesh.vertex_property<pmp::Point>("v:point");
	auto fnormal = mesh.face_property<pmp::Normal>("f:normal");

	std::vector<float> vertices = {};
	std::vector<float> normals = {};
	std::vector<unsigned int> indices = {};

	unsigned int index = 0;
	for (auto f : mesh.faces()) {
		pmp::Normal n = fnormal[f];
		std::vector<unsigned int> faceIndices;

		for (auto v : mesh.vertices(f)) {
			pmp::Point p = vpos[v];

			vertices.push_back(p[0]);
			vertices.push_back(p[1]);
			vertices.push_back(p[2]);

			normals.push_back(n[0]);
			normals.push_back(n[1]);
			normals.push_back(n[2]);

			faceIndices.push_back(index++);
		}

		if (faceIndices.size() == 3) {
			indices.push_back(faceIndices[0]);
			indices.push_back(faceIndices[1]);
			indices.push_back(faceIndices[2]);
		} else {
			qWarning() << "Non-triangular face detected!";
		}
	}
	numIndices = indices.size();

	gl->glGenVertexArrays(1, &VAO);
	gl->glGenBuffers(1, &VBO_pos);
	gl->glGenBuffers(1, &VBO_norm);
	gl->glGenBuffers(1, &EBO);

	gl->glBindVertexArray(VAO);

	// --- Position VBO (location = 0)
	gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
	gl->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(),
	                 GL_STATIC_DRAW);
	gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	gl->glEnableVertexAttribArray(0);

	// --- Normal VBO (location = 1)
	gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_norm);
	gl->glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(),
	                 GL_STATIC_DRAW);
	gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	gl->glEnableVertexAttribArray(1);

	// --- Element Buffer
	gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), indices.data(),
	                 GL_STATIC_DRAW);

	gl->glBindVertexArray(0);
}

void Mesh::setupBoundingBox()
{
	// 1. Calculate the 8 vertices of the bounding box
	std::vector<glm::vec3> bb_vertices;
	bb_vertices.resize(8);

	bb_vertices[0] = glm::vec3(aabb_min.x, aabb_min.y, aabb_min.z);  // 0 --- Near bottom left
	bb_vertices[1] = glm::vec3(aabb_max.x, aabb_min.y, aabb_min.z);  // 1 --= Near bottom right
	bb_vertices[2] = glm::vec3(aabb_max.x, aabb_max.y, aabb_min.z);  // 2 -=- Near top right
	bb_vertices[3] = glm::vec3(aabb_min.x, aabb_max.y, aabb_min.z);  // 3 --- Near top left
	bb_vertices[4] = glm::vec3(aabb_min.x, aabb_min.y, aabb_max.z);  // 4 --- Far bottom left
	bb_vertices[5] = glm::vec3(aabb_max.x, aabb_min.y, aabb_max.z);  // 5 --= Far bottom right
	bb_vertices[6] = glm::vec3(aabb_max.x, aabb_max.y, aabb_max.z);  // 6 -=- Far top right
	bb_vertices[7] = glm::vec3(aabb_min.x, aabb_max.y, aabb_max.z);  // 7 --- Far top left

	// 2. Define the indices for the 12 lines (edges) of the bounding box
	// Each pair of indices defines one line segment.
	std::vector<unsigned int> bb_indices = {// Bottom face
	                                        0, 1, 1, 2, 2, 3, 3, 0,
	                                        // Top face
	                                        4, 5, 5, 6, 6, 7, 7, 4,
	                                        // Connecting edges
	                                        0, 4, 1, 5, 2, 6, 3, 7};
	// Total 24 indices for 12 lines

	// 3. Generate and bind OpenGL objects
	gl->glGenVertexArrays(1, &VAO_BB);
	gl->glGenBuffers(1, &VBO_BB);
	gl->glGenBuffers(1, &EBO_BB);

	gl->glBindVertexArray(VAO_BB);

	// 4. Buffer vertex data (only positions)
	gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_BB);
	// Use bb_vertices data and size. Note the size calculation uses sizeof(glm::vec3)
	gl->glBufferData(GL_ARRAY_BUFFER, bb_vertices.size() * sizeof(glm::vec3), bb_vertices.data(),
	                 GL_STATIC_DRAW);

	// 5. Buffer index data
	gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_BB);
	// Use bb_indices data and size
	gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, bb_indices.size() * sizeof(unsigned int),
	                 bb_indices.data(), GL_STATIC_DRAW);

	// 6. Set up vertex attributes (only position)
	gl->glEnableVertexAttribArray(0);
	gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	// 7. Unbind VAO (good practice)
	gl->glBindVertexArray(0);

	// Optional: Unbind VBO and EBO after VAO is unbound. VAO remembers these bindings.
	// gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
	// gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Fills a hole in the mesh, defined by a halfedge that lies on the hole boundary
// Creates vertex in the middle of the hole, connects it to hole boundary vertices and smooths them
void Mesh::fillHole(pmp::Halfedge h)
{
	pmp::Point centerPoint;
	std::vector<pmp::Vertex> holeVertices;

	assert(mesh.is_boundary(h));  // Make sure it's a boundary halfedge

	// Find the vertices on the hole boundary
	pmp::Halfedge start = h;
	pmp::Halfedge he = start;
	do {
		pmp::Vertex v = mesh.from_vertex(he);
		holeVertices.push_back(v);
		he = mesh.next_halfedge(he);
	} while (he != start);

	// Find the center of the hole
	for (pmp::Vertex v : holeVertices) {
		centerPoint += mesh.position(v);
	}
	centerPoint /= (float)holeVertices.size();

	// Create new vertex and connect it to hole boundary vertices
	pmp::Vertex centerVertex = mesh.add_vertex(centerPoint);
	for (int i = 0; i < holeVertices.size() - 1; i++) {
		std::vector<pmp::Vertex> face = {centerVertex, holeVertices[i], holeVertices[i + 1]};
		mesh.add_face(face);
	}
	std::vector<pmp::Vertex> face = {holeVertices[0], centerVertex,
	                                 holeVertices[holeVertices.size() - 1]};
	mesh.add_face(face);

	// Smooth boundary vertices to avoid jaggy sharp faces
	auto vselected = mesh.add_vertex_property<bool>("v:selected");
	for (pmp::Vertex v : holeVertices) vselected[v] = true;
	pmp::fair(mesh, 1);

	pmp::face_normals(mesh);
	updateBuffers();
}

void Mesh::updateBoundingBoxBuffers()
{
	std::vector<glm::vec3> bb_vertices;
	bb_vertices.resize(8);

	bb_vertices[0] = glm::vec3(aabb_min.x, aabb_min.y, aabb_min.z);  // 0 --- Near bottom left
	bb_vertices[1] = glm::vec3(aabb_max.x, aabb_min.y, aabb_min.z);  // 1 --= Near bottom right
	bb_vertices[2] = glm::vec3(aabb_max.x, aabb_max.y, aabb_min.z);  // 2 -=- Near top right
	bb_vertices[3] = glm::vec3(aabb_min.x, aabb_max.y, aabb_min.z);  // 3 --- Near top left
	bb_vertices[4] = glm::vec3(aabb_min.x, aabb_min.y, aabb_max.z);  // 4 --- Far bottom left
	bb_vertices[5] = glm::vec3(aabb_max.x, aabb_min.y, aabb_max.z);  // 5 --= Far bottom right
	bb_vertices[6] = glm::vec3(aabb_max.x, aabb_max.y, aabb_max.z);  // 6 -=- Far top right
	bb_vertices[7] = glm::vec3(aabb_min.x, aabb_max.y, aabb_max.z);  // 7 --- Far top left

	gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_BB);
	// Use bb_vertices data and size. Note the size calculation uses sizeof(glm::vec3)
	gl->glBufferData(GL_ARRAY_BUFFER, bb_vertices.size() * sizeof(glm::vec3), bb_vertices.data(),
	                 GL_STATIC_DRAW);
	gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::updateVerticesBuffer()
{
	auto vpos = mesh.vertex_property<pmp::Point>("v:point");
	std::vector<float> vertices;
	vertices.reserve(mesh.n_faces() * 3 * 3);

	for (auto f : mesh.faces()) {
		for (auto v : mesh.vertices(f)) {
			pmp::Point p = vpos[v];
			vertices.push_back(p[0]);
			vertices.push_back(p[1]);
			vertices.push_back(p[2]);
		}
	}
	// Update position buffer
	gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
	gl->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), nullptr,
	                 GL_DYNAMIC_DRAW);  // orphan old buffer
	gl->glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
	gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::updateBuffers()
{
	auto vpos = mesh.vertex_property<pmp::Point>("v:point");
	auto fnormal = mesh.face_property<pmp::Normal>("f:normal");

	std::vector<float> vertices = {};
	std::vector<float> normals = {};
	std::vector<unsigned int> indices = {};

	unsigned int index = 0;
	for (auto f : mesh.faces()) {
		pmp::Normal n = fnormal[f];
		std::vector<unsigned int> faceIndices;

		for (auto v : mesh.vertices(f)) {
			pmp::Point p = vpos[v];

			vertices.push_back(p[0]);
			vertices.push_back(p[1]);
			vertices.push_back(p[2]);

			normals.push_back(n[0]);
			normals.push_back(n[1]);
			normals.push_back(n[2]);

			faceIndices.push_back(index++);
		}

		if (faceIndices.size() == 3) {
			indices.push_back(faceIndices[0]);
			indices.push_back(faceIndices[1]);
			indices.push_back(faceIndices[2]);
		} else {
			qWarning() << "Non-triangular face detected!";
		}
	}
	numIndices = indices.size();

	// Update position buffer
	gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
	gl->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(),
	                 GL_DYNAMIC_DRAW);

	// Update normal buffer
	gl->glBindBuffer(GL_ARRAY_BUFFER, VBO_norm);
	gl->glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(),
	                 GL_DYNAMIC_DRAW);

	// Update indices buffer
	gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(),
	                 GL_DYNAMIC_DRAW);

	// Unbind buffers
	gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::draw()
{
	gl->glBindVertexArray(VAO);
	gl->glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
	gl->glBindVertexArray(0);

	if (drawBoundingBox) {
		gl->glBindVertexArray(VAO_BB);
		// Draw 12 lines using the 24 indices provided
		gl->glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
		gl->glBindVertexArray(0);
	}
}

void Mesh::setScale(float scaleFactor)
{
	currentScale = scaleFactor;
	scaleTransform = glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor, scaleFactor, 1.0f));
}

void Mesh::setScaleDirectional(float x, float y, float z)
{
	directionalScaleTransform = glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z));
}

void Mesh::updateMeshScale(glm::vec3 sceneCenter)
{
	pmp::Point center = pmp::centroid(mesh);
	// pmp::Point center = Utils::glmToPmpPoint(sceneCenter);
	// Scale vertices around center
	for (auto v : mesh.vertices()) {
		pmp::Point p = mesh.position(v);
		p = center + (p - center) * currentScale;
		mesh.position(v) = p;
	}
	pmp::face_normals(mesh);
	pmp::vertex_normals(mesh);
	scaleTransform = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	updateBuffers();
}

bool Mesh::testRayOBBIntersection(
    glm::vec3 ray_origin,     // Ray origin, in world space
    glm::vec3 ray_direction,  // Ray direction (NOT target position!), in world space. Must be
                              // normalize()'d.
    glm::mat4 ModelMatrix,    // Transformation applied to the mesh (which will thus be also applied
                              // to its bounding box)
    float& intersection_distance  // Output : distance between ray_origin and the intersection with
                                  // the OBB
)
{
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

		if (fabs(f) > 0.001f) {               // Standard case
			float t1 = (e + aabb_min.x) / f;  // Intersection with the "left" plane
			float t2 = (e + aabb_max.x) / f;  // Intersection with the "right" plane
			// t1 and t2 now contain distances betwen ray origin and ray-plane intersections

			// We want t1 to represent the nearest intersection,
			// so if it's not the case, invert t1 and t2
			if (t1 > t2) {
				float w = t1;
				t1 = t2;
				t2 = w;  // swap t1 and t2
			}

			// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
			if (t2 < tMax) tMax = t2;
			// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
			if (t1 > tMin) tMin = t1;

			// If "far" is closer than "near", then there is NO intersection.
			if (tMax < tMin) return false;

		} else {  // Rare case : the ray is almost parallel to the planes, so they don't have any
			      // "intersection"
			if (-e + aabb_min.x > 0.0f || -e + aabb_max.x < 0.0f) return false;
		}
	}

	// Test intersection with the 2 planes perpendicular to the OBB's Y axis
	// Exactly the same thing than above.
	{
		glm::vec3 yaxis(ModelMatrix[1].x, ModelMatrix[1].y, ModelMatrix[1].z);
		float e = glm::dot(yaxis, delta);
		float f = glm::dot(ray_direction, yaxis);

		if (fabs(f) > 0.001f) {
			float t1 = (e + aabb_min.y) / f;
			float t2 = (e + aabb_max.y) / f;

			if (t1 > t2) {
				float w = t1;
				t1 = t2;
				t2 = w;
			}

			if (t2 < tMax) tMax = t2;
			if (t1 > tMin) tMin = t1;
			if (tMin > tMax) return false;

		} else {
			if (-e + aabb_min.y > 0.0f || -e + aabb_max.y < 0.0f) return false;
		}
	}

	// Test intersection with the 2 planes perpendicular to the OBB's Z axis
	// Exactly the same thing than above.
	{
		glm::vec3 zaxis(ModelMatrix[2].x, ModelMatrix[2].y, ModelMatrix[2].z);
		float e = glm::dot(zaxis, delta);
		float f = glm::dot(ray_direction, zaxis);

		if (fabs(f) > 0.001f) {
			float t1 = (e + aabb_min.z) / f;
			float t2 = (e + aabb_max.z) / f;

			if (t1 > t2) {
				float w = t1;
				t1 = t2;
				t2 = w;
			}

			if (t2 < tMax) tMax = t2;
			if (t1 > tMin) tMin = t1;
			if (tMin > tMax) return false;

		} else {
			if (-e + aabb_min.z > 0.0f || -e + aabb_max.z < 0.0f) return false;
		}
	}

	intersection_distance = tMin;
	return true;
}
