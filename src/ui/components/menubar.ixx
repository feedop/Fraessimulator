export module gui:menubar;

import std;

import <imguiFileDialog/ImGuiFileDialog.h>;

import config;
import parser;
import scene;
import :controller;

export void GuiController::renderDialogWindow(GuiController::IoOperation& operation)
{
	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
			try
			{
				switch (operation)
				{
				case IoOperation::Open:
				{
					try
					{
						bool flat;
						int size;
						auto path = Parser::parse(filePath, flat, size);
						scene.drillRadius = size * 0.05f;
						scene.setPath(std::move(path));
						scene.setDrill(size, flat);
					}
					catch (const std::exception& e)
					{
						std::cerr << "An exception occurred while reading the file" << e.what() << "\n";
					}
				}
					break;
				}
			}
			catch (...)
			{
				std::cerr << "Error in gcode parsing\n";
			}

			operation = IoOperation::None;
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
}

export void GuiController::renderMenuBar()
{
	static IoOperation selectedOperation = IoOperation::None;

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
			IGFD::FileDialogConfig config;
			config.path = "C:/Users/filip/Studia/mgr1/MG1/CAD Bane/paths/";
            if (ImGui::MenuItem("Open"))
            {
				selectedOperation = IoOperation::Open;
				
				ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose file", "((.+[.]))", config);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    renderDialogWindow(selectedOperation);
}