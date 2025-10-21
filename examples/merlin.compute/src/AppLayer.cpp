#include "AppLayer.h"

using namespace Merlin;

#include <iostream>
#include <iomanip>
#include <glfw/glfw3.h>

const float radius = 3;

AppLayer::AppLayer(){}
AppLayer::~AppLayer(){}

void AppLayer::setupScene() {
	camera().setPosition(glm::vec3(0.7, -35, 7.4));
	camera().setView(CameraView::Iso, 30);

	//scene.add(Primitives::createFloor(200, 10));
	Mesh_Ptr floor = Primitives::createCircle(30, 100);
	floor->setMaterial("gray plastic");
	scene.add(floor);
}

void AppLayer::setupPhysics() {

	std::vector<glm::vec4> position;
	for(float y = -24.5 * 0.5; y < 25 * 0.5; y += 0.25*0.5)
		for (float x = -24.5 * 0.5; x < 25 * 0.5; x += 0.25*0.5) {
			position.push_back(glm::vec4(x, y, (0.5*(x + y) + 40.0)/20.0, 1));
	}

	ps = ParticleSystem::create("Particles", position.size());

	MemoryManager::instance().createBuffer<glm::vec4>("position_buffer", position.size());
	MemoryManager::instance().createBuffer<glm::vec4>("velocity_buffer", position.size());

	MemoryManager::instance().writeBuffer<glm::vec4>("position_buffer", position);
	MemoryManager::instance().clearBuffer("velocity_buffer");

	solver = ComputeShader::create("solver", "assets/shaders/solver.comp");
	ps->setShader(Shader::create("particle", "./assets/shaders/particle.vert", "./assets/shaders/particle.frag"));
	ps->setDisplayMode(ParticleSystemDisplayMode::POINT_SPRITE_SHADED);
	ps->setPositionBuffer(MemoryManager::instance().getBuffer("position_buffer"));

	solver->use();
	solver->setUInt("numParticles", position.size());
	

	GLuint pThread = position.size();
	GLuint pWkgSize = 64; //Number of thread per workgroup
	GLuint pWkgCount = (pThread + pWkgSize - 1) / pWkgSize; //Total number of workgroup needed
	solver->setWorkgroupLayout(pWkgCount);

	scene.add(ps);
}

void AppLayer::onAttach(){
	Layer3D::onAttach();

	renderer.initialize();
	renderer.enableSampleShading();
	renderer.disableFaceCulling();
	renderer.setEnvironmentGradientColor(0.903, 0.803, 0.703);
	//renderer.showLights();
	//glfwSwapInterval(0);

	setupScene();
	setupPhysics();
}

void AppLayer::onDetach(){}

void AppLayer::onEvent(Event& event){
	Layer3D::onEvent(event);
}

void AppLayer::onPhysicsUpdate(Timestep ts) {
	MemoryManager::instance().resetBindings();
	solver->bindBuffer();
	solver->use();
	for (int i = 0; i < 20; i++) {
		solver->dispatch();
		solver->barrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
}

void AppLayer::onUpdate(Timestep ts){
	Layer3D::onUpdate(ts);

	renderer.clear();
	renderer.renderScene(scene, camera());
	renderer.reset();

	onPhysicsUpdate(0.016);
}

void AppLayer::onImGuiRender()
{
	ImGui::Begin("Camera");

	model_matrix_translation = camera().getPosition();
	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		camera().setPosition(model_matrix_translation);

	}
	ImGui::End();

	// Define a recursive lambda function to traverse the scene graph
	std::function<void(const std::list<shared<RenderableObject>>&)> traverseNodes = [&](const std::list<shared<RenderableObject>>& nodes){
		for (auto& node : nodes){
			bool node_open = ImGui::TreeNode(node->name().c_str());
			if (node_open){
				
				if (node != nullptr){
					ImGui::Text(node->name().c_str());
				}

				traverseNodes(node->children());
				ImGui::TreePop();
			}
		}
	};

	// draw the scene graph starting from the root node
	ImGui::Begin("Scene Graph");
	traverseNodes(scene.nodes());
	ImGui::End();
	


}
