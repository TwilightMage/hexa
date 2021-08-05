﻿#include "OgreApp.h"

OgreApp::OgreApp(const String& name)
    : OgreBites::ApplicationContext(name.c())
{
}

Shared<OgreBites::TrayManager> OgreApp::spawn_ui()
{
    if (ui_) return ui_;
    
    ui_ = MakeShared<OgreBites::TrayManager>("UI", getRenderWindow(), this);
    addInputListener(ui_.get());
    ui_->hideCursor();

    return ui_;
}

void OgreApp::setup()
{
    OgreBites::ApplicationContext::setup();

    addInputListener(this);

    OgreBites::WindowEventUtilities::_addRenderWindow(getRenderWindow());
    OgreBites::WindowEventUtilities::addWindowEventListener(getRenderWindow(), this);

    on_setup();
}

bool OgreApp::keyPressed(const OgreBites::KeyboardEvent& evt)
{
    return on_keyPressed(evt.keysym.sym, evt.repeat);
}

bool OgreApp::keyReleased(const OgreBites::KeyboardEvent& evt)
{
    return on_keyReleased(evt.keysym.sym);
}

bool OgreApp::textInput(const OgreBites::TextInputEvent& evt)
{
    return on_textInput(evt.chars);
}

bool OgreApp::mousePressed(const OgreBites::MouseButtonEvent& evt)
{
    return on_mousePressed(evt.button);
}

bool OgreApp::mouseReleased(const OgreBites::MouseButtonEvent& evt)
{
    return on_mouseReleased(evt.button);
}

bool OgreApp::axisMoved(const OgreBites::AxisEvent& evt)
{
    return on_axisMoved(evt.axis, evt.value);
}

bool OgreApp::mouseMoved(const OgreBites::MouseMotionEvent& evt)
{
    return on_mouseMoved(Vector2((float)evt.x, (float)evt.y), Vector2((float)evt.xrel, (float)evt.yrel));
}

void OgreApp::windowResized(Ogre::RenderWindow* rw)
{
    on_windowResized(rw);
}
