# OpenGL 3D Scene

This project creates a 3D representation of a 2D image using the OpenGL API. The image includes a laptop, a book, and a piece of paper sitting on a granite countertop. The screencast below demonstrates navigating the 3D. The usage section discusses scene navigation in more depth. 

<h2>Installation</h2>
</br>
Install the Visual Studio version that matches your operating system from the [download page](https://visualstudio.microsoft.com/downloads/).
</br>
Select the C++ plug-ins at installation.
</br>
Clone or fork the project and unzip the OpenGL zip folder containing the project's dependencies. This zip folder should include subfolders for GLAD, GLEW, GLFW, and glm. My university provided the library folders to guide the completion of this project. 
</br>

<h2>Getting Started</h2>
Open the project in visual studio by launching the .sln file. 
</br>
Follow the instructions below to link the dependencies with the project. 
</br>
1. Right click on the top level of the project in visual studio and select properties. 
   ![](/Images/setup/setupImage1.png)
2. Select VC++ directories in the window that pops up.
   ![](/Images/setup/setupImage2.png)
3. Select include directories from the option given.
   ![](/Images/setup/setupImage3.png)
4. Click on the folder icon, then the three dots to add the paths to the OpenGL directories.
   ![](/Images/setup/setupImage4.png)
5. Add paths to the GLAD and gml folders at the top level as demonstrated below.
   ![](/Images/setup/setupImage5.png)
6. Add paths to the include folders within the GLFW and GLEW directories.
   ![](/Images/setup/setupImage6.png)
7. Click ok
8. Next click the library directories tab within VC++ directories and expand the arrow next to library Path.
   ![](/Images/setup/setupImage7.png)
9. Next, add the path to the Win32 folder in GLEW > Lib > Release.
   ![](/Images/setup/setupImage8.png)
10. Next, add the path to the visual studio version in the GLFW folder.
    ![](/Images/setup/setupImage9.png)
11. Finally, go to linker > input > and expand the arrow on additional dependencies.
    ![](/Images/setup/setupImage10.png)
12. Type the four dependencies given below.
    ![](/Images/setup/setupImage11.png)
13. Click apply and close.
14. The project should now be set up without errors. 
</br>

<h2> Usage </h2>




