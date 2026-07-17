//
//  ProjectStructure.cpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 6/16/23.
//

#include "ProjectStructure.h"
#include "../ProjectManagement/Project.h"
#include "../ProjectManagement/FileTemplate.h"
#include "../AssetManagement/AssetMetaFile.h"
#include "../AssetManagement/AssetDatabase.h"
#include "../AssetManagement/Importers/ShaderImporter.h"
#include "../LoadSettings.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace VWolfPup {

    class ProjectNode;

    ProjectNode* selectedEntry;

    class ProjectNode {
    public:
        ProjectNode(std::filesystem::directory_entry entry): entry(entry) {
            isDirectory = entry.is_directory();
            nodes.clear();
            if (isDirectory) {
                for(auto const& dir_entry : std::filesystem::directory_iterator(entry.path())) {
                    if (dir_entry.path().extension().string() == AssetMetaFile::META_FILE_EXTENSION) continue;
                    nodes.push_back(VWolf::CreateRef<ProjectNode>(dir_entry));
                }
                
                std::vector<VWolf::Ref<ProjectNode>> directories;
                
                std::copy_if(nodes.begin(), nodes.end(), std::back_inserter(directories), [](VWolf::Ref<ProjectNode> node) { return node->isDirectory; });
                isLeaf = directories.size() == 0;
            } else {
                isLeaf = true;
            }
            
        }

        ~ProjectNode() {
            nodes.clear();
        }
    public:
        void OnDirectoryGui() {
            if (isDirectory) {
                auto filename = entry.path().filename();
                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

                if (isLeaf) {
                    flags |= ImGuiTreeNodeFlags_Leaf;
                    ImGui::PushID(filename.string().c_str());
                    ImGui::TreePush((std::string("##") + filename.string()).c_str());

                    if (ImGui::Selectable(filename.string().c_str(), selectedEntry == this)) {
                        selectedEntry = this;
                        VWOLF_CLIENT_INFO("Selected %s", entry.path().string().c_str());
                    }
                    ImGui::TreePop();
                    ImGui::PopID();
                    return;
                }
                
                ImGui::PushID(filename.string().c_str());
                bool isOpen = ImGui::TreeNodeEx((std::string("##") + filename.string()).c_str(), flags);
                ImGui::SameLine();
                if (ImGui::Selectable(filename.string().c_str(), selectedEntry == this)) {
                    selectedEntry = this;
                    VWOLF_CLIENT_INFO("Selected %s", entry.path().string().c_str());
                }

                if (isOpen && !isLeaf) {
                    for(VWolf::Ref<ProjectNode> node : nodes) {
                        node->OnDirectoryGui();
                    }
                    ImGui::TreePop();
                }
                ImGui::PopID();
            }
        }

        void OnFileExplorerGui() {
            if (!isDirectory) return;
            
            float width = ImGui::GetContentRegionMax().x;
            uint32_t numOfItemsPerGroup = 6;
            
            try {
                int index = 0;
                for (auto it: nodes) {
                    if (index > 0 && index % numOfItemsPerGroup != 0)
                        ImGui::SameLine();
                    auto filename = it->entry.path().filename();
                    ImGui::PushID(filename.string().c_str());
                    if (ImGui::Selectable(filename.string().c_str(),
                                          selectedPath == filename,
                                          ImGuiSelectableFlags_AllowDoubleClick,
                                          ImVec2(width / numOfItemsPerGroup, width / numOfItemsPerGroup))) {
                        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && it->isDirectory) {
                            selectedEntry = it.get();
                        }
                        VWOLF_CLIENT_INFO("Test %s", filename.string().c_str());
                        selectedPath = filename;
                    } else if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
//                        VWOLF_CLIENT_INFO("Test %s", filename.string().c_str());
                        selectedPath = filename;
                    }
                    ImGui::PopID();
                    index++;
                }
            } catch (std::length_error& error) {
                VWOLF_CLIENT_ERROR("Error %s", error.what());
            }
        }
    public:
        inline std::filesystem::path GetSelectedPath() { return selectedPath; }
        void SetSelectedPath(std::filesystem::path path) { selectedPath = path; }
        inline std::filesystem::path GetDirectorySelectedPath() { return entry.path(); }
        inline std::filesystem::path GetFullPath() { return GetDirectorySelectedPath() / GetSelectedPath(); }
    private:
        bool isLeaf = true;
        bool isDirectory = false;
        std::filesystem::directory_entry entry;
        std::filesystem::path selectedPath;

        std::vector<VWolf::Ref<ProjectNode>> nodes;
    };

    class ProjectTree {
    public:
        ProjectTree() {}
        ~ProjectTree() {}
    public:
        void Build(std::filesystem::path path) {
            nodes.clear();
            for(auto const& dir_entry : std::filesystem::directory_iterator(path)) {
                if (dir_entry.path().extension().string() == AssetMetaFile::META_FILE_EXTENSION) continue;
                nodes.push_back(VWolf::CreateRef<ProjectNode>(dir_entry));
            }
            selectedEntry = new ProjectNode(std::filesystem::directory_entry(path));
            root = selectedEntry;
        }

        void OnDirectoryGui() {
            for(VWolf::Ref<ProjectNode> node : nodes) {
                node->OnDirectoryGui();
            }
        }

        ProjectNode* GetRoot() { return root; }
    private:
        std::vector<VWolf::Ref<ProjectNode>> nodes;
        ProjectNode* root;
    };

    ProjectStructure::ProjectStructure(): View("Project Structure"), projectTree(VWolf::CreateRef<ProjectTree>()) {
        auto proj = VWolfPup::Project::CurrentProject();
        proj->AddObserver(this, [this](const std::string& path, const efsw::Action event) {
//            std::cout << path << ' ' << filewatch::event_to_string(event) << '\n';
            this->Rebuild();
        });
        projectTree->Build(proj->GetAssetsPath());
//        VWOLF_CLIENT_INFO("Test");
    }

    ProjectStructure::~ProjectStructure() {
        
    }

    void ProjectStructure::Rebuild() {
        std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);
        projectTree->Build(VWolfPup::Project::CurrentProject()->GetAssetsPath());
    }

    std::filesystem::path GetPathForUntitled(std::filesystem::path directory, std::string extension, uint32_t number = 0) {
        std::string extension1 = extension != "" ? (extension.find('.') == std::string::npos ? "." + extension : extension ) : "";
        std::string filename = directory / ((std::string("Untitled") + (number == 0 ? std::string() : std::to_string(number))) + extension1);
        if (std::filesystem::exists(filename)) {
            return GetPathForUntitled(directory, extension, ++number);
        }
        return filename;
    }

    void ProjectStructure::OnGui() {
        std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

        ImGui::Begin(title.c_str());
        const float minusSize = 40.0f;
        
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));
        ImGui::BeginChild("child1", ImVec2(startingWidth, ImGui::GetWindowSize().y - minusSize), true);
        
        bool isOpen = ImGui::TreeNodeEx("##Project",
                                        ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow);
        ImGui::SameLine();
        if (ImGui::Selectable("Project", selectedEntry == projectTree->GetRoot())) {
            selectedEntry = projectTree->GetRoot();
            VWOLF_CLIENT_INFO("Test");
        }
        if (isOpen) {
            projectTree->OnDirectoryGui();
            ImGui::TreePop();
        }
        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::InvisibleButton("vsplitter", ImVec2(8.0f, ImGui::GetWindowSize().y - minusSize));
        if (ImGui::IsItemActive())
            startingWidth += ImGui::GetIO().MouseDelta.x;
        ImGui::SameLine();
        ImGui::BeginChild("child2", ImVec2(0, ImGui::GetWindowSize().y - minusSize), true);
        if (selectedEntry) {
            selectedEntry->OnFileExplorerGui();
            
            bool isHoveringDropZone = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

            // 4. Accept the file payload if hovering the child window and a file was dropped
            if (isHoveringDropZone && filesDropped.size() > 0) {
                for (std::string& pathString : filesDropped) {
                    std::filesystem::path path(pathString);
                    std::filesystem::path newFile = GetPathForUntitled(selectedEntry->GetDirectorySelectedPath(), path.extension());
                    std::filesystem::copy(path, newFile);
                }
                filesDropped.clear();
            }

        }
        if (showDialog) {
            if (ImGui::BeginPopupContextWindow("CreateMenu"))
            {
                didSelection = true;
                if (selectedEntry->GetSelectedPath() != "") {
                    if (ImGui::MenuItem("Delete")) {
                        std::filesystem::remove(selectedEntry->GetFullPath());
                    }
                }
                if (ImGui::BeginMenu("Create")) {

                    if (ImGui::MenuItem("Folder")) {
                        std::filesystem::path newPath = GetPathForUntitled(selectedEntry->GetDirectorySelectedPath(), "");
                        std::filesystem::create_directory(newPath);
                    }
                    if (ImGui::MenuItem("Scene"))
                    {
                        showDialog = false;

                        std::filesystem::path newFile = GetPathForUntitled(selectedEntry->GetDirectorySelectedPath(), "scene");
                        VWolf::Scene scene(newFile.stem().string());
                        scene.Save(newFile);
                    }
                    if (ImGui::MenuItem("Material"))
                    {
                        showDialog = false;
                        
                        std::filesystem::path newFile = GetPathForUntitled(selectedEntry->GetDirectorySelectedPath(), "vwolfmat");
                        std::string shader = VWolf::MaterialLibrary::Default()->GetShaderName();
                        VWolf::Material material(newFile.stem().string(), shader);
                        material.Save(newFile);
                    }
                    if (ImGui::MenuItem("Shader"))
                    {
                        showDialog = false;
                        std::filesystem::path newFile = GetPathForUntitled(selectedEntry->GetDirectorySelectedPath(), "vwolfshader");
                        VWolf::Ref<FileTemplate> fileTemplate = FileTemplate::Find(ShaderImporter::GetShaderExtension());
                        VWolf::Shader shader(fileTemplate->GetPath(), newFile.stem());
                        shader.Save(newFile);
                    }
                    ImGui::EndMenu();
                }

                ImGui::EndPopup();
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleVar();

        ImGui::End();
    }

    void ProjectStructure::SetInContainer() {
        GetContainer()->GetRoot()->Install(this, ImGuiDir_Down);
    }

    void ProjectStructure::OnEvent(VWolf::Event& evt) {
        VWolf::Dispatch<VWolf::MouseButtonReleasedEvent>(evt, VWOLF_BIND_EVENT_FN(ProjectStructure::OnMouseButtonReleasedEvent));
        VWolf::Dispatch<VWolf::WindowDragDropEvent>(evt, VWOLF_BIND_EVENT_FN(ProjectStructure::OnWindowDragDropEvent));
    }

    bool ProjectStructure::OnMouseButtonReleasedEvent(VWolf::MouseButtonReleasedEvent& e) {
        if (e.GetMouseButton() == VWolf::MouseCode::Right) {
            showDialog = true;
        }
        else if (e.GetMouseButton() == VWolf::MouseCode::Left && !didSelection) {
            selectedEntry->SetSelectedPath("");
        }
        didSelection = false;
        return true;
    }

    bool ProjectStructure::OnWindowDragDropEvent(VWolf::WindowDragDropEvent& e) {
        
        filesDropped.clear();
        for (int index = 0; index < e.GetPathCount(); index++)
            filesDropped.push_back(e.GetPaths()[index]);
        return true;
    }
}
