#include "main.h"

const GLFWvidmode* monitorVideoMode = nullptr;
GLFWwindow* mainWindow = nullptr;

void CreateDocumentFolder() {
    WCHAR documentsPath[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_PERSONAL, NULL, 0, documentsPath);
    std::wstring ramboGameDir = std::wstring(documentsPath) + L"/Rambo_Game";

    int ret = CreateDirectoryW(ramboGameDir.c_str(), NULL);
    if (ret != 0 && ret != EEXIST) {
        MessageBoxA(NULL,
                    "Couldn't create a folder (Rambo_Game) inside your Documents "
                    "...\nTry creating it manually.",
                    "Fatal error", MB_OK);
        ExitProcess(0);
    }
}

void InitializeWindowAndGraphics() {
    if (!glfwInit()) {
        const char* errorDesc = nullptr;
        glfwGetError(&errorDesc);
        ABORT_EXIT("This game cannot boot without a OpenGL compatible graphic "
                   "card. (GLFW)\n\n%s\n",
                   errorDesc ? errorDesc : "No error description available");
    }

    monitorVideoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    mainWindow = glfwCreateWindow(monitorVideoMode->width, monitorVideoMode->height, "3D Project", nullptr, nullptr);
    glfwMakeContextCurrent(mainWindow);

    glfwSetInputMode(mainWindow, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
    glfwSetInputMode(mainWindow, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    GLenum res = glewInit();
    if (res != GLEW_OK) {
        ABORT_EXIT("This game cannot boot without a OpenGL compatible graphic "
                   "card. (GLEW)\n\n%s\n",
                   glewGetErrorString(res));
    }

    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(mainWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    glClearColor(0.0f, 0.0f, 0.4f, 1.0f);
    glfwSwapInterval(0);

    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_FRAMEBUFFER_SRGB);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

physx::PxFoundation* gFoundation = nullptr;
physx::PxPhysics* gPhysics = nullptr;
physx::PxPvd* gPvd = nullptr;
physx::PxCooking* gCooking = nullptr;
physx::PxCpuDispatcher* gDispatcher = nullptr;

static physx::PxDefaultErrorCallback gDefaultErrorCallback;
static physx::PxDefaultAllocator gDefaultAllocatorCallback;

void InitializePhysX() {

    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
    if (!gFoundation) {
        ABORT_EXIT("Error in PxCreateFoundation()\n");
    }

    gPvd = physx::PxCreatePvd(*gFoundation);
    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
    gPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(), true, gPvd);
    if (!gPhysics) {
        ABORT_EXIT("Error in PxCreatePhysics()\n");
    }

    gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, physx::PxCookingParams(physx::PxTolerancesScale()));
    if (!gCooking) {
        ABORT_EXIT("Error in PxCreateCooking()\n");
    }

    gDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
}

int main(int argc, char** argv) {

    CreateDocumentFolder();
    InitializeWindowAndGraphics();
    InitializePhysX();

#ifdef _DEBUG

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(mainWindow, true);
    ImGui_ImplOpenGL3_Init("#version 330");

#endif

    Level_Test1 level(mainWindow, (GLFWvidmode*)monitorVideoMode);

    double lastFrameTime = (float)glfwGetTime();
    double deltaTime = 1.0f / 60.0f;

    int frameCount = 0;
    double lastFrameCountTime = lastFrameTime;

    while ((glfwWindowShouldClose(mainWindow) == 0)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrameTime;
        lastFrameTime = currentFrame;

        if (glfwGetTime() - lastFrameCountTime >= 1) {
            lastFrameCountTime = glfwGetTime();
            char buffer[256];
            snprintf(buffer, sizeof(buffer), "3D Project - %d FPS", frameCount);
            SetWindowTextA(glfwGetWin32Window(mainWindow), buffer);
            frameCount = 0;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef _DEBUG
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
#endif

        level.updateAndRender((float)deltaTime);
#ifdef _DEBUG
        level.updateImGui((float)deltaTime);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

        glfwSwapBuffers(mainWindow);
        glfwPollEvents();

        frameCount++;
    }

    /* Exit */
    gPhysics->release();
    gPvd->release();
    gCooking->release();
    gFoundation->release();

#ifdef _DEBUG
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
#endif

    glfwTerminate();
    return 0;
}