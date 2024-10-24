#include "assignment5.hpp"
#include "parametric_shapes.hpp"

#include "config.hpp"
#include "core/Bonobo.h"
#include "core/FPSCamera.h"
#include "core/helpers.hpp"
#include "core/node.hpp"
#include "core/ShaderProgramManager.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <tinyfiledialogs.h>

#include <clocale>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <list>

edaf80::Assignment5::Assignment5(WindowManager& windowManager) :
	mCamera(0.5f * glm::half_pi<float>(),
	        static_cast<float>(config::resolution_x) / static_cast<float>(config::resolution_y),
	        0.01f, 1000.0f),
	inputHandler(), mWindowManager(windowManager), window(nullptr)
{
	WindowManager::WindowDatum window_datum{ inputHandler, mCamera, config::resolution_x, config::resolution_y, 0, 0, 0, 0};

	window = mWindowManager.CreateGLFWWindow("EDAF80: Assignment 5", window_datum, config::msaa_rate);
	if (window == nullptr) {
		throw std::runtime_error("Failed to get a window: aborting!");
	}

	bonobo::init();
}

edaf80::Assignment5::~Assignment5()
{
	bonobo::deinit();
}

const float START_POSITION = 50.0f;
const float MOVEMENT_DELTA = 5.0f;
const float ACCELERATION = 1000.0f;
const int N_OBSTACLES = 10;

void
edaf80::Assignment5::run()
{
	// Set up the camera
	mCamera.mWorld.SetTranslate(glm::vec3(0.0f, 5.0f, 50.0f));
	mCamera.mWorld.LookTowards(glm::vec3(1.0f, -0.1f, 0.0f));
	mCamera.mMouseSensitivity = glm::vec2(0.003f);
	mCamera.mMovementSpeed = glm::vec3(15.0f); // 3 m/s => 10.8 km/h
	auto camera_position = mCamera.mWorld.GetTranslation();
	auto light_position = glm::vec3(-2.0f, 4.0f, 2.0f);
	float elapsed_time_s = 0.0f;

	int hp = 10;
	
	float surfer_position = START_POSITION;
	float surfer_velocity = 0.0f;
	float surfer_acceleration = 0.0f;
	float spawn_rate = 2.0f;

	auto const set_uniforms = [&camera_position, &light_position, &elapsed_time_s](GLuint program){
		glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(camera_position));
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform1f(glGetUniformLocation(program, "t"), elapsed_time_s);
	};

	ShaderProgramManager program_manager;

	GLuint fallback_shader = 0u;
	program_manager.CreateAndRegisterProgram("Fallback",
	                                         { { ShaderType::vertex, "common/fallback.vert" },
	                                           { ShaderType::fragment, "common/fallback.frag" } },
	                                         fallback_shader);
	if (fallback_shader == 0u) {
		LogError("Failed to load fallback shader");
		return;
	}

	GLuint tissue_shader = 0u;
	program_manager.CreateAndRegisterProgram("Tissue",
	                                         { { ShaderType::vertex, "EDAF80/tissue.vert" },
	                                           { ShaderType::fragment, "EDAF80/tissue.frag" } },
	                                         tissue_shader);
	if (tissue_shader == 0u){
		LogError("Failed to load tissue shader");
		return;
	}

	GLuint skybox_shader = 0u;
	program_manager.CreateAndRegisterProgram("Skybox",
		{ { ShaderType::vertex, "EDAF80/skybox.vert" },
		  { ShaderType::fragment, "EDAF80/skybox.frag" } },
		skybox_shader);
	if (skybox_shader == 0u)
		LogError("Failed to load skybox shader");


	GLuint water_shader = 0u;
	program_manager.CreateAndRegisterProgram("Water",
	                                         { { ShaderType::vertex, "EDAF80/water.vert" },
	                                           { ShaderType::fragment, "EDAF80/water.frag" } },
	                                         water_shader);
	if (water_shader == 0u)
		LogError("Failed to load water shader");

	auto skybox_shape = parametric_shapes::createSphere(20.0f, 10u, 10u);
	if (skybox_shape.vao == 0u) {
		LogError("Failed to retrieve the mesh for the skybox");
	}

	GLuint phong_shader = 0u;
	program_manager.CreateAndRegisterProgram("Phong",
		{ { ShaderType::vertex, "EDAF80/phong.vert" },
		  { ShaderType::fragment, "EDAF80/phong.frag" } },
		phong_shader);
	if (phong_shader == 0u)
		LogError("Failed to load phong shader");

	bool use_normal_mapping = true;
	auto const phong_set_uniforms = [&use_normal_mapping,&light_position,&camera_position](GLuint program){
		glUniform1i(glGetUniformLocation(program, "use_normal_mapping"), use_normal_mapping ? 1 : 0);
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(camera_position));
	};

	GLuint cubemap = bonobo::loadTextureCubeMap(
		config::resources_path("cubemaps/Ocean/left.jpg"),
		config::resources_path("cubemaps/Ocean/right.jpg"),
		config::resources_path("cubemaps/Ocean/top.jpg"),
		config::resources_path("cubemaps/Ocean/bottom.jpg"),
		config::resources_path("cubemaps/Ocean/back.jpg"),
		config::resources_path("cubemaps/Ocean/front.jpg"));

	Node skybox;
	skybox.set_geometry(skybox_shape);
	skybox.set_program(&skybox_shader, set_uniforms);
	skybox.add_texture("cubemap", cubemap, GL_TEXTURE_CUBE_MAP);

	auto quad_shape = parametric_shapes::createTessQuad(300.0f, 500.0f, 2000u, 2000u);
	if (quad_shape.vao == 0u) {
		LogError("Failed to retrieve the mesh for the quad");
		return;
	}

	Node quad;
	quad.set_geometry(quad_shape);
	quad.add_texture("normal_map", bonobo::loadTexture2D(config::resources_path("textures/waves.png")), GL_TEXTURE_2D);
	quad.add_texture("cubemap", cubemap, GL_TEXTURE_CUBE_MAP);
	quad.set_program(&water_shader, set_uniforms);
	quad.get_transform().SetTranslate(glm::vec3(0.0f, 0.0f, -250.0f));

	
	auto tissue_shape = parametric_shapes::createTessQuad(5.0f, 5.0f, 100u, 100u);
	
	
	Node tissue;
	tissue.set_geometry(tissue_shape);
	tissue.set_program(&tissue_shader, set_uniforms);



	GLuint phong_cubemap = bonobo::loadTextureCubeMap(
		config::resources_path("textures/leather_red_02_coll1_2k.jpg"),
		config::resources_path("textures/leather_red_02_coll1_2k.jpg"),
		config::resources_path("textures/leather_red_02_coll1_2k.jpg"),
		config::resources_path("textures/leather_red_02_coll1_2k.jpg"),
		config::resources_path("textures/leather_red_02_coll1_2k.jpg"),
		config::resources_path("textures/leather_red_02_coll1_2k.jpg")
	);

	bonobo::material_data sphere_material;
	sphere_material.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
	sphere_material.diffuse = glm::vec3(0.7f, 0.2f, 0.4f);
	sphere_material.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	sphere_material.shininess = 10.0f;


	auto sphere_shape = parametric_shapes::createSphere(1.5f, 20u, 20u);
	if (sphere_shape.vao == 0u) {
		LogError("Failed to retrieve the mesh for the demo sphere");
		return;
	}

	Node sphere;
	sphere.set_geometry(sphere_shape);
	sphere.set_material_constants(sphere_material);
	sphere.set_program(&phong_shader, phong_set_uniforms);
	//sphere.add_texture("phong_cubemap", phong_cubemap, GL_TEXTURE_CUBE_MAP);
	sphere.add_texture("normal_map", bonobo::loadTexture2D(config::resources_path("textures/leather_red_02_nor_2k.jpg")), GL_TEXTURE_2D);
	sphere.add_texture("diffuse_map", bonobo::loadTexture2D(config::resources_path("textures/leather_red_02_coll1_2k.jpg")), GL_TEXTURE_2D);
	sphere.add_texture("specular_map", bonobo::loadTexture2D(config::resources_path("textures/leather_red_02_rough_2k.jpg")), GL_TEXTURE_2D);
	sphere.get_transform().SetTranslate(glm::vec3(10.0f, 2.0f, surfer_position));



	//
	// Todo: Insert the creation of other shader programs.
	//       (Check how it was done in assignment 3.)
	//

	//
	// Todo: Load your geometry
	//

	glClearDepthf(1.0f);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);


	auto lastTime = std::chrono::high_resolution_clock::now();

	bool show_logs = true;
	bool show_gui = true;
	bool shader_reload_failed = false;
	bool show_basis = false;
	float basis_thickness_scale = 1.0f;
	float basis_length_scale = 1.0f;
	std::int32_t quad_program_index = 0;

	Node tissues[N_OBSTACLES];
	for(int i = 0; i < 10; i++){
		Node new_tissue = tissue;
		tissues[i] = new_tissue;
	}
	std::list<Node>::iterator it;
	int tissue_index = 0;
	float tissue_latest_spawn = 0.0f;

	float latest_dmg = 0.0f;

	while (!glfwWindowShouldClose(window)) {
		auto const nowTime = std::chrono::high_resolution_clock::now();
		auto const deltaTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(nowTime - lastTime);
		lastTime = nowTime;
		elapsed_time_s += std::chrono::duration<float>(deltaTimeUs).count();

		auto& io = ImGui::GetIO();
		// inputHandler.SetUICapture(io.WantCaptureMouse, io.WantCaptureKeyboard);
		inputHandler.SetUICapture(io.WantCaptureMouse, false);

		glfwPollEvents();
		inputHandler.Advance();
		// mCamera.Update(deltaTimeUs, inputHandler);

		if (inputHandler.GetKeycodeState(GLFW_KEY_R) & JUST_PRESSED) {
			shader_reload_failed = !program_manager.ReloadAllPrograms();
			if (shader_reload_failed)
				tinyfd_notifyPopup("Shader Program Reload Error",
				                   "An error occurred while reloading shader programs; see the logs for details.\n"
				                   "Rendering is suspended until the issue is solved. Once fixed, just reload the shaders again.",
				                   "error");
		}
		if (inputHandler.GetKeycodeState(GLFW_KEY_F3) & JUST_RELEASED)
			show_logs = !show_logs;
		if (inputHandler.GetKeycodeState(GLFW_KEY_F2) & JUST_RELEASED)
			show_gui = !show_gui;
		if (inputHandler.GetKeycodeState(GLFW_KEY_F11) & JUST_RELEASED)
			mWindowManager.ToggleFullscreenStatusForWindow(window);


		// Retrieve the actual framebuffer size: for HiDPI monitors,
		// you might end up with a framebuffer larger than what you
		// actually asked for. For example, if you ask for a 1920x1080
		// framebuffer, you might get a 3840x2160 one instead.
		// Also it might change as the user drags the window between
		// monitors with different DPIs, or if the fullscreen status is
		// being toggled.
		int framebuffer_width, framebuffer_height;
		glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
		glViewport(0, 0, framebuffer_width, framebuffer_height);


		//
		// Todo: If you need to handle inputs, you can do it here
		//
		if (inputHandler.GetKeycodeState(GLFW_KEY_A) & JUST_PRESSED) {
			// surfer_position -= MOVEMENT_DELTA;
			surfer_acceleration -= ACCELERATION;
		} else if (inputHandler.GetKeycodeState(GLFW_KEY_D) & JUST_PRESSED) {
			// surfer_position += MOVEMENT_DELTA;
			surfer_acceleration += ACCELERATION;
		} else {
			surfer_acceleration = 0;
		}

		if (surfer_acceleration == 0) {
			if (surfer_velocity < 0) {
				surfer_velocity += 0.5f;
			} else {
				surfer_velocity -= 0.5f;
			}
		}
		float dt = std::chrono::duration<float>(deltaTimeUs).count();
		surfer_velocity += surfer_acceleration * dt;
		surfer_position += surfer_velocity * dt;
		surfer_position = std::clamp(surfer_position, START_POSITION - MOVEMENT_DELTA, START_POSITION + MOVEMENT_DELTA);


		mWindowManager.NewImGuiFrame();

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		// if 5 seconds have passed:
		if (elapsed_time_s - tissue_latest_spawn > spawn_rate) {
				// spawn a new tissue at a random offset from start position
				tissues[tissue_index % N_OBSTACLES].get_transform().SetTranslate(glm::vec3(100.0f, 2.0f, START_POSITION + static_cast<float>((rand() % 21) - 10)));
				
				// and update variables tracking time and current tissue in list
				tissue_latest_spawn = elapsed_time_s;
				tissue_index++;
				spawn_rate = std::max(0.4f, spawn_rate - 0.1f);
			}


		if (!shader_reload_failed) {
			glDisable(GL_DEPTH_TEST);
			skybox.render(mCamera.GetViewToClipMatrix() * glm::mat4(glm::mat3(mCamera.GetWorldToViewMatrix())));
			glEnable(GL_DEPTH_TEST);
			quad.render(mCamera.GetWorldToClipMatrix());
			sphere.get_transform().SetTranslate(glm::vec3(10.0f, 2.0f, surfer_position));
			sphere.render(mCamera.GetWorldToClipMatrix());

			// for all tissues, move them towards the player
			for (int i = 0; i < N_OBSTACLES; i++) {
				glm::vec3 paper_pos = tissues[i].get_transform().GetTranslation();
				glm::vec3 player_pos = sphere.get_transform().GetTranslation();
				paper_pos.x -= MOVEMENT_DELTA-4.5f;
				tissues[i].get_transform().SetTranslate(paper_pos);
				
				if (std::hypot((paper_pos.x + 2.5) - player_pos.x, (paper_pos.z + 2.5) - player_pos.z) < 1.5 + 3) {
					if(elapsed_time_s - latest_dmg > 1.0f){
						latest_dmg = elapsed_time_s;
						hp--;
						std::cout << "HP: " << hp << std::endl;
						
						// move tissue out of view
						tissues[i].get_transform().SetTranslate(glm::vec3(-10.f, 0.0f, 0.0f));
					}
					if(hp == 0){
						std::cout << "Game Over!" << std::endl;
						std::cout << "Score: " << static_cast<int>(elapsed_time_s * 10) << "!" << std::endl;
						return;
					}
				}
				
				tissues[i].render(mCamera.GetWorldToClipMatrix());
			}

		}


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//
		// Todo: If you want a custom ImGUI window, you can set it up
		//       here
		//
		bool const opened = ImGui::Begin("Scene Controls", nullptr, ImGuiWindowFlags_None);
		if (opened) {
			ImGui::Checkbox("Show basis", &show_basis);
			ImGui::SliderFloat("Basis thickness scale", &basis_thickness_scale, 0.0f, 100.0f);
			ImGui::SliderFloat("Basis length scale", &basis_length_scale, 0.0f, 100.0f);
			//auto quad_selection_result = program_manager.SelectProgram("Program", quad_program_index);
			//if (quad_selection_result.was_selection_changed) {
			//	quad.set_program(quad_selection_result.program, set_uniforms);
			//}
		}
		ImGui::End();

		if (show_basis)
			bonobo::renderBasis(basis_thickness_scale, basis_length_scale, mCamera.GetWorldToClipMatrix());
		if (show_logs)
			Log::View::Render();
		mWindowManager.RenderImGuiFrame(show_gui);

		glfwSwapBuffers(window);
	}
}

int main()
{
	std::setlocale(LC_ALL, "");

	Bonobo framework;

	try {
		edaf80::Assignment5 assignment5(framework.GetWindowManager());
		assignment5.run();
	} catch (std::runtime_error const& e) {
		LogError(e.what());
	}
}
