//My files
#include "Application.h"
#include "LinearMotionProfile.h"
#include "ConversionCalculator.h"

//Imgui and Implot
#include "..//imgui/imgui.h"
#include "..//implot/implot.h"

#include <iostream>

#include <chrono>
#include <filesystem>
#include <fstream>


//Terminal 
#include <sstream>
#include <string>

//Windows
#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>


#ifdef _MSC_VER
#define sprintf sprintf_s
#endif

#ifndef PI
#define PI 3.14159265358979323846
#endif

#define CHECKBOX_FLAG(flags, flag) ImGui::CheckboxFlags(#flag, (unsigned int*)&flags, flag)


ConversionCalculator my_calculator;
ConversionCalculator rpm_calculator;

namespace fs = std::filesystem;
wchar_t test[260];

std::string wstring_to_string(const std::wstring& wstr) {
    std::string str(wstr.begin(), wstr.end());
    return str;
}

std::wstring ShowSelectFolderDialog() {
    BROWSEINFO bi = { 0 };  // Estructura para el cuadro de diálogo de selección de carpetas
    bi.lpszTitle = L"Select a folder";
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);  // Abre el cuadro de diálogo
    if (pidl != 0) {
        // Convertir la selección a una ruta de directorio
        wchar_t path[MAX_PATH];
        if (SHGetPathFromIDList(pidl, path)) {
            return std::wstring(path);  // Retorna la ruta seleccionada
        }
        CoTaskMemFree(pidl);  // Libera la memoria asignada
    }
    return L"";  // Retorna una cadena vacía si hay un error o no se seleccionó una carpeta
}

std::wstring ShowSaveFileDialog(FileType type) {
    OPENFILENAME ofn;        // Estructura para almacenar información del cuadro de diálogo
    static wchar_t szFile[260] = L"motion.light.txt" ;  // Buffer para el nombre del archivo
    static wchar_t csvFile[260] = L"motion.light.csv"; 

    HWND hwnd = NULL;        // Manejador de ventana principal (opcional)

    
    // Inicializa la estructura OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrTitle = L"MotionLight";
    

    if (type == TXT) {
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);  // Cambia el tamaño para ser compatible con wchar_t
        ofn.lpstrFilter = L".txt\0";  // Usa cadenas wide (prefijo L)
    }

    if (type == CSV) {
        ofn.lpstrFile = csvFile;
        ofn.nMaxFile = sizeof(csvFile) / sizeof(wchar_t);  // Cambia el tamaño para ser compatible con wchar_t
        ofn.lpstrFilter = L".csv\0";  // Usa cadenas wide (prefijo L)
    }
    ofn.nFilterIndex = 1;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

    // Muestra el cuadro de diálogo "Guardar como"
    if (GetSaveFileNameW(&ofn) == TRUE) {  // Llama a la versión Unicode de la función
        return std::wstring(ofn.lpstrFile);
    }
    return std::wstring(L"");  // Retorna una cadena vacía si hay un error
}

std::wstring ShowOpenFileDialog() {
    OPENFILENAME ofn;        // Estructura para almacenar información del cuadro de diálogo
    wchar_t szFile[260] = { 0 };  // Buffer para el nombre del archivo
    HWND hwnd = NULL;        // Manejador de ventana principal (opcional)

    // Inicializa la estructura OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
    ofn.lpstrFilter = L".txt\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;  // Configuración para "abrir" archivo

    // Muestra el cuadro de diálogo "Abrir"
    if (GetOpenFileNameW(&ofn) == TRUE) {
        return std::wstring(ofn.lpstrFile);
    }
    return std::wstring(L"");  // Retorna una cadena vacía si hay un error
}



namespace MyApp
{
    //Kinemactics
    float Velocity = 0.0f;
    float Acceleration = 0.5f;
    float Desacceleration = 0.0f; 
    float Jerk = 0.0f;
    float iDiameter = 1; 
  
    float max_velocity = 0;
    float max_acceleration = 0;
    
    float min_velocity = FLT_MAX;
    float min_acceleration = FLT_MAX;

    //Targets
    float Distance = 0.0f;
    float SampleTime = 0.0f;
    float total_time = 0.0f; 

    bool run_simulation = false;
    bool opened_file = false; 

    float vel_axis_max = 1;
    float vel_axis_min = -1;

    float accel_axis_max = 1;
    float accel_axis_min = -1; 

    float simulation_time = 0; 
  
    //Profiler
    int profiler = 0;

    //Vectors
    std::vector<float> vel;
    std::vector<float> accel;
    std::vector<float> pos;

    std::vector<float> time_vector;

    //Terminal
    std::string TerminalOutput ="";
    char bufferSimulation[32];

    std::unique_ptr<char[]> ArrayOutput= std::make_unique<char[]>(100);

    //Windows stuff
    fs::path  current_path = fs::current_path();
    fs::path write_file_path=".";
    fs::path read_file_path="*";
    fs::path csv_file_path ="-";

    fs::path terminal_path = "";

    void WriteMotionFile(fs::path srcpath) {
        std::stringstream ss;

        ss <<profiler<<","<<Velocity<<","<<Acceleration<<","<<Desacceleration<<","<<Jerk<<","<<Distance<<","<<SampleTime<<"\n";

        for (int i = 0; i < vel.size(); i++) {
            ss << vel[i] << "\n";
        }

        std::ofstream file (srcpath);

        if (!file.is_open()) {
            std::cerr << "The file can not opened." << std::endl;
            return;
        }
        
        file << ss.str();

        file.close(); 
    }

    void ReadMotionFile(fs::path srcpath) {
        std::ifstream file(srcpath);
        if (!file.is_open()) {
            std::cerr << "The file can not opened" << std::endl;
            return; 
        }

        std::string line;

        if (std::getline(file,line)){
            std::istringstream iss(line);
            std::string token;
            std::vector<float> parameters;

            while (std::getline(iss, token, ',')) {
                parameters.push_back(static_cast<float>(std::stod(token)));
            }

            profiler = static_cast<int>(parameters[0]);
            Velocity = parameters[1];
            Acceleration = parameters[2];
            Desacceleration = parameters[3];
            Jerk = parameters[4];
            Distance = parameters[5];
            SampleTime = parameters[6];

          
        }        

        opened_file = true; 
        file.close(); 
    }

    void Writecsv(fs::path srcpath) {
        std::stringstream ss;
        ss << "Time[sec],Position[m],Velocity[m/s2],Acceleration[m/s3],Jerk[m/s3]"<< "\n";

        for (int i = 0; i < vel.size(); i++) {
            ss <<time_vector[i]<<","<<pos[i]<<","<<vel[i]<<","<<accel[i]<<","<<Jerk<<"\n";
        }

        std::ofstream file(srcpath);

        if (!file.is_open()) {
            std::cerr << "The file can not opened." << std::endl;
            return;
        }

        file << ss.str();

        file.close();
    }

    std::unique_ptr<char[]> VectorToCommaSeparatedString(const std::vector<float>& vec) {
        // Usamos un stringstream para crear la cadena separada por comas
        std::stringstream ss;

        // Establecemos el formato de 4 decimales
        ss << std::fixed << std::setprecision(4);

        ss << "{"; 
        // Iteramos sobre el vector
        for (size_t i = 0; i < vec.size(); ++i) {
            ss << vec[i];  // Agregamos el número al stringstream con 4 decimales
            if (i < vec.size() - 1) {
                ss << ",";  // Agregamos una coma excepto después del último elemento
            }
        }

        ss << "}"; 
        // Convertimos el stringstream a std::string
        std::string result = ss.str();

        // Creamos un std::unique_ptr para manejar el arreglo de char
        std::unique_ptr<char[]> charArray(new char[result.size() + 1]);  // +1 para el terminador '\0'

        // Copiamos la cadena al arreglo de char
        std::copy(result.begin(), result.end(), charArray.get());

        // Añadimos el terminador null al final
        charArray[result.size()] = '\0';

        // Retornamos el std::unique_ptr
        return charArray;
    }

    void render_a_space(int spaces) {
        for (int i = 0; i < spaces; i++) {
            ImGui::Text(""); 
        }
    }

    void Profiler_calculations(LinearProfile::LinearMotionProfile &Profile) {
        int step_index = 0;
        if (SampleTime == 0) {
            SampleTime = 0.1f; 
        }

        for (float i = 0; i <= Profile.getTotalTime(); i += abs(SampleTime), step_index++) {
            pos.push_back(Profile.getPosition(i));
            vel.push_back(Profile.getVelocity(i));
            accel.push_back(Profile.getAcceleration(i));
            time_vector.push_back(i);

            max_velocity = vel[step_index] > max_velocity ? vel[step_index] : max_velocity;
            max_acceleration = accel[step_index] > max_acceleration ? accel[step_index] : max_acceleration;

            min_velocity = vel[step_index] < min_velocity ? vel[step_index] : min_velocity;
            min_acceleration = accel[step_index] < min_acceleration ? accel[step_index] : min_acceleration;

        }
        total_time = Profile.getTotalTime();
        pos.push_back(Distance);
        vel.push_back(0.0f);
        accel.push_back(0.0f);
        time_vector.push_back(total_time); 
        vel_axis_max = max_velocity + 1.0f;
        vel_axis_min = min_velocity - 1.0f;

        accel_axis_max = max_acceleration + 1.0f;
        accel_axis_min = max_acceleration - 3.0f;
    }

    void ConversionCalculator() {
        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

        ImGui::BeginTabBar("Conversion", tab_bar_flags);

        if (ImGui::BeginTabItem("Length"))
        {
            ImGui::Text("From:");
            
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * .25f);

            ImGui::DragFloat(" ", &my_calculator.raw_length); ImGui::SameLine();
            const char* items[] = { "Centimeter", "Kilometer", "Yard", "Mile", "Foot", "Inch"};
            static int item_current = 0;
            const char* combo_preview_value = items[item_current];

            if (ImGui::BeginCombo("Length Unit", combo_preview_value, ImGuiComboFlags_WidthFitPreview)) {
                for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                {
                    const bool is_selected = (item_current == n);
                    if (ImGui::Selectable(items[n], is_selected))
                        item_current = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            my_calculator.ConvertLength(static_cast <LengthUnits>(item_current));
            render_a_space(1);

            ImGui::Text("To:");
            ImGui::DragFloat("Meters",&my_calculator.length);
            my_calculator.PopLength(static_cast<LengthUnits>(item_current)); 
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Time"))
        {
            ImGui::Text("From:");
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * .25f);
            ImGui::DragFloat(" ", &my_calculator.raw_time); ImGui::SameLine();
          
            const char* items[] = { "Millis", "Minutes","Hours"};
            static int item_current = 0;
            const char* combo_preview_value = items[item_current];

            if (ImGui::BeginCombo("Time Unit", combo_preview_value, ImGuiComboFlags_WidthFitPreview)) {
                for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                {
                    const bool is_selected = (item_current == n);
                    if (ImGui::Selectable(items[n], is_selected))
                        item_current = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            my_calculator.ConvertTime(static_cast <TimeUnits>(item_current));
  
            render_a_space(1);
            ImGui::Text("To:");
            ImGui::DragFloat("Seconds", &my_calculator.time);
            my_calculator.PopTime(static_cast<TimeUnits>(item_current));
            
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Velocity"))
        {
            ImGui::Text("From:");
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * .25f);
            ImGui::DragFloat("  ", &my_calculator.raw_velocity); ImGui::SameLine();

            const char* items[] = { "cm/s", "yards/s","ft/s","in/s","km/hr","Mph"};
    
            static int item_current = 0;
            const char* combo_preview_value = items[item_current];

            if (ImGui::BeginCombo("Velocity Unit", combo_preview_value, ImGuiComboFlags_WidthFitPreview)) {
                for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                {
                    const bool is_selected = (item_current == n);
                    if (ImGui::Selectable(items[n], is_selected))
                        item_current = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            my_calculator.ConvertVelocity(static_cast <VelocityUnits>(item_current));

            render_a_space( 1);
            ImGui::Text("To: ");
            ImGui::DragFloat("m/s", &my_calculator.velocity);
            my_calculator.PopVelocity(static_cast<VelocityUnits>(item_current)); 
           
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Angular")) {
            ImGui::Text("From:");
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * .25f);
            ImGui::DragFloat("  ", &my_calculator.raw_angular_velocity); ImGui::SameLine();

            const char* items[] = { "RPM", "rad/s","deg/s" };

            static int item_current = 0;
            const char* combo_preview_value = items[item_current];

            if (ImGui::BeginCombo("Angular Velocity Unit", combo_preview_value, ImGuiComboFlags_WidthFitPreview)) {
                for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                {
                    const bool is_selected = (item_current == n);
                    if (ImGui::Selectable(items[n], is_selected))
                        item_current = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            my_calculator.ConvertRPMtoMS(static_cast<AngularUnits>(item_current),rpm_calculator.length);

            //////////
            ImGui::DragFloat("    ", &rpm_calculator.raw_length); ImGui::SameLine();
        
            const char* items_diameter[] = {"Centimeter", "Kilometer", "Yard", "Mile", "Foot", "Inch", "Meter"};

            static int item_current_diameter = 0;

            const char* combo_preview_value_diameter = items_diameter[item_current_diameter];

            if (ImGui::BeginCombo("Diameter unit", combo_preview_value_diameter, ImGuiComboFlags_WidthFitPreview)) {
                for (int n = 0; n < IM_ARRAYSIZE(items_diameter); n++)
                {
                    const bool is_selected = (item_current_diameter == n);
                    if (ImGui::Selectable(items_diameter[n], is_selected))
                        item_current_diameter = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            rpm_calculator.ConvertLength(static_cast <LengthUnits>(item_current_diameter));


            //---
            static char angular_conversion[32];
            snprintf(angular_conversion, 32, "%.6f", my_calculator.velocity);
            
            render_a_space(1);
            ImGui::Text(angular_conversion);  ImGui::SameLine();  ImGui::Text(" m/s");

            if (ImGui::Button("Copy Velocity")) {
                ImGui::SetClipboardText(angular_conversion);
            }

            ImGui::EndTabItem(); 
        }

        if (ImGui::BeginTabItem("Acceleration"))
        {
            ImGui::Text("From:");
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * .25f);
            ImGui::DragFloat("    ", &my_calculator.raw_acceleration); ImGui::SameLine();

            const char* items[] = { "cm/s2", "yards/s2","ft/s2","in/s2","km/hr2","Mph2" };

            static int item_current = 0;
            
            const char* combo_preview_value = items[item_current];

            if (ImGui::BeginCombo("Acceleration Unit", combo_preview_value, ImGuiComboFlags_WidthFitPreview)) {
                for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                {
                    const bool is_selected = (item_current == n);
                    if (ImGui::Selectable(items[n], is_selected))
                        item_current = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            my_calculator.ConvertAccel(static_cast <AccelerationUnits>(item_current));
      
            render_a_space(1);
            ImGui::Text("To:");
            ImGui::DragFloat("m/s2", &my_calculator.acceleration);
            my_calculator.PopAccel(static_cast<AccelerationUnits>(item_current));

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Jerk"))
        {
            ImGui::Text("From:");
            ImGui::DragFloat("     ", &my_calculator.raw_jerk); ImGui::SameLine();

            const char* items[] = { "cm/s3", "yards/s3","ft/s3","in/s3"};

            static int item_current = 0;
            const char* combo_preview_value = items[item_current];

            if (ImGui::BeginCombo("Jerk Unit", combo_preview_value, ImGuiComboFlags_WidthFitPreview)) {
                for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                {
                    const bool is_selected = (item_current == n);
                    if (ImGui::Selectable(items[n], is_selected))
                        item_current = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            my_calculator.ConvertJerk(static_cast <JerkUnits>(item_current));
 
            render_a_space(1);
            ImGui::Text("To:");
            ImGui::DragFloat("m/s3", &my_calculator.jerk);
            my_calculator.PopJerk(static_cast<JerkUnits>(item_current));

            ImGui::EndTabItem();
        }



        ImGui::EndTabBar();
    }

    void RenderParametersView() {
        
        ImGui::Begin("Configuration");
        ImGui::Text("Kinemactic Values [m/s]");
        ImGui::DragFloat("Velocity[m/s]", &Velocity);
        ImGui::Text("Acceleration can not be 0");
        ImGui::DragFloat("Acceleration", &Acceleration);
        ImGui::DragFloat("Desacceleration", &Desacceleration); 
        ImGui::Text("Jerk 0 for Trapezoidal");
        ImGui::DragFloat("Jerk", &Jerk);


        //Calculations stuffs
        ImGui::Text("Profile Inputs");
        ImGui::DragFloat("Distance [m]", &Distance);
        ImGui::DragFloat("Sample Time [s]", &SampleTime);

        ImGui::Separator();
        ImGui::Text("Profilers");
      
        ImGui::RadioButton("Trapezoidal", &profiler, 0); ImGui::SameLine();
        ImGui::RadioButton("S curve", &profiler, 1);

        if (profiler == 0.0f) {
            Jerk = 0.0f;
        }

        if (Acceleration == 0.0f) {
                Acceleration = .0001f;
        }

        if (Velocity == 0.0f) {
            Velocity = .0001f; 
        }

     
        ImGui::PushID(2);
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(4 / 7.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(4 / 7.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(4 / 7.0f, 0.8f, 0.8f));

        static ImGuiInputFlags route_options = ImGuiInputFlags_Repeat;
        static ImGuiInputFlags route_type = ImGuiInputFlags_RouteAlways;
        ImGuiInputFlags flags = route_type | route_options; // Merged flags

      
        render_a_space(1); 
        ImGui::Text("Simulation: ");
        ImGui::SetNextItemShortcut(ImGuiKey_F5, flags); 
        run_simulation = ImGui::Button("RUN",ImVec2(ImGui::GetWindowWidth()*.9f, 25.0f));     
         
        ImGui::PopStyleColor(3);
        ImGui::PopID();

        render_a_space(1);
        ImGui::Text("Conversion calculator"); 
        ConversionCalculator(); 
        ImGui::End();
    }

    void MakeTheMaths() {
        if (run_simulation || opened_file) {
            auto start = std::chrono::high_resolution_clock::now();

            //std::cout << "\nPath: " << current_path; 
            if (Desacceleration == 0.0f) {
                Desacceleration = Acceleration;
            }

            LinearProfile::ProfileConstraint InputConstraints(Velocity, Acceleration, Desacceleration, Jerk, Distance);
            LinearProfile::TrapezoidalMotionProfile TrapezoidalProfile(InputConstraints);
            LinearProfile::SCurveMotionProfile ScurveProfile(InputConstraints);

            pos.clear();
            vel.clear();
            accel.clear();
            time_vector.clear();

            max_velocity = 0.0f;
            max_acceleration = 0.0f;

            if (profiler == 0) {
                Profiler_calculations(TrapezoidalProfile);
                TerminalOutput = "Simulation Completed succesfully using a Trapezoidal Profile.";
            }

            else {
                Profiler_calculations(ScurveProfile);
                TerminalOutput = "Simulation Completed succesfully using a S-Curve Profile.";
            }

            auto end = std::chrono::high_resolution_clock::now();

            //Delay in millis
            std::chrono::duration<float, std::milli> duration_ms = end - start;
            simulation_time = duration_ms.count();

            snprintf(bufferSimulation, 32, " Total time: (msec) %.3f ", simulation_time);
        }
    }

    void RenderCharts() {
        
        ImPlot::SetNextAxisToFit(ImAxis_X1);
        if (ImPlot::BeginPlot("Velocity Chart")) {
            ImPlot::SetupAxes("Time(s)", "Velocity(m/s)");
            ImPlot::SetupAxisLimits(ImAxis_Y1, vel_axis_min, vel_axis_max, ImPlotCond_Always);
            ImPlot::SetNextLineStyle(ImVec4(0.0f, 0.0f, 1.0f, 1.0f), 5.0f);
            ImPlot::PlotLine("Velocity", time_vector.data(), vel.data(), time_vector.size());
            ImPlot::EndPlot();
        }

        ImPlot::SetNextAxisToFit(ImAxis_X1);
        if (ImPlot::BeginPlot("Acceleration Chart")) {
            ImPlot::SetupAxes("Time(s)", "Acceleration(m/s2)");
            ImPlot::SetupAxisLimits(ImAxis_Y1, accel_axis_min, accel_axis_max, ImPlotCond_Always);
            ImPlot::SetNextLineStyle(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), 5.0f);
            ImPlot::PlotLine("Acceleration", time_vector.data(), accel.data(), time_vector.size());
            ImPlot::EndPlot();
        }

    }

    void RenderTable() {

            ImGui::BeginTable("Profile Simulation", 5, ImGuiTableFlags_Resizable);
        
            // Submit columns name with TableSetupColumn() and call TableHeadersRow() to create a row with a header in each column.
            // (Later we will show how TableSetupColumn() has other uses, optional flags, sizing weight etc.)
            ImGui::TableSetupColumn("Time [sec]");
            ImGui::TableSetupColumn("Position");
            ImGui::TableSetupColumn("Velocity");
            ImGui::TableSetupColumn("Acceleration");
            ImGui::TableSetupColumn("Jerk");

            ImGui::TableHeadersRow();
            int index = 0; 
            for (int row = 0; row <vel.size(); row++, index++)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%.4f", time_vector[index]);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.4f", pos[index]);

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%.4f", vel[index]);

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%.4f", accel[index]);

                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%.4f", Jerk);
                   
            }
            ImGui::EndTable();
        

        if (ImGui::Button("Download table")) {
            csv_file_path= ShowSaveFileDialog(CSV);
            Writecsv(csv_file_path);
            terminal_path = csv_file_path; 
        }
    }

    void RenderSimulation() {
        ImGui::Begin("Simulation");

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

        ImGui::BeginTabBar("Plot selector", tab_bar_flags);

        if (ImGui::BeginTabItem("Charts"))
        {  
            RenderCharts();    
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Data"))
        {
            RenderTable();
            ImGui::EndTabItem();
        }


        ImGui::EndTabBar();


        ImGui::End();

    }

    void RenderTerminal() {
        ImGui::Begin("Terminal");

        if (terminal_path == read_file_path) {
            ImGui::Text("File succesfully opened: ");
            ImGui::Text(wstring_to_string(terminal_path).data());
        }

        else if (terminal_path == write_file_path) {
            ImGui::Text("File saved to: "); ImGui::SameLine();
            ImGui::Text(wstring_to_string(terminal_path).data());
        }

        else if (terminal_path == csv_file_path) {
            ImGui::Text("Table exported to: "); ImGui::SameLine();
            ImGui::Text(wstring_to_string(terminal_path).data()); 
        }

        else {
            ImGui::Text("Current workspace: "); ImGui::SameLine();
            ImGui::Text(wstring_to_string(current_path).data());
        }
        
        ImGui::Text(TerminalOutput.data()); ImGui::SameLine();
        ImGui::Text(bufferSimulation);

 
        if (vel.size() > 1) {
            if (run_simulation || opened_file) {
                ArrayOutput = VectorToCommaSeparatedString(vel);
            }
        }

        ImGui::Text("Velocity Array");
        ImGui::Text(ArrayOutput.get());

        
        if (ImGui::Button("Copy Array to clipboard")) {
            ImGui::SetClipboardText(ArrayOutput.get()); 
        }

        ImGui::End(); 
    }

    void RenderUI() {
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        //File options
        
        static bool open_file = false;
        static bool download = false;
        static bool download_as = false; 

        //Help options
        static bool welcome = false;
        static bool wiki = false;
        static bool about = false;

        static std::wstring save_path = L"";

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("MotionLight", nullptr, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }
       

        if (ImGui::BeginMenuBar())
        {
            static ImGuiInputFlags route_options = ImGuiInputFlags_Repeat;
            static ImGuiInputFlags route_type = ImGuiInputFlags_RouteAlways;
            ImGuiInputFlags flags = route_type | route_options; // Merged flags


            if (ImGui::BeginMenu("File")) {

                if (ImGui::MenuItem("Open File", "")){
                    save_path = ShowOpenFileDialog();
                    read_file_path = wstring_to_string(save_path);
                    terminal_path = read_file_path; 
                    ReadMotionFile(read_file_path);
                }

                ImGui::Separator();
                
                if (ImGui::MenuItem("Download", "") ) {
                    save_path = ShowSaveFileDialog(TXT);
                    write_file_path=wstring_to_string(save_path);
                    terminal_path = write_file_path;
                    WriteMotionFile(write_file_path); 
                }

                
                ImGui::EndMenu();
            }


            if (ImGui::BeginMenu("Help")) {

                if (ImGui::MenuItem("Welcome")) {
                    welcome = true; 
                    
                }
                if (ImGui::MenuItem("Wiki")) {
                    wiki = true; 
                }
                if (ImGui::MenuItem("About")) {
                    about = true; 
                }

                ImGui::EndMenu();
            }
    
            ImGui::EndMenuBar();
  
        }

        
        if (welcome) {
            
            welcome = false; // Reseteamos el flag para que no se siga abriendo en cada frame
        }

        
        // Ventana "Wiki"
        if (wiki) {
            ShellExecuteA(0, "open", "https://hectoralonso18.github.io/Lightninglib/docs/intro", 0, 0, SW_SHOW); 
            wiki = false; // Reseteamos el flag
        }        

        // Ventana "About"
        if (about) {
            ImGui::OpenPopup("About");
            about = false; // Reseteamos el flag
        }

        RenderParametersView();
        MakeTheMaths();
        RenderSimulation();
        RenderTerminal(); 
        opened_file = false; 
        ImGui::End(); 

    }
}
