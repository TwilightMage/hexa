﻿#include "DebugPlayer.h"

#include <GLFW/glfw3.h>


#include "Camera.h"

void DebugPlayer::start()
{
    camera = new Camera();
    camera->owner = this;
    camera->fov = 45.0f;
}

void DebugPlayer::key_down(int key)
{
    if (key == GLFW_KEY_W) move_forward += 1;
    else if (key == GLFW_KEY_S) move_forward -= 1;
    if (key == GLFW_KEY_D) move_right += 1;
    else if (key == GLFW_KEY_A) move_right -= 1;
    if (key == GLFW_KEY_SPACE) move_up += 1;
    else if (key == GLFW_KEY_LEFT_SHIFT) move_up -= 1;
}

void DebugPlayer::key_up(int key)
{
    if (key == GLFW_KEY_W) move_forward -= 1;
    else if (key == GLFW_KEY_S) move_forward += 1;
    if (key == GLFW_KEY_D) move_right -= 1;
    else if (key == GLFW_KEY_A) move_right += 1;
    if (key == GLFW_KEY_SPACE) move_up -= 1;
    else if (key == GLFW_KEY_LEFT_SHIFT) move_up += 1;
}

void DebugPlayer::tick(float delta_time)
{
    position.x += delta_time * move_forward;
    position.y += delta_time * move_right;
    position.z += delta_time * move_up;
}
