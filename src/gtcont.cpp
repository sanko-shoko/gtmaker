#include "gtmaker.h"

using namespace sp;

//--------------------------------------------------------------------------------
// global
//--------------------------------------------------------------------------------

Mem1<Vec2> *g_crnt;

Vec2 *g_select;

//--------------------------------------------------------------------------------
// function
//--------------------------------------------------------------------------------

void autoContour(Mem1<Vec2> &contour, const Mem2<Col3> &img, const Rect2 &rect) {
    //const int unit = min(rect.dsize[0], rect.dsize[1]) / 5;

    //Mem1<Vec2> tmp;
    //snake(tmp, img, getVtx2(rect), unit);
    //if (tmp.size() > 10) {
    //    contour = tmp;
    //}
}


//--------------------------------------------------------------------------------
// member
//--------------------------------------------------------------------------------

void GTMakerGUI::ContEditor::init(){
    g_crnt = &m_ptr->m_focus->contour;
    g_select = NULL;
}

void GTMakerGUI::ContEditor::display() {
    if (m_ptr->m_focus == NULL) return;

    const GT &gt = *m_ptr->m_focus;

    if (ImGui::Begin("editor", NULL, ImGuiWindowFlags_Block)) {
        {
            const Vec2 pos = m_ptr->m_wmat * getVec2(gt.rect.dbase[0], gt.rect.dbase[1]) + getVec2(0.0, -40.0);
            ImGui::SetWindowPos(ImVec2(static_cast<float>(pos.x), static_cast<float>(pos.y)), ImGuiCond_Always);
        }

        ImGui::AlignTextToFramePadding();

        {
            ImGui::SetWindowSize(ImVec2(168.0f, 35.0f), ImGuiCond_Always);

            ImGui::Text("-");

            ImGui::SameLine();

            ImGui::Text("contour");

            ImGui::SameLine(0, 30.0f);

            if (ImGui::ButtonPopup("reset", "reset?")) {
                g_crnt->clear();
            }

            ImGui::SameLine();

            //if (ImGui::Button("auto")) {
            //    autoContour(m_focus->contour, m_img, m_focus->rect);
            //}

        }

        ImGui::End();
    }


    const Vec2 pix = invMat(m_ptr->m_wmat) * m_ptr->m_mouse.pos;

    const int findPos = m_ptr->findNearPos(m_ptr->m_focus->contour, pix);
    const int findLine = m_ptr->findNearLine(*g_crnt, pix);

    MemP<GT> &gts = m_ptr->m_database.gtsList[m_ptr->m_select];

    {
        {
            Render::line(getVtx2(m_ptr->m_focus->rect), RENDER_BASE, 3.0f, true);
        }

        Mem1<Vec2> contour = *g_crnt;
        bool loop = true;

        if (m_ptr->m_state == S_Init) {
            contour.push(*g_select);
            loop = false;
        }
        
        Render::line(contour, RENDER_HIGH, 3.0f, loop);
        Render::point(contour, RENDER_HIGH, 7.0f);
    }

    if (findPos >= 0) {
        Render::point(m_ptr->m_focus->contour[findPos], RENDER_NEAR, 7.0f);
    }
    else if (m_ptr->m_focus->contour.size() > 0 && findLine >= 0) {
        const Vec2 a = m_ptr->m_focus->contour.lacs(findLine + 0);
        const Vec2 b = m_ptr->m_focus->contour.lacs(findLine + 1);
        const Vec2 v = unitVec(a - b);

        const Vec2 nrm = getVec2(-v.y, v.x);
        const double norm = ::fabs(dotVec(nrm, a - pix));
        const double in = dotVec(v, a - pix) * dotVec(v, b - pix);
        const double thresh = 8.0 / m_ptr->m_viewScale;

        if (norm < thresh && in <= 0) {
            const Vec2 p = pix + nrm * dotVec(nrm, a - pix);
            Render::point(p, RENDER_NEAR, 7.0f);
        }
    }

}

void GTMakerGUI::ContEditor::mouseButton() {
    if (m_ptr->m_focus == NULL) return;

    const Vec2 pix = invMat(m_ptr->m_wmat) * m_ptr->m_mouse.pos;

    const int findPos = m_ptr->findNearPos(*g_crnt, pix);
    const int findLine = m_ptr->findNearLine(*g_crnt, pix);
 
    static Mem1<Vec2> edit;
    static Vec2 select;

    select = pix;

    switch (m_ptr->m_mouse.buttonL) {
    case 1:
    {
        if (m_ptr->m_state == S_Base && m_ptr->m_focus->contour.size() == 0){
            edit.clear();

            g_crnt = &edit;
            g_crnt->push(pix);
            
            g_select = &select;

            m_ptr->m_state = S_Init;
        }
        else if (m_ptr->m_state == S_Init) {
            if (findPos == 0) {
                m_ptr->m_focus->contour = edit;
                g_crnt = &m_ptr->m_focus->contour;

                g_select = NULL;

                m_ptr->m_state = S_Base;
            }
            else {
                g_crnt->push(pix);
                g_select = &select;

                m_ptr->m_state = S_Init;
            }
        }
        else if (findPos >= 0) {
            edit = m_ptr->m_focus->contour;
            g_crnt = &edit;

            g_select = &edit[findPos];
            m_ptr->m_state = S_Edit;
        }
        else if (findLine >= 0) {
            m_ptr->m_focus->contour.add(findLine + 1, pix);

            edit = m_ptr->m_focus->contour;
            g_crnt = &edit;

            g_select = &edit[findLine + 1];
            m_ptr->m_state = S_Edit;
        }
        break;
    }
    case 0:
    {
        if (m_ptr->m_state == S_Edit) {
            m_ptr->m_focus->contour = edit;
            g_crnt = &m_ptr->m_focus->contour;
            m_ptr->m_state = S_Base;
        }
        break;
    }
    }

    switch (m_ptr->m_mouse.buttonR) {
    case 1:
    {  
        if (findPos >= 0) {
            if (m_ptr->m_focus->contour.size() > 1) {
                m_ptr->m_focus->contour.del(findPos);
            }
            else {
                m_ptr->m_focus->contour.clear();
            }
            m_ptr->m_mouse.reset();
        }

        break;
    }
    case 0:
    {
        break;
    }
    }

    {
        const Rect2 rect = getRect2(m_ptr->m_img.dsize);
        const Vec2 a = getVec2(rect.dbase[0], rect.dbase[1]);
        const Vec2 b = a + getVec2(rect.dsize[0] - 1, rect.dsize[1] - 1);
        for (int i = 0; i < g_crnt->size(); i++) {
            (*g_crnt)[i].x = sp::round((*g_crnt)[i].x);
            (*g_crnt)[i].y = sp::round((*g_crnt)[i].y);

            (*g_crnt)[i].x = max((*g_crnt)[i].x, a.x);
            (*g_crnt)[i].y = max((*g_crnt)[i].y, a.y);
            (*g_crnt)[i].x = min((*g_crnt)[i].x, b.x);
            (*g_crnt)[i].y = min((*g_crnt)[i].y, b.y);
        }
    }
}

void GTMakerGUI::ContEditor::mousePos() {
    if (m_ptr->m_focus == NULL || m_ptr->m_state == S_Base) return;

    const Vec2 pix = invMat(m_ptr->m_wmat) * m_ptr->m_mouse.pos;
   
    const int findPos = m_ptr->findNearPos(m_ptr->m_focus->contour, pix);
    
    if (g_select != NULL) {
        *g_select = pix;
    }
    if (m_ptr->m_state == S_Edit) {
        m_ptr->m_focus->contour = *g_crnt;
    }
}
