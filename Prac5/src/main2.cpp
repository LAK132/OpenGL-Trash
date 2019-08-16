#include "main.h"
#define GLAK_IMPLEMENTATION
#include <glak.hpp>

///
/// consoleIO()
/// Console IO thread
///

void consoleIO(userData_t* ud)
{
    unordered_map<string, void(*)(userData_t*)> funcs;
    funcs["exit"] = [](userData_t* ud)
    { 
        exit(EXIT_SUCCESS); 
    };
    funcs["update"] = [](userData_t* ud) 
    { 
        ud->scene.obj->update(); 
        ud->image->update();
        cout << "Buffer Updated" << endl;
    };
    funcs["assert"] = [](userData_t*)
    {
        assert(false);
    };
	funcs["break"] = [](userData_t*)
	{
		cout << "breakpoint" << endl;
	};
    while(true)
    {
        // Check input
        string str;
        cin >> str;
        auto fn = funcs.find(str);

        ud->consoleInputReady.lock();
        ud->consoleInput.lock();
        ud->consoleInputReady.unlock();
        // Execute on input

        if(fn != funcs.end() && fn->second != NULL)
        {
            fn->second(ud);
        }

        ud->consoleInput.unlock();
    }
}

///
/// loop()
/// Called every loop
///
void update(glakLoopData* ld)
{
    userData_t* ud = (userData_t*)ld->userData;

    ud->consoleInput.unlock();
    ud->consoleInputReady.lock();
    ud->consoleInput.lock();
    ud->consoleInputReady.unlock();

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT: {
                ld->run = false;
            } break;
            case SDL_WINDOWEVENT: {
                if(event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    ud->screenw = event.window.data1;
                    ud->screenh = event.window.data2;
                    glViewport(0, 0, ud->screenw, ud->screenh);
                    ud->scene.camera->projection = glm::perspective(90.0f * glm::pi<float>() / 180.0f, (float)ud->screenw / (float)ud->screenh, 0.01f, 100.0f);
                    ud->scene.obj->mesh[0].shader->setUniform("projection", &(ud->scene.camera->projection[0][0]));
                }
            } break;
            case SDL_KEYDOWN: {
                switch(event.key.keysym.scancode)
                {
                    case SDL_SCANCODE_LEFT: {
                        ud->scene.cameraBoom->transform.rotVel.y = 1;
                    } break;
                    case SDL_SCANCODE_UP: {
                        ud->scene.cameraBoom->transform.rotVel.x = 1;
                    } break;
                    case SDL_SCANCODE_RIGHT: {
                        ud->scene.cameraBoom->transform.rotVel.y = -1;
                    } break;
                    case SDL_SCANCODE_DOWN: {
                        ud->scene.cameraBoom->transform.rotVel.x = -1;
                    } break;
                }
            } break;
            case SDL_KEYUP: {
                switch(event.key.keysym.scancode)
                {
                    case SDL_SCANCODE_LEFT:
                    case SDL_SCANCODE_RIGHT:
                        ud->scene.cameraBoom->transform.rotVel.y = 0;
                        break;
                    case SDL_SCANCODE_UP:
                    case SDL_SCANCODE_DOWN:
                        ud->scene.cameraBoom->transform.rotVel.x = 0;
                        break;
                }
            } break;
        }
        ImGui_ImplSdlGL3_ProcessEvent(&event);
    }
    
    ud->scene.cameraBoom->transform.update(ld->updateDelta);
    ud->scene.camera->transform.make();
    ud->shader->setUniform("view", &ud->scene.camera->getTransform());

    ImGui_ImplSdlGL3_NewFrame(ld->window);

    // ud->memEdit.DrawContents(&(ud->bitmap->pixels[0].r), ud->bitmap->pixels.size() * 4);

    ImGui::Text("%f", ld->updateDelta * 1000.0);
    ImGui::Text("%f", ld->drawDelta * 1000.0);
    // ImGui::Image((ImTextureID)ud->image->tex, ImVec2(ud->image->w, ud->image->h));
    
    ImGui::Render();
}

///
/// draw()
/// Called every loop
///
void draw(glakLoopData* ld)
{
    userData_t* ud = (userData_t*)ld->userData;
    glViewport(0, 0, (int)ud->io->DisplaySize.x, (int)ud->io->DisplaySize.y);
    glClearColor(ud->clearCol[0], ud->clearCol[1], ud->clearCol[2], ud->clearCol[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw our object
    ud->scene.obj->draw();

    ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
}

void MessageCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam )
{
    DEBUG << std::ios::hex << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "") 
        << " type = 0x" << type 
        << ", severity = 0x" << severity
        << ", message = " << endl << message << endl << endl;
}

///
/// init()
/// This will only be run once (when the application starts)
///
void init(glakLoopData* ld)
{
    userData_t* ud = new userData_t();
    // Setup SDL
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        throw exception();
    }

    ud->screenw = 1280;
    ud->screenh = 720;

    // Setup Window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    ld->window = SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ud->screenw, ud->screenh, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    ld->glContext = SDL_GL_CreateContext(ld->window);
    if (SDL_GL_SetSwapInterval(-1) == -1) // adaptive vsync
    {
        SDL_GL_SetSwapInterval(1); // standard vsync
    }

    // Initialise gl3w
    gl3wInit();

    // Initialise ImGui
    ImGui::CreateContext();
    ud->io = &ImGui::GetIO();
    ImGui_ImplSdlGL3_Init(ld->window);
    ImGui::StyleColorsDark();
    ud->style = &ImGui::GetStyle();
    ud->style->WindowRounding = 0;

    // Loadind a shader from file doesn't require recompiling to change shader but less portable
    // Creating a shader requires recompiling to change shader but more portable
    // So try loading but if it fails use the compiled shader
    string vshader = glakReadFile("vshader.glsl");
    string fshader = glakReadFile("fshader.glsl");
    if(vshader == "")
    {
        vshader = R"(
#version 330 core

in vec4 vPosition;
in vec2 vTexCoord;
in vec3 vNormal;
in vec4 vColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform float shininess;

#define MAX_LIGHTS 6
struct light {
    vec3 position; // world space
    vec4 color; // w is intensity
};
uniform int lightCount;
uniform light lights[MAX_LIGHTS];

out vec4 fColor;
out vec3 fNormal;
out vec3 fPosition;
out vec2 fTexCoord;
out vec3 fReflection;

out vec3 fEye;

void main()
{
    fTexCoord = vTexCoord;
    // Checkpoint 1 / 2 / 3 / 4
    fColor = vColor;
    vec4 vertPos = model * vPosition; // object -> world space

    gl_Position = projection * view * vertPos; // world -> camera -> screen space

    // Checkpoint 2 / 3 / 4
    fEye = vec3(vec4(0.0f, 0.0f, 0.0f, 1.0f) * transpose(inverse(projection * view))); // screen -> camera -> world space
    fNormal = mat3(model) * vNormal; // object -> world space
    fPosition = vertPos.xyz;

    // R = 2(fNormal*vertPos)fNormal-vertPos;
    fReflection = reflect((view*vertPos).xyz, (mat3(view)*fNormal));
    // fReflection = reflect(vertPos.xyz, fNormal);

    // // Checkpoint 3
    // vec3 normal = normalize(fNormal);
    // vec3 viewDir = normalize(fEye - fPosition);
    // fColor = ambient * vColor;
    // for(int i = 0; i < lightCount && i < MAX_LIGHTS; i++)
    // {
    //     vec3 lightDir = normalize(lights[i].position - fPosition);
    //     float dNL = max(dot(normal, lightDir), 0.0f);
    //     vec4 color = diffuse;
    //     color = diffuse * vColor;
    //     vec4 lambert = color * lights[i].color * dNL;

    //     vec3 halfVec = normalize(lightDir + viewDir);
    //     float dNH = max(dot(normal, halfVec), 0.0f);
    //     vec4 phong = specular * lights[i].color * pow(dNH, shininess);
    //     fColor += lambert + phong;
    // }
})";
    }
    if(fshader == "")
    {
        fshader = R"(
#version 330 core

in vec3 fEye;
smooth in vec4 fColor;
smooth in vec3 fNormal;
smooth in vec3 fPosition;
smooth in vec2 fTexCoord;
smooth in vec3 fReflection;

uniform sampler2D albedo;
uniform samplerCube envCube;

uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform float shininess;

#define MAX_LIGHTS 6
struct light {
    vec3 position;
    vec4 color; // w is intensity
};
uniform int lightCount;
uniform light lights[MAX_LIGHTS];

out vec4 pColor;

void main() 
{
    // Checkpoint 1 / 2 / 3
    // vec4 baseColor = fColor * texture(albedo, fTexCoord);
    // vec4 baseColor = fColor;
    vec4 baseColor = texture(albedo, fTexCoord);
    // vec4 baseColor = vec4(fTexCoord, 0.0f, 1.0f);
    // vec4 baseColor = texture(envCube, fReflection);

    pColor = baseColor;
    // Checkpoint 4
    // vec3 normal = normalize(fNormal);
    // vec3 viewDir = normalize(fEye - fPosition);
    // pColor = ambient * baseColor; // fColor;
    // for(int i = 0; i < lightCount && i < MAX_LIGHTS; i++)
    // {
    //     vec3 lightDir = normalize(lights[i].position - fPosition);
    //     float dNL = max(dot(normal, lightDir), 0.0f);
    //     vec4 color = diffuse;
    //     color = diffuse * baseColor;
    //     vec4 lambert = color * lights[i].color * dNL;

    //     vec3 halfVec = normalize(lightDir + viewDir);
    //     float dNH = max(dot(normal, halfVec), 0.0f);
    //     vec4 phong = specular * lights[i].color * pow(dNH, shininess);
    //     pColor += lambert + phong;
    // }
})";
    }

    shared_ptr<glakShader> shader = make_shared<glakShader>(vshader, fshader);

    // Enable OpenGL Z buffer
    glEnable(GL_DEPTH_TEST);

    glakBitmap bmp = glakReadPNM("wood.ppm");
    // glakBitmap bmp(4, vector<glakColor>{
    //     {0, 0, 0, 255}, {255, 255, 255, 255}, {0, 0, 0, 255}, {255, 255, 255, 255}, 
    //     {255, 255, 255, 255}, {0, 0, 0, 255}, {255, 255, 255, 255}, {0, 0, 0, 255}, 
    //     {0, 0, 0, 255}, {255, 255, 255, 255}, {0, 0, 0, 255}, {255, 255, 255, 255}, 
    //     {255, 255, 255, 255}, {0, 0, 0, 255}, {255, 255, 255, 255}, {0, 0, 0, 255}
    // });

    ud->bitmap = make_shared<glakBitmap>(bmp);

    ud->image = make_shared<glakImage>();
    ud->image->generate(GL_TEXTURE_2D, 0, GL_RGBA, 0, bmp, {
        glakTexParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST), 
        glakTexParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST),
        glakTexParam(GL_TEXTURE_WRAP_S, GL_REPEAT),
        glakTexParam(GL_TEXTURE_WRAP_T, GL_REPEAT)
    });

    // Add a mesh to the object
    ud->scene.obj->mesh.resize(1);
    ud->scene.obj->mesh[0].shader = shader;
    ud->scene.obj->mesh[0].images["albedo"] = ud->image;
    ud->scene.obj->mesh[0].images["envCube"] = make_shared<glakImage>(
        GL_TEXTURE_CUBE_MAP, 0, GL_RGB, 0, glakBitmap(), vector<glakTexParam>{
        glakTexParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR), 
        glakTexParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR),
        glakTexParam(GL_TEXTURE_WRAP_S, GL_REPEAT),
        glakTexParam(GL_TEXTURE_WRAP_T, GL_REPEAT),
        glakTexParam(GL_TEXTURE_WRAP_R, GL_REPEAT)
    });
    bitmapToTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, 0, bmp);//glakBitmap(1, {glakColor(255, 0, 0)}));
    bitmapToTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, 0, bmp);//glakBitmap(1, {glakColor(0, 255, 255)}));
    bitmapToTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, 0, bmp);//glakBitmap(1, {glakColor(0, 255, 0)}));
    bitmapToTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, 0, bmp);//glakBitmap(1, {glakColor(255, 0, 255)}));
    bitmapToTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, 0, bmp);//glakBitmap(1, {glakColor(0, 0, 255)}));
    bitmapToTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, 0, bmp);//glakBitmap(1, {glakColor(255, 255, 0)}));

    vector<uint8_t> indices = {
        0, 1, 2, 
        0, 2, 3,

        0, 4, 5, 
        0, 5, 1,

        1, 5, 6, 
        1, 6, 2,

        2, 6, 7, 
        2, 7, 3,

        3, 7, 4, 
        3, 4, 0,

        4, 5, 6, 
        4, 6, 7
    };

    vector<glm::vec4> vertPos = {
        { 0.7f, -0.7f,  0.7f, 1.0f},
        { 0.7f,  0.7f,  0.7f, 1.0f},
        {-0.7f,  0.7f,  0.7f, 1.0f},
        {-0.7f, -0.7f,  0.7f, 1.0f},
        { 0.7f, -0.7f, -0.7f, 1.0f},
        { 0.7f,  0.7f, -0.7f, 1.0f},
        {-0.7f,  0.7f, -0.7f, 1.0f},
        {-0.7f, -0.7f, -0.7f, 1.0f}
    };

    // Add vertices
    glakMeshElement& vpos = ud->scene.obj->mesh[0].elements["vPosition"];
    glm::vec4* vposp = vpos.init<glm::vec4>(indices.size());
    for(size_t i = 0; i < indices.size(); i++)
    {
        vposp[i] = vertPos[indices[i]];
    }
    vpos.normalized = false;
    vpos.active = true;

    // Add colors
    glakMeshElement& vcol = ud->scene.obj->mesh[0].elements["vColor"];
    glm::vec4* vcolp = vcol.init<glm::vec4>(indices.size());
    vcolp[0]  = {1.0f, 0.0f, 0.0f, 1.0f}; vcolp[1]  = {1.0f, 0.0f, 0.0f, 1.0f}; vcolp[2]  = {1.0f, 0.0f, 0.0f, 1.0f};
    vcolp[3]  = {1.0f, 0.0f, 0.0f, 1.0f}; vcolp[4]  = {1.0f, 0.0f, 0.0f, 1.0f}; vcolp[5]  = {1.0f, 0.0f, 0.0f, 1.0f};

    vcolp[6]  = {0.0f, 1.0f, 0.0f, 1.0f}; vcolp[7]  = {0.0f, 1.0f, 0.0f, 1.0f}; vcolp[8]  = {0.0f, 1.0f, 0.0f, 1.0f};
    vcolp[9]  = {0.0f, 1.0f, 0.0f, 1.0f}; vcolp[10] = {0.0f, 1.0f, 0.0f, 1.0f}; vcolp[11] = {0.0f, 1.0f, 0.0f, 1.0f};

    vcolp[12] = {0.0f, 0.0f, 1.0f, 1.0f}; vcolp[13] = {0.0f, 0.0f, 1.0f, 1.0f}; vcolp[14] = {0.0f, 0.0f, 1.0f, 1.0f};
    vcolp[15] = {0.0f, 0.0f, 1.0f, 1.0f}; vcolp[16] = {0.0f, 0.0f, 1.0f, 1.0f}; vcolp[17] = {0.0f, 0.0f, 1.0f, 1.0f};

    vcolp[18] = {0.0f, 1.0f, 1.0f, 1.0f}; vcolp[19] = {0.0f, 1.0f, 1.0f, 1.0f}; vcolp[20] = {0.0f, 1.0f, 1.0f, 1.0f};
    vcolp[21] = {0.0f, 1.0f, 1.0f, 1.0f}; vcolp[22] = {0.0f, 1.0f, 1.0f, 1.0f}; vcolp[23] = {0.0f, 1.0f, 1.0f, 1.0f};

    vcolp[24] = {1.0f, 0.0f, 1.0f, 1.0f}; vcolp[25] = {1.0f, 0.0f, 1.0f, 1.0f}; vcolp[26] = {1.0f, 0.0f, 1.0f, 1.0f};
    vcolp[27] = {1.0f, 0.0f, 1.0f, 1.0f}; vcolp[28] = {1.0f, 0.0f, 1.0f, 1.0f}; vcolp[29] = {1.0f, 0.0f, 1.0f, 1.0f};

    vcolp[30] = {1.0f, 1.0f, 0.0f, 1.0f}; vcolp[31] = {1.0f, 1.0f, 0.0f, 1.0f}; vcolp[32] = {1.0f, 1.0f, 0.0f, 1.0f};
    vcolp[33] = {1.0f, 1.0f, 0.0f, 1.0f}; vcolp[34] = {1.0f, 1.0f, 0.0f, 1.0f}; vcolp[35] = {1.0f, 1.0f, 0.0f, 1.0f};
    vcol.normalized = false;
    vcol.active = true;

    glakMeshElement& vtex = ud->scene.obj->mesh[0].elements["vTexCoord"];
    glm::vec2* vtexp = vtex.init<glm::vec2>(indices.size());
    vtexp[0]  = {0.0f, 0.0f}; vtexp[1]  = {1.0f, 0.0f}; vtexp[2]  = {1.0f, 1.0f};
    vtexp[3]  = {0.0f, 0.0f}; vtexp[4]  = {1.0f, 1.0f}; vtexp[5]  = {0.0f, 1.0f};
    
    vtexp[6]  = {0.0f, 0.0f}; vtexp[7]  = {1.0f, 0.0f}; vtexp[8]  = {1.0f, 1.0f};
    vtexp[9]  = {0.0f, 0.0f}; vtexp[10] = {1.0f, 1.0f}; vtexp[11] = {0.0f, 1.0f};
    
    vtexp[12] = {0.0f, 0.0f}; vtexp[13] = {1.0f, 0.0f}; vtexp[14] = {1.0f, 1.0f};
    vtexp[15] = {0.0f, 0.0f}; vtexp[16] = {1.0f, 1.0f}; vtexp[17] = {0.0f, 1.0f};
    
    vtexp[18] = {0.0f, 0.0f}; vtexp[19] = {1.0f, 0.0f}; vtexp[20] = {1.0f, 1.0f};
    vtexp[21] = {0.0f, 0.0f}; vtexp[22] = {1.0f, 1.0f}; vtexp[23] = {0.0f, 1.0f};
    
    vtexp[24] = {0.0f, 0.0f}; vtexp[25] = {1.0f, 0.0f}; vtexp[26] = {1.0f, 1.0f};
    vtexp[27] = {0.0f, 0.0f}; vtexp[28] = {1.0f, 1.0f}; vtexp[29] = {0.0f, 1.0f};
    
    vtexp[30] = {0.0f, 0.0f}; vtexp[31] = {1.0f, 0.0f}; vtexp[32] = {1.0f, 1.0f};
    vtexp[33] = {0.0f, 0.0f}; vtexp[34] = {1.0f, 1.0f}; vtexp[35] = {0.0f, 1.0f};
    vtex.normalized = false;
    vtex.active = true;

    glakMeshElement& norm = ud->scene.obj->mesh[0].elements["vNormal"];
    glm::vec3* normp = norm.init<glm::vec3>(indices.size());
    for(int i = 0; i < indices.size(); i++) normp[i] = glm::normalize((glm::vec3)vposp[i]);
    norm.normalized = true;
    norm.active = true;

    ud->scene.obj->mesh[0].index.clear();

    ud->scene.obj->update();

    /*
     * model:       model transform     model -> world      updated on model change
     * view:        camera transform    world -> camera     updated on camera change
     * projection:  screen transform    camera -> screen    updated on screen change
     * screenPosition = projection * view * model * vertPosition
    */

    ud->shader = shader;

    ud->scene.camera->parent = ud->scene.cameraBoom;
    ud->scene.camera->transform.clear();
    ud->scene.camera->transform.setTranslation({0.0f, 0.0f, -3.0f});
    ud->shader->setUniform("view", &ud->scene.camera->getTransform());

    ud->scene.camera->projection = glm::perspective(90.0f * glm::pi<float>() / 180.0f, (float)ud->screenw / (float)ud->screenh, 0.01f, 100.0f);
    ud->shader->setUniform("projection", &ud->scene.camera->projection[0][0]);

    int lightCount = 3;
    ud->shader->setUniform("lightCount", &lightCount);
    ud->shader->setUniform("lights[0].position", &glm::vec3(1.5f, 1.5f, 1.5f));
    ud->shader->setUniform("lights[0].color", &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    ud->shader->setUniform("lights[1].position", &glm::vec3(-1.5f, 1.5f, 1.5f));
    ud->shader->setUniform("lights[1].color", &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    ud->shader->setUniform("lights[2].position", &glm::vec3(1.5f, -1.5f, -1.5f));
    ud->shader->setUniform("lights[2].color", &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    ud->shader->setUniform("ambient", &glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
    ud->shader->setUniform("diffuse", &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    ud->shader->setUniform("specular", &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    float shine = 100.0f;
    ud->shader->setUniform("shininess", &shine);

    

    // During init, enable debug output
    // glEnable(GL_DEBUG_OUTPUT);
    // glDebugMessageCallback(MessageCallback, 0);

    ud->consoleInput.lock();
    ud->consoleIO = thread(consoleIO, ud);
    ld->userData = (void*)ud;
}

///
/// destroy()
/// Called only once (at application shutdown)
///
void destroy(glakLoopData* ld)
{
    userData_t* ud = (userData_t*)ld->userData;
    ImGui_ImplSdlGL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(ld->glContext);
    SDL_DestroyWindow(ld->window);


    delete ud;

    SDL_Quit();

	exit(EXIT_SUCCESS);
}