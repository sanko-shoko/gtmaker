#include "gtmaker.h"

using namespace sp;

//--------------------------------------------------------------------------------
// global
//--------------------------------------------------------------------------------

// rectangle base pos
Vec2 g_basePos;


//--------------------------------------------------------------------------------
// member
//--------------------------------------------------------------------------------

void GTMakerGUI::RectEditor::init() {
}


void GTMakerGUI::RectEditor::display() {

    MemP<GT> &gts = m_ptr->m_database.gtsList[m_ptr->m_select];

    Mem1<const char *> combolist;
    for (int i = 0; i < m_ptr->m_database.gtNames.size(); i++) {
        combolist.push(m_ptr->m_database.gtNames[i].c_str());
    }

    for (int i = 0; i < gts.size(); i++) {
        GT &gt = gts[i];

        if (&gt == m_ptr->m_focus &&  m_ptr->m_state == S_Init) continue;

        if (ImGui::Begin(strFormat("GT %p", &gt).c_str(), NULL, ImGuiWindowFlags_Block)) {
            {
                const Vec2 pos = m_ptr->m_wmat * getVec2(gt.rect.dbase[0], gt.rect.dbase[1]) + getVec2(0.0, -40.0);
                ImGui::SetWindowPos(ImVec2(static_cast<float>(pos.x), static_cast<float>(pos.y)), ImGuiCond_Always);
            }

            if (&gt != m_ptr->m_focus && ImGui::IsMouseClicked(0) == true && ImGui::IsWindowHovered() == true) {
                m_ptr->m_focus = &gt;
            }

            ImGui::AlignTextToFramePadding();

            if (&gt != m_ptr->m_focus) {
                const int wsize = (gt.label >= 0) ? static_cast<int>(m_ptr->m_database.gtNames[gt.label].size()) : 0;

                ImGui::SetWindowSize(ImVec2(wsize * 7.0f + 30.0f, 35.0f), ImGuiCond_Always);

                ImGui::Text(">");

                if (gt.label >= 0) {
                    ImGui::SameLine();
                    ImGui::Text(m_ptr->m_database.gtNames[gt.label].c_str());
                }
            }
            else {
                ImGui::SetWindowSize(ImVec2(168.0f, 35.0f), ImGuiCond_Always);

                ImGui::Text("-");

                ImGui::SameLine();

                ImGui::PushItemWidth(100);
                ImGui::Combo("", &gt.label, combolist.ptr, combolist.size());
                ImGui::PopItemWidth();

                ImGui::SameLine();

                if (ImGui::ButtonPopup("del", "delete?")) {
                    gts.free(&gt);
                    m_ptr->m_focus = NULL;
                }

            }
            ImGui::End();
        }
    }

    { // render line

        for (int i = 0; i < gts.size(); i++) {
            GT &gt = gts[i];

            Render::line(getVtx2(gt.rect), RENDER_BASE, 3.0f, true);

            //if (gt.contour.size() == 0) continue;
            //Render::line(gt.contour, RENDER_GRAY, 3.0f, true);
        }

        if (m_ptr->m_focus != NULL) {
            if (m_ptr->m_state == S_Edit) {
                Render::line(getVtx2(m_ptr->m_focus->rect), RENDER_HIGH, 3.0f, true);
            }
            else {
                Render::line(getVtx2(m_ptr->m_focus->rect), RENDER_HIGH, 3.0f, true);
                Render::point(getVtx2(m_ptr->m_focus->rect), RENDER_HIGH, 7.0f);
            }
        }
    }
}

void GTMakerGUI::RectEditor::mouseButton() {
    const Vec2 pix = invMat(m_ptr->m_wmat) * m_ptr->m_mouse.pos;

    switch (m_ptr->m_mouse.buttonL) {
    case 1:
    {
        int find = -1;
        g_basePos = pix;

        if (m_ptr->m_focus != NULL) {
            const Mem1<Vec2> pixs = getVtx2(m_ptr->m_focus->rect);
            find = m_ptr->findNearPos(pixs, pix);
            if (find >= 0) {
                g_basePos = pixs[(find + 2) % 4];
            }
        }

        if (find < 0) {
            m_ptr->m_state = S_Init;
            m_ptr->m_focus = m_ptr->m_database.gtsList[m_ptr->m_select].malloc();
            m_ptr->m_focus->init(getRect2(pix));
        }
        else {
            m_ptr->m_state = S_Edit;
        }
        break;
    }
    case 2:
    {
        break;
    }
    case 0:
    {
        if (m_ptr->m_focus == NULL) break;

        if (m_ptr->m_state == S_Init && minval(m_ptr->m_focus->rect.dsize[0], m_ptr->m_focus->rect.dsize[1]) < 10) {
            m_ptr->m_database.gtsList[m_ptr->m_select].free(m_ptr->m_focus);
            m_ptr->m_focus = NULL;
        }

        if (m_ptr->m_focus != NULL) {
            m_ptr->m_focus->rect = andRect(m_ptr->m_focus->rect, getRect2(m_ptr->m_img.dsize));
        }

        m_ptr->m_state = S_Base;

        break;
    }
    }
}
void GTMakerGUI::RectEditor::mousePos() {
    if (m_ptr->m_focus == NULL || m_ptr->m_state == S_Base) return;

    const Vec2 pix = invMat(m_ptr->m_wmat) * m_ptr->m_mouse.pos;
    m_ptr->m_focus->rect = orRect(getRect2(pix), getRect2(g_basePos));

}

