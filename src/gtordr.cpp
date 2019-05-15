#include "gtmaker.h"

using namespace sp;

//--------------------------------------------------------------------------------
// global
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
// member
//--------------------------------------------------------------------------------

void GTMakerGUI::OrdrEditor::init() {

}

void GTMakerGUI::OrdrEditor::display() {

    MemP<GT> &gts = m_ptr->m_database.gtsList[m_ptr->m_select];

    for (int i = 0; i < gts.size(); i++) {
        GT &gt = gts[i];

        if (gt.contour.size() == 0) continue;
        if (&gt == m_ptr->m_focus && m_ptr->m_state == S_Init) continue;

        char str[256];
        sprintf(str, "GT %p", &gt);

        if (ImGui::Begin(str, NULL, ImGuiWindowFlags_Block)) {
            {
                const Vec2 pos = m_ptr->m_wmat * getVec2(gt.rect.dbase[0], gt.rect.dbase[1]) + getVec2(0.0, -40.0);
                ImGui::SetWindowPos(ImVec2(static_cast<float>(pos.x), static_cast<float>(pos.y)), ImGuiCond_Always);
            }

            if (&gt != m_ptr->m_focus && ImGui::IsMouseClicked(0) == true && ImGui::IsWindowHovered() == true) {
                m_ptr->m_focus = &gt;
            }

            ImGui::AlignTextToFramePadding();

            if (&gt != m_ptr->m_focus) {
                ImGui::SetWindowSize(ImVec2(50.0f, 35.0f), ImGuiCond_Always);

                ImGui::Text("> %02d", i);
            }
            else {
                ImGui::SetWindowSize(ImVec2(168.0f, 35.0f), ImGuiCond_Always);
                
                ImGui::Text("- %02d", i);
               
                ImGui::SameLine();

                ImGui::Text("order");

                ImGui::SameLine(0, 15.0f);

                if (ImGui::Button("++")) {
                    if (i < gts.size() - 1) {
                        sp::swap(gts[i], gts[i + 1]);
                        m_ptr->m_focus = &gts[i + 1];
                    }
                }
                
                ImGui::SameLine(0, 15.0f);

                if (ImGui::Button("--")) {
                    if (i > 0) {
                        sp::swap(gts[i], gts[i - 1]);
                        m_ptr->m_focus = &gts[i - 1];
                    }
                }
            }
            ImGui::End();
        }
    }

    for (int i = 0; i < gts.size(); i++) {
        GT &gt = gts[i];
        if (gt.contour.size() == 0) continue;

        Render::line(getVtx2(gt.rect), RENDER_GRAY, 3.0f, true);
    }

    for (int i = 0; i < gts.size(); i++) {
        GT &gt = gts[i];
        if (gt.contour.size() == 0) continue;

        const Mem1<Mesh2> meshes = divPolygon(gt.contour);
        Render::fill(meshes, getCol3(gt.label), 3.0f);
    }

    if (m_ptr->m_focus != NULL) {
        Render::line(m_ptr->m_focus->contour, RENDER_HIGH, 3.0f, true);
        Render::point(m_ptr->m_focus->contour, RENDER_HIGH, 7.0f);
    }

}

void GTMakerGUI::OrdrEditor::mouseButton() {
}

void GTMakerGUI::OrdrEditor::mousePos() {
}

