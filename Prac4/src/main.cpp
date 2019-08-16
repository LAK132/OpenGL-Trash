#include "main.h"
#define GLAK_IMPLEMENTATION
#include <glak.hpp>

///
/// loop()
/// Called every loop
///
void update(glakLoopData* ld, double deltaTime)
{
    userData_t* ud = (userData_t*)ld->userData;
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
                    ud->scene.obj->shader[0]->setUniform("projection", &(ud->scene.camera->projection[0][0]));
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
    
    ud->scene.cameraBoom->transform.update(deltaTime);
    ud->scene.camera->transform.make();
    ud->shader->setUniform("view", &ud->scene.camera->getTransform());

    ImGui_ImplSdlGL3_NewFrame(ld->window);

    ImGui::Text("%f", deltaTime * 1000.0);
    
    ImGui::Render();
}

glm::vec4 ComputeLight (glm::vec3 direction, glm::vec4 lightcolor, glm::vec3 normal, glm::vec3 halfvec, glm::vec4 mydiffuse, glm::vec4 myspecular, float myshininess) 
{
    float nDotL = glm::dot(normal, direction);         
    glm::vec4 lambert = mydiffuse * lightcolor * max(nDotL, 0.0f);  

    float nDotH = glm::dot(normal, halfvec); 
    glm::vec4 phong = myspecular * lightcolor * pow(max(nDotH, 0.0f), myshininess); 

    glm::vec4 retval = lambert + phong; 
    return retval;
}  

///
/// draw()
/// Called every loop
///
void draw(glakLoopData* ld, double deltaTime)
{
    userData_t* ud = (userData_t*)ld->userData;
    glViewport(0, 0, (int)ud->io->DisplaySize.x, (int)ud->io->DisplaySize.y);
    glClearColor(ud->clearCol[0], ud->clearCol[1], ud->clearCol[2], ud->clearCol[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw our object
    // /*
    glm::vec4* vcolp = (glm::vec4*)ud->scene.obj->mesh[0].elements["vColor"].data;
    glm::vec4* ucolp = (glm::vec4*)ud->scene.obj->mesh[0].elements["unlitColor"].data;
    glm::vec3* normp = (glm::vec3*)ud->scene.obj->mesh[0].elements["vNormal"].data;
    glm::vec4* vposp = (glm::vec4*)ud->scene.obj->mesh[0].elements["vPosition"].data;
    const glm::vec4 eyePos = glakTransform::WUP;
    for(int i = 0; i < 8; i++)
    {

        const glm::vec4 vertPos = ud->scene.camera->getTransform() * ud->scene.obj->getTransform() * vposp[i];
        const glm::vec4 viewDir = glm::normalize(eyePos - vertPos);

        const glm::vec4 lightPos = glakTransform::WUP;
        const glm::vec4 lightDir = glm::normalize(lightPos - vertPos);
        const glm::vec4 lightCol = {1.0f, 1.0f, 1.0f, 1.0f};
        const glm::vec4 half = normalize(lightDir + viewDir);
        const glm::vec4 col = ComputeLight(lightDir, lightCol, glm::mat3(ud->scene.camera->getTransform() * ud->scene.obj->getTransform()) * normp[i], half, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, 100.0f);

        vcolp[i] = ucolp[i] * (col);
        if(vcolp[i].x > 1.0f) vcolp[i].x = 1.0f - (vcolp[i].x - 1.0f);
        if(vcolp[i].y > 1.0f) vcolp[i].y = 1.0f - (vcolp[i].y - 1.0f);
        if(vcolp[i].z > 1.0f) vcolp[i].z = 1.0f - (vcolp[i].z - 1.0f);
    }
    ud->scene.obj->updateBuffer();
    // */
    ud->scene.obj->draw();

    ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
}

template<typename T>
constexpr T sqr(T a) {
    return a * a;
}

template<typename T>
constexpr T power(T a, std::size_t n) {
    return n == 0 ? 1 : sqr(power(a, n / 2)) * (n % 2 == 0 ?  1 : a);
}

const int NumTimesToSubdivide = 5;
// const int NumTriangles        = 4096;  // (4 faces)^(NumTimesToSubdivide + 1)
const int NumTriangles        = power(4, NumTimesToSubdivide+1);  // (4 faces)^(NumTimesToSubdivide + 1)
const int NumVertices         = 3 * NumTriangles;

glm::vec4 points[NumVertices];
glm::vec3 normals[NumVertices];
GLuint indices[NumVertices];

//----------------------------------------------------------------------------

void triangle(const glm::vec4& a, const glm::vec4& b, const glm::vec4& c)
{
    static int Index = 0;
    glm::vec3 normal = glm::normalize(glm::cross((glm::vec3)(b - a), (glm::vec3)(c - b)));

    normals[Index] = normal; points[Index] = a; indices[Index] = Index; Index++;
    normals[Index] = normal; points[Index] = b; indices[Index] = Index; Index++;
    normals[Index] = normal; points[Index] = c; indices[Index] = Index; Index++;
}

//----------------------------------------------------------------------------

glm::vec4 unit(const glm::vec4& p)
{
    float len = p.x*p.x + p.y*p.y + p.z*p.z;
    
    glm::vec4 t;
    if (len > E_EPSILON) 
    {
        t = p / sqrt(len);
        t.w = 1.0;
    }

    return t;
}

void divide_triangle(const glm::vec4& a, const glm::vec4& b, const glm::vec4& c, int count)
{
    if (count > 0) 
    {
        glm::vec4 v1 = unit( a + b );
        glm::vec4 v2 = unit( a + c );
        glm::vec4 v3 = unit( b + c );
        divide_triangle(  a, v1, v2, count - 1 );
        divide_triangle(  c, v2, v3, count - 1 );
        divide_triangle(  b, v3, v1, count - 1 );
        divide_triangle( v1, v3, v2, count - 1 );
    }
    else 
    {
        triangle( a, b, c );
    }
}

void tetrahedron(int count)
{
    glm::vec4 v[4] = {
	    {0.0f, 0.0f, 1.0f, 1.0f},
	    {0.0f, 0.942809f, -0.333333f, 1.0f},
	    {-0.816497f, -0.471405f, -0.333333f, 1.0f},
	    {0.816497f, -0.471405f, -0.333333f, 1.0f}
    };

    divide_triangle(v[0], v[1], v[2], count);
    divide_triangle(v[3], v[2], v[1], count);
    divide_triangle(v[0], v[3], v[1], count);
    divide_triangle(v[0], v[2], v[3], count);
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
in vec3 vNormal;
in vec2 vTexCoord;
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
out vec3 fEye;

void main()
{
    // Checkpoint 1 / 2 / 3 / 4
    fColor = vColor;
    vec4 vertPos = model * vPosition; // object -> world space
    gl_Position = projection * view * vertPos; // world -> camera -> screen space

    // Checkpoint 2 / 3 / 4
    // fEye = vec3(vec4(0.0f, 0.0f, 0.0f, 1.0f) * transpose(inverse(projection * view))); // screen -> camera -> world space
    // fNormal = mat3(model) * vNormal; // object -> world space
    // fPosition = vertPos.xyz;

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
    pColor = fColor;

    // Checkpoint 4
    // vec3 normal = normalize(fNormal);
    // vec3 viewDir = normalize(fEye - fPosition);
    // pColor = ambient * fColor;
    // for(int i = 0; i < lightCount && i < MAX_LIGHTS; i++)
    // {
    //     vec3 lightDir = normalize(lights[i].position - fPosition);
    //     float dNL = max(dot(normal, lightDir), 0.0f);
    //     vec4 color = diffuse;
    //     color = diffuse * fColor;
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

    // Add a shader to the object
    ud->scene.obj->shader.resize(1);
    ud->scene.obj->shader[0] = shader;

    // Add a mesh to the object
    ud->scene.obj->mesh.resize(1);
    ud->scene.obj->mesh[0].material = 0;

    // tetrahedron(NumTimesToSubdivide);

    // Add vertices
    glakMeshElement& vpos = ud->scene.obj->mesh[0].elements["vPosition"];
    glm::vec4* vposp = (glm::vec4*)vpos.init(sizeof(glm::vec4), 8);
    vposp[0] = { 0.7f, -0.7f,  0.7f, 1.0f};
    vposp[1] = { 0.7f,  0.7f,  0.7f, 1.0f};
    vposp[2] = {-0.7f,  0.7f,  0.7f, 1.0f};
    vposp[3] = {-0.7f, -0.7f,  0.7f, 1.0f};
    vposp[4] = { 0.7f, -0.7f, -0.7f, 1.0f};
    vposp[5] = { 0.7f,  0.7f, -0.7f, 1.0f};
    vposp[6] = {-0.7f,  0.7f, -0.7f, 1.0f};
    vposp[7] = {-0.7f, -0.7f, -0.7f, 1.0f};
    vpos.normalized = false;
    vpos.active = true;

    // glakMeshElement& vpos = ud->scene.obj->mesh[0].elements["vPosition"];
    // vpos.init(sizeof(glm::vec4), NumVertices, points);
    // vpos.normalized = false;
    // vpos.active = true;

    // Add colors
    glakMeshElement& vcol = ud->scene.obj->mesh[0].elements["vColor"];
    glm::vec4* vcolp = (glm::vec4*)vcol.init(sizeof(glm::vec4), 8);
    vcolp[0] = {0.0f, 0.0f, 0.0f, 1.0f};
    vcolp[1] = {1.0f, 0.0f, 0.0f, 1.0f};
    vcolp[2] = {1.0f, 0.0f, 1.0f, 1.0f};
    vcolp[3] = {0.0f, 0.0f, 1.0f, 1.0f};
    vcolp[4] = {0.0f, 1.0f, 0.0f, 1.0f};
    vcolp[5] = {1.0f, 1.0f, 0.0f, 1.0f};
    vcolp[6] = {1.0f, 1.0f, 1.0f, 1.0f};
    vcolp[7] = {0.0f, 1.0f, 1.0f, 1.0f};
    vcol.normalized = false;
    vcol.active = true;

    // glakMeshElement& vcol = ud->scene.obj->mesh[0].elements["vColor"];
    // glm::vec4* vcolp = (glm::vec4*)vcol.init(sizeof(glm::vec4), NumVertices);
    // for(size_t i = 0; i < NumVertices; i++) vcolp[i] = points[i];
    // vcol.normalized = false;
    // vcol.active = true;

    glakMeshElement& ucol = ud->scene.obj->mesh[0].elements["unlitColor"];
    glm::vec4* ucolp = (glm::vec4*)ucol.init(sizeof(glm::vec4), 8);
    ucolp[0] = {0.0f, 0.0f, 0.0f, 1.0f};
    ucolp[1] = {1.0f, 0.0f, 0.0f, 1.0f};
    ucolp[2] = {1.0f, 0.0f, 1.0f, 1.0f};
    ucolp[3] = {0.0f, 0.0f, 1.0f, 1.0f};
    ucolp[4] = {0.0f, 1.0f, 0.0f, 1.0f};
    ucolp[5] = {1.0f, 1.0f, 0.0f, 1.0f};
    ucolp[6] = {1.0f, 1.0f, 1.0f, 1.0f};
    ucolp[7] = {0.0f, 1.0f, 1.0f, 1.0f};
    ucol.active = false;

    glakMeshElement& norm = ud->scene.obj->mesh[0].elements["vNormal"];
    glm::vec3* normp = (glm::vec3*)norm.init(sizeof(glm::vec3), 8);
    for(int i = 0; i < 8; i++) normp[i] = glm::normalize((glm::vec3)vposp[i]);
    norm.normalized = true;
    norm.active = true;

    // glakMeshElement& norm = ud->scene.obj->mesh[0].elements["vNormal"];
    // norm.init(sizeof(glm::vec3), NumVertices, normals);
    // norm.normalized = true;
    // norm.active = true;

    ud->scene.obj->mesh[0].index = {
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

    // ud->scene.obj->mesh[0].index.resize(NumVertices);
    // for(int i = 0; i < NumVertices; i++) ud->scene.obj->mesh[0].index[i] = indices[i];

    ud->scene.obj->updateBuffer();

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
}