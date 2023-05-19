
#include <Task.h>
using json = nlohmann::json;


bool operator==(ImVec2& a, ImVec2& b) {
    if (a.x == b.x && a.y == b.y) {
        return true;
    }
    return false;
}

ImVec2 operator+(ImVec2& a, ImVec2& b) {
    return ImVec2(a.x + b.x, a.y + b.y);
}


static int InputTextCallback2(ImGuiInputTextCallbackData* data)
{
    InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        // Resize string callback
        // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
        std::string* str = user_data->Str;
        IM_ASSERT(data->Buf == str->c_str());
        str->resize(data->BufTextLen);
        data->Buf = (char*)str->c_str();
    }
    else if (user_data->ChainCallback)
    {
        // Forward to user callback, if any
        data->UserData = user_data->ChainCallbackUserData;
        return user_data->ChainCallback(data);
    }
    return 0;
}

namespace ImGui {
    void BeginPopup(const char* id, ImVec2 size, ImVec2 pos, bool bring_to_front) {
        if (bring_to_front)
        {
            ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
        }
        if (size.x <= 0 || size.y <= 0) {
            size = ImVec2(400, 300);
        }
        if (pos.x < 0 || pos.y < 0) {
            pos = ImVec2(0, 0);
        }
        ImGui::SetNextWindowSize(size);
        ImGui::SetNextWindowPos(pos);
        ImGui::Begin(id, nullptr, POPUP_FLAGS);
    }
    void BeginPopup(const char* id, ImVec2 size, ImVec2 pos, ImGuiWindowFlags flags, bool bring_to_front) {
        if (bring_to_front)
        {
            ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
        }
        if (size.x <= 0 || size.y <= 0) {
            size = ImVec2(400, 300);
        }
        if (pos.x < 0 || pos.y < 0) {
            pos = ImVec2(0, 0);
        }
        ImGui::SetNextWindowSize(size);
        ImGui::SetNextWindowPos(pos);
        ImGui::Begin(id, nullptr, flags);
    }
    void BeginPopup(string id, ImVec2 size, ImVec2 pos, bool bring_to_front) {
        BeginPopup(id.c_str(), size, pos, bring_to_front);
    }

    void BeginPopup(int id, ImVec2 size, ImVec2 pos, bool bring_to_front) {
        BeginPopup(to_string(id).c_str(), size, pos, bring_to_front);
    }

    ImVec2 CenterRect(ImVec2 rc, ImVec2 winsize) {
        return ImVec2(winsize.x / 2 - rc.x / 2, winsize.y / 2 - rc.y / 2);
    }
    bool InputText2(const char* label, std::string* str, size_t max_lenght, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
    {
        IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
        flags |= ImGuiInputTextFlags_CallbackResize;

        InputTextCallback_UserData cb_user_data;
        cb_user_data.Str = str;
        cb_user_data.ChainCallback = callback;
        cb_user_data.ChainCallbackUserData = user_data;
        return InputText(label, (char*)str->c_str(), max_lenght, flags, InputTextCallback2, &cb_user_data);
    }
};

int dayNumber(int day, int month, int year)
{

    static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1,
                       4, 6, 2, 4 };
    year -= month < 3;
    return (year + year / 4 - year / 100 +
        year / 400 + t[month - 1] + day) % 7;
}

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
string getMonthName(int monthNumber)
{
    string months[] = { "January", "February", "March",
                       "April", "May", "June",
                       "July", "August", "September",
                       "October", "November", "December"
    };

    return (months[monthNumber]);
}

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
int numberOfDays(int monthNumber, int year)
{
    // January
    if (monthNumber == 0)
        return (31);

    // February
    if (monthNumber == 1)
    {
        // If the year is leap then February has
        // 29 days
        if (year % 400 == 0 ||
            (year % 4 == 0 && year % 100 != 0))
            return (29);
        else
            return (28);
    }

    // March
    if (monthNumber == 2)
        return (31);

    // April
    if (monthNumber == 3)
        return (30);

    // May
    if (monthNumber == 4)
        return (31);

    // June
    if (monthNumber == 5)
        return (30);

    // July
    if (monthNumber == 6)
        return (31);

    // August
    if (monthNumber == 7)
        return (31);

    // September
    if (monthNumber == 8)
        return (30);

    // October
    if (monthNumber == 9)
        return (31);

    // November
    if (monthNumber == 10)
        return (30);

    // December
    if (monthNumber == 11)
        return (31);
}

// Function to print the calendar of the given year
void printCalendar(int year)
{
    printf("         Calendar - %d\n\n", year);
    int days;

    // Index of the day from 0 to 6
    int current = dayNumber(1, 1, year);

    // i --> Iterate through all the months
    // j --> Iterate through all the days of the
    //       month - i
    for (int i = 0; i < 12; i++)
    {
        days = numberOfDays(i, year);

        // Print the current month name
        printf("\n  ------------%s-------------\n",
            getMonthName(i).c_str());

        // Print the columns
        printf("  Sun  Mon  Tue  Wed  Thu  Fri  Sat\n");

        // Print appropriate spaces
        int k;
        for (k = 0; k < current; k++)
            printf("     ");

        for (int j = 1; j <= days; j++)
        {
            printf("%5d", j);

if (++k > 6)
{
    k = 0;
    printf("\n");
}
        }

        if (k)
            printf("\n");

        current = k;
    }

    return;
}
bool extractDate(const std::string& s, int& d, int& m, int& y) {
    std::istringstream is(s);
    char delimiter;
    if (is >> d >> delimiter >> m >> delimiter >> y) {
        struct tm t = { 0 };
        t.tm_mday = d;
        t.tm_mon = m - 1;
        t.tm_year = y - 1900;
        t.tm_isdst = -1;

        // normalize:
        time_t when = mktime(&t);
        const struct tm* norm = localtime(&when);
        // the actual date would be:
        // m = norm->tm_mon + 1;
        // d = norm->tm_mday;
        // y = norm->tm_year;
        // e.g. 29/02/2013 would become 01/03/2013

        // validate (is the normalized date still the same?):
        return (norm->tm_mday == d &&
            norm->tm_mon == m - 1 &&
            norm->tm_year == y - 1900);
    }
    return false;
}
string date_tostr(struct tm& time) {
    char tmp[20] = "";
    strftime(tmp, 20, "%d-%b-%Y %H:%M", &time);
    return string(tmp);
}

Task::Task() {
    title = "";
    datetxt = "";
    have_date = false;
    completed = false;
    std::time_t t = std::time(0);
    deadline = localtime(&t);

}

Task::Task(const char* _title, bool _have_date, struct tm* _deadline) {
    title = string(_title);
    have_date = _have_date;
    deadline = _deadline;
    have_date = true;
    completed = false;
    datetxt = "";
    if (deadline != NULL) {
        datetxt = date_tostr(*deadline);
    }
    else {
        datetxt = "";
    }
    
}

Task::Task(const char* _title, bool _have_date, bool _completed, struct tm* _deadline) {
    title = string(_title);
    have_date = _have_date;
    deadline = _deadline;
    have_date = true;
    completed = _completed;
    datetxt = "";
    if (deadline != NULL) {
        datetxt = date_tostr(*deadline);
    }
    else {
        datetxt = "";
    }
}

Task::Task(string _title, bool _have_date, struct tm* _deadline) {
    title = _title;
    have_date = _have_date;
    deadline = _deadline;
    have_date = true;
    completed = false;
    datetxt = "";
    if (deadline != NULL) {
        datetxt = date_tostr(*deadline);
    }
    else {
        datetxt = "";
    }

}

Task::Task(string _title, bool _have_date, bool _completed, struct tm* _deadline) {
    title = _title;
    have_date = _have_date;
    deadline = _deadline;
    have_date = true;
    completed = _completed;
    datetxt = "";
    if (deadline != NULL) {
        datetxt = date_tostr(*deadline);
    }
    else {
        datetxt = "";
    }
}

void Task::getTimeToCompleteTask(Task* task, int& minutes, int& hours, int& days, int& months) {
    if (!task->have_date) {
        // The task doesn't have a deadline, so there's no time left to complete it
        minutes = 0;
        hours = 0;
        days = 0;
        months = 0;
        return;
    }

    time_t now = time(NULL);
    time_t deadlineTime = mktime(task->deadline);

    double diffSeconds = difftime(deadlineTime, now);

    // Compute time left in minutes, hours, days, and months
    minutes = diffSeconds / 60;
    hours = minutes / 60;
    days = hours / 24;
    months = days / 30;
}

void MyMergeIconsWithLatestFont(float font_size, bool FontDataOwnedByAtlas = false)
{
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.FontDataOwnedByAtlas = FontDataOwnedByAtlas;

    ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)fa_solid_900, sizeof(fa_solid_900), font_size, &icons_config, icons_ranges);
}

bool compareTasksByDeadline(Task* task1, Task* task2) {
    if (task1->deadline == nullptr && task2->deadline == nullptr) {
        // If both tasks have no deadline, sort by name
        return strcmp(task1->title.c_str(), task2->title.c_str()) < 0;
    }
    else if (task1->deadline == nullptr) {
        // Task1 has no deadline, so it should come after task2
        return false;
    }
    else if (task2->deadline == nullptr) {
        // Task2 has no deadline, so it should come after task1
        return true;
    }
    else {
        // Both tasks have deadlines, so sort by deadline
        return mktime(task1->deadline) < mktime(task2->deadline);
    }
}

bool compareTasksByName(Task* task1, Task* task2) {
    return strcmp(task1->title.c_str(), task2->title.c_str()) < 0;
}

string date_tostr(struct tm *time) {
    return string(to_string((*time).tm_mday) + "." + to_string((*time).tm_mon) + "." + to_string((*time).tm_year) + " " + to_string((*time).tm_hour) + ":" + to_string((*time).tm_min));
}

char* date_tochar(struct tm* time) {
    char* t = new char[20];
    strftime(t, 20, "%d-%b-%Y %H:%M", time);
    return t;
    //return string(to_string(time.tm_mday) + "." + to_string(time.tm_mon) + "." + to_string(time.tm_year) + " " + to_string(time.tm_hour) + ":" + to_string(time.tm_min)).c_str();
}

//converts string to date
struct tm* Str2Date(string date) {
    const char* format = "%d.%m.%Y %H:%M"; // format string for strptime
    istringstream ss(date);
    struct tm* timeinfo = new struct tm;
    char delim;
    ss >> timeinfo->tm_mday >> delim
        >> timeinfo->tm_mon >> delim
        >> timeinfo->tm_year >> delim
        >> timeinfo->tm_hour >> delim
        >> timeinfo->tm_min;
    timeinfo->tm_mon -= 1; // adjust month value to range [0,11]
    timeinfo->tm_year -= 1900; // adjust year value to count from 1900
    mktime(timeinfo); // adjust other fields and compute weekday/daylight saving time
    return timeinfo;
}

//converts const char* to date
struct tm* Char2Date(const char* date) {
    try {
        const char* format = "%d.%m.%Y %H:%M"; // format string for strptime
        char* endptr;
        struct tm* timeinfo = new struct tm;
        timeinfo->tm_mday = strtol(date, &endptr, 10);
        timeinfo->tm_mon = strtol(endptr + 1, &endptr, 10) - 1;
        timeinfo->tm_year = strtol(endptr + 1, &endptr, 10) - 1900;
        timeinfo->tm_hour = strtol(endptr + 1, &endptr, 10);
        timeinfo->tm_min = strtol(endptr + 1, &endptr, 10);
        mktime(timeinfo); // adjust other fields and compute weekday/daylight saving time
        return timeinfo;
    }
    catch (exception& e) {
        return NULL;
    }
}

//converts char[] to string 
std::string Char2Str(char* src, int size) {
    return std::string(src, size);
}

//makes the color more darker
void Darken(ImVec4* col) {
    col->x -= 0.1f;
    col->y -= 0.1f;
    col->w -= 0.1f;
    col->z -= 0.1f;
    if (col->x < 0) {
        col->x = 0;
    }
    if (col->y < 0) {
        col->y = 0;
    }
    if (col->z < 0) {
        col->z = 0;
    }
    if (col->w < 0) {
        col->w = 0;
    }
}

TaskList::TaskList() {
    name = "";
    theme_color = ImVec4(0, 0, 0, 0);
    in_action = false;
    action = 0;
    index = 0;
}

TaskList::TaskList(string _name, ImVec4 _theme_color) {
    theme_color = _theme_color;
    name = _name;
    in_action = false;
    action = 0;
    index = 0;
}

void TaskList::LoadFonts(string font_folder, bool mode) {
    if (mode) {
        FontText = ImGui::GetIO().Fonts->AddFontFromFileTTF((font_folder + "/Noto_Sans/NotoSans-Regular.ttf").c_str(), 30.0f);
        MyMergeIconsWithLatestFont(30.f, false);
        FontTitle = ImGui::GetIO().Fonts->AddFontFromFileTTF((font_folder + "/Noto_Sans/NotoSans-Regular.ttf").c_str(), 75.0f);
        MyMergeIconsWithLatestFont(30.f, false);
        FontDate = ImGui::GetIO().Fonts->AddFontFromFileTTF((font_folder + "/Noto_Sans/NotoSans-Regular.ttf").c_str(), 24.0f);
        MyMergeIconsWithLatestFont(24.f, false);
    }
    else {
        FontText = ImGui::GetIO().Fonts->AddFontFromFileTTF((font_folder + "/Roboto/Roboto-Medium.ttf").c_str(), 30.0f);
        MyMergeIconsWithLatestFont(30.f, false);
        FontTitle = ImGui::GetIO().Fonts->AddFontFromFileTTF((font_folder + "/Roboto/Roboto-Medium.ttf").c_str(), 75.0f);
        MyMergeIconsWithLatestFont(30.f, false);
        FontDate = ImGui::GetIO().Fonts->AddFontFromFileTTF((font_folder + "/Noto_Sans/NotoSans-Regular.ttf").c_str(), 24.0f);
        MyMergeIconsWithLatestFont(24.f, false);
    }
    
}

void TaskList::NewTask() {
    //tmp = new Task();
    //blur on bg
    ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Blur", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.5f));
    ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetWindowPos(), ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y), ImGui::GetColorU32(ImGuiCol_WindowBg));
    ImGui::PopStyleColor();
    ImGui::End();

    float windowWidth = 400;
    float windowHeight = 300;
    ImVec2 windowPos = ImVec2(ImGui::GetIO().DisplaySize.x / 2 - windowWidth / 2, ImGui::GetIO().DisplaySize.y / 2 - windowHeight / 2);
    ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
    ImGui::SetNextWindowPos(windowPos);
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
    ImGui::Begin("New task", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    //popup
    ImGui::PushFont(FontTitle);
    ImGui::TextColored(ImVec4(1, 1, 1, 1), "New task");
    ImGui::PopFont();
    ImGui::InputText("Title", &(tmp->title));
    //cout << "tmp->title: " << tmp->title << endl;
    ImGui::Checkbox("Have deadline", &tmp->have_date);
    if (tmp->have_date) {
        ImGui::InputText("Have deadline: ", &tmp->datetxt);
    }
    else {
        ImGui::BeginDisabled();
        ImGui::InputText("Deadline: ", &tmp->datetxt);
        ImGui::EndDisabled();
    }
    ImGui::PushFont(FontText);
   // ImGui::SetNextWindowPos(ImVec2(windowWidth - (FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Create").x + 10), windowHeight - (FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Vytvoøit").y + 10)));
   // ImGui::BeginChild(1, FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Create"), false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoResize);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0.7, 0, 1));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0.6, 0.1, 1));
    if(ImGui::Button("Create")){
        if (tmp->have_date) {
            //have deadline
            tmp->deadline = Char2Date(tmp->datetxt.c_str());
            if (tmp->deadline != NULL) {
                //something....
            }
            cout << "Creating new task(" << tmp->title << ", true,  tmp->deadline)" << endl;
            tasks.push_back(new Task(tmp->title, tmp->have_date, tmp->deadline));
        }
        else {
            cout << "Creating new task(" << tmp->title << ", false, a)" << endl;
            tasks.push_back(new Task(tmp->title, false, NULL));
        }
        SortTasks();
        in_action = false;
        action = 0;
        once = false;
        tmp = new Task();
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
  //  ImGui::EndChild();
    ImGui::PopFont();

    ImGui::PushFont(FontText);
  //  ImGui::SetNextWindowPos(ImVec2(10, windowHeight - (FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Zrušit").y + 10)));
   // ImGui::BeginChild(2, FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Zrušit"), false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoResize);
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8, 0, 0, 1));
    if (ImGui::Button("Cancel")) {
        in_action = false;
        action = 0;
        once = false;
        tmp = new Task();
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
   // ImGui::EndChild();
    ImGui::PopFont();
    ImGui::End();
}

void TaskList::EditTask(Task* t) {
    cout << "EditTask()" << endl;
    //tmp = new Task();
    //blur on bg
    ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Blur", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.5f));
    ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetWindowPos(), ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y), ImGui::GetColorU32(ImGuiCol_WindowBg));
    ImGui::PopStyleColor();
    ImGui::End();

    float windowWidth = 400;
    float windowHeight = 300;
    ImVec2 windowPos = ImVec2(ImGui::GetIO().DisplaySize.x / 2 - windowWidth / 2, ImGui::GetIO().DisplaySize.y / 2 - windowHeight / 2);
    ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
    ImGui::SetNextWindowPos(windowPos);
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
    ImGui::Begin("Upravit úkol", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    //popup
    ImGui::PushFont(FontTitle);
    ImGui::TextColored(ImVec4(1, 1, 1, 1), "Upravit úkol");
    ImGui::PopFont();
    ImGui::InputText("Název", &tmp->title);
    ImGui::Checkbox("Má datum dokonèení", &tmp->have_date);
    if (tmp->have_date) {
        ImGui::InputText("Datum dokonèení: ", &tmp->datetxt);
    }
    else {
        ImGui::BeginDisabled();
        ImGui::InputText("Datum dokonèení: ", &tmp->datetxt);
        ImGui::EndDisabled();
    }
    ImGui::PushFont(FontText);
    ImGui::SetNextWindowPos(ImVec2(windowWidth - (FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Vytvoøit").x + 10), windowHeight - (FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Vytvoøit").y + 10)));
    ImGui::BeginChild(1, FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Vytvoøit"), false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoResize);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 1, 0, 1));
    if (ImGui::Button("Upravit")) {
        if (tmp->have_date) {
            //have deadline
            tmp->deadline = Char2Date(tmp->datetxt.c_str());
            if (tmp->deadline != NULL) {
                //something....
            }
            auto it = find(tasks.begin(), tasks.end(), t);
            // If element was found
            if (it != tasks.end())
            {
                // calculating the index
                // of t
                int index = it - tasks.begin();
                tasks.at(index)->deadline = tmp->deadline;
                tasks.at(index)->have_date = tmp->have_date;
                tasks.at(index)->title = tmp->title;
            }
       
        }
        else {
            auto it = find(tasks.begin(), tasks.end(), t);
            // If element was found
            if (it != tasks.end())
            {
                // calculating the index
                // of t
                int index = it - tasks.begin();
                tasks.at(index)->deadline = tmp->deadline;
                tasks.at(index)->have_date = tmp->have_date;
                tasks.at(index)->title = tmp->title;
            }
        }
        SortTasks();
        in_action = false;
        action = 0;
        once = false;
        tmp = new Task();
    }
    ImGui::PopStyleColor();
    ImGui::EndChild();
    ImGui::PopFont();

    ImGui::PushFont(FontText);
    ImGui::SetNextWindowPos(ImVec2(10, windowHeight - (FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Zrušit").y + 10)));
    ImGui::BeginChild(2, FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Zrušit"), false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoResize);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 1));
    if (ImGui::Button("Cancel")) {
        in_action = false;
        action = 0;
        once = false;
        tmp = new Task();
    }
    ImGui::PopStyleColor();
    ImGui::EndChild();
    ImGui::PopFont();
    ImGui::End();
}

void TaskList::EditTask(int index) {
    //cout << "EditTask()" << endl;
    //tmp = new Task();
    //blur on bg
    ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Blur", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.5f));
    ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetWindowPos(), ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y), ImGui::GetColorU32(ImGuiCol_WindowBg));
    ImGui::PopStyleColor();
    ImGui::End();

   // float windowWidth = 400;
   // float windowHeight = 300;
   // ImVec2 windowPos = ImVec2(ImGui::GetIO().DisplaySize.x / 2 - windowWidth / 2, ImGui::GetIO().DisplaySize.y / 2 - windowHeight / 2);
    ImVec2 center = ImGui::CenterRect(ImVec2(400, 300), ImGui::GetIO().DisplaySize);
   // cout << "center(" << center.x << "," << center.y << ")" << endl;
   // cout << "windowpos(" << windowPos.x << "," << windowPos.y << ")" << endl;

    ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
    ImGui::SetNextWindowPos(center);
    ImGui::SetNextWindowSize(ImVec2(400, 300));
    ImGui::Begin("Edit task", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    //zatim nefunguje
    //ImGui::BeginPopup("Edit task", ImVec2(400, 300), center, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    //popup
    ImGui::PushFont(FontTitle);
    ImGui::TextColored(ImVec4(1, 1, 1, 1), "Edit task");
    ImGui::PopFont();
    ImGui::InputText2("Name", &tmp->title, 64);
    ImGui::Checkbox("Have deadline", &tmp->have_date);
    if (tmp->have_date) {
        ImGui::InputText2("Deadline: ", &tmp->datetxt,32);
    }
    else {
        ImGui::BeginDisabled();
        ImGui::InputText2("Deadline: ", &tmp->datetxt,32);
        ImGui::EndDisabled();
    }
    ImGui::PushFont(FontText);
    //ImGui::SetNextWindowPos(ImVec2(windowWidth - (FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Vytvoøit").x + 10), windowHeight - (FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Vytvoøit").y + 10)));
   // ImGui::BeginChild("EditWin", FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Vytvoøit"), false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoResize);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0.8, 0.1, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0.8, 0.1, 1));
    if (ImGui::Button("Edit")) {
        cout << "Editing task at: " << index << "(" << tasks.at(index)->title  << ")" << endl;
        if (tmp->have_date) {
            //have deadline
            tmp->deadline = Char2Date(tmp->datetxt.c_str());
            if (tmp->deadline != NULL) {
                //something....
            }
            tasks.at(index)->deadline = tmp->deadline;
            tasks.at(index)->have_date = tmp->have_date;
            tasks.at(index)->title = tmp->title;
        }
        else {
            tasks.at(index)->deadline = tmp->deadline;
            tasks.at(index)->have_date = tmp->have_date;
            tasks.at(index)->title = tmp->title; 
        }
        cout << "To: " << tasks.at(index)->title << endl;
        SortTasks();
        in_action = false;
        action = 0;
        tmp = new Task();
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
   // ImGui::EndChild();
    ImGui::PopFont();

    if (ImGui::IsKeyPressed(ImGuiKey_5)) {
        if (tmp->have_date) {
            //have deadline
            tmp->deadline = Char2Date(tmp->datetxt.c_str());
            if (tmp->deadline != NULL) {
                //something....
            }
            tasks.at(index)->deadline = tmp->deadline;
            tasks.at(index)->have_date = tmp->have_date;
            tasks.at(index)->title = tmp->title;
        }
        else {
            tasks.at(index)->deadline = tmp->deadline;
            tasks.at(index)->have_date = tmp->have_date;
            tasks.at(index)->title = tmp->title;
        }
        SortTasks();
        in_action = false;
        action = 0;
        once = false;
        tmp = new Task();
    }

    ImGui::PushFont(FontText);
   // ImGui::SetNextWindowPos(ImVec2(10, windowHeight - (FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Zrušit").y + 10)));
  //  ImGui::BeginChild("CancelWin", FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Zrušit"), false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoResize);
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8, 0, 0, 1));
    if (ImGui::Button("Cancel")) {
        cout << "Editing task at: " << index << " was canceled." << endl;
        in_action = false;
        action = 0;
        once = false;
        tmp = new Task();
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
   // ImGui::EndChild();
    ImGui::PopFont();
    ImGui::End();
}

void TaskList::DeleteTask(Task* t) {
    cout << "DeleteTask()" << endl;
    //dialog for asking...

    //tmp = new Task();
    //blur on bg
    ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Blur", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.5f));
    ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetWindowPos(), ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y), ImGui::GetColorU32(ImGuiCol_WindowBg));
    ImGui::PopStyleColor();
    ImGui::End();

    float windowWidth = 400;
    float windowHeight = 300;
    ImVec2 windowPos = ImVec2(ImGui::GetIO().DisplaySize.x / 2 - windowWidth / 2, ImGui::GetIO().DisplaySize.y / 2 - windowHeight / 2);
    ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
    ImGui::SetNextWindowPos(windowPos);
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
    ImGui::Begin("Smazat úkol", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    //popup
    ImGui::PushFont(FontTitle);
    ImGui::TextColored(ImVec4(1, 1, 1, 1), "Smazat úkol");
    ImGui::PopFont();
    
    ImGui::PushFont(FontText);
    ImGui::SetNextWindowPos(ImVec2(windowWidth - (FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Smazat").x + 10), windowHeight - (FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Smazat").y + 10)));
    ImGui::BeginChild(1, FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Smazat"), false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoResize);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 1, 0, 1));
    if (ImGui::Button("Smazat")) {
        SortTasks();
        tasks.erase(std::remove(tasks.begin(), tasks.end(), t), tasks.end());
        in_action = false;
        action = 0;
        tmp = new Task();
    }
    ImGui::PopStyleColor();
    ImGui::EndChild();
    ImGui::PopFont();
    ImGui::PushFont(FontText);
    ImGui::SetNextWindowPos(ImVec2(10, windowHeight - (FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Vytvoøit").y + 10)));
    ImGui::BeginChild(2, FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Zrušit"), false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoResize);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 1));
    if (ImGui::Button("Cancel")) {
        in_action = false;
        action = 0;
        tmp = new Task();
    }
    ImGui::PopStyleColor();
    ImGui::EndChild();
    ImGui::PopFont();
    ImGui::End();


}

void TaskList::DeleteTask(int index) {
    
    //tmp = new Task();
    //blur on bg
    ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Blur2", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
    
   // ImGui::BeginPopup("Blur", ImGui::GetIO().DisplaySize, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.5f));
    ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetWindowPos(), ImGui::GetWindowPos()+ImGui::GetWindowSize(), ImGui::GetColorU32(ImGuiCol_WindowBg));
    ImGui::PopStyleColor();
    ImGui::End();

    float windowWidth = 400;
    float windowHeight = 300;
    ImVec2 windowPos = ImVec2(ImGui::GetIO().DisplaySize.x / 2 - windowWidth / 2, ImGui::GetIO().DisplaySize.y / 2 - windowHeight / 2);
    ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
    ImGui::SetNextWindowPos(windowPos);
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
    ImGui::Begin("Delete task", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    //popup
    ImGui::PushFont(FontTitle);
    ImGui::TextColored(ImVec4(1, 1, 1, 1), "Delete task");
    ImGui::PopFont();
   
    ImGui::PushFont(FontText);
   // ImGui::SetNextWindowPos(ImVec2(windowWidth - (FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Delete").x + 10), windowHeight - (FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Delete").y + 10)));
   // ImGui::BeginChild(1, FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Delete"), false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoResize);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8, 0, 0, 1));
    if (ImGui::Button("Delete")) {
        cout << "Erasing task at: " << index << "(" << tasks.at(index)->title << ")" << endl;
        delete tasks.at(index);
        tasks.erase(tasks.begin() + index);
        SortTasks();
        in_action = false;
        action = 0;
        once = false;
        tmp = new Task();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
        cout << "Erasing task at: " << index << "(" << tasks.at(index)->title << ")" << endl;
        delete tasks.at(index);
        tasks.erase(tasks.begin() + index);
        SortTasks();
        in_action = false;
        action = 0;
        once = false;
        tmp = new Task();
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
   // ImGui::EndChild();
    ImGui::PopFont();
    ImGui::PushFont(FontText);
  //  ImGui::SetNextWindowPos(ImVec2(10, windowHeight - (FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Vytvoøit").y + 10)));
  //  ImGui::BeginChild(2, FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, "Cancel"), false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoResize);
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0.7, 0, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0.7, 0, 1));
    if (ImGui::Button("Cancel")) {
        cout << "Erasing task at: " << index << "was canceled." << endl;
        in_action = false;
        action = 0;
        once = false;
        tmp = new Task();
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
 //   ImGui::EndChild();
    ImGui::PopFont();
    ImGui::End();
}

void TaskList::SortTasks() {
    cout << "SortTasks()" << endl;
    // Sort by deadline
    std::sort(tasks.begin(), tasks.end(), compareTasksByDeadline);

    // Sort by name
    std::sort(tasks.begin(), tasks.end(), compareTasksByName);

    // Sort tasks with no deadline separately by name
    auto firstNoDeadline = std::stable_partition(tasks.begin(), tasks.end(),
        [](Task* task) { return task->deadline != nullptr; });
    std::sort(firstNoDeadline, tasks.end(), compareTasksByName);
}

void TaskList::Draw(ImVec2 lu_corn, ImVec2 rd_corn, int w, int h, ImVec2 cursorpos) {
    if (h <= 0) {
        cout << "h <= 0" << endl;
        throw runtime_error("h <= 0");
    }
    if (w <= 0) {
        cout << "w <= 0" << endl;
        throw runtime_error("w <= 0");
    }
    
    ImGui::SetNextWindowPos(ImVec2(lu_corn.x+50, lu_corn.y+20));
    ImGui::BeginChild(90, ImVec2(0, 0), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav);// | ImGuiWindowFlags_NoTitleBar);
    ImGui::PushFont(FontTitle);
    ImGui::TextColored(ImVec4(1,1,1,1), name.c_str());
   // cout << "name: " << name.c_str() << endl;
    ImGui::SameLine();
    ImGui::PopFont();
    ImGui::PushFont(FontText);
    if (ImGui::Button(ICON_FA_SHARE_ALT_SQUARE)) {
        //save/share
        if (CopyToClipboard()) {
            //ok
            cout << "Tasks copied to clipboard" << endl;
        }
        else {
            
        }
    }

    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2, 0.9, 0.4, 1));//179, 28, 2
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4, 0.9, 0.4, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4, 0.9, 0.4, 1));
   // ImGui::PushFont(FontText);
    if (ImGui::Button("\xef\x81\xa7  New task")) {
        in_action = true;
        action = 3;
        cout << "New task button 1." << endl;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);
    ImGui::EndChild();

    ImGui::SetNextWindowPos(ImVec2(lu_corn.x + 50 + 50 + FontTitle->CalcTextSizeA(FontTitle->FontSize, FLT_MAX,0,"Title").x, lu_corn.y + 20));
    ImGui::BeginChild(91, ImVec2(0, 0), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar);
    ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(0.2, 0.9, 0.4, 1));//179, 28, 2
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4, 0.9, 0.4, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4, 0.9, 0.4, 1));
    ImGui::PushFont(FontText);
    if(ImGui::Button("\xef\x81\xa7  New task") ){
        in_action = true;
        action = 3;
        cout << "New task button 2." << endl;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);
    ImGui::EndChild();


    ImVec2 tsize = FontTitle->CalcTextSizeA(FontTitle->FontSize, FLT_MAX, 0, "Title");


    //tasklist
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
    ImGui::SetNextWindowPos(ImVec2(lu_corn.x + 50 , lu_corn.y + 20 + tsize.y + 20));
    //w - (50*2) - lu_corn.x
    ImGui::SetNextWindowSize(ImVec2(w - (50 * 2) - lu_corn.x, 0));
    ImGui::BeginChild(92, ImVec2(0,0), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar);
    // Get window size
    ImVec2 window_size = ImGui::GetWindowSize();
    float table_width = window_size.x - ImGui::GetStyle().ScrollbarSize - ImGui::GetStyle().WindowPadding.x * 2;

    // Position table in the bottom-right corner of the window
    ImGui::SetCursorPosX(0);
    ImGui::SetCursorPosY(0);

    // Begin table with rounded borders and background color
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4, 4));
    ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TableBorderLight, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
    ImGui::BeginTable("table1", 1, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Borders);

    // Set up column with header
    //ImGui::TableSetupColumn("Column 1");
    //ImGui::TableHeadersRow();

    // Add rows to table with alternating row colors
    for (int i = 0; i < tasks.size(); i++) {
        ImGui::TableNextRow();
        if (i % 2 == 0) {
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(255, 0, 0, 255));
        }
        ImGui::TableSetColumnIndex(0);
        /*if (FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, tasks.at(i)->title).x >= (w - (50 * 2) - lu_corn.x)) {
            //text is bigger than size of table
            for (int y = 0; y < 25; y++) {

            }
        }*/
        ImGui::PushItemWidth(w - (50 * 2) - lu_corn.x);
        ImGui::PushFont(FontText);
        ImGui::Checkbox(tasks.at(i)->title.c_str(), &tasks.at(i)->completed);
       // ImGui::Checkbox("c", &tasks.at(i)->completed);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(-ImGui::GetStyle().ItemSpacing.x, 0));
        ImGui::SameLine(FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, tasks.at(i)->title.c_str()).x + 40.f);
        string b = "\xef\x8c\x84##" + to_string(i);//ICON_FA_PEN
        if (ImGui::Button(b.c_str())) {
            //edit
            in_action = true;
            action = 1;
            index = i;
            cout << "Edit task for: " << i << endl;
        }
        ImGui::SameLine(FontText->CalcTextSizeA(FontText->FontSize, FLT_MAX, 0, tasks.at(i)->title.c_str()).x + 65.f);
        string a = "\xef\x8b\xad##" + to_string(i);//ICON_FA_TRASH_ALT
        if (ImGui::Button(a.c_str())) {
            //delete
            in_action = true;
            action = 2;
            index = i;
            cout << "Delete task for: " << i << endl;
        }
        ImGui::PopStyleVar();
        ImGui::PopFont();
        // Set maximum width of the cell
     //  ImGui::TextWrapped("%s", values[i].c_str()); // Wrap the text
        
        ImGui::PopItemWidth();
    }

    // End table and pop styles
    ImGui::EndTable();
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();


    ImGui::EndChild();
    ImGui::PopStyleVar();
}


bool TaskList::Load(const char* filename, int mode) {
    try {
        //load from json file
        std::ifstream f(filename);
        json data = json::parse(f);
        
        auto tasklist = data["tasklist"];

        // Access the name and theme_color properties of the tasklist object
        name = tasklist["name"];
        theme_color.x = tasklist["theme_color"][0];
        theme_color.y = tasklist["theme_color"][1];
        theme_color.z = tasklist["theme_color"][2];
        theme_color.w = tasklist["theme_color"][3];

        // Access the tasks array and loop through each task object
        for (const auto& task : tasklist["tasks"]) {
            // Access the properties of the task object
            tasks.push_back(new Task(task["title"].get<std::string>().c_str(),  task["havedate"].get<bool>(), task["completed"].get<bool>(), Str2Date(task["date"].get<std::string>().c_str())));
        }
        SortTasks();
        return true;
    }
    catch (exception& e) {
        return false;
    }
}

bool TaskList::Export(const char* filename, int mode) {
    try {
        // Create a JSON object with the desired format
        json j;
        j["tasklist"]["name"] = name;
        j["tasklist"]["theme_color"] = { theme_color.x, theme_color.y, theme_color.z, theme_color.w };
        j["tasklist"]["tasks"] = json::array();

        // Add each task to the "tasks" array
        for (auto task : tasks) {
            json task_json;
            task_json["title"] = task->title;
            task_json["date"] = task->datetxt;
            task_json["completed"] = task->completed;
            task_json["havedate"] = task->have_date;
            j["tasklist"]["tasks"].push_back(task_json);
        }

        // Write the JSON object to a file
        std::ofstream file(filename);
        file << std::setw(4) << j << std::endl;
        return true;
    }
    catch (exception& e) {
        return false;
    }
}

bool TaskList::CopyToClipboard() {
    try {
        json j;
        j["tasklist"]["name"] = name;
        j["tasklist"]["theme_color"] = { theme_color.x, theme_color.y, theme_color.z, theme_color.w };
        j["tasklist"]["tasks"] = json::array();

        // Add each task to the "tasks" array
        for (auto task : tasks) {
            json task_json;
            task_json["title"] = task->title;
            task_json["date"] = task->datetxt;
            task_json["completed"] = task->completed;
            task_json["havedate"] = task->have_date;
            j["tasklist"]["tasks"].push_back(task_json);
        }

        string str = j.dump();

        if (str.length() == 0) {
            throw runtime_error("cannot copy empty string to clipboard.");
        }

        if (!OpenClipboard(NULL))
            return false;

        // Clear the clipboard
        if (!EmptyClipboard()) {
            throw runtime_error("cannot clear the clipboard.");
        }

        // Get the size of the string
        size_t len = str.length();

        // Allocate memory for the string
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len + 1);
        if (hMem == NULL) {
            throw runtime_error("cannot allocate memory for the string.");
        }

        // Lock the memory and copy the string into it
        char* pBuf = (char*)GlobalLock(hMem);
        if (pBuf == NULL) {
            throw runtime_error("can't lock memory to copy string");
        }
        memcpy(pBuf, str.c_str(), len + 1);
        GlobalUnlock(hMem);

        // Set the clipboard data
        if (!SetClipboardData(CF_TEXT, hMem)) {
            throw runtime_error("cannot set the cliboard data.");
        }

        // Close the clipboard
        if (!CloseClipboard()) {
            throw runtime_error("cannot close the clipboard.");
        }
        return true;
    }
    catch (exception& e) {
        cout << "ERROR: " << e.what() << endl;
        return false;
    }
}

TaskWidget::TaskWidget() {
    index = 0;
    action = 0;
    in_action = false;
    active = nullptr;
    active_index = 0;
}

TaskWidget::TaskWidget(vector<TaskList*> _lists) {
    lists = _lists;
    if (lists.size() <= 0 || lists.empty()) {
        throw runtime_error("list of task lists cant be empty.");
    }
    for (int i = 0; i < lists.size(); i++) {
        if (lists.at(i)->tasks.size() <= 0 || lists.at(i)->tasks.empty()) {
            cout << "WARNING: tasklist at(" << i << ") is empty." << endl;
        }
        for (int y = 0; y < lists.at(i)->tasks.size(); y++) {
            cout << "titles: " << string(lists.at(i)->tasks.at(y)->title) << endl;
        }
    }
    index = 0;
    action = 0;
    in_action = false;
    active = &lists.at(0);
    active_index = 0;
}

void TaskWidget::NewTaskList() {
    cout << "New task list " << endl;
}

void TaskWidget::EditTaskList(TaskList* t) {
    cout << "Editing task list for: " << t->name.c_str() << endl;
}

void TaskWidget::EditTaskList(int index) {
    cout << "Editing task list for i: " << index << endl;
}

void TaskWidget::DeleteTaskList(TaskList* t) {
    cout << "DeleteTaskList()" << endl;
    ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Blur", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.5f));
    ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetWindowPos(), ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y), ImGui::GetColorU32(ImGuiCol_WindowBg));
    ImGui::PopStyleColor();
    ImGui::End();

    float windowWidth = 400;
    float windowHeight = 300;
    ImVec2 windowPos = ImVec2(ImGui::GetIO().DisplaySize.x / 2 - windowWidth / 2, ImGui::GetIO().DisplaySize.y / 2 - windowHeight / 2);
    ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
    ImGui::SetNextWindowPos(windowPos);
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
    ImGui::Begin("Smazat seznam úkolù", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    //popup
    ImGui::PushFont((*active)->FontTitle);
    ImGui::TextColored(ImVec4(1, 1, 1, 1), "Smazat seznam úkolù");
    ImGui::PopFont();

    ImGui::PushFont((*active)->FontText);
    ImGui::SetNextWindowPos(ImVec2(windowWidth - ((*active)->FontText->CalcTextSizeA((*active)->FontText->FontSize, FLT_MAX, 0, "Smazat").x + 10), windowHeight - ((*active)->FontText->CalcTextSizeA((*active)->FontText->FontSize, FLT_MAX, 0, "Smazat").y + 10)));
    ImGui::BeginChild(1, (*active)->FontText->CalcTextSizeA((*active)->FontText->FontSize, FLT_MAX, 0, "Smazat"), false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoResize);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 1, 0, 1));
    if (ImGui::Button("Smazat")) {
        lists.erase(std::remove(lists.begin(), lists.end(), t), lists.end());
        in_action = false;
        action = 0;
    }
    ImGui::PopStyleColor();
    ImGui::EndChild();
    ImGui::PopFont();
    ImGui::PushFont((*active)->FontText);
    ImGui::SetNextWindowPos(ImVec2(10, windowHeight - ((*active)->FontText->CalcTextSizeA((*active)->FontText->FontSize, FLT_MAX, 0, "Vytvoøit").y + 10)));
    ImGui::BeginChild(2, (*active)->FontText->CalcTextSizeA((*active)->FontText->FontSize, FLT_MAX, 0, "Zrušit"), false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoResize);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 1));
    if (ImGui::Button("Cancel")) {
        in_action = false;
        action = 0;
    }
    ImGui::PopStyleColor();
    ImGui::EndChild();
    ImGui::PopFont();
    ImGui::End();
}

void TaskWidget::DeleteTaskList(int index) {
    cout << "DeleteTaskList()" << endl;
    ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Blur", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.5f));
    ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetWindowPos(), ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y), ImGui::GetColorU32(ImGuiCol_WindowBg));
    ImGui::PopStyleColor();
    ImGui::End();

    float windowWidth = 400;
    float windowHeight = 300;
    ImVec2 windowPos = ImVec2(ImGui::GetIO().DisplaySize.x / 2 - windowWidth / 2, ImGui::GetIO().DisplaySize.y / 2 - windowHeight / 2);
    ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
    ImGui::SetNextWindowPos(windowPos);
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
    ImGui::Begin("Smazat seznam úkolù", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    //popup
    ImGui::PushFont((*active)->FontTitle);
    ImGui::TextColored(ImVec4(1, 1, 1, 1), "Smazat seznam úkolù");
    ImGui::PopFont();

    ImGui::PushFont((*active)->FontText);
    ImGui::SetNextWindowPos(ImVec2(windowWidth - ((*active)->FontText->CalcTextSizeA((*active)->FontText->FontSize, FLT_MAX, 0, "Smazat").x + 10), windowHeight - ((*active)->FontText->CalcTextSizeA((*active)->FontText->FontSize, FLT_MAX, 0, "Smazat").y + 10)));
    ImGui::BeginChild(1, (*active)->FontText->CalcTextSizeA((*active)->FontText->FontSize, FLT_MAX, 0, "Smazat"), false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoResize);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 1, 0, 1));
    if (ImGui::Button("Smazat")) {
        cout << "Erasing task List at: " << index << "(" << lists.at(index)->name <<  ")" << endl;
        delete lists.at(index);
        
        lists.erase(lists.begin() + index);
        in_action = false;
        action = 0;
    }
    ImGui::PopStyleColor();
    ImGui::EndChild();
    ImGui::PopFont();
    ImGui::PushFont((*active)->FontText);
    ImGui::SetNextWindowPos(ImVec2(10, windowHeight - ((*active)->FontText->CalcTextSizeA((*active)->FontText->FontSize, FLT_MAX, 0, "Vytvoøit").y + 10)));
    ImGui::BeginChild(2, (*active)->FontText->CalcTextSizeA((*active)->FontText->FontSize, FLT_MAX, 0, "Zrušit"), false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoResize);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 1));
    if (ImGui::Button("Cancel")) {
        in_action = false;
        action = 0;
    }
    ImGui::PopStyleColor();
    ImGui::EndChild();
    ImGui::PopFont();
    ImGui::End();
}


void TaskWidget::Draw(int w, int h, ImVec2 cursorpos) {
    //horni cast: 15% z h -> main panel
    //vlevo: 15% z w -> seznam tasklistu

    //samotny active tasklist
    (*active)->Draw(ImVec2((w / 100) * 15, ((h / 100) * 15)+50), ImVec2(w, h), w, h, cursorpos);

    /*
    //draw left table
    //h = h-15%
    //w = 15%
    ImGui::SetNextWindowPos(ImVec2(0, (h / 100) * 15));
    ImGui::SetNextWindowSize(ImVec2((h - (h / 100) * 15), (w / 100) * 15));
    ImGui::BeginChild(70, ImVec2((h-(h / 100) * 15), (w / 100) * 15), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar);

    ImVec2 window_size = ImGui::GetWindowSize();
    float table_width = window_size.x - ImGui::GetStyle().ScrollbarSize - ImGui::GetStyle().WindowPadding.x * 2;

    // Position table in the bottom-right corner of the window
    ImGui::SetCursorPosX(0);
    ImGui::SetCursorPosY(0);

    // Begin table with rounded borders and background color
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4, 4));
    ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TableBorderLight, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
    ImGui::BeginTable("table4", 1, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Borders);

    // Set up column with header
    //ImGui::TableSetupColumn("Column 1");
    //ImGui::TableHeadersRow();

    // Add rows to table with alternating row colors
    for (int i = 0; i < lists.size(); i++) {
        ImGui::TableNextRow();
        if (i % 2 == 0) {
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(255, 0, 0, 255));
        }
        ImGui::TableSetColumnIndex(0);
        ImGui::PushItemWidth((h - (h / 100) * 15));
        ImGui::PushFont((*active)->FontText);
        ImGui::Button(lists.at(i)->name.c_str());
        ImGui::PopFont();
        // Set maximum width of the cell
     //  ImGui::TextWrapped("%s", values[i].c_str()); // Wrap the text
        ImGui::PopItemWidth();
    }

    // End table and pop styles
    ImGui::EndTable();
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    //ImGui::PopStyleVar();
    
    ImGui::EndChild();
    */


    ImGui::End();
    

    //check for actions in active list
    if ((*active)->in_action) {
        if ((*active)->action == 1) {
            //editing task..
            if (!(*active)->once) {
                (*active)->tmp = (*active)->tasks.at((*active)->index);
                (*active)->once = true;
            }
            (*active)->EditTask((*active)->index);
        }else if((*active)->action == 2) {
            //delete task..
            if (!(*active)->once) {
                (*active)->tmp = (*active)->tasks.at((*active)->index);
                (*active)->once = true;
            }
            (*active)->DeleteTask((*active)->index);
        }
        else if ((*active)->action == 3) {
            //new task..
            if (!(*active)->once) {
                (*active)->tmp = new Task();
                (*active)->once = true;
            }
            (*active)->NewTask();
        }
    }
    else {
        if (in_action) {
            if (action == 1) {
                //editing task list..
                EditTaskList(index);
            }
            else if (action == 2) {
                //delete task list..
                DeleteTaskList(index);
            }
            else if (action == 3) {
                //new task list..
                NewTaskList();
            }
        }
    }
    
}

void TaskWidget::SetActive(int index) {
    active = &lists.at(index);
}


//
