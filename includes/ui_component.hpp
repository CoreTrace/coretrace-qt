#pragma once

#include <QWidget>

class UIComponent {
public:
    virtual ~UIComponent() = default;
    virtual void render() = 0;
}; 