//
// Created by lewin on 06.07.16.
//

#ifndef SUBMENU
#define SUBMENU

#include "../menu.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "../../state.hpp"

class SubMenu : public Menu{
public:
    SubMenu(Menu *menu,GLFWwindow *window,GLFWcursor *cur);
    Menu *primaryMenu;

    virtual void draw() override;
    virtual void buttonControl() override;

};
void goBack(State *state, GLFWwindow *window, GLFWcursor *cursor);

#endif //CATERPILLARS_SUBMENU_HPP
