#include <nanogui/nanogui.h>
#include <thread>
#include <chrono>
#include <atomic>
#include <iostream>
#include <shlobj.h>
#include <windows.h>
#include <locale>
#include <codecvt>
// Function to simulate file scanning
void scan_file(nanogui::Screen* screen, const std::string& filePath) {
    // Simulate file scanning by waiting for 10 seconds
    std::this_thread::sleep_for(std::chrono::seconds(10));
    // Display the scanned file path in the window
    screen->add<nanogui::Label>("Scanned file: " + filePath);
    // Refresh layout to ensure the new label is visible
    screen->performLayout();
}

// Function to simulate folder scanning
void scan_folder(nanogui::Screen* screen, const std::string& folderPath) {
    // Simulate folder scanning by waiting for 10 seconds
    std::this_thread::sleep_for(std::chrono::seconds(10));
    // Display the scanned folder path in the window
    screen->add<nanogui::Label>("Scanned folder: " + folderPath);
    // Refresh layout to ensure the new label is visible
    screen->performLayout();
}
std::string getFolderPath() {
    std::string selectedFolderPath;

    // Initialize COM
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    // Display the folder picker dialog
    BROWSEINFO browseInfo = { 0 };
    TCHAR selectedPath[MAX_PATH];
    browseInfo.hwndOwner = NULL; // Use the desktop window as the owner
    browseInfo.pidlRoot = NULL;  // Start from the desktop
    browseInfo.pszDisplayName = selectedPath;
    browseInfo.lpszTitle = TEXT("Select a folder");
    browseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    LPITEMIDLIST pidlSelected = SHBrowseForFolder(&browseInfo);
    if (pidlSelected != NULL) {
        SHGetPathFromIDList(pidlSelected, selectedPath);

        // Convert TCHAR array to std::string
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        selectedFolderPath = converter.to_bytes(selectedPath);

        // Free the PIDL
        IMalloc* pMalloc;
        if (SUCCEEDED(SHGetMalloc(&pMalloc))) {
            pMalloc->Free(pidlSelected);
            pMalloc->Release();
        }
    }

    // Uninitialize COM
    CoUninitialize();

    return selectedFolderPath;
}
int main() {
    // Initialize NanoGUI
    nanogui::init();

    // Create a NanoGUI screen
    nanogui::Screen screen(nanogui::Vector2i(400, 300), "Antivirus");

    // Create a layout for the buttons
    nanogui::Widget* buttonWidget = new nanogui::Widget(&screen);
    nanogui::BoxLayout* buttonLayout = new nanogui::BoxLayout(nanogui::Orientation::Vertical,
        nanogui::Alignment::Middle,
        5, 5);
    buttonWidget->setLayout(buttonLayout);

    // Create a button for scanning a file
    nanogui::Button* scanFileButton = new nanogui::Button(buttonWidget, "Scan File");
    scanFileButton->setFixedWidth(150);
    scanFileButton->setCallback([&] {
        // Open file dialog to select a file
        std::string selectedFile = nanogui::file_dialog(
            { {"*", "All Files"} },
            false
        );
        if (!selectedFile.empty()) {
            // Call scan_file function in a separate thread
            std::thread(scan_file, &screen, selectedFile).detach();
        }
        });

    // Create a button for scanning a folder
    nanogui::Button* scanFolderButton = new nanogui::Button(buttonWidget, "Scan Folder");
    scanFolderButton->setFixedWidth(150);
    scanFolderButton->setCallback([&] {
        // Open file dialog to select a folder
        std::string selectedFolder = getFolderPath();
        if (!selectedFolder.empty()) {
            // Call scan_folder function in a separate thread
            std::thread(scan_folder, &screen, selectedFolder).detach();
        }
        });

    // Set the layout for the main screen
    nanogui::BoxLayout* layout = new nanogui::BoxLayout(nanogui::Orientation::Horizontal,
        nanogui::Alignment::Middle,
        10, 10);
    screen.setLayout(layout);

    // Draw NanoGUI
    screen.performLayout();
    screen.setVisible(true);

    // Main event loop
    nanogui::mainloop();

    // Shutdown NanoGUI
    nanogui::shutdown();

    return 0;
}
