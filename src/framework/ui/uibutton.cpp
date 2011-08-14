#include "uibutton.h"
#include <graphics/borderimage.h>
#include <graphics/font.h>
#include <otml/otmlnode.h>
#include <luascript/luainterface.h>

UIButton::UIButton(): UIWidget(UITypeButton)
{
    m_state = ButtonUp;

    // by default, all callbacks call lua fields
    m_onClick = [this]() { this->callLuaField("onClick"); };
}

UIButtonPtr UIButton::create()
{
    UIButtonPtr button(new UIButton);
    button->setStyle("Button");
    return button;
}

void UIButton::loadStyleFromOTML(const OTMLNodePtr& styleNode)
{
    UIWidget::loadStyleFromOTML(styleNode);

    for(int i=0; i<3; ++i) {
        m_statesStyle[i].image = getImage();
        m_statesStyle[i].color = getColor();
        m_statesStyle[i].textTranslate = Point(0,0);
    }

    if(OTMLNodePtr node = styleNode->get("state.up"))
        loadStateStyle(m_statesStyle[ButtonUp], node);
    if(OTMLNodePtr node = styleNode->get("state.hover"))
        loadStateStyle(m_statesStyle[ButtonHover], node);
    if(OTMLNodePtr node = styleNode->get("state.down"))
        loadStateStyle(m_statesStyle[ButtonDown], node);

    m_text = styleNode->readAt("text", aux::empty_string);

    if(OTMLNodePtr node = styleNode->get("onClick")) {
        g_lua.loadFunction(node->read<std::string>(), "@" + node->source() + "[" + node->tag() + "]");
        luaSetField("onClick");
    }
}

void UIButton::loadStateStyle(ButtonStateStyle& stateStyle, const OTMLNodePtr& stateStyleNode)
{
    if(OTMLNodePtr node = stateStyleNode->get("border-image"))
        stateStyle.image = BorderImage::loadFromOTML(node);
    if(OTMLNodePtr node = stateStyleNode->get("image"))
        stateStyle.image = Image::loadFromOTML(node);
    stateStyle.textTranslate = stateStyleNode->readAt("text-translate", Point());
    stateStyle.color = stateStyleNode->readAt("color", getColor());
}

void UIButton::render()
{
    const ButtonStateStyle& currentStyle = m_statesStyle[m_state];
    Rect textRect = getGeometry();

    if(currentStyle.image)
        currentStyle.image->draw(textRect);

    textRect.translate(currentStyle.textTranslate);
    getFont()->renderText(m_text, textRect, AlignCenter, currentStyle.color);
}

void UIButton::onHoverChange(bool hovered)
{
    if(hovered && m_state == ButtonUp)
        m_state = ButtonHover;
    else if(m_state == ButtonHover)
        m_state = ButtonUp;
}

void UIButton::onMousePress(const UIMouseEvent& event)
{
    if(event.button == MouseLeftButton)
        m_state = ButtonDown;
}

void UIButton::onMouseRelease(const UIMouseEvent& event)
{
    if(m_state == ButtonDown) {
        if(m_onClick && getGeometry().contains(event.mousePos))
            m_onClick();
        m_state = isHovered() ? ButtonHover : ButtonUp;
    }
}
