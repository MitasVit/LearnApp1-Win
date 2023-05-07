#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <cerrno>
#include <iostream>
#include <sstream>
#include <fstream>

#include <cstdlib>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <list>

#define WIN32_LEAN_AND_MEAN
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Multi.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <imgui.h>

using namespace std;
using namespace placeholders;

void remove_string(std::string* src, const std::string& what);

void change_encoding(std::string* a);

//downloads compiled latex
// latex     - LaTeX source
// outFile   - output file to write the output image
// outFormat - format of image that is outputed - {PNG, SVG, JPG}
// outScale  - percentage scale of image
bool get_latex(string latex = "\\sqrt{2e}", string outFile = "", string outFormat = "PNG", int outScale = 125);

//intern - for download img
size_t callbackfunction(void* ptr, size_t size, size_t nmemb, void* userdata);

//download image/file from url and saves it to path
bool download_img(const char* url, const char* path);


typedef unsigned int uint;
typedef unsigned char uchar;

struct vec4 {
    int x, y, w, h;
};

//budu pouzivat hlavne v ostatnich tridach krome Registred tags
struct tag {
    /*
    8 - id - (0-255) - max 255 tagu?
    */
    unsigned char id;
    string url;//zatim pouze jmeno tagu
    tag(uchar _id, string _url) {
        id = _id;
        url = _url;
    }
    tag() {
        id = 0;
        url = "";
    }
};


ImVec2 get_text_size(ImFont* font, const char* text);

class Text
{
public:
    ImVec2 pos;
    string text;
    ImFont* font;
    ImU32 col;
    Text(ImVec2 pos_, string text_, ImFont* font_, ImU32 col_);
    void SetPos(ImVec2 pos_);
    void SetPos(double x, double y);
    void SetFont(ImFont* font_);
    void SetCol(ImU32 col_);
    void SetText(string text_);
    void Draw(ImDrawList* dx);
    bool IsHovered(ImVec2 cursorpos);
    //is hovered but seting the colision rect
    bool IsHovered(ImVec2 cursorpos, ImVec2 leftup, ImVec2 rightdown);
    bool IsClicked(ImVec2 cursorpos);
};


//converts hex number to dec
long to_dec(const std::string& hex);

class RegistredTags {
public:
    map<unsigned char, string> tags;

    RegistredTags();
    RegistredTags(initializer_list<tag> _tags);
    //RegistredTags(initializer_list<unsigned char> id, initializer_list<string> url);
    RegistredTags(vector<tag> _tags);
    RegistredTags(vector<unsigned char> id, vector<string> url);

    ~RegistredTags();


    void AddTag(tag t);
    void AddTag(unsigned char id, string url);

    //same as addtag but checks if not tag exist or id is not registred
    //return true if registring was ok
    bool RegisterTag(tag t);
    bool RegisterTag(unsigned char id, string url);

    //remove tag from registred tags
    //return true if deleting was ok
    //can delete tag by id or url
    bool RemoveTag(tag t);
    bool RemoveTag(unsigned char id, string url);
    bool RemoveTag(unsigned char id);
    bool RemoveTag(string url);

    //return if there exists some duplicate ids or urls
    bool HaveDuplicateIDs();
    bool HaveDuplicateURLs();

    //return pos if found
    //-1 if not
    //it is assumed that there is only one tag with this url/id
    int FindByUrl(string url);
    int FindById(unsigned char id);

    //like Find but return the tag on this pos
    //a if not found
    tag* GetTag(unsigned char id);
    tag* GetTag(string url);

    //return 0 or ""
    unsigned char GetTagID(string url);
    string GetTagUrl(unsigned char id);

    bool ExistTag(unsigned char id);
    bool ExistTag(string url);


    //edit tag id for tag with name "url"
    //return false if not ok
    bool EditTagID(unsigned char id, string url);
    bool EditTagURL(unsigned char id, string url);

    int GetTagCount();

    vector<uchar> ExtractIDs();

    //for SQL database
    //put this in 'tags' column
    uint ComputeTags();

    //for SQL database
    //tags_col  - computed value for 'tags' column
    //tag_count - count of tags
    //return IDs tags from SQL
    vector<uchar> GetIDTags(uint tags_col, int tag_count);

    //return true if there aren't tags 
    bool Empty();

    //these function deletes or erases all registred tags
    void EraseAll();
    void Delete();

    //other function see README at the end

};


//generate uint for SQL from tag ids
uint genN(vector<uchar> src);

//get tag ids from SQL
vector<uchar> getN(uint src, int N);

enum class unit_prefix_types
{
    // yocto = 1,
    // zepto = 2,
    atto = 3,
    femto = 4,
    piko = 5,
    nano = 6,
    mikro = 7,
    mili = 8,
    centi = 9,
    deci = 10,
    deka = 11,
    hekto = 12,
    kilo = 13,
    mega = 14,
    giga = 15,
    tera = 16,
    peta = 17,
    exa = 18,
    // zetta = 19,
   //  yotta = 20
};

struct unit_prefix {
    unit_prefix_types type;
    string str;
    /*
    value of the unit
    examples:
    deci - 10-1  -> 10 powered on -1  -> 0,1
    deka - 10+1  -> 10 powered on +1  -> 10
    peta - 10+15 -> 10 powered on +15 -> 1000000000000000
    piko - 10-12 -> 10 powered on -12 -> 0,000000000001
    */
    string value;
};

struct unit {
    unit_prefix pre;
    string pre_str;
};

unit_prefix id2abbrev(unit_prefix_types pre, int val);
/*
atto	a	10-18
femto	f	10-15
piko	p	10-12
nano	n	10-9
mikro	μ	10-6
mili	m	10-3
centi	c	10-2
deci	d	10-1
deka	da	10+1
hekto	h	10+2
kilo	k	10+3
mega	M	10+6
giga	G	10+9
tera	T	10+12
peta	P	10+15
exa	    E	10+18

*/



//the calculation which will be on full screen, position:
// top: 15% 
// left: 5%
// right: 5%
class Calculation {
private:
    vector<Text*> tag_buttons;
public:
    string title, subtitle, text, formula, create_date, mod_date;//date when it was modified
    tag* tags;
    static RegistredTags* registred_tags;//for connecting with tags

    ImFont* font_title, * font_subtitle, * font_text, * font_formula, * font_date, * font_tag;

    //recreates buttons for tags 
    //WARNING: on constructing call after creating fonts
    void OnTagChange();

    Calculation(string font_folder);
    Calculation(string font_folder, string title, string subtitle, string text, string formula, string create_date, string mod_date, tag* _tags);

    //call once
    //ex. path: ../res/fonts
    void LoadFonts(string font_folder);

    void SetTitle(string _title);
    void SetSubtitle(string _subtitle);
    void SetText(string _text);
    void SetFormula(string _formula);
    void SetCreateDate(string _create_date);
    void SetModDate(string _mod_date);

    string GetTitle();
    string GetSubtitle();
    string GetText();
    string GetFormula();
    string GetCreateDate();
    string GetModDate();

    //w - window size, rendering on fullsize
    void Render(ImDrawList* dx, int wwidth, int wheight);

    //preview
    //paste inside imgui table
    void RenderAsComumn();

    //rc - full position + size of the preview
    void RenderPrev(ImDrawList* dx, vec4 rc);

    //render
    //render to specific rect
    //render as preview -> for dashboard
    //set variables 
    //get variable values
    //setting tags
    //formula retransofrmation
    //formula variable extract
    //unit system + unit converter
    //unit api = regist, d tag api?
    //unit{id, name, type}
    //unit types = time, lenght, force
    //unit prefix
    //unit vector = 1D, 2D, 3D
    //unit get api for updating
    //latex renderer + parser - TexLive?
    //postfix command calculator - PEGTL

    //tools
    //calculation creator
    //calculation searcher - dashboard - further function
    //calculation explorer - dashboard
    //calculation editor

    //interactive formula editor
    //left: title + subtitle, tags
    //middle: formula into which values can be inserted, every variable in formula will have drobdown tipbox and unit
    //middle down: formula log - setting values, retrasnforing, etc.
    //right: restransforing buttons, export, share
};

//http + curl helpers 


class InteractiveCalculations {
public:
    //FUNKCE:
    //load from json
    //get via my api
    //Calculation* calcs;

    //api ukazka
    //pr curl -H "Auth: token $2y$10$jDVjnJRJRjJZD/fmRIR5X.WMHn2r97Su7CxVOjCyd6uZYD/TWv2Vu" https://mitasvit.cz/api/get_calcs.php
    //vraci:
    // [{"title":"Test","subtitle":"Poslano z PHP","_text":"ahoj","formula":"F=ma","tag_count":"1","tags":"01","createdate":"2023-01-01","moddate":"2023-01-01"}]{"completed":"ok"}
};
