#ifndef MENU_H
#define MENU_H

#include <cstring>

#include "IMenu.h"
#include "../Painter/Painter.h"
#include "../Widgets/MenuItem.h"

#include "../Widgets/PopUpParameterMenu.h"
#include "../Widgets/CheckboxMenuItem.h"
#include "../Widgets/PopUpMenuItem.h"

#include "../ButtonDefinitions.h"

#include "../../GlobalSettings.h"

#include "../../../Bootloader/Periphery/USB/IUSBDevice.h"

//#include <Helpers.h>

class Menu : public IMenu
{
  protected:
    char const* _label;
    char const* _menuBreadcrumbs;

    uint8_t _menuItemsCount;
    uint8_t _menuSelectionIndex;

    uint8_t _maxVisibleItems;

    uint8_t _parameterSelectionIndex;
    uint8_t _parameterMenuActive;
    uint8_t _menuOffset;

    // Color Defintions
    uint16_t _menuBackgroundColor;
    uint16_t _menuItemColor;
    uint16_t _menuDimmedItemColor;
    uint16_t _menuDisabledItemColor;
    uint16_t _menuSelectedItemColor;
    uint16_t _menuHightlightedItemColor;
    uint16_t _menuTextColor;
    uint16_t _menuDisabledTextColor;
    uint16_t _menuSelectedTextColor;

    PopUpParameterMenu _popUpParameterMenu;
    int8_t _popUpParameterMenuActive;

    // this array of pointers to menuItems allows a menu of up to 64 entries to be filled by inherited class
    MenuItem* _menuItem[64] = {nullptr};

  public:
    // TODO: Add assignment of menu system to IMenu
    explicit Menu(IUSBDevice* cdcDevice) :
        IMenu(cdcDevice), _menuItemsCount(0), _menuSelectionIndex(0), _maxVisibleItems(7), _popUpParameterMenu(10, 10)
    {
        // UNUSED(cdcDevice);
        //_usbDevice = cdcDevice;

        _label = "Menu";
        _menuBreadcrumbs = "Menu";

        // Color defintions
        _menuBackgroundColor = RGB565(180, 180, 180);
        _menuItemColor = (uint16_t)Color565::White;
        _menuSelectedItemColor = RGB565(255, 128, 0);
        _menuDimmedItemColor = RGB565(247, 251, 247);
        _menuSelectedTextColor = RGB565(255, 255, 255);
        _menuHightlightedItemColor = RGB565(0, 128, 255);
        _menuTextColor = (uint16_t)Color565::Black;
        _menuDisabledTextColor = RGB565(40, 40, 40);
        _menuDisabledItemColor = RGB565(180, 180, 180);

        // init menu selection indexes
        _parameterMenuActive = 0;
        _parameterSelectionIndex = 0;
        _menuOffset = 0;

        // Default selection is first item
        if (_menuItem[_menuSelectionIndex] != nullptr)
        {
            _menuItem[_menuSelectionIndex]->SetHighlighted(true);
        }

        _popUpParameterMenuActive = -1;
    }

    void SetLabel(char* value)
    {
        _label = value;
    }

    void SetMenuItem(uint8_t index, MenuItem* menuItem)
    {
        if (_menuItem[index] != nullptr)
        {
            _menuItem[index] = menuItem;
        }
    }

    const char* GetLabel()
    {
        return _label;
    }

    int8_t GetMenuItemsCount()
    {
        return _menuItemsCount;
    }

  protected:
    virtual void Draw(IPainter* painter) override
    {
        painter->SetFont(Font::FreeSans9pt7b);
        DrawHeader(painter);

        // draw menu items
        DrawMenuItems(painter);

        // draw scroll bar indicator only if there are more than 7 menu items
        if (_menuItemsCount > 7)
        {
            DrawScrollIndicator(painter);
        }

        // draw parameter menu
        if (_popUpParameterMenuActive > -1)
        {
            _popUpParameterMenu.Draw(painter);
        }
    }

    void DrawHeader(IPainter* painter)
    {
        // draw header background
        painter->DrawFillRectangle(0, 0, GlobalSettings::LCDWidth, 28, _menuItemColor);

        // draw header bread crumbs
        painter->SetFont(Font::FreeSans9pt7b);
        painter->DrawText(5, 20, _menuBreadcrumbs, _menuTextColor, TextAlign::TEXT_ALIGN_LEFT, 0);

        // two header separation lines
        painter->DrawLine(0, 29, GlobalSettings::LCDWidth - 1, 29, _menuSelectedItemColor);

        painter->DrawLine(0, 30, GlobalSettings::LCDWidth - 1, 30, _menuBackgroundColor);
    }

    void DrawMenuItems(IPainter* painter)
    {
        int8_t displaySelectionIndex = _menuSelectionIndex - _menuOffset;

        // the _menuOffset is added to the item index and defines which item is the first one shown on screen

        // scrolling up from the first item
        if (displaySelectionIndex < 0)
        {
            _menuOffset -= 1;
        }

        // scrolling down from the last item
        if (displaySelectionIndex >= 7)
        {
            _menuOffset += 1;
        }

        // only up to 7 menu items fit on screen at once
        uint8_t displayItemsCount = LimitRange(_menuItemsCount, 0, 7);

        for (uint8_t itemIndex = 0; itemIndex < displayItemsCount; itemIndex++)
        {
            MenuItem* currentMenuItem = _menuItem[itemIndex + _menuOffset];

            if (currentMenuItem == nullptr)
            {
                continue;
            }

            uint16_t y = 31 + itemIndex * 30;
            currentMenuItem->SetDimensions(31, y, GlobalSettings::LCDWidth - 30, 29);

            currentMenuItem->Draw(painter);
        }
    }

    void DrawScrollIndicator(IPainter* painter)
    {
        // start the scrollbar below the header
        uint8_t scrollbarYOrigin = 31;

        // maximum height is the screen without header area
        uint8_t scrollbarHeight = GlobalSettings::LCDHeight - scrollbarYOrigin;

        // height of the scroll indicator is defined by the ratio of number of items on screen vs total number of item.
        // for example: if there are 7 items on screen of total 14 items the scroll indicator shall be 50% of the
        // scrollbar height
        uint8_t sliderHeight = scrollbarHeight * ((float)_maxVisibleItems / _menuItemsCount);

        // Calculate offset of scrollbar for 1 item
        float segmentOffset = (scrollbarHeight - sliderHeight) / (_menuItemsCount - _maxVisibleItems);

        uint8_t scrollbarYOffset = scrollbarYOrigin + (_menuOffset * segmentOffset);

        // Background
        painter->DrawFillRectangle(GlobalSettings::LCDWidth - 16, scrollbarYOrigin, 16,
                                   GlobalSettings::LCDHeight - scrollbarYOrigin, _menuItemColor);

        // Thin Line
        painter->DrawFillRectangle(GlobalSettings::LCDWidth - 10, scrollbarYOrigin, 4,
                                   GlobalSettings::LCDHeight - scrollbarYOrigin, _menuTextColor);

        // Draw slider as thick line
        painter->DrawFillRectangle(GlobalSettings::LCDWidth - 14, scrollbarYOffset, 12, sliderHeight, _menuTextColor);
    }

    void UnselectAllMenuItems()
    {
        uint8_t b;
        for (b = 0; b < _menuItemsCount; b++)
        {
            if (_menuItem[b] != nullptr)
            {
                _menuItem[b]->SetPressed(false);
            }
        }
    }

    void UnhighlightAllMenuItems()
    {
        uint8_t b;
        for (b = 0; b < _menuItemsCount; b++)
        {
            if (_menuItem[b] != nullptr)
            {
                _menuItem[b]->SetHighlighted(false);
            }
        }
    }

    void Update(Button button, int8_t knob, IMenuSystem* menuSystem) override
    {
        if (_menuItem[_menuSelectionIndex] == nullptr)
        {
            return;
        }

        /*if (_menuItem[_menuSelectionIndex] == nullptr)
        {
            return;
        }*/

        if (knob != 0)
        {
            if (knob > 0)
            {
                SelectionUp(menuSystem);
            } else
            {
                SelectionDown(menuSystem);
            }
        }

        switch (button)
        {
        case Button::BUTTON_3_UP:
            SelectionUp(menuSystem);
            break;
        case Button::BUTTON_6_UP:
            SelectionDown(menuSystem);
            break;
        case Button::BUTTON_5_UP:
            SelectionPress(menuSystem);
            _menuItem[_menuSelectionIndex]->SetPressed(false);
            break;
        case Button::BUTTON_5_DOWN:
            _menuItem[_menuSelectionIndex]->SetPressed(true);
            break;
        case Button::E_1_UP:
            _menuItem[_menuSelectionIndex]->SetPressed(false);
            SelectionPress(menuSystem);
            break;
        case Button::E_1_DOWN:
            _menuItem[_menuSelectionIndex]->SetPressed(true);
            break;
        default:
            break;
        }

        //_usbDevice->Send((uint8_t*)"Knob \r\n", 10);

        UNUSED(menuSystem);
    }

    void SelectionUp(IMenuSystem* menuSystem)
    {
        if (_menuItem[_menuSelectionIndex] == nullptr)
        {
            return;
        }

        if (_popUpParameterMenuActive >= 0)
        {
            _popUpParameterMenu.SetHighlighted(_popUpParameterMenu.GetHighlightIndex() + 1);
        } else
        {
            // Remove highlighting from last selected item as new button event occured
            _menuItem[_menuSelectionIndex]->SetHighlighted(false);

            _menuSelectionIndex--;
            _menuSelectionIndex = LimitRange(_menuSelectionIndex, 0, _menuItemsCount - 1);
            _menuItem[_menuSelectionIndex]->SetHighlighted(true);
        }
    }

    void SelectionDown(IMenuSystem* menuSystem)
    {
        if (_menuItem[_menuSelectionIndex] == nullptr)
        {
            return;
        }

        if (_popUpParameterMenuActive >= 0)
        {
            _popUpParameterMenu.SetHighlighted(_popUpParameterMenu.GetHighlightIndex() - 1);
        } else
        {
            // Remove highlighting from last selected item as new button event occured
            _menuItem[_menuSelectionIndex]->SetHighlighted(false);

            _menuSelectionIndex++;
            _menuSelectionIndex = LimitRange(_menuSelectionIndex, 0, _menuItemsCount - 1);
            _menuItem[_menuSelectionIndex]->SetHighlighted(true);
        }
    }

    void SelectionPress(IMenuSystem* menuSystem)
    {
        if (_menuItem[_menuSelectionIndex] == nullptr)
        {
            return;
        }

        if (_popUpParameterMenuActive >= 0)
        {
            _popUpParameterMenuActive = -1;

            PopUpMenuItem* currentPopUpMenuItem = (PopUpMenuItem*)_menuItem[_menuSelectionIndex];
            currentPopUpMenuItem->UpdateChoice(_popUpParameterMenu.GetHighlightIndex());

            _popUpParameterMenu.SetPressed(_popUpParameterMenu.GetHighlightIndex());
        } else
        {
            _menuItem[_menuSelectionIndex]->SetPressed(false);
            if (_menuItem[_menuSelectionIndex]->GetMenuType() == MenuItemType::MENU_ITEM_TYPE_CHECKBOX)
            {
                _menuItem[_menuSelectionIndex]->ExecuteAction(menuSystem);
            } else if (_menuItem[_menuSelectionIndex]->GetMenuType() == MenuItemType::MENU_ITEM_TYPE_SCREENLINK)
            {
                _menuItem[_menuSelectionIndex]->ExecuteAction(menuSystem);
            } else if (_menuItem[_menuSelectionIndex]->GetMenuType() == MenuItemType::MENU_ITEM_TYPE_DROPDOWN)
            {
                PopUpMenuItem* currentPopUpMenuItem = (PopUpMenuItem*)_menuItem[_menuSelectionIndex];
                const char* choices[7];
                for (uint8_t i = 0; i < currentPopUpMenuItem->GetChoiceCount(); i++)
                {
                    choices[i] = currentPopUpMenuItem->GetChoice(i);
                }
                _popUpParameterMenu.SetChoices(choices, currentPopUpMenuItem->GetChoiceCount());
                _popUpParameterMenu.SetHighlighted(currentPopUpMenuItem->GetChoiceIndex());
                int8_t displaySelectionIndex = _menuSelectionIndex - _menuOffset;
                _popUpParameterMenuActive = _menuSelectionIndex;
                _popUpParameterMenu.SetPosition(200, 29 + (displaySelectionIndex + 1) * 30);
            }
        }
    }
};

#endif // MENU_H
