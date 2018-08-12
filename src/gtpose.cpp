#include "gtmaker.h"

using namespace sp;

//--------------------------------------------------------------------------------
// global
//--------------------------------------------------------------------------------



//--------------------------------------------------------------------------------
// member
//--------------------------------------------------------------------------------

void GTMakerGUI::PoseEditor::init() {
}


void GTMakerGUI::PoseEditor::display() {
    if (m_ptr->m_focus == NULL) return;

    const GT &gt = *m_ptr->m_focus;

    if (ImGui::Begin("editor", NULL, ImGuiWindowFlags_Block)) {
        {
            const Vec2 pos = m_ptr->m_wmat * getVec(gt.rect.dbase[0], gt.rect.dbase[1]) + getVec(0.0, -40.0);
            ImGui::SetWindowPos(ImVec2(static_cast<float>(pos.x), static_cast<float>(pos.y)), ImGuiCond_Always);
        }

        ImGui::AlignTextToFramePadding();

        {
            ImGui::SetWindowSize(ImVec2(168.0f, 35.0f), ImGuiCond_Always);

            ImGui::Text("-");

            ImGui::SameLine();

            ImGui::Text("contour");

            ImGui::SameLine(0, 30.0f);

            if (ImGui::Button("load ply")) {

            }

            ImGui::SameLine();

            //if (ImGui::Button("auto")) {
            //    autoContour(m_focus->contour, m_img, m_focus->rect);
            //}

        }

        ImGui::End();
    }


    const Vec2 pix = invMat(m_ptr->m_wmat) * m_ptr->m_mouse.pos;

    MemP<GT> &gts = m_ptr->m_database.gtsList[m_ptr->m_select];

    {
        {
            Render::line(getVtx2(m_ptr->m_focus->rect), RENDER_BASE, 3.0f, true);
        }

    }
}

void GTMakerGUI::PoseEditor::mouseButton() {

}
void GTMakerGUI::PoseEditor::mousePos() {
    if (m_ptr->m_focus == NULL || m_ptr->m_state == S_Base) return;

}

