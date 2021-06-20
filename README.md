# OpenGL 3D Scene

This project creates a 3D representation of a 2D image using the OpenGL API. The image includes a laptop, a book, and a piece of paper sitting on a granite countertop. The screencast below demonstrates navigating the 3D. The usage section discusses scene navigation in more depth.
</br>
</br>
<img src="README_Images/3D_Scene_Screencast.gif" width="800" height="600" />

<h2>Installation</h2>
</br>
1. Install the Visual Studio version that matches your operating system from the <a href="https://visualstudio.microsoft.com/downloads/">download page</a>.
</br>
2. Select the C++ plug-ins at installation.
</br>
3. Clone or fork the project and unzip the OpenGL zip folder containing the project's dependencies. This zip folder should include subfolders for GLAD, GLEW, GLFW, and glm. My university provided the library folders to guide the completion of this project. 
</br>

<h2>Getting Started</h2>
Open the project in visual studio by launching the .sln file. 
</br>
</br>
Follow the instructions below to link the dependencies with the project. 
</br>
</br>
1. Right click on the top level of the project in visual studio and select properties. 
   </br>
   <img src="/README_Images/setup/setupImage1.png" alt="Step 1" width="500" height="400">
   </br>
2. Select VC++ directories in the window that pops up.
   </br>
   <img src="/README_Images/setup/setupImage2.png" alt="Step 2" width="500" height="400">
   </br>
3. Select include directories from the option given.
   </br>
   <img src="/README_Images/setup/setupImage3.png" alt="Step 3" width="500" height="400">
   </br>
4. Click on the folder icon, then the three dots to add the paths to the OpenGL directories.
   </br>
   <img src="/README_Images/setup/setupImage4.png" alt="Step 4" width="500" height="200">
   </br>
5. Add paths to the GLAD and gml folders at the top level as demonstrated below.
   </br>
   <img src="/README_Images/setup/setupImage5.png" alt="Step 5" width="500" height="200">
   </br>
6. Add paths to the include folders within the GLFW and GLEW directories.
   </br>
   <img src="/README_Images/setup/setupImage6.png" alt="Step 6" width="500" height="200">
   </br>
7. Click ok
8. Next click the library directories tab within VC++ directories and expand the arrow next to library Path.
   </br>
   <img src="/README_Images/setup/setupImage7.png" alt="Step 7" width="500" height="200">
   </br>
9. Next, add the path to the Win32 folder in GLEW > Lib > Release.
   </br>
   <img src="/README_Images/setup/setupImage8.png" alt="Step 8" width="500" height="200">
   </br>
10. Next, add the path to the visual studio version in the GLFW folder.
   </br>
   <img src="/README_Images/setup/setupImage9.png" alt="Step 9" width="500" height="400">
   </br>
11. Finally, go to linker > input > and expand the arrow on additional dependencies.
   </br>
   <img src="/README_Images/setup/setupImage10.png" alt="Step 10" width="500" height="400">
   </br>
12. Type the four dependencies given below.
   </br>
   <img src="/README_Images/setup/setupImage11.png" alt="Step 11" width="500" height="200">
   </br>
13. Click apply and close.
    </br>
15. The project should now be set up without errors. 
    </br>

<h2> Usage </h2>
#FIXME
</br>
Add usage




