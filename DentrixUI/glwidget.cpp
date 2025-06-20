#include "glwidget.h"

#include <mcg/dental/arch_alignment.h>
#include <mcg/dental/segmentation.h>
#include <mcg/mesh_utils.h>
#include <pmp/io/io.h>
#include <pmp/surface_mesh.h>
#include <qnamespace.h>

#include <QThread>
#include <chrono>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>

#include "Gizmo/gizmo.h"
#include "Gizmo/gizmoComponent.h"
#include "glm/fwd.hpp"
#include "mainwindow.h"
#include "pmp/algorithms/differential_geometry.h"
#include "pmp/algorithms/utilities.h"
#include "pmp/mat_vec.h"
#include "scene.h"
#include "shader.h"
#include "utils.h"

GLWidget::GLWidget(QWidget *parent, std::string path, int jaw_index)
    : QOpenGLWidget(parent), initialFilePath(path), jaw_index(jaw_index)
{
    setFocusPolicy(Qt::StrongFocus);
    // loadUpperJawAsync(QString::fromStdString(path));
    // loadUpperJaw(path);
}

GLWidget::~GLWidget() {}

void GLWidget::loadModel(const std::string &path)
{
    std::cout << __func__ << std::endl;
    makeCurrent();

    // Delete references to previous meshes
    for (int i = 0; i < meshes.size(); i++) delete meshes[i];

    meshes = Scene::loadScene(path);

    mainScene = Scene(meshes);
    currentScene = &mainScene;
    selectedMesh = nullptr;
    update();  // Refresh the OpenGL view
}

void GLWidget::loadUpperJaw(const std::string &path)
{
    using clock = std::chrono::high_resolution_clock;
    auto startTotal = clock::now();

    auto t0 = clock::now();
    // opengl
    makeCurrent();
    auto t1 = clock::now();

    // Delete previous meshes
    for (int i = 0; i < upperJawMeshes.size(); i++) delete upperJawMeshes[i];
    // opengl
    upperJawMeshes.clear();
    auto t2 = clock::now();

    // Read mesh and label files
    // non-opengl
    upperJawLabelsPath = path.substr(0, path.size() - 4) + ".txt";
    upperJawUnsegmented = pmp::read(path, &upperRemap);
    auto t3 = clock::now();

    // Calculate jaw center
    pmp::Point jawCenter(0.0f);
    int noOfMeshes = 0;

    // Segment jaw and teeth
    upperArch = mcg::arch_segment(upperJawUnsegmented, upperJawLabelsPath.c_str(), upperRemap);
    auto t4 = clock::now();

    // Push gum to mesh array
    pmp::SurfaceMesh upperGum = mcg::mesh_extract(upperJawUnsegmented, upperArch.gum_faces);
    upperJawMeshes.push_back(new Mesh(upperGum, 0));

    pmp::BoundingBox aabb = pmp::bounds(upperGum);
    jawCenter += 0.5f * (aabb.max() + aabb.min());
    noOfMeshes++;
    auto t5 = clock::now();

    for (int idx = 0; idx < upperArch.teeth.size() && idx < 16; idx++) {
        if (upperArch.teeth[idx].is_present) {
            pmp::SurfaceMesh tooth = mcg::mesh_extract(upperJawUnsegmented, upperArch.teeth[idx].faces);
            upperJawMeshes.push_back(new Mesh(tooth, idx + 1));
            aabb = pmp::bounds(tooth);
            jawCenter += 0.5f * (aabb.max() + aabb.min());
            noOfMeshes++;
        }
    }
    auto t6 = clock::now();

    // Translate jaw to center
    jawCenter /= (float)noOfMeshes;
    for (pmp::Vertex v : upperGum.vertices()) upperGum.position(v) -= jawCenter;
    for (Mesh *m : upperJawMeshes) {
        for (pmp::Vertex v : m->surfaceMesh.vertices()) m->surfaceMesh.position(v) -= jawCenter;
        m->updateBuffers();
        m->recalculateBoundingBox();
        m->updateBoundingBoxBuffers();
    }
    auto t7 = clock::now();

    upperJawLoaded = true;
    rebuildMainScene();
    update();
    auto t8 = clock::now();

    // Timing prints
    auto ms = [](auto d) { return std::chrono::duration_cast<std::chrono::milliseconds>(d).count(); };

    std::cout << "Timing (ms):\n";
    std::cout << "  makeCurrent:       " << ms(t1 - t0) << "\n";
    std::cout << "  clear meshes:      " << ms(t2 - t1) << "\n";
    std::cout << "  read mesh:         " << ms(t3 - t2) << "\n";
    std::cout << "  segment:           " << ms(t4 - t3) << "\n";
    std::cout << "  extract gum:       " << ms(t5 - t4) << "\n";
    std::cout << "  extract teeth:     " << ms(t6 - t5) << "\n";
    std::cout << "  center + buffers:  " << ms(t7 - t6) << "\n";
    std::cout << "  rebuild + update:  " << ms(t8 - t7) << "\n";
    std::cout << "  Total:             " << ms(t8 - startTotal) << "\n";
}

void GLWidget::loadUpperJawAsync(const QString &path)
{
    auto *thread = new QThread;
    auto *worker = new JawLoaderWorker(path);

    worker->moveToThread(thread);
    std::cout << "created worker thread" << std::endl;

    connect(thread, &QThread::started, worker, &JawLoaderWorker::run);
    connect(worker, &JawLoaderWorker::finished, this, [this, worker, thread](const JawLoadResult &result) {
        applyUpperJawResult(result);  // Now run on main thread
        worker->deleteLater();
        thread->quit();
        thread->deleteLater();
    });
    std::cout << "about to start worker thread" << std::endl;
    thread->start();
    std::cout << "started worker thread" << std::endl;
}

void GLWidget::applyUpperJawResult(const JawLoadResult &result)
{
    makeCurrent();

    this->upperRemap = result.remap;
    this->upperJawUnsegmented = result.upperJawUnsegmented;
    this->upperArch = result.arch;

    // Clear previous
    for (auto *m : upperJawMeshes) delete m;
    upperJawMeshes.clear();

    pmp::Point center = result.center;
    if (lowerJawLoaded) {
        center = pmp::Point(center[0] + 80.0f, center[1], center[2]);
    }

    // Center gum
    pmp::SurfaceMesh gum = result.gum;
    for (pmp::Vertex v : gum.vertices()) gum.position(v) -= center;

    // Compute connected components (adds v:component property)
    int num_components = connected_components(gum);
    std::cout << "Found " << num_components << " connected components\n";

    // Access vertex component property
    auto vcomp = gum.vertex_property<int>("v:component");

    // Count number of vertices in each component
    std::vector<int> component_sizes(num_components, 0);
    for (auto v : gum.vertices()) {
        component_sizes[vcomp[v]]++;
    }

    // Find the largest component
    int largest_component =
        std::distance(component_sizes.begin(), std::max_element(component_sizes.begin(), component_sizes.end()));
    std::cout << "largest component: " << largest_component << std::endl;
    std::cout << "array start" << std::endl;
    for (int i : component_sizes) std::cout << i << std::endl;
    std::cout << "array end" << std::endl;

    for (pmp::Vertex v : gum.vertices()) {
        if (vcomp[v] != largest_component) gum.delete_vertex(v);
    }
    gum.garbage_collection();

    std::cout << gum.n_vertices() << std::endl;
    int bounds = 0;
    for (pmp::Halfedge h : gum.halfedges()) {
        if (gum.is_boundary(h)) {
            bounds++;
        }
    }
    std::cout << "Number of bounds: " << bounds << std::endl;

    upperJawMeshes.push_back(new Mesh(gum, 0));
    // Fill gum holes
    // upperJawMeshes[0]->fillHoles();

    // Center teeth
    for (int i = 0; i < result.teeth.size(); i++) {
        const auto &mesh = result.teeth[i].first;
        pmp::SurfaceMesh m = mesh;
        for (pmp::Vertex v : m.vertices()) m.position(v) -= center;

        upperJawMeshes.push_back(new Mesh(m, i + 1));
    }

    for (Mesh *m : upperJawMeshes) {
        m->updateBuffers();
        m->recalculateBoundingBox();
        m->updateBoundingBoxBuffers();
    }

    std::cout << "upper jaw meshes size from applyUpperJawResults" << upperJawMeshes.size() << std::endl;
    std::cout << "number of faces, vertices" << upperJawUnsegmented.n_faces() << " , "
              << upperJawUnsegmented.n_vertices() << std::endl;
    upperJawLoaded = true;
    rebuildMainScene();
    update();
}

void GLWidget::applyLowerJawResult(const JawLoadResult &result)
{
    makeCurrent();

    this->lowerRemap = result.remap;
    this->lowerJawUnsegmented = result.upperJawUnsegmented;
    this->lowerArch = result.arch;

    // Clear previous
    for (auto *m : lowerJawMeshes) delete m;
    lowerJawMeshes.clear();

    pmp::Point center = result.center;
    if (upperJawLoaded) {
        center = pmp::Point(center[0] - 80.0f, center[1], center[2]);
    }

    // Center gum
    pmp::SurfaceMesh gum = result.gum;
    for (pmp::Vertex v : gum.vertices()) gum.position(v) -= center;

    // Compute connected components (adds v:component property)
    int num_components = connected_components(gum);
    std::cout << "Found " << num_components << " connected components\n";

    // Access vertex component property
    auto vcomp = gum.vertex_property<int>("v:component");

    // Count number of vertices in each component
    std::vector<int> component_sizes(num_components, 0);
    for (auto v : gum.vertices()) {
        component_sizes[vcomp[v]]++;
    }

    // Find the largest component
    int largest_component =
        std::distance(component_sizes.begin(), std::max_element(component_sizes.begin(), component_sizes.end()));
    std::cout << "largest component: " << largest_component << std::endl;
    std::cout << "array start" << std::endl;
    for (int i : component_sizes) std::cout << i << std::endl;
    std::cout << "array end" << std::endl;

    for (pmp::Vertex v : gum.vertices()) {
        if (vcomp[v] != largest_component) gum.delete_vertex(v);
    }
    gum.garbage_collection();

    std::cout << gum.n_vertices() << std::endl;
    int bounds = 0;
    for (pmp::Halfedge h : gum.halfedges()) {
        if (gum.is_boundary(h)) {
            bounds++;
        }
    }
    std::cout << "Number of bounds: " << bounds << std::endl;

    lowerJawMeshes.push_back(new Mesh(gum, 0));

    // Fill gum holes
    // lowerJawMeshes[0]->fillHoles();

    // Center teeth
    for (int i = 0; i < result.teeth.size(); i++) {
        const auto &mesh = result.teeth[i].first;
        pmp::SurfaceMesh m = mesh;
        for (pmp::Vertex v : m.vertices()) m.position(v) -= center;

        lowerJawMeshes.push_back(new Mesh(m, i + 17));
    }

    for (Mesh *m : lowerJawMeshes) {
        m->updateBuffers();
        m->recalculateBoundingBox();
        m->updateBoundingBoxBuffers();
    }

    std::cout << "upper jaw meshes size from applyUpperJawResults" << upperJawMeshes.size() << std::endl;
    std::cout << "number of faces, vertices" << upperJawUnsegmented.n_faces() << " , "
              << upperJawUnsegmented.n_vertices() << std::endl;
    lowerJawLoaded = true;
    rebuildMainScene();
    update();
}

void GLWidget::loadLowerJaw(const std::string &path)
{
    makeCurrent();
    // Delete previous meshes
    for (int i = 0; i < lowerJawMeshes.size(); i++) delete lowerJawMeshes[i];
    lowerJawMeshes.clear();

    // Read mesh and label files
    lowerJawLabelsPath = path.substr(0, path.size() - 4) + ".txt";
    lowerJawUnsegmented = pmp::read(path, &lowerRemap);

    // Calculate jaw center
    pmp::Point jawCenter(0.0f);
    int noOfMeshes = 0;

    // Segment jaw and teeth
    lowerArch = mcg::arch_segment(lowerJawUnsegmented, lowerJawLabelsPath.c_str(), lowerRemap);

    // Push gum to mesh array
    pmp::SurfaceMesh lowerGum = mcg::mesh_extract(lowerJawUnsegmented, lowerArch.gum_faces);
    lowerJawMeshes.push_back(new Mesh(lowerGum, 33));

    pmp::BoundingBox aabb = pmp::bounds(lowerGum);
    jawCenter += 0.5f * (aabb.max() + aabb.min());
    noOfMeshes++;

    // Push teeth to mesh array
    for (int idx = 0; idx < lowerArch.teeth.size() && idx < 16; idx++) {
        if (lowerArch.teeth[idx].is_present) {
            pmp::SurfaceMesh tooth = mcg::mesh_extract(lowerJawUnsegmented, lowerArch.teeth[idx].faces);
            lowerJawMeshes.push_back(new Mesh(tooth, idx + 17));
            aabb = pmp::bounds(tooth);
            jawCenter += 0.5f * (aabb.max() + aabb.min());
            noOfMeshes++;
        }
    }

    // Translate jaw to center
    jawCenter /= (float)noOfMeshes;
    for (pmp::Vertex v : lowerGum.vertices()) lowerGum.position(v) -= jawCenter;
    for (Mesh *m : lowerJawMeshes) {
        for (pmp::Vertex v : m->surfaceMesh.vertices()) m->surfaceMesh.position(v) -= jawCenter;
        m->updateBuffers();
        m->recalculateBoundingBox();
        m->updateBoundingBoxBuffers();
    }

    std::cout << " from lower jaw load number of faces, vertices" << upperJawUnsegmented.n_faces() << " , "
              << upperJawUnsegmented.n_vertices() << std::endl;

    lowerJawLoaded = true;
    rebuildMainScene();
    update();
}

// todo: merge both into one async load
void GLWidget::loadLowerJawAsync(const QString &path)
{
    auto *thread = new QThread;
    auto *worker = new JawLoaderWorker(path);

    worker->moveToThread(thread);
    std::cout << "created worker thread" << std::endl;

    connect(thread, &QThread::started, worker, &JawLoaderWorker::run);
    connect(worker, &JawLoaderWorker::finished, this, [this, worker, thread](const JawLoadResult &result) {
        applyLowerJawResult(result);  // Now run on main thread
        worker->deleteLater();
        thread->quit();
        thread->deleteLater();
    });
    std::cout << "about to start worker thread" << std::endl;
    thread->start();
    std::cout << "started worker thread" << std::endl;
}

void GLWidget::archAlign()
{
    std::cout << "from align number of faces, vertices" << upperJawUnsegmented.n_faces() << " , "
              << upperJawUnsegmented.n_vertices() << std::endl;

    makeCurrent();

    mcg::Segmentation_Result seg{};
    seg.upper = upperArch;
    seg.lower = lowerArch;

    pmp::mat4 t = pmp::rotation_matrix_x(10.0f) * pmp::rotation_matrix_y(67.0f) * pmp::rotation_matrix_z(103.0f);
    t = pmp::translation_matrix(pmp::vec3{5.0f, 2.3f, -4.2}) * t;
    mcg::mesh_transform(upperJawUnsegmented, t);

    mcg::Alignment_Result res = mcg::arch_align_upper_and_lower(upperJawUnsegmented, lowerJawUnsegmented, seg);

    // Apply arch alignment
    mcg::mesh_transform(lowerJawUnsegmented, res.lower_transform);
    mcg::mesh_transform(upperJawUnsegmented, res.upper_transform);

    // Rotate towards camera
    t = pmp::rotation_matrix_x(-90.0f);
    mcg::mesh_transform(upperJawUnsegmented, t);
    t = pmp::rotation_matrix_x(-90.0f);
    mcg::mesh_transform(lowerJawUnsegmented, t);

    for (int i = 0; i < lowerJawMeshes.size(); i++) delete lowerJawMeshes[i];
    for (int i = 0; i < upperJawMeshes.size(); i++) delete upperJawMeshes[i];
    upperJawMeshes.clear();
    lowerJawMeshes.clear();

    pmp::SurfaceMesh upperGumMesh = mcg::mesh_extract(upperJawUnsegmented, upperArch.gum_faces);
    upperJawMeshes.push_back(new Mesh(upperGumMesh, 0));
    for (int idx = 0; idx < upperArch.teeth.size() && idx < 16; idx++) {
        if (upperArch.teeth[idx].is_present) {
            pmp::SurfaceMesh tooth = mcg::mesh_extract(upperJawUnsegmented, upperArch.teeth[idx].faces);
            upperJawMeshes.push_back(new Mesh(tooth, idx + 1));
        }
    }

    pmp::SurfaceMesh lowerGumMesh = mcg::mesh_extract(lowerJawUnsegmented, lowerArch.gum_faces);
    lowerJawMeshes.push_back(new Mesh(lowerGumMesh, 33));
    for (int idx = 0; idx < lowerArch.teeth.size() && idx < 16; idx++) {
        if (lowerArch.teeth[idx].is_present) {
            pmp::SurfaceMesh tooth = mcg::mesh_extract(lowerJawUnsegmented, lowerArch.teeth[idx].faces);
            lowerJawMeshes.push_back(new Mesh(tooth, idx + 17));
        }
    }

    upperJawCenter = Utils::pmpPointToGlm(pmp::centroid(upperJawUnsegmented));
    lowerJawCenter = Utils::pmpPointToGlm(pmp::centroid(lowerJawUnsegmented));
    areArchesAligned = true;

    rebuildMainScene();
    update();
}

void GLWidget::rebuildMainScene()
{
    std::vector<Mesh *> allMeshes;
    allMeshes.insert(allMeshes.end(), upperJawMeshes.begin(), upperJawMeshes.end());
    allMeshes.insert(allMeshes.end(), lowerJawMeshes.begin(), lowerJawMeshes.end());

    mainScene = Scene(allMeshes);
    currentScene = &mainScene;

    // Calculate new scene center
    if (!allMeshes.empty()) {
        glm::vec3 sceneCenter(0.0f);
        for (Mesh *mesh : allMeshes) {
            sceneCenter += mesh->center;
        }
        sceneCenter /= (float)allMeshes.size();
        mainScene.center = sceneCenter;
    }
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);  // set background color
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    // Shaders
    shader = new Shader("shaders/vertexShader.vs", "shaders/fragmentShader.fs");
    shader->use();

    // Setup projection matrix
    projection = glm::perspective(glm::radians(45.0f), (float)QWidget::width() / QWidget::height(), 0.1f, 1000.0f);
    shader->setMatrix4("projection", glm::value_ptr(projection));

    model = glm::mat4(1.0f);
    shader->setMatrix4("model", glm::value_ptr(model));

    // Load model

    auto modelLoadTime1 = std::chrono::steady_clock::now();
    if (jaw_index == 0) {
        loadLowerJawAsync(QString::fromStdString(initialFilePath));
    } else {
        loadUpperJawAsync(QString::fromStdString(initialFilePath));
    }
    auto modelLoadTime2 = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> ms_double = modelLoadTime2 - modelLoadTime1;
    // std::cout << "Model loaded in " << ms_double << std::endl;

    std::cout << "mesh loaded" << std::endl;

    toothGizmo = new Gizmo();
}

void GLWidget::resizeGL(int w, int h)
{
    projection = glm::perspective(glm::radians(45.0f), (float)w / h, 0.1f, 1000.0f);
    shader->setMatrix4("projection", glm::value_ptr(projection));
    glViewport(0, 0, w, h);  // adjust viewport to new size
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear screen

    view = camera.GetViewMatrix();
    shader->setMatrix4("view", glm::value_ptr(view));

    shader->setBool("isFlatColor", false);

    if (currentScene == nullptr) return;
    currentScene->Draw(shader, selectedMesh, gizmoModelMatrix, isUpperJawSelected, isLowerJawSelected);

    if (isGizmoEnabled) {
        glClear(GL_DEPTH_BUFFER_BIT);  // Clear depth buffer to always render gizmo on top
        toothGizmo->draw(shader, camera.distance, camera.position, camera.front);
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    std::cout << __func__ << std::endl;
    mousePosX = event->position().x();
    mousePosY = event->position().y();
    isRotating = false;
    glm::vec3 rayDirection;
    Camera::ScreenPosToWorldRay(mousePosX, mousePosY, GLWidget::width(), GLWidget::height(), view, projection,
                                rayDirection);

    if (isGizmoEnabled) {
        // Check if gizmo is clicked
        bool gizmoIntersected = false;
        for (GizmoComponent *c : toothGizmo->components) {
            // Check individual intersection with each gizmo component
            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), toothGizmo->position) * c->rotation;
            modelMatrix = glm::scale(modelMatrix, glm::vec3(camera.distance / 100.0f));

            float intersection_distance;
            gizmoIntersected = Utils::doesRayIntersectAABB(c->aabb_min, c->aabb_max, camera.position, rayDirection,
                                                           modelMatrix, intersection_distance);
            if (gizmoIntersected) {
                isDraggingGizmo = true;
                selectedGizmoComponent = c;
                return;
            }
        }
    }

    Mesh *hitMesh = nullptr;
    pmp::Vertex hitVertexIndex;
    glm::vec3 intersectionPoint;
    bool intersection = currentScene->IntersectTriangles(camera.position, rayDirection, model, hitMesh, hitVertexIndex,
                                                         intersectionPoint);
    if (intersection) {
        std::cout << "Hit Mesh: " << hitMesh->tooth_number << std::endl;
        std::cout << "Vertex index: " << hitVertexIndex << std::endl;
        std::cout << "Intersection point: " << intersectionPoint.x << " " << intersectionPoint.y << " "
                  << intersectionPoint.z << std::endl;
    }
    if (MainWindow::inFreeDeformation && shiftHeld) {
        brush.setPosition(intersectionPoint);
        selectedMesh->applyFreeDeformation(brush, freeDeformAddMode);
        update();
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    float newMousePosX = event->position().x();
    float newMousePosY = event->position().y();
    float offsetX = newMousePosX - mousePosX;
    float offsetY = newMousePosY - mousePosY;
    if (abs(offsetX) > 3 || abs(offsetY) > 3) {
        isRotating = true;
    }
    if (isDraggingGizmo && selectedGizmoComponent != nullptr) {
        glm::ivec4 viewport = glm::ivec4(0, 0, GLWidget::width(), GLWidget::height());

        glm::vec3 objectPosition;
        if (MainWindow::inMoveTooth) {
            objectPosition = selectedMesh->center;
        } else if (MainWindow::inMoveJaw) {
            objectPosition = isUpperJawSelected ? upperJawCenter : lowerJawCenter;
        }
        selectedGizmoComponent->onDrag(mousePosX, mousePosY, newMousePosX, newMousePosY, objectPosition,
                                       camera.position, camera.front, view, projection, viewport, gizmoModelMatrix);
        if (MainWindow::inMoveTooth) {
            toothGizmo->position = gizmoModelMatrix * glm::vec4(selectedMesh->center, 1.0f);
        } else if (MainWindow::inMoveJaw) {
            if (isUpperJawSelected) toothGizmo->position = gizmoModelMatrix * glm::vec4(upperJawCenter, 1.0f);
            if (isLowerJawSelected) toothGizmo->position = gizmoModelMatrix * glm::vec4(lowerJawCenter, 1.0f);
        }
    } else if (MainWindow::inFreeDeformation && shiftHeld) {
        glm::vec3 rayDirection;
        Camera::ScreenPosToWorldRay(event->position().x(), event->position().y(), GLWidget::width(), GLWidget::height(),
                                    view, projection, rayDirection);

        Mesh *hitMesh = nullptr;
        pmp::Vertex dummyVertex;
        glm::vec3 intersectionPoint;
        bool intersection = currentScene->IntersectTriangles(camera.position, rayDirection, model, hitMesh, dummyVertex,
                                                             intersectionPoint);

        if (intersection) {
            brush.setPosition(intersectionPoint);
            selectedMesh->applyFreeDeformation(brush, freeDeformAddMode);
            update();
        }
    } else if (ctrlHeld) {
        camera.processMove(offsetX, offsetY);
    } else {
        camera.processMouse(offsetX, offsetY);
    }
    mousePosX = event->position().x();
    mousePosY = event->position().y();
    update();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (isDraggingGizmo) {
        isDraggingGizmo = false;
        if (MainWindow::inMoveTooth) {
            selectedMesh->translate(gizmoModelMatrix);
        } else if (MainWindow::inMoveJaw) {
            if (isUpperJawSelected) {
                for (Mesh *m : upperJawMeshes) m->translate(gizmoModelMatrix);
                upperJawCenter = gizmoModelMatrix * glm::vec4(upperJawCenter, 1.0f);
            }
            if (isLowerJawSelected) {
                for (Mesh *m : lowerJawMeshes) m->translate(gizmoModelMatrix);
                lowerJawCenter = gizmoModelMatrix * glm::vec4(lowerJawCenter, 1.0f);
            }
        }
        gizmoModelMatrix = glm::mat4(1.0f);
    }
    if (!isRotating) {
        float mouseX = event->position().x();
        float mouseY = event->position().y();
        glm::vec3 rayDirection;
        Camera::ScreenPosToWorldRay(mouseX, mouseY, GLWidget::width(), GLWidget::height(), view, projection,
                                    rayDirection);

        if (!MainWindow::inMoveJaw) {
            Mesh *intersectedMesh = nullptr;
            bool intersection = currentScene->Intersect(camera.position, rayDirection, model, intersectedMesh, true);
            if (intersection && intersectedMesh != nullptr) {
                selectedMesh = intersectedMesh;
                if (MainWindow::inMoveTooth) {
                    isGizmoEnabled = true;
                    toothGizmo->position = intersectedMesh->center;
                }
                if (inMainScene) emit meshSelectedInMainScene();
            } else {
                std::cout << "nullpointer" << std::endl;
            }
            update();
        } else if (MainWindow::inMoveJaw) {
            Mesh *intersectedMesh = nullptr;
            bool intersection = currentScene->Intersect(camera.position, rayDirection, model, intersectedMesh, false);
            if (intersection) {
                if (intersectedMesh->tooth_number < 17) {
                    isLowerJawSelected = false;
                    isUpperJawSelected = true;
                    isGizmoEnabled = true;
                    toothGizmo->position = upperJawCenter;
                }
                if (intersectedMesh->tooth_number >= 17) {
                    isLowerJawSelected = true;
                    isUpperJawSelected = false;
                    isGizmoEnabled = true;
                    toothGizmo->position = lowerJawCenter;
                }
            } else {
                isGizmoEnabled = false;
                isLowerJawSelected = false;
                isUpperJawSelected = false;
            }
            update();
        }
    }
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull()) {
        camera.addDistance(numPixels.y());
    } else if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        camera.addDistance(numSteps.y());
    }

    update();
    event->accept();
}

void GLWidget::saveEditSceneAndReturnToMainScene()
{
    currentScene = &mainScene;
    inMainScene = true;
    update();
}

void GLWidget::setSelectedMeshDirectionalScale(int val, bool xActive, bool yActive, bool zActive)
{
    std::cout << __func__;
    float scaleF = val / 10.0;
    std::cout << "scale: " << scaleF << std::endl;
    if (selectedMesh != nullptr) {
        float x = 1.0f;
        float y = 1.0f;
        float z = 1.0f;
        if (xActive) x = val / 10.0;
        if (yActive) y = val / 10.0f;
        if (zActive) z = val / 10.0f;
        selectedMesh->setScaleDirectional(x, y, z);
        update();
    }
}

void GLWidget::onEditSceneClicked()
{
    // if (selectedMesh != nullptr && inMainScene) {
    //     camera.resetPosition();
    //     std::string neighbor1Name = "";
    //     std::string neighbor2Name = "";
    //     Scene::GetNeighboringMeshNames(selectedMesh->tooth_number, neighbor1Name, neighbor2Name);
    //
    //     std::vector<Mesh *> editMeshes = {selectedMesh};
    //
    //     // Add neighboring teeth meshes
    //     if (neighbor1Name != "") {
    //         for (int i = 0; i < meshes.size(); i++) {
    //             if (meshes[i]->tooth_number == neighbor1Name) {
    //                 editMeshes.push_back(meshes[i]);
    //             }
    //         }
    //     }
    //     if (neighbor2Name != "") {
    //         for (int i = 0; i < meshes.size(); i++) {
    //             if (meshes[i]->tooth_number == neighbor2Name) editMeshes.push_back(meshes[i]);
    //         }
    //     }
    //
    //     editScene = Scene(editMeshes);
    //     editScene.center = selectedMesh->center;
    //     currentScene = &editScene;
    //     emit movedToEditScene();
    //     inMainScene = false;
    // } else if (!inMainScene) {
    //     saveEditSceneAndReturnToMainScene();
    //     emit movedToMainScene();
    // }
    // update();
}

void GLWidget::onResetJawClicked()
{
    // Reset upper jaw to left of origin
    glm::mat4 mat(1.0f);
    float direction = 1.0f;
    if (areArchesAligned) {
        mat = glm::rotate(mat, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mat = glm::rotate(mat, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        direction = -1.0f;
    }
    mat = glm::translate(mat, -upperJawCenter);
    mat = glm::translate(mat,
                         glm::vec3(direction * -40.0f, 0.0f, 0.0f));  // TODO: Use AABB width instead of hardcoded value
    for (Mesh *m : upperJawMeshes) {
        m->translate(mat);
    }
    upperJawCenter = mat * glm::vec4(upperJawCenter, 1.0f);
    if (isGizmoEnabled && isUpperJawSelected) toothGizmo->position = mat * glm::vec4(toothGizmo->position, 1.0f);

    // Reset lower jaw to right of origin
    mat = glm::mat4(1.0f);
    if (areArchesAligned) mat = glm::rotate(mat, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    mat = glm::translate(mat, -lowerJawCenter);
    mat = glm::translate(mat, glm::vec3(40.0f, 0.0f, 0.0f));
    for (Mesh *m : lowerJawMeshes) {
        m->translate(mat);
    }
    lowerJawCenter = mat * glm::vec4(lowerJawCenter, 1.0f);
    if (isGizmoEnabled && isLowerJawSelected) toothGizmo->position = mat * glm::vec4(toothGizmo->position, 1.0f);

    areArchesAligned = false;
    update();
}

void GLWidget::setSelectedMeshScale(int scale)
{
    // std::cout<<"scale :"<<scale<<std::end;
    float scaleF = scale / 10.0f;
    std::cout << "scaleF :" << scaleF << std::endl;
    // std::cout<<"scale: "<<scaleF<<std::endl;
    if (selectedMesh != nullptr) {
        selectedMesh->setScale(scaleF);
        update();
    }
}

void GLWidget::setSelectedMeshRotationAngle(int rotationAngleDegrees)
{
    std::cout << "rotationAngleDegrees :" << rotationAngleDegrees << std::endl;
    if (selectedMesh != nullptr) {
        selectedMesh->setRotationAngle(rotationAngleDegrees);
        update();
    }
}

void GLWidget::updateMeshScale()
{
    if (selectedMesh != nullptr) {
        selectedMesh->updateMeshScale();
        update();
    }
}

void GLWidget::rotateMesh()
{
    if (selectedMesh != nullptr) {
        selectedMesh->rotateMesh(currentScene->center);
        update();
    }
}

void GLWidget::setFreeDeformAddMode(bool isAdd)
{
    freeDeformAddMode = isAdd;
    currentBrushMode = isAdd ? BrushMode::Add : BrushMode::Remove;
    updateCursor();
    update();
    std::cout << "[FreeDeform] Mode:" << (isAdd ? "Add" : "Remove") << std::endl;
}

void GLWidget::setDeformationStrength(int value)
{
    brush.setStrength(value);
    std::cout << "[FreeDeform] Strength set to:" << value << std::endl;
}

void GLWidget::setBrushSize(int value)
{
    brush.setRadius(value);
    std::cout << "[Brush] Size set to: " << value << std::endl;
    updateCursor();
}

void GLWidget::updateCursor()
{
    if (MainWindow::inFreeDeformation && shiftHeld) {
        setCursor(CursorFactory::createCursor(brush.getRadius(), currentBrushMode));
    } else {
        unsetCursor();
    }
}

void GLWidget::onToolChange()
{
    makeCurrent();

    if (!MainWindow::inMoveJaw && !MainWindow::inMoveTooth) {
        isGizmoEnabled = false;
    }

    if (MainWindow::inMoveTooth) {
        if (selectedMesh != nullptr) {
            isGizmoEnabled = true;
            toothGizmo->position = selectedMesh->center;
        } else {
            isGizmoEnabled = false;
        }
    }
    if (MainWindow::inMoveJaw) {
        selectedMesh = nullptr;
        isGizmoEnabled = false;
    } else {
        isUpperJawSelected = false;
        isLowerJawSelected = false;
    }
    updateCursor();
    update();
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Shift && !shiftHeld) {
        shiftHeld = true;
        if (MainWindow::inFreeDeformation) updateCursor();
    } else if (event->key() == Qt::Key_Control) {
        ctrlHeld = true;
    }
    QOpenGLWidget::keyPressEvent(event);
}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Shift) {
        shiftHeld = false;
        unsetCursor();
    } else if (event->key() == Qt::Key_Control) {
        ctrlHeld = false;
    }
    QOpenGLWidget::keyReleaseEvent(event);
}
