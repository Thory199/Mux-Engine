\# 🧱 Estructura Interna del Motor



\## 📦 Carpetas principales



\- \*\*src/\*\*: Contiene el código fuente principal del motor

\- \*\*include/\*\*: Contiene headers organizados por módulos

\- \*\*external/\*\*: Librerías externas integradas

\- \*\*lib/\*\*: Librerías ya compiladas (.lib, .dll)

\- \*\*editor/\*\*: Código del editor visual



\## 📁 Módulos en desarrollo



\### 1. Core/

\- `System.cpp / .h`: Inicio y cierre del motor, bucle principal.

\- `Timer.cpp`: Delta time, tiempo de frames.



\### 2. Graphics/

\- `Renderer.cpp / .h`: OpenGL básico, dibujo de texturas.

\- `Shader.cpp`: Compilación y uso de shaders.



\### 3. Input/

\- `InputManager.cpp`: Teclado y ratón.

\- Pensado para extender a mandos y eventos personalizados.



\### 4. UI/

\- Usa ImGui. Interfaz visual para pruebas y editor.



\## 📂 Archivos importantes



\- `MuxEngine.h`: Punto de entrada al motor.

\- `main.cpp`: Ejemplo de uso o test principal.

\- `Editor.cpp`: Control del editor visual.



---



\## 🔁 Reglas internas



\- Separar siempre `.h` (en include/) de `.cpp` (en src/)

\- No mezclar ImGui con lógica del motor si se puede evitar

\- Mantener cada módulo en su propio namespace en el futuro (`Mux::Graphics`, etc.)

