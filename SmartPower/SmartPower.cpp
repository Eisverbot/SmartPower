#include <iostream>
#include <windows.h>
#include <string>
#include <fstream>
#include <map>

// Set console text color
void SetColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// Set power plan
bool SetPowerPlan(const std::string& guid) {
    std::string cmd = "powercfg /s " + guid;
    int result = system(cmd.c_str());
    return result == 0;
}

// Set refresh rate by enumerating available modes
bool SetRefreshRate(int targetHz) {
    DEVMODE cur = {};
    cur.dmSize = sizeof(DEVMODE);
    if (!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &cur)) {
        SetColor(12); // Red
        std::cerr << "Failed to get current display settings.\n";
        SetColor(15); // Reset
        return false;
    }

    DEVMODE mode = {};
    mode.dmSize = sizeof(DEVMODE);
    bool found = false;
    int i = 0;
    while (EnumDisplaySettings(NULL, i, &mode)) {
        if (mode.dmPelsWidth == cur.dmPelsWidth &&
            mode.dmPelsHeight == cur.dmPelsHeight &&
            mode.dmBitsPerPel == cur.dmBitsPerPel &&
            mode.dmDisplayFrequency == targetHz) {
            found = true;
            break;
        }
        i++;
    }

    if (!found) {
        SetColor(12); // Red
        std::cerr << "Desired refresh rate " << targetHz << " Hz not available at current resolution.\n";
        SetColor(15); // Reset
        return false;
    }

    mode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;
    LONG res = ChangeDisplaySettingsEx(NULL, &mode, NULL, CDS_UPDATEREGISTRY | CDS_GLOBAL, NULL);
    if (res != DISP_CHANGE_SUCCESSFUL) {
        SetColor(12); // Red
        std::cerr << "Failed to set refresh rate: " << res << "\n";
        SetColor(15); // Reset
        return false;
    }

    return true;
}

// Create default config file
void CreateDefaultConfig() {
    std::ofstream file("config.txt");
    file << "Full Power Mode Plan \"fba7f244-7756-44d4-96df-26ce0edbcd1e\";\n";
    file << "Full Power Mode Hertz \"240\";\n\n";
    file << "Energy Saver Mode Plan \"381b4222-f694-41f0-9685-ff5bb260df2e\";\n";
    file << "Energy Saver Mode Hertz \"100\";\n";
    file.close();
    std::cout << "Default config.txt created. You can edit this file to change settings.\n\n";
}

// Load configuration
bool LoadConfig(std::map<std::string, std::string>& config) {
    std::ifstream file("config.txt");
    if (!file.is_open()) {
        SetColor(12); // Red
        std::cerr << "Error reading config.txt. Exiting.\n";
        SetColor(15); // Reset
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        if (line.back() == ';') line.pop_back();

        size_t firstQuote = line.find('"');
        if (firstQuote == std::string::npos) continue;
        size_t secondQuote = line.find('"', firstQuote + 1);
        if (secondQuote == std::string::npos) continue;

        std::string key = line.substr(0, firstQuote - 1);
        std::string value = line.substr(firstQuote + 1, secondQuote - firstQuote - 1);

        config[key] = value;
    }

    file.close();
    return true;
}

int main() {
    SetConsoleTitleA("Performance Mode Switcher");

    // Create default config if it doesn't exist
    std::ifstream test("config.txt");
    if (!test.good()) {
        CreateDefaultConfig();
    }
    test.close();

    // Load config
    std::map<std::string, std::string> config;
    if (!LoadConfig(config)) return 1;

    // Display menu
    SetColor(15);

    std::cout << R"(
     ______                         ______                        
    / _____)                    _  (_____ \                       
   ( (____  ____  _____  ____ _| |_ _____) )__  _ _ _ _____  ____ 
    \____ \|    \(____ |/ ___|_   _)  ____/ _ \| | | | ___ |/ ___)
    _____) ) | | / ___ | |     | |_| |   | |_| | | | | ____| |    
   (______/|_|_|_\_____|_|      \__)_|    \___/ \___/|_____)_|    
                                                                       
)";
    std::cout << "\n";

    SetColor(15); std::cout << " [1] ";
    SetColor(12); std::cout << "> Activate Full Power Mode (" << config["Full Power Mode Hertz"] << " Hz, high power)\n";

    SetColor(15); std::cout << " [2] ";
    SetColor(3); std::cout << "> Activate Energy Saver Mode (" << config["Energy Saver Mode Hertz"] << " Hz, low power)\n\n";

    SetColor(15); std::cout << " Choose mode (1 or 2): ";
    int choice;
    std::cin >> choice;
    system("cls");

    if (choice == 1) {
        std::cout << "\n";
        // Activating Full Power Mode
        SetColor(10); std::cout << "  Activating ";
        SetColor(12); std::cout << "Full Power Mode";
        SetColor(10); std::cout << " ...\n";

        std::cout << "\n";

        if (SetPowerPlan(config["Full Power Mode Plan"])) {
            SetColor(12); std::cout << "  Full Power Plan";
            SetColor(10); std::cout << " successfully activated!\n";
            SetColor(15);
        }
        else {
            SetColor(12); std::cout << "Error: Failed to change power plan.\n";
            SetColor(15);
        }

        int hz = std::stoi(config["Full Power Mode Hertz"]);
        if (SetRefreshRate(hz)) {
            SetColor(10); std::cout << "  Refresh rate set to " << hz << " Hz successfully.\n";
            SetColor(15);
        }
        else {
            SetColor(12); std::cout << "Warning: Could not set refresh rate to " << hz << " Hz.\n";
            SetColor(15);
        }

    }
    else if (choice == 2) {
        std::cout << "\n";
        // Activating Energy Saver Mode
        SetColor(10); std::cout << "  Activating ";
        SetColor(3); std::cout << "Energy Saver Mode";
        SetColor(10); std::cout << " ...\n";

        std::cout << "\n";

        if (SetPowerPlan(config["Energy Saver Mode Plan"])) {
            SetColor(3); std::cout << "  Energy Saver Plan";
            SetColor(10); std::cout << " successfully activated!\n";
            SetColor(15);
        }
        else {
            SetColor(12); std::cout << "Error: Failed to change power plan.\n";
            SetColor(15);
        }

        int hz = std::stoi(config["Energy Saver Mode Hertz"]);
        if (SetRefreshRate(hz)) {
            SetColor(10); std::cout << "  Refresh rate set to " << hz << " Hz successfully.\n";
            SetColor(15);
        }
        else {
            SetColor(12); std::cout << "Warning: Could not set refresh rate to " << hz << " Hz.\n";
            SetColor(15);
        }

    }
    else {
        SetColor(12); std::cout << "Invalid input. Please choose 1 or 2.\n";
        SetColor(15);
    }

    SetColor(15);
    std::cout << "\n  > Press Enter to exit...";
    std::cin.ignore();
    std::cin.get();
    return 0;
}
