#pragma once
#include <string>
#include <locale.h>
#include <vector>

// Tạo locale chuẩn C (dùng dấu chấm) một lần duy nhất
static _locale_t c_locale = _create_locale(LC_NUMERIC, "C");

// Hàm đọc số thực chuẩn xác
static float ParseNumber(const char*& ptr)
{
    while (*ptr && (*ptr == ' ' || *ptr == ',' || *ptr == '\t' || *ptr == '\n' || *ptr == '\r')) ptr++;

    if (*ptr == '\0') return 0.0f;

    char* endPtr;
    double val = _strtod_l(ptr, &endPtr, c_locale);

    if (ptr == endPtr) return 0.0f;
    ptr = endPtr;
    return (float)val;
}

static void ParseNumberList(const char* s, std::vector<float>& numbers)
{
    const char* ptr = s;
    while (*ptr) {
        while (*ptr && !isdigit(*ptr) && *ptr != '-' && *ptr != '.' && *ptr != '+') ptr++;
        if (!*ptr) break;

        numbers.push_back(ParseNumber(ptr));
    }
}

// Hàm đọc đơn vị (cho Gradient)
static float ParseUnit(const std::string& str, float defaultVal = 0.0f)
{
    if (str.empty()) return defaultVal;

    const char* ptr = str.c_str();
    float val = ParseNumber(ptr);

    // Kiểm tra hậu tố đơn vị
    std::string unit = ptr;

    // Xử lý đơn vị
    if (unit == "px") return val;
    if (unit == "pt") return val * 1.3333f;
    if (unit == "pc") return val * 16.0f;
    if (unit == "mm") return val * 3.7795f;
    if (unit == "cm") return val * 37.795f;
    if (unit == "in") return val * 96.0f;
    if (unit == "%") return val;

    return val;
}