#pragma once

#include "Windows.h"

#include <iostream>

namespace cs
{

    using std::string;
    using Uint32 = unsigned int;
    using Uint8 = unsigned char;

    class color
    {
    private:
        Uint32 _color;

    public:
        color()
        {
            this->_color = 0xffffff;
        }

        explicit color(const Uint32& value)
        {
            this->_color = value;
        }

        color(const Uint8& r, const Uint8& g, const Uint8& b)
        {
            this->_color = r << 16 | g << 8 | b << 0;
        }

    public:
        Uint32 raw() const
        {
            auto* c = (Uint8*) &_color;

            Uint32 temp_color = c[0] << 16 | c[1] << 8 | c[2];

            return temp_color;
        }

        Uint8& r() const
        {
            auto color_part = (Uint8*) (&_color);
            return color_part[3];
        }

        Uint8& g() const
        {
            auto color_part = (Uint8*) (&_color);
            return color_part[2];
        }

        Uint8& b() const
        {
            auto color_part = (Uint8*) (&_color);
            return color_part[1];
        }
    };


    bool selected_colors[] = {true, false, false, false, false, false, false, true, false, false, false, false, false
                              , false, false, false
    };


    class color_string
    {
        string text;
        color text_color;
        color_string* next;

    public:
        color_string() : color_string("")
        {
        }

        explicit color_string(const string& str)
        {
            this->text = str;
            this->text_color = color(0xffffff);
            this->next = nullptr;
        }

        explicit color_string(const char* str)
        {
            this->text = string(str);
            this->text_color = color(0xffffff);
            this->next = nullptr;
        }

    public:
        static int set_color(const color& _color)
        {
            CONSOLE_SCREEN_BUFFER_INFOEX info;
            info.cbSize = sizeof(info);
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            GetConsoleScreenBufferInfoEx(hConsole, &info);

            int selected = 1;

            for (int i = 0; i < 16; i++)
            {
                if (!selected_colors[i])
                {
                    selected = i;
                    selected_colors[i] = true;
                    break;
                }
            }

            const COLORREF colorref = _color.raw();
            info.ColorTable[selected] = colorref;
            SetConsoleScreenBufferInfoEx(hConsole, &info);
            return selected;
        }

        color_string& operator<<(const string& str)
        {
            text += str;
            return *this;
        }

        color_string& use(const color& _color)
        {
            this->text_color = _color;
            return *this;
        }

        color_string& operator<<(const color& _color)
        {
            return use(_color);
        }

        color_string& concat(color_string& _color_color)
        {
            if (this->next == nullptr)
            {
                this->next = &_color_color;
            }
            else
            {
                this->next->concat(_color_color);
            }

            return *this;
        }

        color_string& operator<<(color_string& _color_color)
        {
            return concat(_color_color);
        }

        friend std::ostream& print(std::ostream& os, color_string& string)
        {
            int position = set_color(string.text_color);
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(hConsole, position);

            os << string.text;

            if (string.next != nullptr)
            {
                print(os, *string.next);
            }

            SetConsoleTextAttribute(hConsole, 7);

            return os;
        }

        friend std::ostream& operator<<(std::ostream& os, color_string& string)
        {
            return print(os, string);
        }
    };


    color_string operator ""_cs(const char* str, size_t size)
    {
        return color_string(string(str, size));
    }

}