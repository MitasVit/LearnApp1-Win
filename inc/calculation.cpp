#include "calculation.h"

using json = nlohmann::json;


using namespace std;
using namespace placeholders;

size_t callbackfunction(void* ptr, size_t size, size_t nmemb, void* userdata);

bool download_img(const char* url, const char* path);

void remove_string(std::string* src, const std::string& what) {
    size_t pos = 0;
    while ((pos = src->find(what, pos)) != std::string::npos) {
        src->erase(pos, what.length());
    }
}

void change_encoding(std::string* a) {

    for (int i = 0; i < a->length(); i++) {
        if (a->at(i) == '/') {
            a->at(i) = ' ';
            a->insert(i, "%2F");
        }
        else if (a->at(i) == '\\') {
            a->at(i) = ' ';
            a->insert(i, "%5C");
        }
        else if (a->at(i) == '&') {
            a->at(i) = ' ';
            a->insert(i, "%26");
        }
        else if (a->at(i) == '%') {
            a->at(i) = ' ';
            a->insert(i, "%25");
        }
        else if (a->at(i) == '=') {
            a->at(i) = ' ';
            a->insert(i, "%3D");
        }
        else if (a->at(i) == '(') {
            a->at(i) = ' ';
            a->insert(i, "%5B");
        }
        else if (a->at(i) == ')') {
            a->at(i) = ' ';
            a->insert(i, "%5D");
        }
        else if (a->at(i) == '[') {
            a->at(i) = ' ';
            a->insert(i, "%28");
        }
        else if (a->at(i) == ']') {
            a->at(i) = ' ';
            a->insert(i, "%29");
        }
        else if (a->at(i) == '{') {
            a->at(i) = ' ';
            a->insert(i, "%7B");
        }
        else if (a->at(i) == '}') {
            a->at(i) = ' ';
            a->insert(i, "%7D");
        }
    }
}

//downloads compiled latex
// latex     - LaTeX source
// outFile   - output file to write the output image
// outFormat - format of image that is outputed
// outScale  - percentage scale of image
bool get_latex(string latex, string outFile, string outFormat, int outScale) {
    try {
        curlpp::Easy request;

        request.setOpt(new curlpp::options::Url("https://latex.unacademy.com/convert"));
        request.setOpt(new curlpp::options::Verbose(true));

        change_encoding(&latex);
        string* query = new string("latexInput=" + latex + "&outputFormat=" + outFormat + "&outputScale=" + to_string(outScale) + "%");
        cout << "\033[32m" << "Full query: " << query->c_str() << "\033[0m" << endl;
        request.setOpt(new curlpp::options::PostFields(query->c_str()));

        stringstream* ss = new stringstream();

        request.setOpt(new curlpp::options::WriteStream(ss));
        request.perform();

        json d = json::parse(ss->str());
        string* imgurl = new string(d["imageURL"]);
        string* nname = new string("");
        *nname = *imgurl;
        remove_string(nname, "https://static.uacdn.net/equestions/");

        if (outFile == "") {
            if (!download_img(imgurl->c_str(), nname->c_str())) {
                delete imgurl;
                delete nname;
                delete ss;
                delete query;
                return false;
            }
        }
        else {
            if (!download_img(imgurl->c_str(), outFile.c_str())) {
                delete imgurl;
                delete nname;
                delete ss;
                delete query;
                return false;
            }
        }


        delete imgurl;
        delete nname;
        delete ss;
        delete query;
        return true;
    }
    catch (curlpp::LogicError& e) {
        return false;
    }
    catch (curlpp::RuntimeError& e) {
        return false;
    }
    catch (exception& e) {
        return false;
    }
}


size_t callbackfunction(void* ptr, size_t size, size_t nmemb, void* userdata)
{
    FILE* stream = (FILE*)userdata;
    if (!stream)
    {
        printf("!!! No stream\n");
        return 0;
    }

    size_t written = fwrite((FILE*)ptr, size, nmemb, stream);
    return written;
}

//download image from url and saves it to path
bool download_img(const char* url, const char* path)
{
    FILE* fp = fopen(path, "wb");
    if (!fp)
    {
        printf("!!! Failed to create file on the disk\n");
        return false;
    }

    CURL* curlCtx = curl_easy_init();
    curl_easy_setopt(curlCtx, CURLOPT_URL, url);
    curl_easy_setopt(curlCtx, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curlCtx, CURLOPT_WRITEFUNCTION, callbackfunction);
    curl_easy_setopt(curlCtx, CURLOPT_FOLLOWLOCATION, 1);

    CURLcode rc = curl_easy_perform(curlCtx);
    if (rc)
    {
        printf("!!! Failed to download: %s\n", url);
        return false;
    }

    long res_code = 0;
    curl_easy_getinfo(curlCtx, CURLINFO_RESPONSE_CODE, &res_code);
    if (!((res_code == 200 || res_code == 201) && rc != CURLE_ABORTED_BY_CALLBACK))
    {
        printf("!!! Response code: %d\n", res_code);
        return false;
    }

    curl_easy_cleanup(curlCtx);

    fclose(fp);

    return true;
}


ImVec2 get_text_size(ImFont* font, const char* text) {
    return font->CalcTextSizeA(font->FontSize, FLT_MAX, 0.0f, text, NULL, NULL);
}


Text::Text(ImVec2 pos_, string text_, ImFont* font_, ImU32 col_)
{
    pos = pos_;
    text = text_;
    font = font_;
    col = col_;
}
void Text::SetPos(ImVec2 pos_)
{
    pos = pos_;
}
void Text::SetPos(double x, double y)
{
    pos = ImVec2(x, y);
}
void Text::SetFont(ImFont* font_) {
    font = font_;
}
void Text::SetCol(ImU32 col_) {
    col = col_;
}
void Text::SetText(string text_) {
    text = text_;
}
void Text::Draw(ImDrawList* dx)
{
    ImGui::PushFont(font);
    dx->AddText(pos, col, text.c_str());
    ImGui::PopFont();
}
bool Text::IsHovered(ImVec2 cursorpos)
{
    ImVec2 size = get_text_size(font, text.c_str());
    if ((cursorpos.x >= pos.x) && (cursorpos.y >= pos.y) && (cursorpos.x <= (pos.x + size.x)) && (cursorpos.y <= (pos.y + size.y)))
    {
        return true;
    }
    return false;
}
//is hovered but seting the colision rect
bool Text::IsHovered(ImVec2 cursorpos, ImVec2 leftup, ImVec2 rightdown)
{
    if ((cursorpos.x >= leftup.x) && (cursorpos.y >= leftup.y) && (cursorpos.x <= (leftup.x + rightdown.x)) && (cursorpos.y <= (leftup.y + rightdown.y)))
    {
        return true;
    }
    return false;
}
bool Text::IsClicked(ImVec2 cursorpos)
{
    ImVec2 size = get_text_size(font, text.c_str());
    if ((cursorpos.x >= pos.x) && (cursorpos.y >= pos.y) && (cursorpos.x <= (pos.x + size.x)) && (cursorpos.y <= (pos.y + size.y)))
    {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            return true;
        }
    }
    return false;
}


long to_dec(const string& hex) {
    return strtol(hex.c_str(), nullptr, 16);
}

RegistredTags::RegistredTags() {

}

RegistredTags::RegistredTags(initializer_list<tag> _tags) {
    for (const auto& p : _tags) {
        tags.insert({ p.id, p.url });
    }
}

RegistredTags::RegistredTags(vector<tag> _tags) {
    for (const auto& p : _tags) {
        tags.insert({ p.id, p.url });
    }
}

RegistredTags::RegistredTags(vector<unsigned char> id, vector<string> url) {
    for (int i = 0; i < id.size(); i++) {
        tags.insert({ id.at(i), url.at(i) });
    }
}

RegistredTags::~RegistredTags() {
    tags.clear();
}

void RegistredTags::AddTag(tag t) {
    tags.insert({ t.id, t.url });
}

void RegistredTags::AddTag(unsigned char id, string url) {
    tags.insert({ id, url });
}

bool RegistredTags::RegisterTag(tag t) {
    //checking for id and url duplicates
    for (const auto& p : tags) {
        if (p.first == t.id) {
            return false;
        }
        if (p.second == t.url) {
            return false;
        }
    }
    tags.insert({ t.id, t.url });
    return true;
}

bool RegistredTags::RegisterTag(unsigned char id, string url) {
    //checking for id and url duplicates
    for (const auto& p : tags) {
        if (p.first == id) {
            return false;
        }
        if (p.second == url) {
            return false;
        }
    }
    tags.insert({ id, url });
    return true;
}

bool RegistredTags::RemoveTag(tag t) {
    return (tags.erase(t.id) > 0) ? true : false;
}

bool RegistredTags::RemoveTag(unsigned char id, string url) {
    return (tags.erase(id) > 0) ? true : false;
}

bool RegistredTags::RemoveTag(unsigned char id) {
    return (tags.erase(id) > 0) ? true : false;
}

bool RegistredTags::RemoveTag(string url) {
    auto it = find_if(tags.begin(), tags.end(), [&url](const auto& pair) { return pair.second == url; });
    if (it != tags.end()) {
        return (tags.erase(it->first) > 0) ? true : false;
    }
    return false;
}

bool RegistredTags::HaveDuplicateIDs() {
    set<unsigned char> seenKeys;
    for (const auto& kv : tags) {
        if (seenKeys.count(kv.first) > 0) {
            return true;
        }
        seenKeys.insert(kv.first);
    }
    return false;
}

bool RegistredTags::HaveDuplicateURLs() {
    set<string> seenValues;
    for (const auto& kv : tags) {
        if (seenValues.count(kv.second) > 0) {
            return true;
        }
        seenValues.insert(kv.second);
    }
    return false;
}

int RegistredTags::FindByUrl(string url) {
    auto it = find_if(tags.begin(), tags.end(), [&](const auto& pair) { return pair.second == url; });
    if (it != tags.end()) {
        return distance(tags.begin(), it);
    }
    return -1;
}

int RegistredTags::FindById(unsigned char id) {
    auto it = tags.find(id);
    if (it != tags.end()) {
        return distance(tags.begin(), it);
    }
    return -1;
}

tag* RegistredTags::GetTag(unsigned char id) {
    auto it = tags.find(id);
    if (it != tags.end()) {
        return new tag{ it->first, it->second };
    }
    return NULL;
}

tag* RegistredTags::GetTag(string url) {
    auto it = find_if(tags.begin(), tags.end(), [&](const auto& pair) { return pair.second == url; });
    if (it != tags.end()) {
        return new tag{ it->first, it->second };
    }
    return NULL;
}

unsigned char RegistredTags::GetTagID(string url) {
    auto it = find_if(tags.begin(), tags.end(), [&](const auto& pair) { return pair.second == url; });
    if (it != tags.end()) {
        return it->first;
    }
    throw invalid_argument("cannot return tag coresponding to bad/nonexisting url");
}

string RegistredTags::GetTagUrl(unsigned char id) {
    auto it = tags.find(id);
    if (it != tags.end()) {
        return it->second;
    }
    return "";
}

bool RegistredTags::ExistTag(unsigned char id) {
    auto it = tags.find(id);
    if (it != tags.end()) {
        return true;
    }
    return false;
}

bool RegistredTags::ExistTag(string url) {
    auto it = find_if(tags.begin(), tags.end(), [&](const auto& pair) { return pair.second == url; });
    if (it != tags.end()) {
        return true;
    }
    return false;
}

bool RegistredTags::EditTagID(unsigned char id, string url) {
    auto it = find_if(tags.begin(), tags.end(), [&](const auto& pair) { return pair.second == url; });
    if (it != tags.end()) {
        tags.erase(it);
        tags.emplace(id, url);
        return true;
    }
    return false;
}
bool RegistredTags::EditTagURL(unsigned char id, string url) {
    auto it = tags.find(id);
    if (it != tags.end()) {
        tags.at(distance(tags.begin(), it)) = url;
        return true;
    }
    return false;
}

int RegistredTags::GetTagCount() {
    return tags.size();
}

vector<uchar> RegistredTags::ExtractIDs() {
    vector<uchar> a;
    transform(tags.begin(), tags.end(), back_inserter(a), [](const auto& pair) { return pair.first; });
    return a;
}

uint RegistredTags::ComputeTags() {
    vector<uchar> src = ExtractIDs();
    size_t size = src.size();
    vector<uint> tmp;
    for (int i = 0; i < src.size(); i++) {
        tmp.push_back(((uint)(src.at(i)) << ((size - (i + 1)) * 8)));
    }
    uint t = tmp.at(0);
    for (int i = 1; i < tmp.size(); i++) {
        t |= tmp.at(i);
    }
    return t;
}

vector<uchar> RegistredTags::GetIDTags(uint tags_col, int tag_count) {
    vector<uchar> tmp;
    for (int i = 0; i < tag_count; i++) {

        char* s = new char[tag_count * 2];
        for (int j = 0; j < (tag_count * 2); j++) {
            s[j] = '0';
        }
        s[((i + 1) * 2) - 2] = 'F';
        s[((i + 1) * 2) - 1] = 'F';
        string hex(s);
        //(src & 0xFF000000) >> 24;
        tmp.push_back((tags_col & to_dec(s)) >> ((tag_count - (i + 1)) * 8));

    }
    return tmp;
}

bool RegistredTags::Empty() {
    return tags.empty();
}

void RegistredTags::EraseAll() {
    tags.clear();
}

void RegistredTags::Delete() {
    tags.clear();
}



uint genN(vector<uchar> src) {
    size_t size = src.size();
    vector<uint> tmp;
    for (int i = 0; i < src.size(); i++) {
        tmp.push_back(((uint)(src.at(i)) << ((size - (i + 1)) * 8)));
    }
    uint t = tmp.at(0);
    for (int i = 1; i < tmp.size(); i++) {
        t |= tmp.at(i);
    }
    return t;
}

vector<uchar> getN(uint src, int N) {
    vector<uchar> tmp;
    for (int i = 0; i < N; i++) {

        char* s = new char[N * 2];
        for (int j = 0; j < (N * 2); j++) {
            s[j] = '0';
        }
        s[((i + 1) * 2) - 2] = 'F';
        s[((i + 1) * 2) - 1] = 'F';
        string hex(s);
        //(src & 0xFF000000) >> 24;
        tmp.push_back((src & to_dec(s)) >> ((N - (i + 1)) * 8));

    }
    return tmp;
}

/*io2.Fonts->AddFontFromFileTTF(path.c_str(), 16.0f*/

Calculation::Calculation(string font_folder) {
    LoadFonts(font_folder);
}


Calculation::Calculation(string font_folder, string title, string subtitle, string text, string formula, string create_date, string mod_date, tag* _tags) {
    LoadFonts(font_folder);
    SetTitle(title);
    SetSubtitle(subtitle);
    SetText(text);
    SetFormula(formula);
    SetCreateDate(create_date);
    SetModDate(mod_date);
    tags = _tags;
}

void Calculation::OnTagChange() {
    //recrate the vector fill of buttons with tags
    tag_buttons.clear();
    for (int i = 0; i < (sizeof(tags) / sizeof(tag)); i++) {
        tag_buttons.push_back(new Text(ImVec2(0, 0), tags[i].url, font_tag, IM_COL32(255, 255, 255, 255)));
    }
}

void Calculation::LoadFonts(string font_folder) {
    ImGuiIO& io2 = ImGui::GetIO();
    string path = font_folder + "/Roboto/Roboto-Bold.ttf";
    string path2 = font_folder + "/Roboto/Roboto-Medium.ttf";
    string path3 = font_folder + "/NotoSansMath-Regular.ttf";
    string path4 = font_folder + "/RobotoSerif/RobotoSerif-ExtraLightItalic.ttf";
    font_title = io2.Fonts->AddFontFromFileTTF(path.c_str(), 35.0f);
    font_text = io2.Fonts->AddFontFromFileTTF(path2.c_str(), 18.0f);
    font_subtitle = io2.Fonts->AddFontFromFileTTF(path.c_str(), 20.0f);
    font_formula = io2.Fonts->AddFontFromFileTTF(path3.c_str(), 50.0f);
    font_date = io2.Fonts->AddFontFromFileTTF(path2.c_str(), 20.0f);

    font_tag = io2.Fonts->AddFontFromFileTTF(path4.c_str(), 20.0f);
}

void Calculation::SetTitle(string _title) {
    title = _title;
}
void Calculation::SetSubtitle(string _subtitle) {
    subtitle = _subtitle;
}
void Calculation::SetText(string _text) {
    text = _text;
}
void Calculation::SetFormula(string _formula) {
    formula = _formula;
}
void Calculation::SetCreateDate(string _create_date) {
    create_date = _create_date;
}
void Calculation::SetModDate(string _mod_date) {
    mod_date = _mod_date;
}

string Calculation::GetTitle() {
    return title;
}
string Calculation::GetSubtitle() {
    return subtitle;
}
string Calculation::GetText() {
    return text;
}
string Calculation::GetFormula() {
    return formula;
}
string Calculation::GetCreateDate() {
    return create_date;
}
string Calculation::GetModDate() {
    return mod_date;
}

void Calculation::Render(ImDrawList* dx, int wwidth, int wheight) {
    //on full screen
    ImVec2 pos = ImVec2((wwidth / 100) * 6, (wheight / 100) * 17);
    ImVec2 size = ImVec2(wwidth - ((wwidth / 100) * 12), wheight - ((wheight / 100) * 17));
    ImU32 bgcol = IM_COL32(0, 43, 110, 255);
    dx->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), bgcol, 5.0f);

    //left panel of formula editor size: 
    // 10 left + 10 right - margin
    // 25% - total size with margin
    dx->AddText(font_title, font_title->FontSize, ImVec2(pos.x + 10, pos.y + 10), IM_COL32(255, 255, 255, 255), title.c_str(), NULL, ((size.x / 100) * 25) - 20);
    dx->AddText(font_text, font_text->FontSize, ImVec2(pos.x + 10, pos.y + 10 + font_title->CalcTextSizeA(font_title->FontSize, FLT_MAX, ((size.x / 100) * 25) - 20, title.c_str()).y), IM_COL32(255, 255, 255, 255), text.c_str(), NULL, ((size.x / 100) * 25) - 20);

    //middle panel of formula editor size:
    // 10 left + 10 right - margin
    // 50% - total size with margin
    ImVec2 formula_size = font_formula->CalcTextSizeA(font_formula->FontSize, FLT_MAX, NULL, formula.c_str());
    //centered formula
    dx->AddText(ImVec2(((size.x / 100) * 25) + 10 + ((((size.x / 100) * 50) - 20) - formula_size.x), pos.y + 10), IM_COL32(255, 255, 255, 255), formula.c_str());

    //centered moddate
    string date = "Last modified: " + mod_date;
    ImVec2 date_size = font_date->CalcTextSizeA(font_date->FontSize, FLT_MAX, 0, date.c_str());
    dx->AddText(ImVec2(((size.x / 100) * 25) + 10 + ((((size.x / 100) * 50) - 20) - date_size.x), wheight - date_size.y - 5), IM_COL32(255, 255, 255, 255), date.c_str());

    //todo: tags = buttons?
    ImVec2 prev_pos = ImVec2((wwidth / 100) * 75, ((wheight / 100) * 15) + 10);
    for (int i = 0; i < tag_buttons.size(); i++) {
        //computing size
        tag_buttons[i]->SetPos(ImVec2(prev_pos.x + 10, prev_pos.y));
        prev_pos.x = prev_pos.x + tag_buttons[i]->font->CalcTextSizeA(tag_buttons[i]->font->FontSize, FLT_MAX, NULL, tag_buttons[i]->text.c_str()).x;
        if (i != (tag_buttons.size() - 1)) {
            if ((prev_pos.x >= wwidth) || ((prev_pos.x + 10 + tag_buttons[i + 1]->font->CalcTextSizeA(tag_buttons[i + 1]->font->FontSize, FLT_MAX, NULL, tag_buttons[i + 1]->text.c_str()).x) >= wwidth)) {
                prev_pos.x = (wwidth / 100) * 75;
                prev_pos.y = prev_pos.y + 50;
            }
        }
    }

}

void Calculation::RenderAsComumn() {
    //only preview
    if (ImGui::GetColumnIndex() == 0)
        ImGui::Separator();

    ImGui::PushFont(font_title);
    ImGui::Text(title.c_str());
    ImGui::PopFont();

    ImGui::PushFont(font_subtitle);
    ImGui::Text(subtitle.c_str());
    ImGui::PopFont();

    ImGui::PushFont(font_date);
    ImGui::TextColored(ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f }, ("Last modified: " + mod_date).c_str());
    ImGui::PopFont();

    //ImGui::PushFont(font_text);
   // ImGui::TextWrapped(text.c_str());
   // ImGui::PopFont();

    ImGui::Button("Open", ImVec2(-FLT_MIN, 0.0f));
    ImGui::NextColumn();
}

void Calculation::RenderPrev(ImDrawList* dx, vec4 rc) {
    //todo...
}
