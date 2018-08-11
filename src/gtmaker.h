#ifndef __GTMAKER__
#define __GTMAKER__

#include "gtutil.h"

using namespace sp;

class GTMakerGUI : public BaseWindow {

public:
    DataBase m_database;

    // selected image
    Mem2<Col3> m_img;

    // selected id
    int m_selectid;

    // focused gt
    GT *m_focus;
    
    // editor mode
    enum Mode {
        M_Rect = 0, // Rectangle
        M_Ordr = 1, // Order
        M_Cont = 2  // Contour
    };
    Mode m_mode;

    // ui state
    enum State {
        S_Base = 0,
        S_Init = 1,
        S_Edit = 2
    };
    State m_state;

    // image to window matrix
    Mat m_vmat;

public:

    GTMakerGUI() {
        ImGui::GetIO().IniFilename = NULL;
        m_mode = M_Rect;
        m_editor = NULL;

        reset();

        m_database.gtNames.push("dog");
        m_database.gtNames.push("cat");
    }

    void reset() {
        m_focus = NULL;
        m_state = S_Base;
        setMode(M_Rect);
    }

private:

    void select(const int id) {
        m_selectid = maxVal(0, minVal(m_database.imNames.size() - 1, id));

        const string path = m_database.imDir + "/" + m_database.imNames[m_selectid];
        SP_ASSERT(cvLoadImg(m_img, path.c_str()));

        reset();

        adjustImg();
    }

    void setMode(const Mode mode) {
        static RectEditor rectEditor(this);
        static OrdrEditor ordrEditor(this);
        static ContEditor contEditor(this);

        if (m_editor == NULL || (checkMode(mode) == true && mode != m_mode)) {
            switch (mode) {
            case M_Rect: m_editor = &rectEditor; break;
            case M_Cont: m_editor = &contEditor; break;
            case M_Ordr: m_editor = &ordrEditor; break;
            }

            m_mode = mode;
            m_state = S_Base;
            m_editor->init();
        }
    }

    bool checkMode(const Mode mode) {
        if (m_database.isValid() == false) false;

        if (mode == M_Cont) {
            if (m_focus == NULL) return false;
            if (m_mode == M_Rect && m_state != S_Base) return false;
        }
        if (mode == M_Ordr) {
            MemP<GT> &gts = m_database.gtsList[m_selectid];
            int cnt = 0;
            for (int i = 0; i < gts.size(); i++) {
                if (gts[i].contour.size() > 0) cnt++;
            }
            if (cnt < 2) return false;
        }
        return true;
    }

public:

    //--------------------------------------------------------------------------------
    // ui
    //--------------------------------------------------------------------------------
  
    virtual void display() {
        
        m_vmat = glGetViewMat(m_img.dsize[0], m_img.dsize[1], m_viewPos, m_viewScale);

        if (ImGui::BeginMainMenuBar()) {

            if (ImGui::BeginMenu("file")) {

                if (ImGui::MenuItem("open image dir") && m_database.open_imDir()) {
                    select(0);
                }

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        {
            glLoadView2D(m_img.dsize, m_viewPos, m_viewScale);
            glRenderImg(m_img);
        }

        if (m_database.isValid() == true) {
            
            dispData();

            m_editor->display();

            //const ImVec4 col(0.8f, 0.8f, 0.8f, 0.8f);
            //ImGui::PushStyleColor(ImGuiCol_WindowBg, col);
            //ImGui::PopStyleColor();
        }
    }

    void dispData();

    //--------------------------------------------------------------------------------
    // others
    //--------------------------------------------------------------------------------

    void adjustImg() {
        if (m_img.size() == 0) return;

        m_viewPos = getVec(100.0, 10.0);
        m_viewScale = 0.92 * minVal(static_cast<double>(m_wcam.dsize[0] - 180) / m_img.dsize[0], static_cast<double>(m_wcam.dsize[1]) / m_img.dsize[1]);
    }


    virtual void windowSize(int width, int height) {
        if (m_database.isValid() == false) return;

        adjustImg();
    }

    virtual void keyFun(int key, int scancode, int action, int mods) {
        if (m_database.isValid() == false) return;
 
        if (m_keyAction[GLFW_KEY_A] > 0) {
            select(m_selectid - 1);
        }
        if (m_keyAction[GLFW_KEY_S] > 0) {
            select(m_selectid + 1);
        }

        if (m_keyAction[GLFW_KEY_Q] > 0) {
            setMode(M_Rect);
        }
        if (m_keyAction[GLFW_KEY_W] > 0) {
            setMode(M_Cont);
        }
        if (m_keyAction[GLFW_KEY_E] > 0) {
            setMode(M_Ordr);
        }
    }

    virtual void mouseButton(int button, int action, int mods) {
        if (m_database.isValid() == false) return;
        m_editor->mouseButton();
    }

    virtual void mousePos(double x, double y) {
        if (m_database.isValid() == false) return;
        m_editor->mousePos();
    }

    virtual void mouseScroll(double x, double y) {
    }
    

    int findNearPos(const Mem1<Vec2> &pnst, const Vec2 &pix);
    int findNearLine(const Mem1<Vec2> &pnst, const Vec2 &pix);


    //--------------------------------------------------------------------------------
    // editor
    //--------------------------------------------------------------------------------

    class Editor {
    public:
        GTMakerGUI *m_ptr;
    public:
        Editor(GTMakerGUI *ptr) { m_ptr = ptr; }
        virtual void init() = 0;
        virtual void display() = 0;
        virtual void mouseButton() = 0;
        virtual void mousePos() = 0;
    };
    Editor *m_editor;

    class RectEditor : public Editor {
    public:
        RectEditor(GTMakerGUI *ptr) : Editor(ptr) {};
        virtual void init();
        virtual void display();
        virtual void mouseButton();
        virtual void mousePos();
    };
    class OrdrEditor : public Editor {
    public:
        OrdrEditor(GTMakerGUI *ptr) : Editor(ptr) {};
        virtual void init();
        virtual void display();
        virtual void mouseButton();
        virtual void mousePos();
    };
    class ContEditor : public Editor {
    public:
        ContEditor(GTMakerGUI *ptr) : Editor(ptr) {};
        virtual void init();
        virtual void display();
        virtual void mouseButton();
        virtual void mousePos();
    };
 };

#endif
