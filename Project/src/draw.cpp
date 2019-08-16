#include "main.h"
///
/// draw()
/// Called every loop
///
void draw(lak::loopData* ld)
{
    userData_t& ud = ld->userData;
    glViewport(0, 0, (int)ud.io->DisplaySize.x, (int)ud.io->DisplaySize.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw our object
    ud.scene.shader->setUniform("projection", &(ud.scene.camera->projection[0][0]));
    ud.scene.shader->setUniform("view", &(ud.scene.camera->update()[0][0]));
    
    ud.scene.ball->draw();
    for (auto& it : ud.scene.blocks)
        it->draw();
    for (auto& it : ud.scene.coins)
        it->draw(true);

    if(ImDrawData* draw_data = ImGui::GetDrawData()) 
        ImGui_ImplSdlGL3_RenderDrawData(draw_data);
}