export module gui:configcomp;

import std;

import :controller;
import scene;
import imguiext;


export void GuiController::renderConfig()
{
	static int resolution = scene.getResolution();
	static bool instantMill = false;

	if (scene.isSimulationInProgress())
	{
		ImGui::Text("Simulation in progress");
		if (ImGui::Button("Stop"))
			scene.stopSimulation();
		return;
	}

	if (ImGui::SliderInt("Resolution", &resolution, 3, 1600))
	{
		scene.setResolution(resolution);
	}

	ext::InputClampedFloat("Base height", &scene.baseHeight, 0.0f, 5.0f);

	if (ext::InputClampedFloat("Mill height", &scene.drillHeight, 0.0f, 5.0f))
	{
		scene.setDrillHeight();
	}

	if (ext::InputClampedFloat("Mill radius", &scene.drillRadius, 0.0f, 5.0f))
	{
		scene.setDrillRadius();
	}

	ImGui::Checkbox("Instant milling", &instantMill);

	if (!instantMill)
		ext::InputClampedFloat("Milling velocity", &scene.velocity, 0.001f, 15.0f);

	if (scene.isMillable())
	{
		if (ImGui::Button("Begin simulation"))
		{
			if (instantMill)
				scene.instantMill();
			else
				scene.beginGradualMill();
		}
	}
	else
	{
		ImGui::Text("Load a file to begin the simulation");
	}
}