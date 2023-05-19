// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
//#define GL_SILENCE_DEPRECATION
//#if defined(IMGUI_IMPL_OPENGL_ES2)
//#include <GLES2/gl2.h>
//#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include "imgradient.h"
#include "imspinner.h"

#include "Header.h"
#include "imgui_plot.h"

#include "imHotKey.h"

#include <iostream>
#include <memory>
#include <string>
#include <cmath>
#include "cparse/shunting-yard.h"
#include "cparse/shunting-yard-exceptions.h"

#include "rapidcsv.h"

#include "imgui_notify.hpp"
#include "calculation.h"

#include "Task.h"

using namespace std;
using namespace cparse;

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


//my own function see: https://github.com/cparse/cparse/wiki/Defining-New-Functions
packToken fibonacci(TokenMap scope) {
    int N = scope["N"].asInt();
    if (N == 0) return 0;
    if (N == 1) return 1;

    scope["N"] = N - 1;
    int result = fibonacci(scope).asInt();
    scope["N"] = N - 2;
    return result + fibonacci(scope).asInt();
}

packToken arctan(TokenMap scope) {
    double number = scope["num"].asDouble();
    return atan(number);
}
packToken arcsin(TokenMap scope) {
    double number = scope["num"].asDouble();
    return asin(number);
}
packToken arccos(TokenMap scope) {
    double number = scope["num"].asDouble();
    return acos(number);
}
packToken goldencut(TokenMap scope) {
    return ((1 + sqrt(5)) / 1);
}

packToken factorial(TokenMap scope) {
    int N = scope["N"].asInt();
    if (N <= 0) return 0;
    if (N == 1) return 1;

    scope["N"] = N - 1;
    int result = fibonacci(scope).asInt();
    scope["N"] = N - 2;
    return result + fibonacci(scope).asInt();
}


//defined functions:
/*
print
sum
sqrt
pow
sin
tan
cos
abs
float
real
int
str
eval
type
extend
*/


#include <iostream>
#include <chrono>

class Timer {
public:

    clock_t start = 0, end = 0;
    void Start() {
        start = clock();
    }

    void End() {
        end = clock();
    }
    double GetTime2() {
        return (((double)end - start));
    }
    double GetTime() {
        return ((((double)clock() - start) / CLOCKS_PER_SEC) * 10.00);
    }
};



ImVec4 rgba(int r, int g, int b, int a) {
    return ImVec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 100.0f);
}

ImColor rgbac(int r, int g, int b, int a) {
    return ImColor((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 100.0f);
}

constexpr size_t buf_size = 512;
static float x_data[buf_size];
static float y_data1[buf_size];
static float y_data2[buf_size] = { -0.65,-1.95,-1.84,-0.91,-0.09,0.49,1.93,2.27,1.24,0.55,1.28,1.26,0.88,0.48,0.65,0.03,0.22,0.55,1.22,1.5,1.45,1.35,1.06,0.36,-0.04,0.9,1.14,0.5,0.91,0.78,-0.03,0.16,0.73,0.91,0.83,0.49,0.02,0.49,1.41,1.2,1.33,1.04,2,2.18,0.71,0.3,1.38,1.73,-0.39,-1.44,-0.75,0.93,1.54,2.73,2.38,0.56,-0.97,-0.69,0.34,1.32,1.26,1.05,0.15,-0.27,-0.33,0.15,0.96,1.05,1.99,1.81,1.66,0.89,1.03,1.21,0.41,0.16,0.44,0.22,-0.14,0.97,0.82,0.43,0.68,1.56,1.21,0.3,-0.34,0.51,0.99,1.81,2.36,1.89,0.72,0.17,-0.22,-0.3,-0.79,0.51,1.37,2.07,2.55,1.46,0.81,0.28,0.01,0.14,0.09,-0.34,-0.09,1.41,2.04,1.94,0.32,-0.43,-0.03,1.25,1.84,2.09,2.4,0.74,1.03,0.19,0.14,0.19,1.97,1.15,0.37,1.33,-1.57,1.64,5.27,-0.56,-3.37,0.39,2.02,2.07,0.47,-0.67,0.53,1.78,1.56,2.05,0.26,-2.2,-1.42,-0.01,1.48,2.01,0.75,0.27,0.39,1.06,0.56,0.77,0.47,0.94,0.42,-0.24,0.38,3.74,10.26,23.95,58.14,91.2,108.06,105.19,85.66,58.85,38.81,26.12,29.46,44.33,64.39,84.44,95.79,95.7,88.43,74.18,60.38,52.13,49.62,52.32,60.69,70.28,76.76,80.8,81.58,78.61,73.9,67.59,63.03,61.79,63.11,66.12,70.23,73.57,74.71,74.49,71.6,67.51,63.19,59.84,57.1,55.6,55.56,54.27,55.15,53.5,51.43,48.93,45.25,39.79,34.5,30.67,27.08,24.73,24.58,23.45,21.42,17.77,15.96,12.58,10.19,7.94,3.12,6.32,8.8,10.88,9.02,7.3,3.58,2.9,0.87,-1.71,-0.81,2.77,4.74,4.88,4.7,2.18,1.77,-0.04,-0.44,-0.62,1.95,1.62,1.1,1.34,1.62,0.93,1.6,-0.93,-2.55,-2.08,-2.05,-1.33,3.81,7.89,7.04,4.25,0.47,-3.02,-4.51,-5.27,-3.88,-0.6,4.99,7.65,10.67,8.22,4.12,-2.91,-5.55,-7.97,-8.63,-7.91,-4.45,-0.64,6.97,13.69,13.1,9.31,1.21,-5.39,-8.95,-10.53,-7.07,0.51,4.99,7.48,7.92,4.88,1.03,-1.44,-3.95,-5.17,-3.23,-1.32,1.24,4.67,6.62,5.32,3.87,-0.95,-4.52,-4.86,-3.05,-1.45,2.38,4.03,1.86,3.48,3.29,0.94,-0.63,-2.12,-2.32,-0.23,2.06,2.27,4.24,3.75,0.91,0.04,-0.56,-3.12,-1.12,0.34,1.3,2.06,4.77,3.2,2.33,1.06,-1.71,-0.86,-2.36,-2.8,0.12,1.06,1.12,4.35,3.26,4.08,2.59,0.16,-0.53,-0.9,0.39,0.98,0.09,2.1,0.58,-0.34,2.7,3.52,3.61,0.45,-3.93,-5.34,-3.41,0.91,3.35,3.12,3.79,1.78,0.59,0.07,-1.85,0.65,1.05,1.9,2.43,1.97,1.39,0.95,-0.14,-0.72,1.17,1.41,3.61,3.01,2.16,-0.11,-1.32,-4.14,-3.61,-1.63,1.02,2.36,4.7,5.2,2.49,1.47,0.84,0.48,0.04,0.88,0.47,2.93,3.09,0.54,0.12,-0.99,-1.41,-1.15,1.59,3.13,3.97,2.76,-0.08,-4.25,-3.91,-3.15,-2.04,-1.39,0.51,3.89,5.59,5.05,1.86,0.33,-3.87,-3.72,-4.11,-3.11,-3.39,-1.8,-2.58,-2.22,-0.35,-0.19,0.68,2.12,4.42,3.64,4.31,3.37,3.49,1.23,-0.88,-2.69,-1.96,-0.57,-0.04,0.91,0.01,1.15,1.42,1.98,1.26,2.12,2.83,3,2.45,3.36,2.17,0.1,-0.14,-0.5,1.09,0.31,1.54,2.18,1.76,-1.18,0.23,-1.43,-1.7,-0.59,1.24,2.52,3.61,5.2,3.64,4,1.52,0.75,0.31,1.69,-1,1.53,1.08,0.53,0.17,1.22,0.6,0.51,1.45,2.43,2.82,2.66,2.15,1.73,3.56,3.16,1.39,2.85,1.82,3.13,4.01,5.03,4.51,4.11,0.79,0.76,2.29,2.14,0.84,1.4 };
static float y_data3[buf_size];

calculator calc("sin(x)");
TokenMap vars2;
char in[64] = "sin(x)";
int y_size15 = 200, min_plotval = -1, maxplotval = 1, x_size15 = 128;

//opravit
void LoadData() {
    //from data.csv
    rapidcsv::Document doc("C:/Users/vitat/Desktop/programming/superimgui/imgui/data.csv");
    vector<float> a = doc.GetColumn<float>("Vertical Force (N)");
    for (int i = 0; i < 512; i++) {//jen po 512
        y_data2[i] = a.at(i);
    }
}

void generate_data() {
    constexpr float sampling_freq = 44100;
    constexpr float freq = 500;
    for (size_t i = 0; i < buf_size; ++i) {
        const float t = i / sampling_freq;
        x_data[i] = t;
        const float arg = 2 * M_PI * freq * t;
        y_data1[i] = sin(arg);
        y_data2[i] = cos(arg);
        y_data3[i] = 0;//tan(arg);
        //y_data2[i] = y_data1[i] * -0.6 + sin(2 * arg) * 0.4;
        //y_data3[i] = y_data2[i] * -0.6 + sin(3 * arg) * 0.4;
    }
}

void generate_data2(string fc) {
    calc.compile(fc.c_str());
    constexpr float sampling_freq = 44100;
    constexpr float freq = 500;
    for (size_t i = 0; i < buf_size; ++i) {
        const float t = i / sampling_freq;
        x_data[i] = t;
        const float arg = 2 * M_PI * freq * t;
        vars2["x"] = arg;
        y_data1[i] = calc.eval(vars2).asDouble();
        //y_data2[i] = calc.eval(vars2).asDouble();
        y_data3[i] = calc.eval(vars2).asDouble();//tan(arg);
        //y_data2[i] = y_data1[i] * -0.6 + sin(2 * arg) * 0.4;
        //y_data3[i] = y_data2[i] * -0.6 + sin(3 * arg) * 0.4;
    }
}

float sampling_freq = 44100;
void generate_data3() {
    
    constexpr float freq = 500;
    for (size_t i = 0; i < buf_size; ++i) {
        const float t = i / sampling_freq;
        x_data[i] = t;
        const float arg = 2 * M_PI * freq * t;
        vars2["x"] = arg;
        y_data1[i] = calc.eval(vars2).asDouble();
        //y_data2[i] = calc.eval(vars2).asDouble();
        y_data3[i] = calc.eval(vars2).asDouble();//tan(arg);
        //y_data2[i] = y_data1[i] * -0.6 + sin(2 * arg) * 0.4;
        //y_data3[i] = y_data2[i] * -0.6 + sin(3 * arg) * 0.4;
    }
    for (size_t i = 1; i < (buf_size - 1); ++i) {
        y_data2[i] = (y_data2[i - 1] / 4) + (y_data2[i] / 2) + (y_data2[i + 1] / 4);
    }
}
static float empty[512] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

void draw_multi_plot() {
    static const float* y_data[] = { y_data1, y_data3, y_data2 };
    static ImU32 colors[3] = { ImColor(0, 255, 0), ImColor(255, 0, 0), ImColor(0, 0, 255) };
    static uint32_t selection_start = 0, selection_length = 0;

    ImGui::Begin("Example plot", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    // Draw first plot with multiple sources
    ImGui::PlotConfig conf;
    conf.values.xs = x_data;
    conf.values.count = buf_size;
    conf.values.ys_list = y_data; // use ys_list to draw several lines simultaneously
    conf.values.ys_count = 3;
    conf.values.colors = colors;
    conf.scale.min = min_plotval;
    conf.scale.max = maxplotval;
    conf.tooltip.show = true;
    conf.grid_x.show = true;
    conf.grid_x.size = x_size15;
    conf.grid_x.subticks = 4;
    conf.grid_y.show = true;
    conf.grid_y.size = 0.5f;
    conf.grid_y.subticks = 5;
    conf.selection.show = true;
    conf.selection.start = &selection_start;
    conf.selection.length = &selection_length;
    conf.frame_size = ImVec2(buf_size, y_size15);
    ImGui::Plot("plot1", conf);

    // Draw second plot with the selection
    // reset previous values
   /* conf.values.ys_list = nullptr;
    conf.selection.show = false;
    // set new ones
    conf.values.ys = y_data3;
    conf.values.offset = selection_start;
    conf.values.count = selection_length;
    conf.line_thickness = 2.f;
    ImGui::Plot("plot2", conf);*/
    ImGui::InputText("function to plot", in, 64);
    if (ImGui::Button("Change")) {
        calc.compile(const_cast<const char*>(in));
        generate_data3();
    }
    ImGui::InputInt("Y size of plot", &y_size15);
    ImGui::InputInt("X size of plot", &x_size15);
    ImGui::InputInt("minimal value of plot", &min_plotval);
    ImGui::InputInt("maximal value of plot", &maxplotval);
    ImGui::InputFloat("sampling frequency", &sampling_freq, 10, 100);

    ImGui::End();
}



class ImTexture
{
public:
    int width, height;
    GLuint id;

    ImTexture();

    ImTexture(string path);

    void Load(string path);

    int GetWidth();

    int GetHeight();

    GLuint GetID();
};

int i = 0;

int nextIndex() {
    i++;
    return i;
}

bool IsHovered(int curx, int cury, ImVec2 pos, ImVec2 size) {
    if ((curx >= pos.x) && (curx <= (size.x + pos.x)) && (cury >= pos.y) && (cury <= (size.y + pos.y))) {
        return true;
    }
    return false;
}

// Function to get the size of a file
int get_file_size(const std::string& filename) {
    struct stat st;
    if (stat(filename.c_str(), &st) == 0) {
        return st.st_size;
    }
    return -1;
}

struct color32 {
    int r;
    int g;
    int b;
    int a;
};

color32 ExtractIM_COL32(ImU32 color) {
    // Extract the red channel (8 bits)
    int r = (color & 0xFF000000) >> 24;

    // Extract the green channel (8 bits)
    int g = (color & 0x00FF0000) >> 16;

    // Extract the blue channel (8 bits)
    int b = (color & 0x0000FF00) >> 8;

    // Extract the alpha channel (8 bits)
    int a = (color & 0x000000FF);
    return color32{ r,g,b,a };
}

std::string char128_str(char arr[128])
{
    // Convert the char array to a string using the string constructor
    return std::string(arr);
}

std::string XORencrypt(std::string message, char key) {
    for (int i = 0; i < message.size(); i++) {
        message[i] = message[i] ^ key;
    }
    return message;
}

std::string XORdecrypt(std::string message, char key) {
    return XORencrypt(message, key);
}

ImVector<ImWchar> ranges;
ImFontGlyphRangesBuilder builder;

void ColorTextByValue(ImFont* font, int value) {
    ImGui::PushFont(font);
    if (value < 0) {
        ImGui::TextColored(rgba(255, 0, 0, 255), to_string(value).c_str());
    }
    else if (value == 0) {
        ImGui::TextColored(rgba(255, 253,83, 255), ("+" + to_string(value)).c_str());
    }
    else {
        ImGui::TextColored(rgba(0, 255, 0, 255), ("+" + to_string(value)).c_str());
    }
    ImGui::PopFont();
}


int main(int argc, char* argv[])

{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    //ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

   // printCalendar(2023);


    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, a, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != a);

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //ImGui::MergeIconsWithLatestFont(18.f, false);

    ImFontConfig font_cfg;
    font_cfg.FontDataOwnedByAtlas = false;
    io.Fonts->AddFontFromMemoryTTF((void*)tahoma, sizeof(tahoma), 17.f, &font_cfg);

    // Initialize notify
    ImGui::MergeIconsWithLatestFont(17.f, false);

    // If you use multiple fonts, repeat the same thing!
    // io->Fonts->AddFontFromMemoryTTF((void*)another_font, sizeof(another_font), 17.f, &font_cfg);
    // ImGui::MergeIconsWithLatestFont(16.f, false);


    cout << argv[0] << endl;


    //try load fonts

    vector<string> fontiky = { "../../../res/fonts/Roboto-Medium.ttf", "/../res/fonts/Roboto-Medium.ttf", "../../res/fonts/Roboto-Medium.ttf",
        "res/fonts/Roboto-Medium.ttf", "../../../../res/fonts/Roboto-Medium.ttf" };
    for (int i = 0; i < fontiky.size(); i++) {
        cout << "Trying: " << fontiky.at(i) << endl;
        try {
            //io.Fonts->AddFontFromFileTTF(f.at(i).c_str(), 18.0f);
            ifstream _i(fontiky.at(i).c_str());
            if (_i.fail()) {
                _i.close();
                throw runtime_error("bruh");
            }
            _i.close();
            cout << "ok :)" << endl;
        }
        catch (exception& e) {
            cout << "not ok :/" << endl << endl;
        }
    }


    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    bool* dark_prince = new bool(true);
    bool* op = new bool(true);
    io.Fonts->AddFontFromFileTTF("res/fonts/Roboto-Medium.ttf", 18.0f);
    ImGui::MergeIconsWithLatestFont(18.f, false);
    ImFont* font1 = io.Fonts->AddFontFromFileTTF("res/fonts/Roboto-Medium.ttf", 50.0f);
    ImGui::MergeIconsWithLatestFont(50.f, false);
    ImFont* title = io.Fonts->AddFontFromFileTTF("res/fonts/Roboto-Medium.ttf", 25.0f);
    ImGui::MergeIconsWithLatestFont(25.f, false);
    ImFont* text = io.Fonts->AddFontFromFileTTF("res/fonts/Roboto-Medium.ttf", 30.0f);
    int w = 0, h = 0;
    ImGui::MergeIconsWithLatestFont(15.f, false);


    /*

    INFO FOR TEXTURES:

    if i want to check if they're loadable/existing as files:

    #define DEBUG

    - Important note: it made slower loading but its more secure

    */



    ImTexture acc("res/img/accw.png");

    //for planner + planner menu
    ImFont* planner_font = io.Fonts->AddFontFromFileTTF("c:/Windows/Fonts/segoeui.ttf", 150.0f);
    ImGui::MergeIconsWithLatestFont(33.28f, false);


    builder.AddText("ùìšèøžòïýáíúóéÙÌŠÈØŽÒÏÝÁÍÚÓÉ");                        // Add a string (here "Hello world" contains 7 unique characters)
    //builder.AddChar(0x7262);                               // Add a specific character
    builder.AddRanges(io.Fonts->GetGlyphRangesJapanese()); // Add one of the default ranges
    builder.AddRanges(io.Fonts->GetGlyphRangesChineseFull());
    builder.BuildRanges(&ranges);// Build the final result (ordered ranges with all the unique characters submitted)



    //analysis textures, fonts
    ImFont* ana_text = io.Fonts->AddFontFromFileTTF("res/fonts/Roboto-Medium.ttf", 50.0f, NULL, ranges.Data);
    ImGui::MergeIconsWithLatestFont(50.0f, false);

    ImFont* ana_percent = io.Fonts->AddFontFromFileTTF("res/fonts/Roboto/Roboto-Black.ttf", 50.0f, NULL, ranges.Data);
    ImGui::MergeIconsWithLatestFont(50.0f, false);

    ImTexture ana_notify("res/img/notify.png");
    ImTexture ana_check("res/img/check.png");
    ImTexture ana_calcheck("res/img/cal_check.png");
    ImTexture ana_save("res/img/save.png");

    int ana_num_solved = 54, ana_num_solved_ch = 7;
    int ana_num_feedback = 1, ana_num_feedback_ch = -1;//ch = value change
    int ana_num_saved = 6, ana_num_saved_ch = 0;
    int ana_num_completed = 21, ana_num_completed_ch = 21;




    ImFont* smradoch = io.Fonts->AddFontFromFileTTF("res/fonts/Noto_Sans/NotoSans-Regular.ttf", 30.f);
    ImGui::MergeIconsWithLatestFont(30.0f, false);


    std::time_t t = std::time(0);

    
    TaskList* list1 = new TaskList();
    list1->tasks.push_back(new Task("Do the test", false, NULL));
    list1->tasks.push_back(new Task("Do the test2", false, NULL));
    list1->tasks.push_back(new Task("Go with dog", false, NULL));
    list1->LoadFonts("res/fonts");
    list1->name = "List1";
    vector<TaskList*> l1 = { list1 };
    TaskWidget test_widget(l1);
   // test_widget.lists.push_back(list1);
 //   test_widget.SetActive(0);



    uint64_t creation_time = 0;
    bool poprve = false;
    float opacity = 1.0f;
    float winsize24 = 20.f;

    ImVec2 a(200, 300), b(400, 800), c(400, 800);
    ImVec2 d = a + b;
    cout << "200+400=" << d.x << endl;
    cout << "300+800=" << d.y << endl;
    if (b == c) {
        cout << "400==400 && 800==800" << endl;
    }





    string account_name_text = "Mitas Vit";//kotva
    //RobotoSerif/RobotoSerif-ExtraLight.ttf
    ImFont* account_name = io.Fonts->AddFontFromFileTTF("res/fonts/Roboto-Medium.ttf", 40.0, NULL, ranges.Data);
    ImGui::MergeIconsWithLatestFont(40.0f, false);

    ImFont* summary = io.Fonts->AddFontFromFileTTF("c:/Windows/Fonts/segoeui.ttf", 150.f);
    ImGui::MergeIconsWithLatestFont(50.f, false);

    ImVec2 account_logout_pos = ImVec2(0, 0), account_logout_size = ImVec2(0, 0);
    ImVec2 account_setting_pos = ImVec2(0, 0), account_setting_size = ImVec2(0, 0);
    ImFont* account_logout = io.Fonts->AddFontFromFileTTF("res/fonts/Roboto-Medium.ttf", 20.0f, NULL, ranges.Data);
    ImGui::MergeIconsWithLatestFont(20.0f, false);



    ImTexture logo_event("res/img/png/events_dark.png");//png/event.png
    ImTexture logo_calendar("res/img/png/calendar_dark.png");//png/calendar.png
    ImTexture logo_aim("res/img/png/aim_dark.png");//png/aim.png


    ImTexture logo_planner("res/img/planner.png");
    ImTexture logo_explore("res/img/explore.png");
    ImTexture logo_learning("res/img/learning.png");


    ImTexture logo_house("res/img/png/house.png");

    ImTexture logo_nextbut("res/img/next.png"), logo_nextbut_hov("res/img/next2.png");
    //299x46
    //sirka = vyska * 6,5
    //vyska = vyska_next / 2
    ImTexture logo_fullsum("res/img/fullsum.png"), logo_fullsum_hov("res/img/fullsum2.png");
    ImVec2 logo_fullsum_pos = ImVec2(0, 0), logo_fullsum_size = ImVec2(0, 0);
    //vyska - 8.75% z vysky
    ImVec2 logo_nextbut_size = ImVec2(0,0), logo_nextbut_pos = ImVec2(0, 0);
    float nexbut_mult = 437.f / 83.f;//vynasobit vyskou -> dostanu sirku
    //437x83

    ImVec2 logo_event_pos = ImVec2(0, 0), logo_event_size = ImVec2(0, 0);
    ImVec2 logo_calendar_pos = ImVec2(0, 0), logo_calendar_size = ImVec2(0, 0);
    ImVec2 logo_aim_pos = ImVec2(0, 0), logo_aim_size = ImVec2(0, 0);

    ImVec2 logo_house_pos = ImVec2(0, 0), logo_house_size = ImVec2(0, 0);
        
    ImVec2 calendar_pos = ImVec2(0, 0), calendar_size = ImVec2(0, 0);

    //tmave modra na pozadi panelu
    bool panel = true;

    // 0  - main
    // 1  - planner menu
    // 2  - personal learning
    // 3  - just explore
    // 4  - events
    // 5  - todo and goals
    // 6  - calendar
    // 10 - settings menu
    // 11 - log out
    // 12 - motivation summary
    // 13 - full summary
    // 14 - latex compile
    int menu = 0;
    bool loaded = false, started = false;
    double cursor_x = 0, cursor_y = 0;


    //some random code, why not
    //https://github.com/ocornut/imgui/pull/2005
    io.KeysDown[GLFW_KEY_ENTER] |= io.KeysDown[GLFW_KEY_KP_ENTER];

    
    // Now using a custom title...
    ImGuiToast toast(ImGuiToastType_Success, 5000); // <-- content can also be passed here as above
    toast.set_title("Yo yo brother");
    toast.set_content("You finally reached planner");

    ImGuiToast toast2(ImGuiToastType_Info, 10000); // <-- content can also be passed here as above
    toast2.set_title("Info");
    toast2.set_content("There will be some things connected with personal learning and learning by itself");


    static std::vector<ImHotKey::HotKey> hotkeys =
    {
        { "Exit", "exits the app", 0xFFFF261D}
        ,{"Save", "Save the current graph", 0xFFFF1F1D}
        ,{"Load", "Load an existing graph file", 0xFFFF181D}
        ,{"Play/Stop", "Play or stop the animation from the current graph", 0xFFFFFF3F}
        ,{"SetKey", "Make a new animation key with the current parameters values at the current time", 0xFFFFFF1F}
    };

    char latex_input[64] = "\\sqrt{2f}";
    bool ___loaded = false;
    ImTexture latex;

    string font_path = "res/fonts/";
    tag* _tags = new tag[3];
    _tags[0] = { 1, "physics" };
    _tags[1] = { 2, "math" };
    _tags[2] = { 3, "informatics" };
    Calculation calc_test(font_path, "Linear equations","And how to learn them.","text", "F=ma","11.3.2023", "12.3.2023", _tags);

    generate_data2("sin(x)");
    LoadData();

    bool* opened_popup = new bool(true);
    bool baf = false;
    float col[3] = { 0.5,0.5,0.5 };

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        glfwGetWindowSize(window, &w, &h);
        glfwSetWindowTitle(window, ("w: " + to_string(w) + " h: " + to_string(h)).c_str());

        ImGui::NewFrame();
        {
            glfwGetCursorPos(window, &cursor_x, &cursor_y);
            glfwGetWindowSize(window, &w, &h);
            
            static float f = 0.0f;
            static int counter = 0;

            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(w, h));
            ImGui::Begin("main", op, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

            int imgsize1 = (((h / 100) * 15) / 100) * 75;

            if (panel) {
                ImDrawList* Tdraw_list = ImGui::GetWindowDrawList();
                Tdraw_list->AddRectFilled(ImVec2(0, 0), ImVec2(w, h), IM_COL32(0, 0, 0, 255));
                Tdraw_list->AddRectFilled(ImVec2(0, 0), ImVec2(w, (h / 100) * 15), IM_COL32(12, 0, 92, 255));



                ImGui::SetNextWindowPos(ImVec2(w - imgsize1 - 20, (((h / 100) * 15) - imgsize1) / 2));
                //ImGui::BeginChild(10, ImVec2(w, (h / 100) * 15), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                ImGui::BeginChild(10, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);

                ImGui::PushStyleColor(ImGuiCol_Button, rgba(12, 0, 92, 255));//2, 156, 58
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, rgba(22, 2, 155, 255));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, rgba(22, 2, 155, 255));

                ImGui::ImageButton((void*)(intptr_t)acc.GetID(), ImVec2(imgsize1, imgsize1));
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();

                ImGui::EndChild();
            }
            ImVec2 txt_size = account_name->CalcTextSizeA(account_name->FontSize, FLT_MAX, 0, account_name_text.c_str());
            
            account_logout_size = account_logout->CalcTextSizeA(account_logout->FontSize, FLT_MAX, 0, "Log out");
            account_logout_pos = ImVec2(w - imgsize1 - 25 - account_logout_size.x, ((((h / 100) * 15) - imgsize1) / 2) + txt_size.y);

            account_setting_size = account_logout->CalcTextSizeA(account_logout->FontSize, FLT_MAX, 0, "Settings");
            account_setting_pos = ImVec2(w - imgsize1 - 25 - account_setting_size.x, ((((h / 100) * 15) - imgsize1) / 2) + txt_size.y + account_logout_size.y);


            ImGui::SetNextWindowPos(ImVec2(w - imgsize1 - 25  - txt_size.x, (((h / 100) * 15) - imgsize1) / 2));
            //ImGui::BeginChild(10, ImVec2(w, (h / 100) * 15), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
            ImGui::BeginChild(101, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);

            ImGui::PushFont(account_name);
            ImGui::TextColored(ImVec4{ 1,1,1,1 }, account_name_text.c_str());
            ImGui::PopFont();

            ImGui::EndChild();


            ImGui::SetNextWindowPos(account_logout_pos);
            //ImGui::BeginChild(10, ImVec2(w, (h / 100) * 15), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
            ImGui::BeginChild(100, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);

            ImGui::PushFont(account_logout);
            if (IsHovered(cursor_x, cursor_y, account_logout_pos, account_logout_size)) {
                ImGui::TextColored(ImVec4{ 0.7f,0.7f,0.7f,1 }, "Log out");
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    menu = 11;
                }
            }
            else {
                ImGui::TextColored(ImVec4{ 1,1,1,1 }, "Log out");
            }
            ImGui::PopFont();

            ImGui::EndChild();

            ImGui::SetNextWindowPos(account_setting_pos);
            //ImGui::BeginChild(10, ImVec2(w, (h / 100) * 15), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
            ImGui::BeginChild(102, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);

            ImGui::PushFont(account_logout);
            if (IsHovered(cursor_x, cursor_y, account_setting_pos, account_setting_size)) {
                ImGui::TextColored(ImVec4{ 0.7f,0.7f,0.7f,1 }, "Settings");
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    menu = 10;
                }
            }
            else {
                ImGui::TextColored(ImVec4{ 1,1,1,1 }, "Settings");
            }
            
            ImGui::PopFont();

            ImGui::EndChild();


            logo_house_pos = ImVec2(20, (((h / 100) * 15) - imgsize1) / 2);
            logo_house_size = ImVec2(imgsize1, imgsize1 / 1.125);

            if (panel) {
                ImGui::SetNextWindowPos(logo_house_pos);
                //ImGui::BeginChild(10, ImVec2(w, (h / 100) * 15), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                ImGui::BeginChild(28, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                ImGui::PushStyleColor(ImGuiCol_Button, rgba(12, 0, 92, 255));//2, 156, 58
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, rgba(22, 2, 155, 255));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, rgba(22, 2, 155, 255));
                ImGui::ImageButton((void*)(intptr_t)logo_house.GetID(), logo_house_size);
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::EndChild();

                if (IsHovered(cursor_x, cursor_y, logo_house_pos, logo_house_size)) {
                    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                        menu = 0;
                    }
                }
            }

            /*

            ONLY FOR TESTING & PROTOTYPING
            REMOVE FOR FUTURE USE!!!

            */
            if (ImGui::IsKeyPressed(ImGuiKey_0)) {
                menu = 12;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_9)) {
                menu = 14;
            }
            if (ImGui::IsKeyPressed(ImGuiKey_1)) {
                menu = 13;
            }

            if (menu == 0) {
                panel = true;

                ImGui::SetNextWindowPos(ImVec2(0, (h / 100) * 15));
                ImGui::BeginChild(1, ImVec2(w / 3, (h - ((h / 100) * 15))), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                ImGui::PushStyleColor(ImGuiCol_Button, rgba(144, 158, 255, 255));//2, 156, 58
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, rgba(144, 158, 255, 255));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, rgba(144, 158, 255, 255));
                //186,52,38 - tmavsi
                ImGui::PushFont(font1);
                ImGui::Button("B", ImVec2(w / 3, (h - ((h / 100) * 15))));
                if (ImGui::IsItemClicked()) {
                    //planner...
                    menu = 1;
                    ImGui::InsertNotification(toast);
                }
                ImGui::PopFont();
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::EndChild();
                //Image size: 548*645
                ImVec2 planner_size1 = ImVec2((548.f / 645.f) * ((((float)h / 100.f) * 60.f) - 50.f), (((float)h / 100.f) * 60.f) - 50.f);
                //cout << "w: " << planner_size1.x << ", h: " << planner_size1.y << endl;
                ImGui::SetNextWindowPos(ImVec2((((((float)w / 100.f) * 33.f) - planner_size1.x) / 2.f) + 20, (((((float)h / 100.f) * 75.f) - planner_size1.y) / 2.f) + ((h / 100) * 15)));
                ImGui::BeginChild(48, planner_size1, false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                ImGui::Image((void*)(intptr_t)logo_planner.GetID(), planner_size1);
                if ((cursor_x > 0) && ((float)cursor_x < (((float)w / 100.f) * 33.f)) && ((float)cursor_y > (((float)h / 100) * 15))) {
                    //planner image hovered...
                    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                        menu = 1;
                        ImGui::InsertNotification(toast);
                    }
                }
                ImGui::EndChild();


                ImGui::SetNextWindowPos(ImVec2(w / 3, (h / 100) * 15));
                ImGui::BeginChild(2, ImVec2(w / 3, (h - ((h / 100) * 15))), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                ImGui::PushStyleColor(ImGuiCol_Button, rgba(205, 100, 68, 255));//179, 28, 2
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, rgba(205, 100, 68, 255));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, rgba(205, 100, 68, 255));
                ImGui::PushFont(font1);
                //ImGui::ColorPicker3("c", col);
                ImGui::Button("A", ImVec2(w / 3, (h - ((h / 100) * 15))));
                if (ImGui::IsItemClicked()) {
                    //personal learning
                    menu = 2;
                    ImGui::InsertNotification(toast2);
                }
                ImGui::PopFont();
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::EndChild();
                //Image size: 612*782
                ImVec2 planner_size2 = ImVec2((612.f / 782.f) * ((((float)h / 100.f) * 60.f) - 50.f), (((float)h / 100.f) * 60.f) - 50.f);
                //cout << "w: " << planner_size1.x << ", h: " << planner_size1.y << endl;
                ImGui::SetNextWindowPos(ImVec2((((((float)w / 100.f) * 33.f) - planner_size2.x) / 2.f) + 20 + (((float)w / 100.f) * 33.f), (((((float)h / 100.f) * 75.f) - planner_size2.y) / 2.f) + ((h / 100) * 15)));
                ImGui::BeginChild(49, planner_size2, false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                ImGui::Image((void*)(intptr_t)logo_learning.GetID(), planner_size2);
                if (((float)cursor_x > (((float)w / 100.f) * 33.f)) && ((float)cursor_x < (((float)w / 100.f) * 66.f)) && ((float)cursor_y > (((float)h / 100) * 15))) {
                    //personal learning
                    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                        menu = 2;
                        ImGui::InsertNotification(toast);
                    }
                }
                ImGui::EndChild();


                ImGui::SetNextWindowPos(ImVec2((w / 3) * 2, (h / 100) * 15));
                ImGui::BeginChild(3, ImVec2(w / 3, (h - ((h / 100) * 15))), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                ImGui::PushStyleColor(ImGuiCol_Button, rgba(150, 150, 150, 255));//179, 28, 2
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, rgba(150, 150, 150, 255));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, rgba(150, 150, 150, 255));
                ImGui::PushFont(font1);
                ImGui::Button("C", ImVec2(w / 3, (h - ((h / 100) * 15))));
                if (ImGui::IsItemClicked()) {
                    //just explore...
                    menu = 3;
                }
                ImGui::PopFont();
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::EndChild();
                //Image size: 652*759
                ImVec2 planner_size3 = ImVec2((652.f / 759.f) * ((((float)h / 100.f) * 60.f) - 50.f), (((float)h / 100.f) * 60.f) - 50.f);
                ImGui::SetNextWindowPos(ImVec2((((((float)w / 100.f) * 33.f) - planner_size3.x) / 2.f) + 20 + (((float)w / 100.f) * 66.f), (((((float)h / 100.f) * 75.f) - planner_size3.y) / 2.f) + ((h / 100) * 15)));
                ImGui::BeginChild(50, planner_size3, false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                ImGui::Image((void*)(intptr_t)logo_explore.GetID(), planner_size3);
                if (((float)cursor_x > (((float)w / 100.f) * 66.f)) && ((float)cursor_y > (((float)h / 100) * 15))) {
                    //explore
                    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                        menu = 3;
                        ImGui::InsertNotification(toast);
                    }
                }
                ImGui::EndChild();
            }
            else if (menu == 1) {
                if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                    menu = 0;
                }
                ImDrawList* dr11 = ImGui::GetWindowDrawList();
                ImGui::SetNextWindowPos(ImVec2(((float)w/100.0f)*10.16f, ((float)h / 100.0f) * 19.11f));
                ImGui::BeginChild(11, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                ImGui::PushFont(planner_font);
                ImGui::TextColored(rgba(255, 255, 255, 255), "Planner");
                ImGui::PopFont();
               
                //1,15=11,5
                ImVec2 a = planner_font->CalcTextSizeA(planner_font->FontSize, FLT_MAX, 0, "Planner");
                                /*
                if (ImGui::Button("Edit Hotkeys"))
                {
                    ImGui::OpenPopup("HotKeys Editor");
                }
                ImHotKey::Edit(hotkeys.data(), hotkeys.size(), "HotKeys Editor");
                // ImHotKey also provides a way to retrieve HotKey
                int hotkey = ImHotKey::GetHotKey(hotkeys.data(), hotkeys.size());
                if (hotkey != -1)
                {
                    cout << hotkey << endl;
                    // handle the hotkey index!
                    if (hotkey == 0) {
                        MessageBox(a, "this app will now turn off", "Info", MB_OK | MB_ICONINFORMATION);
                        exit(0);
                    }
                }
                */
                ImGui::EndChild();
                dr11->AddRectFilled(ImVec2((((float)w / 100.0f) * 10.16f), (((float)h / 100.0f) * 19.11f) + 11.5f + a.y - 5.0f), ImVec2((((float)w / 100.0f) * 10.16f) + a.x, (((float)h / 100.0f) * 19.11f) + 23.0f + a.y), IM_COL32(255, 255, 255, 255));

                

                logo_event_pos = ImVec2(((float)w / 100.0f) * 10.16f, ((float)h / 100.0f) * 22.3f + 23.0f + a.y);
                logo_event_size = ImVec2(((float)w / 100.0f) * 22.48, (((float)w / 100.0f) * 22.48f)* 1.142f);

                float x_mezera = (float)w - ((logo_event_pos.x) * 2.0f);//vlevo + vpravo kraj
                x_mezera = x_mezera - ((logo_event_size.x) * 3.0f);
                x_mezera /= 2;//2 mezery

                logo_aim_pos = ImVec2(((logo_event_pos.x)+ (logo_event_size.x))+x_mezera, logo_event_pos.y);
                logo_aim_size = ImVec2(logo_event_size.x, logo_event_size.y);

                logo_calendar_pos = ImVec2(((logo_event_pos.x) + (logo_event_size.x)*2) + x_mezera+x_mezera, logo_event_pos.y);
                logo_calendar_size = ImVec2(logo_event_size.x, logo_event_size.y);


                ImGui::SetNextWindowPos(logo_event_pos);
                ImGui::BeginChild(12, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                //(void*)(intptr_t)acc.GetID()
                ImGui::Image((void*)(intptr_t)logo_event.GetID(), logo_event_size);
                ImGui::EndChild();

                ImGui::SetNextWindowPos(logo_aim_pos);
                ImGui::BeginChild(13, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                //(void*)(intptr_t)acc.GetID()
                ImGui::Image((void*)(intptr_t)logo_aim.GetID(), logo_aim_size);
                ImGui::EndChild();

                ImGui::SetNextWindowPos(logo_calendar_pos);
                ImGui::BeginChild(14, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                //(void*)(intptr_t)acc.GetID()
                ImGui::Image((void*)(intptr_t)logo_calendar.GetID(), logo_calendar_size);
                ImGui::EndChild();

                if (IsHovered(cursor_x, cursor_y, logo_event_pos, logo_event_size)) {
                    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                        //events...
                        ImGui::InsertNotification({ ImGuiToastType_Info, 3000, "Events should be opened" });
                        menu = 4;
                    }
                }

                if (IsHovered(cursor_x, cursor_y, logo_aim_pos, logo_aim_size)) {
                    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                        //aim...
                        ImGui::InsertNotification({ ImGuiToastType_Info, 3000, "Aims should be opened" });
                        menu = 5;
                    }
                }

                if (IsHovered(cursor_x, cursor_y, logo_calendar_pos, logo_calendar_size)) {
                    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                        //calendar...
                        ImGui::InsertNotification({ ImGuiToastType_Info, 3000, "Calendar should be opened" });
                        menu = 6;
                    }
                }
                    
            }
            else if (menu == 2) {
                if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                    menu = 0;
                }
                ImGui::SetNextWindowPos(ImVec2(0, (h / 100) * 15));
                ImGui::BeginChild(5, ImVec2(w, (h - ((h / 100) * 15))), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);

                //nahore: rgb(164, 42, 79) -- 175, 85, 84
                //dole  : 169, 62, 70  -- rgb(178, 117, 43)
                ImDrawList* mDrawList = ImGui::GetWindowDrawList();

                /*mDrawList->AddRectFilledMultiColor(ImVec2(padding.x, ((h / 100) * 15) + padding.y), ImVec2(size.x + padding.x, ((h / 100) * 15) + size.y + padding.y),
                    IM_COL32(164, 42, 79, 255), IM_COL32(175, 85, 84, 255), IM_COL32(178, 117, 43, 255), IM_COL32(169, 62, 70, 255));
                *///vlevo nahore            , vpravo nahore             , vpravo dole                , vlevo dole


                ImVec2 size = ImVec2(500, 500), padding = ImVec2(10, 10);
                float border_dia = size.x / 6.9f;//rounding diameter
                float thickness = size.x / 75.0f;//border thickness

                //rounded:
                AddRectFilledMultiColor(mDrawList, ImVec2(padding.x, ((h / 100) * 15) + padding.y), ImVec2(size.x + padding.x, ((h / 100) * 15) + size.y + padding.y),
                    IM_COL32(164, 42, 79, 255), IM_COL32(175, 85, 84, 255), IM_COL32(178, 117, 43, 255), IM_COL32(169, 62, 70, 255), border_dia, ImDrawCornerFlags_Top | ImDrawCornerFlags_Left | ImDrawCornerFlags_Right | ImDrawCornerFlags_Bot);
                //okraj:
                mDrawList->AddRect(ImVec2(padding.x, ((h / 100) * 15) + padding.y), ImVec2(size.x + padding.x, ((h / 100) * 15) + size.y + padding.y),
                    IM_COL32(227,77,123, 255), border_dia, ImDrawCornerFlags_Top | ImDrawCornerFlags_Left | ImDrawCornerFlags_Right | ImDrawCornerFlags_Bot, thickness);
                
                //draw_multi_plot();
                
                //z inkscapu:
                //w: 12,2, h: 12,2
                //rounding-diameter: 9px
                //thickness of border: 0,2px

                ImGui::EndChild();
            }
            else if (menu == 3) {
                if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                    menu = 0;
                }
                ImGui::SetNextWindowPos(ImVec2(0, (h / 100) * 15));
                ImGui::BeginChild(6, ImVec2(w, (h - ((h / 100) * 15))), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);

                //loading data animation
               // ImGui::PushFont(font1);
                //ImGui::Text("Loading");
                //ImSpinner::SpinnerSineArcs("test", 50.0f, 5.0f);
                //ImGui::PopFont();

                static bool h_borders = true;
                static bool v_borders = true;
                static int columns_count = 4;
                const int lines_count = 30;
                ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);

                ImGui::Columns(columns_count, NULL, v_borders);
                calc_test.RenderAsComumn();
                for (int i = 0; i < columns_count * lines_count; i++)
                {
                    if (ImGui::GetColumnIndex() == 0)
                        ImGui::Separator();

                    ImGui::PushFont(title);
                    ImGui::Text("Title%d", i);
                    ImGui::PopFont();

                    ImGui::PushFont(text);
                    ImGui::TextWrapped("Some extremly random text, .../*ImGui::Text(\" % c % c % c\", 'a' + i, 'a' + i, 'a' + i); \
                            ImGui::Text(\"Width %.2f\", ImGui::GetColumnWidth());\
                        ImGui::Text(\"Avail %.2f\", ImGui::GetContentRegionAvail().x);\
                        ImGui::Text(\"Offset %.2f\", ImGui::GetColumnOffset());\
                        ImGui::Text(\"Long text that is likely to clip\");  \");");
                    ImGui::PopFont();
                    //ImGui::Text("%c%c%c", 'a' + i, 'a' + i, 'a' + i);
                    //ImGui::Text("Width %.2f", ImGui::GetColumnWidth());
                    //ImGui::Text("Avail %.2f", ImGui::GetContentRegionAvail().x);
                    //ImGui::Text("Offset %.2f", ImGui::GetColumnOffset());
                    //ImGui::Text("Long text that is likely to clip");
                    ImGui::Button("Open", ImVec2(-FLT_MIN, 0.0f));
                    ImGui::NextColumn();
                }
                
                ImGui::Columns(1);
                ImGui::Separator();

                ImGui::EndChild();

            }
            else if (menu == 4) {
                //events
                if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                    menu = 1;
                }
                ImDrawList* dr11 = ImGui::GetWindowDrawList();
                ImGui::SetNextWindowPos(ImVec2(((float)w / 100.0f) * 10.16f, ((float)h / 100.0f) * 19.11f));
                ImGui::BeginChild(11, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                ImGui::PushFont(planner_font);
                ImGui::TextColored(rgba(255, 255, 255, 255), "Events");
                ImGui::PopFont();

                //1,15=11,5
                ImVec2 a = planner_font->CalcTextSizeA(planner_font->FontSize, FLT_MAX, 0, "Events");
                            ImGui::EndChild();
                dr11->AddRectFilled(ImVec2((((float)w / 100.0f) * 10.16f), (((float)h / 100.0f) * 19.11f) + 11.5f + a.y - 5.0f), ImVec2((((float)w / 100.0f) * 10.16f) + a.x, (((float)h / 100.0f) * 19.11f) + 23.0f + a.y), IM_COL32(255, 255, 255, 255));



                calendar_pos = ImVec2(((float)w / 100.0f) * 10.16f, ((float)h / 100.0f) * 22.3f + 23.0f + a.y);
                calendar_size = ImVec2(((float)w / 100.0f) * 22.48, (((float)w / 100.0f) * 22.48f) * 1.142f);

                

                //events

            }
            else if (menu == 5) {
                //to do and goals
                if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                    menu = 1;
                }
                ImDrawList* dr11 = ImGui::GetWindowDrawList();
                ImGui::SetNextWindowPos(ImVec2(((float)w / 100.0f) * 10.16f, ((float)h / 100.0f) * 19.11f));
                ImGui::BeginChild(11, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                ImGui::PushFont(planner_font);
                ImGui::TextColored(rgba(255, 255, 255, 255), "ToDo&Goals");
                ImGui::PopFont();

                //1,15=11,5
                ImVec2 a = planner_font->CalcTextSizeA(planner_font->FontSize, FLT_MAX, 0, "ToDo&Goals");
                ImGui::EndChild();
                dr11->AddRectFilled(ImVec2((((float)w / 100.0f) * 10.16f), (((float)h / 100.0f) * 19.11f) + 11.5f + a.y - 5.0f), ImVec2((((float)w / 100.0f) * 10.16f) + a.x, (((float)h / 100.0f) * 19.11f) + 23.0f + a.y), IM_COL32(255, 255, 255, 255));



                calendar_pos = ImVec2(((float)w / 100.0f) * 10.16f, ((float)h / 100.0f) * 22.3f + 23.0f + a.y);
                calendar_size = ImVec2(((float)w / 100.0f) * 22.48, (((float)w / 100.0f) * 22.48f) * 1.142f);



                //to do and goals

            }
            else if (menu == 12) {

                if (ImGui::IsKeyPressed(ImGuiKey_R)) {
                    //random data test
                    srand(time(NULL));
                    ana_num_saved = rand() % 200;
                    ana_num_completed = rand() % 200;
                    ana_num_feedback = rand() % 200;
                    ana_num_solved = rand() % 200;

                    ana_num_saved_ch = rand() % 400;
                    ana_num_completed_ch = rand() % 400;
                    ana_num_feedback_ch = rand() % 400;
                    ana_num_solved_ch = rand() % 400;
                    if (ana_num_saved_ch > 200) {
                        ana_num_saved_ch *= -1;
                        ana_num_saved_ch += 200;
                    }
                    if (ana_num_completed_ch > 200) {
                        ana_num_completed_ch *= -1;
                        ana_num_completed_ch += 200;
                    }
                    if (ana_num_feedback_ch > 200) {
                        ana_num_feedback_ch *= -1;
                        ana_num_feedback_ch += 200;
                    }
                    if (ana_num_solved_ch > 200) {
                        ana_num_solved_ch *= -1;
                        ana_num_solved_ch += 200;
                    }
                }

                //motivation summary prototype
                panel = false;
                ImDrawList* mdrawlist = ImGui::GetWindowDrawList();
                ImVec2 caption_size = summary->CalcTextSizeA(summary->FontSize, FLT_MAX, 0, "Month summary");
                ImVec2 caption_pos = ImVec2(((float)w - caption_size.x) / 2.f, ((float)h / 100.0f) * 10.f);

                ImVec2 msize = summary->CalcTextSizeA(summary->FontSize, FLT_MAX, 0, "Month ");

               // mdrawlist->AddRect(caption_pos, ImVec2(caption_pos.x + caption_size.x, caption_pos.y + caption_size.y), IM_COL32(255, 0, 0, 255));

                mdrawlist->AddRectFilled(ImVec2(caption_pos.x + msize.x, caption_pos.y + caption_size.y + 15), ImVec2(caption_pos.x + msize.x + (caption_size.x - msize.x), caption_pos.y + caption_size.y + 10 + 10), IM_COL32(255, 255, 255, 255));

                logo_nextbut_size = ImVec2(nexbut_mult * (((float)h / 100.f) * 8.75f), (((float)h / 100.f) * 8.75f));
                logo_nextbut_pos = ImVec2((float)w - logo_nextbut_size.x - 20, (float)h - logo_nextbut_size.y - 20);

                ImGui::SetNextWindowPos(caption_pos);
                ImGui::BeginChild(12, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                ImGui::PushFont(summary);
                ImGui::TextColored(rgba(255, 255, 255, 255), "Month summary");
                ImGui::PopFont();

                mdrawlist->AddImage((void*)(intptr_t)acc.GetID(), ImVec2(w - imgsize1 -10, (((h / 100) * 15) - imgsize1) / 2),
                    ImVec2((w - imgsize1 -10) + imgsize1, ((((h / 100) * 15) - imgsize1) / 2) + imgsize1));

                ImGui::EndChild();

                if (IsHovered(cursor_x, cursor_y, logo_nextbut_pos, logo_nextbut_size)) {
                    mdrawlist->AddImage((void*)(intptr_t)logo_nextbut_hov.GetID(), logo_nextbut_pos, ImVec2(logo_nextbut_pos.x + logo_nextbut_size.x, logo_nextbut_pos.y + logo_nextbut_size.y));
                    //on "next" button
                    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                        menu = 0;//return to main menu
                    }
                }
                else {
                    mdrawlist->AddImage((void*)(intptr_t)logo_nextbut.GetID(), logo_nextbut_pos, ImVec2(logo_nextbut_pos.x + logo_nextbut_size.x, logo_nextbut_pos.y + logo_nextbut_size.y));
                }

                logo_fullsum_size = ImVec2((logo_nextbut_size.y / 2.f) * 6.5f, logo_nextbut_size.y / 2.f);
                logo_fullsum_pos = ImVec2(logo_nextbut_pos.x - 20.f - logo_fullsum_size.x, logo_nextbut_pos.y+((logo_nextbut_size.y - logo_fullsum_size.y)/2.f));

                if (IsHovered(cursor_x, cursor_y, logo_fullsum_pos, logo_fullsum_size)) {
                    mdrawlist->AddImage((void*)(intptr_t)logo_fullsum_hov.GetID(), logo_fullsum_pos, ImVec2(logo_fullsum_pos.x + logo_fullsum_size.x, logo_fullsum_pos.y + logo_fullsum_size.y));
                    //on "see full " button
                    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                        menu = 13;//return to main menu
                    }
                }
                else {
                    mdrawlist->AddImage((void*)(intptr_t)logo_fullsum.GetID(), logo_fullsum_pos, ImVec2(logo_fullsum_pos.x + logo_fullsum_size.x, logo_fullsum_pos.y + logo_fullsum_size.y));
                }

                //vykreslovani samotne analyzy
                ImVec2 spos = ImVec2(((float)w / 100.f) * 10.f, caption_pos.y + caption_size.y + 10 + 10 + 20 + 30);

                //feedback
                //img: 136x135
                ImVec2 ssize = summary->CalcTextSizeA(summary->FontSize, FLT_MAX, 0, "1");
                mdrawlist->AddImage((void*)(intptr_t)ana_notify.GetID(), spos, ImVec2(spos.x + ssize.y, spos.y + ssize.y));

                ImGui::SetNextWindowPos(ImVec2(spos.x + ssize.y + 20, spos.y -40));
                ImGui::BeginChild(13, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                ImGui::PushFont(summary);
                ImGui::TextColored(rgba(255, 255, 255, 255), to_string(ana_num_feedback).c_str());
                ImGui::PopFont();
                ImGui::EndChild();

                ImVec2 ssize2 = ana_text->CalcTextSizeA(ana_text->FontSize, FLT_MAX, 0, "feedback");
                ImGui::SetNextWindowPos(ImVec2(spos.x + ssize.y + 20, spos.y + ssize.y  - 50));
                ImGui::BeginChild(14, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                ImGui::PushFont(ana_text);
                ImGui::TextColored(rgba(255, 255, 255, 255), "feedback");
                ImGui::PopFont();
                ImGui::EndChild();

                ImVec2 ssize4 = ana_percent->CalcTextSizeA(ana_percent->FontSize, FLT_MAX, 0, "-1");
                ImGui::SetNextWindowPos(ImVec2(spos.x + ssize.y + 20, spos.y + ssize.y + ssize2.y - 50));
                ImGui::BeginChild(15, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                ColorTextByValue(ana_percent, ana_num_feedback_ch);
                ImGui::EndChild();


                spos.y = spos.y + ( + ssize.y + ssize2.y + ((h/100)*10) );
                //solved examples
                mdrawlist->AddImage((void*)(intptr_t)ana_check.GetID(), spos, ImVec2(spos.x + ssize.y, spos.y + ssize.y));

                ImGui::SetNextWindowPos(ImVec2(spos.x + ssize.y + 20, spos.y - 40));
                ImGui::BeginChild(16, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                ImGui::PushFont(summary);
                ImGui::TextColored(rgba(255, 255, 255, 255), to_string(ana_num_solved).c_str());
                ImGui::PopFont();
                ImGui::EndChild();

                ImGui::SetNextWindowPos(ImVec2(spos.x + ssize.y + 20, spos.y + ssize.y - 50));
                ImGui::BeginChild(17, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                ImGui::PushFont(ana_text);
                ImGui::TextColored(rgba(255, 255, 255, 255), "Solved examples");
                ImGui::PopFont();
                ImGui::EndChild();

                ImGui::SetNextWindowPos(ImVec2(spos.x + ssize.y + 20, spos.y + ssize.y + ssize2.y - 50));
                ImGui::BeginChild(18, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                ColorTextByValue(ana_percent, ana_num_solved_ch);
                ImGui::EndChild();


                //saved lessons

                spos.x = w - (ana_text->CalcTextSizeA(ana_text->FontSize, FLT_MAX, 0, "completed goals").x + spos.x + ssize.y + 10);
                spos.y = caption_pos.y + caption_size.y + 10 + 10 + 20 + 30;

                mdrawlist->AddImage((void*)(intptr_t)ana_save.GetID(), ImVec2((ssize.y * 0.25f) + spos.x, spos.y), ImVec2(spos.x + ssize.y , spos.y + ssize.y));

                ImGui::SetNextWindowPos(ImVec2(spos.x + ssize.y + 20, spos.y - 40));
                ImGui::BeginChild(19, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                ImGui::PushFont(summary);
                ImGui::TextColored(rgba(255, 255, 255, 255), to_string(ana_num_saved).c_str());
                ImGui::PopFont();
                ImGui::EndChild();

                ImGui::SetNextWindowPos(ImVec2(spos.x + ssize.y + 20, spos.y + ssize.y - 50));
                ImGui::BeginChild(20, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                ImGui::PushFont(ana_text);
                ImGui::TextColored(rgba(255, 255, 255, 255), "Saved lessons");
                ImGui::PopFont();
                ImGui::EndChild();

                ImGui::SetNextWindowPos(ImVec2(spos.x + ssize.y + 20, spos.y + ssize.y + ssize2.y - 50));
                ImGui::BeginChild(21, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                ColorTextByValue(ana_percent, ana_num_saved_ch);
                ImGui::EndChild();


                if ((h < 899) || (w < 1213)) {

                }
                else {
                    //completed goals
                    spos.y = spos.y + (+ssize.y + ssize2.y + ((h / 100) * 10));

                    mdrawlist->AddImage((void*)(intptr_t)ana_save.GetID(), ImVec2((ssize.y * 0.25f) + spos.x, spos.y), ImVec2(spos.x + ssize.y, spos.y + ssize.y));

                    ImGui::SetNextWindowPos(ImVec2(spos.x + ssize.y + 20, spos.y - 40));
                    ImGui::BeginChild(22, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                    ImGui::PushFont(summary);
                    ImGui::TextColored(rgba(255, 255, 255, 255), to_string(ana_num_completed).c_str());
                    ImGui::PopFont();
                    ImGui::EndChild();

                    ImGui::SetNextWindowPos(ImVec2(spos.x + ssize.y + 20, spos.y + ssize.y - 50));
                    ImGui::BeginChild(23, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                    ImGui::PushFont(ana_text);
                    ImGui::TextColored(rgba(255, 255, 255, 255), "Completed goals");
                    ImGui::PopFont();
                    ImGui::EndChild();

                    ImGui::SetNextWindowPos(ImVec2(spos.x + ssize.y + 20, spos.y + ssize.y + ssize2.y - 50));
                    ImGui::BeginChild(24, ImVec2(w, h), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                    ColorTextByValue(ana_percent, ana_num_completed_ch);
                    ImGui::EndChild();
                }

            }
            else if (menu == 13) {

                //my own popup
                if (!poprve) {
                    creation_time = GetTickCount64();
                    poprve = true;
                }
                else {
                    if ((GetTickCount64() - creation_time) > 2000) {
                        //konec
                    }
                    else {
                        opacity = (1.f - (((float)(GetTickCount64() - creation_time) - (float)NOTIFY_FADE_IN_OUT_TIME - 3000.f / (float)NOTIFY_FADE_IN_OUT_TIME)) * NOTIFY_OPACITY);
                        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f); // Round borders
                        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(43.f / 255.f, 43.f / 255.f, 43.f / 255.f, 100.f / 255.f)); // Background color
                        const auto vp_size = ImGui::GetMainViewport()->Size;

                        float height = 0.f;

                        // Generate new unique name for this toast
                        char window_name[50];
                        sprintf_s(window_name, "##TOAST%d", i);

                        //PushStyleColor(ImGuiCol_Text, text_color);
                        ImGui::SetNextWindowBgAlpha(opacity);
                        
                        ImGui::SetNextWindowPos(ImVec2(vp_size.x - 20.f, vp_size.y - 20.f - height), ImGuiCond_Always, ImVec2(1.0f, 1.0f));
                        //ImGui::SetNextWindowPos(ImVec2((w- winsize24)/2, vp_size.y - 20.f - height), ImGuiCond_Always, ImVec2(1.0f, 1.0f));
                        ImGui::Begin(window_name, NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing);
                        ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
                        // Here we render the toast content
                        ImGui::PushTextWrapPos(vp_size.x / 3.f); // We want to support multi-line text, this will wrap the text after 1/3 of the screen width
                        bool was_title_rendered = false;
                        ImGui::TextColored(ImVec4(1.0, 0, 0, 1.0), ICON_FA_EXCLAMATION);
                        ImGui::SameLine();
                        ImGui::Text("Cs"); // Render title text
                        ImGui::Text("Ok"); // Render content text
                        ImGui::PopTextWrapPos();
                        ImGui::End();
                        ImGui::PopStyleVar(1); // Don't forget to Pop()
                        ImGui::PopStyleColor(1);
                    }
                }

                

                


                ImGui::PushFont(smradoch);

                ImGui::Begin("baf");
              //  ImGui::Text("%s Bruh", ICON_FA_BIOHAZARD);
               // ImGui::Text("%s Delete", ICON_FA_TRASH);
                ImGui::Text("%s Delete", ICON_FA_TRASH_ALT);
               // ImGui::Text("%s Delete", ICON_FA_TRASH_UNDO);
              //  ImGui::Text("%s Edit", ICON_FA_PEN_SQUARE);
                ImGui::Text("%s Edit2", ICON_FA_PEN);
                //ICON_FA_PLUS  -  \xef\x81\xa7
                if(ImGui::Button("\xef\x81\xa7  New task")){
                    baf = true;
                }
                if (baf) {
                    ImGui::Text("%s %s %s %s", ICON_FA_UNDO, ICON_FA_REDO, ICON_FA_FILE, ICON_FA_FILE_CODE);
                }
              //  ImGui::Text("%s Edit3", ICON_FA_PENCIL_RULER);
              //  ImGui::Text("%s Edit4", ICON_FA_PEN_ALT);
              //  ImGui::Text("%s Edit5", ICON_FA_PEN_FANCY);
                ImGui::End();
                ImGui::PopFont();
            }
            else if (menu == 14) {
                //latex compile
                /*ImDrawList* d = ImGui::GetWindowDrawList();
                d->AddRectFilled(ImVec2(0, (h / 100) * 15), ImVec2(w, (h - ((h / 100) * 15))), IM_COL32(255, 255, 255, 255));
                ImGui::SetNextWindowPos(ImVec2(0, (h / 100) * 15));
                ImGui::BeginChild(6, ImVec2(w, (h - ((h / 100) * 15))), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
                ImGui::InputText("Latex:", latex_input, 64);
                if (ImGui::Button("Compile")) {
                    get_latex(latex_input, "m.png", "PNG", 500);
                    latex.Load("m.png");
                    ___loaded = true;
                }
                if (___loaded) {
                    ImGui::Image((void*)(intptr_t)latex.GetID(), ImVec2(latex.width, latex.height));
                }
                ImGui::EndChild();
                */
                ImGui::End();
                ImGui::SetNextWindowPos(ImVec2(((w / 100) * 15 )+ 50, ((h / 100) * 15) + 20));
                ImGui::SetNextWindowSize(ImVec2(w, h));
                ImGui::Begin("ok", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar);
                test_widget.Draw(w, h, ImVec2(cursor_x, cursor_y));
                //ImGui::End();
            }

            if (menu != 14) {
                ImGui::End();
            }
            
           
        }


        // Render toasts on top of everything, at the end of your code!
            // You should push style vars here
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f); // Round borders
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(43.f / 255.f, 43.f / 255.f, 43.f / 255.f, 100.f / 255.f)); // Background color
        ImGui::RenderNotifications(); // <-- Here we render all notifications
        ImGui::PopStyleVar(1); // Don't forget to Pop()
        ImGui::PopStyleColor(1);

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}



ImTexture::ImTexture()
{
}

ImTexture::ImTexture(string path)
{
#ifdef DEBUG
    ifstream* t3 = new ifstream(path.c_str());
    if (t3->fail()) {
        MessageBox(a, ("cannot load \"" + path + "\"").c_str(), "Error", MB_OK | MB_ICONERROR);
        throw logic_error(("cannot load \"" + path + "\"").c_str());
    }
    t3->close();
    delete t3;
#endif
    // Assigns the type of the texture ot the texture object

    // Stores the width, height, and the number of color channels of the image
    int widthImg, heightImg, numColCh;
    // Flips the image so it appears right side up
    // stbi_set_flip_vertically_on_load(true);

    // Reads the image from a file and stores it in bytes
    unsigned char* bytes = stbi_load(path.c_str(), &widthImg, &heightImg, &numColCh, 0);
    width = widthImg;
    height = heightImg;
    // Generates an OpenGL texture object
    glGenTextures(1, &id);
    // Assigns the texture to a Texture Unit
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, id);

    // Configures the type of algorithm that is used to make the image smaller or bigger
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Configures the way the texture repeats (if it does at all)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Extra lines in case you choose to use GL_CLAMP_TO_BORDER
    // float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

    // Check what type of color channels the texture has and load it accordingly
    if (numColCh == 4)
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            widthImg,
            heightImg,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            bytes);
    else if (numColCh == 3)
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            widthImg,
            heightImg,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            bytes);
    else if (numColCh == 1)
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            widthImg,
            heightImg,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            bytes);
    else
        throw std::invalid_argument("Automatic Texture type recognition failed");

    // Generates MipMaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Deletes the image data as it is already in the OpenGL Texture object
    stbi_image_free(bytes);

    // Unbinds the OpenGL Texture object so that it can't accidentally be modified
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ImTexture::Load(string path)
{
    // Assigns the type of the texture ot the texture object

    // Stores the width, height, and the number of color channels of the image
    int widthImg, heightImg, numColCh;
    // Flips the image so it appears right side up
    // stbi_set_flip_vertically_on_load(true);

    // Reads the image from a file and stores it in bytes
    unsigned char* bytes = stbi_load(path.c_str(), &widthImg, &heightImg, &numColCh, 0);
    width = widthImg;
    height = heightImg;
    // Generates an OpenGL texture object
    glGenTextures(1, &id);
    // Assigns the texture to a Texture Unit
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, id);

    // Configures the type of algorithm that is used to make the image smaller or bigger
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Configures the way the texture repeats (if it does at all)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Extra lines in case you choose to use GL_CLAMP_TO_BORDER
    // float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

    // Check what type of color channels the texture has and load it accordingly
    if (numColCh == 4)
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            widthImg,
            heightImg,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            bytes);
    else if (numColCh == 3)
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            widthImg,
            heightImg,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            bytes);
    else if (numColCh == 1)
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            widthImg,
            heightImg,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            bytes);
    else
        throw std::invalid_argument("Automatic Texture type recognition failed");

    // Generates MipMaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Deletes the image data as it is already in the OpenGL Texture object
    stbi_image_free(bytes);

    // Unbinds the OpenGL Texture object so that it can't accidentally be modified
    glBindTexture(GL_TEXTURE_2D, 0);
}

int ImTexture::GetWidth()
{
    return width;
}

int ImTexture::GetHeight()
{
    return height;
}

GLuint ImTexture::GetID()
{
    return id;
}
