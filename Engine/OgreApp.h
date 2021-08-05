﻿#pragma once

#include <OGRE/Bites/OgreApplicationContext.h>
#include <OGRE/Bites/OgreTrays.h>
#include <OGRE/Bites/OgreWindowEventUtilities.h>

#include "Pointers.h"
#include "String.h"
#include "Vector2.h"

class Texture;

class OgreApp : public OgreBites::ApplicationContext, public OgreBites::InputListener, OgreBites::WindowEventListener, public OgreBites::TrayListener
{
public:
    explicit OgreApp(const String& name);
    
    void setup() override;
    void load();
    void close();

    bool keyPressed(const OgreBites::KeyboardEvent& evt) override;
    bool keyReleased(const OgreBites::KeyboardEvent& evt) override;
    bool textInput(const OgreBites::TextInputEvent& evt) override;
    bool mousePressed(const OgreBites::MouseButtonEvent& evt) override;
    bool mouseReleased(const OgreBites::MouseButtonEvent& evt) override;
    bool axisMoved(const OgreBites::AxisEvent& evt) override;
    bool mouseMoved(const OgreBites::MouseMotionEvent& evt) override;
    void windowResized(Ogre::RenderWindow* rw) override;

    void set_window_icon(const Shared<Texture>& icon_texture);

    std::function<void()> on_setup;

    std::function<bool(int key, bool repeat)> on_keyPressed;
    std::function<bool(int key)> on_keyReleased;
    std::function<bool(const char* chars)> on_textInput;
    std::function<bool(int button)> on_mousePressed;
    std::function<bool(int button)> on_mouseReleased;
    std::function<bool(int axis, float value)> on_axisMoved;
    std::function<bool(const Vector2& new_pos, const Vector2& delta)> on_mouseMoved;
    std::function<void(Ogre::RenderWindow*)> on_windowResized;

private:
    Shared<OgreBites::TrayManager> ui_;
};
