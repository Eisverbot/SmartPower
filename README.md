# SmartPower

A compact C++ console tool for Windows to quickly toggle between two fully customizable system profiles. Instantly change your active power plan and your monitor's refresh rate with a single command.

## 🎯 Purpose and Utility

On modern PCs, especially laptops, there is often a conflict between maximum performance (e.g., for gaming or video editing) and power efficiency (e.g., for battery life, office work, or quiet operation).

Switching these settings manually is tedious:

1.  **Power Plan:** Control Panel -\> Power Options -\> Select a plan.
2.  **Refresh Rate:** Settings -\> System -\> Display -\> Advanced display -\> Select refresh rate.

This tool **automates** this process. It bundles a specific power plan (e.g., "High performance") with a high refresh rate (e.g., 240 Hz) and a saver profile (e.g., "Power saver") with a low refresh rate (e.g., 100 Hz or 60 Hz).

**Ideal for:**

  * **Laptop Users:** Quickly switching between "Gaming Mode" (plugged in) and "Battery Mode" (on the go).
  * **Desktop Users:** Toggling between "Full Performance" and a quiet "Work/Night Mode."

## ✨ Features

  * **Dual-Profile System:** Toggles between a "Full Power" and an "Energy Saver" mode.
  * **Fully Configurable:** All values (Power Plan GUIDs, target Hz) are read from a `config.txt`.
  * **Smart Refresh Rate Validation:** The tool checks if the desired refresh rate is actually available at the current resolution before attempting to apply it.
  * **Auto-Configuration:** Automatically creates a default `config.txt` on first launch if one isn't found.
  * **Lightweight:** A single, native C++ executable with no external dependencies.

## ⚙️ How it Works (Technical Deep Dive)

The program uses native Windows APIs and standard C++ libraries to perform its tasks.

### 1\. Configuration Management (`<fstream>`, `<map>`)

1.  **On Startup:** The program checks for the existence of `config.txt` in its directory.
2.  **Creation (`CreateDefaultConfig`):** If the file is missing, it's created with default placeholder values.
3.  **Loading (`LoadConfig`):** The `config.txt` is read line by line. A simple parser splits the key (e.g., `"Full Power Mode Plan"`) from the value (e.g., `"fba7f2...bcd1e"`) and stores everything in a `std::map<string, string>`.

### 2\. Power Plan Switching (`SetPowerPlan`)

  * This function is a simple wrapper for the Windows command-line utility `powercfg.exe`.
  * It constructs a command string, e.g., `powercfg /s 381b4222-f694-41f0-9685-ff5bb260df2e`.
  * The `system()` call executes this command, changing the active power plan.
  * A return value of `0` signals success.

### 3\. Refresh Rate Switching (`SetRefreshRate`)

This is the most complex part of the program, utilizing the Windows Display API (`<windows.h>`):

1.  **Get Current Mode:** First, `EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, ...)` is called. This fills a `DEVMODE` struct with the *current* settings (resolution, color depth, etc.).
2.  **Enumerate Available Modes:** The program enters a `while` loop, calling `EnumDisplaySettings(NULL, i, ...)` and incrementing `i` from `0`. This lists **all** modes supported by the display driver and monitor.
3.  **Validation:** Inside the loop, each found mode is checked:
      * Does its resolution (`dmPelsWidth`, `dmPelsHeight`) match the current mode?
      * Does its color depth (`dmBitsPerPel`) match the current mode?
      * Does its frequency (`dmDisplayFrequency`) match the `targetHz` from the `config.txt`?
4.  **Apply Mode:** If a mode matches all criteria (`found = true`), `ChangeDisplaySettingsEx(NULL, &mode, ...)` is called to apply the change.
      * `CDS_UPDATEREGISTRY` makes the change persistent.
      * `CDS_GLOBAL` applies the change to all users.
5.  **Error Handling:** If the loop finishes without finding a match (`found = false`), a user-friendly error is printed, and no change is attempted. This safely prevents the tool from trying to set an unsupported mode.

## 🚀 Getting Started & Configuration

**You must edit the `config.txt` to match your system for this tool to work\!**

1.  Run the `.exe` once. It will create the `config.txt` file.
2.  Open `config.txt` with any text editor.

### a) Finding your Power Plan GUIDs

Every power plan on your system has a unique GUID.

1.  Open **Command Prompt (cmd)** or **PowerShell**.

2.  Type the command `powercfg /list` and press Enter.

3.  You will see an output similar to this:

    ```
    Existing Power Schemes (* Active)
    -----------------------------------
    Power Scheme GUID: 381b4222-f694-41f0-9685-ff5bb260df2e  (Power saver)
    Power Scheme GUID: 8c5e7fda-e8bf-4a96-9a85-a6e23a8c635c  (Balanced) *
    Power Scheme GUID: fba7f244-7756-44d4-96df-26ce0edbcd1e  (My Custom Plan)
    ```

4.  Copy the GUIDs for the two plans you want to use (e.ias, "Power saver" and "My Custom Plan").

### b) Finding your Refresh Rates (Hz)

Check which refresh rates your monitor supports:

  * Go to **Settings \> System \> Display \> Advanced display**.
  * Look at the dropdown list under "Choose a refresh rate".

### c) Editing the `config.txt`

Open the `config.txt` and replace the placeholder values with your own.

**Example `config.txt`:**

```ini
Full Power Mode Plan "fba7f244-7756-44d4-96df-26ce0edbcd1e";
Full Power Mode Hertz "240";

Energy Saver Mode Plan "381b4222-f694-41f0-9685-ff5bb260df2e";
Energy Saver Mode Hertz "100";
```

  * `Full Power Mode Plan`: Paste your high-performance GUID here.
  * `Full Power Mode Hertz`: Set your desired high refresh rate.
  * `Energy Saver Mode Plan`: Paste your power-saving GUID here.
  * `Energy Saver Mode Hertz`: Set your desired low refresh rate.

Save the file and run the program again. Your custom profiles are now ready to use\!