// Markus Buchholz, 2023

#include <stdafx.hpp>
#include "imgui_helper.hpp"
#include <tuple>
#include <thread>
#include <chrono>
#include <vector>
#include <math.h>
#include <list>
#include <numeric>
#include <memory>

//----------- system dynamic parameters --------------------

float g = 9.81f;
float dt = 0.1f;

//---------------------------------------------------------------

int GRID_SIZE = 50;
float dxdy = 0.4f;
float pix_step = 5.0f;

/*

u	is the velocity in the x direction, or zonal velocity
v	is the velocity in the y direction, or meridional velocity
H	is the mean height of the horizontal pressure surface
h	is the height deviation of the horizontal pressure surface from its mean height, where h: η(x, y, t) = H(x, y) + h(x, y, t)

*/


//---------------------------------------------------------------

struct Grid
{

	std::vector<std::vector<float>> mesh;

	Grid(float init)
	{
		std::vector<float> i_grid(GRID_SIZE, 0);
		mesh = std::vector<std::vector<float>>(GRID_SIZE, i_grid);

		for (int ii = 0; ii < GRID_SIZE; ii++)
		{
			for (int jj = 0; jj < GRID_SIZE; jj++)
			{

				mesh[ii][jj] = init;
			}
		}
	}
};

//-----------------------------------------------------------

int main(int argc, char const *argv[])
{
	ImVec4 clear_color = ImVec4(0.0f / 255.0, 0.0f / 255.0, 0.0f / 255.0, 1.00f);
	ImVec4 white_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 pink_color = ImVec4(245.0f / 255.0, 5.0f / 255.0, 150.0f / 255.0, 1.00f);
	ImVec4 blue_color = ImVec4(0.0f / 255.0, 0.0f / 255.0, 255.0f / 255.0, 1.00f);
	int flag = true;

	int w = 500;
	int h = 500;
	std::string title = "Shallow Water Simulation";
	initImgui(w, h, title);


	Grid water_h(1.0f); // water hight
	Grid vel_u(0.0f);	// water velocity in x direction
	Grid vel_v(0.0f);	// water velocity in y dirction

	// source
	for (int ii = 20; ii < 30; ii++)
	{
		for (int jj = 20; jj < 30; jj++)
		{
		water_h.mesh[ii][jj] = 3.0f;
		}
	}


	
		while (!glfwWindowShouldClose(window) && flag == true)
		{
			glfwPollEvents();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();

			ImGui::NewFrame();

			ImGuiWindowFlags window_flags = 0;
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_FirstUseEver);
			window_flags |= ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoResize;
			window_flags |= ImGuiWindowFlags_NoCollapse;
			window_flags |= ImGuiWindowFlags_NoBackground;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

			ImGui::Begin("Simulation", nullptr, window_flags);
			ImDrawList *draw_list = ImGui::GetWindowDrawList();


			for (int ii = 1; ii < GRID_SIZE - 1; ii++)
			{

				for (int jj = 1; jj < GRID_SIZE - 1; jj++)
				{

					float dh_dx = [=]()
					{
						float f = (water_h.mesh[ii + 1][jj] - water_h.mesh[ii - 1][jj]) / (2 * dxdy);

						return f;
					}();

					float dh_dy = [=]()
					{
						float f = (water_h.mesh[ii][jj + 1] - water_h.mesh[ii][jj - 1]) / (2 * dxdy);

						return f;
					}();

					vel_u.mesh[ii][jj] = vel_u.mesh[ii][jj] - dt * g * dh_dx;
					vel_v.mesh[ii][jj] = vel_v.mesh[ii][jj] - dt * g * dh_dy;

					float du_dx = [=]()
					{
						float f = (vel_u.mesh[ii+1][jj] - vel_u.mesh[ii-1][jj]) / (2 * dxdy);

						return f;
					}();

					 float dv_dy = [=]()
					{
						float f = (vel_v.mesh[ii][jj + 1] - vel_v.mesh[ii][jj - 1]) / (2 * dxdy);

						return f;
					}();

					water_h.mesh[ii][jj] = water_h.mesh[ii][jj] + dt * (-(du_dx + dv_dy));
				}
			}

			for (int ii = 0; ii < GRID_SIZE; ii++)
			{
				for (int jj = 0; jj < GRID_SIZE; jj++)
				{
					float D = 0.0;
					ImVec2 p0;
					ImVec2 p1;
					p0 = {(float)ii * pix_step, (float)jj * pix_step};
					p1 = {((float)ii * pix_step) + pix_step, ((float)jj * pix_step) + pix_step};

					float hi = water_h.mesh[ii][jj];

					ImVec4 pix = ImVec4(0.0f, 0.0f, hi - 0.3f, 1.0f);

					draw_list->AddRectFilled(p0, p1, ImColor(pix));

				}
			}

			ImGui::End();

			// Rendering
			ImGui::Render();
			int display_w, display_h;
			glfwGetFramebufferSize(window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
			glClear(GL_COLOR_BUFFER_BIT);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(window);
		}

		termImgui();
		return 0;
	}