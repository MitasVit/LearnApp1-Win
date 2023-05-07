#pragma once
#include <calculation.h>
#include <chrono>
#include <ctime>
#include <time.h>
#include <cstring>
#include <algorithm>
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui_internal.h"
#include "font_awesome_5.h"
#include "imgui_stdlib.h"
#include "fa_solid_900.h"
#include <Windows.h>
#include "tahoma.h"




using namespace std;


/*

UTIL FUNCTIONS

- centering
- popup creation
- working with dates

*/

#define POPUP_FLAGS  ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs

bool operator==(ImVec2& a, ImVec2& b);
ImVec2 operator+(ImVec2& a, ImVec2& b);

static int InputTextCallback(ImGuiInputTextCallbackData* data);


namespace ImGui {
    /*
    My own functions to fast up creating popu windows in ImGui
        - flags
        - own ID
        - bringing to front
        - size + position
    */
    void BeginPopup(const char* id, ImVec2 size, ImVec2 pos = ImVec2(0, 0), bool bring_to_front=true);
    void BeginPopup(const char* id, ImVec2 size, ImGuiWindowFlags flags = 0, ImVec2 pos = ImVec2(0, 0), bool bring_to_front = true);
    void BeginPopup(int id, ImVec2 size, ImVec2 pos=ImVec2(0,0), bool bring_to_front = true);
    void BeginPopup(string id, ImVec2 size, ImVec2 pos = ImVec2(0, 0), bool bring_to_front = true);

    ImVec2 CenterRect(ImVec2 rc, ImVec2 winsize);

    //Same as ImGui::InputText but with max lenght limitation
    bool InputText2(const char* label, std::string* str, size_t max_lenght, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
};

//%d-%m-%Y %H:%M
//converts date struct to string
string date_tostr(struct tm& time);

char* date_tochar(struct tm* time);

//converts string to date
struct tm* Str2Date(string date);

//converts const char* to date
struct tm* Char2Date(const char* date);

//converts char[] to string 
std::string Char2Str(char* src, int size);

//makes the color more darker
void Darken(ImVec4* col);


class Task {
public:
    string title, datetxt;
    //title[25],  datetxt[25]
    //char* title, * datetxt;

    bool completed, have_date;
    //d,m,y,h,M,s
    struct tm *deadline;
    Task();
    Task(const char* _title, bool _have_date, struct tm* _deadline);
    Task(const char* _title, bool _have_date, bool _completed, struct tm *_deadline);

    
    Task(string _title, bool _have_date, struct tm* _deadline);
    Task(string _title, bool _have_date, bool _completed, struct tm *_deadline);
    

    //check if not have to be completed
    //how many time i have to complete this task
    void getTimeToCompleteTask(Task* task, int& minutes, int& hours, int& days, int& months);
};

//JSON data:
/*

        {
            "tasklist":
            {
                "name": "...",
                "theme_color": {1.0,1.0,1.0,1.0},
                "tasks":
                [
                {
                    "title": "...",
                    "date": "5.4.2023",
                    "completed": false,
                    "havedate": true,
                },
                {
                    "title": "...",
                    "date": "4.4.2023",
                    "completed": true,
                    "havedate": true,
                },
                ]
            }
        }
        */


void MyMergeIconsWithLatestFont(float font_size, bool FontDataOwnedByAtlas);

bool compareTasksByDeadline(Task* task1, Task* task2);

bool compareTasksByName(Task* task1, Task* task2);

class TaskList {
public:
    //TODO: add tmp init to constructor
    //tmp = new Task();
    TaskList();
    TaskList(string _name, ImVec4 _theme_color);

    //is any action in queue
    //ends when completed edit/new/delete tasks...
    //setted from taskwidget
    bool in_action;
    /*
    0 - nothing
    1 - edit
    2 - delete
    3 - new
    */
    int action, index;
    //TODO: in_action return to TaskWidget
    ImFont* FontTitle, * FontText, * FontDate;
    //C:\Users\vitat\Desktop\programming\superimgui\imgui\misc\fonts
    //for ex.: ../../misc/fonts/
    //mode:
    //  - 0, Roboto
    //  - 1, Noto Sans
    void LoadFonts(string font_folder, bool mode = true);

    vector<Task*> tasks;
    string name;
    ImVec4 theme_color;
    //NewTask() - widget in popup window for creating new task
    void NewTask();
    //EditTask() - widget in popup window for editing task
    void EditTask(Task* t);
    void EditTask(int index);
    void DeleteTask(Task* t);
    void DeleteTask(int index);

    //sorts tasks:
    // 1. per deadline
    // 2. per name
    // - with deadline sorted by name
    // - without deadline sorted by name
    //called on every change with the list of tasks eg:
    //   - on Edit/Delete/New/Load
    void SortTasks();

    //Draw() - Draw this task on full screen
    //lu_corn - left up corner position
    //rd_corn - right down corner position
    //lu_corn + rd_corn -> size/pos of the task
    // h - window height, w - window width
    //handles inputs,...
    void Draw(ImVec2 lu_corn, ImVec2 rd_corn, int w, int h, ImVec2 cursorpos);

    /*mode:
    0 - JSON
    Coming soon: 1 - XML
    Coming soon: 2 - Binary
    Coming soon: 3 - Compressed JSON (7-ZIP)
    Coming soon: 4 - Compressed XML (7-ZIP)

    return true if OK
    */
    bool Load(const char* filename, int mode = 0);


    /*mode:
    0 - JSON
    Coming soon: 1 - XML
    Coming soon: 2 - Binary
    Coming soon: 3 - Compressed JSON (7-ZIP)
    Coming soon: 4 - Compressed XML (7-ZIP)

    Return true if oK
    */
    bool Export(const char* filename, int mode = 0);

    //copy all tasks as json to clipboard
    //return true if ok
    bool CopyToClipboard();

    bool once = false;

    //private:
    Task* tmp;
};



class TaskWidget {
public:
    vector<TaskList*> lists;


    //Checks if there're any tasks that have to be completed
    /*
    time_t now = time(a);
    vector<Task*> result;
    for (Task* task : active->tasks) {
        if(task->have_deadline){
            if (!task->completed && task->deadline != nullptr) {
                time_t deadlineTime = mktime(task->deadline);
                if (now > deadlineTime) {
                    //have to be completed!!
                    result.push_back(task);
                }
            }
        }
    }
    //number of tasks to be completed: result.size()
    //if not have to be completed:
    //Task::getTimeToCompleteTask() -> load how many time I have to complete the task
    */

    //TODO: void CheckForDeadlines();

    TaskWidget();
    TaskWidget(vector<TaskList*> _lists);

    //pro tasklisty jako takove
    int index, action;
    bool in_action;

    void NewTaskList();
    //EditTask() - widget in popup window for editing task
    void EditTaskList(TaskList* t);
    void EditTaskList(int index);
    void DeleteTaskList(TaskList* t);
    void DeleteTaskList(int index);

    void Draw(int w, int h, ImVec2 cursorpos);

    //set active tasklist
    void SetActive(int index);

    /*mode:
    0 - JSON
    Coming soon: 1 - XML
    Coming soon: 2 - Binary
    Coming soon: 3 - Compressed JSON (7-ZIP)
    Coming soon: 4 - Compressed XML (7-ZIP)

    return true if OK
    */

    //TODO: bool Load(const char* filename, int mode = 0);


    /*mode:
    0 - JSON
    Coming soon: 1 - XML
    Coming soon: 2 - Binary
    Coming soon: 3 - Compressed JSON (7-ZIP)
    Coming soon: 4 - Compressed XML (7-ZIP)

    Return true if oK
    */

    //TODO: bool Export(const char* filename, int mode = 0);

    
private:
    //index of active tasklist
    int active_index;
    //active = *lists.at(active_index)
    TaskList** active;

    
};



/*A Function that returns the index of the day
  of the date- day/month/year
  For e.g-

  Index     Day
  0         Sunday
  1         Monday
  2         Tuesday
  3         Wednesday
  4         Thursday
  5         Friday
  6         Saturday*/
int dayNumber(int day, int month, int year);

/*
  A Function that returns the name of the month
  with a given month number

  Month Number     Name
  0                January
  1                February
  2                March
  3                April
  4                May
  5                June
  6                July
  7                August
  8                September
  9                October
  10               November
  11               December */
string getMonthName(int monthNumber);

/* A Function to return the number of days in
   a month

  Month Number     Name        Number of Days
  0                January     31
  1                February    28 (non-leap) / 29 (leap)
  2                March       31
  3                April       30
  4                May         31
  5                June        30
  6                July        31
  7                August      31
  8                September   30
  9                October     31
  10               November    30
  11               December    31

*/
int numberOfDays(int monthNumber, int year);

// Function to print the calendar of the given year
void printCalendar(int year);

// function expects the string in format dd/mm/yyyy:
bool extractDate(const std::string& s, int& d, int& m, int& y);
