# GUI-TD

## Configuration in Visual Studio
### Visual Studio
I'm using VS2019. You have to install "C++ desktop development" and "Buildtools for .NET desktop applications". In "C++ desktop development", make sure you also install "C++ moduls for BuildTools v142 (x64/x86 - Experimental)" and "Take in charge of C++/CLI for Build Tools v142". My translation might not be exact.
### Libraries
Library needed:
  - glad
  - glfw
  - opencv
  - glm
 
For each, put the path to the library and include directories. See on internet how to install each in visual studio.
 
For linkers, you need:
  - glfw3.lib
  - glfw3dll.lib
  - User32.lib
  - Gdi32.lib
  - Shell32.lib
  - opencv_world412d.lib
  - opengl32.lib
  - opencv_world412.lib
  
#### Glad
Copy GLAD/src/glad.c in the source files of the project folder. Select it, right-click and properties. In property, C/C++ and modify the parameter "Prise en charge du Common Language Runtime" (in english: something like "Take in charge of the Common Language Runtime") with "Pas de prise en charge du CLR" (english: "No charge of CLR").
#### OpenCV
Do not forget to add the path in your environment variables.
 
## Modification of the code
Do it in new branch then push.
