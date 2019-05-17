#ifndef __GTUTIL__
#define __GTUTIL__

#define SP_USE_IMGUI 1

#include "simplesp.h"
#include "spex/spgl.h"
#include "spex/spcv.h"

#include "tinyfiledialogs.h"

using namespace sp;

namespace ImGui {

    inline bool ButtonPopup(const char *name, const char *popup) {
        if (ImGui::Button(name)) {
            ImGui::OpenPopup(popup);
        }

        bool ret = false;
        if (ImGui::BeginPopupModal(popup, NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (ImGui::Button("ok", ImVec2(80, 0))) {
                ImGui::CloseCurrentPopup();
                ret = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("cancel", ImVec2(80, 0))) {
                ImGui::CloseCurrentPopup();
                ret = false;
            }
            ImGui::EndPopup();
        }
        return ret;
    }
}

SP_CPUFUNC string strTrim(const char *str, const char *tok = " ,\t\n\r") {
    string dst;

    const string src = str;

    const string::size_type start = src.find_first_not_of(tok);

    if (start != string::npos) {
        const string::size_type end = src.find_last_not_of(tok);
        dst = src.substr(start, end - start + 1);
    }
    return dst;
}
SP_CPUFUNC string trimDir(const char *dir) {
    char buf[SP_STRMAX];
    ::strcpy(buf, dir);

    const int n = (int)::strlen(dir);
    if (buf[n - 1] == '\\' || buf[n - 1] == '/') {
        buf[n - 1] = '\0';
    }
    return string(buf);
}


SP_CPUFUNC Mem1<string> strSplit(const char *str, const char *tok = " ,\t\n\r") {
    Mem1<string> dst;
    Mem1<char> buf(static_cast<int>(::strlen(str) + 1), str);

    const char *ret = ::strtok(buf.ptr, tok);
    while (ret) {
        dst.push(ret);
        ret = ::strtok(NULL, tok);
    }

    return dst;
}
SP_CPUFUNC Mem1<string> getFileList(const char *dir, const char *ext = NULL) {
    Mem1<string> list;

    Mem1<string> all;
#if WIN32
    WIN32_FIND_DATA fd;

    const HANDLE handle = FindFirstFile((string(dir) + "\\*.*").c_str(), &fd);
    SP_ASSERT(handle != INVALID_HANDLE_VALUE);

    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // directory
        }
        else {
            // file
            all.push(fd.cFileName);
        }
    } while (FindNextFile(handle, &fd));

    FindClose(handle);

#else
    string search_path;

    struct stat stat_buf;

    struct dirent **namelist = NULL;
    const int dirElements = scandir(dir, &namelist, NULL, NULL);

    for (int i = 0; i < dirElements; i += 1) {
        const char *name = namelist[i]->d_name;
        // skip . and ..
        if ((strcmp(name, ".\0") != 0) && (strcmp(name, "..\0") != 0)) {

            string path = dir + string(name);

            if (stat(path.c_str(), &stat_buf) == 0) {

                if ((stat_buf.st_mode & S_IFMT) == S_IFDIR) {
                    // directory
                }
                else {
                    // file
                    all.push(name);
                }
            }
            else {
                // error
            }
        }
    }
    if (namelist != NULL) {
        free(namelist);
    }
#endif
    for (int i = 0; i < all.size(); i++) {
        if (extcmp(all[i].c_str(), ext) == true) {
            list.push(all[i]);
        }
    }

    return list;
}


//--------------------------------------------------------------------------------
// data class
//--------------------------------------------------------------------------------

class GT {

public:

    // label id
    int label;

    // rect
    Rect rect;
    
    // contour
    Mem1<Vec2> contour;

    // pose
    Pose pose;

public:

    GT() {
    }

    GT(const GT &gt) {
        *this = gt;
    }

    GT& operator = (const GT &gt) {
        label = gt.label;
        rect = gt.rect;
        contour = gt.contour;
        pose = gt.pose;
        return *this;
    }

    void init(const Rect rect) {
        this->label = -1;
        this->rect = rect;
        this->contour.clear();
        this->pose = zeroPose();
    }
};

class DataBase {

public:
    // work directory
    string wkDir;

    // image directory
    string imDir;

    // image names
    Mem1<string> imNames;

    // gt names;
    Mem1<string> gtNames;

    // gt models;
    Mem1<Mem1<Mesh3> > gtModels;

    // output list
    Mem1<MemP<GT> > gtsList;

public:

    SP_CPUFUNC string getTimeStamp(const char *format = "%Y%m%d_%H%M%S") {
        char str[SP_STRMAX];
        time_t t = time(NULL);
        strftime(str, sizeof(str), format, localtime(&t));
        return string(str);
    }

    DataBase() {
        wkDir = getTimeStamp();
    }

    bool valid() {
        return (imNames.size() > 0) ? true : false;
    }

    bool open_imDir() {

        const char *path = tinyfd_selectFolderDialog("open image dir", getCrntDir());
        //const char *path = SP_DATA_DIR "/image";
        if (path == NULL) return false;

        imDir = trimDir(path);
        imNames = getFileList(path, "bmp, BMP, png, PNG, jpeg, JPEG, jpg, JPG");
        
        gtsList.resize(imNames.size());

        if (imNames.size() > 0) {
            for (int i = 0; i < imNames.size(); i++) {
                printf("%06d %s\n", i, imNames[i].c_str());
            }
        }
        else {
            printf("no image in the directory");
        }

        return valid();
    }

    bool open_wkDir() {
        static string base = getCrntDir();

        const char *path = tinyfd_selectFolderDialog("open work dir", base.c_str());
        if (path == NULL) return false;
        
        base = trimDir(path);

        wkDir = base;

        gtNames.clear();

        {
            const string path = wkDir + "/labels.csv";
                
            File file;
            if (file.open(path.c_str(), "r") == false) return false;

            file.scanf("index,name\n");

            char str[SP_STRMAX];
            while (file.gets(str) == true) {
                const Mem1<string> split = strSplit(str, ",");
                if (split.size() < 2) break;

                gtNames.push(strTrim(split[1].c_str(), " "));
            }
        }

        {
            for (int i = 0; i < imNames.size(); i++) {

                const string path = (wkDir + "/rect/" + imNames[i] + ".csv");

                MemP<GT> &gts = gtsList[i];
                gts.clear();

                File file;
                if (file.open(path.c_str(), "r") == false) continue;

                file.scanf("index,label,x,y,width,height\n");

                char str[SP_STRMAX];
                while (file.gets(str) == true) {
                    const Mem1<string> split = strSplit(str, ",");
                    if (split.size() < 6) break;

                    int buf;
                    GT &gt = *gts.malloc();
                    gt.rect.dim = 2;
                    sscanf(str, "%d,%d,%d,%d,%d,%d\n", &buf, &gt.label, &gt.rect.dbase[0], &gt.rect.dbase[1], &gt.rect.dsize[0], &gt.rect.dsize[1]);
                }
            }
        }

        {
            for (int i = 0; i < imNames.size(); i++) {

                const string path = (wkDir + "/cont/" + imNames[i] + ".csv");

                MemP<GT> &gts = gtsList[i];

                File file;
                if (file.open(path.c_str(), "r") == false) continue;

                file.scanf("index,label,x,y\n");

                char str[SP_STRMAX];

                while (file.gets(str) == true) {
                    const Mem1<string> split = strSplit(str, ",");
                    if (split.size() < 4) break;

                    int index, label;
                    Vec2 pos;

                    sscanf(str, "%d,%d,%lf,%lf\n", &index, &label, &pos.x, &pos.y);
                    
                    if (index < gts.size()) {
                        GT &gt = gts[index];
                        gt.contour.push(pos);
                    }
                }
            }
        }
        return true;
    }

    void updateLabel(const int id, const int val) {
        if (val > 0) {
            gtNames.add(id, "");
            gtModels.add(id, Mem1<Mesh3>());
        }
        else {
            gtNames.del(id);
            gtModels.del(id);
        }

        for (int i = 0; i < gtsList.size(); i++) {
            MemP<GT> &gts = gtsList[i];

            for (int j = 0; j < gts.size(); j++) {
                GT &gt = gts[j];
                if (gt.label == id && val < 0) {
                    gt.label = -1;
                    continue;
                }
                if (gt.label >= id) {
                    gt.label += val;
                }
            }
        }

    }

    void save() {

        {
            makeDir(wkDir.c_str());

            const string path = wkDir + "/labels.csv";
            
            File file;
            SP_ASSERT(file.open(path.c_str(), "w"));

            file.printf("index,name\n");

            for (int i = 0; i < gtNames.size(); i++) {
                file.printf("%d,%s\n", i, gtNames[i].c_str());
            }
        }


        {
            makeDir((wkDir + "/rect").c_str());
            for (int i = 0; i < gtsList.size(); i++) {

                const string path = wkDir + "/rect/" + imNames[i] + ".csv";

                MemP<GT> &gts = gtsList[i];

                if (gts.size() == 0) {
                    remove(path.c_str());
                    continue;
                }

                File file;
                SP_ASSERT(file.open(path.c_str(), "w"));

                file.printf("index,label,x,y,width,height\n");

                for (int j = 0; j < gts.size(); j++) {
                    const GT &gt = gts[j];
                    file.printf("%d,%d,%d,%d,%d,%d\n", j, gt.label, gt.rect.dbase[0], gt.rect.dbase[1], gt.rect.dsize[0], gt.rect.dsize[1]);
                }
            }
        }

        {
            makeDir((wkDir + "/cont").c_str());
            for (int i = 0; i < gtsList.size(); i++) {

                const string path = wkDir + "/cont/" + imNames[i] + ".csv";

                MemP<GT> &gts = gtsList[i];

                if (gts.size() == 0) {
                    remove(path.c_str());
                    continue;
                }

                File file;
                SP_ASSERT(file.open(path.c_str(), "w"));

                file.printf("index,label,x,y\n");

                for (int j = 0; j < gts.size(); j++) {
                    const GT &gt = gts[j];
                    for (int k = 0; k < gt.contour.size(); k++) {
                        file.printf("%d,%d,%lf,%lf\n", j, gt.label, gt.contour[k].x, gt.contour[k].y);
                    }
                }
            }
        }
    }

};
SP_CPUFUNC Mem<Vec2> getVtx2(const Rect &rect) {
    Mem1<Vec2> vtxs;
    if (rect.dim >= 2) {
        vtxs.push(getVec2(rect.dbase[0], rect.dbase[1]));
        vtxs.push(getVec2(rect.dbase[0], rect.dbase[1] + rect.dsize[1]));
        vtxs.push(getVec2(rect.dbase[0] + rect.dsize[0], rect.dbase[1] + rect.dsize[1]));
        vtxs.push(getVec2(rect.dbase[0] + rect.dsize[0], rect.dbase[1]));
    }
    return vtxs;
}


//--------------------------------------------------------------------------------
// render
//--------------------------------------------------------------------------------

class Render{

#define RENDER_BASE getCol3( 80, 180, 160)
#define RENDER_HIGH getCol3(220, 240, 220)
#define RENDER_GRAY getCol3(180, 180, 180)
#define RENDER_NEAR getCol3(160, 160, 250)

public:
 
    static void point(const Vec2 &a, const Col3 &col, const float size) {
        for (int s = 0; s < 2; s++) {
            glPointSize(s == 0 ? size : size - 2.0f);

            glBegin(GL_POINTS);
            glColor(s == 0 ? col / 3.0 : col);
            glVertex(a);
            glEnd();
        }
    }
    static void point(const Mem1<Vec2> &pnts, const Col3 &col, const float size) {
        for (int i = 0; i < pnts.size(); i++) {
            point(pnts[i], col, size);
        }
    }

    static void line(const Mem1<Vec2> &vtxs, const Col3 &col, const float size, const bool loop = false) {

        for (int s = 0; s < 2; s++) {
            glLineWidth(s == 0 ? size : size - 2.0f);
            glColor(s == 0 ? col / 3.0 : col);
            glLine(vtxs, loop);
        }
    };

    static void fill(const Rect &rect, const Col3 &col, const float size) {
        const Mem1<Vec2> vtxs = getVtx2(rect);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBegin(GL_TRIANGLE_FAN);

        glColor(getCol4(col, 220));
        glVertex(vtxs[0]);
        glVertex(vtxs[1]);
        glVertex(vtxs[2]);
        glVertex(vtxs[3]);
        glEnd();

        glDisable(GL_BLEND);
    }

    static void fill(const Mem1<Mesh2> &meshes, const Col3 &col, const float size) {
        glEnable(GL_BLEND);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor(getCol4(col, 220));
        for (int i = 0; i < meshes.size(); i++) {
            glMesh(meshes[i]);
        }

        glDisable(GL_BLEND);
    }
};

#endif