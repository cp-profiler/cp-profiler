# CP-Profiler Installer - Instructions and items to deploy Cp Profiler installer

This README file help you to generate an installer for cp profiler depending on the operating system you have

## Table of Contents

- [Dependencies](#dependencies)
- [Items](#items)
- [Installer for MAC OS](#installer-for-mac-os)
- [Installer for Linux](#installer-for-linux)
- [Installer for Windows](#installer-for-windows)

### Dependencies

- Qt5.x


### Items
- Cp profiler source files: You can clone the files from here or [Cp-Profiler Repository](https://github.com/cp-profiler/cp-profiler.git) 
- Enter the following commands from the root of the folder to get all the source files:
```
    git submodule update --init
    mkdir build && cd build
    qmake .. && make
```

### Installer For Mac OS
   **Set up the environment**
- Operating System for MAC OS
- A setup of Qt Creator IDE (for macOS, version 5.x)
- The QT code of the Cp-Profiler application

 **Step 1 :**
   
 Add the `app_bundle` specification in the project .pro file.
 
 QMake should automatically generate a bundle for the application in the build folder. Nevertheless, it is recommended to explicitly add this in the .pro file. 
 <p align="center">
  <img width="300" height="200" src="https://github.com/iamklaus01/cp-profiler_installer/blob/assets/docs/images/installer_screenshots/1.png">
</p>

 **Step 2 :**
- Clean up the project from previous versions. 
    
   <p align="center">
    <img width="300" height="300" src="https://github.com/iamklaus01/cp-profiler_installer/blob/assets/docs/images/installer_screenshots/2.png">
   </p>
    
- Cleaning the project from previous versions is a good practice to avoid errors that could be related to other versions you have already made.

 **Step 3:**
  
 Run the project using the "Release" option

  **Step 4 :** Deployment
  
- Locate the 'macdeployqt' binary in your Qt installation
To do this, go to the Qt folder and follow this path: `Qt>> 5.x.x>> clang_64>> bin>>`

Note the path to the  'macdeployqt' :


   **Step 4.1 :**
- Go to the build directory of the project.
- This directory is generated after building the Qt code (you did it in step 3)
- Run "macdeployqt" on the app in the build directory with the following command (the first part being the path to macdeployqt:

    `Qt>> 5.x.x>> clang_64>> bin>>macdeployqt  nomProjet.app`
- In order to have a .dmg file for easy distribution, just add the '-dmg' parameter to the command.
- The dmg file appears in the same directory as the .app file
- Check the contents of the .app bundle
- Run the executable file: The path to the executable is the following one: `bundle.app>> Contents>> MACOS>> executable_file`
- Test the .dmg disk image


    
### Installer For Linux

  **Set up environment**
- Install  [CQtDeployer Linux Offline Installer](https://github.com/QuasarApp/CQtDeployer/releases/download/v1.5.4.14/CQtDeployer_1.5.4.14_OfflineInstaller_Linux_x86_64.run)
- A setup of Qt Creator IDE (for Linux, version 5.x)
- The QT code of the Cp-Profiler application

**Step 1 :**
   
 Open the project in the QT IDE and compile it for the first time: This will generate a build folder at the root of the project.
    
    
**Step 2 :**
  
  
Run the following command:


(Make sure the command has been added to the path or that you are in the command folder.)


`cqtdeployer -bin myApp -qmake {path to qmake} -qmlDir ./`


**Where:**

- `cqtdeployer` is the utility
- `myApp` is the path to the executable file of the application itself

You will get a Distribution Kit folder in the debug folder of the build directory. The `.run` file is there.


For some Linux distributions additional packages are required. To automate the downloading of these packages, an .sh file can be written that launches on installation to automatically download additional packages.


**.sh file for LInux Bundle**

```
    #!/bin/sh
    echo "\n---------cp profiler installer---------"
    echo "\n Install Dependencies ..."
    sudo apt-get update -y
    echo " [libxcb-xinerama0]\n"
    sudo apt-get install libxcb-xinerama0 -y
    echo "---------------------------------------"  
    echo "Install cp-profiler\n"
    chmod +x $PWD/run/Installercp-profiler.run
    exec $PWD/run/Installercp-profiler.run
    echo "---------------------------------------"  
    echo "Installation done.\n"
```


### Installer For Windows

  **Set up environment**
- Install  [CQtDeployer Linux Offline Installer](https://github.com/QuasarApp/CQtDeployer/releases/download/v1.5.4.14/CQtDeployer_1.5.4.14_OfflineInstaller_Win_x86_64.exe)
- A setup of Qt Creator IDE (for Windows, version 5.x)
- The QT code of the Cp-Profiler application

**Step 1 :**
   
 Open the project in the QT IDE and compile it for the first time: This will generate a build folder at the root of the project.
    
    
**Step 2 :**
  
  
Run the following command:


(Make sure the command has been added to the path or that you are in the command folder.)


`cqtdeployer -bin myApp.exe -qmake {path to qmake} -qmlDir ./`


**Where:**

- `cqtdeployer` is the utility
- `myApp.exe` is the path to the executable file of the application itself

You will get a Distribution Kit folder in the debug folder of the build directory. The `.exe` file is there.

  **Step 3** : Go further: Generate the installer for Windows
  
- Download [Inno Setup](https://jrsoftware.org/download.php/is.exe)
- Use the following guidelines to generate the installer
    - Launch Inno Setup and select the second option as in the screenshot below
    
    <p align="center">
        <img width="300" height="200" src="https://github.com/iamklaus01/cp-profiler_installer/blob/assets/docs/images/installer_screenshots/3.png">
   </p>
   
    - Click on next then fill in the different fields with the corresponding values
   <p align="center">
    <img width="300" height="200" src="https://github.com/iamklaus01/cp-profiler_installer/blob/assets/docs/images/installer_screenshots/4.png">
   </p>
   
    - Click Next and choose a destination folder for the application, then the name of the application
      
   <p align="center">
    <img width="300" height="200" src="https://github.com/iamklaus01/cp-profiler_installer/blob/assets/docs/images/installer_screenshots/5.png">
   </p>
   
    - Click on Next then carry on.
      
  <p align="center">
    <img width="300" height="200" src="https://github.com/iamklaus01/cp-profiler_installer/blob/assets/docs/images/installer_screenshots/6.png">
  </p>
   
   At this step (see screenshot above), enter the path to the generated executable in the Distribution Kit folder.
   
   Then, add all the other files and folders present in this folder by clicking on Add file(s) or Add folder respectively.
    - Click on Next and fill in the fields that appear to you. They are quite intuitive.
    - It is advisable to leave the boxes checked by default
    
    
   Your installer is then generated.
   
   <p align="center">
    Readme edited by EDX MOOC project - 18/05/2022
  </p>
