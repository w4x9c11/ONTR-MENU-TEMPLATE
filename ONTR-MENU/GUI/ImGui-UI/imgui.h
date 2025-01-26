// dear imgui, v1.91.7 WIP
// (headers)

// Help:
// - See links below.
// - Call and read ImGui::ShowDemoWindow() in imgui_demo.cpp. All applications in examples/ are doing that.
// - Read top of imgui.cpp for more details, links and comments.
// - Add '#define IMGUI_DEFINE_MATH_OPERATORS' before including this file (or in imconfig.h) to access courtesy maths operators for ImVec2 and ImVec4.

// Resources:
// - FAQ ........................ https://dearimgui.com/faq (in repository as docs/FAQ.md)
// - Homepage ................... https://github.com/ocornut/imgui
// - Releases & changelog ....... https://github.com/ocornut/imgui/releases
// - Gallery .................... https://github.com/ocornut/imgui/issues?q=label%3Agallery (please post your screenshots/video there!)
// - Wiki ....................... https://github.com/ocornut/imgui/wiki (lots of good stuff there)
//   - Getting Started            https://github.com/ocornut/imgui/wiki/Getting-Started (how to integrate in an existing app by adding ~25 lines of code)
//   - Third-party Extensions     https://github.com/ocornut/imgui/wiki/Useful-Extensions (ImPlot & many more)
//   - Bindings/Backends          https://github.com/ocornut/imgui/wiki/Bindings (language bindings, backends for various tech/engines)
//   - Glossary                   https://github.com/ocornut/imgui/wiki/Glossary
//   - Debug Tools                https://github.com/ocornut/imgui/wiki/Debug-Tools
//   - Software using Dear ImGui  https://github.com/ocornut/imgui/wiki/Software-using-dear-imgui
// - Issues & support ........... https://github.com/ocornut/imgui/issues
// - Test Engine & Automation ... https://github.com/ocornut/imgui_test_engine (test suite, test engine to automate your apps)

// For first-time users having issues compiling/linking/running/loading fonts:
// please post in https://github.com/ocornut/imgui/discussions if you cannot find a solution in resources above.
// Everything else should be asked in 'Issues'! We are building a database of cross-linked knowledge there.

// Library Version
// (Integer encoded as XYYZZ for use in #if preprocessor conditionals, e.g. '#if IMGUI_VERSION_NUM >= 12345')
#define IMGUI_VERSION       "1.91.7 WIP"
#define IMGUI_VERSION_NUM   19164
#define IMGUI_HAS_TABLE
#define IMGUI_HAS_VIEWPORT          // Viewport WIP branch
#define IMGUI_HAS_DOCK              // Docking WIP branch

/*

Index of this file:
// [SECTION] Header mess
// [SECTION] Forward declarations and basic types
// [SECTION] Dear ImGui end-user API functions
// [SECTION] Flags & Enumerations
// [SECTION] Tables API flags and structures (ImGuiTableFlags, ImGuiTableColumnFlags, ImGuiTableRowFlags, ImGuiTableBgTarget, ImGuiTableSortSpecs, ImGuiTableColumnSortSpecs)
// [SECTION] Helpers: Debug log, Memory allocations macros, ImVector<>
// [SECTION] ImGuiStyle
// [SECTION] ImGuiIO
// [SECTION] Misc data structures (ImGuiInputTextCallbackData, ImGuiSizeCallbackData, ImGuiWindowClass, ImGuiPayload)
// [SECTION] Helpers (ImGuiOnceUponAFrame, ImGuiTextFilter, ImGuiTextBuffer, ImGuiStorage, ImGuiListClipper, Math Operators, ImColor)
// [SECTION] Multi-Select API flags and structures (ImGuiMultiSelectFlags, ImGuiMultiSelectIO, ImGuiSelectionRequest, ImGuiSelectionBasicStorage, ImGuiSelectionExternalStorage)
// [SECTION] Drawing API (ImDrawCallback, ImDrawCmd, ImDrawIdx, ImDrawVert, ImDrawChannel, ImDrawListSplitter, ImDrawFlags, ImDrawListFlags, ImDrawList, ImDrawData)
// [SECTION] Font API (ImFontConfig, ImFontGlyph, ImFontGlyphRangesBuilder, ImFontAtlasFlags, ImFontAtlas, ImFont)
// [SECTION] Viewports (ImGuiViewportFlags, ImGuiViewport)
// [SECTION] ImGuiPlatformIO + other Platform Dependent Interfaces (ImGuiPlatformMonitor, ImGuiPlatformImeData)
// [SECTION] Obsolete functions and types

*/

#pragma once

// Configuration file with compile-time options
// (edit imconfig.h or '#define IMGUI_USER_CONFIG "myfilename.h" from your build system)
#ifdef IMGUI_USER_CONFIG
#include IMGUI_USER_CONFIG
#endif
#include "imconfig.h"

#ifndef IMGUI_DISABLE

//-----------------------------------------------------------------------------
// [SECTION] Header mess
//-----------------------------------------------------------------------------

// Includes
#include <float.h>                  // FLT_MIN, FLT_MAX
#include <stdarg.h>                 // va_list, va_start, va_end
#include <stddef.h>                 // ptrdiff_t, NULL
#include <string.h>                 // memset, memmove, memcpy, strlen, strchr, strcpy, strcmp

// Define attributes of all API symbols declarations (e.g. for DLL under Windows)
// IMGUI_API is used for core imgui functions, IMGUI_IMPL_API is used for the default backends files (imgui_impl_xxx.h)
// Using dear imgui via a shared library is not recommended: we don't guarantee backward nor forward ABI compatibility + this is a call-heavy library and function call overhead adds up.
#ifndef IMGUI_API
#define IMGUI_API
#endif
#ifndef IMGUI_IMPL_API
#define IMGUI_IMPL_API              IMGUI_API
#endif

// Helper Macros
#ifndef IM_ASSERT
#include <assert.h>
#define IM_ASSERT(_EXPR)            assert(_EXPR)                               // You can override the default assert handler by editing imconfig.h
#endif
#define IM_ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR) / sizeof(*(_ARR))))     // Size of a static C-style array. Don't use on pointers!
#define IM_UNUSED(_VAR)             ((void)(_VAR))                              // Used to silence "unused variable warnings". Often useful as asserts may be stripped out from final builds.

// Check that version and structures layouts are matching between compiled imgui code and caller. Read comments above DebugCheckVersionAndDataLayout() for details.
#define IMGUI_CHECKVERSION()        ImGui::DebugCheckVersionAndDataLayout(IMGUI_VERSION, sizeof(ImGuiIO), sizeof(ImGuiStyle), sizeof(ImVec2), sizeof(ImVec4), sizeof(ImDrawVert), sizeof(ImDrawIdx))

// Helper Macros - IM_FMTARGS, IM_FMTLIST: Apply printf-style warnings to our formatting functions.
// (MSVC provides an equivalent mechanism via SAL Annotations but it would require the macros in a different
//  location. e.g. #include <sal.h> + void myprintf(_Printf_format_string_ const char* format, ...))
#if !defined(IMGUI_USE_STB_SPRINTF) && defined(__MINGW32__) && !defined(__clang__)
#define IM_FMTARGS(FMT)             __attribute__((format(gnu_printf, FMT, FMT+1)))
#define IM_FMTLIST(FMT)             __attribute__((format(gnu_printf, FMT, 0)))
#elif !defined(IMGUI_USE_STB_SPRINTF) && (defined(__clang__) || defined(__GNUC__))
#define IM_FMTARGS(FMT)             __attribute__((format(printf, FMT, FMT+1)))
#define IM_FMTLIST(FMT)             __attribute__((format(printf, FMT, 0)))
#else
#define IM_FMTARGS(FMT)
#define IM_FMTLIST(FMT)
#endif

// Disable some of MSVC most aggressive Debug runtime checks in function header/footer (used in some simple/low-level functions)
#if defined(_MSC_VER) && !defined(__clang__)  && !defined(__INTEL_COMPILER) && !defined(IMGUI_DEBUG_PARANOID)
#define IM_MSVC_RUNTIME_CHECKS_OFF      __pragma(runtime_checks("",off))     __pragma(check_stack(off)) __pragma(strict_gs_check(push,off))
#define IM_MSVC_RUNTIME_CHECKS_RESTORE  __pragma(runtime_checks("",restore)) __pragma(check_stack())    __pragma(strict_gs_check(pop))
#else
#define IM_MSVC_RUNTIME_CHECKS_OFF
#define IM_MSVC_RUNTIME_CHECKS_RESTORE
#endif

// Warnings
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 26495)    // [Static Analyzer] Variable 'XXX' is uninitialized. Always initialize a member variable (type.6).
#endif
#if defined(__clang__)
#pragma clang diagnostic push
#if __has_warning("-Wunknown-warning-option")
#pragma clang diagnostic ignored "-Wunknown-warning-option"         // warning: unknown warning group 'xxx'
#endif
#pragma clang diagnostic ignored "-Wunknown-pragmas"                // warning: unknown warning group 'xxx'
#pragma clang diagnostic ignored "-Wold-style-cast"                 // warning: use of old-style cast
#pragma clang diagnostic ignored "-Wfloat-equal"                    // warning: comparing floating point with == or != is unsafe
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"  // warning: zero as null pointer constant
#pragma clang diagnostic ignored "-Wreserved-identifier"            // warning: identifier '_Xxx' is reserved because it starts with '_' followed by a capital letter
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"            // warning: 'xxx' is an unsafe pointer used for buffer access
#pragma clang diagnostic ignored "-Wnontrivial-memaccess"           // warning: first argument in call to 'memset' is a pointer to non-trivially copyable type
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"                          // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wfloat-equal"                      // warning: comparing floating-point with '==' or '!=' is unsafe
#pragma GCC diagnostic ignored "-Wclass-memaccess"                  // [__GNUC__ >= 8] warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#endif

//-----------------------------------------------------------------------------
// [SECTION] Forward declarations and basic types
//-----------------------------------------------------------------------------

// Scalar data types
typedef unsigned int        ImGuiID;// A unique ID used by widgets (typically the result of hashing a stack of string)
typedef signed char         ImS8;   // 8-bit signed integer
typedef unsigned char       ImU8;   // 8-bit unsigned integer
typedef signed short        ImS16;  // 16-bit signed integer
typedef unsigned short      ImU16;  // 16-bit unsigned integer
typedef signed int          ImS32;  // 32-bit signed integer == int
typedef unsigned int        ImU32;  // 32-bit unsigned integer (often used to store packed colors)
typedef signed   long long  ImS64;  // 64-bit signed integer
typedef unsigned long long  ImU64;  // 64-bit unsigned integer

// Forward declarations
struct ImDrawChannel;                 // 临时存储，用于无序输出绘制命令，由 ImDrawListSplitter 和 ImDrawList::ChannelsSplit() 使用。
struct ImDrawCmd;                     // 父 ImDrawList 中的单个绘制命令（通常映射到一次 GPU 绘制调用，除非是回调）。
struct ImDrawData;                    // 当前帧需要渲染的所有绘制命令列表 + 用于投影矩阵的位置信息和大小坐标。
struct ImDrawList;                    // 单个绘制命令列表（通常每个窗口一个，可看作动态“网格”生成器）。
struct ImDrawListSharedData;          // 多个绘制列表间共享的数据（通常由父 ImGui 上下文拥有，但也可自行创建）。
struct ImDrawListSplitter;            // 帮助将绘制列表分割为不同层，可无序绘制后再合并。
struct ImDrawVert;                    // 单个顶点（默认布局为 pos + uv + col = 20 字节，可用 IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT 覆盖布局）。
struct ImFont;                        // 单个字体的运行时数据，属于父 ImFontAtlas。
struct ImFontAtlas;                   // 多字体的运行时数据，支持将多个字体烘焙到一个纹理中，包含 TTF/OTF 字体加载器。
struct ImFontBuilderIO;               // 字体生成器的不透明接口（如 stb_truetype 或 FreeType）。
struct ImFontConfig;                  // 添加或合并字体时的配置数据。
struct ImFontGlyph;                   // 单个字体字形（代码点 + ImFontAtlas 中的坐标 + 偏移量）。
struct ImFontGlyphRangesBuilder;      // 从文本/字符串数据构建字形范围的辅助工具。
struct ImColor;                       // 创建颜色的辅助函数，可转换为 u32 或 float4（*过时*，请避免使用）。
struct ImGuiContext;                  // Dear ImGui 上下文（不透明结构，除非包含 imgui_internal.h）。
struct ImGuiIO;                       // 应用程序与 ImGui 之间的主要配置和输入/输出接口（另见：ImGuiPlatformIO）。
struct ImGuiInputTextCallbackData;    // 使用自定义 ImGuiInputTextCallback 时的 InputText() 共享状态（少见/高级用法）。
struct ImGuiKeyData;                  // 用于存储按键状态（与 IsKeyDown()、IsKeyPressed() 等函数一起使用）。
struct ImGuiListClipper;              // 帮助手动裁剪大型列表项。
struct ImGuiMultiSelectIO;            // 用于在 BeginMultiSelect()/EndMultiSelect() 块中交互的结构。
struct ImGuiOnceUponAFrame;           // 帮助代码块每帧运行一次。
struct ImGuiPayload;                  // 拖放操作的用户数据负载。
struct ImGuiPlatformIO;               // 平台/渲染器后端与 ImGui 的接口（如剪贴板、输入法、Multi-Viewport 支持）。扩展自 ImGuiIO。
struct ImGuiPlatformImeData;          // 用于 io.PlatformSetImeDataFn() 函数的输入法数据。
struct ImGuiPlatformMonitor;          // 多视口支持：用户提供的每个连接显示器/屏幕的边界。用于在定位弹出窗口和工具提示时避免跨越显示器。
struct ImGuiSelectionBasicStorage;    // 可选辅助工具，用于存储多选状态并应用多选请求。
struct ImGuiSelectionExternalStorage; // 可选辅助工具，用于将多选请求应用于现有随机可访问存储。
struct ImGuiSelectionRequest;         // 多选请求（存储在 ImGuiMultiSelectIO 中）。
struct ImGuiSizeCallbackData;         // 使用 SetNextWindowSizeConstraints() 时的回调数据（少见/高级用法）。
struct ImGuiStorage;                  // 键值对存储的辅助工具（按键排序的容器）。
struct ImGuiStoragePair;              // 键值对存储的辅助工具（键值对）。
struct ImGuiStyle;                    // 样式/颜色的运行时数据。
struct ImGuiTableSortSpecs;           // 表格的排序规范（通常处理单列的排序规范，有时会处理多列）。
struct ImGuiTableColumnSortSpecs;     // 表格单列的排序规范。
struct ImGuiTextBuffer;               // 用于保存和追加到文本缓冲区的辅助工具（类似字符串构建器）。
struct ImGuiTextFilter;               // 用于解析和应用文本过滤器的辅助工具（如 "aaaaa[,bbbbb][,ccccc]"）。
struct ImGuiViewport;                 // 一个平台窗口（除非启用了多视口，否则始终为 1。每个平台窗口对应一个视口）。未来可能表示平台监视器。
struct ImGuiWindowClass;              // 窗口类（少见/高级用法：通过更改视口标志和父/子信息向平台后端提供提示）。

// Enumerations
// 不常使用强类型枚举，因为它们会增加限制（不能在私有代码中扩展，不能将类型存储在位域中，迭代时需要额外的类型转换）。
// 提示：使用 IDE 的导航功能查找下面中间列的名称以获取实际的标志/枚举列表：
// Visual Studio：按 CTRL+逗号 ("Edit.GoToAll") 可跟随注释中的符号，CTRL+F12 ("Edit.GoToImplementation") 不行。
// Visual Studio + Visual Assist：按 ALT+G ("VAssistX.GoToImplementation") 可跟随注释中的符号。
// VS Code/CLion 等：按住 CTRL 并点击注释中的符号即可。
enum ImGuiDir : int;            // -> enum ImGuiDir              // 枚举：表示一个基本方向（左、右、上、下）
enum ImGuiKey : int;            // -> enum ImGuiKey              // 枚举：按键标识符（ImGuiKey_XXX 或 ImGuiMod_XXX）
enum ImGuiMouseSource : int;    // -> enum ImGuiMouseSource      // 枚举：鼠标输入源标识符（鼠标、触摸屏、笔）
enum ImGuiSortDirection : ImU8; // -> enum ImGuiSortDirection    // 枚举：排序方向（升序或降序）
typedef int ImGuiCol;           // -> enum ImGuiCol_             // 枚举：用于样式的颜色标识符
typedef int ImGuiCond;          // -> enum ImGuiCond_            // 枚举：用于许多 `Set*()` 函数的条件标识符
typedef int ImGuiDataType;      // -> enum ImGuiDataType_        // 枚举：主要数据类型
typedef int ImGuiMouseButton;   // -> enum ImGuiMouseButton_     // 枚举：鼠标按键标识符（0=左键，1=右键，2=中键）
typedef int ImGuiMouseCursor;   // -> enum ImGuiMouseCursor_     // 枚举：鼠标光标形状
typedef int ImGuiStyleVar;      // -> enum ImGuiStyleVar_        // 枚举：用于样式的变量标识符
typedef int ImGuiTableBgTarget; // -> enum ImGuiTableBgTarget_   // 枚举：表格背景颜色的目标标识符

// （声明为 int 类型，方便用作标志位且无额外开销，同时避免污染文件顶部命名空间）
// 提示：使用 IDE 导航功能查找下面中间列名称，获取实际的标志/枚举列表：
// Visual Studio：按 CTRL+逗号 ("Edit.GoToAll") 可以跳转注释中的符号，CTRL+F12 ("Edit.GoToImplementation") 不行。
// Visual Studio + Visual Assist：按 ALT+G ("VAssistX.GoToImplementation") 可以跳转注释中的符号。
// VS Code/CLion 等：按住 CTRL 并点击注释中的符号即可。
typedef int ImDrawFlags;           // -> enum ImDrawFlags_          // 标志：用于 ImDrawList 函数
typedef int ImDrawListFlags;       // -> enum ImDrawListFlags_      // 标志：用于 ImDrawList 实例
typedef int ImFontAtlasFlags;      // -> enum ImFontAtlasFlags_     // 标志：用于 ImFontAtlas 构建
typedef int ImGuiBackendFlags;     // -> enum ImGuiBackendFlags_    // 标志：用于 io.BackendFlags
typedef int ImGuiButtonFlags;      // -> enum ImGuiButtonFlags_     // 标志：用于 InvisibleButton()
typedef int ImGuiChildFlags;       // -> enum ImGuiChildFlags_      // 标志：用于 BeginChild()
typedef int ImGuiColorEditFlags;   // -> enum ImGuiColorEditFlags_  // 标志：用于 ColorEdit4()、ColorPicker4() 等
typedef int ImGuiConfigFlags;      // -> enum ImGuiConfigFlags_     // 标志：用于 io.ConfigFlags
typedef int ImGuiComboFlags;       // -> enum ImGuiComboFlags_      // 标志：用于 BeginCombo()
typedef int ImGuiDockNodeFlags;    // -> enum ImGuiDockNodeFlags_   // 标志：用于 DockSpace()
typedef int ImGuiDragDropFlags;    // -> enum ImGuiDragDropFlags_   // 标志：用于 BeginDragDropSource()、AcceptDragDropPayload()
typedef int ImGuiFocusedFlags;     // -> enum ImGuiFocusedFlags_    // 标志：用于 IsWindowFocused()
typedef int ImGuiHoveredFlags;     // -> enum ImGuiHoveredFlags_    // 标志：用于 IsItemHovered()、IsWindowHovered() 等
typedef int ImGuiInputFlags;       // -> enum ImGuiInputFlags_      // 标志：用于 Shortcut()、SetNextItemShortcut()
typedef int ImGuiInputTextFlags;   // -> enum ImGuiInputTextFlags_  // 标志：用于 InputText()、InputTextMultiline()
typedef int ImGuiItemFlags;        // -> enum ImGuiItemFlags_       // 标志：用于 PushItemFlag()，由所有项共享
typedef int ImGuiKeyChord;         // -> ImGuiKey | ImGuiMod_XXX    // 标志：用于 IsKeyChordPressed()、Shortcut() 等，可选地与一个或多个 ImGuiMod_XXX 组合
typedef int ImGuiPopupFlags;       // -> enum ImGuiPopupFlags_      // 标志：用于 OpenPopup*()、BeginPopupContext*()、IsPopupOpen()
typedef int ImGuiMultiSelectFlags; // -> enum ImGuiMultiSelectFlags_// 标志：用于 BeginMultiSelect()
typedef int ImGuiSelectableFlags;  // -> enum ImGuiSelectableFlags_ // 标志：用于 Selectable()
typedef int ImGuiSliderFlags;      // -> enum ImGuiSliderFlags_     // 标志：用于 DragFloat()、DragInt()、SliderFloat()、SliderInt() 等
typedef int ImGuiTabBarFlags;      // -> enum ImGuiTabBarFlags_     // 标志：用于 BeginTabBar()
typedef int ImGuiTabItemFlags;     // -> enum ImGuiTabItemFlags_    // 标志：用于 BeginTabItem()
typedef int ImGuiTableFlags;       // -> enum ImGuiTableFlags_      // 标志：用于 BeginTable()
typedef int ImGuiTableColumnFlags; // -> enum ImGuiTableColumnFlags_// 标志：用于 TableSetupColumn()
typedef int ImGuiTableRowFlags;    // -> enum ImGuiTableRowFlags_   // 标志：用于 TableNextRow()
typedef int ImGuiTreeNodeFlags;    // -> enum ImGuiTreeNodeFlags_   // 标志：用于 TreeNode()、TreeNodeEx()、CollapsingHeader()
typedef int ImGuiViewportFlags;    // -> enum ImGuiViewportFlags_   // 标志：用于 ImGuiViewport
typedef int ImGuiWindowFlags;      // -> enum ImGuiWindowFlags_     // 标志：用于 Begin()、BeginChild()

// ImTexture: user data for renderer backend to identify a texture [Compile-time configurable type]
// - To use something else than an opaque void* pointer: override with e.g. '#define ImTextureID MyTextureType*' in your imconfig.h file.
// - This can be whatever to you want it to be! read the FAQ about ImTextureID for details.
// - You can make this a structure with various constructors if you need. You will have to implement ==/!= operators.
// - (note: before v1.91.4 (2024/10/08) the default type for ImTextureID was void*. Use intermediary intptr_t cast and read FAQ if you have casting warnings)
#ifndef ImTextureID
typedef ImU64 ImTextureID;          // Default: store a pointer or an integer fitting in a pointer (most renderer backends are ok with that)
#endif

// ImDrawIdx: vertex index. [Compile-time configurable type]
// - To use 16-bit indices + allow large meshes: backend need to set 'io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset' and handle ImDrawCmd::VtxOffset (recommended).
// - To use 32-bit indices: override with '#define ImDrawIdx unsigned int' in your imconfig.h file.
#ifndef ImDrawIdx
typedef unsigned short ImDrawIdx;   // Default: 16-bit (for maximum compatibility with renderer backends)
#endif

// Character types
// (we generally use UTF-8 encoded string in the API. This is storage specifically for a decoded character used for keyboard input and display)
typedef unsigned int ImWchar32;     // A single decoded U32 character/code point. We encode them as multi bytes UTF-8 when used in strings.
typedef unsigned short ImWchar16;   // A single decoded U16 character/code point. We encode them as multi bytes UTF-8 when used in strings.
#ifdef IMGUI_USE_WCHAR32            // ImWchar [configurable type: override in imconfig.h with '#define IMGUI_USE_WCHAR32' to support Unicode planes 1-16]
typedef ImWchar32 ImWchar;
#else
typedef ImWchar16 ImWchar;
#endif

// Multi-Selection item index or identifier when using BeginMultiSelect()
// - Used by SetNextItemSelectionUserData() + and inside ImGuiMultiSelectIO structure.
// - Most users are likely to use this store an item INDEX but this may be used to store a POINTER/ID as well. Read comments near ImGuiMultiSelectIO for details.
typedef ImS64 ImGuiSelectionUserData;

// Callback and functions types
typedef int     (*ImGuiInputTextCallback)(ImGuiInputTextCallbackData* data);    // Callback function for ImGui::InputText()
typedef void    (*ImGuiSizeCallback)(ImGuiSizeCallbackData* data);              // Callback function for ImGui::SetNextWindowSizeConstraints()
typedef void*   (*ImGuiMemAllocFunc)(size_t sz, void* user_data);               // Function signature for ImGui::SetAllocatorFunctions()
typedef void    (*ImGuiMemFreeFunc)(void* ptr, void* user_data);                // Function signature for ImGui::SetAllocatorFunctions()

// ImVec2: 2D vector used to store positions, sizes etc. [Compile-time configurable type]
// - This is a frequently used type in the API. Consider using IM_VEC2_CLASS_EXTRA to create implicit cast from/to our preferred type.
// - Add '#define IMGUI_DEFINE_MATH_OPERATORS' before including this file (or in imconfig.h) to access courtesy maths operators for ImVec2 and ImVec4.
IM_MSVC_RUNTIME_CHECKS_OFF
struct ImVec2
{
    float                                   x, y;
    constexpr ImVec2()                      : x(0.0f), y(0.0f) { }
    constexpr ImVec2(float _x, float _y)    : x(_x), y(_y) { }
    float& operator[] (size_t idx)          { IM_ASSERT(idx == 0 || idx == 1); return ((float*)(void*)(char*)this)[idx]; } // We very rarely use this [] operator, so the assert overhead is fine.
    float  operator[] (size_t idx) const    { IM_ASSERT(idx == 0 || idx == 1); return ((const float*)(const void*)(const char*)this)[idx]; }
#ifdef IM_VEC2_CLASS_EXTRA
    IM_VEC2_CLASS_EXTRA     // Define additional constructors and implicit cast operators in imconfig.h to convert back and forth between your math types and ImVec2.
#endif
};

// ImVec4: 4D vector used to store clipping rectangles, colors etc. [Compile-time configurable type]
struct ImVec4
{
    float                                                     x, y, z, w;
    constexpr ImVec4()                                        : x(0.0f), y(0.0f), z(0.0f), w(0.0f) { }
    constexpr ImVec4(float _x, float _y, float _z, float _w)  : x(_x), y(_y), z(_z), w(_w) { }
#ifdef IM_VEC4_CLASS_EXTRA
    IM_VEC4_CLASS_EXTRA     // Define additional constructors and implicit cast operators in imconfig.h to convert back and forth between your math types and ImVec4.
#endif
};
IM_MSVC_RUNTIME_CHECKS_RESTORE

//-----------------------------------------------------------------------------
// [SECTION] Dear ImGui end-user API functions
// (Note that ImGui:: being a namespace, you can add extra ImGui:: functions in your own separate file. Please don't modify imgui source files!)
//-----------------------------------------------------------------------------

namespace ImGui
{
    // Context creation and access
    // - Each context create its own ImFontAtlas by default. You may instance one yourself and pass it to CreateContext() to share a font atlas between contexts.
    // - DLL users: heaps and globals are not shared across DLL boundaries! You will need to call SetCurrentContext() + SetAllocatorFunctions()
    //   for each static/DLL boundary you are calling from. Read "Context and Memory Allocators" section of imgui.cpp for details.
    // Context 管理
    IMGUI_API ImGuiContext *CreateContext(ImFontAtlas *shared_font_atlas = NULL); // 创建一个新的 ImGui 上下文，可以共享一个字体图集（可选）。
    IMGUI_API void DestroyContext(ImGuiContext *ctx = NULL);                      // 销毁指定的 ImGui 上下文，默认销毁当前上下文。
    IMGUI_API ImGuiContext *GetCurrentContext();                                  // 获取当前的 ImGui 上下文。
    IMGUI_API void SetCurrentContext(ImGuiContext *ctx);                          // 设置当前的 ImGui 上下文。

    // 主功能
    IMGUI_API ImGuiIO &GetIO();                 // 访问 ImGuiIO 结构体（鼠标/键盘/手柄输入、时间、各种配置选项/标志）。
    IMGUI_API ImGuiPlatformIO &GetPlatformIO(); // 访问 ImGuiPlatformIO 结构体（主要是与平台/渲染器和操作系统剪贴板、输入法等连接的钩子/函数）。
    IMGUI_API ImGuiStyle &GetStyle();           // 访问样式结构体（颜色、尺寸等）。在帧中动态修改样式时请使用 PushStyleColor() 或 PushStyleVar()。
    IMGUI_API void NewFrame();                  // 开始新的 ImGui 帧。从此时起可以提交任何命令，直到调用 Render()/EndFrame()。
    IMGUI_API void EndFrame();                  // 结束 ImGui 帧，Render() 会自动调用。如果不需要渲染数据，可以调用 EndFrame() 而不调用 Render()，但 CPU 资源可能已经  浪费。如果不需要渲染，最好不创建窗口且不调用 NewFrame()。
    IMGUI_API void Render();                    // 结束 ImGui 帧，生成最终的绘制数据。之后可以调用 GetDrawData() 获取数据。
    IMGUI_API ImDrawData *GetDrawData();        // 在 Render() 调用后有效，直到下一次调用 NewFrame()。这是需要渲染的数据。

    // 示例、调试和信息
    IMGUI_API void ShowDemoWindow(bool *p_open = NULL);        // 创建演示窗口，展示大多数 ImGui 功能。调用此方法了解库功能，建议始终在应用程序中保留此功能。
    IMGUI_API void ShowMetricsWindow(bool *p_open = NULL);     // 创建指标/调试器窗口，显示 ImGui 内部状态：窗口、绘制命令、各种内部状态等。
    IMGUI_API void ShowDebugLogWindow(bool *p_open = NULL);    // 创建调试日志窗口，显示简化的 ImGui 事件日志。
    IMGUI_API void ShowIDStackToolWindow(bool *p_open = NULL); // 创建堆栈工具窗口，鼠标悬停项目以查询其唯一 ID 的来源信息。
    IMGUI_API void ShowAboutWindow(bool *p_open = NULL);       // 创建关于窗口，显示 ImGui 版本、贡献者及构建/系统信息。
    IMGUI_API void ShowStyleEditor(ImGuiStyle *ref = NULL);    // 添加样式编辑器块（不是窗口），可传入参考样式用于对比、恢复和保存（默认使用当前样式）。
    IMGUI_API bool ShowStyleSelector(const char *label);       // 添加样式选择器块（不是窗口），本质上是一个列出默认样式的下拉菜单。
    IMGUI_API void ShowFontSelector(const char *label);        // 添加字体选择器块（不是窗口），本质上是一个列出已加载字体的下拉菜单。
    IMGUI_API void ShowUserGuide();                            // 添加基础帮助/信息块（不是窗口）：如何作为终端用户操作 ImGui（鼠标/键盘控制）。
    IMGUI_API const char *GetVersion();                        // 获取编译版本的字符串，例如 "1.80 WIP"（本质上是 imgui.cpp 中定义的 IMGUI_VERSION）。

    // 样式
    IMGUI_API void StyleColorsDark(ImGuiStyle *dst = NULL);    // 新的推荐样式（默认）。
    IMGUI_API void StyleColorsLight(ImGuiStyle *dst = NULL);   // 最适合使用边框和自定义、较粗字体。
    IMGUI_API void StyleColorsClassic(ImGuiStyle *dst = NULL); // 经典的 ImGui 样式。

    // Windows
    // - Begin() = push window to the stack and start appending to it. End() = pop window from the stack.
    // - Passing 'bool* p_open != NULL' shows a window-closing widget in the upper-right corner of the window,
    //   which clicking will set the boolean to false when clicked.
    // - You may append multiple times to the same window during the same frame by calling Begin()/End() pairs multiple times.
    //   Some information such as 'flags' or 'p_open' will only be considered by the first call to Begin().
    // - Begin() return false to indicate the window is collapsed or fully clipped, so you may early out and omit submitting
    //   anything to the window. Always call a matching End() for each Begin() call, regardless of its return value!
    //   [Important: due to legacy reason, Begin/End and BeginChild/EndChild are inconsistent with all other functions
    //    such as BeginMenu/EndMenu, BeginPopup/EndPopup, etc. where the EndXXX call should only be called if the corresponding
    //    BeginXXX function returned true. Begin and BeginChild are the only odd ones out. Will be fixed in a future update.]
    // - Note that the bottom of window stack always contains a window called "Debug".
    IMGUI_API bool          Begin(const char* name, bool* p_open = NULL, ImGuiWindowFlags flags = 0);
    IMGUI_API void          End();

    // Child Windows
    // - Use child windows to begin into a self-contained independent scrolling/clipping regions within a host window. Child windows can embed their own child.
    // - Before 1.90 (November 2023), the "ImGuiChildFlags child_flags = 0" parameter was "bool border = false".
    //   This API is backward compatible with old code, as we guarantee that ImGuiChildFlags_Borders == true.
    //   Consider updating your old code:
    //      BeginChild("Name", size, false)   -> Begin("Name", size, 0); or Begin("Name", size, ImGuiChildFlags_None);
    //      BeginChild("Name", size, true)    -> Begin("Name", size, ImGuiChildFlags_Borders);
    // - Manual sizing (each axis can use a different setting e.g. ImVec2(0.0f, 400.0f)):
    //     == 0.0f: use remaining parent window size for this axis.
    //      > 0.0f: use specified size for this axis.
    //      < 0.0f: right/bottom-align to specified distance from available content boundaries.
    // - Specifying ImGuiChildFlags_AutoResizeX or ImGuiChildFlags_AutoResizeY makes the sizing automatic based on child contents.
    //   Combining both ImGuiChildFlags_AutoResizeX _and_ ImGuiChildFlags_AutoResizeY defeats purpose of a scrolling region and is NOT recommended.
    // - BeginChild() returns false to indicate the window is collapsed or fully clipped, so you may early out and omit submitting
    //   anything to the window. Always call a matching EndChild() for each BeginChild() call, regardless of its return value.
    //   [Important: due to legacy reason, Begin/End and BeginChild/EndChild are inconsistent with all other functions
    //    such as BeginMenu/EndMenu, BeginPopup/EndPopup, etc. where the EndXXX call should only be called if the corresponding
    //    BeginXXX function returned true. Begin and BeginChild are the only odd ones out. Will be fixed in a future update.]
    IMGUI_API bool          BeginChild(const char* str_id, const ImVec2& size = ImVec2(0, 0), ImGuiChildFlags child_flags = 0, ImGuiWindowFlags window_flags = 0);
    IMGUI_API bool          BeginChild(ImGuiID id, const ImVec2& size = ImVec2(0, 0), ImGuiChildFlags child_flags = 0, ImGuiWindowFlags window_flags = 0);
    IMGUI_API void          EndChild();

    // 窗口工具
    // - “当前窗口” = 在 Begin()/End() 块内追加内容的窗口。 “下一个窗口” = 下一个将要 Begin() 的窗口。
    IMGUI_API bool IsWindowAppearing();                          // 当前窗口是否正在出现（例如首次渲染或从隐藏到显示）。
    IMGUI_API bool IsWindowCollapsed();                          // 当前窗口是否被折叠。
    IMGUI_API bool IsWindowFocused(ImGuiFocusedFlags flags = 0); // 当前窗口是否获得焦点？可根据 flags 判断是否包含根窗口/子窗口的焦点，查看 flags 获取选项。
    IMGUI_API bool IsWindowHovered(ImGuiHoveredFlags flags = 0); // 当前窗口是否被悬停并可悬停（例如未被弹出窗口/模态窗口阻挡）？参见 ImGuiHoveredFlags_ 了解选项。
                                                                 // 重要：如果需要判断鼠标事件应该分发到 ImGui 还是底层应用，请不要使用此函数！应使用 io.WantCaptureMouse 布尔值。详情参考 FAQ 条目 "如何判断鼠标/键盘事件应该分发给 ImGui 或应用程序？"。
    IMGUI_API ImDrawList *GetWindowDrawList();                   // 获取与当前窗口关联的绘制列表，可用于追加自定义绘图原语。
    IMGUI_API float GetWindowDpiScale();                         // 获取当前窗口视口的 DPI 缩放比例。
    IMGUI_API ImVec2 GetWindowPos();                             // 获取当前窗口在屏幕空间的坐标位置（通常不需要直接使用此函数，建议使用 GetCursorScreenPos() 和 GetContentRegionAvail()）。
    IMGUI_API ImVec2 GetWindowSize();                            // 获取当前窗口的尺寸（通常不需要直接使用此函数，建议使用 GetCursorScreenPos() 和 GetContentRegionAvail()）。
    IMGUI_API float GetWindowWidth();                            // 获取当前窗口的宽度（通常不需要直接使用）。相当于 GetWindowSize().x 的快捷方式。
    IMGUI_API float GetWindowHeight();                           // 获取当前窗口的高度（通常不需要直接使用）。相当于 GetWindowSize().y 的快捷方式。
    IMGUI_API ImGuiViewport *GetWindowViewport();                // 获取当前窗口关联的视口。

    // 窗口操作
    // - 在调用 Begin 之前，优先使用 SetNextXXX 函数，而不是在 Begin 之后使用 SetXXX 函数。
    IMGUI_API void SetNextWindowPos(const ImVec2 &pos, ImGuiCond cond = 0, const ImVec2 &pivot = ImVec2(0, 0));                                                               // 设置下一个窗口的位置。调用在 Begin() 之前。使用 pivot=(0.5f, 0.5f) 来使窗口居中在给定点，等等。
    IMGUI_API void SetNextWindowSize(const ImVec2 &size, ImGuiCond cond = 0);                                                                                                 // 设置下一个窗口的大小。设置某一轴为 0.0f 以强制该轴自动调整大小。调用在 Begin() 之前。
    IMGUI_API void SetNextWindowSizeConstraints(const ImVec2 &size_min, const ImVec2 &size_max, ImGuiSizeCallback custom_callback = NULL, void *custom_callback_data = NULL); // 设置下一个窗口的大小限制。如果不想设置限制，可以使用 0.0f 或 FLT_MAX。使用 -1 作为最小和最大值可以保留当前大小（本身就是一种限制）。使用回调函数来应用非平凡的程序化约束。
    IMGUI_API void SetNextWindowContentSize(const ImVec2 &size);                                                                                                              // 设置下一个窗口的内容大小（大约是可滚动的客户区域，用来限制滚动条的范围）。不包括窗口装饰（如标题栏、菜单栏等）或窗口内边距。设置某一轴为 0.0f 以让其自动调整。调用在 Begin() 之前。
    IMGUI_API void SetNextWindowCollapsed(bool collapsed, ImGuiCond cond = 0);                                                                                                // 设置下一个窗口的折叠状态。调用在 Begin() 之前。
    IMGUI_API void SetNextWindowFocus();                                                                                                                                      // 设置下一个窗口为焦点窗口 / 最顶层窗口。调用在 Begin() 之前。
    IMGUI_API void SetNextWindowScroll(const ImVec2 &scroll);                                                                                                                 // 设置下一个窗口的滚动值（使用 < 0.0f 来不影响某一轴的滚动）。
    IMGUI_API void SetNextWindowBgAlpha(float alpha);                                                                                                                         // 设置下一个窗口背景色的透明度。此函数帮助你轻松覆盖 ImGuiCol_WindowBg/ChildBg/PopupBg 的透明度组件。你也可以使用 ImGuiWindowFlags_NoBackground 来达成此效果。
    IMGUI_API void SetNextWindowViewport(ImGuiID viewport_id);                                                                                                                // 设置下一个窗口的视口。
    IMGUI_API void SetWindowPos(const ImVec2 &pos, ImGuiCond cond = 0);                                                                                                       // （不推荐使用）设置当前窗口的位置 - 在 Begin()/End() 之间调用。优先使用 SetNextWindowPos()，因为此方法可能会导致撕裂和副作用。
    IMGUI_API void SetWindowSize(const ImVec2 &size, ImGuiCond cond = 0);                                                                                                     // （不推荐使用）设置当前窗口的大小 - 在 Begin()/End() 之间调用。设置为 ImVec2(0, 0) 可以强制自动调整大小。优先使用 SetNextWindowSize()，因为此方法可能会导致撕裂和副作用。
    IMGUI_API void SetWindowCollapsed(bool collapsed, ImGuiCond cond = 0);                                                                                                    // （不推荐使用）设置当前窗口的折叠状态。优先使用 SetNextWindowCollapsed()。
    IMGUI_API void SetWindowFocus();                                                                                                                                          // （不推荐使用）设置当前窗口为焦点窗口 / 最顶层窗口。优先使用 SetNextWindowFocus()。
    IMGUI_API void SetWindowFontScale(float scale);                                                                                                                           // [已废弃] 设置字体缩放。若要缩放所有窗口，请调整 IO.FontGlobalScale。此 API 已过时！对于正确的缩放，优先重新加载字体并重建 ImFontAtlas，然后调用 style.ScaleAllSizes()。
    IMGUI_API void SetWindowPos(const char *name, const ImVec2 &pos, ImGuiCond cond = 0);                                                                                     // 设置指定名称的窗口位置。
    IMGUI_API void SetWindowSize(const char *name, const ImVec2 &size, ImGuiCond cond = 0);                                                                                   // 设置指定名称的窗口大小。将某一轴设置为 0.0f 以强制该轴自动调整大小。
    IMGUI_API void SetWindowCollapsed(const char *name, bool collapsed, ImGuiCond cond = 0);                                                                                  // 设置指定名称的窗口折叠状态。
    IMGUI_API void SetWindowFocus(const char *name);                                                                                                                          // 设置指定名称的窗口为焦点窗口 / 最顶层窗口。使用 NULL 来移除焦点。

    // 窗口滚动
    // - 滚动的任何更改将在下一帧的第一次调用 Begin() 时应用。
    // - 您可以在调用 Begin() 之前使用 SetNextWindowScroll() 来避免此延迟，这是使用 SetScrollX()/SetScrollY() 的替代方法。
    IMGUI_API float GetScrollX();                                                 // 获取当前滚动量 [0 .. GetScrollMaxX()]
    IMGUI_API float GetScrollY();                                                 // 获取当前滚动量 [0 .. GetScrollMaxY()]
    IMGUI_API void SetScrollX(float scroll_x);                                    // 设置滚动量 [0 .. GetScrollMaxX()]
    IMGUI_API void SetScrollY(float scroll_y);                                    // 设置滚动量 [0 .. GetScrollMaxY()]
    IMGUI_API float GetScrollMaxX();                                              // 获取最大滚动量 ~~ ContentSize.x - WindowSize.x - DecorationsSize.x
    IMGUI_API float GetScrollMaxY();                                              // 获取最大滚动量 ~~ ContentSize.y - WindowSize.y - DecorationsSize.y
    IMGUI_API void SetScrollHereX(float center_x_ratio = 0.5f);                   // 调整滚动量使当前光标位置可见。center_x_ratio=0.0: 左边, 0.5: 居中, 1.0: 右边。若要使“默认/当前项”可见，考虑使用 SetItemDefaultFocus()。
    IMGUI_API void SetScrollHereY(float center_y_ratio = 0.5f);                   // 调整滚动量使当前光标位置可见。center_y_ratio=0.0: 顶部, 0.5: 居中, 1.0: 底部。若要使“默认/当前项”可见，考虑使用 SetItemDefaultFocus()。
    IMGUI_API void SetScrollFromPosX(float local_x, float center_x_ratio = 0.5f); // 调整滚动量使给定位置可见。通常使用 GetCursorStartPos() + 偏移量来计算有效位置。
    IMGUI_API void SetScrollFromPosY(float local_y, float center_y_ratio = 0.5f); // 调整滚动量使给定位置可见。通常使用 GetCursorStartPos() + 偏移量来计算有效位置。

    // 参数堆栈（共享）
    IMGUI_API void PushFont(ImFont *font);                             // 使用 NULL 作为快捷方式来推送默认字体
    IMGUI_API void PopFont();                                          // 恢复上一个字体
    IMGUI_API void PushStyleColor(ImGuiCol idx, ImU32 col);            // 修改样式颜色。若在 NewFrame() 之后修改样式，请始终使用此函数。
    IMGUI_API void PushStyleColor(ImGuiCol idx, const ImVec4 &col);    // 修改样式颜色。使用 ImVec4 来指定颜色。
    IMGUI_API void PopStyleColor(int count = 1);                       // 恢复指定数量的样式颜色
    IMGUI_API void PushStyleVar(ImGuiStyleVar idx, float val);         // 修改样式的浮动变量。若在 NewFrame() 之后修改样式，请始终使用此函数！
    IMGUI_API void PushStyleVar(ImGuiStyleVar idx, const ImVec2 &val); // 修改样式的 ImVec2 变量。
    IMGUI_API void PushStyleVarX(ImGuiStyleVar idx, float val_x);      // 修改样式中 ImVec2 变量的 X 分量。
    IMGUI_API void PushStyleVarY(ImGuiStyleVar idx, float val_y);      // 修改样式中 ImVec2 变量的 Y 分量。
    IMGUI_API void PopStyleVar(int count = 1);                         // 恢复指定数量的样式变量
    IMGUI_API void PushItemFlag(ImGuiItemFlags option, bool enabled);  // 修改指定的共享项标志，例如 PushItemFlag(ImGuiItemFlags_NoTabStop, true)
    IMGUI_API void PopItemFlag();                                      // 恢复上一个项标志

    // 参数堆栈（当前窗口）
    IMGUI_API void PushItemWidth(float item_width);                // 推送常见“大型项+标签”控件的宽度。>0.0f：以像素为单位的宽度，<0.0f：将宽度对齐到窗口的右侧（因此 -FLT_MIN 始终将宽度对齐到右侧）。
    IMGUI_API void PopItemWidth();                                 // 恢复上一个项宽度
    IMGUI_API void SetNextItemWidth(float item_width);             // 设置下一个常见“大型项+标签”控件的宽度。>0.0f：以像素为单位的宽度，<0.0f：将宽度对齐到窗口的右侧（因此 -FLT_MIN 始终将宽度对齐到右侧）
    IMGUI_API float CalcItemWidth();                               // 给定推送设置和当前光标位置后，计算项的宽度。与大多数“项”函数不同，这不一定是最后一个项的宽度。
    IMGUI_API void PushTextWrapPos(float wrap_local_pos_x = 0.0f); // 推送 Text*() 命令的换行位置。< 0.0f：不换行；0.0f：换行到窗口（或列）的末尾；> 0.0f：在窗口本地空间中的“wrap_pos_x”位置换行
    IMGUI_API void PopTextWrapPos();                               // 恢复上一个换行位置

    // 样式读取访问
    // - 使用 ShowStyleEditor() 函数与颜色交互式查看/编辑。
    IMGUI_API ImFont *GetFont();                                       // 获取当前字体
    IMGUI_API float GetFontSize();                                     // 获取当前字体的大小（即当前字体的像素高度）并应用当前缩放
    IMGUI_API ImVec2 GetFontTexUvWhitePixel();                         // 获取白色像素的 UV 坐标，用于通过 ImDrawList API 绘制自定义形状
    IMGUI_API ImU32 GetColorU32(ImGuiCol idx, float alpha_mul = 1.0f); // 获取给定样式颜色并应用样式的透明度，且可以选择性地使用额外的透明度乘法器，以 32 位值形式返回，适用于 ImDrawList
    IMGUI_API ImU32 GetColorU32(const ImVec4 &col);                    // 获取给定颜色并应用样式的透明度，以 32 位值形式返回，适用于 ImDrawList
    IMGUI_API ImU32 GetColorU32(ImU32 col, float alpha_mul = 1.0f);    // 获取给定颜色并应用样式的透明度，以 32 位值形式返回，适用于 ImDrawList
    IMGUI_API const ImVec4 &GetStyleColorVec4(ImGuiCol idx);           // 获取存储在 ImGuiStyle 结构中的样式颜色。用于反馈到 PushStyleColor() 中，否则使用 GetColorU32() 获取应用样式透明度的样式颜色

    // 布局光标定位
    // - "光标"指的是当前输出的位置。
    // - 典型的小部件行为是在当前光标位置输出自己，然后将光标向下一行移动。
    // - 你可以在小部件之间调用 SameLine() 来撤销上一个换行并在前一个小部件的右侧输出。
    // - 你可以通过 GetCursorScreenPos() 和 GetContentRegionAvail() 来完成 99% 的需求。
    // - 注意！当前我们在窗口本地和绝对坐标之间存在不一致，未来的 API 会解决这个问题：
    //    - 绝对坐标：        GetCursorScreenPos()，SetCursorScreenPos()，所有 ImDrawList:: 函数。 -> 这是推荐的方式。
    //    - 窗口本地坐标：   SameLine(offset)，GetCursorPos()，SetCursorPos()，GetCursorStartPos()，PushTextWrapPos()
    //    - 窗口本地坐标：   GetContentRegionMax()，GetWindowContentRegionMin()，GetWindowContentRegionMax() --> 都已经废弃。你不再需要它们。
    // - GetCursorScreenPos() = GetCursorPos() + GetWindowPos()。GetWindowPos() 几乎只在从窗口本地坐标转换为绝对坐标时有用。尽量不要使用它。
    IMGUI_API ImVec2 GetCursorScreenPos();                // 获取光标位置，绝对坐标。这是你最好的朋友（建议使用它而非 GetCursorPos()，在 ImDrawList API 中也更为有用）。
    IMGUI_API void SetCursorScreenPos(const ImVec2 &pos); // 设置光标位置，绝对坐标。这是你最好的朋友。
    IMGUI_API ImVec2 GetContentRegionAvail();             // 从当前光标位置开始的可用空间。这是你最好的朋友。
    IMGUI_API ImVec2 GetCursorPos();                      // [窗口本地] 光标位置，窗口本地坐标。这不是你最好的朋友。
    IMGUI_API float GetCursorPosX();                      // [窗口本地] 获取 X 坐标
    IMGUI_API float GetCursorPosY();                      // [窗口本地] 获取 Y 坐标
    IMGUI_API void SetCursorPos(const ImVec2 &local_pos); // [窗口本地] 设置光标位置
    IMGUI_API void SetCursorPosX(float local_x);          // [窗口本地] 设置 X 坐标
    IMGUI_API void SetCursorPosY(float local_y);          // [窗口本地] 设置 Y 坐标
    IMGUI_API ImVec2 GetCursorStartPos();                 // [窗口本地] 初始光标位置，窗口本地坐标。可以在 Begin() 后调用 GetCursorScreenPos() 来获取绝对坐标版本。

    // 其他布局函数
    IMGUI_API void Separator();                                                       // 分隔符，通常是水平的。如果在菜单栏内或处于水平布局模式中，这将成为一个垂直分隔符。
    IMGUI_API void SameLine(float offset_from_start_x = 0.0f, float spacing = -1.0f); // 在小部件或组之间调用，水平布局。X 位置以窗口坐标给出。
    IMGUI_API void NewLine();                                                         // 撤销 SameLine() 或在水平布局上下文中强制换行。
    IMGUI_API void Spacing();                                                         // 添加垂直间距。
    IMGUI_API void Dummy(const ImVec2 &size);                                         // 添加给定大小的虚拟项。与 InvisibleButton() 不同，Dummy() 不会响应鼠标点击或无法导航。
    IMGUI_API void Indent(float indent_w = 0.0f);                                     // 将内容位置向右移动，移动的宽度为 indent_w，或者如果 indent_w <= 0，则使用 style.IndentSpacing。
    IMGUI_API void Unindent(float indent_w = 0.0f);                                   // 将内容位置向左移动，移动的宽度为 indent_w，或者如果 indent_w <= 0，则使用 style.IndentSpacing。
    IMGUI_API void BeginGroup();                                                      // 锁定水平起始位置
    IMGUI_API void EndGroup();                                                        // 解锁水平起始位置并将整个组的边界框捕获为一个“项”（这样你就可以对整个组使用 IsItemHovered() 或布局原语如 SameLine() 等）
    IMGUI_API void AlignTextToFramePadding();                                         // 将即将到来的文本基线垂直对齐到 FramePadding.y，这样它就能与常规框架项正确对齐（如果你有文本在框架项之前，调用此函数）
    IMGUI_API float GetTextLineHeight();                                              // ~ 字体大小
    IMGUI_API float GetTextLineHeightWithSpacing();                                   // ~ 字体大小 + style.ItemSpacing.y（两行文本之间的像素距离）
    IMGUI_API float GetFrameHeight();                                                 // ~ 字体大小 + style.FramePadding.y * 2
    IMGUI_API float GetFrameHeightWithSpacing();                                      // ~ 字体大小 + style.FramePadding.y * 2 + style.ItemSpacing.y（两行框架控件之间的像素距离）

    // ID 栈/作用域
    // 阅读 FAQ（docs/FAQ.md 或 http://dearimgui.com/faq）以获取更多有关如何在 Dear ImGui 中处理 ID 的详细信息。
    // - 这些问题的答案和影响是基于对 ID 栈系统的理解：
    //   - "Q: 为什么我的小部件在点击时没有反应？"
    //   - "Q: 如何让小部件的标签为空？"
    //   - "Q: 如何让多个小部件使用相同的标签？"
    // - 简短版本：ID 是整个 ID 栈的哈希值。如果你在循环中创建小部件，通常你需要推送一个唯一的标识符（例如对象指针、循环索引），以便唯一区分它们。
    // - 你也可以在小部件标签中使用 "Label##foobar" 语法来区分它们。
    // - 在这个头文件中，我们使用 "label"/"name" 术语来表示一个将显示并作为 ID 使用的字符串，
    //   而 "str_id" 表示一个仅用作 ID 的字符串，通常不会显示。
    IMGUI_API void PushID(const char *str_id);                               // 将字符串推入 ID 栈（将对字符串进行哈希处理）。
    IMGUI_API void PushID(const char *str_id_begin, const char *str_id_end); // 将字符串推入 ID 栈（将对字符串进行哈希处理）。
    IMGUI_API void PushID(const void *ptr_id);                               // 将指针推入 ID 栈（将对指针进行哈希处理）。
    IMGUI_API void PushID(int int_id);                                       // 将整数推入 ID 栈（将对整数进行哈希处理）。
    IMGUI_API void PopID();                                                  // 从 ID 栈中弹出。
    IMGUI_API ImGuiID GetID(const char *str_id);                             // 计算唯一 ID（整个 ID 栈 + 给定参数的哈希值）。例如，如果你想自己查询 ImGuiStorage。
    IMGUI_API ImGuiID GetID(const char *str_id_begin, const char *str_id_end);
    IMGUI_API ImGuiID GetID(const void *ptr_id);
    IMGUI_API ImGuiID GetID(int int_id);

    // 小部件：文本
    IMGUI_API void TextUnformatted(const char *text, const char *text_end = NULL); // 无格式文本。大致等价于 Text("%s", text)，但：A) 如果指定了 'text_end'，不需要空终止的字符串，B) 它更快，不进行内存复制，没有缓冲区大小限制，推荐用于长文本块。
    IMGUI_API void Text(const char *fmt, ...) IM_FMTARGS(1);                       // 格式化文本
    IMGUI_API void TextV(const char *fmt, va_list args) IM_FMTLIST(1);
    IMGUI_API void TextColored(const ImVec4 &col, const char *fmt, ...) IM_FMTARGS(2); // 用颜色显示文本的快捷方式（相当于 PushStyleColor(ImGuiCol_Text, col); Text(fmt, ...); PopStyleColor();）
    IMGUI_API void TextColoredV(const ImVec4 &col, const char *fmt, va_list args) IM_FMTLIST(2);
    IMGUI_API void TextDisabled(const char *fmt, ...) IM_FMTARGS(1); // 禁用文本的快捷方式（相当于 PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TextDisabled]); Text(fmt, ...); PopStyleColor();）
    IMGUI_API void TextDisabledV(const char *fmt, va_list args) IM_FMTLIST(1);
    IMGUI_API void TextWrapped(const char *fmt, ...) IM_FMTARGS(1); // 换行文本的快捷方式（相当于 PushTextWrapPos(0.0f); Text(fmt, ...); PopTextWrapPos();）。请注意，如果窗口是自动调整大小的且没有其他小部件扩展窗口宽度，可能需要使用 SetNextWindowSize() 设置大小。
    IMGUI_API void TextWrappedV(const char *fmt, va_list args) IM_FMTLIST(1);
    IMGUI_API void LabelText(const char *label, const char *fmt, ...) IM_FMTARGS(2); // 显示文本+标签，布局方式与值+标签小部件相同
    IMGUI_API void LabelTextV(const char *label, const char *fmt, va_list args) IM_FMTLIST(2);
    IMGUI_API void BulletText(const char *fmt, ...) IM_FMTARGS(1); // Bullet() + Text() 的快捷方式
    IMGUI_API void BulletTextV(const char *fmt, va_list args) IM_FMTLIST(1);
    IMGUI_API void SeparatorText(const char *label); // 当前实现：带有水平线的格式化文本

    // 小部件：主控件
    // - 大多数小部件在值发生变化或被按下/选择时返回 true。
    // - 你还可以使用多个 IsItemXXX 函数之一（例如 IsItemActive、IsItemHovered 等）来查询小部件的状态。
    IMGUI_API bool Button(const char *label, const ImVec2 &size = ImVec2(0, 0));                        // 按钮
    IMGUI_API bool SmallButton(const char *label);                                                      // 按钮（FramePadding.y == 0），易于嵌入文本中
    IMGUI_API bool InvisibleButton(const char *str_id, const ImVec2 &size, ImGuiButtonFlags flags = 0); // 灵活的小部件行为，没有视觉效果，常用于构建自定义行为（结合 IsItemActive，IsItemHovered 等）
    IMGUI_API bool ArrowButton(const char *str_id, ImGuiDir dir);                                       // 带箭头形状的方形按钮
    IMGUI_API bool Checkbox(const char *label, bool *v);                                                // 复选框
    IMGUI_API bool CheckboxFlags(const char *label, int *flags, int flags_value);                       // 复选框，支持标志位
    IMGUI_API bool CheckboxFlags(const char *label, unsigned int *flags, unsigned int flags_value);
    IMGUI_API bool RadioButton(const char *label, bool active);                                                           // 单选按钮（例如，if (RadioButton("one", my_value==1)) { my_value = 1; }）
    IMGUI_API bool RadioButton(const char *label, int *v, int v_button);                                                  // 整型值的单选按钮快捷方式
    IMGUI_API void ProgressBar(float fraction, const ImVec2 &size_arg = ImVec2(-FLT_MIN, 0), const char *overlay = NULL); // 进度条
    IMGUI_API void Bullet();                                                                                              // 绘制一个小圆圈并将光标保持在同一行。光标的 x 位置会被 GetTreeNodeToLabelSpacing() 移动，这是 TreeNode() 使用的相同距离
    IMGUI_API bool TextLink(const char *label);                                                                           // 超链接文本按钮，点击时返回 true
    IMGUI_API void TextLinkOpenURL(const char *label, const char *url = NULL);                                            // 超链接文本按钮，点击时自动打开文件/URL

    // 小部件：图片
    // - 关于 ImTextureID 的更多信息，请参阅这里：https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
    // - 'uv0' 和 'uv1' 是纹理坐标。更多信息请参阅上面的链接。
    // - 注意，如果边框可见，Image() 可能会在提供的尺寸上加上 +2.0f，ImageButton() 会在提供的尺寸上加上 style.FramePadding*2.0f。
    // - ImageButton() 会基于常规的 Button() 颜色绘制一个背景，并且如果指定，还会绘制一个内背景。
    IMGUI_API void Image(ImTextureID user_texture_id, const ImVec2 &image_size, const ImVec2 &uv0 = ImVec2(0, 0), const ImVec2 &uv1 = ImVec2(1, 1), const ImVec4 &tint_col = ImVec4(1, 1, 1, 1), const ImVec4 &border_col = ImVec4(0, 0, 0, 0));
    IMGUI_API bool ImageButton(const char *str_id, ImTextureID user_texture_id, const ImVec2 &image_size, const ImVec2 &uv0 = ImVec2(0, 0), const ImVec2 &uv1 = ImVec2(1, 1), const ImVec4 &bg_col = ImVec4(0, 0, 0, 0), const ImVec4 &tint_col = ImVec4(1, 1, 1, 1));

    // 小部件：组合框（下拉框）
    // - BeginCombo()/EndCombo() API 允许你通过创建例如 Selectable() 项目来管理内容和选择状态，完全按你的需求。
    // - 旧版的 Combo() API 是对 BeginCombo()/EndCombo() 的辅助函数，仍然为了方便而保留使用。这个类似于如何创建 ListBox。
    IMGUI_API bool BeginCombo(const char *label, const char *preview_value, ImGuiComboFlags flags = 0);
    IMGUI_API void EndCombo(); // 只有在 BeginCombo() 返回 true 时才调用 EndCombo()！
    IMGUI_API bool Combo(const char *label, int *current_item, const char *const items[], int items_count, int popup_max_height_in_items = -1);
    IMGUI_API bool Combo(const char *label, int *current_item, const char *items_separated_by_zeros, int popup_max_height_in_items = -1); // 用 \0 分隔字符串中的项目，项目列表以 \0\0 结束。例如："One\0Two\0Three\0"
    IMGUI_API bool Combo(const char *label, int *current_item, const char *(*getter)(void *user_data, int idx), void *user_data, int items_count, int popup_max_height_in_items = -1);

    // 小部件：拖动滑块
    // - CTRL+点击任何拖动框会将其转换为输入框。手动输入的值默认不被限制，可以超出边界。使用 ImGuiSliderFlags_AlwaysClamp 来始终进行限制。
    // - 对于所有的 Float2/Float3/Float4/Int2/Int3/Int4 版本的函数，请注意，'float v[X]' 函数参数与 'float* v' 是相同的，
    //   数组语法只是用来记录预期的元素数量。你可以传递一个连续数组的第一个元素地址，例如：&myvector.x。
    // - 调整格式字符串以修饰值的前缀、后缀，或调整编辑和显示精度，例如 "%.3f" -> 1.234； "%5.2f secs" -> 01.23 secs；"Biscuit: %.0f" -> Biscuit: 1 等等。
    // - 格式字符串也可以设置为 NULL 或使用默认格式 ("%f" 或 "%d")。
    // - 速度是每像素鼠标移动（v_speed=0.2f：鼠标需要移动 5 个像素才能使值增加 1）。对于键盘/游戏手柄导航，最小速度是 Max(v_speed, 给定精度下的最小步长)。
    // - 使用 v_min < v_max 来限制编辑范围。如果没有使用 ImGuiSliderFlags_AlwaysClamp，CTRL+Click 手动输入可以覆盖这些限制。
    // - 使用 v_max = FLT_MAX / INT_MAX 等来避免限制最大值，同理，v_min = -FLT_MAX / INT_MIN 来避免限制最小值。
    // - 我们为 DragXXX() 和 SliderXXX() 函数使用相同的标志集，因为它们的功能相同，这使得它们之间的切换更加简单。
    // - 旧版：在 1.78 之前，DragXXX() 函数签名使用了最终的 `float power=1.0f` 参数，而不是 `ImGuiSliderFlags flags=0` 参数。
    //   如果你遇到将浮动值转换为 ImGuiSliderFlags 的警告，请阅读 https://github.com/ocornut/imgui/issues/3361。
    IMGUI_API bool DragFloat(const char *label, float *v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char *format = "%.3f", ImGuiSliderFlags flags = 0); // 如果 v_min >= v_max，我们没有限制
    IMGUI_API bool DragFloat2(const char *label, float v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char *format = "%.3f", ImGuiSliderFlags flags = 0);
    IMGUI_API bool DragFloat3(const char *label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char *format = "%.3f", ImGuiSliderFlags flags = 0);
    IMGUI_API bool DragFloat4(const char *label, float v[4], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char *format = "%.3f", ImGuiSliderFlags flags = 0);
    IMGUI_API bool DragFloatRange2(const char *label, float *v_current_min, float *v_current_max, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char *format = "%.3f", const char *format_max = NULL, ImGuiSliderFlags flags = 0);
    IMGUI_API bool DragInt(const char *label, int *v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char *format = "%d", ImGuiSliderFlags flags = 0); // 如果 v_min >= v_max，我们没有限制
    IMGUI_API bool DragInt2(const char *label, int v[2], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char *format = "%d", ImGuiSliderFlags flags = 0);
    IMGUI_API bool DragInt3(const char *label, int v[3], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char *format = "%d", ImGuiSliderFlags flags = 0);
    IMGUI_API bool DragInt4(const char *label, int v[4], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char *format = "%d", ImGuiSliderFlags flags = 0);
    IMGUI_API bool DragIntRange2(const char *label, int *v_current_min, int *v_current_max, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char *format = "%d", const char *format_max = NULL, ImGuiSliderFlags flags = 0);
    IMGUI_API bool DragScalar(const char *label, ImGuiDataType data_type, void *p_data, float v_speed = 1.0f, const void *p_min = NULL, const void *p_max = NULL, const char *format = NULL, ImGuiSliderFlags flags = 0);
    IMGUI_API bool DragScalarN(const char *label, ImGuiDataType data_type, void *p_data, int components, float v_speed = 1.0f, const void *p_min = NULL, const void *p_max = NULL, const char *format = NULL, ImGuiSliderFlags flags = 0);

    // 小部件：常规滑块
    // - CTRL+点击任何滑块会将其转换为输入框。手动输入的值默认不被限制，可以超出边界。使用 ImGuiSliderFlags_AlwaysClamp 来始终进行限制。
    // - 调整格式字符串以修饰值的前缀、后缀，或调整编辑和显示精度，例如 "%.3f" -> 1.234； "%5.2f secs" -> 01.23 secs；"Biscuit: %.0f" -> Biscuit: 1 等等。
    // - 格式字符串也可以设置为 NULL 或使用默认格式 ("%f" 或 "%d")。
    // - 旧版：在 1.78 之前，SliderXXX() 函数签名使用了最终的 `float power=1.0f` 参数，而不是 `ImGuiSliderFlags flags=0` 参数。
    //   如果你遇到将浮动值转换为 ImGuiSliderFlags 的警告，请阅读 https://github.com/ocornut/imgui/issues/3361。
    IMGUI_API bool SliderFloat(const char *label, float *v, float v_min, float v_max, const char *format = "%.3f", ImGuiSliderFlags flags = 0); // 调整格式以修饰值的前缀或后缀，用于滑块标签或单位显示。
    IMGUI_API bool SliderFloat2(const char *label, float v[2], float v_min, float v_max, const char *format = "%.3f", ImGuiSliderFlags flags = 0);
    IMGUI_API bool SliderFloat3(const char *label, float v[3], float v_min, float v_max, const char *format = "%.3f", ImGuiSliderFlags flags = 0);
    IMGUI_API bool SliderFloat4(const char *label, float v[4], float v_min, float v_max, const char *format = "%.3f", ImGuiSliderFlags flags = 0);
    IMGUI_API bool SliderAngle(const char *label, float *v_rad, float v_degrees_min = -360.0f, float v_degrees_max = +360.0f, const char *format = "%.0f deg", ImGuiSliderFlags flags = 0);
    IMGUI_API bool SliderInt(const char *label, int *v, int v_min, int v_max, const char *format = "%d", ImGuiSliderFlags flags = 0);
    IMGUI_API bool SliderInt2(const char *label, int v[2], int v_min, int v_max, const char *format = "%d", ImGuiSliderFlags flags = 0);
    IMGUI_API bool SliderInt3(const char *label, int v[3], int v_min, int v_max, const char *format = "%d", ImGuiSliderFlags flags = 0);
    IMGUI_API bool SliderInt4(const char *label, int v[4], int v_min, int v_max, const char *format = "%d", ImGuiSliderFlags flags = 0);
    IMGUI_API bool SliderScalar(const char *label, ImGuiDataType data_type, void *p_data, const void *p_min, const void *p_max, const char *format = NULL, ImGuiSliderFlags flags = 0);
    IMGUI_API bool SliderScalarN(const char *label, ImGuiDataType data_type, void *p_data, int components, const void *p_min, const void *p_max, const char *format = NULL, ImGuiSliderFlags flags = 0);
    IMGUI_API bool VSliderFloat(const char *label, const ImVec2 &size, float *v, float v_min, float v_max, const char *format = "%.3f", ImGuiSliderFlags flags = 0);
    IMGUI_API bool VSliderInt(const char *label, const ImVec2 &size, int *v, int v_min, int v_max, const char *format = "%d", ImGuiSliderFlags flags = 0);
    IMGUI_API bool VSliderScalar(const char *label, const ImVec2 &size, ImGuiDataType data_type, void *p_data, const void *p_min, const void *p_max, const char *format = NULL, ImGuiSliderFlags flags = 0);

    // 小部件：使用键盘输入
    // - 如果你想使用 InputText() 和 std::string 或任何自定义动态字符串类型，请参阅 misc/cpp/imgui_stdlib.h 和 imgui_demo.cpp 中的注释。
    // - 大多数 ImGuiInputTextFlags 标志仅对 InputText() 有用，而不适用于 InputFloatX、InputIntX、InputDouble 等。
    IMGUI_API bool InputText(const char *label, char *buf, size_t buf_size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void *user_data = NULL);
    IMGUI_API bool InputTextMultiline(const char *label, char *buf, size_t buf_size, const ImVec2 &size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void *user_data = NULL);
    IMGUI_API bool InputTextWithHint(const char *label, const char *hint, char *buf, size_t buf_size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void *user_data = NULL);
    IMGUI_API bool InputFloat(const char *label, float *v, float step = 0.0f, float step_fast = 0.0f, const char *format = "%.3f", ImGuiInputTextFlags flags = 0);
    IMGUI_API bool InputFloat2(const char *label, float v[2], const char *format = "%.3f", ImGuiInputTextFlags flags = 0);
    IMGUI_API bool InputFloat3(const char *label, float v[3], const char *format = "%.3f", ImGuiInputTextFlags flags = 0);
    IMGUI_API bool InputFloat4(const char *label, float v[4], const char *format = "%.3f", ImGuiInputTextFlags flags = 0);
    IMGUI_API bool InputInt(const char *label, int *v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
    IMGUI_API bool InputInt2(const char *label, int v[2], ImGuiInputTextFlags flags = 0);
    IMGUI_API bool InputInt3(const char *label, int v[3], ImGuiInputTextFlags flags = 0);
    IMGUI_API bool InputInt4(const char *label, int v[4], ImGuiInputTextFlags flags = 0);
    IMGUI_API bool InputDouble(const char *label, double *v, double step = 0.0, double step_fast = 0.0, const char *format = "%.6f", ImGuiInputTextFlags flags = 0);
    IMGUI_API bool InputScalar(const char *label, ImGuiDataType data_type, void *p_data, const void *p_step = NULL, const void *p_step_fast = NULL, const char *format = NULL, ImGuiInputTextFlags flags = 0);
    IMGUI_API bool InputScalarN(const char *label, ImGuiDataType data_type, void *p_data, int components, const void *p_step = NULL, const void *p_step_fast = NULL, const char *format = NULL, ImGuiInputTextFlags flags = 0);

    // 小部件：颜色编辑器/选择器（提示：ColorEdit* 函数有一个小的颜色方块，可以通过左键点击打开选择器，右键点击打开选项菜单。）
    // - 请注意，在 C++ 中，'float v[X]' 函数参数与 'float* v' 是相同的，数组语法只是用来记录期望可以访问的元素数量。
    // - 您可以传递一个连续结构中的第一个 float 元素的地址，例如 &myvector.x。
    IMGUI_API bool ColorEdit3(const char *label, float col[3], ImGuiColorEditFlags flags = 0);
    IMGUI_API bool ColorEdit4(const char *label, float col[4], ImGuiColorEditFlags flags = 0);
    IMGUI_API bool ColorPicker3(const char *label, float col[3], ImGuiColorEditFlags flags = 0);
    IMGUI_API bool ColorPicker4(const char *label, float col[4], ImGuiColorEditFlags flags = 0, const float *ref_col = NULL);
    IMGUI_API bool ColorButton(const char *desc_id, const ImVec4 &col, ImGuiColorEditFlags flags = 0, const ImVec2 &size = ImVec2(0, 0)); // 显示一个颜色方块/按钮，悬停时显示详细信息，按下时返回 true。
    IMGUI_API void SetColorEditOptions(ImGuiColorEditFlags flags);                                                                        // 初始化当前选项（通常在应用程序启动时）如果您想选择默认的格式、选择器类型等。用户将能够更改许多设置，除非在您的调用中传递 _NoOptions 标志。

    // 小部件：树形节点
    // - TreeNode 函数在节点打开时返回 true，此时您需要在显示完树节点内容后调用 TreePop()。
    IMGUI_API bool TreeNode(const char *label);
    IMGUI_API bool TreeNode(const char *str_id, const char *fmt, ...) IM_FMTARGS(2); // 帮助函数变体，可以轻松将 ID 与显示的字符串解耦。阅读 FAQ 了解为什么以及如何使用 ID。要将任意文本与 TreeNode() 对齐，您可以使用 Bullet()。
    IMGUI_API bool TreeNode(const void *ptr_id, const char *fmt, ...) IM_FMTARGS(2); // 同上
    IMGUI_API bool TreeNodeV(const char *str_id, const char *fmt, va_list args) IM_FMTLIST(2);
    IMGUI_API bool TreeNodeV(const void *ptr_id, const char *fmt, va_list args) IM_FMTLIST(2);
    IMGUI_API bool TreeNodeEx(const char *label, ImGuiTreeNodeFlags flags = 0);
    IMGUI_API bool TreeNodeEx(const char *str_id, ImGuiTreeNodeFlags flags, const char *fmt, ...) IM_FMTARGS(3);
    IMGUI_API bool TreeNodeEx(const void *ptr_id, ImGuiTreeNodeFlags flags, const char *fmt, ...) IM_FMTARGS(3);
    IMGUI_API bool TreeNodeExV(const char *str_id, ImGuiTreeNodeFlags flags, const char *fmt, va_list args) IM_FMTLIST(3);
    IMGUI_API bool TreeNodeExV(const void *ptr_id, ImGuiTreeNodeFlags flags, const char *fmt, va_list args) IM_FMTLIST(3);
    IMGUI_API void TreePush(const char *str_id);                                                       // ~ Indent()+PushID()。当 TreeNode() 返回 true 时已自动调用，但如果需要，您可以自己调用 TreePush/TreePop。
    IMGUI_API void TreePush(const void *ptr_id);                                                       // 同上
    IMGUI_API void TreePop();                                                                          // ~ Unindent()+PopID()
    IMGUI_API float GetTreeNodeToLabelSpacing();                                                       // 使用 TreeNode*() 或 Bullet() 时，标签前的水平距离 == (g.FontSize + style.FramePadding.x*2) 对于普通无框 TreeNode
    IMGUI_API bool CollapsingHeader(const char *label, ImGuiTreeNodeFlags flags = 0);                  // 如果返回 'true'，则标题被打开。不会缩进也不会推送到 ID 栈中。用户不需要调用 TreePop()。
    IMGUI_API bool CollapsingHeader(const char *label, bool *p_visible, ImGuiTreeNodeFlags flags = 0); // 当 'p_visible != NULL' 时：如果 '*p_visible==true'，在标题右上角显示一个小的关闭按钮，点击时将布尔值设置为 false；如果 '*p_visible==false'，则不显示标题。
    IMGUI_API void SetNextItemOpen(bool is_open, ImGuiCond cond = 0);                                  // 设置下一个 TreeNode/CollapsingHeader 的打开状态。
    IMGUI_API void SetNextItemStorageID(ImGuiID storage_id);                                           // 设置用于存储开/关状态的 ID（默认为与项目 ID 相同）。

    // 小部件：可选择项
    // - 当悬停时，选择项会高亮显示，并且在选中时可以显示另一种颜色。
    // - 邻近的选择项会扩展它们的高亮范围，以避免它们之间出现间隙。这样，一系列选中的选择项看起来是连续的。
    IMGUI_API bool Selectable(const char *label, bool selected = false, ImGuiSelectableFlags flags = 0, const ImVec2 &size = ImVec2(0, 0)); // "bool selected" 表示选择状态（只读）。Selectable() 被点击时返回 true，这样您就可以修改选择状态。size.x==0.0: 使用剩余宽度，size.x>0.0: 指定宽度。size.y==0.0: 使用标签的高度，size.y>0.0: 指定高度。
    IMGUI_API bool Selectable(const char *label, bool *p_selected, ImGuiSelectableFlags flags = 0, const ImVec2 &size = ImVec2(0, 0));      // "bool* p_selected" 指向选择状态（读写），作为一个方便的助手。

    // 多重选择系统用于 Selectable()、Checkbox()、TreeNode() 函数 [BETA]
    // - 该功能启用标准的多重选择/范围选择方式（CTRL+鼠标/键盘，SHIFT+鼠标/键盘等），并且也允许使用裁剪器（clipper）。
    // - ImGuiSelectionUserData 通常用于存储当前视图中的项目索引（但也可以存储其他内容）。
    // - 阅读 ImGuiMultiSelectIO 附近的注释获取指令/详细信息，并在“Demo->Widgets->Selection State & Multi-Select”中查看演示。
    // - TreeNode() 技术上是支持的，但... 正确使用它更为复杂。您需要某种线性/随机访问您的树，
    //   这适用于已经实现过滤器和裁剪器的高级树设置。我们会继续简化当前的演示。
    // - 'selection_size' 和 'items_count' 参数是可选的，并且由一些功能使用。如果它们对您来说计算开销很大，可以避免使用它们。
    IMGUI_API ImGuiMultiSelectIO *BeginMultiSelect(ImGuiMultiSelectFlags flags, int selection_size = -1, int items_count = -1);
    IMGUI_API ImGuiMultiSelectIO *EndMultiSelect();
    IMGUI_API void SetNextItemSelectionUserData(ImGuiSelectionUserData selection_user_data);
    IMGUI_API bool IsItemToggledSelection(); // 最后一个项目的选择状态是否被切换了？如果您需要在达到 EndMultiSelect() 之前获取每个项目的信息，这很有用。我们只返回切换事件，以便正确处理裁剪。

    // 小部件：列表框（List Boxes）
    // - 这本质上是使用 BeginChild/EndChild 结合 ImGuiChildFlags_FrameStyle 标志进行样式化更改和显示标签的薄包装。
    // - 如果您不需要标签，您可以简单地使用 BeginChild() 并设置 ImGuiChildFlags_FrameStyle 标志来获得相同的效果。
    // - 您可以按照自己的方式提交内容并管理选择状态，例如通过创建 Selectable() 或任何其他项目。
    // - 简化的/旧版 ListBox() API 是 BeginListBox()/EndListBox() 的辅助函数，出于便利性目的仍然可用。这类似于如何创建 Combo。
    // - 选择框架宽度：   size.x > 0.0f: 自定义  /  size.x < 0.0f 或 -FLT_MIN: 右对齐   /  size.x = 0.0f（默认）：使用当前的 ItemWidth
    // - 选择框架高度：  size.y > 0.0f: 自定义  /  size.y < 0.0f 或 -FLT_MIN: 底部对齐  /  size.y = 0.0f（默认）：默认高度，适合显示大约 7 项
    IMGUI_API bool BeginListBox(const char *label, const ImVec2 &size = ImVec2(0, 0)); // 打开带框的滚动区域
    IMGUI_API void EndListBox();                                                       // 仅在 BeginListBox() 返回 true 时调用 EndListBox()
    IMGUI_API bool ListBox(const char *label, int *current_item, const char *const items[], int items_count, int height_in_items = -1);
    IMGUI_API bool ListBox(const char *label, int *current_item, const char *(*getter)(void *user_data, int idx), void *user_data, int items_count, int height_in_items = -1);

    // 小部件：数据绘图
    // - 可以考虑使用 ImPlot（https://github.com/epezent/implot），它要好得多！
    IMGUI_API void PlotLines(const char *label, const float *values, int values_count, int values_offset = 0, const char *overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0), int stride = sizeof(float));
    IMGUI_API void PlotLines(const char *label, float (*values_getter)(void *data, int idx), void *data, int values_count, int values_offset = 0, const char *overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0));
    IMGUI_API void PlotHistogram(const char *label, const float *values, int values_count, int values_offset = 0, const char *overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0), int stride = sizeof(float));
    IMGUI_API void PlotHistogram(const char *label, float (*values_getter)(void *data, int idx), void *data, int values_count, int values_offset = 0, const char *overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0));

    // 小部件：Value() 辅助函数
    // - 这些仅仅是调用 Text() 的快捷方式，带有格式化字符串。输出单个值，格式为 "name: value"（提示：可以在代码中自由声明更多函数来处理你的类型，您可以向 ImGui 命名空间添加函数）
    IMGUI_API void Value(const char *prefix, bool b);
    IMGUI_API void Value(const char *prefix, int v);
    IMGUI_API void Value(const char *prefix, unsigned int v);
    IMGUI_API void Value(const char *prefix, float v, const char *float_format = NULL);

    // 小部件：菜单
    // - 使用 BeginMenuBar() 在窗口 ImGuiWindowFlags_MenuBar 上追加到其菜单栏。
    // - 使用 BeginMainMenuBar() 在屏幕顶部创建菜单栏并附加到它。
    // - 使用 BeginMenu() 创建一个子菜单。可以多次调用 BeginMenu() 使用相同的标识符来向其追加更多项目。
    // - 请注意，MenuItem() 的键盘快捷键只是作为便利显示，但目前并未被 Dear ImGui 处理。
    IMGUI_API bool BeginMenuBar();                                                                                       // 追加到当前窗口的菜单栏（需要设置 ImGuiWindowFlags_MenuBar 标志）。
    IMGUI_API void EndMenuBar();                                                                                         // 只有在 BeginMenuBar() 返回 true 时才调用 EndMenuBar()！
    IMGUI_API bool BeginMainMenuBar();                                                                                   // 创建并追加到全屏菜单栏。
    IMGUI_API void EndMainMenuBar();                                                                                     // 只有在 BeginMainMenuBar() 返回 true 时才调用 EndMainMenuBar()！
    IMGUI_API bool BeginMenu(const char *label, bool enabled = true);                                                    // 创建一个子菜单项，只有在此返回 true 时才调用 EndMenu()！
    IMGUI_API void EndMenu();                                                                                            // 只有在 BeginMenu() 返回 true 时才调用 EndMenu()！
    IMGUI_API bool MenuItem(const char *label, const char *shortcut = NULL, bool selected = false, bool enabled = true); // 激活时返回 true。
    IMGUI_API bool MenuItem(const char *label, const char *shortcut, bool *p_selected, bool enabled = true);             // 激活时返回 true，如果 p_selected != NULL，则切换 (*p_selected)。

    // 提示工具
    // - 提示工具是跟随鼠标的窗口，它们不会夺取焦点。
    // - 提示工具窗口可以包含任何类型的项目。
    // - SetTooltip() 是 'if (BeginTooltip()) { Text(...); EndTooltip(); }' 的快捷方式（带有一个微妙的细节，即它会丢弃之前提交的提示工具）。
    IMGUI_API bool BeginTooltip();                                 // 开始/追加一个提示工具窗口。
    IMGUI_API void EndTooltip();                                   // 只有在 BeginTooltip()/BeginItemTooltip() 返回 true 时才调用 EndTooltip()！
    IMGUI_API void SetTooltip(const char *fmt, ...) IM_FMTARGS(1); // 设置一个纯文本提示工具，通常在 ImGui::IsItemHovered() 检查后使用。覆盖之前的 SetTooltip() 调用。
    IMGUI_API void SetTooltipV(const char *fmt, va_list args) IM_FMTLIST(1);

    // 提示工具：用于在悬停某个项目时显示提示工具的助手函数
    // - BeginItemTooltip() 是 'if (IsItemHovered(ImGuiHoveredFlags_ForTooltip) && BeginTooltip())' 的快捷方式。
    // - SetItemTooltip() 是 'if (IsItemHovered(ImGuiHoveredFlags_ForTooltip)) { SetTooltip(...); }' 的快捷方式。
    // - 其中，'ImGuiHoveredFlags_ForTooltip' 本身是使用 'style.HoverFlagsForTooltipMouse' 或 'style.HoverFlagsForTooltipNav'，取决于当前输入类型。对于鼠标，它默认使用 'ImGuiHoveredFlags_Stationary | ImGuiHoveredFlags_DelayShort'。
    IMGUI_API bool BeginItemTooltip();                                 // 如果前面的项目被悬停，则开始/追加提示工具窗口。
    IMGUI_API void SetItemTooltip(const char *fmt, ...) IM_FMTARGS(1); // 如果前面的项目被悬停，则设置纯文本提示工具，覆盖之前的 SetTooltip() 调用。
    IMGUI_API void SetItemTooltipV(const char *fmt, va_list args) IM_FMTLIST(1);

    // 弹出窗口，模态窗口
    //  - 它们会阻止正常的鼠标悬停检测（因此大多数鼠标交互）在它们后面。
    //  - 如果不是模态窗口：它们可以通过点击它们外部的任何地方，或按 ESC 键关闭。
    //  - 它们的可见性状态（~bool）被内部保持，而不是由程序员控制，这与我们通常在常规 Begin*() 调用中使用的方式不同。
    //  - 上述三个属性是相关的：我们需要在库中保持弹出窗口的可见性状态，因为弹出窗口可能在任何时候被关闭。
    //  - 可以通过在调用 IsItemHovered() 或 IsWindowHovered() 时使用 ImGuiHoveredFlags_AllowWhenBlockedByPopup 来绕过悬停限制。
    //  - 重要：弹出窗口标识符是相对于当前 ID 栈的，因此 OpenPopup 和 BeginPopup 通常需要在同一栈级别。
    //    这有时会导致混淆的错误。未来可能会重新设计这个部分。
    //  - BeginPopup()：查询弹出窗口状态，如果弹出窗口打开，则开始向其中添加内容。返回 true 后，调用 EndPopup()。
    //  - BeginPopupModal()：阻止窗口背后的所有交互，无法由用户关闭，添加背景变暗，带有标题栏。
    IMGUI_API bool BeginPopup(const char *str_id, ImGuiWindowFlags flags = 0);                         // 如果弹出窗口打开，则返回 true，你可以开始向其输出内容。
    IMGUI_API bool BeginPopupModal(const char *name, bool *p_open = NULL, ImGuiWindowFlags flags = 0); // 如果模态窗口打开，则返回 true，你可以开始向其输出内容。
    IMGUI_API void EndPopup();                                                                         // 只有在 BeginPopupXXX() 返回 true 时才调用 EndPopup()！

    // 弹出窗口：打开/关闭功能
    //  - OpenPopup()：将弹出窗口状态设置为打开。ImGuiPopupFlags 可用作打开选项。
    //  - 如果不是模态窗口：它们可以通过点击它们外部的任何地方，或按 ESC 键关闭。
    //  - CloseCurrentPopup()：在 BeginPopup()/EndPopup() 范围内使用，用于手动关闭弹出窗口。
    //  - 默认情况下，Selectable()/MenuItem() 被激活时会自动调用 CloseCurrentPopup()（FIXME: 需要一些选项）。
    //  - 使用 ImGuiPopupFlags_NoOpenOverExistingPopup 来避免在相同级别已经有一个弹出窗口时再次打开。
    //    这等价于在 OpenPopup() 前先测试 !IsAnyPopupOpen()。
    //  - 使用 IsWindowAppearing() 来判断 BeginPopup() 后窗口是否刚刚打开。
    //  - 重要：请注意，对于 OpenPopupOnItemClick()，为了向后兼容，我们异常地将默认标志设置为 1（即 ImGuiPopupFlags_MouseButtonRight），
    //    以兼容旧 API 中传递 'int mouse_button = 1' 参数的情况。
    IMGUI_API void OpenPopup(const char *str_id, ImGuiPopupFlags popup_flags = 0);                   // 标记弹出窗口为打开（不要每帧调用！）。
    IMGUI_API void OpenPopup(ImGuiID id, ImGuiPopupFlags popup_flags = 0);                           // id 重载，以方便从嵌套栈调用。
    IMGUI_API void OpenPopupOnItemClick(const char *str_id = NULL, ImGuiPopupFlags popup_flags = 1); // 帮助函数，在最后一个项目上点击时打开弹出窗口。默认使用 ImGuiPopupFlags_MouseButtonRight == 1。
    IMGUI_API void CloseCurrentPopup();                                                              // 手动关闭我们已经打开的弹出窗口。

    // 弹出窗口：打开+开始组合函数助手
    //  - 这些是 OpenPopup+BeginPopup 的助手函数，例如在悬停一个项目并右键点击时触发打开动作。
    //  - 它们便于轻松创建上下文菜单，因此命名为“上下文菜单”。
    //  - 重要：请注意，BeginPopupContextXXX 与 OpenPopup() 一样接收 ImGuiPopupFlags，而与 BeginPopup() 不同。为了完全一致，未来可能会将 ImGuiWindowFlags 添加到 BeginPopupContextXXX 函数中。
    //  - 重要：请注意，出于向后兼容性考虑，我们将其标志默认设置为 1（即 ImGuiPopupFlags_MouseButtonRight），
    //    所以如果你添加其他标志，请记得重新添加 ImGuiPopupFlags_MouseButtonRight。
    IMGUI_API bool BeginPopupContextItem(const char *str_id = NULL, ImGuiPopupFlags popup_flags = 1);   // 在最后一个项目上点击时打开+开始弹出窗口。使用 str_id==NULL 将弹出窗口关联到前一个项目。如果你想在非交互项目（如 Text()）上使用它，则需要显式传入 ID。
    IMGUI_API bool BeginPopupContextWindow(const char *str_id = NULL, ImGuiPopupFlags popup_flags = 1); // 在当前窗口点击时打开+开始弹出窗口。
    IMGUI_API bool BeginPopupContextVoid(const char *str_id = NULL, ImGuiPopupFlags popup_flags = 1);   // 在空白区域点击时打开+开始弹出窗口（没有窗口的地方）。

    // 弹出窗口：查询功能
    //  - IsPopupOpen()：如果弹出窗口在当前 BeginPopup() 弹出窗口栈级别处打开，则返回 true。
    //  - 使用 ImGuiPopupFlags_AnyPopupId 的 IsPopupOpen()：如果任何弹出窗口在当前 BeginPopup() 弹出窗口栈级别处打开，则返回 true。
    //  - 使用 ImGuiPopupFlags_AnyPopupId 和 ImGuiPopupFlags_AnyPopupLevel 的 IsPopupOpen()：如果任何弹出窗口打开，则返回 true。
    IMGUI_API bool IsPopupOpen(const char *str_id, ImGuiPopupFlags flags = 0); // 如果弹出窗口打开，则返回 true。

    // 表格
    // - 是旧 Columns API 的完整替代方案。
    // - 请参阅 Demo->Tables 示例代码。参见 imgui_tables.cpp 顶部的常规注释。
    // - 请参阅 ImGuiTableFlags_ 和 ImGuiTableColumnFlags_ 枚举以了解可用的标志。
    // 常见的调用流程是：
    // - 1. 调用 BeginTable()，如果返回 false 则提前退出。
    // - 2. 可选地调用 TableSetupColumn() 来提交列名/标志/默认值。
    // - 3. 可选地调用 TableSetupScrollFreeze() 来请求列/行的滚动冻结。
    // - 4. 可选地调用 TableHeadersRow() 来提交表头行。列名来自 TableSetupColumn() 数据。
    // - 5. 填充内容：
    //    - 在大多数情况下，您可以使用 TableNextRow() + TableSetColumnIndex(N) 来开始在列中添加内容。
    //    - 如果您使用表格作为一种网格，其中每列包含相同类型的内容，
    //      您可能更倾向于使用 TableNextColumn() 而不是 TableNextRow() + TableSetColumnIndex()。
    //      TableNextColumn() 将在需要时自动换到下一行。
    //    - 重要：与旧的 Columns() API 相比，我们需要为第一列调用 TableNextColumn()！
    //    - 调用流程总结：
    //        - TableNextRow() -> TableSetColumnIndex(0) -> Text("Hello 0") -> TableSetColumnIndex(1) -> Text("Hello 1")  // OK
    //        - TableNextRow() -> TableNextColumn()      -> Text("Hello 0") -> TableNextColumn()      -> Text("Hello 1")  // OK
    //        -                   TableNextColumn()      -> Text("Hello 0") -> TableNextColumn()      -> Text("Hello 1")  // OK: TableNextColumn() 自动进入下一行！
    //        - TableNextRow()                           -> Text("Hello 0")                                               // 不推荐！缺少 TableSetColumnIndex() 或 TableNextColumn()！Text 不会显示！
    // - 6. 调用 EndTable()
    IMGUI_API bool BeginTable(const char *str_id, int columns, ImGuiTableFlags flags = 0, const ImVec2 &outer_size = ImVec2(0.0f, 0.0f), float inner_width = 0.0f);
    IMGUI_API void EndTable();                                                                  // 只有在 BeginTable() 返回 true 时才调用 EndTable()！
    IMGUI_API void TableNextRow(ImGuiTableRowFlags row_flags = 0, float min_row_height = 0.0f); // 在新行的第一个单元格中追加内容。
    IMGUI_API bool TableNextColumn();                                                           // 在下一个列中追加内容（如果当前是最后一列，则进入下一行的第一列）。当列可见时返回 true。
    IMGUI_API bool TableSetColumnIndex(int column_n);                                           // 在指定的列中追加内容。当列可见时返回 true。

    // 表格：表头和列声明
    // - 使用 TableSetupColumn() 来指定标签、调整大小策略、默认宽度/权重、ID、各种其他标志等。
    // - 使用 TableHeadersRow() 创建一个表头行，并自动为每一列提交 TableHeader()。
    //   表头是进行重新排序、排序和打开上下文菜单的必要条件。
    //   上下文菜单也可以通过 ImGuiTableFlags_ContextMenuInBody 在列的主体中提供。
    // - 您可以手动提交表头，使用 TableNextRow() + TableHeader() 调用，但这仅在一些高级用例中有用
    //   （例如在表头行中添加自定义小部件）。
    // - 使用 TableSetupScrollFreeze() 锁定列/行，使其在滚动时保持可见。
    IMGUI_API void TableSetupColumn(const char *label, ImGuiTableColumnFlags flags = 0, float init_width_or_weight = 0.0f, ImGuiID user_id = 0);
    IMGUI_API void TableSetupScrollFreeze(int cols, int rows); // 锁定列/行，使其在滚动时保持可见。
    IMGUI_API void TableHeader(const char *label);             // 手动提交一个表头单元格（很少使用）
    IMGUI_API void TableHeadersRow();                          // 提交一行表头单元格，基于数据提交到 TableSetupColumn() + 提交上下文菜单
    IMGUI_API void TableAngledHeadersRow();                    // 提交一行带有角度表头的列，每一列都带有 ImGuiTableColumnFlags_AngledHeader 标志。必须是第一行。

    // 表格：排序和其他功能
    // - 排序：调用 TableGetSortSpecs() 来检索表格的最新排序规范。未排序时返回 NULL。
    //   当 'sort_specs->SpecsDirty == true' 时，您应该对数据进行排序。如果排序规范自上次调用以来发生变化，或者第一次调用时，'SpecsDirty' 会为 true。
    //   确保在排序后设置 'SpecsDirty = false'，否则您可能每一帧都浪费时间排序数据！
    // - 函数参数 'int column_n' 将默认值 -1 视为与传递当前列索引相同。
    IMGUI_API ImGuiTableSortSpecs *TableGetSortSpecs();                                        // 获取表格的最新排序规范（如果未排序，则返回 NULL）。生命周期：不要在多个帧中持有此指针或任何随后的 BeginTable() 调用后持有此指针。
    IMGUI_API int TableGetColumnCount();                                                       // 返回列数（传递给 BeginTable 的值）
    IMGUI_API int TableGetColumnIndex();                                                       // 返回当前列索引。
    IMGUI_API int TableGetRowIndex();                                                          // 返回当前行索引。
    IMGUI_API const char *TableGetColumnName(int column_n = -1);                               // 返回 "" 如果列没有通过 TableSetupColumn() 声明名称。传递 -1 使用当前列。
    IMGUI_API ImGuiTableColumnFlags TableGetColumnFlags(int column_n = -1);                    // 返回列标志，以便查询它们的启用/可见/排序/悬停状态标志。传递 -1 使用当前列。
    IMGUI_API void TableSetColumnEnabled(int column_n, bool v);                                // 更改列的用户可访问启用/禁用状态。设置为 false 隐藏该列。用户可以通过上下文菜单自行更改此状态（右键点击表头或右键点击表格主体中的列，启用 ImGuiTableFlags_ContextMenuInBody）。
    IMGUI_API int TableGetHoveredColumn();                                                     // 返回悬停的列。如果表格未悬停，则返回 -1。如果悬停在可见列右侧的未使用空间，则返回列数。也可以使用 (TableGetColumnFlags() & ImGuiTableColumnFlags_IsHovered) 代替。
    IMGUI_API void TableSetBgColor(ImGuiTableBgTarget target, ImU32 color, int column_n = -1); // 更改单元格、行或列的背景色。详见 ImGuiTableBgTarget_ 标志。

    // 传统列 API（建议使用表格！）
    // - 您还可以使用 SameLine(pos_x) 来模拟简化的列布局。
    IMGUI_API void Columns(int count = 1, const char *id = NULL, bool borders = true); // 创建列，设置列数、列ID和是否显示边框
    IMGUI_API void NextColumn();                                                       // 移动到下一个列，默认是当前行或当前行结束后移动到下一行
    IMGUI_API int GetColumnIndex();                                                    // 获取当前列的索引
    IMGUI_API float GetColumnWidth(int column_index = -1);                             // 获取列宽（单位：像素）。传递 -1 使用当前列
    IMGUI_API void SetColumnWidth(int column_index, float width);                      // 设置列宽（单位：像素）。传递 -1 使用当前列
    IMGUI_API float GetColumnOffset(int column_index = -1);                            // 获取列线的位置（单位：像素，从内容区域的左边开始）。传递 -1 使用当前列，否则传递 0 到 GetColumnsCount() 范围内的列索引。列 0 通常为 0.0f
    IMGUI_API void SetColumnOffset(int column_index, float offset_x);                  // 设置列线的位置（单位：像素，从内容区域的左边开始）。传递 -1 使用当前列
    IMGUI_API int GetColumnsCount();                                                   // 获取列数

    // 标签栏、标签
    // - 注意：标签是由停靠系统自动创建的（在“停靠”分支中）。使用此功能自行创建标签栏/标签。
    IMGUI_API bool BeginTabBar(const char *str_id, ImGuiTabBarFlags flags = 0);                       // 创建并附加到一个标签栏
    IMGUI_API void EndTabBar();                                                                       // 仅当 BeginTabBar() 返回 true 时调用 EndTabBar()
    IMGUI_API bool BeginTabItem(const char *label, bool *p_open = NULL, ImGuiTabItemFlags flags = 0); // 创建一个标签。返回 true 如果该标签被选中。
    IMGUI_API void EndTabItem();                                                                      // 仅当 BeginTabItem() 返回 true 时调用 EndTabItem()
    IMGUI_API bool TabItemButton(const char *label, ImGuiTabItemFlags flags = 0);                     // 创建一个行为像按钮的标签。点击时返回 true。无法在标签栏中选择。
    IMGUI_API void SetTabItemClosed(const char *tab_or_docked_window_label);                          // 在标签/窗口被关闭时通知标签栏或停靠系统（有助于减少可排序标签栏的视觉闪烁）。对于标签栏：在 BeginTabBar() 后、Tab 提交之前调用。否则，使用窗口名称调用。

    // 停靠
    // [BETA API] 通过 io.ConfigFlags |= ImGuiConfigFlags_DockingEnable 启用。
    // 注意：您可以在不调用任何 API 的情况下使用大多数停靠功能。您不需要调用 DockSpace() 来使用停靠！
    // - 从窗口标题栏或标签拖动以停靠/取消停靠。按住 SHIFT 键可禁用停靠。
    // - 从窗口菜单按钮（左上角按钮）拖动可以取消停靠整个节点（所有窗口）。
    // - 当 io.ConfigDockingWithShift == true 时，您需要按住 SHIFT 键来启用停靠。
    // 关于停靠空间：
    // - 使用 DockSpaceOverViewport() 创建一个覆盖整个屏幕或特定视口的窗口，并在其中包含一个停靠空间。
    //   通常与 ImGuiDockNodeFlags_PassthruCentralNode 一起使用，以使其透明。
    // - 使用 DockSpace() 在现有窗口中创建一个显式的停靠节点。有关详细信息，请参见停靠示例。
    // - 重要：停靠空间需要在任何它可以承载的窗口之前提交。请在帧的早期提交它！
    // - 重要：如果停靠空间被隐藏，它需要保持存在，否则停靠到它的窗口将被取消停靠。
    //   例如，如果您有多个标签，每个标签内都有一个停靠空间：使用 ImGuiDockNodeFlags_KeepAliveOnly 提交不可见的停靠空间。
    IMGUI_API ImGuiID DockSpace(ImGuiID dockspace_id, const ImVec2 &size = ImVec2(0, 0), ImGuiDockNodeFlags flags = 0, const ImGuiWindowClass *window_class = NULL);
    IMGUI_API ImGuiID DockSpaceOverViewport(ImGuiID dockspace_id = 0, const ImGuiViewport *viewport = NULL, ImGuiDockNodeFlags flags = 0, const ImGuiWindowClass *window_class = NULL);
    IMGUI_API void SetNextWindowDockID(ImGuiID dock_id, ImGuiCond cond = 0); // 设置下一个窗口的停靠ID
    IMGUI_API void SetNextWindowClass(const ImGuiWindowClass *window_class); // 设置下一个窗口的类（控制停靠兼容性 + 通过自定义视口标志和平台父子关系向平台后端提供提示）
    IMGUI_API ImGuiID GetWindowDockID();
    IMGUI_API bool IsWindowDocked(); // 当前窗口是否已停靠到另一个窗口？

    // 日志/捕获
    // - 所有来自界面的文本输出都可以捕获到 tty/文件/剪贴板。默认情况下，树节点在日志过程中会自动展开。
    IMGUI_API void LogToTTY(int auto_open_depth = -1);                               // 开始日志记录到 tty（stdout）
    IMGUI_API void LogToFile(int auto_open_depth = -1, const char *filename = NULL); // 开始日志记录到文件
    IMGUI_API void LogToClipboard(int auto_open_depth = -1);                         // 开始日志记录到操作系统剪贴板
    IMGUI_API void LogFinish();                                                      // 停止日志记录（关闭文件等）
    IMGUI_API void LogButtons();                                                     // 显示用于日志记录到 tty/文件/剪贴板的按钮的辅助函数
    IMGUI_API void LogText(const char *fmt, ...) IM_FMTARGS(1);                      // 直接将文本数据传递到日志（不显示）
    IMGUI_API void LogTextV(const char *fmt, va_list args) IM_FMTLIST(1);

    // 拖放
    // - 在源项目上，调用 BeginDragDropSource()，如果它返回 true，则调用 SetDragDropPayload() + EndDragDropSource()。
    // - 在目标候选项上，调用 BeginDragDropTarget()，如果它返回 true，则调用 AcceptDragDropPayload() + EndDragDropTarget()。
    // - 如果停止调用 BeginDragDropSource()，则有效负载将被保留，但不会显示预览工具提示（我们目前显示的是回退的 "..." 工具提示，见 #1725）。
    // - 一个项目可以既是拖动源又是放置目标。
    IMGUI_API bool BeginDragDropSource(ImGuiDragDropFlags flags = 0);                                     // 在提交可能被拖动的项目后调用。如果返回 true，则可以调用 SetDragDropPayload() + EndDragDropSource()
    IMGUI_API bool SetDragDropPayload(const char *type, const void *data, size_t sz, ImGuiCond cond = 0); // type 是用户定义的最大长度为 32 字符的字符串。以 '_' 开头的字符串为 dear imgui 内部类型。数据被复制并由 imgui 持有。当负载被接受时返回 true。
    IMGUI_API void EndDragDropSource();                                                                   // 仅当 BeginDragDropSource() 返回 true 时调用 EndDragDropSource()
    IMGUI_API bool BeginDragDropTarget();                                                                 // 在提交可能接收有效负载的项目后调用。如果返回 true，则可以调用 AcceptDragDropPayload() + EndDragDropTarget()
    IMGUI_API const ImGuiPayload *AcceptDragDropPayload(const char *type, ImGuiDragDropFlags flags = 0);  // 接受给定类型的内容。如果设置了 ImGuiDragDropFlags_AcceptBeforeDelivery，则可以在鼠标按钮释放之前查看有效负载。
    IMGUI_API void EndDragDropTarget();                                                                   // 仅当 BeginDragDropTarget() 返回 true 时调用 EndDragDropTarget()
    IMGUI_API const ImGuiPayload *GetDragDropPayload();                                                   // 从任何地方直接查看当前有效负载。在拖放完成或不活跃时返回 NULL。使用 ImGuiPayload::IsDataType() 来测试有效负载类型。

    // 禁用 [BETA API]
    // - 禁用所有用户交互并使项目信息变暗（在当前颜色上应用 style.DisabledAlpha）
    // - 这些可以嵌套，但不能用于启用已禁用的部分（栈中一个 BeginDisabled(true) 就足以保持所有项禁用）
    // - 弹出工具提示窗口例外，不会被禁用。
    // - BeginDisabled(false)/EndDisabled() 本质上不做任何事，但为了简化布尔表达式的使用（作为微优化：如果你有成千上万的 BeginDisabled(false)/EndDisabled() 配对，你可能需要重新构思代码，以避免多次调用这些函数）
    IMGUI_API void BeginDisabled(bool disabled = true);
    IMGUI_API void EndDisabled();

    // 裁剪
    // - 鼠标悬停受 ImGui::PushClipRect() 调用的影响，不像直接调用 ImDrawList::PushClipRect() 那样，它只影响渲染。
    IMGUI_API void PushClipRect(const ImVec2 &clip_rect_min, const ImVec2 &clip_rect_max, bool intersect_with_current_clip_rect);
    IMGUI_API void PopClipRect();

    // 聚焦，激活
    IMGUI_API void SetItemDefaultFocus();                // 使最后一个项成为新出现窗口的默认聚焦项。
    IMGUI_API void SetKeyboardFocusHere(int offset = 0); // 聚焦键盘到下一个小部件。使用正的 'offset' 访问多个组件小部件的子组件。使用 -1 访问前一个小部件。

    // 键盘/游戏手柄导航
    IMGUI_API void SetNavCursorVisible(bool visible); // 改变键盘/游戏手柄光标的可见性。默认情况下：使用箭头键时显示，点击鼠标时隐藏。

    // 重叠模式
    IMGUI_API void SetNextItemAllowOverlap(); // 允许下一个项被后续项覆盖。对不可见按钮、可选择项、树节点覆盖的区域特别有用，此区域可能需要添加后续项。注意，Selectable() 和 TreeNode() 有专门的标志来实现这一功能。

    // 项目/小部件实用工具和查询函数
    // - 大多数函数引用的是之前提交的项。
    // - 在 "Widgets->Querying Status" 的演示窗口中可以交互式地可视化大多数这些函数。
    IMGUI_API bool IsItemHovered(ImGuiHoveredFlags flags = 0);       // 判断最后一个项是否被悬停？（且可用，即未被弹出窗口等阻挡）。有关更多选项，请参见 ImGuiHoveredFlags。
    IMGUI_API bool IsItemActive();                                   // 判断最后一个项是否激活？（例如按钮被按下，文本框正在编辑。当鼠标按钮按住一个项目时，它将持续返回 true。没有交互的项目始终返回 false）。
    IMGUI_API bool IsItemFocused();                                  // 判断最后一个项是否为键盘/游戏手柄导航焦点？
    IMGUI_API bool IsItemClicked(ImGuiMouseButton mouse_button = 0); // 判断最后一个项是否被鼠标点击并悬停？（**） == IsMouseClicked(mouse_button) && IsItemHovered()，重要。（**）这与例如 Button() 的行为不同。请阅读函数定义中的注释。
    IMGUI_API bool IsItemVisible();                                  // 判断最后一个项是否可见？（项目可能因为裁剪/滚动而不在视野内）。
    IMGUI_API bool IsItemEdited();                                   // 判断最后一个项是否在此帧修改了其底层值或被按下？这通常与许多小部件的 "bool" 返回值相同。
    IMGUI_API bool IsItemActivated();                                // 判断最后一个项是否刚刚被激活（项目之前是非活动的）。
    IMGUI_API bool IsItemDeactivated();                              // 判断最后一个项是否刚刚被禁用（项目之前是活动的）。对于需要连续编辑的小部件，适用于撤销/重做模式。
    IMGUI_API bool IsItemDeactivatedAfterEdit();                     // 判断最后一个项是否在编辑后禁用并更改了其值？（例如，Slider/Drag 被移动）。对于需要连续编辑的小部件，适用于撤销/重做模式。请注意，你可能会得到误报（某些小部件，如 Combo()/ListBox()/Selectable()，即使单击已选项目，也会返回 true）。
    IMGUI_API bool IsItemToggledOpen();                              // 判断最后一个项的打开状态是否被切换？由 TreeNode() 设置。
    IMGUI_API bool IsAnyItemHovered();                               // 判断是否有任何项被悬停？
    IMGUI_API bool IsAnyItemActive();                                // 判断是否有任何项被激活？
    IMGUI_API bool IsAnyItemFocused();                               // 判断是否有任何项被聚焦？
    IMGUI_API ImGuiID GetItemID();                                   // 获取最后一个项的 ID（~~ 通常与事先调用 ImGui::GetID(label) 相同）。
    IMGUI_API ImVec2 GetItemRectMin();                               // 获取最后一个项的左上角边界矩形（屏幕空间）。
    IMGUI_API ImVec2 GetItemRectMax();                               // 获取最后一个项的右下角边界矩形（屏幕空间）。
    IMGUI_API ImVec2 GetItemRectSize();                              // 获取最后一个项的大小。

    // 视口
    // - 当前表示由应用程序创建的托管 Dear ImGui 窗口的 Platform Window。
    // - 在 'docking' 分支中启用多视口时，我们将这个概念扩展为多个活动视口。
    // - 未来我们将进一步扩展这个概念，使其代表 Platform Monitor，并支持 "无主平台窗口" 操作模式。
    IMGUI_API ImGuiViewport *GetMainViewport(); // 返回主视口/默认视口。此视口永远不为 NULL。

    // 背景/前景绘制列表
    IMGUI_API ImDrawList *GetBackgroundDrawList(ImGuiViewport *viewport = NULL); // 获取给定视口或与当前窗口关联的视口的背景绘制列表。该绘制列表将是第一个渲染的。适用于快速在 Dear ImGui 内容后绘制形状/文本。
    IMGUI_API ImDrawList *GetForegroundDrawList(ImGuiViewport *viewport = NULL); // 获取给定视口或与当前窗口关联的视口的前景绘制列表。该绘制列表将是最上层渲染的。适用于快速在 Dear ImGui 内容上绘制形状/文本。

    // 杂项实用工具
    IMGUI_API bool IsRectVisible(const ImVec2 &size);                             // 测试矩形（给定大小，从光标位置开始）是否可见/未被裁剪。
    IMGUI_API bool IsRectVisible(const ImVec2 &rect_min, const ImVec2 &rect_max); // 测试矩形（在屏幕空间中）是否可见/未被裁剪。用于在用户端执行粗略裁剪。
    IMGUI_API double GetTime();                                                   // 获取全局 ImGui 时间。每帧由 io.DeltaTime 增量。
    IMGUI_API int GetFrameCount();                                                // 获取全局 ImGui 帧计数。每帧增加 1。
    IMGUI_API ImDrawListSharedData *GetDrawListSharedData();                      // 创建自己的 ImDrawList 实例时可以使用此数据。
    IMGUI_API const char *GetStyleColorName(ImGuiCol idx);                        // 获取与枚举值对应的字符串（用于显示、保存等）。
    IMGUI_API void SetStateStorage(ImGuiStorage *storage);                        // 替换当前窗口存储为我们自己的（如果你想自己操作它，通常清除它的子部分）。
    IMGUI_API ImGuiStorage *GetStateStorage();

    // 文本实用工具
    IMGUI_API ImVec2 CalcTextSize(const char *text, const char *text_end = NULL, bool hide_text_after_double_hash = false, float wrap_width = -1.0f);

    // 颜色实用工具
    IMGUI_API ImVec4 ColorConvertU32ToFloat4(ImU32 in);
    IMGUI_API ImU32 ColorConvertFloat4ToU32(const ImVec4 &in);
    IMGUI_API void ColorConvertRGBtoHSV(float r, float g, float b, float &out_h, float &out_s, float &out_v);
    IMGUI_API void ColorConvertHSVtoRGB(float h, float s, float v, float &out_r, float &out_g, float &out_b);

    // 输入实用工具：键盘/鼠标/游戏手柄
    // - ImGuiKey 枚举包含所有可能的键盘、鼠标和游戏手柄输入（例如 ImGuiKey_A、ImGuiKey_MouseLeft、ImGuiKey_GamepadDpadUp 等）。
    // - （过时：在 v1.87 之前，我们使用 ImGuiKey 来传递每个后端定义的本地/用户索引。此功能在 1.87（2022-02）中被废弃，并在 1.91.5（2024-11）中完全移除。详见 https://github.com/ocornut/imgui/issues/4921）
    // - （过时：使用 ImGuiKey 时，如果 key < 512，会触发断言来检测是否传递了旧版本地/用户索引）
    IMGUI_API bool IsKeyDown(ImGuiKey key);                                          // 键是否被按住。
    IMGUI_API bool IsKeyPressed(ImGuiKey key, bool repeat = true);                   // 键是否被按下（从未按下到按下）。如果 repeat=true，使用 io.KeyRepeatDelay / KeyRepeatRate。
    IMGUI_API bool IsKeyReleased(ImGuiKey key);                                      // 键是否被释放（从按下到未按下）？
    IMGUI_API bool IsKeyChordPressed(ImGuiKeyChord key_chord);                       // 键组合（修饰键 + 键）是否被按下，例如你可以传递 'ImGuiMod_Ctrl | ImGuiKey_S' 作为键组合。此函数不进行路由或焦点检查，请考虑使用 Shortcut() 函数。
    IMGUI_API int GetKeyPressedAmount(ImGuiKey key, float repeat_delay, float rate); // 使用提供的重复延迟/速率。返回一个计数，通常是 0 或 1，但如果重复速率足够小且 DeltaTime > 重复速率，可能会大于 1。
    IMGUI_API const char *GetKeyName(ImGuiKey key);                                  // [调试] 返回按键的英文名称。此名称仅用于调试，不应持久保存或进行比较。
    IMGUI_API void SetNextFrameWantCaptureKeyboard(bool want_capture_keyboard);      // 重写下一个帧的 io.WantCaptureKeyboard 标志（该标志留给应用程序处理，通常当值为 true 时，它指示应用程序忽略输入）。例如，当你的控件被悬停时强制捕获键盘。等同于在下一个 NewFrame() 调用后设置 "io.WantCaptureKeyboard = want_capture_keyboard"。

    // 输入实用工具：快捷键测试与路由 [BETA]
    // - ImGuiKeyChord = ImGuiKey + 可选的 ImGuiMod_Alt / ImGuiMod_Ctrl / ImGuiMod_Shift / ImGuiMod_Super。
    //       ImGuiKey_C                          // 被接受的 ImGuiKey 或 ImGuiKeyChord 参数函数
    //       ImGuiMod_Ctrl | ImGuiKey_C          // 被接受的 ImGuiKeyChord 参数函数
    //   只有 ImGuiMod_XXX 值可以与 ImGuiKey 组合。你不能将两个 ImGuiKey 值组合在一起。
    // - 一般思想是，多个调用者可能对一个快捷键注册感兴趣，最终只有一个所有者获得它。
    //      父控件   -> 调用 Shortcut(Ctrl+S)    // 当父控件获得焦点时，父控件获取该快捷键。
    //        子控件1 -> 调用 Shortcut(Ctrl+S)    // 当子控件1获得焦点时，子控件1获取该快捷键（子控件1会覆盖父控件的快捷键）
    //        子控件2 -> 不调用                  // 当子控件2获得焦点时，父控件获得该快捷键。
    //   整个系统是顺序无关的，因此即使子控件1先调用，再调用父控件，结果也是一致的。
    //   这是一个重要特性，有助于处理外部代码或更大的代码库。
    // - 理解区别：
    //   - IsKeyChordPressed() 比较修饰键并调用 IsKeyPressed() -> 函数没有副作用。
    //   - Shortcut() 提交一个路由，路由被解析，如果当前可以路由，它会调用 IsKeyChordPressed() -> 函数有副作用（有助于防止其他调用获取该路由）。
    // - 在 'Metrics/Debugger->Inputs' 中可视化注册的路由。
    IMGUI_API bool Shortcut(ImGuiKeyChord key_chord, ImGuiInputFlags flags = 0);
    IMGUI_API void SetNextItemShortcut(ImGuiKeyChord key_chord, ImGuiInputFlags flags = 0);

    // 输入实用工具：键盘/输入所有权 [BETA]
    // - 一个常见的用例是允许你的控件禁用标准的输入行为，例如禁用 Tab 键或 Alt 键处理，鼠标滚轮滚动等。
    //   例如：Button(...); SetItemKeyOwner(ImGuiKey_MouseWheelY); 使得悬停/激活按钮时禁用滚轮滚动。
    // - 提醒：ImGuiKey 枚举包括访问鼠标按钮和游戏手柄，因此键所有权也适用于它们。
    // - 许多相关功能仍然在 imgui_internal.h 中。例如，大多数 IsKeyXXX() / IsMouseXXX() 函数都有一个与所有者 ID 相关的版本。
    IMGUI_API void SetItemKeyOwner(ImGuiKey key); // 设置键的所有权给最后一个被悬停或激活的控件的 ID。等同于 'if (IsItemHovered() || IsItemActive()) { SetKeyOwner(key, GetItemID());'。

    // 输入实用工具：鼠标
    // - 要引用鼠标按钮，可以在代码中使用命名的枚举，例如 ImGuiMouseButton_Left、ImGuiMouseButton_Right。
    // - 你也可以使用普通的整数：永远保证 0=左键，1=右键，2=中键。
    // - 拖动操作只有在鼠标移动超过初始点击位置的某个距离后才会报告（请参阅 'lock_threshold' 和 'io.MouseDraggingThreshold'）
    IMGUI_API bool IsMouseDown(ImGuiMouseButton button);                                            // 鼠标按钮是否按下？
    IMGUI_API bool IsMouseClicked(ImGuiMouseButton button, bool repeat = false);                    // 鼠标按钮是否被点击？（从未按下到按下）。与 GetMouseClickedCount() == 1 相同。
    IMGUI_API bool IsMouseReleased(ImGuiMouseButton button);                                        // 鼠标按钮是否被释放？（从按下到未按下）
    IMGUI_API bool IsMouseDoubleClicked(ImGuiMouseButton button);                                   // 鼠标按钮是否被双击？与 GetMouseClickedCount() == 2 相同。（注意：双击也会报告 IsMouseClicked() == true）
    IMGUI_API int GetMouseClickedCount(ImGuiMouseButton button);                                    // 返回在点击发生时连续点击的次数（否则返回 0）。
    IMGUI_API bool IsMouseHoveringRect(const ImVec2 &r_min, const ImVec2 &r_max, bool clip = true); // 鼠标是否悬停在给定的矩形区域内（在屏幕空间中）。会根据当前的剪裁设置进行裁剪，但忽略其他焦点/窗口顺序/弹出窗口的考虑。
    IMGUI_API bool IsMousePosValid(const ImVec2 *mouse_pos = NULL);                                 // 根据约定，使用 (-FLT_MAX,-FLT_MAX) 来表示没有可用的鼠标。
    IMGUI_API bool IsAnyMouseDown();                                                                // [即将废弃] 是否有任何鼠标按钮按下？这是为后端设计的，但更推荐让后端维护按下鼠标按钮的掩码，因为即将到来的输入队列系统将使这个方法失效。
    IMGUI_API ImVec2 GetMousePos();                                                                 // 获取鼠标位置，等同于 ImGui::GetIO().MousePos，为了与其他调用一致。
    IMGUI_API ImVec2 GetMousePosOnOpeningCurrentPopup();                                            // 获取弹出窗口打开时的鼠标位置（帮助避免用户自己备份该值）。
    IMGUI_API bool IsMouseDragging(ImGuiMouseButton button, float lock_threshold = -1.0f);          // 鼠标是否正在拖动？（如果 lock_threshold < 0.0f，则使用 io.MouseDraggingThreshold）
    IMGUI_API ImVec2 GetMouseDragDelta(ImGuiMouseButton button = 0, float lock_threshold = -1.0f);  // 返回从初始点击位置到鼠标当前位置的位移。直到鼠标超过一个距离阈值才会返回非零值（如果 lock_threshold < 0.0f，则使用 io.MouseDraggingThreshold）。
    IMGUI_API void ResetMouseDragDelta(ImGuiMouseButton button = 0);                                // 重置鼠标拖动的位移。
    IMGUI_API ImGuiMouseCursor GetMouseCursor();                                                    // 获取期望的鼠标光标形状。重要：在 ImGui::NewFrame() 中重置此值，且在帧期间会更新。渲染前有效。如果你使用软件渲染并设置 io.MouseDrawCursor，ImGui 会为你渲染鼠标光标。
    IMGUI_API void SetMouseCursor(ImGuiMouseCursor cursor_type);                                    // 设置期望的鼠标光标形状。
    IMGUI_API void SetNextFrameWantCaptureMouse(bool want_capture_mouse);                           // 重写下一个帧的 io.WantCaptureMouse 标志（该标志留给应用程序处理，通常当值为 true 时，它指示应用程序忽略输入）。等同于在下一个 NewFrame() 调用后设置 "io.WantCaptureMouse = want_capture_mouse;"。

    // 剪贴板实用工具
    // - 还可以查看 LogToClipboard() 函数，用于将 GUI 捕获到剪贴板，或者轻松地将文本数据输出到剪贴板。
    IMGUI_API const char *GetClipboardText();          // 获取剪贴板中的文本内容。
    IMGUI_API void SetClipboardText(const char *text); // 设置剪贴板的文本内容。

    // 设置/.Ini 实用工具
    // - 如果 io.IniFilename != NULL（默认值为 "imgui.ini"），则磁盘操作会自动执行。
    // - 将 io.IniFilename 设置为 NULL 以手动加载/保存。阅读 io.WantSaveIniSettings 说明，了解如何手动处理 .ini 保存。
    // - 重要提示：默认值 "imgui.ini" 是相对于当前工作目录的！大多数应用程序将希望将其锁定到绝对路径（例如，与可执行文件相同的路径）。
    IMGUI_API void LoadIniSettingsFromDisk(const char *ini_filename);                    // 在 CreateContext() 后、首次调用 NewFrame() 之前调用。NewFrame() 会自动调用 LoadIniSettingsFromDisk(io.IniFilename)。
    IMGUI_API void LoadIniSettingsFromMemory(const char *ini_data, size_t ini_size = 0); // 在 CreateContext() 后、首次调用 NewFrame() 之前调用，以提供来自自定义数据源的 .ini 数据。
    IMGUI_API void SaveIniSettingsToDisk(const char *ini_filename);                      // 在任何修改后（如果 io.IniFilename 非空），几秒钟后自动调用，以将修改反映到 .ini 文件中（DestroyContext() 也会调用该方法）。
    IMGUI_API const char *SaveIniSettingsToMemory(size_t *out_ini_size = NULL);          // 返回零终止字符串，包含 .ini 数据，可以自行保存。调用时 io.WantSaveIniSettings 被设置，然后以自己的方式保存数据，并清除 io.WantSaveIniSettings。

    // 调试工具
    // - 你的主要调试工具是 ShowMetricsWindow() 函数，也可以通过 Demo->Tools->Metrics Debugger 访问。
    IMGUI_API void DebugTextEncoding(const char *text);                                                                                                                           // 调试文本编码。
    IMGUI_API void DebugFlashStyleColor(ImGuiCol idx);                                                                                                                            // 调试闪烁样式颜色。
    IMGUI_API void DebugStartItemPicker();                                                                                                                                        // 开始调试项目选择器。
    IMGUI_API bool DebugCheckVersionAndDataLayout(const char *version_str, size_t sz_io, size_t sz_style, size_t sz_vec2, size_t sz_vec4, size_t sz_drawvert, size_t sz_drawidx); // 由 IMGUI_CHECKVERSION() 宏调用，用于检查版本和数据布局。
#ifndef IMGUI_DISABLE_DEBUG_TOOLS
    IMGUI_API void DebugLog(const char *fmt, ...) IM_FMTARGS(1);           // 调用 IMGUI_DEBUG_LOG() 以最大限度地减少调用代码中的剥离。
    IMGUI_API void DebugLogV(const char *fmt, va_list args) IM_FMTLIST(1); // 调用带有可变参数列表的调试日志。
#endif

    // 内存分配器
    // - 这些函数与当前上下文无关。
    // - DLL 用户：堆和全局变量在 DLL 边界之间不会共享！你需要为每个静态/DLL 边界调用 SetCurrentContext() 和 SetAllocatorFunctions()。
    //   详细信息请阅读 imgui.cpp 中的 "上下文和内存分配器" 部分。
    IMGUI_API void SetAllocatorFunctions(ImGuiMemAllocFunc alloc_func, ImGuiMemFreeFunc free_func, void *user_data = NULL);   // 设置内存分配函数。
    IMGUI_API void GetAllocatorFunctions(ImGuiMemAllocFunc *p_alloc_func, ImGuiMemFreeFunc *p_free_func, void **p_user_data); // 获取当前内存分配函数。
    IMGUI_API void *MemAlloc(size_t size);                                                                                    // 分配指定大小的内存。
    IMGUI_API void MemFree(void *ptr);                                                                                        // 释放指定的内存。

    // （可选）平台/操作系统接口，用于多视口支持
    // 请阅读 ImGuiPlatformIO 结构相关的注释，了解更多细节。
    // 注意：你可以使用 GetWindowViewport() 来获取当前窗口的视口。
    IMGUI_API void UpdatePlatformWindows();                                                                          // 在主循环中调用。将为每个次级视口调用 CreateWindow/ResizeWindow 等平台函数，并为每个非活动视口调用 DestroyWindow。
    IMGUI_API void RenderPlatformWindowsDefault(void *platform_render_arg = NULL, void *renderer_render_arg = NULL); // 在主循环中调用。将为每个没有设置 ImGuiViewportFlags_Minimized 标志的次级视口调用 RenderWindow/SwapBuffers 平台函数。可以由用户重新实现以满足自定义渲染需求。
    IMGUI_API void DestroyPlatformWindows();                                                                         // 为所有视口调用 DestroyWindow 平台函数。如果你需要在 ImGui 关闭之前关闭平台窗口，请从后台的 Shutdown() 调用该函数。否则将在 DestroyContext() 中调用。
    IMGUI_API ImGuiViewport *FindViewportByID(ImGuiID id);                                                           // 帮助函数，供后端使用。
    IMGUI_API ImGuiViewport *FindViewportByPlatformHandle(void *platform_handle);                                    // 帮助函数，供后端使用。platform_handle 的类型由后端决定（例如 HWND、MyWindow*、GLFWwindow* 等）。

} // namespace ImGui

//-----------------------------------------------------------------------------
// [SECTION] Flags & Enumerations
//-----------------------------------------------------------------------------

// ImGui::Begin() 标志
// （这些是每个窗口的标志。ImGuiIO 中还有一些共享的标志：io.ConfigWindowsResizeFromEdges 和 io.ConfigWindowsMoveFromTitleBarOnly）
enum ImGuiWindowFlags_
{
    ImGuiWindowFlags_None = 0,                                                                                                                            // 无标志
    ImGuiWindowFlags_NoTitleBar = 1 << 0,                                                                                                                 // 禁用标题栏
    ImGuiWindowFlags_NoResize = 1 << 1,                                                                                                                   // 禁用用户通过右下角拖拽调整大小
    ImGuiWindowFlags_NoMove = 1 << 2,                                                                                                                     // 禁用用户移动窗口
    ImGuiWindowFlags_NoScrollbar = 1 << 3,                                                                                                                // 禁用滚动条（窗口仍然可以通过鼠标或程序matically滚动）
    ImGuiWindowFlags_NoScrollWithMouse = 1 << 4,                                                                                                          // 禁用用户通过鼠标滚轮垂直滚动。在子窗口中，鼠标滚轮将转发到父窗口，除非同时设置了 NoScrollbar。
    ImGuiWindowFlags_NoCollapse = 1 << 5,                                                                                                                 // 禁用用户通过双击窗口标题栏进行折叠，也被称为窗口菜单按钮（例如在停靠节点中）。
    ImGuiWindowFlags_AlwaysAutoResize = 1 << 6,                                                                                                           // 每帧自动调整窗口大小以适应内容
    ImGuiWindowFlags_NoBackground = 1 << 7,                                                                                                               // 禁用绘制背景颜色（WindowBg 等）和外部边框。类似于使用 SetNextWindowBgAlpha(0.0f)。
    ImGuiWindowFlags_NoSavedSettings = 1 << 8,                                                                                                            // 永不加载/保存设置到 .ini 文件
    ImGuiWindowFlags_NoMouseInputs = 1 << 9,                                                                                                              // 禁用捕获鼠标、悬停测试和透传
    ImGuiWindowFlags_MenuBar = 1 << 10,                                                                                                                   // 窗口具有菜单栏
    ImGuiWindowFlags_HorizontalScrollbar = 1 << 11,                                                                                                       // 允许出现水平滚动条（默认关闭）。可以在调用 Begin() 前使用 SetNextWindowContentSize(ImVec2(width,0.0f)) 来指定宽度。可以参考 imgui_demo 中 "Horizontal Scrolling" 部分的代码。
    ImGuiWindowFlags_NoFocusOnAppearing = 1 << 12,                                                                                                        // 禁用从隐藏到可见状态时获取焦点
    ImGuiWindowFlags_NoBringToFrontOnFocus = 1 << 13,                                                                                                     // 禁用点击窗口或程序matically 给它焦点时将其带到最前面
    ImGuiWindowFlags_AlwaysVerticalScrollbar = 1 << 14,                                                                                                   // 始终显示垂直滚动条（即使 ContentSize.y < Size.y）
    ImGuiWindowFlags_AlwaysHorizontalScrollbar = 1 << 15,                                                                                                 // 始终显示水平滚动条（即使 ContentSize.x < Size.x）
    ImGuiWindowFlags_NoNavInputs = 1 << 16,                                                                                                               // 禁用窗口中的键盘/游戏手柄导航
    ImGuiWindowFlags_NoNavFocus = 1 << 17,                                                                                                                // 禁用通过键盘/游戏手柄导航聚焦此窗口（例如 CTRL+TAB 跳过此窗口）
    ImGuiWindowFlags_UnsavedDocument = 1 << 18,                                                                                                           // 在标题旁显示一个点。当在标签/停靠上下文中使用时，点击 X 时不关闭标签，而是选中标签（直到用户停止提交标签）。否则按下 X 时会关闭标签。
    ImGuiWindowFlags_NoDocking = 1 << 19,                                                                                                                 // 禁用此窗口的停靠功能
    ImGuiWindowFlags_NoNav = ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus,                                                                  // 禁用所有导航功能
    ImGuiWindowFlags_NoDecoration = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse, // 无装饰：无标题栏、无调整大小、无滚动条、无折叠
    ImGuiWindowFlags_NoInputs = ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus,                              // 禁用所有输入

    // [内部使用]
    ImGuiWindowFlags_ChildWindow = 1 << 24,  // 内部使用，供 BeginChild() 调用
    ImGuiWindowFlags_Tooltip = 1 << 25,      // 内部使用，供 BeginTooltip() 调用
    ImGuiWindowFlags_Popup = 1 << 26,        // 内部使用，供 BeginPopup() 调用
    ImGuiWindowFlags_Modal = 1 << 27,        // 内部使用，供 BeginPopupModal() 调用
    ImGuiWindowFlags_ChildMenu = 1 << 28,    // 内部使用，供 BeginMenu() 调用
    ImGuiWindowFlags_DockNodeHost = 1 << 29, // 内部使用，供 Begin()/NewFrame() 调用

// 过时的标志名称
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    ImGuiWindowFlags_AlwaysUseWindowPadding = 1 << 30, // 从 1.90.0 版本开始过时：在 BeginChild() 调用中使用 ImGuiChildFlags_AlwaysUseWindowPadding
    ImGuiWindowFlags_NavFlattened = 1 << 31,           // 从 1.90.9 版本开始过时：在 BeginChild() 调用中使用 ImGuiChildFlags_NavFlattened
#endif
};

// ImGui::BeginChild() 标志
// （遗留：位 0 必须始终对应 ImGuiChildFlags_Borders，以便与旧 API 兼容，旧 API 使用 'bool border = false'）
// 关于使用 AutoResizeX/AutoResizeY 标志：
// - 可以与 SetNextWindowSizeConstraints() 一起使用，以为每个轴设置最小/最大尺寸（请参阅“Demo->Child->Auto-resize with Constraints”）。
// - 对于给定轴的尺寸测量，仅当子窗口位于可见边界内或刚刚出现时才会执行。
//   - 这使得 BeginChild() 在不在边界内时返回 false（例如在滚动时），这样更高效。但它不会在被裁剪时更新其自动大小。
//     尽管这样做并不完美，但它是更好的默认行为，因为始终开启的性能提升比偶尔的“重新调整大小”要更有价值。
//   - 您还可以使用 ImGuiChildFlags_AlwaysAutoResize 来强制更新，即使子窗口不在视图中。
//     但是，请理解，这样做会阻止 BeginChild() 永远返回 false，从而禁用粗略裁剪的好处。
enum ImGuiChildFlags_
{
    ImGuiChildFlags_None = 0,                        // 无标志
    ImGuiChildFlags_Borders = 1 << 0,                // 显示外部边框并启用 WindowPadding。（重要：这始终为 1，出于兼容性原因）
    ImGuiChildFlags_AlwaysUseWindowPadding = 1 << 1, // 即使没有绘制边框，也使用 style.WindowPadding 填充（默认情况下没有填充，因为没有边框的子窗口更合适）
    ImGuiChildFlags_ResizeX = 1 << 2,                // 允许从右边缘调整大小（布局方向）。启用 .ini 保存（除非 ImGuiWindowFlags_NoSavedSettings 传递给窗口标志）
    ImGuiChildFlags_ResizeY = 1 << 3,                // 允许从底部边缘调整大小（布局方向）。"
    ImGuiChildFlags_AutoResizeX = 1 << 4,            // 启用自动调整宽度。请参阅上面“重要：尺寸测量”部分的详细信息。
    ImGuiChildFlags_AutoResizeY = 1 << 5,            // 启用自动调整高度。请参阅上面“重要：尺寸测量”部分的详细信息。
    ImGuiChildFlags_AlwaysAutoResize = 1 << 6,       // 与 AutoResizeX/AutoResizeY 结合使用。始终测量尺寸，即使子窗口隐藏，也始终返回 true，始终禁用裁剪优化！不推荐使用。
    ImGuiChildFlags_FrameStyle = 1 << 7,             // 将子窗口样式化为框架项目：使用 FrameBg、FrameRounding、FrameBorderSize、FramePadding，而不是 ChildBg、ChildRounding、ChildBorderSize、WindowPadding。
    ImGuiChildFlags_NavFlattened = 1 << 8,           // [BETA] 共享焦点范围，允许键盘/游戏手柄导航跨越父边框到这个子窗口或兄弟子窗口之间。

// 过时的名称
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    ImGuiChildFlags_Border = ImGuiChildFlags_Borders, // 在 1.91.1 版本（2024 年 8 月）中重命名以保持一致性。
#endif
};

// ImGui::PushItemFlag() 标志
// （这些标志适用于所有项目）
enum ImGuiItemFlags_
{
    ImGuiItemFlags_None = 0,                   // 默认标志
    ImGuiItemFlags_NoTabStop = 1 << 0,         // 禁用键盘标签切换。这是 ImGuiItemFlags_NoNav 的“简化版”。
    ImGuiItemFlags_NoNav = 1 << 1,             // 禁用任何形式的聚焦（键盘/游戏手柄方向导航和 SetKeyboardFocusHere() 调用）。
    ImGuiItemFlags_NoNavDefaultFocus = 1 << 2, // 禁用项目作为默认焦点候选（例如，标题栏项目会用到这个标志）。
    ImGuiItemFlags_ButtonRepeat = 1 << 3,      // 启用按钮重复模式（基于 io.KeyRepeatDelay 和 io.KeyRepeatRate 的值）。请注意，您还可以在任何按钮之后调用 `IsItemActive()` 来判断该按钮是否处于被按住状态。
    ImGuiItemFlags_AutoClosePopups = 1 << 4,   // MenuItem()/Selectable() 会自动关闭其父弹出窗口。
    ImGuiItemFlags_AllowDuplicateId = 1 << 5,  // 允许提交一个与当前帧中已提交项目具有相同标识符的项目，而不会触发警告工具提示（如果 `io.ConfigDebugHighlightIdConflicts` 被设置）。
};

// ImGui::InputText() 标志
// （这些是针对每个项目的标志。ImGuiIO 中有共享标志：io.ConfigInputTextCursorBlink 和 io.ConfigInputTextEnterKeepActive）
enum ImGuiInputTextFlags_
{
    // 基本过滤器（也参见 ImGuiInputTextFlags_CallbackCharFilter）
    ImGuiInputTextFlags_None = 0,                  // 默认标志
    ImGuiInputTextFlags_CharsDecimal = 1 << 0,     // 允许 0123456789.+-*/
    ImGuiInputTextFlags_CharsHexadecimal = 1 << 1, // 允许 0123456789ABCDEFabcdef
    ImGuiInputTextFlags_CharsScientific = 1 << 2,  // 允许 0123456789.+-*/eE（科学记数法输入）
    ImGuiInputTextFlags_CharsUppercase = 1 << 3,   // 将 a..z 转换为 A..Z
    ImGuiInputTextFlags_CharsNoBlank = 1 << 4,     // 过滤空格和制表符

    // 输入选项
    ImGuiInputTextFlags_AllowTabInput = 1 << 5,       // 按 TAB 键时在文本框中输入 '\t' 字符
    ImGuiInputTextFlags_EnterReturnsTrue = 1 << 6,    // 按 Enter 键时返回 'true'（与每次修改值时不同）。建议使用 IsItemDeactivatedAfterEdit()！
    ImGuiInputTextFlags_EscapeClearsAll = 1 << 7,     // 按 Escape 键清空内容（如果不为空），否则取消编辑（与 Escape 键默认还原行为对比）
    ImGuiInputTextFlags_CtrlEnterForNewLine = 1 << 8, // 在多行模式下，按 Enter 键验证，按 Ctrl+Enter 键换行（默认相反：按 Ctrl+Enter 验证，按 Enter 换行）

    // 其他选项
    ImGuiInputTextFlags_ReadOnly = 1 << 9,            // 只读模式
    ImGuiInputTextFlags_Password = 1 << 10,           // 密码模式，显示所有字符为 '*'，禁用复制
    ImGuiInputTextFlags_AlwaysOverwrite = 1 << 11,    // 始终覆盖模式
    ImGuiInputTextFlags_AutoSelectAll = 1 << 12,      // 第一次获得鼠标焦点时自动选择整个文本
    ImGuiInputTextFlags_ParseEmptyRefVal = 1 << 13,   // 仅对 InputFloat()、InputInt()、InputScalar() 等有效：将空字符串解析为零值
    ImGuiInputTextFlags_DisplayEmptyRefVal = 1 << 14, // 仅对 InputFloat()、InputInt()、InputScalar() 等有效：当值为零时，不显示它。通常与 ImGuiInputTextFlags_ParseEmptyRefVal 一起使用
    ImGuiInputTextFlags_NoHorizontalScroll = 1 << 15, // 禁用水平滚动
    ImGuiInputTextFlags_NoUndoRedo = 1 << 16,         // 禁用撤销/重做。请注意，输入文本在激活时拥有文本数据，如果您要提供自己的撤销/重做堆栈，您需要例如调用 ClearActiveID()。

    // 显示省略/对齐选项
    ImGuiInputTextFlags_ElideLeft = 1 << 17, // 当文本不适应时，省略左侧以确保右侧可见。适用于路径/文件名。仅适用于单行！

    // 回调功能
    ImGuiInputTextFlags_CallbackCompletion = 1 << 18, // 按 TAB 键时回调（用于补全处理）
    ImGuiInputTextFlags_CallbackHistory = 1 << 19,    // 按上/下箭头时回调（用于历史记录处理）
    ImGuiInputTextFlags_CallbackAlways = 1 << 20,     // 每次迭代时回调。用户代码可以查询光标位置并修改文本缓冲区。
    ImGuiInputTextFlags_CallbackCharFilter = 1 << 21, // 按字符输入时回调以替换或丢弃字符。修改 'EventChar' 以替换或丢弃，或者在回调中返回 1 以丢弃字符。
    ImGuiInputTextFlags_CallbackResize = 1 << 22,     // 请求缓冲区容量更改时回调（超出 'buf_size' 参数值），允许字符串增长。当字符串希望被调整大小时会通知。您将在回调中获得新的 BufSize 并需要遵守它。（参见 misc/cpp/imgui_stdlib.h 中使用此功能的示例）
    ImGuiInputTextFlags_CallbackEdit = 1 << 23,       // 在任何编辑时回调（注意，InputText() 在编辑时已经返回 true，回调主要用于在焦点处于活动状态时操作底层缓冲区）

    // 废弃的名称
    // ImGuiInputTextFlags_AlwaysInsertMode  = ImGuiInputTextFlags_AlwaysOverwrite   // [在 1.82 中重命名] 之前的名称与行为不匹配
};

// ImGui::TreeNodeEx(), ImGui::CollapsingHeader*() 的标志
enum ImGuiTreeNodeFlags_
{
    ImGuiTreeNodeFlags_None = 0,                       // 默认标志
    ImGuiTreeNodeFlags_Selected = 1 << 0,              // 绘制为选中状态
    ImGuiTreeNodeFlags_Framed = 1 << 1,                // 绘制带有背景框（例如用于 CollapsingHeader）
    ImGuiTreeNodeFlags_AllowOverlap = 1 << 2,          // 允许后续小部件与此节点重叠（通过点击测试）
    ImGuiTreeNodeFlags_NoTreePushOnOpen = 1 << 3,      // 打开时不执行 TreePush()（例如用于 CollapsingHeader）= 不增加额外缩进，也不将节点压入 ID 堆栈
    ImGuiTreeNodeFlags_NoAutoOpenOnLog = 1 << 4,       // 在启用日志时不要自动打开节点（默认情况下，日志会自动打开树节点）
    ImGuiTreeNodeFlags_DefaultOpen = 1 << 5,           // 默认打开节点
    ImGuiTreeNodeFlags_OpenOnDoubleClick = 1 << 6,     // 双击时打开节点，而不是单击（默认情况下多重选择除非显式设置 _OpenOnXXX 行为）。可以同时使用这两种行为。
    ImGuiTreeNodeFlags_OpenOnArrow = 1 << 7,           // 点击箭头部分时打开节点（默认情况下多重选择除非显式设置 _OpenOnXXX 行为）。可以同时使用这两种行为。
    ImGuiTreeNodeFlags_Leaf = 1 << 8,                  // 无法折叠，无箭头（作为叶子节点的便利标志）。
    ImGuiTreeNodeFlags_Bullet = 1 << 9,                // 显示一个圆点而不是箭头。重要：如果没有设置 _Leaf 标志，节点仍然可以标记为打开/关闭！
    ImGuiTreeNodeFlags_FramePadding = 1 << 10,         // 使用 FramePadding（即使是未框定的文本节点）来垂直对齐文本基线至常规小部件高度。等同于在节点之前调用 AlignTextToFramePadding()。
    ImGuiTreeNodeFlags_SpanAvailWidth = 1 << 11,       // 扩展点击框至最右边，即使没有框架。默认情况下不是这样做，因为它允许在同一行上添加其他项目而无需使用 AllowOverlap 模式。
    ImGuiTreeNodeFlags_SpanFullWidth = 1 << 12,        // 扩展点击框至最左边和最右边（覆盖缩进区域）。
    ImGuiTreeNodeFlags_SpanTextWidth = 1 << 13,        // 狭窄的点击框 + 狭窄的高亮显示，仅覆盖标签文本。
    ImGuiTreeNodeFlags_SpanAllColumns = 1 << 14,       // 框架将跨越其容器表的所有列（文本仍将适应当前列）
    ImGuiTreeNodeFlags_NavLeftJumpsBackHere = 1 << 15, // （开发中）导航：向左方向可以从子节点跳回此 TreeNode()（在 TreeNode 和 TreePop 之间提交的项目）
    // ImGuiTreeNodeFlags_NoScrollOnOpen     = 1 << 16,      // FIXME: TODO: 禁用 TreePop() 打开时的自动滚动，如果节点刚刚打开且内容不可见
    ImGuiTreeNodeFlags_CollapsingHeader = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog,

// 旧版名称
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    ImGuiTreeNodeFlags_AllowItemOverlap = ImGuiTreeNodeFlags_AllowOverlap, // 在 1.89.7 中重命名
#endif
};

// 用于 OpenPopup*(), BeginPopupContext*(), IsPopupOpen() 函数的标志
// - 为了与旧 API 向后兼容，旧 API 使用了 'int mouse_button = 1' 参数代替了 'ImGuiPopupFlags flags'，
//   我们需要将小的标志值视为鼠标按钮索引，因此在标志的前几位中编码鼠标按钮。
//   因此可以确保在 ImGuiPopupFlags 中传递鼠标按钮索引是合法的。
// - 出于同样的原因，我们特意将 BeginPopupContextXXX 函数的 ImGuiPopupFlags 参数的默认值设置为 1，而不是 0。
//   重要：由于默认参数为 1（==ImGuiPopupFlags_MouseButtonRight），如果依赖默认参数并希望使用其他标志，
//   需要显式传递 ImGuiPopupFlags_MouseButtonRight 标志。
// - 当前不能在这些函数中组合或按位操作多个按钮（以后可能允许）。
enum ImGuiPopupFlags_
{
    ImGuiPopupFlags_None = 0,                // 默认标志
    ImGuiPopupFlags_MouseButtonLeft = 0,     // 对于 BeginPopupContext*(): 在左键释放时打开。保证始终等于 0（与 ImGuiMouseButton_Left 相同）
    ImGuiPopupFlags_MouseButtonRight = 1,    // 对于 BeginPopupContext*(): 在右键释放时打开。保证始终等于 1（与 ImGuiMouseButton_Right 相同）
    ImGuiPopupFlags_MouseButtonMiddle = 2,   // 对于 BeginPopupContext*(): 在中键释放时打开。保证始终等于 2（与 ImGuiMouseButton_Middle 相同）
    ImGuiPopupFlags_MouseButtonMask_ = 0x1F, // 鼠标按钮掩码（允许位掩码）
    ImGuiPopupFlags_MouseButtonDefault_ = 1, // 默认鼠标按钮（右键）
    ImGuiPopupFlags_NoReopen = 1 << 5,       // 对于 OpenPopup*(), BeginPopupContext*(): 如果已经打开相同的弹出窗口，则不重新打开（不会重新定位，也不会重新初始化导航）
    // ImGuiPopupFlags_NoReopenAlwaysNavInit = 1 << 6,   // 对于 OpenPopup*(), BeginPopupContext*(): 即使不重新打开时，也要聚焦并初始化导航。
    ImGuiPopupFlags_NoOpenOverExistingPopup = 1 << 7,                                      // 对于 OpenPopup*(), BeginPopupContext*(): 如果当前已经有一个弹出窗口在同一层级，则不打开新弹出窗口
    ImGuiPopupFlags_NoOpenOverItems = 1 << 8,                                              // 对于 BeginPopupContextWindow(): 仅当鼠标悬停在空白区域时才返回 true，悬停在其他项目上时不返回。
    ImGuiPopupFlags_AnyPopupId = 1 << 10,                                                  // 对于 IsPopupOpen(): 忽略 ImGuiID 参数，测试是否有任何弹出窗口。
    ImGuiPopupFlags_AnyPopupLevel = 1 << 11,                                               // 对于 IsPopupOpen(): 在弹出窗口堆栈的任何层级进行搜索/测试（默认只测试当前层级）
    ImGuiPopupFlags_AnyPopup = ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel, // 任何弹出窗口（即忽略 ID 和层级）
};

// 用于 ImGui::Selectable() 的标志
enum ImGuiSelectableFlags_
{
    ImGuiSelectableFlags_None = 0,                   // 默认标志
    ImGuiSelectableFlags_NoAutoClosePopups = 1 << 0, // 点击此项时不关闭父级弹出窗口（覆盖 ImGuiItemFlags_AutoClosePopups 标志）
    ImGuiSelectableFlags_SpanAllColumns = 1 << 1,    // 帧将跨越其容器表格的所有列（文本仍然会适应当前列）
    ImGuiSelectableFlags_AllowDoubleClick = 1 << 2,  // 双击时也生成按下事件
    ImGuiSelectableFlags_Disabled = 1 << 3,          // 该项不能被选中，显示为灰色文本
    ImGuiSelectableFlags_AllowOverlap = 1 << 4,      // （WIP）允许此项与后续控件重叠的点击测试
    ImGuiSelectableFlags_Highlight = 1 << 5,         // 使该项显示为被悬停状态

// 以下为重命名标志，使用时请注意
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    ImGuiSelectableFlags_DontClosePopups = ImGuiSelectableFlags_NoAutoClosePopups, // 在 1.91.0 中重命名
    ImGuiSelectableFlags_AllowItemOverlap = ImGuiSelectableFlags_AllowOverlap,     // 在 1.89.7 中重命名
#endif
};

// 用于 ImGui::BeginCombo() 的标志
enum ImGuiComboFlags_
{
    ImGuiComboFlags_None = 0,                                                                                                                                // 默认标志
    ImGuiComboFlags_PopupAlignLeft = 1 << 0,                                                                                                                 // 默认情况下将弹出窗口对齐到左侧
    ImGuiComboFlags_HeightSmall = 1 << 1,                                                                                                                    // 最大显示约 4 个项。提示：如果你想要弹出窗口有特定大小，可以在调用 BeginCombo() 前使用 SetNextWindowSizeConstraints() 设置约束
    ImGuiComboFlags_HeightRegular = 1 << 2,                                                                                                                  // 最大显示约 8 个项（默认）
    ImGuiComboFlags_HeightLarge = 1 << 3,                                                                                                                    // 最大显示约 20 个项
    ImGuiComboFlags_HeightLargest = 1 << 4,                                                                                                                  // 显示尽可能多的项
    ImGuiComboFlags_NoArrowButton = 1 << 5,                                                                                                                  // 在预览框中不显示方形箭头按钮
    ImGuiComboFlags_NoPreview = 1 << 6,                                                                                                                      // 仅显示方形箭头按钮，没有预览框
    ImGuiComboFlags_WidthFitPreview = 1 << 7,                                                                                                                // 根据预览内容动态计算宽度
    ImGuiComboFlags_HeightMask_ = ImGuiComboFlags_HeightSmall | ImGuiComboFlags_HeightRegular | ImGuiComboFlags_HeightLarge | ImGuiComboFlags_HeightLargest, // 用于高度的掩码
};

// 用于 ImGui::BeginTabBar() 的标志
enum ImGuiTabBarFlags_
{
    ImGuiTabBarFlags_None = 0,                                                                                             // 默认标志
    ImGuiTabBarFlags_Reorderable = 1 << 0,                                                                                 // 允许手动拖动标签以重新排序 + 新标签追加到列表末尾
    ImGuiTabBarFlags_AutoSelectNewTabs = 1 << 1,                                                                           // 新标签出现时自动选择
    ImGuiTabBarFlags_TabListPopupButton = 1 << 2,                                                                          // 禁用打开标签列表弹出窗口的按钮
    ImGuiTabBarFlags_NoCloseWithMiddleMouseButton = 1 << 3,                                                                // 禁用使用中键关闭标签的行为（当 p_open != NULL 时）。你可以在用户端手动处理此行为，如 if (IsItemHovered() && IsMouseClicked(2)) *p_open = false。
    ImGuiTabBarFlags_NoTabListScrollingButtons = 1 << 4,                                                                   // 禁用滚动按钮（当适配策略为 ImGuiTabBarFlags_FittingPolicyScroll 时适用）
    ImGuiTabBarFlags_NoTooltip = 1 << 5,                                                                                   // 禁用在悬停标签时显示工具提示
    ImGuiTabBarFlags_DrawSelectedOverline = 1 << 6,                                                                        // 在选中的标签上绘制选中上划线标记
    ImGuiTabBarFlags_FittingPolicyResizeDown = 1 << 7,                                                                     // 当标签不适合时，调整标签大小
    ImGuiTabBarFlags_FittingPolicyScroll = 1 << 8,                                                                         // 当标签不适合时，添加滚动按钮
    ImGuiTabBarFlags_FittingPolicyMask_ = ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_FittingPolicyScroll, // 高度适配策略的掩码
    ImGuiTabBarFlags_FittingPolicyDefault_ = ImGuiTabBarFlags_FittingPolicyResizeDown,                                     // 默认的适配策略
};

// 用于 ImGui::BeginTabItem() 的标志
enum ImGuiTabItemFlags_
{
    ImGuiTabItemFlags_None = 0,                              // 默认标志
    ImGuiTabItemFlags_UnsavedDocument = 1 << 0,              // 在标题旁显示一个点 + 设置 ImGuiTabItemFlags_NoAssumedClosure
    ImGuiTabItemFlags_SetSelected = 1 << 1,                  // 设置该标签为选中状态，当调用 BeginTabItem() 时程序化地使标签被选中
    ImGuiTabItemFlags_NoCloseWithMiddleMouseButton = 1 << 2, // 禁用使用中键关闭标签的行为（当 p_open != NULL 时）。你可以在用户端手动处理此行为，如 if (IsItemHovered() && IsMouseClicked(2)) *p_open = false。
    ImGuiTabItemFlags_NoPushId = 1 << 3,                     // 在 BeginTabItem()/EndTabItem() 中不调用 PushID()/PopID()
    ImGuiTabItemFlags_NoTooltip = 1 << 4,                    // 禁用该标签的工具提示
    ImGuiTabItemFlags_NoReorder = 1 << 5,                    // 禁用重新排序此标签或让其他标签覆盖此标签
    ImGuiTabItemFlags_Leading = 1 << 6,                      // 强制将标签位置放置在标签栏的左侧（标签列表弹出按钮之后）
    ImGuiTabItemFlags_Trailing = 1 << 7,                     // 强制将标签位置放置在标签栏的右侧（滚动按钮之前）
    ImGuiTabItemFlags_NoAssumedClosure = 1 << 8,             // 尝试关闭时该标签会被选中 + 不立即假定关闭（会等待用户停止提交标签）。否则在按下 X 时假定关闭，因此如果继续提交标签，标签可能会重新出现在标签栏的末尾。
};

// 用于 ImGui::IsWindowFocused() 的标志
enum ImGuiFocusedFlags_
{
    ImGuiFocusedFlags_None = 0,                                                                            // 默认标志
    ImGuiFocusedFlags_ChildWindows = 1 << 0,                                                               // 如果任何子窗口被聚焦，则返回 true
    ImGuiFocusedFlags_RootWindow = 1 << 1,                                                                 // 从根窗口测试（当前层级结构的最顶层父窗口）
    ImGuiFocusedFlags_AnyWindow = 1 << 2,                                                                  // 如果任何窗口被聚焦，则返回 true。重要提示：如果你正在尝试了解如何分发低级输入，请不要使用此标志。请改用 'io.WantCaptureMouse'！请阅读常见问题解答！
    ImGuiFocusedFlags_NoPopupHierarchy = 1 << 3,                                                           // 不考虑弹出窗口层级（不将弹出窗口发出者视为弹出窗口的父窗口）（与 _ChildWindows 或 _RootWindow 一起使用时）
    ImGuiFocusedFlags_DockHierarchy = 1 << 4,                                                              // 考虑停靠层级（将停靠空间宿主视为停靠窗口的父窗口）（与 _ChildWindows 或 _RootWindow 一起使用时）
    ImGuiFocusedFlags_RootAndChildWindows = ImGuiFocusedFlags_RootWindow | ImGuiFocusedFlags_ChildWindows, // 组合标志：根窗口和子窗口都被聚焦时返回 true
};

// 用于 ImGui::IsItemHovered() 和 ImGui::IsWindowHovered() 的标志
enum ImGuiHoveredFlags_
{
    ImGuiHoveredFlags_None = 0,                                                                                                                                      // 仅在鼠标直接位于项/窗口上方时返回 true，不被其他窗口遮挡，不被活动弹出窗口或模态窗口遮挡
    ImGuiHoveredFlags_ChildWindows = 1 << 0,                                                                                                                         // 仅对 IsWindowHovered() 生效：如果窗口的任何子窗口被悬停，返回 true
    ImGuiHoveredFlags_RootWindow = 1 << 1,                                                                                                                           // 仅对 IsWindowHovered() 生效：从根窗口（当前层级结构的最顶层父窗口）测试
    ImGuiHoveredFlags_AnyWindow = 1 << 2,                                                                                                                            // 仅对 IsWindowHovered() 生效：如果任何窗口被悬停，返回 true
    ImGuiHoveredFlags_NoPopupHierarchy = 1 << 3,                                                                                                                     // 仅对 IsWindowHovered() 生效：不考虑弹出窗口层级（不将弹出窗口发出者视为弹出窗口的父窗口）
    ImGuiHoveredFlags_DockHierarchy = 1 << 4,                                                                                                                        // 仅对 IsWindowHovered() 生效：考虑停靠窗口层级（将停靠空间宿主视为停靠窗口的父窗口）
    ImGuiHoveredFlags_AllowWhenBlockedByPopup = 1 << 5,                                                                                                              // 即使被弹出窗口阻挡，仍返回 true
    ImGuiHoveredFlags_AllowWhenBlockedByModal = 1 << 6,                                                                                                              // 即使被模态弹出窗口阻挡，仍返回 true（尚未实现）
    ImGuiHoveredFlags_AllowWhenBlockedByActiveItem = 1 << 7,                                                                                                         // 即使被活动项阻挡，仍返回 true。对拖放模式很有用
    ImGuiHoveredFlags_AllowWhenOverlappedByItem = 1 << 8,                                                                                                            // 仅对 IsItemHovered() 生效：即使项目使用允许重叠模式且被其他可悬停项目重叠，仍返回 true
    ImGuiHoveredFlags_AllowWhenOverlappedByWindow = 1 << 9,                                                                                                          // 仅对 IsItemHovered() 生效：即使被另一个窗口遮挡或重叠，仍返回 true
    ImGuiHoveredFlags_AllowWhenDisabled = 1 << 10,                                                                                                                   // 仅对 IsItemHovered() 生效：即使项目被禁用，仍返回 true
    ImGuiHoveredFlags_NoNavOverride = 1 << 11,                                                                                                                       // 仅对 IsItemHovered() 生效：禁用使用键盘/游戏手柄导航状态，始终查询鼠标
    ImGuiHoveredFlags_AllowWhenOverlapped = ImGuiHoveredFlags_AllowWhenOverlappedByItem | ImGuiHoveredFlags_AllowWhenOverlappedByWindow,                             // 允许重叠：允许与其他项或窗口重叠
    ImGuiHoveredFlags_RectOnly = ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem | ImGuiHoveredFlags_AllowWhenOverlapped, // 仅用于矩形区域的悬停检查
    ImGuiHoveredFlags_RootAndChildWindows = ImGuiHoveredFlags_RootWindow | ImGuiHoveredFlags_ChildWindows,                                                           // 根窗口和子窗口都被悬停时返回 true

    // 工具提示模式
    // - 通常在 IsItemHovered() + SetTooltip() 序列中使用。
    ImGuiHoveredFlags_ForTooltip = 1 << 12, // 使用 IsItemHovered() + SetTooltip() 序列时的快捷方式

    // （高级）鼠标悬停延迟
    // - 通常可以使用 ImGuiHoveredFlags_ForTooltip 来使用应用程序标准化的标志。
    // - 如果需要特定的覆盖，可以使用这些标志。
    ImGuiHoveredFlags_Stationary = 1 << 13,    // 要求鼠标静止至少一次，延迟为 style.HoverStationaryDelay (~0.15 秒)。之后可以在同一项目/窗口上移动。
    ImGuiHoveredFlags_DelayNone = 1 << 14,     // 仅对 IsItemHovered() 生效：立即返回 true（默认值）。通常忽略此标志。
    ImGuiHoveredFlags_DelayShort = 1 << 15,    // 仅对 IsItemHovered() 生效：在 style.HoverDelayShort 延迟后返回 true (~0.15 秒)
    ImGuiHoveredFlags_DelayNormal = 1 << 16,   // 仅对 IsItemHovered() 生效：在 style.HoverDelayNormal 延迟后返回 true (~0.40 秒)
    ImGuiHoveredFlags_NoSharedDelay = 1 << 17, // 仅对 IsItemHovered() 生效：禁用共享延迟系统，避免在项目间移动时延迟保持短时间
};

// 用于 ImGui::DockSpace() 和子节点共享/继承的标志
enum ImGuiDockNodeFlags_
{
    ImGuiDockNodeFlags_None = 0,               // 默认值：没有任何标志
    ImGuiDockNodeFlags_KeepAliveOnly = 1 << 0, // 仅保持 DockSpace 节点的生命，不显示该节点，已停靠的窗口不会被解除停靠
    // ImGuiDockNodeFlags_NoCentralNode              = 1 << 1,   // 禁用中央节点（可以保持空白的节点）
    ImGuiDockNodeFlags_NoDockingOverCentralNode = 1 << 2, // 禁止在中央节点上方停靠窗口，中央节点始终保持空白
    ImGuiDockNodeFlags_PassthruCentralNode = 1 << 3,      // 启用通过模式：1) DockSpace() 会渲染一个覆盖除了中央节点之外区域的背景（ImGuiCol_WindowBg）。当中央节点为空时，允许输入穿透并不会显示 DockingEmptyBg 背景。可以通过示例查看细节。2) 中央节点为空时：允许输入穿透
    ImGuiDockNodeFlags_NoDockingSplit = 1 << 4,           // 禁止其他窗口/节点分割此节点
    ImGuiDockNodeFlags_NoResize = 1 << 5,                 // 禁用使用分隔符调整节点大小。适用于程序化设置的停靠空间
    ImGuiDockNodeFlags_AutoHideTabBar = 1 << 6,           // 当停靠节点中只有一个窗口时，自动隐藏标签栏
    ImGuiDockNodeFlags_NoUndocking = 1 << 7,              // 禁止解除停靠此节点

#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    ImGuiDockNodeFlags_NoSplit = ImGuiDockNodeFlags_NoDockingSplit,                          // 在 1.90 版本中重命名
    ImGuiDockNodeFlags_NoDockingInCentralNode = ImGuiDockNodeFlags_NoDockingOverCentralNode, // 在 1.90 版本中重命名
#endif
};

// 用于 ImGui::BeginDragDropSource() 和 ImGui::AcceptDragDropPayload() 的标志
enum ImGuiDragDropFlags_
{
    ImGuiDragDropFlags_None = 0, // 默认值：没有任何标志
    // BeginDragDropSource() 标志
    ImGuiDragDropFlags_SourceNoPreviewTooltip = 1 << 0,   // 禁用预览工具提示。默认情况下，成功调用 BeginDragDropSource() 会打开一个工具提示，用于显示源内容的预览或描述。此标志禁用此行为。
    ImGuiDragDropFlags_SourceNoDisableHover = 1 << 1,     // 默认情况下，拖动时会清除数据，以便 IsItemHovered() 返回 false，避免后续代码提交工具提示。此标志禁用此行为，您仍然可以在源项上调用 IsItemHovered()。
    ImGuiDragDropFlags_SourceNoHoldToOpenOthers = 1 << 2, // 禁用通过在拖动源项上悬停来打开树节点和折叠标题的行为。
    ImGuiDragDropFlags_SourceAllowNullID = 1 << 3,        // 允许没有唯一标识符的项（如 Text()、Image()）作为拖动源，通过根据它们的窗口相对位置生成一个临时标识符。这在 Dear ImGui 中极为罕见，因此我们特别指出。
    ImGuiDragDropFlags_SourceExtern = 1 << 4,             // 外部源（来自 Dear ImGui 外部），不会尝试读取当前项/窗口信息。始终返回 true。一次只能有一个 Extern 源处于活动状态。
    ImGuiDragDropFlags_PayloadAutoExpire = 1 << 5,        // 如果源停止提交，自动过期有效负载（否则有效负载在拖动时会持续存在）。
    ImGuiDragDropFlags_PayloadNoCrossContext = 1 << 6,    // 提示指定有效负载可能无法复制到当前 Dear ImGui 上下文之外。
    ImGuiDragDropFlags_PayloadNoCrossProcess = 1 << 7,    // 提示指定有效负载可能无法复制到当前进程之外。
    // AcceptDragDropPayload() 标志
    ImGuiDragDropFlags_AcceptBeforeDelivery = 1 << 10,                                                                        // AcceptDragDropPayload() 即使在鼠标按钮未释放之前也会返回 true。然后，您可以调用 IsDelivery() 来测试有效负载是否需要交付。
    ImGuiDragDropFlags_AcceptNoDrawDefaultRect = 1 << 11,                                                                     // 悬停目标时不绘制默认的高亮矩形。
    ImGuiDragDropFlags_AcceptNoPreviewTooltip = 1 << 12,                                                                      // 请求隐藏 BeginDragDropSource 工具提示，不在 BeginDragDropTarget 站点显示。
    ImGuiDragDropFlags_AcceptPeekOnly = ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoDrawDefaultRect, // 仅用于提前查看并检查有效负载，而无需交付。

#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    ImGuiDragDropFlags_SourceAutoExpirePayload = ImGuiDragDropFlags_PayloadAutoExpire, // 在 1.90.9 版本中重命名
#endif
};

// 标准拖放有效负载类型。您可以使用简短的字符串定义自己的有效负载类型。以'_'开头的类型由 Dear ImGui 定义。
#define IMGUI_PAYLOAD_TYPE_COLOR_3F "_COL3F" // float[3]: 标准颜色类型，没有 alpha。用户代码可以使用此类型。
#define IMGUI_PAYLOAD_TYPE_COLOR_4F "_COL4F" // float[4]: 标准颜色类型。用户代码可以使用此类型。

// 主数据类型
enum ImGuiDataType_
{
    ImGuiDataType_S8,     // signed char / char（对于某些编译器）
    ImGuiDataType_U8,     // unsigned char
    ImGuiDataType_S16,    // short
    ImGuiDataType_U16,    // unsigned short
    ImGuiDataType_S32,    // int
    ImGuiDataType_U32,    // unsigned int
    ImGuiDataType_S64,    // long long / __int64
    ImGuiDataType_U64,    // unsigned long long / unsigned __int64
    ImGuiDataType_Float,  // float
    ImGuiDataType_Double, // double
    ImGuiDataType_Bool,   // bool（提供给用户方便，但标量控件不支持）
    ImGuiDataType_String, // char*（提供给用户方便，但标量控件不支持）
    ImGuiDataType_COUNT   // 数据类型的总数
};

// 基本方向
enum ImGuiDir : int
{
    ImGuiDir_None = -1, // 没有方向
    ImGuiDir_Left = 0,  // 左
    ImGuiDir_Right = 1, // 右
    ImGuiDir_Up = 2,    // 上
    ImGuiDir_Down = 3,  // 下
    ImGuiDir_COUNT      // 方向总数
};

// 排序方向
enum ImGuiSortDirection : ImU8
{
    ImGuiSortDirection_None = 0,      // 无排序
    ImGuiSortDirection_Ascending = 1, // 升序 = 0->9，A->Z 等
    ImGuiSortDirection_Descending = 2 // 降序 = 9->0，Z->A 等
};

// 键盘、鼠标和游戏手柄的按键标识符（ImGuiKey_XXX 或 ImGuiMod_XXX 值）
// 所有命名按键的值 >= 512。0 到 511 的按键值未使用，属于早期的本地/不透明按键值（< 1.87）。
// 1.91.5 完全移除了对旧版按键的支持。
// 了解 1.87+ 过渡的详细信息：https://github.com/ocornut/imgui/issues/4921
// 注意，“按键”是指物理按键，而不是输入的“字符”，后者是通过 io.AddInputCharacter() 提交的。
// 键盘按键枚举值是根据标准美国键盘的按键命名的，在其他键盘类型上，报告的按键可能与按键帽不匹配。
enum ImGuiKey : int
{
    // Keyboard
    ImGuiKey_None = 0,
    ImGuiKey_NamedKey_BEGIN = 512,  // First valid key value (other than 0)

    ImGuiKey_Tab = 512,             // == ImGuiKey_NamedKey_BEGIN
    ImGuiKey_LeftArrow,
    ImGuiKey_RightArrow,
    ImGuiKey_UpArrow,
    ImGuiKey_DownArrow,
    ImGuiKey_PageUp,
    ImGuiKey_PageDown,
    ImGuiKey_Home,
    ImGuiKey_End,
    ImGuiKey_Insert,
    ImGuiKey_Delete,
    ImGuiKey_Backspace,
    ImGuiKey_Space,
    ImGuiKey_Enter,
    ImGuiKey_Escape,
    ImGuiKey_LeftCtrl, ImGuiKey_LeftShift, ImGuiKey_LeftAlt, ImGuiKey_LeftSuper,
    ImGuiKey_RightCtrl, ImGuiKey_RightShift, ImGuiKey_RightAlt, ImGuiKey_RightSuper,
    ImGuiKey_Menu,
    ImGuiKey_0, ImGuiKey_1, ImGuiKey_2, ImGuiKey_3, ImGuiKey_4, ImGuiKey_5, ImGuiKey_6, ImGuiKey_7, ImGuiKey_8, ImGuiKey_9,
    ImGuiKey_A, ImGuiKey_B, ImGuiKey_C, ImGuiKey_D, ImGuiKey_E, ImGuiKey_F, ImGuiKey_G, ImGuiKey_H, ImGuiKey_I, ImGuiKey_J,
    ImGuiKey_K, ImGuiKey_L, ImGuiKey_M, ImGuiKey_N, ImGuiKey_O, ImGuiKey_P, ImGuiKey_Q, ImGuiKey_R, ImGuiKey_S, ImGuiKey_T,
    ImGuiKey_U, ImGuiKey_V, ImGuiKey_W, ImGuiKey_X, ImGuiKey_Y, ImGuiKey_Z,
    ImGuiKey_F1, ImGuiKey_F2, ImGuiKey_F3, ImGuiKey_F4, ImGuiKey_F5, ImGuiKey_F6,
    ImGuiKey_F7, ImGuiKey_F8, ImGuiKey_F9, ImGuiKey_F10, ImGuiKey_F11, ImGuiKey_F12,
    ImGuiKey_F13, ImGuiKey_F14, ImGuiKey_F15, ImGuiKey_F16, ImGuiKey_F17, ImGuiKey_F18,
    ImGuiKey_F19, ImGuiKey_F20, ImGuiKey_F21, ImGuiKey_F22, ImGuiKey_F23, ImGuiKey_F24,
    ImGuiKey_Apostrophe,        // '
    ImGuiKey_Comma,             // ,
    ImGuiKey_Minus,             // -
    ImGuiKey_Period,            // .
    ImGuiKey_Slash,             // /
    ImGuiKey_Semicolon,         // ;
    ImGuiKey_Equal,             // =
    ImGuiKey_LeftBracket,       // [
    ImGuiKey_Backslash,         // \ (this text inhibit multiline comment caused by backslash)
    ImGuiKey_RightBracket,      // ]
    ImGuiKey_GraveAccent,       // `
    ImGuiKey_CapsLock,
    ImGuiKey_ScrollLock,
    ImGuiKey_NumLock,
    ImGuiKey_PrintScreen,
    ImGuiKey_Pause,
    ImGuiKey_Keypad0, ImGuiKey_Keypad1, ImGuiKey_Keypad2, ImGuiKey_Keypad3, ImGuiKey_Keypad4,
    ImGuiKey_Keypad5, ImGuiKey_Keypad6, ImGuiKey_Keypad7, ImGuiKey_Keypad8, ImGuiKey_Keypad9,
    ImGuiKey_KeypadDecimal,
    ImGuiKey_KeypadDivide,
    ImGuiKey_KeypadMultiply,
    ImGuiKey_KeypadSubtract,
    ImGuiKey_KeypadAdd,
    ImGuiKey_KeypadEnter,
    ImGuiKey_KeypadEqual,
    ImGuiKey_AppBack,               // Available on some keyboard/mouses. Often referred as "Browser Back"
    ImGuiKey_AppForward,

    // Gamepad (some of those are analog values, 0.0f to 1.0f)                          // NAVIGATION ACTION
    // (download controller mapping PNG/PSD at http://dearimgui.com/controls_sheets)
    ImGuiKey_GamepadStart,          // Menu (Xbox)      + (Switch)   Start/Options (PS)
    ImGuiKey_GamepadBack,           // View (Xbox)      - (Switch)   Share (PS)
    ImGuiKey_GamepadFaceLeft,       // X (Xbox)         Y (Switch)   Square (PS)        // Tap: Toggle Menu. Hold: Windowing mode (Focus/Move/Resize windows)
    ImGuiKey_GamepadFaceRight,      // B (Xbox)         A (Switch)   Circle (PS)        // Cancel / Close / Exit
    ImGuiKey_GamepadFaceUp,         // Y (Xbox)         X (Switch)   Triangle (PS)      // Text Input / On-screen Keyboard
    ImGuiKey_GamepadFaceDown,       // A (Xbox)         B (Switch)   Cross (PS)         // Activate / Open / Toggle / Tweak
    ImGuiKey_GamepadDpadLeft,       // D-pad Left                                       // Move / Tweak / Resize Window (in Windowing mode)
    ImGuiKey_GamepadDpadRight,      // D-pad Right                                      // Move / Tweak / Resize Window (in Windowing mode)
    ImGuiKey_GamepadDpadUp,         // D-pad Up                                         // Move / Tweak / Resize Window (in Windowing mode)
    ImGuiKey_GamepadDpadDown,       // D-pad Down                                       // Move / Tweak / Resize Window (in Windowing mode)
    ImGuiKey_GamepadL1,             // L Bumper (Xbox)  L (Switch)   L1 (PS)            // Tweak Slower / Focus Previous (in Windowing mode)
    ImGuiKey_GamepadR1,             // R Bumper (Xbox)  R (Switch)   R1 (PS)            // Tweak Faster / Focus Next (in Windowing mode)
    ImGuiKey_GamepadL2,             // L Trig. (Xbox)   ZL (Switch)  L2 (PS) [Analog]
    ImGuiKey_GamepadR2,             // R Trig. (Xbox)   ZR (Switch)  R2 (PS) [Analog]
    ImGuiKey_GamepadL3,             // L Stick (Xbox)   L3 (Switch)  L3 (PS)
    ImGuiKey_GamepadR3,             // R Stick (Xbox)   R3 (Switch)  R3 (PS)
    ImGuiKey_GamepadLStickLeft,     // [Analog]                                         // Move Window (in Windowing mode)
    ImGuiKey_GamepadLStickRight,    // [Analog]                                         // Move Window (in Windowing mode)
    ImGuiKey_GamepadLStickUp,       // [Analog]                                         // Move Window (in Windowing mode)
    ImGuiKey_GamepadLStickDown,     // [Analog]                                         // Move Window (in Windowing mode)
    ImGuiKey_GamepadRStickLeft,     // [Analog]
    ImGuiKey_GamepadRStickRight,    // [Analog]
    ImGuiKey_GamepadRStickUp,       // [Analog]
    ImGuiKey_GamepadRStickDown,     // [Analog]

    // Aliases: Mouse Buttons (auto-submitted from AddMouseButtonEvent() calls)
    // - This is mirroring the data also written to io.MouseDown[], io.MouseWheel, in a format allowing them to be accessed via standard key API.
    ImGuiKey_MouseLeft, ImGuiKey_MouseRight, ImGuiKey_MouseMiddle, ImGuiKey_MouseX1, ImGuiKey_MouseX2, ImGuiKey_MouseWheelX, ImGuiKey_MouseWheelY,

    // [Internal] Reserved for mod storage
    ImGuiKey_ReservedForModCtrl, ImGuiKey_ReservedForModShift, ImGuiKey_ReservedForModAlt, ImGuiKey_ReservedForModSuper,
    ImGuiKey_NamedKey_END,

    // Keyboard Modifiers (explicitly submitted by backend via AddKeyEvent() calls)
    // - This is mirroring the data also written to io.KeyCtrl, io.KeyShift, io.KeyAlt, io.KeySuper, in a format allowing
    //   them to be accessed via standard key API, allowing calls such as IsKeyPressed(), IsKeyReleased(), querying duration etc.
    // - Code polling every key (e.g. an interface to detect a key press for input mapping) might want to ignore those
    //   and prefer using the real keys (e.g. ImGuiKey_LeftCtrl, ImGuiKey_RightCtrl instead of ImGuiMod_Ctrl).
    // - In theory the value of keyboard modifiers should be roughly equivalent to a logical or of the equivalent left/right keys.
    //   In practice: it's complicated; mods are often provided from different sources. Keyboard layout, IME, sticky keys and
    //   backends tend to interfere and break that equivalence. The safer decision is to relay that ambiguity down to the end-user...
    // - On macOS, we swap Cmd(Super) and Ctrl keys at the time of the io.AddKeyEvent() call.
    ImGuiMod_None                   = 0,
    ImGuiMod_Ctrl                   = 1 << 12, // Ctrl (non-macOS), Cmd (macOS)
    ImGuiMod_Shift                  = 1 << 13, // Shift
    ImGuiMod_Alt                    = 1 << 14, // Option/Menu
    ImGuiMod_Super                  = 1 << 15, // Windows/Super (non-macOS), Ctrl (macOS)
    ImGuiMod_Mask_                  = 0xF000,  // 4-bits

    // [Internal] If you need to iterate all keys (for e.g. an input mapper) you may use ImGuiKey_NamedKey_BEGIN..ImGuiKey_NamedKey_END.
    ImGuiKey_NamedKey_COUNT         = ImGuiKey_NamedKey_END - ImGuiKey_NamedKey_BEGIN,
    //ImGuiKey_KeysData_SIZE        = ImGuiKey_NamedKey_COUNT,  // Size of KeysData[]: only hold named keys
    //ImGuiKey_KeysData_OFFSET      = ImGuiKey_NamedKey_BEGIN,  // Accesses to io.KeysData[] must use (key - ImGuiKey_NamedKey_BEGIN) index.

#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    ImGuiKey_COUNT                  = ImGuiKey_NamedKey_END,    // Obsoleted in 1.91.5 because it was extremely misleading (since named keys don't start at 0 anymore)
    ImGuiMod_Shortcut               = ImGuiMod_Ctrl,            // Removed in 1.90.7, you can now simply use ImGuiMod_Ctrl
    ImGuiKey_ModCtrl = ImGuiMod_Ctrl, ImGuiKey_ModShift = ImGuiMod_Shift, ImGuiKey_ModAlt = ImGuiMod_Alt, ImGuiKey_ModSuper = ImGuiMod_Super, // Renamed in 1.89
    //ImGuiKey_KeyPadEnter = ImGuiKey_KeypadEnter,              // Renamed in 1.87
#endif
};

// Shortcut()、SetNextItemShortcut() 的标志，
// （以及即将发布的 IsKeyPressed()、IsMouseClicked()、Shortcut()、SetKeyOwner()、SetItemKeyOwner() 的扩展版本，仍在 imgui_internal.h 中）
// 不要与 ImGuiInputTextFlags 混淆！(它是用于 ImGui::InputText() 函数的)
enum ImGuiInputFlags_
{
    ImGuiInputFlags_None = 0,
    ImGuiInputFlags_Repeat = 1 << 0, // 启用重复。连续重复时返回 true。对于传统的 IsKeyPressed() 为默认值。不是传统的 IsMouseClicked() 默认值。必须为 == 1。

    // Shortcut()、SetNextItemShortcut() 的标志
    // - 路由策略：RouteGlobal+OverActive >> RouteActive 或 RouteFocused（如果拥有者是活动项）>> RouteGlobal+OverFocused >> RouteFocused（如果在聚焦窗口堆栈中）>> RouteGlobal。
    // - 默认策略是 RouteFocused。所有可用的策略中只能选择一个。
    ImGuiInputFlags_RouteActive = 1 << 10,  // 仅路由到活动项。
    ImGuiInputFlags_RouteFocused = 1 << 11, // 路由到聚焦堆栈中的窗口（默认）。最深层的聚焦窗口接收输入。活动项优先于最深层聚焦窗口接收输入。
    ImGuiInputFlags_RouteGlobal = 1 << 12,  // 全局路由（除非聚焦窗口或活动项已注册路由）。
    ImGuiInputFlags_RouteAlways = 1 << 13,  // 不注册路由，直接轮询键。
    // - 路由选项
    ImGuiInputFlags_RouteOverFocused = 1 << 14,     // 选项：全局路由：优先级高于聚焦路由（除非活动项在聚焦路由中）。
    ImGuiInputFlags_RouteOverActive = 1 << 15,      // 选项：全局路由：优先级高于活动项。不太可能需要使用：会干扰每个活动项，例如由 InputText 注册的 CTRL+A 会被此覆盖。可能不会完全遵守，因为用户/内部代码可能始终假设可以访问活动时的键。
    ImGuiInputFlags_RouteUnlessBgFocused = 1 << 16, // 选项：全局路由：如果底层背景/空白被聚焦（即没有 Dear ImGui 窗口被聚焦），则不应用此路由。适用于叠加应用程序。
    ImGuiInputFlags_RouteFromRootWindow = 1 << 17,  // 选项：从根窗口的角度评估路由，而不是当前窗口。

    // SetNextItemShortcut() 的标志
    ImGuiInputFlags_Tooltip = 1 << 18, // 当鼠标悬停在项上时自动显示工具提示 [BETA] 不确定正确的 API（选择加入/选择退出）
};

// 配置标志存储在 io.ConfigFlags 中。由用户/应用程序设置。
enum ImGuiConfigFlags_
{
    ImGuiConfigFlags_None = 0,
    ImGuiConfigFlags_NavEnableKeyboard = 1 << 0,   // 主键盘导航启用标志。启用完整的 Tab 键 + 方向箭头 + 空格/回车激活。
    ImGuiConfigFlags_NavEnableGamepad = 1 << 1,    // 主游戏手柄导航启用标志。后端还需要设置 ImGuiBackendFlags_HasGamepad。
    ImGuiConfigFlags_NoMouse = 1 << 4,             // 指示 Dear ImGui 禁用鼠标输入和交互。
    ImGuiConfigFlags_NoMouseCursorChange = 1 << 5, // 指示后端不要更改鼠标光标形状和可见性。如果后端的光标更改与你的光标干扰，并且你不想使用 SetMouseCursor() 来更改鼠标光标，可以使用此选项。你可能希望通过读取 GetMouseCursor() 来遵守 imgui 的请求。
    ImGuiConfigFlags_NoKeyboard = 1 << 6,          // 指示 Dear ImGui 禁用键盘输入和交互。这是通过忽略键盘事件并清除现有状态来完成的。

    // [BETA] 停靠
    ImGuiConfigFlags_DockingEnable = 1 << 7, // 启用停靠标志。

    // [BETA] 视口
    // 使用视口时，建议将 ImGuiCol_WindowBg 的默认值设置为不透明（Alpha=1.0），这样在转换到视口时不会引起注意。
    ImGuiConfigFlags_ViewportsEnable = 1 << 10,         // 视口启用标志（需要后端分别设置 ImGuiBackendFlags_PlatformHasViewports 和 ImGuiBackendFlags_RendererHasViewports）。
    ImGuiConfigFlags_DpiEnableScaleViewports = 1 << 14, // [BETA: 不要使用] FIXME-DPI: 当视口的 DpiScale 改变时重新定位和调整 ImGui 窗口的大小（主要适用于托管其他窗口的主视口）。请注意，调整主窗口本身的大小取决于你的应用程序。
    ImGuiConfigFlags_DpiEnableScaleFonts = 1 << 15,     // [BETA: 不要使用] FIXME-DPI: 请求位图缩放字体以匹配 DpiScale。这是一个非常低质量的解决方法。处理 DPI 的正确方法是_目前_在 Platform_OnChangedViewport 回调中替换图集和/或字体，但这仍然是早期的工作进展。

    // 用户存储（允许你的后端/引擎与可能在多个项目间共享的代码进行通信。核心 Dear ImGui 不使用这些标志）
    ImGuiConfigFlags_IsSRGB = 1 << 20,        // 应用程序支持 SRGB。
    ImGuiConfigFlags_IsTouchScreen = 1 << 21, // 应用程序使用触摸屏而不是鼠标。

#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    ImGuiConfigFlags_NavEnableSetMousePos = 1 << 2, // [已移动/重命名于 1.91.4] -> 使用 bool io.ConfigNavMoveSetMousePos
    ImGuiConfigFlags_NavNoCaptureKeyboard = 1 << 3, // [已移动/重命名于 1.91.4] -> 使用 bool io.ConfigNavCaptureKeyboard
#endif
};

// 后端能力标志存储在 io.BackendFlags 中。由 imgui_impl_xxx 或自定义后端设置。
enum ImGuiBackendFlags_
{
    ImGuiBackendFlags_None = 0,
    ImGuiBackendFlags_HasGamepad = 1 << 0,           // 后端平台支持游戏手柄，并且当前有一个连接。
    ImGuiBackendFlags_HasMouseCursors = 1 << 1,      // 后端平台支持尊重 GetMouseCursor() 的值来更改操作系统光标形状。
    ImGuiBackendFlags_HasSetMousePos = 1 << 2,       // 后端平台支持 io.WantSetMousePos 请求以重新定位操作系统鼠标位置（仅在 io.ConfigNavMoveSetMousePos 设置时使用）。
    ImGuiBackendFlags_RendererHasVtxOffset = 1 << 3, // 后端渲染器支持 ImDrawCmd::VtxOffset。这允许输出大网格（64K+ 顶点），同时仍使用 16 位索引。

    // [BETA] 视口
    ImGuiBackendFlags_PlatformHasViewports = 1 << 10,    // 后端平台支持多个视口。
    ImGuiBackendFlags_HasMouseHoveredViewport = 1 << 11, // 后端平台支持调用 io.AddMouseViewportEvent() 来获取鼠标下的视口。如果可能，忽略具有 ImGuiViewportFlags_NoInputs 标志的视口（Win32 后端，GLFW 3.30+ 后端可以做到这一点，SDL 后端不能）。如果无法做到这一点，Dear ImGui 需要使用不完美的启发式方法来找到鼠标下的视口。
    ImGuiBackendFlags_RendererHasViewports = 1 << 12,    // 后端渲染器支持多个视口。
};

// PushStyleColor() / PopStyleColor() 的枚举
enum ImGuiCol_
{
    ImGuiCol_Text,                      // 文本颜色
    ImGuiCol_TextDisabled,              // 禁用文本颜色
    ImGuiCol_WindowBg,                  // 普通窗口背景色
    ImGuiCol_ChildBg,                   // 子窗口背景色
    ImGuiCol_PopupBg,                   // 弹出框、菜单、工具提示窗口的背景色
    ImGuiCol_Border,                    // 边框颜色
    ImGuiCol_BorderShadow,              // 边框阴影颜色
    ImGuiCol_FrameBg,                   // 复选框、单选按钮、图表、滑动条、文本输入框的背景色
    ImGuiCol_FrameBgHovered,            // 复选框、单选按钮、图表、滑动条、文本输入框被悬停时的背景色
    ImGuiCol_FrameBgActive,             // 复选框、单选按钮、图表、滑动条、文本输入框被激活时的背景色
    ImGuiCol_TitleBg,                   // 标题栏背景色
    ImGuiCol_TitleBgActive,             // 标题栏聚焦时的背景色
    ImGuiCol_TitleBgCollapsed,          // 标题栏折叠时的背景色
    ImGuiCol_MenuBarBg,                 // 菜单栏背景色
    ImGuiCol_ScrollbarBg,               // 滚动条背景色
    ImGuiCol_ScrollbarGrab,             // 滚动条拖动块
    ImGuiCol_ScrollbarGrabHovered,      // 滚动条拖动块被悬停时的颜色
    ImGuiCol_ScrollbarGrabActive,       // 滚动条拖动块被激活时的颜色
    ImGuiCol_CheckMark,                 // 复选框勾选标记和单选按钮圆圈
    ImGuiCol_SliderGrab,                // 滑动条抓取块
    ImGuiCol_SliderGrabActive,          // 滑动条抓取块被激活时的颜色
    ImGuiCol_Button,                    // 按钮颜色
    ImGuiCol_ButtonHovered,             // 按钮被悬停时的颜色
    ImGuiCol_ButtonActive,              // 按钮被激活时的颜色
    ImGuiCol_Header,                    // Header* 颜色用于 CollapsingHeader、TreeNode、Selectable、MenuItem
    ImGuiCol_HeaderHovered,             // Header 被悬停时的颜色
    ImGuiCol_HeaderActive,              // Header 被激活时的颜色
    ImGuiCol_Separator,                 // 分隔符颜色
    ImGuiCol_SeparatorHovered,          // 分隔符被悬停时的颜色
    ImGuiCol_SeparatorActive,           // 分隔符被激活时的颜色
    ImGuiCol_ResizeGrip,                // 窗口右下角和左下角的调整大小控件
    ImGuiCol_ResizeGripHovered,         // 调整大小控件被悬停时的颜色
    ImGuiCol_ResizeGripActive,          // 调整大小控件被激活时的颜色
    ImGuiCol_TabHovered,                // 标签背景色，当标签被悬停时
    ImGuiCol_Tab,                       // 标签背景色，当标签栏聚焦且标签未被选中时
    ImGuiCol_TabSelected,               // 标签背景色，当标签栏聚焦且标签被选中时
    ImGuiCol_TabSelectedOverline,       // 标签横向上划线，当标签栏聚焦且标签被选中时
    ImGuiCol_TabDimmed,                 // 标签背景色，当标签栏未聚焦且标签未被选中时
    ImGuiCol_TabDimmedSelected,         // 标签背景色，当标签栏未聚焦且标签被选中时
    ImGuiCol_TabDimmedSelectedOverline, // 标签横向上划线，当标签栏未聚焦且标签被选中时
    ImGuiCol_DockingPreview,            // 即将对某个窗口进行停靠时的预览覆盖色
    ImGuiCol_DockingEmptyBg,            // 空节点的背景色（例如没有窗口停靠在其中的中央节点）
    ImGuiCol_PlotLines,                 // 图表线条颜色
    ImGuiCol_PlotLinesHovered,          // 图表线条被悬停时的颜色
    ImGuiCol_PlotHistogram,             // 图表直方图颜色
    ImGuiCol_PlotHistogramHovered,      // 图表直方图被悬停时的颜色
    ImGuiCol_TableHeaderBg,             // 表格头部背景色
    ImGuiCol_TableBorderStrong,         // 表格外部和头部边框（这里建议使用 Alpha=1.0）
    ImGuiCol_TableBorderLight,          // 表格内部边框（这里建议使用 Alpha=1.0）
    ImGuiCol_TableRowBg,                // 表格行背景色（偶数行）
    ImGuiCol_TableRowBgAlt,             // 表格行背景色（奇数行）
    ImGuiCol_TextLink,                  // 超链接颜色
    ImGuiCol_TextSelectedBg,            // 文本选中背景色
    ImGuiCol_DragDropTarget,            // 拖放目标的矩形高亮
    ImGuiCol_NavCursor,                 // 键盘/游戏手柄导航光标/矩形的颜色，当其可见时
    ImGuiCol_NavWindowingHighlight,     // 使用 CTRL+TAB 时，突出显示的窗口
    ImGuiCol_NavWindowingDimBg,         // 当 CTRL+TAB 窗口列表活动时，整个屏幕的暗化/着色
    ImGuiCol_ModalWindowDimBg,          // 当模态窗口活动时，整个屏幕的暗化/着色
    ImGuiCol_COUNT,                     // 计数器

#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    ImGuiCol_TabActive = ImGuiCol_TabSelected,                // [在 1.90.9 中重命名]
    ImGuiCol_TabUnfocused = ImGuiCol_TabDimmed,               // [在 1.90.9 中重命名]
    ImGuiCol_TabUnfocusedActive = ImGuiCol_TabDimmedSelected, // [在 1.90.9 中重命名]
    ImGuiCol_NavHighlight = ImGuiCol_NavCursor,               // [在 1.91.4 中重命名]
#endif
};

// PushStyleVar() / PopStyleVar() 的枚举，用于临时修改 ImGuiStyle 结构
// - 该枚举仅涉及 ImGuiStyle 中适合在 UI 代码中推送/弹出的字段。
//   在初始化或每帧之间，直接修改 ImGuiStyle 即可。
// - 提示：使用编程 IDE 的导航功能，可以在第二列名称处找到实际的成员和描述。
//   - 在 Visual Studio 中：CTRL+逗号 ("Edit.GoToAll") 可以跟随注释中的符号，而 CTRL+F12 ("Edit.GoToImplementation") 则不行。
//   - 如果 Visual Studio 安装了 Visual Assist：ALT+G ("VAssistX.GoToImplementation") 也可以跟随注释中的符号。
//   - 在 VS Code、CLion 等中：CTRL+点击可以跟随注释中的符号。
// - 修改此枚举时，需要更新相关的内部表 GStyleVarInfo[]，此表用于将枚举值链接到成员偏移量/类型。

enum ImGuiStyleVar_
{
    // 枚举名称 -------------------------- // ImGuiStyle 结构中的成员（参见 ImGuiStyle 的描述）
    ImGuiStyleVar_Alpha,                       // float     Alpha
    ImGuiStyleVar_DisabledAlpha,               // float     DisabledAlpha
    ImGuiStyleVar_WindowPadding,               // ImVec2    WindowPadding
    ImGuiStyleVar_WindowRounding,              // float     WindowRounding
    ImGuiStyleVar_WindowBorderSize,            // float     WindowBorderSize
    ImGuiStyleVar_WindowMinSize,               // ImVec2    WindowMinSize
    ImGuiStyleVar_WindowTitleAlign,            // ImVec2    WindowTitleAlign
    ImGuiStyleVar_ChildRounding,               // float     ChildRounding
    ImGuiStyleVar_ChildBorderSize,             // float     ChildBorderSize
    ImGuiStyleVar_PopupRounding,               // float     PopupRounding
    ImGuiStyleVar_PopupBorderSize,             // float     PopupBorderSize
    ImGuiStyleVar_FramePadding,                // ImVec2    FramePadding
    ImGuiStyleVar_FrameRounding,               // float     FrameRounding
    ImGuiStyleVar_FrameBorderSize,             // float     FrameBorderSize
    ImGuiStyleVar_ItemSpacing,                 // ImVec2    ItemSpacing
    ImGuiStyleVar_ItemInnerSpacing,            // ImVec2    ItemInnerSpacing
    ImGuiStyleVar_IndentSpacing,               // float     IndentSpacing
    ImGuiStyleVar_CellPadding,                 // ImVec2    CellPadding
    ImGuiStyleVar_ScrollbarSize,               // float     ScrollbarSize
    ImGuiStyleVar_ScrollbarRounding,           // float     ScrollbarRounding
    ImGuiStyleVar_GrabMinSize,                 // float     GrabMinSize
    ImGuiStyleVar_GrabRounding,                // float     GrabRounding
    ImGuiStyleVar_TabRounding,                 // float     TabRounding
    ImGuiStyleVar_TabBorderSize,               // float     TabBorderSize
    ImGuiStyleVar_TabBarBorderSize,            // float     TabBarBorderSize
    ImGuiStyleVar_TabBarOverlineSize,          // float     TabBarOverlineSize
    ImGuiStyleVar_TableAngledHeadersAngle,     // float     TableAngledHeadersAngle
    ImGuiStyleVar_TableAngledHeadersTextAlign, // ImVec2  TableAngledHeadersTextAlign
    ImGuiStyleVar_ButtonTextAlign,             // ImVec2    ButtonTextAlign
    ImGuiStyleVar_SelectableTextAlign,         // ImVec2    SelectableTextAlign
    ImGuiStyleVar_SeparatorTextBorderSize,     // float     SeparatorTextBorderSize
    ImGuiStyleVar_SeparatorTextAlign,          // ImVec2    SeparatorTextAlign
    ImGuiStyleVar_SeparatorTextPadding,        // ImVec2    SeparatorTextPadding
    ImGuiStyleVar_DockingSeparatorSize,        // float     DockingSeparatorSize
    ImGuiStyleVar_COUNT                        // 计数器
};

// InvisibleButton() 的标志 [扩展在 imgui_internal.h 中]
enum ImGuiButtonFlags_
{
    ImGuiButtonFlags_None = 0,                                                                                                                     // 无标志
    ImGuiButtonFlags_MouseButtonLeft = 1 << 0,                                                                                                     // 响应左键鼠标按钮（默认）
    ImGuiButtonFlags_MouseButtonRight = 1 << 1,                                                                                                    // 响应右键鼠标按钮
    ImGuiButtonFlags_MouseButtonMiddle = 1 << 2,                                                                                                   // 响应中键鼠标按钮
    ImGuiButtonFlags_MouseButtonMask_ = ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_MouseButtonMiddle, // [内部] 鼠标按钮掩码
    ImGuiButtonFlags_EnableNav = 1 << 3,                                                                                                           // InvisibleButton()：不禁用导航/切换。否则默认禁用。
};

// ColorEdit3() / ColorEdit4() / ColorPicker3() / ColorPicker4() / ColorButton 的标志
enum ImGuiColorEditFlags_
{
    ImGuiColorEditFlags_None = 0,                // 无标志
    ImGuiColorEditFlags_NoAlpha = 1 << 1,        // ColorEdit, ColorPicker, ColorButton: 忽略 Alpha 分量（仅读取输入指针的 3 个分量）
    ImGuiColorEditFlags_NoPicker = 1 << 2,       // ColorEdit: 禁用点击颜色方块时弹出的调色板
    ImGuiColorEditFlags_NoOptions = 1 << 3,      // ColorEdit: 禁用右键单击输入框或小预览时切换选项菜单
    ImGuiColorEditFlags_NoSmallPreview = 1 << 4, // ColorEdit, ColorPicker: 禁用输入框旁的小颜色方块预览（例如仅显示输入框）
    ImGuiColorEditFlags_NoInputs = 1 << 5,       // ColorEdit, ColorPicker: 禁用输入框滑块/文本组件（例如仅显示小预览颜色方块）
    ImGuiColorEditFlags_NoTooltip = 1 << 6,      // ColorEdit, ColorPicker, ColorButton: 禁用悬停预览时的工具提示
    ImGuiColorEditFlags_NoLabel = 1 << 7,        // ColorEdit, ColorPicker: 禁用内联文本标签的显示（标签仍会转发到工具提示和调色板）
    ImGuiColorEditFlags_NoSidePreview = 1 << 8,  // ColorPicker: 禁用调色板右侧的大颜色预览，改为使用小颜色方块预览
    ImGuiColorEditFlags_NoDragDrop = 1 << 9,     // ColorEdit: 禁用拖放目标，ColorButton: 禁用拖放源
    ImGuiColorEditFlags_NoBorder = 1 << 10,      // ColorButton: 禁用边框（默认启用）

    // 用户选项（右键点击组件可更改其中一些）
    ImGuiColorEditFlags_AlphaBar = 1 << 16,         // ColorEdit, ColorPicker: 在调色板中显示垂直的 Alpha 条/渐变
    ImGuiColorEditFlags_AlphaPreview = 1 << 17,     // ColorEdit, ColorPicker, ColorButton: 显示透明色预览，使用棋盘格背景，而非不透明的颜色
    ImGuiColorEditFlags_AlphaPreviewHalf = 1 << 18, // ColorEdit, ColorPicker, ColorButton: 显示一半透明/一半棋盘格的效果，而非不透明
    ImGuiColorEditFlags_HDR = 1 << 19,              // (开发中) ColorEdit: 当前只禁用 RGBA 编辑中的 0.0f..1.0f 限制（注意：你可能还需要使用 ImGuiColorEditFlags_Float 标志）
    ImGuiColorEditFlags_DisplayRGB = 1 << 20,       // [显示] ColorEdit: 强制显示 RGB 格式，ColorPicker: 使用 RGB/HSV/Hex 组合的任意选择
    ImGuiColorEditFlags_DisplayHSV = 1 << 21,       // [显示]
    ImGuiColorEditFlags_DisplayHex = 1 << 22,       // [显示]
    ImGuiColorEditFlags_Uint8 = 1 << 23,            // [数据类型] ColorEdit, ColorPicker, ColorButton: 以 0..255 格式显示值
    ImGuiColorEditFlags_Float = 1 << 24,            // [数据类型] ColorEdit, ColorPicker, ColorButton: 以 0.0f..1.0f 浮动数值格式显示值，而非 0..255 整数。不经过整数值的回环转换。
    ImGuiColorEditFlags_PickerHueBar = 1 << 25,     // [选择器] ColorPicker: 使用色相条，饱和度/明度使用矩形框
    ImGuiColorEditFlags_PickerHueWheel = 1 << 26,   // [选择器] ColorPicker: 使用色相轮，饱和度/明度使用三角形框
    ImGuiColorEditFlags_InputRGB = 1 << 27,         // [输入] ColorEdit, ColorPicker: 输入和输出数据为 RGB 格式
    ImGuiColorEditFlags_InputHSV = 1 << 28,         // [输入] ColorEdit, ColorPicker: 输入和输出数据为 HSV 格式

    // 默认选项。您可以通过 SetColorEditOptions() 设置应用程序默认值。其目的是您可能不希望在大多数调用中覆盖它们。
    ImGuiColorEditFlags_DefaultOptions_ = ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar,

    // [内部] 掩码
    ImGuiColorEditFlags_DisplayMask_ = ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_DisplayHex,
    ImGuiColorEditFlags_DataTypeMask_ = ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_Float,
    ImGuiColorEditFlags_PickerMask_ = ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_PickerHueBar,
    ImGuiColorEditFlags_InputMask_ = ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_InputHSV,

    // 废弃的名称
    // ImGuiColorEditFlags_RGB = ImGuiColorEditFlags_DisplayRGB, ImGuiColorEditFlags_HSV = ImGuiColorEditFlags_DisplayHSV, ImGuiColorEditFlags_HEX = ImGuiColorEditFlags_DisplayHex  // [在 1.69 中重命名]
};

// 用于 DragFloat()、DragInt()、SliderFloat()、SliderInt() 等函数的标志。
// 由于 DragXXX() 和 SliderXXX() 函数具有相同的特性，所以使用相同的标志集，这样更便于互换使用。
// （这些是每个组件的标志，也有共享行为标志：ImGuiIO: io.ConfigDragClickToInputText）
enum ImGuiSliderFlags_
{
    ImGuiSliderFlags_None = 0,                                                                      // 无标志
    ImGuiSliderFlags_Logarithmic = 1 << 5,                                                          // 使控件成为对数型（默认为线性）。如果使用格式字符串并且小数位很少，考虑使用 ImGuiSliderFlags_NoRoundToFormat。
    ImGuiSliderFlags_NoRoundToFormat = 1 << 6,                                                      // 禁用将底层值四舍五入以匹配显示格式字符串的精度（例如 %.3f 将四舍五入到 3 位小数）。
    ImGuiSliderFlags_NoInput = 1 << 7,                                                              // 禁用通过 CTRL+Click 或回车键直接在控件中输入文本。
    ImGuiSliderFlags_WrapAround = 1 << 8,                                                           // 启用最大值到最小值的循环，以及最小值到最大值的循环。当前仅支持 DragXXX() 函数。
    ImGuiSliderFlags_ClampOnInput = 1 << 9,                                                         // 当手动输入（CTRL+Click）时，确保值在最小/最大范围内。默认情况下，CTRL+Click 允许超出边界。
    ImGuiSliderFlags_ClampZeroRange = 1 << 10,                                                      // 即使 min==max==0.0f，也进行限制。否则由于遗留原因，DragXXX 函数在这些值时不会进行限制。如果您的限制范围是动态的，几乎总是希望使用它。
    ImGuiSliderFlags_NoSpeedTweaks = 1 << 11,                                                       // 禁用通过键盘修饰符调整调整速度。若您希望根据自己的逻辑调整速度，可以使用此标志。
    ImGuiSliderFlags_AlwaysClamp = ImGuiSliderFlags_ClampOnInput | ImGuiSliderFlags_ClampZeroRange, // 总是强制限制
    ImGuiSliderFlags_InvalidMask_ = 0x7000000F,                                                     // [内部] 我们认为使用这些位是将前一个 API 中的 'float power' 参数错误地转换为此枚举值，如果需要会触发断言。
};

// 标识鼠标按钮。
// 这些值保证是稳定的，我们通常直接使用 0/1。提供命名枚举以便于使用。
enum ImGuiMouseButton_
{
    ImGuiMouseButton_Left = 0,   // 左键
    ImGuiMouseButton_Right = 1,  // 右键
    ImGuiMouseButton_Middle = 2, // 中键
    ImGuiMouseButton_COUNT = 5   // 鼠标按钮数量（可能包括额外的自定义按钮）
};

// 获取鼠标光标的枚举类型
// 用户代码可以通过调用 SetMouseCursor() 来请求后端显示指定的光标，这就是为什么我们在这里标记为未使用的光标
enum ImGuiMouseCursor_
{
    ImGuiMouseCursor_None = -1,  // 无光标
    ImGuiMouseCursor_Arrow = 0,  // 默认箭头光标
    ImGuiMouseCursor_TextInput,  // 当鼠标悬停在 InputText 等输入框上时
    ImGuiMouseCursor_ResizeAll,  // （Dear ImGui 函数未使用）
    ImGuiMouseCursor_ResizeNS,   // 当鼠标悬停在水平边框上时
    ImGuiMouseCursor_ResizeEW,   // 当鼠标悬停在垂直边框或列上时
    ImGuiMouseCursor_ResizeNESW, // 当鼠标悬停在窗口的左下角时
    ImGuiMouseCursor_ResizeNWSE, // 当鼠标悬停在窗口的右下角时
    ImGuiMouseCursor_Hand,       // （Dear ImGui 函数未使用。通常用于例如超链接）
    ImGuiMouseCursor_NotAllowed, // 当鼠标悬停在禁止交互的元素上时，通常是一个斜线交叉圆圈光标
    ImGuiMouseCursor_COUNT       // 光标数量
};

// 鼠标输入数据的实际来源的枚举类型
// 历史上我们使用“Mouse”术语来指示指针数据，例如 MousePos, IsMousePressed(), io.AddMousePosEvent()
// 但这些“Mouse”数据可能来自不同的来源，偶尔应用程序需要知道这些来源。
// 可以通过 io.AddMouseSourceEvent() 提交指针类型的变化。
enum ImGuiMouseSource : int
{
    ImGuiMouseSource_Mouse = 0,   // 输入来自实际的鼠标
    ImGuiMouseSource_TouchScreen, // 输入来自触摸屏（没有悬停，初次按压不精确，可能有双轴滚轮）
    ImGuiMouseSource_Pen,         // 输入来自压感笔或磁性笔（通常与高采样率一起使用）
    ImGuiMouseSource_COUNT
};

// ImGui::SetNextWindow***(), SetWindow***(), SetNextItem***() 等函数的条件枚举类型
// 重要：视为常规枚举！不要使用二进制运算符组合多个值！上述函数将 0 视为 ImGuiCond_Always 的快捷方式。
enum ImGuiCond_
{
    ImGuiCond_None = 0,              // 无条件（始终设置变量），与 _Always 相同
    ImGuiCond_Always = 1 << 0,       // 无条件（始终设置变量），与 _None 相同
    ImGuiCond_Once = 1 << 1,         // 在运行时会话中仅设置一次（只有第一次调用会成功）
    ImGuiCond_FirstUseEver = 1 << 2, // 如果对象/窗口没有持久保存数据（在.ini文件中没有条目），则设置变量
    ImGuiCond_Appearing = 1 << 3,    // 如果对象/窗口在隐藏或不活跃之后重新出现（或第一次出现），则设置变量
};

//-----------------------------------------------------------------------------
// [SECTION] 表格 API 标志和结构（ImGuiTableFlags、ImGuiTableColumnFlags、ImGuiTableRowFlags、ImGuiTableBgTarget、ImGuiTableSortSpecs、ImGuiTableColumnSortSpecs）
//-----------------------------------------------------------------------------

// ImGui::BeginTable() 的标志
// - 重要！尺寸策略有复杂且微妙的副作用，比你预期的要多。
//   仔细阅读注释和示例 + 通过实时示例来熟悉它们。
// - 默认的尺寸策略是：
//    - 如果启用了 ScrollX，或者主机窗口有 ImGuiWindowFlags_AlwaysAutoResize，默认为 ImGuiTableFlags_SizingFixedFit。
//    - 如果禁用了 ScrollX，默认为 ImGuiTableFlags_SizingStretchSame。
// - 当 ScrollX 被禁用时：
//    - 表格默认采用 ImGuiTableFlags_SizingStretchSame -> 所有列默认采用 ImGuiTableColumnFlags_WidthStretch，并具有相同的权重。
//    - 列的尺寸策略允许：Stretch（默认），Fixed/Auto。
//    - 固定列（如果有的话）通常会获得其请求的宽度（除非表格无法容纳所有列）。
//    - 拉伸列将根据各自的权重共享剩余宽度。
//    - 混合使用固定列/拉伸列是可能的，但会对调整大小行为产生不同的副作用。
//      混合尺寸策略的典型用法是：任意数量的领先固定列，后跟一个或两个尾部拉伸列。
//      （这是因为列的可见顺序对它们如何响应手动调整大小有微妙但必要的影响。）
// - 当启用了 ScrollX 时：
//    - 表格默认采用 ImGuiTableFlags_SizingFixedFit -> 所有列默认采用 ImGuiTableColumnFlags_WidthFixed。
//    - 允许的列尺寸策略：主要是 Fixed/Auto。
//    - 固定列可以根据需要增大。表格将在需要时显示水平滚动条。
//    - 当使用自动调整大小的（不可调整大小的）固定列时，查询内容宽度以使用项目右对齐，例如 SetNextItemWidth(-FLT_MIN) 是没有意义的，这会导致反馈循环。
//    - 如果启用了 ScrollX，通常不建议使用拉伸列，除非你在 BeginTable() 中指定了 'inner_width' 的值。
//      如果指定了 'inner_width' 的值，那么有效地就知道了滚动区域，拉伸或混合使用固定/拉伸列会再次变得有意义。
// - 详情请阅读 imgui_tables.cpp 顶部的文档。

enum ImGuiTableFlags_
{
    // 表格的功能标志
    ImGuiTableFlags_None = 0,
    ImGuiTableFlags_Resizable = 1 << 0,         // 启用列的调整大小
    ImGuiTableFlags_Reorderable = 1 << 1,       // 启用列的排序（需要调用 TableSetupColumn() + TableHeadersRow() 来显示表头）
    ImGuiTableFlags_Hideable = 1 << 2,          // 启用在上下文菜单中隐藏/禁用列
    ImGuiTableFlags_Sortable = 1 << 3,          // 启用排序。调用 TableGetSortSpecs() 来获取排序规格。也可参考 ImGuiTableFlags_SortMulti 和 ImGuiTableFlags_SortTristate。
    ImGuiTableFlags_NoSavedSettings = 1 << 4,   // 禁用在 .ini 文件中持久保存列的顺序、宽度和排序设置
    ImGuiTableFlags_ContextMenuInBody = 1 << 5, // 在列的内容区域右键单击将显示表格上下文菜单。默认情况下，它在 TableHeadersRow() 中可用。
    // 装饰
    ImGuiTableFlags_RowBg = 1 << 6,                                                               // 设置每行背景颜色，使用 ImGuiCol_TableRowBg 或 ImGuiCol_TableRowBgAlt（相当于在每一行上手动调用 TableSetBgColor，使用 ImGuiTableBgFlags_RowBg0）
    ImGuiTableFlags_BordersInnerH = 1 << 7,                                                       // 绘制行之间的水平边框
    ImGuiTableFlags_BordersOuterH = 1 << 8,                                                       // 绘制顶部和底部的水平边框
    ImGuiTableFlags_BordersInnerV = 1 << 9,                                                       // 绘制列之间的垂直边框
    ImGuiTableFlags_BordersOuterV = 1 << 10,                                                      // 绘制左右两边的垂直边框
    ImGuiTableFlags_BordersH = ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersOuterH,     // 绘制水平边框
    ImGuiTableFlags_BordersV = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuterV,     // 绘制垂直边框
    ImGuiTableFlags_BordersInner = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersInnerH, // 绘制内边框
    ImGuiTableFlags_BordersOuter = ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_BordersOuterH, // 绘制外边框
    ImGuiTableFlags_Borders = ImGuiTableFlags_BordersInner | ImGuiTableFlags_BordersOuter,        // 绘制所有边框
    ImGuiTableFlags_NoBordersInBody = 1 << 11,                                                    // [ALPHA] 禁用列内容区域中的垂直边框（边框始终出现在表头）。-> 可能会转移到样式
    ImGuiTableFlags_NoBordersInBodyUntilResize = 1 << 12,                                         // [ALPHA] 禁用列内容区域中的垂直边框，直到悬停进行调整大小（边框始终出现在表头）。-> 可能会转移到样式
    // 尺寸策略（查看上面的默认值）
    ImGuiTableFlags_SizingFixedFit = 1 << 13,    // 列默认采用 _WidthFixed 或 _WidthAuto（无论是否可调整大小），与内容宽度匹配
    ImGuiTableFlags_SizingFixedSame = 2 << 13,   // 列默认采用 _WidthFixed 或 _WidthAuto（无论是否可调整大小），与所有列的最大内容宽度匹配。隐式启用 ImGuiTableFlags_NoKeepColumnsVisible。
    ImGuiTableFlags_SizingStretchProp = 3 << 13, // 列默认采用 _WidthStretch，权重与各列的内容宽度成比例
    ImGuiTableFlags_SizingStretchSame = 4 << 13, // 列默认采用 _WidthStretch，所有权重相等，除非通过 TableSetupColumn() 重写
    // 尺寸额外选项
    ImGuiTableFlags_NoHostExtendX = 1 << 16,        // 使外部宽度自动适应列，覆盖 outer_size.x 值。仅在 ScrollX/ScrollY 被禁用且没有使用 Stretch 列时可用
    ImGuiTableFlags_NoHostExtendY = 1 << 17,        // 使外部高度恰好停留在 outer_size.y（防止表格超出限制自动扩展）。仅在 ScrollX/ScrollY 被禁用时可用。数据将被裁剪并不可见
    ImGuiTableFlags_NoKeepColumnsVisible = 1 << 18, // 禁用在关闭 ScrollX 且表格过小时始终保持列最小可见。对于可调整大小的列，不推荐使用
    ImGuiTableFlags_PreciseWidths = 1 << 19,        // 禁用将剩余宽度分配给拉伸列（在 100 宽表格中有 3 列时的宽度分配：没有此标志：33,33,34。有此标志：33,33,33）。列数较多时，调整大小将看起来更平滑
    // 剪辑
    ImGuiTableFlags_NoClip = 1 << 20, // 禁用每个单独列的剪辑矩形（减少绘制命令数量，项将能够溢出到其他列）。通常与 TableSetupScrollFreeze() 不兼容
    // 填充
    ImGuiTableFlags_PadOuterX = 1 << 21,   // 如果 BordersOuterV 打开，默认为启用外部填充。通常，如果你有表头，这是推荐的
    ImGuiTableFlags_NoPadOuterX = 1 << 22, // 如果 BordersOuterV 关闭，默认为禁用外部填充
    ImGuiTableFlags_NoPadInnerX = 1 << 23, // 禁用列之间的内填充（如果 BordersOuterV 打开则为双内填充，关闭时为单内填充）
    // 滚动
    ImGuiTableFlags_ScrollX = 1 << 24, // 启用水平滚动。需要在 BeginTable() 中指定容器大小的 'outer_size' 参数。更改默认尺寸策略。由于这会创建一个子窗口，当前在使用 ScrollX 时推荐使用 ScrollY。
    ImGuiTableFlags_ScrollY = 1 << 25, // 启用垂直滚动。需要在 BeginTable() 中指定容器大小的 'outer_size' 参数。
    // 排序
    ImGuiTableFlags_SortMulti = 1 << 26,    // 在点击表头时按住 shift 键以对多个列进行排序。TableGetSortSpecs() 可能返回规格（SpecsCount > 1）
    ImGuiTableFlags_SortTristate = 1 << 27, // 允许无排序，禁用默认排序。TableGetSortSpecs() 可能返回规格（SpecsCount == 0）
    // 其他
    ImGuiTableFlags_HighlightHoveredColumn = 1 << 28, // 悬停时高亮显示列表头（可能演变成更完整的高亮）

    // [内部] 组合和掩码
    ImGuiTableFlags_SizingMask_ = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_SizingStretchSame,

};

//-----------------------------------------------------------------------------
// ImGui::TableSetupColumn() 的标志
//-----------------------------------------------------------------------------

enum ImGuiTableColumnFlags_
{
    // 输入配置标志
    ImGuiTableColumnFlags_None = 0,
    ImGuiTableColumnFlags_Disabled = 1 << 0,              // 覆盖/主禁用标志：隐藏列，不能显示在上下文菜单中（与调用 TableSetColumnEnabled() 不同，后者修改用户可访问状态）
    ImGuiTableColumnFlags_DefaultHide = 1 << 1,           // 默认隐藏/禁用列。
    ImGuiTableColumnFlags_DefaultSort = 1 << 2,           // 默认作为排序列。
    ImGuiTableColumnFlags_WidthStretch = 1 << 3,          // 列将拉伸。最好禁用水平滚动（如果表格尺寸策略为 _SizingStretchSame 或 _SizingStretchProp，则默认）。
    ImGuiTableColumnFlags_WidthFixed = 1 << 4,            // 列不会拉伸。最好启用水平滚动（如果表格尺寸策略为 _SizingFixedFit 且表格可调整大小，则默认）。
    ImGuiTableColumnFlags_NoResize = 1 << 5,              // 禁用手动调整大小。
    ImGuiTableColumnFlags_NoReorder = 1 << 6,             // 禁用手动重新排序此列，这也将阻止其他列穿过此列。
    ImGuiTableColumnFlags_NoHide = 1 << 7,                // 禁用隐藏/禁用此列的功能。
    ImGuiTableColumnFlags_NoClip = 1 << 8,                // 禁用此列的裁剪（所有 NoClip 列将在同一绘制命令中渲染）。
    ImGuiTableColumnFlags_NoSort = 1 << 9,                // 禁用对该字段的排序（即使表格设置了 ImGuiTableFlags_Sortable）。
    ImGuiTableColumnFlags_NoSortAscending = 1 << 10,      // 禁用升序排序。
    ImGuiTableColumnFlags_NoSortDescending = 1 << 11,     // 禁用降序排序。
    ImGuiTableColumnFlags_NoHeaderLabel = 1 << 12,        // TableHeadersRow() 将提交此列的空标签。适用于一些小列。名称仍将出现在上下文菜单或倾斜的列标题中。你可以在 TableHeadersRow() 调用后通过 TableSetColumnIndex() 添加到该单元格。
    ImGuiTableColumnFlags_NoHeaderWidth = 1 << 13,        // 禁用标题文本宽度对自动列宽的贡献。
    ImGuiTableColumnFlags_PreferSortAscending = 1 << 14,  // 当首次对该列进行排序时，初始排序方向为升序（默认）。
    ImGuiTableColumnFlags_PreferSortDescending = 1 << 15, // 当首次对该列进行排序时，初始排序方向为降序。
    ImGuiTableColumnFlags_IndentEnable = 1 << 16,         // 进入单元格时使用当前缩进值（列 0 的默认值）。
    ImGuiTableColumnFlags_IndentDisable = 1 << 17,        // 进入单元格时忽略当前缩进值（列 > 0 的默认值）。单元格内的缩进变化仍然会被遵循。
    ImGuiTableColumnFlags_AngledHeader = 1 << 18,         // TableHeadersRow() 将为此列提交一个倾斜的列标题行。请注意，这将添加额外的一行。

    // 输出状态标志，仅通过 TableGetColumnFlags() 只读
    ImGuiTableColumnFlags_IsEnabled = 1 << 24, // 状态：已启用 == 没有被用户/API 隐藏（在 _DefaultHide 和 _NoHide 中称为 "Hide" 标志）。
    ImGuiTableColumnFlags_IsVisible = 1 << 25, // 状态：已可见 == 已启用并且没有被滚动裁剪。
    ImGuiTableColumnFlags_IsSorted = 1 << 26,  // 状态：当前是排序规格的一部分
    ImGuiTableColumnFlags_IsHovered = 1 << 27, // 状态：当前被鼠标悬停

    // [内部] 组合和掩码
    ImGuiTableColumnFlags_WidthMask_ = ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_WidthFixed,
    ImGuiTableColumnFlags_IndentMask_ = ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_IndentDisable,
    ImGuiTableColumnFlags_StatusMask_ = ImGuiTableColumnFlags_IsEnabled | ImGuiTableColumnFlags_IsVisible | ImGuiTableColumnFlags_IsSorted | ImGuiTableColumnFlags_IsHovered,
    ImGuiTableColumnFlags_NoDirectResize_ = 1 << 30, // [内部] 禁止用户直接调整此列大小（但它可能会通过左边缘间接调整大小）
};

//-----------------------------------------------------------------------------
// ImGui::TableNextRow() 的标志
//-----------------------------------------------------------------------------

enum ImGuiTableRowFlags_
{
    ImGuiTableRowFlags_None = 0,
    ImGuiTableRowFlags_Headers = 1 << 0, // 标识标题行（设置默认背景颜色 + 内容宽度不同于自动列宽度的计算）
};

//-----------------------------------------------------------------------------
// ImGui::TableSetBgColor() 的枚举
// 背景颜色以 3 个层次渲染：
//  - 层 0：如果设置了 RowBg0 颜色，则使用 RowBg0 颜色绘制，否则使用设置的 ColumnBg0。
//  - 层 1：如果设置了 RowBg1 颜色，则使用 RowBg1 颜色绘制，否则使用设置的 ColumnBg1。
//  - 层 2：如果设置了 CellBg 颜色，则使用 CellBg 颜色绘制。
// 这两个行/列层的目的是让你决定背景色更改是否应覆盖或与现有颜色融合。
// 当在表格上使用 ImGuiTableFlags_RowBg 时，每行自动为奇数/偶数行设置 RowBg0 颜色。
// 如果你设置了 RowBg0 目标的颜色，那么你的颜色将覆盖现有的 RowBg0 颜色。
// 如果设置了 RowBg1 或 ColumnBg1 目标的颜色，那么你的颜色将与 RowBg0 颜色融合。
enum ImGuiTableBgTarget_
{
    ImGuiTableBgTarget_None = 0,
    ImGuiTableBgTarget_RowBg0 = 1, // 设置行背景颜色 0（通常用于背景，启用 ImGuiTableFlags_RowBg 时自动设置）
    ImGuiTableBgTarget_RowBg1 = 2, // 设置行背景颜色 1（通常用于选中标记）
    ImGuiTableBgTarget_CellBg = 3, // 设置单元格背景颜色（最上层颜色）
};

// 表格的排序规格（通常处理单列的排序规格，有时处理多个列的排序）
// 通过调用 TableGetSortSpecs() 获取。
// 当 'SpecsDirty == true' 时，表示可以对数据进行排序。
// 如果排序规格自上次调用以来发生变化，或者是第一次调用时，它将为 true。
// 确保在排序后将 'SpecsDirty' 设置为 false，否则每帧可能会浪费性地排序数据！
struct ImGuiTableSortSpecs
{
    const ImGuiTableColumnSortSpecs *Specs; // 指向排序规格数组的指针
    int SpecsCount;                         // 排序规格的数量。通常为 1。当启用 ImGuiTableFlags_SortMulti 时，可能大于 1。当启用 ImGuiTableFlags_SortTristate 时，可能为 0。
    bool SpecsDirty;                        // 当排序规格自上次调用以来发生变化时为 true！用此标志来决定是否需要重新排序数据，然后清除该标志。

    ImGuiTableSortSpecs()       { memset(this, 0, sizeof(*this)); }
};

// 表格单列的排序规格（大小 == 12 字节）
struct ImGuiTableColumnSortSpecs
{
    ImGuiID ColumnUserID;             // 列的用户 ID（如果在 TableSetupColumn() 中指定）
    ImS16 ColumnIndex;                // 列的索引
    ImS16 SortOrder;                  // 在父 ImGuiTableSortSpecs 中的索引（始终按顺序存储，从 0 开始，按单一标准排序的表格将始终为 0）
    ImGuiSortDirection SortDirection; // ImGuiSortDirection_Ascending 或 ImGuiSortDirection_Descending

    ImGuiTableColumnSortSpecs() { memset(this, 0, sizeof(*this)); }
};

//-----------------------------------------------------------------------------
// [SECTION] Helpers: Debug log, memory allocations macros, ImVector<>
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Debug Logging into ShowDebugLogWindow(), tty and more.
//-----------------------------------------------------------------------------

#ifndef IMGUI_DISABLE_DEBUG_TOOLS
#define IMGUI_DEBUG_LOG(...)        ImGui::DebugLog(__VA_ARGS__)
#else
#define IMGUI_DEBUG_LOG(...)        ((void)0)
#endif

//-----------------------------------------------------------------------------
// IM_MALLOC(), IM_FREE(), IM_NEW(), IM_PLACEMENT_NEW(), IM_DELETE()
// We call C++ constructor on own allocated memory via the placement "new(ptr) Type()" syntax.
// Defining a custom placement new() with a custom parameter allows us to bypass including <new> which on some platforms complains when user has disabled exceptions.
//-----------------------------------------------------------------------------

struct ImNewWrapper {};
inline void* operator new(size_t, ImNewWrapper, void* ptr) { return ptr; }
inline void  operator delete(void*, ImNewWrapper, void*)   {} // This is only required so we can use the symmetrical new()
#define IM_ALLOC(_SIZE)                     ImGui::MemAlloc(_SIZE)
#define IM_FREE(_PTR)                       ImGui::MemFree(_PTR)
#define IM_PLACEMENT_NEW(_PTR)              new(ImNewWrapper(), _PTR)
#define IM_NEW(_TYPE)                       new(ImNewWrapper(), ImGui::MemAlloc(sizeof(_TYPE))) _TYPE
template<typename T> void IM_DELETE(T* p)   { if (p) { p->~T(); ImGui::MemFree(p); } }

//-----------------------------------------------------------------------------
// ImVector<>
// 轻量级的 std::vector<> 类，旨在避免引入依赖项（此外，一些启用调试的 STL 实现极其缓慢，我们绕过它以便在调试模式下运行更快）。
//-----------------------------------------------------------------------------
// - 一般情况下，你不需要关心或使用这个类。但我们需要在 imgui.h 中提供它，因为一些公共结构依赖于它。
// - 我们采用类似标准库的命名惯例，这对这个代码库来说有点不常见。
// - 重要：clear() 会释放内存，resize(0) 会保持分配的缓冲区。我们经常使用 resize(0) 来故意回收分配的缓冲区，以便在帧之间平摊成本。
// - 重要：我们的实现不会调用 C++ 的构造函数/析构函数，我们将一切视为原始数据！这是故意为之，但请特别留意，
//   不要在自己的代码中将此类用作 std::vector 的替代品！Dear ImGui 使用的许多结构可以通过零内存初始化安全地初始化。
//-----------------------------------------------------------------------------

IM_MSVC_RUNTIME_CHECKS_OFF
template<typename T>
struct ImVector
{
    int                 Size;
    int                 Capacity;
    T*                  Data;

    // 提供标准的类型定义，但我们自己并不使用它们。
    typedef T                   value_type;
    typedef value_type*         iterator;
    typedef const value_type*   const_iterator;

    // 构造函数，析构函数
    inline ImVector()                                       { Size = Capacity = 0; Data = NULL; }
    inline ImVector(const ImVector<T>& src)                 { Size = Capacity = 0; Data = NULL; operator=(src); }
    inline ImVector<T>& operator=(const ImVector<T>& src)   { clear(); resize(src.Size); if (src.Data) memcpy(Data, src.Data, (size_t)Size * sizeof(T)); return *this; }
    inline ~ImVector()                                      { if (Data) IM_FREE(Data); } // Important: does not destruct anything

    inline void         clear()                             { if (Data) { Size = Capacity = 0; IM_FREE(Data); Data = NULL; } }  // Important: does not destruct anything
    inline void         clear_delete()                      { for (int n = 0; n < Size; n++) IM_DELETE(Data[n]); clear(); }     // Important: never called automatically! always explicit.
    inline void         clear_destruct()                    { for (int n = 0; n < Size; n++) Data[n].~T(); clear(); }           // Important: never called automatically! always explicit.

    inline bool         empty() const                       { return Size == 0; }
    inline int          size() const                        { return Size; }
    inline int          size_in_bytes() const               { return Size * (int)sizeof(T); }
    inline int          max_size() const                    { return 0x7FFFFFFF / (int)sizeof(T); }
    inline int          capacity() const                    { return Capacity; }
    inline T&           operator[](int i)                   { IM_ASSERT(i >= 0 && i < Size); return Data[i]; }
    inline const T&     operator[](int i) const             { IM_ASSERT(i >= 0 && i < Size); return Data[i]; }

    inline T*           begin()                             { return Data; }
    inline const T*     begin() const                       { return Data; }
    inline T*           end()                               { return Data + Size; }
    inline const T*     end() const                         { return Data + Size; }
    inline T&           front()                             { IM_ASSERT(Size > 0); return Data[0]; }
    inline const T&     front() const                       { IM_ASSERT(Size > 0); return Data[0]; }
    inline T&           back()                              { IM_ASSERT(Size > 0); return Data[Size - 1]; }
    inline const T&     back() const                        { IM_ASSERT(Size > 0); return Data[Size - 1]; }
    inline void         swap(ImVector<T>& rhs)              { int rhs_size = rhs.Size; rhs.Size = Size; Size = rhs_size; int rhs_cap = rhs.Capacity; rhs.Capacity = Capacity; Capacity = rhs_cap; T* rhs_data = rhs.Data; rhs.Data = Data; Data = rhs_data; }

    inline int          _grow_capacity(int sz) const        { int new_capacity = Capacity ? (Capacity + Capacity / 2) : 8; return new_capacity > sz ? new_capacity : sz; }
    inline void         resize(int new_size)                { if (new_size > Capacity) reserve(_grow_capacity(new_size)); Size = new_size; }
    inline void         resize(int new_size, const T& v)    { if (new_size > Capacity) reserve(_grow_capacity(new_size)); if (new_size > Size) for (int n = Size; n < new_size; n++) memcpy(&Data[n], &v, sizeof(v)); Size = new_size; }
    inline void         shrink(int new_size)                { IM_ASSERT(new_size <= Size); Size = new_size; } // Resize a vector to a smaller size, guaranteed not to cause a reallocation
    inline void         reserve(int new_capacity)           { if (new_capacity <= Capacity) return; T* new_data = (T*)IM_ALLOC((size_t)new_capacity * sizeof(T)); if (Data) { memcpy(new_data, Data, (size_t)Size * sizeof(T)); IM_FREE(Data); } Data = new_data; Capacity = new_capacity; }
    inline void         reserve_discard(int new_capacity)   { if (new_capacity <= Capacity) return; if (Data) IM_FREE(Data); Data = (T*)IM_ALLOC((size_t)new_capacity * sizeof(T)); Capacity = new_capacity; }

    // 注意：禁止在 ImVector 数据内部引用时调用 push_back/push_front/insert！例如，v.push_back(v[10]) 是禁止的。
    inline void         push_back(const T& v)               { if (Size == Capacity) reserve(_grow_capacity(Size + 1)); memcpy(&Data[Size], &v, sizeof(v)); Size++; }
    inline void         pop_back()                          { IM_ASSERT(Size > 0); Size--; }
    inline void         push_front(const T& v)              { if (Size == 0) push_back(v); else insert(Data, v); }
    inline T*           erase(const T* it)                  { IM_ASSERT(it >= Data && it < Data + Size); const ptrdiff_t off = it - Data; memmove(Data + off, Data + off + 1, ((size_t)Size - (size_t)off - 1) * sizeof(T)); Size--; return Data + off; }
    inline T*           erase(const T* it, const T* it_last){ IM_ASSERT(it >= Data && it < Data + Size && it_last >= it && it_last <= Data + Size); const ptrdiff_t count = it_last - it; const ptrdiff_t off = it - Data; memmove(Data + off, Data + off + count, ((size_t)Size - (size_t)off - (size_t)count) * sizeof(T)); Size -= (int)count; return Data + off; }
    inline T*           erase_unsorted(const T* it)         { IM_ASSERT(it >= Data && it < Data + Size);  const ptrdiff_t off = it - Data; if (it < Data + Size - 1) memcpy(Data + off, Data + Size - 1, sizeof(T)); Size--; return Data + off; }
    inline T*           insert(const T* it, const T& v)     { IM_ASSERT(it >= Data && it <= Data + Size); const ptrdiff_t off = it - Data; if (Size == Capacity) reserve(_grow_capacity(Size + 1)); if (off < (int)Size) memmove(Data + off + 1, Data + off, ((size_t)Size - (size_t)off) * sizeof(T)); memcpy(&Data[off], &v, sizeof(v)); Size++; return Data + off; }
    inline bool         contains(const T& v) const          { const T* data = Data;  const T* data_end = Data + Size; while (data < data_end) if (*data++ == v) return true; return false; }
    inline T*           find(const T& v)                    { T* data = Data;  const T* data_end = Data + Size; while (data < data_end) if (*data == v) break; else ++data; return data; }
    inline const T*     find(const T& v) const              { const T* data = Data;  const T* data_end = Data + Size; while (data < data_end) if (*data == v) break; else ++data; return data; }
    inline int          find_index(const T& v) const        { const T* data_end = Data + Size; const T* it = find(v); if (it == data_end) return -1; const ptrdiff_t off = it - Data; return (int)off; }
    inline bool         find_erase(const T& v)              { const T* it = find(v); if (it < Data + Size) { erase(it); return true; } return false; }
    inline bool         find_erase_unsorted(const T& v)     { const T* it = find(v); if (it < Data + Size) { erase_unsorted(it); return true; } return false; }
    inline int          index_from_ptr(const T* it) const   { IM_ASSERT(it >= Data && it < Data + Size); const ptrdiff_t off = it - Data; return (int)off; }
};
IM_MSVC_RUNTIME_CHECKS_RESTORE

//-----------------------------------------------------------------------------
// [SECTION] ImGuiStyle
//-----------------------------------------------------------------------------
// You may modify the ImGui::GetStyle() main instance during initialization and before NewFrame().
// During the frame, use ImGui::PushStyleVar(ImGuiStyleVar_XXXX)/PopStyleVar() to alter the main style values,
// and ImGui::PushStyleColor(ImGuiCol_XXX)/PopStyleColor() for colors.
//-----------------------------------------------------------------------------

struct ImGuiStyle
{
    float       Alpha;                      // Global alpha applies to everything in Dear ImGui.
    float       DisabledAlpha;              // Additional alpha multiplier applied by BeginDisabled(). Multiply over current value of Alpha.
    ImVec2      WindowPadding;              // Padding within a window.
    float       WindowRounding;             // Radius of window corners rounding. Set to 0.0f to have rectangular windows. Large values tend to lead to variety of artifacts and are not recommended.
    float       WindowBorderSize;           // Thickness of border around windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
    ImVec2      WindowMinSize;              // Minimum window size. This is a global setting. If you want to constrain individual windows, use SetNextWindowSizeConstraints().
    ImVec2      WindowTitleAlign;           // Alignment for title bar text. Defaults to (0.0f,0.5f) for left-aligned,vertically centered.
    ImGuiDir    WindowMenuButtonPosition;   // Side of the collapsing/docking button in the title bar (None/Left/Right). Defaults to ImGuiDir_Left.
    float       ChildRounding;              // Radius of child window corners rounding. Set to 0.0f to have rectangular windows.
    float       ChildBorderSize;            // Thickness of border around child windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
    float       PopupRounding;              // Radius of popup window corners rounding. (Note that tooltip windows use WindowRounding)
    float       PopupBorderSize;            // Thickness of border around popup/tooltip windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
    ImVec2      FramePadding;               // Padding within a framed rectangle (used by most widgets).
    float       FrameRounding;              // Radius of frame corners rounding. Set to 0.0f to have rectangular frame (used by most widgets).
    float       FrameBorderSize;            // Thickness of border around frames. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
    ImVec2      ItemSpacing;                // Horizontal and vertical spacing between widgets/lines.
    ImVec2      ItemInnerSpacing;           // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label).
    ImVec2      CellPadding;                // Padding within a table cell. Cellpadding.x is locked for entire table. CellPadding.y may be altered between different rows.
    ImVec2      TouchExtraPadding;          // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
    float       IndentSpacing;              // Horizontal indentation when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
    float       ColumnsMinSpacing;          // Minimum horizontal spacing between two columns. Preferably > (FramePadding.x + 1).
    float       ScrollbarSize;              // Width of the vertical scrollbar, Height of the horizontal scrollbar.
    float       ScrollbarRounding;          // Radius of grab corners for scrollbar.
    float       GrabMinSize;                // Minimum width/height of a grab box for slider/scrollbar.
    float       GrabRounding;               // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
    float       LogSliderDeadzone;          // The size in pixels of the dead-zone around zero on logarithmic sliders that cross zero.
    float       TabRounding;                // Radius of upper corners of a tab. Set to 0.0f to have rectangular tabs.
    float       TabBorderSize;              // Thickness of border around tabs.
    float       TabMinWidthForCloseButton;  // Minimum width for close button to appear on an unselected tab when hovered. Set to 0.0f to always show when hovering, set to FLT_MAX to never show close button unless selected.
    float       TabBarBorderSize;           // Thickness of tab-bar separator, which takes on the tab active color to denote focus.
    float       TabBarOverlineSize;         // Thickness of tab-bar overline, which highlights the selected tab-bar.
    float       TableAngledHeadersAngle;    // Angle of angled headers (supported values range from -50.0f degrees to +50.0f degrees).
    ImVec2      TableAngledHeadersTextAlign;// Alignment of angled headers within the cell
    ImGuiDir    ColorButtonPosition;        // Side of the color button in the ColorEdit4 widget (left/right). Defaults to ImGuiDir_Right.
    ImVec2      ButtonTextAlign;            // Alignment of button text when button is larger than text. Defaults to (0.5f, 0.5f) (centered).
    ImVec2      SelectableTextAlign;        // Alignment of selectable text. Defaults to (0.0f, 0.0f) (top-left aligned). It's generally important to keep this left-aligned if you want to lay multiple items on a same line.
    float       SeparatorTextBorderSize;    // Thickness of border in SeparatorText()
    ImVec2      SeparatorTextAlign;         // Alignment of text within the separator. Defaults to (0.0f, 0.5f) (left aligned, center).
    ImVec2      SeparatorTextPadding;       // Horizontal offset of text from each edge of the separator + spacing on other axis. Generally small values. .y is recommended to be == FramePadding.y.
    ImVec2      DisplayWindowPadding;       // Apply to regular windows: amount which we enforce to keep visible when moving near edges of your screen.
    ImVec2      DisplaySafeAreaPadding;     // Apply to every windows, menus, popups, tooltips: amount where we avoid displaying contents. Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).
    float       DockingSeparatorSize;       // Thickness of resizing border between docked windows
    float       MouseCursorScale;           // Scale software rendered mouse cursor (when io.MouseDrawCursor is enabled). We apply per-monitor DPI scaling over this scale. May be removed later.
    bool        AntiAliasedLines;           // Enable anti-aliased lines/borders. Disable if you are really tight on CPU/GPU. Latched at the beginning of the frame (copied to ImDrawList).
    bool        AntiAliasedLinesUseTex;     // Enable anti-aliased lines/borders using textures where possible. Require backend to render with bilinear filtering (NOT point/nearest filtering). Latched at the beginning of the frame (copied to ImDrawList).
    bool        AntiAliasedFill;            // Enable anti-aliased edges around filled shapes (rounded rectangles, circles, etc.). Disable if you are really tight on CPU/GPU. Latched at the beginning of the frame (copied to ImDrawList).
    float       CurveTessellationTol;       // Tessellation tolerance when using PathBezierCurveTo() without a specific number of segments. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.
    float       CircleTessellationMaxError; // Maximum error (in pixels) allowed when using AddCircle()/AddCircleFilled() or drawing rounded corner rectangles with no explicit segment count specified. Decrease for higher quality but more geometry.
    ImVec4      Colors[ImGuiCol_COUNT];

    // Behaviors
    // (It is possible to modify those fields mid-frame if specific behavior need it, unlike e.g. configuration fields in ImGuiIO)
    float             HoverStationaryDelay;     // Delay for IsItemHovered(ImGuiHoveredFlags_Stationary). Time required to consider mouse stationary.
    float             HoverDelayShort;          // Delay for IsItemHovered(ImGuiHoveredFlags_DelayShort). Usually used along with HoverStationaryDelay.
    float             HoverDelayNormal;         // Delay for IsItemHovered(ImGuiHoveredFlags_DelayNormal). "
    ImGuiHoveredFlags HoverFlagsForTooltipMouse;// Default flags when using IsItemHovered(ImGuiHoveredFlags_ForTooltip) or BeginItemTooltip()/SetItemTooltip() while using mouse.
    ImGuiHoveredFlags HoverFlagsForTooltipNav;  // Default flags when using IsItemHovered(ImGuiHoveredFlags_ForTooltip) or BeginItemTooltip()/SetItemTooltip() while using keyboard/gamepad.

    IMGUI_API ImGuiStyle();
    IMGUI_API void ScaleAllSizes(float scale_factor);
};

//-----------------------------------------------------------------------------
// [SECTION] ImGuiIO
//-----------------------------------------------------------------------------
// 使用这个结构与 Dear ImGui 通信大多数设置和输入/输出。
// 通过 ImGui::GetIO() 访问。请阅读 .cpp 文件中的 “Programmer guide” 部分以了解一般用法。
// 一般期望：
// - 初始化阶段：后端和用户代码写入 ImGuiIO。
// - 主循环阶段：后端写入 ImGuiIO，用户代码和 ImGui 代码从 ImGuiIO 读取。
//-----------------------------------------------------------------------------
// 还可以查看 ImGui::GetPlatformIO() 和 ImGuiPlatformIO 结构体，以获取与操作系统/平台相关的功能：剪贴板、IME 等。
//-----------------------------------------------------------------------------

// [内部] IsKeyDown()、IsKeyPressed() 等函数使用的存储。
// 如果你在 1.87 之前使用过 io.KeysDownDuration[]（标记为内部），你应该使用 GetKeyData(key)->DownDuration 而不是 io.KeysData[key]->DownDuration。
struct ImGuiKeyData
{
    bool Down;              // 如果按键被按下，则为 true
    float DownDuration;     // 按键被按下的持续时间（<0.0f: 未按下，0.0f: 刚按下，>0.0f: 持续按下的时间）
    float DownDurationPrev; // 上一帧按键按下的持续时间
    float AnalogValue;      // 游戏手柄的值范围为 0.0f..1.0f
};

struct ImGuiIO
{
    //------------------------------------------------------------------
    // 配置项                                // 默认值
    //------------------------------------------------------------------

    ImGuiConfigFlags ConfigFlags;   // = 0              // 参见 ImGuiConfigFlags_ 枚举。由用户/应用程序设置。键盘/游戏手柄导航选项等。
    ImGuiBackendFlags BackendFlags; // = 0              // 参见 ImGuiBackendFlags_ 枚举。由后端（imgui_impl_xxx 文件或自定义后端）设置，用于传达后端支持的特性。
    ImVec2 DisplaySize;             // <unset>          // 主显示大小，以像素为单位（通常等于 GetMainViewport()->Size）。可能每帧变化。
    float DeltaTime;                // = 1.0f/60.0f     // 上一帧以来的时间，单位为秒。可能每帧变化。
    float IniSavingRate;            // = 5.0f           // 保存位置/尺寸到 .ini 文件之间的最小时间，单位为秒。
    const char *IniFilename;        // = "imgui.ini"    // .ini 文件路径（重要：默认的 "imgui.ini" 是相对当前工作目录的！设置为 NULL 禁用自动 .ini 加载/保存，或如果你想手动调用 LoadIniSettingsXXX() / SaveIniSettingsXXX() 函数）。
    const char *LogFilename;        // = "imgui_log.txt"// .log 文件路径（默认参数为 ImGui::LogToFile，当没有指定文件时）。
    void *UserData;                 // = NULL           // 存储自定义数据。

    // 字体系统
    ImFontAtlas *Fonts;             // <auto>           // 字体图集：加载、栅格化并将一个或多个字体打包到一个纹理中。
    float FontGlobalScale;          // = 1.0f           // 全局字体缩放
    bool FontAllowUserScaling;      // = false          // [已废弃] 允许用户通过 CTRL+滚轮缩放单独窗口的文本。
    ImFont *FontDefault;            // = NULL           // 在 NewFrame() 中使用的字体。使用 NULL 时使用 Fonts->Fonts[0]。
    ImVec2 DisplayFramebufferScale; // = (1, 1)         // 用于视网膜显示或其他窗口坐标与帧缓冲坐标不同的情况。通常会出现在 ImDrawData::FramebufferScale 中。

    // 键盘/游戏手柄导航选项
    bool ConfigNavSwapGamepadButtons;     // = false          // 交换 激活<>取消（A<>B）按钮，匹配典型的 "Nintendo/日本风格" 游戏手柄布局。
    bool ConfigNavMoveSetMousePos;        // = false          // 方向键/标签导航将鼠标光标传送到指定位置。可能在电视/控制台系统上有用，因为在这些系统中移动虚拟鼠标较为困难。将更新 io.MousePos 并设置 io.WantSetMousePos=true。
    bool ConfigNavCaptureKeyboard;        // = true           // 当 io.NavActive 被设置时，设置 io.WantCaptureKeyboard。
    bool ConfigNavEscapeClearFocusItem;   // = true           // 按 Escape 可以清除聚焦项 + 导航 ID/高亮。如果希望始终保持高亮，设置为 false。
    bool ConfigNavEscapeClearFocusWindow; // = false          // 按 Escape 还可以清除聚焦窗口（是 io.ConfigNavEscapeClearFocusItem 的超集）。
    bool ConfigNavCursorVisibleAuto;      // = true           // 使用方向键导航时，光标会变为可见。鼠标点击会隐藏光标。
    bool ConfigNavCursorVisibleAlways;    // = false          // 导航光标始终可见。

    // 停靠选项（当 ImGuiConfigFlags_DockingEnable 被设置时）
    bool ConfigDockingNoSplit;            // = false          // 简化的停靠模式：禁用窗口分割，因此停靠仅限于将多个窗口合并到标签栏中。
    bool ConfigDockingWithShift;          // = false          // 启用按住 Shift 键时的停靠（减少视觉噪声，允许在更宽的空间中进行拖放）。
    bool ConfigDockingAlwaysTabBar;       // = false          // [BETA] [FIXME: 当前会导致自动调整大小和一般开销的回归] 使每个浮动窗口都显示在停靠节点内。
    bool ConfigDockingTransparentPayload; // = false          // [BETA] 在停靠时使窗口或视口透明，仅在目标视口上显示停靠框。适用于无法同步渲染多个视口的情况。最好与 ConfigViewportsNoAutoMerge 一起使用。

    // 视口选项（当 ImGuiConfigFlags_ViewportsEnable 被设置时）
    bool ConfigViewportsNoAutoMerge;     // = false;         // 设置为让所有浮动的 ImGui 窗口始终创建它们自己的视口。否则，它们将在重叠时合并到主视口中。也可以在单个视口上设置 ImGuiViewportFlags_NoAutoMerge。
    bool ConfigViewportsNoTaskBarIcon;   // = false          // 禁用二级视口的默认操作系统任务栏图标标志。当视口不想要任务栏图标时，会在其上设置 ImGuiViewportFlags_NoTaskBarIcon。
    bool ConfigViewportsNoDecoration;    // = true           // 禁用二级视口的默认操作系统窗口装饰标志。当视口不想要窗口装饰时，会在其上设置 ImGuiViewportFlags_NoDecoration。启用装饰可能会在操作系统级别引发后续问题（例如最小窗口大小）。
    bool ConfigViewportsNoDefaultParent; // = false          // 禁用二级视口默认的操作系统父视口。默认情况下，视口标记为 ParentViewportId = <main_viewport>，期望平台后端在操作系统窗口之间建立父子关系（某些后端可能会忽略这一点）。如果希望默认值为 0，可以设置为 true，那么所有视口都将是顶级操作系统窗口。

    // 杂项选项
    // （你可以在 'Demo->Configuration' 中可视化并与所有选项进行交互）
    bool MouseDrawCursor;                    // = false          // 请求 ImGui 为你绘制鼠标光标（如果你在没有鼠标光标的平台上）。无法轻易重命名为 'io.ConfigXXX'，因为后端实现中经常使用它。
    bool ConfigMacOSXBehaviors;              // = defined(__APPLE__) // 交换 Cmd<>Ctrl 键 + 使用 Alt 代替 Ctrl 的 macOS 风格文本编辑光标移动，快捷键使用 Cmd/Super 代替 Ctrl，使用 Cmd+箭头键进行行/文本的开始和结束，而不是 Home/End，双击按词选择而不是选择整段文本，列表中的多选使用 Cmd/Super 代替 Ctrl。
    bool ConfigInputTrickleEventQueue;       // = true           // 启用输入队列滴流：在同一帧内提交的某些类型事件（例如按钮按下+松开）将在多帧内分散，改善低帧率下的交互。
    bool ConfigInputTextCursorBlink;         // = true           // 启用光标闪烁（可选，因为一些用户认为它分散注意力）。
    bool ConfigInputTextEnterKeepActive;     // = false          // [BETA] 按 Enter 键将保持项目处于活动状态并选择内容（仅限单行）。
    bool ConfigDragClickToInputText;         // = false          // [BETA] 启用通过简单的鼠标点击释放（不移动）将 DragXXX 小部件转换为文本输入框。在没有键盘的设备上不建议使用。
    bool ConfigWindowsResizeFromEdges;       // = true           // 启用从窗口边缘和左下角调整窗口大小。这需要 ImGuiBackendFlags_HasMouseCursors 以获得更好的鼠标光标反馈。（这曾经是每个窗口的 ImGuiWindowFlags_ResizeFromAnySide 标志）
    bool ConfigWindowsMoveFromTitleBarOnly;  // = false      // 启用仅在点击窗口标题栏时才允许移动窗口。此设置不适用于没有标题栏的窗口。
    bool ConfigWindowsCopyContentsWithCtrlC; // = false      // [实验性] CTRL+C 将复制聚焦窗口的内容到剪贴板。实验性功能，因为：（1）有已知的嵌套 Begin/End 对的问题（2）文本输出质量不稳定（3）文本输出是按提交顺序而不是空间顺序进行的。
    bool ConfigScrollbarScrollByPage;        // = true           // 启用在点击滚动条抓取区外时按页滚动。禁用时，始终滚动到点击位置。启用时，Shift+点击滚动到点击位置。
    float ConfigMemoryCompactTimer;          // = 60.0f          // 定时器（以秒为单位），用于在未使用时释放瞬时窗口/表格内存缓冲区。设置为 -1.0f 以禁用。

    // 输入行为
    // （其他变量，预计在 UI 代码中调整的变量，已在 ImGuiStyle 中暴露）
    float MouseDoubleClickTime;    // = 0.30f          // 双击时间，单位：秒。
    float MouseDoubleClickMaxDist; // = 6.0f           // 验证双击的距离阈值，单位：像素。
    float MouseDragThreshold;      // = 6.0f           // 在认为开始拖动之前的距离阈值。
    float KeyRepeatDelay;          // = 0.275f         // 按住一个键/按钮时，开始重复前的延迟时间，单位：秒（例如按钮的重复模式等）。
    float KeyRepeatRate;           // = 0.050f         // 按住一个键/按钮时，它重复的速率，单位：秒。

    //------------------------------------------------------------------
    // 调试选项
    //------------------------------------------------------------------

    // 配置错误处理和我们如何处理可恢复错误的选项 [实验性]
    // - 错误恢复提供了一种方式来促进：
    //    - 在编程错误后进行恢复（原生代码或脚本语言 - 后者通常有助于在运行时反复修改代码）。
    //    - 在运行异常处理程序或任何错误处理后恢复，可能会跳过错误检测后的代码。
    // - 错误恢复并不完美，也不能保证！它是为了简化开发而提供的功能。
    //   你不应当依赖它来执行正常应用程序的运行。
    // - 支持错误恢复的函数使用 IM_ASSERT_USER_ERROR() 而不是 IM_ASSERT()。
    // - 根据设计，我们不允许错误恢复完全静默。必须勾选以下三个选项之一！
    // - 始终确保程序员座位上至少启用断言或工具提示，以便在直接调用 imgui API 时能够捕获和修正错误！
    //   否则会严重妨碍你捕捉并纠正错误的能力！
    // 阅读 https://github.com/ocornut/imgui/wiki/Error-Handling 获取详细信息。
    // - 程序员座位：保持断言（默认），或禁用断言并保持错误工具提示（新的且不错！）
    // - 非程序员座位：也许可以禁用断言，但确保错误能够重新浮现（工具提示、可见日志条目、使用回调等）。
    // - 错误/异常后的恢复：使用 ErrorRecoveryStoreState() 记录堆栈大小，禁用断言，设置日志回调（例如触发高级断点），用 ErrorRecoveryTryToRecoverState() 恢复，恢复设置。
    bool ConfigErrorRecovery;               // = true       // 启用错误恢复支持。禁用恢复时，一些错误无法被检测并导致直接崩溃。
    bool ConfigErrorRecoveryEnableAssert;   // = true       // 启用可恢复错误时的断言。默认在返回失败的 IM_ASSERT_USER_ERROR() 时调用 IM_ASSERT()。
    bool ConfigErrorRecoveryEnableDebugLog; // = true       // 启用可恢复错误时的调试日志输出。
    bool ConfigErrorRecoveryEnableTooltip;  // = true       // 启用可恢复错误时的工具提示。工具提示包含启用断言的方式（如果已禁用）。

    // 启用各种调试工具，显示按钮以调用 IM_DEBUG_BREAK() 宏。
    // - 即使未启用此选项，项目选择工具仍然可用，以最大化其可发现性。
    // - 需要附加调试器，否则 IM_DEBUG_BREAK() 选项会导致应用程序崩溃。
    //   例如，io.ConfigDebugIsDebuggerPresent = ::IsDebuggerPresent() 在 Win32 上，或参考 ImOsIsDebuggerPresent() imgui_test_engine/imgui_te_utils.cpp 获取 Unix 兼容版本。
    bool ConfigDebugIsDebuggerPresent; // = false          // 启用各种调用 IM_DEBUG_BREAK() 的工具。

    // 检测代码提交具有冲突/重复 ID 的项目的工具
    // - 代码应在循环中使用 PushID()/PopID()，或为相同标签的标识符附加 "##xx"。
    // - 空标签，例如 Button("") == 与父窗口/节点相同的 ID。应使用 Button("##xx") 替代！
    // - 参见常见问题 https://github.com/ocornut/imgui/blob/master/docs/FAQ.md#q-about-the-id-stack-system
    bool ConfigDebugHighlightIdConflicts; // = true           // 当多个项目具有冲突的标识符时，高亮显示并显示错误信息。

    // 测试正确的 Begin/End 和 BeginChild/EndChild 行为的工具。
    // - 当前 Begin()/End() 和 BeginChild()/EndChild() 必须始终成对调用，无论 BeginXXX() 的返回值如何。
    // - 这与其他 BeginXXX 函数不一致，给许多用户带来困惑。
    // - 我们预计最终会更新 API。与此同时，我们提供了工具来帮助检查用户代码的行为。
    bool ConfigDebugBeginReturnValueOnce; // = false          // 首次调用 Begin()/BeginChild() 将返回 false。应用启动时必须设置此选项，否则会错过窗口。
    bool ConfigDebugBeginReturnValueLoop; // = false          // 一些调用 Begin()/BeginChild() 将返回 false。将循环遍历窗口深度然后重复。建议使用：在主循环中添加 "io.ConfigDebugBeginReturnValue = io.KeyShift"，然后偶尔按 SHIFT。窗口在运行时应会闪烁。

    // 使能停用 io.AddFocusEvent(false) 的处理。
    // - 当失去焦点导致清除输入数据时，这可能有助于与调试器交互。
    // - 后端可能会对失去焦点产生其他副作用，因此这将减少副作用，但不一定会去除所有副作用。
    bool ConfigDebugIgnoreFocusLoss; // = false          // 忽略 io.AddFocusEvent(false)，因此在输入处理过程中不调用 io.ClearInputKeys()/io.ClearInputMouse()。

    // 审计 .ini 数据的选项
    bool ConfigDebugIniSettings; // = false          // 保存带有额外注释的 .ini 数据（对 Docking 特别有帮助，但会使保存变慢）

    //------------------------------------------------------------------
    // 平台标识符
    // （imgui_impl_xxxx 后端文件为您设置这些）
    //------------------------------------------------------------------

    // 现在这些将存储在 ImGuiPlatformIO 中，但我们出于遗留原因将它们保留在这里。
    // 可选：平台/渲染器后端名称（仅供参考！将在关于窗口中显示）+ 用户数据供后端/包装器存储其自己的数据。
    const char *BackendPlatformName; // = NULL           // 平台后端名称
    const char *BackendRendererName; // = NULL           // 渲染器后端名称
    void *BackendPlatformUserData;   // = NULL           // 平台后端的用户数据
    void *BackendRendererUserData;   // = NULL           // 渲染器后端的用户数据
    void *BackendLanguageUserData;   // = NULL           // 非 C++ 编程语言后端的用户数据

    //------------------------------------------------------------------
    // 输入 - 在调用 NewFrame() 之前调用
    //------------------------------------------------------------------

    // 输入函数
    IMGUI_API void AddKeyEvent(ImGuiKey key, bool down);                // 排队一个新的键盘按下/松开事件。键应该是“翻译过的”（例如，ImGuiKey_A 匹配的是用户用来发出 'A' 字符的按键）
    IMGUI_API void AddKeyAnalogEvent(ImGuiKey key, bool down, float v); // 排队一个新的键盘按下/松开事件，带有模拟值（例如 ImGuiKey_Gamepad_ 值）。死区应由后端处理。
    IMGUI_API void AddMousePosEvent(float x, float y);                  // 排队一个鼠标位置更新。使用 -FLT_MAX,-FLT_MAX 来表示没有鼠标（例如，应用程序未聚焦且未悬停）
    IMGUI_API void AddMouseButtonEvent(int button, bool down);          // 排队一个鼠标按钮变化事件
    IMGUI_API void AddMouseWheelEvent(float wheel_x, float wheel_y);    // 排队一个鼠标滚轮更新。wheel_y<0: 向下滚动，wheel_y>0: 向上滚动，wheel_x<0: 向右滚动，wheel_x>0: 向左滚动。
    IMGUI_API void AddMouseSourceEvent(ImGuiMouseSource source);        // 排队一个鼠标源变化事件（鼠标/触摸屏/手写笔）
    IMGUI_API void AddMouseViewportEvent(ImGuiID id);                   // 排队一个鼠标悬停视口事件。需要后端设置 ImGuiBackendFlags_HasMouseHoveredViewport 以调用此事件（支持多视口）。
    IMGUI_API void AddFocusEvent(bool focused);                         // 排队应用程序的获得/失去焦点事件（通常基于操作系统/平台窗口焦点）
    IMGUI_API void AddInputCharacter(unsigned int c);                   // 排队一个新的字符输入
    IMGUI_API void AddInputCharacterUTF16(ImWchar16 c);                 // 排队一个新的字符输入，来自一个 UTF-16 字符，它可以是代理字符
    IMGUI_API void AddInputCharactersUTF8(const char *str);             // 排队一组新的字符输入，来自一个 UTF-8 字符串

    IMGUI_API void SetKeyEventNativeData(ImGuiKey key, int native_keycode, int native_scancode, int native_legacy_index = -1); // [可选] 为遗留的 <1.87 IsKeyXXX() 函数指定原生索引 + 指定原生键码、扫描码。
    IMGUI_API void SetAppAcceptingEvents(bool accepting_events);                                                               // 设置是否接受键盘/鼠标/文本事件的主标志（默认为 true）。如果您的应用程序有中断应用程序循环/刷新操作的本地对话框，并且您希望在应用程序冻结时禁用事件队列，此时很有用。
    IMGUI_API void ClearEventsQueue();                                                                                         // 清除所有传入的事件。
    IMGUI_API void ClearInputKeys();                                                                                           // 清除当前的键盘/游戏手柄状态 + 当前帧的文本输入缓冲区。等同于释放所有按键/按钮。
    IMGUI_API void ClearInputMouse();                                                                                          // 清除当前的鼠标状态。
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
IMGUI_API void ClearInputCharacters(); // [在 1.89.8 中废弃] 清除当前帧的文本输入缓冲区。现在包括在 ClearInputKeys() 中。
#endif

    //------------------------------------------------------------------
    // 输出 - 由 NewFrame() 或 EndFrame()/Render() 更新
    // （当读取 io.WantCaptureMouse、io.WantCaptureKeyboard 标志来分派输入时，
    //  通常在调用 NewFrame() 之前使用它们的状态更简单且更正确。详情见 FAQ！）
    //------------------------------------------------------------------

    bool WantCaptureMouse;     // 当 Dear ImGui 使用鼠标输入时设置，在这种情况下不要将它们分派到您的主游戏/应用程序（无论如何，始终将鼠标输入传递给 imgui）。例如，鼠标未点击但悬停在 imgui 窗口上，控件处于激活状态，鼠标点击了 imgui 窗口等。
    bool WantCaptureKeyboard;  // 当 Dear ImGui 使用键盘输入时设置，在这种情况下不要将键盘输入分派到您的主游戏/应用程序（无论如何，始终将键盘输入传递给 imgui）。例如，InputText 处于激活状态，或者一个 imgui 窗口聚焦并启用了导航等。
    bool WantTextInput;        // 移动设备/游戏主机：当设置时，您可能会显示一个屏幕键盘。当 Dear ImGui 需要文本键盘输入时设置此标志（例如，当 InputText 控件处于激活状态时）。
    bool WantSetMousePos;      // 鼠标位置已被更改，后端应在下帧重新定位鼠标。很少使用！仅在启用 io.ConfigNavMoveSetMousePos 时设置。
    bool WantSaveIniSettings;  // 当手动 .ini 加载/保存被激活（io.IniFilename == NULL）时，此标志将设置，以通知您的应用程序可以调用 SaveIniSettingsToMemory() 来保存设置。重要：保存后，请自己清除 io.WantSaveIniSettings！
    bool NavActive;            // 当前允许键盘/游戏手柄导航（会处理 ImGuiKey_NavXXX 事件）= 一个窗口被聚焦且没有使用 ImGuiWindowFlags_NoNavInputs 标志。
    bool NavVisible;           // 当前可见并允许键盘/游戏手柄导航高亮显示（会处理 ImGuiKey_NavXXX 事件）。
    float Framerate;           // 应用程序帧率的估计值（基于 io.DeltaTime 的 60 帧滚动平均值），以每秒帧数为单位。仅供方便参考。较慢的应用程序可能不希望使用滚动平均值，或者可能偶尔需要重置底层缓冲区。
    int MetricsRenderVertices; // 上次调用 Render() 时的顶点数
    int MetricsRenderIndices;  // 上次调用 Render() 时的索引数 = 三角形数 * 3
    int MetricsRenderWindows;  // 可见窗口的数量
    int MetricsActiveWindows;  // 活跃窗口的数量
    ImVec2 MouseDelta;         // 鼠标增量。请注意，如果当前或之前的位置无效（即 -FLT_MAX,-FLT_MAX），则此值为零，因此一个消失/重新出现的鼠标不会产生巨大的增量。

    //------------------------------------------------------------------
    // [内部] Dear ImGui 将维护这些字段。向后兼容性不保证！
    //------------------------------------------------------------------

    ImGuiContext *Ctx; // 父级 UI 上下文（需要由父级显式设置）。

    // 主要输入状态
    // （这个块以前是由后端写入的，从 1.87 开始，最好不要直接写入这些，改为调用上述的 AddXXX 函数）
    // （从这些变量中读取是合法的，因为它们非常不可能会移到其他地方）
    ImVec2 MousePos;              // 鼠标位置，单位像素。如果鼠标不可用（例如在另一个屏幕上），设置为 ImVec2(-FLT_MAX, -FLT_MAX)。
    bool MouseDown[5];            // 鼠标按钮：0=左键，1=右键，2=中键 + 额外按钮（ImGuiMouseButton_COUNT == 5）。Dear ImGui 主要使用左键和右键。其他按钮允许我们跟踪鼠标是否被应用程序使用，并通过 IsMouse** API 提供给用户。
    float MouseWheel;             // 鼠标滚轮垂直：1 单位滚动大约 5 行文本。>0 滚动向上，<0 滚动向下。按住 SHIFT 将垂直滚动转换为水平滚动。
    float MouseWheelH;            // 鼠标滚轮水平。>0 滚动向左，<0 滚动向右。大多数用户没有带水平滚轮的鼠标，可能不是所有后端都会填充此字段。
    ImGuiMouseSource MouseSource; // 鼠标实际输入外设（鼠标/触摸屏/笔）。
    ImGuiID MouseHoveredViewport; // （可选）通过 io.AddMouseViewportEvent() 修改。多视口情况下：操作系统鼠标悬停的视口。如果可能，最好忽略带有 ImGuiViewportFlags_NoInputs 标志的视口（很少有后端能够处理这种情况）。如果您能提供此信息，请设置 io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport。如果没有，imgui 将通过它所知道的视口的矩形和最后聚焦时间推断此值（忽略其他操作系统窗口）。
    bool KeyCtrl;                 // 键盘修饰符按下：Ctrl
    bool KeyShift;                // 键盘修饰符按下：Shift
    bool KeyAlt;                  // 键盘修饰符按下：Alt
    bool KeySuper;                // 键盘修饰符按下：Cmd/Super/Windows

    // 由上述数据和 IO 函数调用维护的其他状态
    ImGuiKeyChord KeyMods;                          // 键盘修饰符标志（任何 ImGuiMod_Ctrl/ImGuiMod_Shift/ImGuiMod_Alt/ImGuiMod_Super 标志，和 io.KeyCtrl/KeyShift/KeyAlt/KeySuper 相同，但合并为标志。只读，由 NewFrame() 更新）
    ImGuiKeyData KeysData[ImGuiKey_NamedKey_COUNT]; // 所有已知键的键状态。使用 IsKeyXXX() 函数访问此信息。
    bool WantCaptureMouseUnlessPopupClose;          // WantCaptureMouse 的替代：当点击空白区域时（期望关闭弹出窗口），当 WantCaptureMouse == true 且 WantCaptureMouseUnlessPopupClose == false 时
    ImVec2 MousePosPrev;                            // 上一鼠标位置（请注意，如果任何位置无效，则 MouseDelta 不等于 MousePos - MousePosPrev）
    ImVec2 MouseClickedPos[5];                      // 点击时的鼠标位置
    double MouseClickedTime[5];                     // 上次点击的时间（用于计算双击）
    bool MouseClicked[5];                           // 鼠标按钮从 !Down 变为 Down（与 MouseClickedCount[x] != 0 相同）
    bool MouseDoubleClicked[5];                     // 鼠标按钮是否为双击？（与 MouseClickedCount[x] == 2 相同）
    ImU16 MouseClickedCount[5];                     // == 0（未点击），== 1（与 MouseClicked[] 相同），== 2（双击），== 3（三击）等，表示从 !Down 到 Down
    ImU16 MouseClickedLastCount[5];                 // 计数连续点击的次数。鼠标释放后保持有效，另一次点击发生后重置
    bool MouseReleased[5];                          // 鼠标按钮从 Down 变为 !Down
    bool MouseDownOwned[5];                         // 跟踪按钮是否在 Dear ImGui 窗口内点击，或者点击了被弹出窗口阻挡的空白区域。如果点击开始在 ImGui 边界外，则不请求鼠标捕获。
    bool MouseDownOwnedUnlessPopupClose[5];         // 跟踪按钮是否在 Dear ImGui 窗口内点击
    bool MouseWheelRequestAxisSwap;                 // 在非 Mac 系统中，按住 SHIFT 请求 WheelY 执行 WheelX 事件的等效操作。在 Mac 系统中，这已由系统强制执行。
    bool MouseCtrlLeftAsRightClick;                 // (OSX) 如果当前点击为 ctrl-click，生成模拟右键单击，则设置为 true
    float MouseDownDuration[5];                     // 鼠标按钮按下的持续时间（0.0f == 刚点击）
    float MouseDownDurationPrev[5];                 // 鼠标按钮之前按下的时间
    ImVec2 MouseDragMaxDistanceAbs[5];              // 每个轴上，鼠标从点击点移动的最大距离，绝对值
    float MouseDragMaxDistanceSqr[5];               // 鼠标从点击点移动的最大距离的平方（用于移动阈值）
    float PenPressure;                              // 触摸/笔压感（0.0f 到 1.0f，仅当 MouseDown[0] == true 时应该大于 0.0f）。当前 Dear ImGui 未使用的辅助存储
    bool AppFocusLost;                              // 仅通过 AddFocusEvent() 修改
    bool AppAcceptingEvents;                        // 仅通过 SetAppAcceptingEvents() 修改
    ImWchar16 InputQueueSurrogate;                  // 用于 AddInputCharacterUTF16()
    ImVector<ImWchar> InputQueueCharacters;         // 字符输入队列（通过平台后端获取）。通过 AddInputCharacter() 辅助函数填充

    // Legacy: before 1.87, we required backend to fill io.KeyMap[] (imgui->native map) during initialization and io.KeysDown[] (native indices) every frame.
    // This is still temporarily supported as a legacy feature. However the new preferred scheme is for backend to call io.AddKeyEvent().
    //   Old (<1.87):  ImGui::IsKeyPressed(ImGui::GetIO().KeyMap[ImGuiKey_Space]) --> New (1.87+) ImGui::IsKeyPressed(ImGuiKey_Space)
    //   Old (<1.87):  ImGui::IsKeyPressed(MYPLATFORM_KEY_SPACE)                  --> New (1.87+) ImGui::IsKeyPressed(ImGuiKey_Space)
    // Read https://github.com/ocornut/imgui/issues/4921 for details.
    //int       KeyMap[ImGuiKey_COUNT];             // [LEGACY] Input: map of indices into the KeysDown[512] entries array which represent your "native" keyboard state. The first 512 are now unused and should be kept zero. Legacy backend will write into KeyMap[] using ImGuiKey_ indices which are always >512.
    //bool      KeysDown[ImGuiKey_COUNT];           // [LEGACY] Input: Keyboard keys that are pressed (ideally left in the "native" order your engine has access to keyboard keys, so you can use your own defines/enums for keys). This used to be [512] sized. It is now ImGuiKey_COUNT to allow legacy io.KeysDown[GetKeyIndex(...)] to work without an overflow.
    //float     NavInputs[ImGuiNavInput_COUNT];     // [LEGACY] Since 1.88, NavInputs[] was removed. Backends from 1.60 to 1.86 won't build. Feed gamepad inputs via io.AddKeyEvent() and ImGuiKey_GamepadXXX enums.
    //void*     ImeWindowHandle;                    // [Obsoleted in 1.87] Set ImGuiViewport::PlatformHandleRaw instead. Set this to your HWND to get automatic IME cursor positioning.

    // Legacy: before 1.91.1, clipboard functions were stored in ImGuiIO instead of ImGuiPlatformIO.
    // As this is will affect all users of custom engines/backends, we are providing proper legacy redirection (will obsolete).
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    const char* (*GetClipboardTextFn)(void* user_data);
    void        (*SetClipboardTextFn)(void* user_data, const char* text);
    void*       ClipboardUserData;
#endif

    IMGUI_API   ImGuiIO();
};

//-----------------------------------------------------------------------------
// [SECTION] Misc data structures (ImGuiInputTextCallbackData, ImGuiSizeCallbackData, ImGuiPayload)
//-----------------------------------------------------------------------------

// InputText() 的共享状态，当使用 ImGuiInputTextFlags_Callback* 标志时，作为回调的参数传递给你的回调函数。
// 默认情况下，回调函数应返回 0。
// 回调函数（请参阅标志名称，详细信息见 ImGuiInputTextFlags_ 声明）
// - ImGuiInputTextFlags_CallbackEdit:        当缓冲区编辑时回调（请注意，InputText() 在编辑时已经返回 true，回调函数主要用于在焦点处于活动状态时操作底层缓冲区）
// - ImGuiInputTextFlags_CallbackAlways:      每次迭代时回调
// - ImGuiInputTextFlags_CallbackCompletion:  按下 TAB 时回调
// - ImGuiInputTextFlags_CallbackHistory:     按下上/下箭头时回调
// - ImGuiInputTextFlags_CallbackCharFilter:  对字符输入进行回调，以替换或丢弃它们。修改 'EventChar' 来替换或丢弃，或者在回调中返回 1 来丢弃。
// - ImGuiInputTextFlags_CallbackResize:      当请求更改缓冲区大小时回调（超出 'buf_size' 参数值），允许字符串增长。
struct ImGuiInputTextCallbackData
{
    ImGuiContext *Ctx;             // 父级 UI 上下文
    ImGuiInputTextFlags EventFlag; // 一个 ImGuiInputTextFlags_Callback* 标志   // 只读
    ImGuiInputTextFlags Flags;     // 用户传递给 InputText() 的标志         // 只读
    void *UserData;                // 用户传递给 InputText() 的数据         // 只读

    // 不同回调事件的参数
    // - 在 Resize 回调期间，Buf 与输入缓冲区相同。
    // - 然而，在 Completion/History/Always 回调期间，Buf 总是指向我们自己的内部数据（与你的缓冲区不同）！对它的更改将在回调后短时间内反映到你的缓冲区。
    // - 要在回调中修改文本缓冲区，优先使用 InsertChars() / DeleteChars() 函数。如果需要，InsertChars() 会负责调用 Resize 回调。
    // - 如果你知道你的编辑不会调整底层缓冲区分配的大小，你可以直接修改 'Buf[]' 的内容。你需要相应地更新 'BufTextLen'（0 <= BufTextLen < BufSize），并将 'BufDirty' 设置为 true，以便 InputText 可以更新其内部状态。
    ImWchar EventChar;  // 字符输入                          // 可读写   // [CharFilter] 用另一个字符替换字符，或者设置为零丢弃。回调中返回 1 等同于将 EventChar 设置为 0；
    ImGuiKey EventKey;  // 按下的键（上/下/Tab）              // 只读    // [Completion,History]
    char *Buf;          // 文本缓冲区                          // 可读写   // [Resize] 可以替换指针 / [Completion,History,Always] 只写指向的数据，不要替换实际指针！
    int BufTextLen;     // 文本长度（字节数）                 // 可读写   // [Resize,Completion,History,Always] 不包括零终止符存储。在 C 中：== strlen(some_text)，在 C++ 中：string.length()
    int BufSize;        // 缓冲区大小（字节数）= 容量+1      // 只读    // [Resize,Completion,History,Always] 包括零终止符存储。在 C 中 == ARRAYSIZE(my_char_array)，在 C++ 中：string.capacity()+1
    bool BufDirty;      // 如果修改了 Buf/BufTextLen，则设置为 true   // 写        // [Completion,History,Always]
    int CursorPos;      // 当前光标位置                      // 可读写   // [Completion,History,Always]
    int SelectionStart; // 选区开始位置                      // 可读写   // [Completion,History,Always] 无选区时，等于 SelectionEnd
    int SelectionEnd;   // 选区结束位置                      // 可读写   // [Completion,History,Always]

    // 文本操作的辅助函数
    // 使用这些函数以受益于 CallbackResize 行为。调用这些函数将重置选择。
    IMGUI_API ImGuiInputTextCallbackData();
    IMGUI_API void      DeleteChars(int pos, int bytes_count);
    IMGUI_API void      InsertChars(int pos, const char* text, const char* text_end = NULL);
    void                SelectAll()             { SelectionStart = 0; SelectionEnd = BufTextLen; }
    void                ClearSelection()        { SelectionStart = SelectionEnd = BufTextLen; }
    bool                HasSelection() const    { return SelectionStart != SelectionEnd; }
};

// 调整大小回调数据，用于应用自定义约束。由 SetNextWindowSizeConstraints() 启用。回调将在下一次 Begin() 中调用。
// 注意：对于每个轴的基本最小/最大尺寸约束，您不需要使用回调！SetNextWindowSizeConstraints() 的参数已经足够。
struct ImGuiSizeCallbackData
{
    void *UserData;     // 只读。用户传递给 SetNextWindowSizeConstraints() 的数据。通常在此存储一个整数或浮动值（需要 reinterpret_cast<>）。
    ImVec2 Pos;         // 只读。窗口位置，作为参考。
    ImVec2 CurrentSize; // 只读。当前窗口大小。
    ImVec2 DesiredSize; // 可读写。期望的大小，基于用户的鼠标位置。写入此字段可以限制调整大小。
};

// [ALPHA] 仅供少数使用 / 高级用途。与 SetNextWindowClass() 和 DockSpace() 函数一起使用。
// 重要：此类的内容仍然是高度开发中的，可能会在我们稳定Docking功能之前发生变化和重构。
// 如果使用时请谨慎。
// 提供提示：
// - 通过更改视口标志（启用/禁用操作系统装饰、操作系统任务栏图标等）提供给平台后端。
// - 提供给平台后端以支持操作系统级别的视口父/子关系。
// - 提供给 Docking 系统用于各种选项和过滤。
struct ImGuiWindowClass
{
    ImGuiID ClassId;                               // 用户数据。0 = 默认类（无分类）。不同类的窗口不能彼此停靠。
    ImGuiID ParentViewportId;                      // 给平台后端的提示。-1：使用默认值。0：请求平台后端不对子视口进行父子关系绑定。!= 0：请求平台后端在平台窗口之间创建父子关系。
    ImGuiID FocusRouteParentWindowId;              // 用于快捷键焦点路由评估的父窗口ID。例如，来自父窗口的 Shortcut() 调用会在该窗口获得焦点时成功。
    ImGuiViewportFlags ViewportFlagsOverrideSet;   // 当此类窗口拥有视口时，设置的视口标志。可以用来强制操作系统装饰或任务栏图标，按窗口逐个覆盖默认值。
    ImGuiViewportFlags ViewportFlagsOverrideClear; // 当此类窗口拥有视口时，清除的视口标志。可以用来强制操作系统装饰或任务栏图标，按窗口逐个覆盖默认值。
    ImGuiTabItemFlags TabItemFlagsOverrideSet;     // [实验性] 当此类窗口提交到停靠节点选项卡条时，设置的选项卡项目标志。可以与 ImGuiTabItemFlags_Leading 或 ImGuiTabItemFlags_Trailing 一起使用。
    ImGuiDockNodeFlags DockNodeFlagsOverrideSet;   // [实验性] 当此类窗口由停靠节点托管时设置的停靠节点标志（它不必被选中！）
    bool DockingAlwaysTabBar;                      // 设置为 true，强制此类的单个浮动窗口始终具有自己的停靠节点（相当于设置全局 io.ConfigDockingAlwaysTabBar）
    bool DockingAllowUnclassed;                    // 设置为 true，允许此类窗口与无类窗口停靠/合并。 // FIXME-DOCK：是否应移动到 DockNodeFlags 覆盖？

    ImGuiWindowClass() { memset(this, 0, sizeof(*this)); ParentViewportId = (ImGuiID)-1; DockingAllowUnclassed = true; }
};

// 用于拖放操作的数据负载：AcceptDragDropPayload(), GetDragDropPayload()
struct ImGuiPayload
{
    // 成员
    void *Data;   // 数据（由 dear imgui 复制并拥有）
    int DataSize; // 数据大小

    // [内部]
    ImGuiID SourceId;       // 源项的 ID
    ImGuiID SourceParentId; // 源项父项的 ID（如果可用）
    int DataFrameCount;     // 数据时间戳
    char DataType[32 + 1];  // 数据类型标签（用户提供的短字符串，最大 32 个字符）
    bool Preview;           // 当 AcceptDragDropPayload() 被调用且鼠标悬停在目标项时设置（注意：处理重叠的拖放目标）
    bool Delivery;          // 当 AcceptDragDropPayload() 被调用且鼠标按钮在目标项上释放时设置

    ImGuiPayload()  { Clear(); }
    void Clear()    { SourceId = SourceParentId = 0; Data = NULL; DataSize = 0; memset(DataType, 0, sizeof(DataType)); DataFrameCount = -1; Preview = Delivery = false; }
    bool IsDataType(const char* type) const { return DataFrameCount != -1 && strcmp(type, DataType) == 0; }
    bool IsPreview() const                  { return Preview; }
    bool IsDelivery() const                 { return Delivery; }
};

//-----------------------------------------------------------------------------
// [SECTION] Helpers (ImGuiOnceUponAFrame, ImGuiTextFilter, ImGuiTextBuffer, ImGuiStorage, ImGuiListClipper, Math Operators, ImColor)
//-----------------------------------------------------------------------------

// Helper: Unicode defines
#define IM_UNICODE_CODEPOINT_INVALID 0xFFFD     // Invalid Unicode code point (standard value).
#ifdef IMGUI_USE_WCHAR32
#define IM_UNICODE_CODEPOINT_MAX     0x10FFFF   // Maximum Unicode code point supported by this build.
#else
#define IM_UNICODE_CODEPOINT_MAX     0xFFFF     // Maximum Unicode code point supported by this build.
#endif

// 帮助器：在每帧最多执行一次代码块。如果你想在深层嵌套的代码中快速创建一个 UI（每帧运行多次），这个工具非常方便。
// 用法：static ImGuiOnceUponAFrame oaf; if (oaf) ImGui::Text("This will be called only once per frame");
struct ImGuiOnceUponAFrame
{
    ImGuiOnceUponAFrame() { RefFrame = -1; }
    mutable int RefFrame;
    operator bool() const { int current_frame = ImGui::GetFrameCount(); if (RefFrame == current_frame) return false; RefFrame = current_frame; return true; }
};

// 帮助器：解析并应用文本过滤器。格式为 "aaaaa[,bbbb][,ccccc]"
struct ImGuiTextFilter
{
    IMGUI_API           ImGuiTextFilter(const char* default_filter = "");
    IMGUI_API bool Draw(const char *label = "Filter (inc,-exc)", float width = 0.0f); // 帮助器调用 InputText + Build
    IMGUI_API bool      PassFilter(const char* text, const char* text_end = NULL) const;
    IMGUI_API void      Build();
    void                Clear()          { InputBuf[0] = 0; Build(); }
    bool                IsActive() const { return !Filters.empty(); }

    // [内部]
    struct ImGuiTextRange
    {
        const char*     b;
        const char*     e;

        ImGuiTextRange()                                { b = e = NULL; }
        ImGuiTextRange(const char* _b, const char* _e)  { b = _b; e = _e; }
        bool            empty() const                   { return b == e; }
        IMGUI_API void  split(char separator, ImVector<ImGuiTextRange>* out) const;
    };
    char                    InputBuf[256];
    ImVector<ImGuiTextRange>Filters;
    int                     CountGrep;
};

// 帮助器：用于日志记录/累积文本的可增长文本缓冲区
// （这可以称为 'ImGuiTextBuilder' 或 'ImGuiStringBuilder'）
struct ImGuiTextBuffer
{
    ImVector<char>      Buf;
    IMGUI_API static char EmptyString[1];

    ImGuiTextBuffer()   { }
    inline char         operator[](int i) const { IM_ASSERT(Buf.Data != NULL); return Buf.Data[i]; }
    const char*         begin() const           { return Buf.Data ? &Buf.front() : EmptyString; }
    const char *end() const
    {
        return Buf.Data ? &Buf.back() : EmptyString;
    } // Buf 是零终止的，因此 end() 将指向零终止符
    int                 size() const            { return Buf.Size ? Buf.Size - 1 : 0; }
    bool                empty() const           { return Buf.Size <= 1; }
    void                clear()                 { Buf.clear(); }
    void                reserve(int capacity)   { Buf.reserve(capacity); }
    const char*         c_str() const           { return Buf.Data ? Buf.Data : EmptyString; }
    IMGUI_API void      append(const char* str, const char* str_end = NULL);
    IMGUI_API void      appendf(const char* fmt, ...) IM_FMTARGS(2);
    IMGUI_API void      appendfv(const char* fmt, va_list args) IM_FMTLIST(2);
};

// [内部] ImGuiStorage 的键值对
struct ImGuiStoragePair
{
    ImGuiID     key;
    union       { int val_i; float val_f; void* val_p; };

    ImGuiStoragePair(ImGuiID _key, int _val)    { key = _key; val_i = _val; }
    ImGuiStoragePair(ImGuiID _key, float _val)  { key = _key; val_f = _val; }
    ImGuiStoragePair(ImGuiID _key, void* _val)  { key = _key; val_p = _val; }
};

// 帮助：键值存储
// 通常您不需要担心此问题，因为每个窗口内都有一个存储。
// 我们用它来存储例如树的折叠状态（整数 0/1）
// 这经过优化，旨在高效查找（二分查找进入连续缓冲区）和罕见插入（通常与用户交互有关，即每帧最多一次）
// 您可以将其用作自定义用户存储，存储临时值。例如，声明您自己的存储，如果您想：
// - 操作接口中某个子树的打开/关闭状态（树节点使用整数 0/1 来存储其状态）。
// - 轻松存储自定义调试数据而无需在代码中添加或编辑结构（可能效率较低，但方便）
// 类型未存储，因此由您确保键不会与不同类型发生冲突。
struct ImGuiStorage
{
    // [内部]
    ImVector<ImGuiStoragePair>      Data;

    // - Get***() 函数查找配对，不会添加/分配。配对已排序，因此查询是 O(log N)
    // - Set***() 函数查找配对，如果缺少则按需插入。
    // - 排序插入开销较大，仅付出一次。一个典型帧不应该需要插入任何新配对。
    void                Clear() { Data.clear(); }
    IMGUI_API int       GetInt(ImGuiID key, int default_val = 0) const;
    IMGUI_API void      SetInt(ImGuiID key, int val);
    IMGUI_API bool      GetBool(ImGuiID key, bool default_val = false) const;
    IMGUI_API void      SetBool(ImGuiID key, bool val);
    IMGUI_API float     GetFloat(ImGuiID key, float default_val = 0.0f) const;
    IMGUI_API void      SetFloat(ImGuiID key, float val);
    IMGUI_API void *GetVoidPtr(ImGuiID key) const; // default_val 为 NULL
    IMGUI_API void      SetVoidPtr(ImGuiID key, void* val);

    // - Get***Ref() 函数查找配对，按需插入，返回指针。适用于您打算进行 Get+Set 的情况。
    // - 引用仅在存储中添加新值之前有效。调用 Set***() 函数或 Get***Ref() 函数会使指针失效。
    // - 这种用法的典型案例是快速调试（例如，如果无法修改现有结构，可以在实时编辑和继续会话期间添加存储）
    //      float* pvar = ImGui::GetFloatRef(key); ImGui::SliderFloat("var", pvar, 0, 100.0f); some_var += *pvar;
    IMGUI_API int*      GetIntRef(ImGuiID key, int default_val = 0);
    IMGUI_API bool*     GetBoolRef(ImGuiID key, bool default_val = false);
    IMGUI_API float*    GetFloatRef(ImGuiID key, float default_val = 0.0f);
    IMGUI_API void**    GetVoidPtrRef(ImGuiID key, void* default_val = NULL);

    // 高级：用于更快的完整重建存储（而不是增量重建），您可以先添加所有内容，然后排序一次。
    IMGUI_API void      BuildSortByKey();
    // 已废弃：如果您只知道存储的是整数（如打开/关闭所有树节点），请在自己的存储中使用。
    IMGUI_API void      SetAllInt(int val);

#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    // typedef ::ImGuiStoragePair ImGuiStoragePair;  // 1.90.8：将类型移出结构体
#endif
};

// 帮助：手动裁剪大型项列表。
// 如果您有许多间隔均匀的项并且可以随机访问该列表，则可以基于可见性执行粗略裁剪，
// 以仅提交视图中的项。
// 裁剪器计算可见项的范围，并通过跳过不可见项来前进游标。
// （亲爱的 ImGui 已经根据项的边界裁剪项，但：它需要首先布局项才能这样做，并且通常
//  获取/提交您自己的数据会带来额外的成本。使用 ImGuiListClipper 的粗略裁剪允许您轻松地
//  扩展到包含成千上万项的列表，而不会有问题）
// 用法：
//   ImGuiListClipper clipper;
//   clipper.Begin(1000);         // 我们有 1000 个项，均匀分布。
//   while (clipper.Step())
//       for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
//           ImGui::Text("line number %d", i);
// 通常发生的情况是：
// - 裁剪器让您处理第一个元素（DisplayStart = 0, DisplayEnd = 1），无论它是否可见。
// - 用户代码提交该元素。
// - 裁剪器可以测量第一个元素的高度。
// - 裁剪器根据当前的裁剪矩形计算要显示的实际元素范围，并将游标定位到第一个可见元素之前。
// - 用户代码提交可见元素。
// - 裁剪器还处理与键盘/游戏手柄导航、换行等相关的各种细节。

struct ImGuiListClipper
{
    ImGuiContext *Ctx;       // 父级 UI 上下文
    int DisplayStart;        // 要显示的第一个项，每次调用 Step() 时更新
    int DisplayEnd;          // 要显示的最后一个项（不包括）
    int ItemsCount;          // [内部] 项目数量
    float ItemsHeight;       // [内部] 项目高度，在第一次步骤后提交项时可以计算
    float StartPosY;         // [内部] 在 Begin() 时或表格冻结行都已处理后的光标位置
    double StartSeekOffsetY; // [内部] 考虑表格中的冻结行和在非常大的窗口中的初始精度丢失
    void *TempData;          // [内部] 内部数据

    // items_count: 如果您不知道有多少项，请使用 INT_MAX（在这种情况下，游标不会在最后一步中前进，您可以在需要时手动调用 SeekCursorForItem()）
    // items_height: 如果想自动计算，请使用 -1.0f；否则传入项目之间的间距，通常是 GetTextLineHeightWithSpacing() 或 GetFrameHeightWithSpacing()。
    IMGUI_API ImGuiListClipper();
    IMGUI_API ~ImGuiListClipper();
    IMGUI_API void  Begin(int items_count, float items_height = -1.0f);
    IMGUI_API void End();  // 在返回 false 的最后一次调用 Step() 时自动调用
    IMGUI_API bool Step(); // 调用直到返回 false。DisplayStart/DisplayEnd 字段将被设置，您可以处理/绘制这些项。

    // 如果您需要一系列项目不被裁剪，不管它们是否可见，请在第一次调用 Step() 之前调用 IncludeItemByIndex() 或 IncludeItemsByIndex()。
    // （由于某些项的对齐/填充，可能会在显示范围的两端包括额外的项）
    inline void     IncludeItemByIndex(int item_index)                  { IncludeItemsByIndex(item_index, item_index + 1); }
    IMGUI_API void IncludeItemsByIndex(int item_begin, int item_end); // item_end 是不包括的，例如使用 (42, 42+1) 来确保项 42 永远不被裁剪。

    // 将光标寻址到给定项。这在步骤过程中会自动调用。
    // - 唯一需要调用此方法的情况是：如果您事先不知道项的数量，您可以使用 ImGuiListClipper::Begin(INT_MAX)。
    // - 在这种情况下，所有步骤完成后，您将需要调用 SeekCursorForItem(item_count)。
    IMGUI_API void  SeekCursorForItem(int item_index);

#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    inline void IncludeRangeByIndices(int item_begin, int item_end)
    {
        IncludeItemsByIndex(item_begin, item_end);
    } // [在 1.89.9 中重命名]
    inline void ForceDisplayRangeByIndices(int item_begin, int item_end)
    {
        IncludeItemsByIndex(item_begin, item_end);
    } // [在 1.89.6 中重命名]
    // inline ImGuiListClipper(int items_count, float items_height = -1.0f) { memset(this, 0, sizeof(*this)); ItemsCount = -1; Begin(items_count, items_height); } // [在 1.79 中移除]
#endif
};

// Helpers: ImVec2/ImVec4 operators
// - It is important that we are keeping those disabled by default so they don't leak in user space.
// - This is in order to allow user enabling implicit cast operators between ImVec2/ImVec4 and their own types (using IM_VEC2_CLASS_EXTRA in imconfig.h)
// - Add '#define IMGUI_DEFINE_MATH_OPERATORS' before including this file (or in imconfig.h) to access courtesy maths operators for ImVec2 and ImVec4.
// - We intentionally provide ImVec2*float but not float*ImVec2: this is rare enough and we want to reduce the surface for possible user mistake.
#ifdef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS_IMPLEMENTED
IM_MSVC_RUNTIME_CHECKS_OFF
static inline ImVec2  operator*(const ImVec2& lhs, const float rhs)     { return ImVec2(lhs.x * rhs, lhs.y * rhs); }
static inline ImVec2  operator/(const ImVec2& lhs, const float rhs)     { return ImVec2(lhs.x / rhs, lhs.y / rhs); }
static inline ImVec2  operator+(const ImVec2& lhs, const ImVec2& rhs)   { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2  operator-(const ImVec2& lhs, const ImVec2& rhs)   { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
static inline ImVec2  operator*(const ImVec2& lhs, const ImVec2& rhs)   { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
static inline ImVec2  operator/(const ImVec2& lhs, const ImVec2& rhs)   { return ImVec2(lhs.x / rhs.x, lhs.y / rhs.y); }
static inline ImVec2  operator-(const ImVec2& lhs)                      { return ImVec2(-lhs.x, -lhs.y); }
static inline ImVec2& operator*=(ImVec2& lhs, const float rhs)          { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const float rhs)          { lhs.x /= rhs; lhs.y /= rhs; return lhs; }
static inline ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs)        { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
static inline ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs)        { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
static inline ImVec2& operator*=(ImVec2& lhs, const ImVec2& rhs)        { lhs.x *= rhs.x; lhs.y *= rhs.y; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const ImVec2& rhs)        { lhs.x /= rhs.x; lhs.y /= rhs.y; return lhs; }
static inline bool    operator==(const ImVec2& lhs, const ImVec2& rhs)  { return lhs.x == rhs.x && lhs.y == rhs.y; }
static inline bool    operator!=(const ImVec2& lhs, const ImVec2& rhs)  { return lhs.x != rhs.x || lhs.y != rhs.y; }
static inline ImVec4  operator+(const ImVec4& lhs, const ImVec4& rhs)   { return ImVec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
static inline ImVec4  operator-(const ImVec4& lhs, const ImVec4& rhs)   { return ImVec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
static inline ImVec4  operator*(const ImVec4& lhs, const ImVec4& rhs)   { return ImVec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }
static inline bool    operator==(const ImVec4& lhs, const ImVec4& rhs)  { return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w; }
static inline bool    operator!=(const ImVec4& lhs, const ImVec4& rhs)  { return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z || lhs.w != rhs.w; }
IM_MSVC_RUNTIME_CHECKS_RESTORE
#endif

// Helpers macros to generate 32-bit encoded colors
// - User can declare their own format by #defining the 5 _SHIFT/_MASK macros in their imconfig file.
// - Any setting other than the default will need custom backend support. The only standard backend that supports anything else than the default is DirectX9.
#ifndef IM_COL32_R_SHIFT
#ifdef IMGUI_USE_BGRA_PACKED_COLOR
#define IM_COL32_R_SHIFT    16
#define IM_COL32_G_SHIFT    8
#define IM_COL32_B_SHIFT    0
#define IM_COL32_A_SHIFT    24
#define IM_COL32_A_MASK     0xFF000000
#else
#define IM_COL32_R_SHIFT    0
#define IM_COL32_G_SHIFT    8
#define IM_COL32_B_SHIFT    16
#define IM_COL32_A_SHIFT    24
#define IM_COL32_A_MASK     0xFF000000
#endif
#endif
#define IM_COL32(R,G,B,A)    (((ImU32)(A)<<IM_COL32_A_SHIFT) | ((ImU32)(B)<<IM_COL32_B_SHIFT) | ((ImU32)(G)<<IM_COL32_G_SHIFT) | ((ImU32)(R)<<IM_COL32_R_SHIFT))
#define IM_COL32_WHITE       IM_COL32(255,255,255,255)  // Opaque white = 0xFFFFFFFF
#define IM_COL32_BLACK       IM_COL32(0,0,0,255)        // Opaque black
#define IM_COL32_BLACK_TRANS IM_COL32(0,0,0,0)          // Transparent black = 0x00000000

// 帮助器：ImColor() 隐式地将颜色转换为 ImU32（打包的 4x1 字节）或 ImVec4（4x1 浮动）
// 如果你希望保证编译时得到 ImU32 用于 ImDrawList API，建议使用 IM_COL32() 宏。
// **避免存储 ImColor！存储 ImU32 或 ImVec4。它不是一个全功能的颜色类。可能会废弃。
// **ImGui 的 API 不直接使用 ImColor，但你可以将其作为方便的方法来传递颜色，可以使用 ImU32 或 ImVec4 格式。需要时可以显式地转换为 ImU32 或 ImVec4。
struct ImColor
{
    ImVec4          Value;

    constexpr ImColor()                                             { }
    constexpr ImColor(float r, float g, float b, float a = 1.0f)    : Value(r, g, b, a) { }
    constexpr ImColor(const ImVec4& col)                            : Value(col) {}
    constexpr ImColor(int r, int g, int b, int a = 255)             : Value((float)r * (1.0f / 255.0f), (float)g * (1.0f / 255.0f), (float)b * (1.0f / 255.0f), (float)a* (1.0f / 255.0f)) {}
    constexpr ImColor(ImU32 rgba)                                   : Value((float)((rgba >> IM_COL32_R_SHIFT) & 0xFF) * (1.0f / 255.0f), (float)((rgba >> IM_COL32_G_SHIFT) & 0xFF) * (1.0f / 255.0f), (float)((rgba >> IM_COL32_B_SHIFT) & 0xFF) * (1.0f / 255.0f), (float)((rgba >> IM_COL32_A_SHIFT) & 0xFF) * (1.0f / 255.0f)) {}
    inline operator ImU32() const                                   { return ImGui::ColorConvertFloat4ToU32(Value); }
    inline operator ImVec4() const                                  { return Value; }

    // FIXME-OBSOLETE: 可能需要废弃/清理这些帮助器。
    inline void    SetHSV(float h, float s, float v, float a = 1.0f){ ImGui::ColorConvertHSVtoRGB(h, s, v, Value.x, Value.y, Value.z); Value.w = a; }
    static ImColor HSV(float h, float s, float v, float a = 1.0f)   { float r, g, b; ImGui::ColorConvertHSVtoRGB(h, s, v, r, g, b); return ImColor(r, g, b, a); }
};

//-----------------------------------------------------------------------------
// [SECTION] Multi-Select API flags and structures (ImGuiMultiSelectFlags, ImGuiSelectionRequestType, ImGuiSelectionRequest, ImGuiMultiSelectIO, ImGuiSelectionBasicStorage)
//-----------------------------------------------------------------------------

// Multi-selection system
// Documentation at: https://github.com/ocornut/imgui/wiki/Multi-Select
// - Refer to 'Demo->Widgets->Selection State & Multi-Select' for demos using this.
// - This system implements standard multi-selection idioms (CTRL+Mouse/Keyboard, SHIFT+Mouse/Keyboard, etc)
//   with support for clipper (skipping non-visible items), box-select and many other details.
// - Selectable(), Checkbox() are supported but custom widgets may use it as well.
// - TreeNode() is technically supported but... using this correctly is more complicated: you need some sort of linear/random access to your tree,
//   which is suited to advanced trees setups also implementing filters and clipper. We will work toward simplifying and demoing it.
// - In the spirit of Dear ImGui design, your code owns actual selection data.
//   This is designed to allow all kinds of selection storage you may use in your application e.g. set/map/hash.
// About ImGuiSelectionBasicStorage:
// - This is an optional helper to store a selection state and apply selection requests.
// - It is used by our demos and provided as a convenience to quickly implement multi-selection.
// Usage:
// - Identify submitted items with SetNextItemSelectionUserData(), most likely using an index into your current data-set.
// - Store and maintain actual selection data using persistent object identifiers.
// - Usage flow:
//     BEGIN - (1) Call BeginMultiSelect() and retrieve the ImGuiMultiSelectIO* result.
//           - (2) Honor request list (SetAll/SetRange requests) by updating your selection data. Same code as Step 6.
//           - (3) [If using clipper] You need to make sure RangeSrcItem is always submitted. Calculate its index and pass to clipper.IncludeItemByIndex(). If storing indices in ImGuiSelectionUserData, a simple clipper.IncludeItemByIndex(ms_io->RangeSrcItem) call will work.
//     LOOP  - (4) Submit your items with SetNextItemSelectionUserData() + Selectable()/TreeNode() calls.
//     END   - (5) Call EndMultiSelect() and retrieve the ImGuiMultiSelectIO* result.
//           - (6) Honor request list (SetAll/SetRange requests) by updating your selection data. Same code as Step 2.
//     If you submit all items (no clipper), Step 2 and 3 are optional and will be handled by each item themselves. It is fine to always honor those steps.
// About ImGuiSelectionUserData:
// - This can store an application-defined identifier (e.g. index or pointer) submitted via SetNextItemSelectionUserData().
// - In return we store them into RangeSrcItem/RangeFirstItem/RangeLastItem and other fields in ImGuiMultiSelectIO.
// - Most applications will store an object INDEX, hence the chosen name and type. Storing an index is natural, because
//   SetRange requests will give you two end-points and you will need to iterate/interpolate between them to update your selection.
// - However it is perfectly possible to store a POINTER or another IDENTIFIER inside ImGuiSelectionUserData.
//   Our system never assume that you identify items by indices, it never attempts to interpolate between two values.
// - If you enable ImGuiMultiSelectFlags_NoRangeSelect then it is guaranteed that you will never have to interpolate
//   between two ImGuiSelectionUserData, which may be a convenient way to use part of the feature with less code work.
// - As most users will want to store an index, for convenience and to reduce confusion we use ImS64 instead of void*,
//   being syntactically easier to downcast. Feel free to reinterpret_cast and store a pointer inside.

// BeginMultiSelect() 的标志
enum ImGuiMultiSelectFlags_
{
    ImGuiMultiSelectFlags_None = 0,                       // 无标志
    ImGuiMultiSelectFlags_SingleSelect = 1 << 0,          // 禁用选择多个项目。此选项可用于共享单选代码/逻辑，如果需要的话。尽管它基本上禁用了 BeginMultiSelect() 的主要功能！
    ImGuiMultiSelectFlags_NoSelectAll = 1 << 1,           // 禁用 CTRL+A 快捷键选择所有。
    ImGuiMultiSelectFlags_NoRangeSelect = 1 << 2,         // 禁用 Shift+鼠标/键盘选择支持（对于无序的二维选择很有用）。与 BoxSelect 一起使用时，也确保不会将连续的 SetRange 请求合并为一个。这允许在 SetRange 请求中不处理插值。
    ImGuiMultiSelectFlags_NoAutoSelect = 1 << 3,          // 禁用在导航时自动选择项目（例如，支持在复选框列表中进行范围选择时很有用）。
    ImGuiMultiSelectFlags_NoAutoClear = 1 << 4,           // 禁用在导航或选择另一个项目时清除选择（通常与 ImGuiMultiSelectFlags_NoAutoSelect 一起使用。例如，支持在复选框列表中进行范围选择时很有用）。
    ImGuiMultiSelectFlags_NoAutoClearOnReselect = 1 << 5, // 禁用在点击/选择已选择的项目时清除选择。
    ImGuiMultiSelectFlags_BoxSelect1d = 1 << 6,           // 启用框选择，适用于宽度相同且 x 位置相同的项目（例如，完整的行 Selectable()）。框选择在项目的点击框之间有一点间距时效果更好，这样可以瞄准空白区域。
    ImGuiMultiSelectFlags_BoxSelect2d = 1 << 7,           // 启用框选择，支持不同宽度或不同 x 位置的项目（例如，不同宽度的标签或二维布局/网格）。这会更慢：修改裁剪逻辑，例如，水平移动会更新通常被裁剪的项目的选择。
    ImGuiMultiSelectFlags_BoxSelectNoScroll = 1 << 8,     // 禁用在框选择时，靠近范围边缘时滚动。
    ImGuiMultiSelectFlags_ClearOnEscape = 1 << 9,         // 按 Escape 键时清除选择（当范围已聚焦时）。
    ImGuiMultiSelectFlags_ClearOnClickVoid = 1 << 10,     // 在范围内点击空白位置时清除选择。
    ImGuiMultiSelectFlags_ScopeWindow = 1 << 11,          // _BoxSelect 和 _ClearOnClickVoid 的作用范围是整个窗口（默认）。如果 BeginMultiSelect() 覆盖整个窗口或在同一窗口中仅使用一次，请使用此选项。
    ImGuiMultiSelectFlags_ScopeRect = 1 << 12,            // _BoxSelect 和 _ClearOnClickVoid 的作用范围是包含 BeginMultiSelect()/EndMultiSelect() 的矩形。若 BeginMultiSelect() 在同一窗口中调用多次，请使用此选项。
    ImGuiMultiSelectFlags_SelectOnClick = 1 << 13,        // 在点击未选择的项目时，鼠标按下时应用选择。（默认）
    ImGuiMultiSelectFlags_SelectOnClickRelease = 1 << 14, // 在点击未选择的项目时，鼠标释放时应用选择。允许拖动未选择的项目而不改变选择。
    // ImGuiMultiSelectFlags_RangeSelect2d       = 1 << 15,  // Shift+选择使用二维几何形状而非线性序列，因此可以使用 Shift+上下选择网格中的项目。类似于 BoxSelect 的功能。
    ImGuiMultiSelectFlags_NavWrapX = 1 << 16, // [临时] 启用 X 轴导航循环。提供此功能作为方便，因为我们尚未设计此功能的通用导航 API。当更通用的功能公开时，我们可能会废弃此标志，转而使用新的标志。
};

// BeginMultiSelect()/EndMultiSelect() 返回的主 IO 结构。
// 主要包含一系列选择请求。
// - 使用 'Demo->Tools->Debug Log->Selection' 来查看请求的执行情况。
// - 一些字段仅在您的列表是动态并允许删除时才有用（删除后获取焦点/状态的正确处理在演示中展示）
// - 以下：每个字段的读取/写入者。'r'=读取，'w'=写入，'ms'=多选代码，'app'=应用程序/用户代码。
struct ImGuiMultiSelectIO
{
    //------------------------------------------// BeginMultiSelect / EndMultiSelect
    ImVector<ImGuiSelectionRequest> Requests; //  ms:w, app:r     /  ms:w  app:r   // 要应用到您的选择数据的请求。
    ImGuiSelectionUserData RangeSrcItem;      //  ms:w  app:r     /                // （如果使用剪辑器）Begin：源项目（通常是第一个选中的项目）绝不应被剪辑：使用 clipper.IncludeItemByIndex() 来确保它被提交。
    ImGuiSelectionUserData NavIdItem;         //  ms:w, app:r     /                // （如果使用删除）NavId 的最后已知 SetNextItemSelectionUserData() 值（如果是已提交项目的一部分）。
    bool NavIdSelected;                       //  ms:w, app:r     /        app:r   // （如果使用删除）NavId 的最后已知选择状态（如果是已提交项目的一部分）。
    bool RangeSrcReset;                       //        app:w     /  ms:r          // （如果使用删除）在 EndMultiSelect() 前设置以重置 ResetSrcItem（例如，如果删除了选择）。
    int ItemsCount;                           //  ms:w, app:r     /        app:r   // 'int items_count' 参数被复制到这里以便于处理，使您的 ApplyRequests 处理程序调用更简便。内部未使用。
};

// 选择请求类型
enum ImGuiSelectionRequestType
{
    ImGuiSelectionRequestType_None = 0,
    ImGuiSelectionRequestType_SetAll,   // 请求应用程序清除选择（如果 Selected==false）或选择所有项目（如果 Selected==true）。我们不能设置 RangeFirstItem/RangeLastItem，因为其内容完全由用户决定（不一定是索引）
    ImGuiSelectionRequestType_SetRange, // 请求应用程序选择/取消选择 [RangeFirstItem..RangeLastItem] 项（包含在内），基于 Selected 的值。只有 EndMultiSelect() 会请求此操作，应用程序代码可以在 BeginMultiSelect() 后读取，并且它将始终为 false。
};

// 选择请求项
struct ImGuiSelectionRequest
{
    //------------------------------------------// BeginMultiSelect / EndMultiSelect
    ImGuiSelectionRequestType Type;        //  ms:w, app:r     /  ms:w, app:r   // 请求类型。通常你会收到 1 个 Clear 和 1 个 SetRange 请求，范围为单个项目。
    bool Selected;                         //  ms:w, app:r     /  ms:w, app:r   // SetAll/SetRange 请求的参数（true = 选择，false = 取消选择）
    ImS8 RangeDirection;                   //                  /  ms:w  app:r   // SetRange 请求的参数：当 RangeFirstItem 在 RangeLastItem 之前时为 +1，否则为 -1。如果你想在反向 Shift+Click 时保持选择顺序，这个参数很有用。
    ImGuiSelectionUserData RangeFirstItem; //                  /  ms:w, app:r   // SetRange 请求的参数（当从上到下 Shift 选择时，这通常等于 RangeSrcItem）。
    ImGuiSelectionUserData RangeLastItem;  //                  /  ms:w, app:r   // SetRange 请求的参数（当从下到上 Shift 选择时，这通常等于 RangeSrcItem）。包含在内！
};

// Optional helper to store multi-selection state + apply multi-selection requests.
// - Used by our demos and provided as a convenience to easily implement basic multi-selection.
// - Iterate selection with 'void* it = NULL; ImGuiID id; while (selection.GetNextSelectedItem(&it, &id)) { ... }'
//   Or you can check 'if (Contains(id)) { ... }' for each possible object if their number is not too high to iterate.
// - USING THIS IS NOT MANDATORY. This is only a helper and not a required API.
// To store a multi-selection, in your application you could:
// - Use this helper as a convenience. We use our simple key->value ImGuiStorage as a std::set<ImGuiID> replacement.
// - Use your own external storage: e.g. std::set<MyObjectId>, std::vector<MyObjectId>, interval trees, intrusively stored selection etc.
// In ImGuiSelectionBasicStorage we:
// - always use indices in the multi-selection API (passed to SetNextItemSelectionUserData(), retrieved in ImGuiMultiSelectIO)
// - use the AdapterIndexToStorageId() indirection layer to abstract how persistent selection data is derived from an index.
// - use decently optimized logic to allow queries and insertion of very large selection sets.
// - do not preserve selection order.
// Many combinations are possible depending on how you prefer to store your items and how you prefer to store your selection.
// Large applications are likely to eventually want to get rid of this indirection layer and do their own thing.
// See https://github.com/ocornut/imgui/wiki/Multi-Select for details and pseudo-code using this helper.
struct ImGuiSelectionBasicStorage
{
    // 成员
    int Size;                                                                      //          // 选中的项目数，由此助手维护。
    bool PreserveOrder;                                                            // = false  // GetNextSelectedItem() 将返回有序的选择（目前通过两次额外排序选择实现。可以改进）。
    void *UserData;                                                                // = NULL   // 用户数据，供适配器函数使用        // 例如：selection.UserData = (void*)my_items;
    ImGuiID (*AdapterIndexToStorageId)(ImGuiSelectionBasicStorage *self, int idx); // 例如：selection.AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage* self, int idx) { return ((MyItems**)self->UserData)[idx]->ID; };
    int _SelectionOrder;                                                           // [内部] 增加计数器来存储选择顺序
    ImGuiStorage _Storage;                                                         // [内部] 选择集合。类似于例如 std::set<ImGuiID>。建议不要直接访问：通过 GetNextSelectedItem() 迭代。

    // 方法
    IMGUI_API ImGuiSelectionBasicStorage();
    IMGUI_API void ApplyRequests(ImGuiMultiSelectIO *ms_io);               // 应用来自 BeginMultiSelect() 和 EndMultiSelect() 函数的选择请求。它使用传递给 BeginMultiSelect() 的 'items_count'。
    IMGUI_API bool Contains(ImGuiID id) const;                             // 查询项目 id 是否在选择中。
    IMGUI_API void Clear();                                                // 清除选择
    IMGUI_API void Swap(ImGuiSelectionBasicStorage &r);                    // 交换两个选择
    IMGUI_API void SetItemSelected(ImGuiID id, bool selected);             // 将一个项目添加到/从选择中移除（通常由 ApplyRequests() 函数完成）
    IMGUI_API bool GetNextSelectedItem(void **opaque_it, ImGuiID *out_id); // 通过 'void* it = NULL; ImGuiID id; while (selection.GetNextSelectedItem(&it, &id)) { ... }' 迭代选择。
    inline ImGuiID GetStorageIdFromIndex(int idx)
    {
        return AdapterIndexToStorageId(this, idx);
    } // 基于提供的适配器，将索引转换为项目 id。
};

// 可选的辅助函数，用于将多选请求应用于现有的可随机访问存储。
// 如果你想快速将多选 API 接入到例如存储自己选择状态的 bool 数组或项目中，这是很方便的。
struct ImGuiSelectionExternalStorage
{
    // 成员
    void *UserData;                                                                              // 用户数据，供适配器函数使用                                // 例如：selection.UserData = (void*)my_items;
    void (*AdapterSetItemSelected)(ImGuiSelectionExternalStorage *self, int idx, bool selected); // 例如：AdapterSetItemSelected = [](ImGuiSelectionExternalStorage* self, int idx, bool selected) { ((MyItems**)self->UserData)[idx]->Selected = selected; }

    // 方法
    IMGUI_API ImGuiSelectionExternalStorage();
    IMGUI_API void ApplyRequests(ImGuiMultiSelectIO *ms_io); // 通过使用 AdapterSetItemSelected() 调用应用选择请求
};

//-----------------------------------------------------------------------------
// [SECTION] Drawing API (ImDrawCmd, ImDrawIdx, ImDrawVert, ImDrawChannel, ImDrawListSplitter, ImDrawListFlags, ImDrawList, ImDrawData)
// Hold a series of drawing commands. The user provides a renderer for ImDrawData which essentially contains an array of ImDrawList.
//-----------------------------------------------------------------------------

// The maximum line width to bake anti-aliased textures for. Build atlas with ImFontAtlasFlags_NoBakedLines to disable baking.
#ifndef IM_DRAWLIST_TEX_LINES_WIDTH_MAX
#define IM_DRAWLIST_TEX_LINES_WIDTH_MAX     (63)
#endif

// ImDrawCallback: Draw callbacks for advanced uses [configurable type: override in imconfig.h]
// NB: You most likely do NOT need to use draw callbacks just to create your own widget or customized UI rendering,
// you can poke into the draw list for that! Draw callback may be useful for example to:
//  A) Change your GPU render state,
//  B) render a complex 3D scene inside a UI element without an intermediate texture/render target, etc.
// The expected behavior from your rendering function is 'if (cmd.UserCallback != NULL) { cmd.UserCallback(parent_list, cmd); } else { RenderTriangles() }'
// If you want to override the signature of ImDrawCallback, you can simply use e.g. '#define ImDrawCallback MyDrawCallback' (in imconfig.h) + update rendering backend accordingly.
#ifndef ImDrawCallback
typedef void (*ImDrawCallback)(const ImDrawList* parent_list, const ImDrawCmd* cmd);
#endif

// Special Draw callback value to request renderer backend to reset the graphics/render state.
// The renderer backend needs to handle this special value, otherwise it will crash trying to call a function at this address.
// This is useful, for example, if you submitted callbacks which you know have altered the render state and you want it to be restored.
// Render state is not reset by default because they are many perfectly useful way of altering render state (e.g. changing shader/blending settings before an Image call).
#define ImDrawCallback_ResetRenderState     (ImDrawCallback)(-8)

// 通常，1 个命令 = 1 次 GPU 绘制调用（除非命令是回调）
// - VtxOffset：当启用 'io.BackendFlags & ImGuiBackendFlags_RendererHasVtxOffset' 时，
//   此字段允许我们渲染超过 64K 顶点的网格，同时保持 16 位索引。
//   适配器通常会忽略 VtxOffset 字段（适配器为 <1.71 的版本）
// - ClipRect/TextureId/VtxOffset 字段必须是连续的，因为我们会将它们一起进行 memcmp()（对此进行断言）。
struct ImDrawCmd
{
    ImVec4 ClipRect;             // 4*4  // 裁剪矩形（x1，y1，x2，y2）。减去 ImDrawData->DisplayPos 获取 "视口" 坐标系中的裁剪矩形
    ImTextureID TextureId;       // 4-8  // 用户提供的纹理 ID。在 ImfontAtlas::SetTexID() 中为字体设置，或传递给 Image*() 函数。若从未使用图像或多个字体图集则忽略。
    unsigned int VtxOffset;      // 4    // 顶点缓冲区的起始偏移。ImGuiBackendFlags_RendererHasVtxOffset：始终为 0，否则可能大于 0，用于支持大于 64K 顶点的网格，使用 16 位索引。
    unsigned int IdxOffset;      // 4    // 索引缓冲区的起始偏移。
    unsigned int ElemCount;      // 4    // 要渲染为三角形的索引数（为 3 的倍数）。顶点存储在调用方 ImDrawList 的 vtx_buffer[] 数组中，索引存储在 idx_buffer[] 中。
    ImDrawCallback UserCallback; // 4-8  // 如果 != NULL，则调用该函数代替渲染顶点。clip_rect 和 texture_id 将正常设置。
    void *UserCallbackData;      // 4-8  // 回调用户数据（当 UserCallback != NULL 时）。如果调用了 AddCallback() 且 size == 0，这是 AddCallback() 参数的副本。如果调用了 AddCallback() 且 size > 0，则指向存储数据的缓冲区。
    int UserCallbackDataSize;    // 4 // 使用存储时回调用户数据的大小，否则为 0。
    int UserCallbackDataOffset;  // 4 // [内部] 使用存储时回调用户数据的偏移量，否则为 -1。

    ImDrawCmd()
    {
        memset(this, 0, sizeof(*this));
    } // 还确保我们的填充字段被置零

    // 从 1.83 起：返回与此绘制调用关联的 ImTextureID。警告：不要假设它始终与 'TextureId' 相同（我们将在即将发布的功能中更改此函数）
    inline ImTextureID GetTexID() const { return TextureId; }
};

// Vertex layout
#ifndef IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT
struct ImDrawVert
{
    ImVec2  pos;
    ImVec2  uv;
    ImU32   col;
};
#else
// You can override the vertex format layout by defining IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT in imconfig.h
// The code expect ImVec2 pos (8 bytes), ImVec2 uv (8 bytes), ImU32 col (4 bytes), but you can re-order them or add other fields as needed to simplify integration in your engine.
// The type has to be described within the macro (you can either declare the struct or use a typedef). This is because ImVec2/ImU32 are likely not declared at the time you'd want to set your type up.
// NOTE: IMGUI DOESN'T CLEAR THE STRUCTURE AND DOESN'T CALL A CONSTRUCTOR SO ANY CUSTOM FIELD WILL BE UNINITIALIZED. IF YOU ADD EXTRA FIELDS (SUCH AS A 'Z' COORDINATES) YOU WILL NEED TO CLEAR THEM DURING RENDER OR TO IGNORE THEM.
IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT;
#endif

// [Internal] For use by ImDrawList
struct ImDrawCmdHeader
{
    ImVec4          ClipRect;
    ImTextureID     TextureId;
    unsigned int    VtxOffset;
};

// [Internal] For use by ImDrawListSplitter
struct ImDrawChannel
{
    ImVector<ImDrawCmd>         _CmdBuffer;
    ImVector<ImDrawIdx>         _IdxBuffer;
};

// Split/Merge 函数用于将绘制列表拆分成不同的层，这些层可以不按顺序地绘制。
// 这被 Columns/Tables API 使用，因此每一列的项可以在同一次绘制调用中一起批处理。
struct ImDrawListSplitter
{
    int _Current;                      // 当前通道号（0）
    int _Count;                        // 活跃通道的数量（1+）
    ImVector<ImDrawChannel> _Channels; // 绘制通道（未调整大小，因此 _Count 可能小于 Channels.Size）

    inline ImDrawListSplitter()  { memset(this, 0, sizeof(*this)); }
    inline ~ImDrawListSplitter() { ClearFreeMemory(); }
    inline void Clear()
    {
        _Current = 0;
        _Count = 1;
    } // 不清除 Channels[]，以便我们的分配在下一帧复用
    IMGUI_API void              ClearFreeMemory();
    IMGUI_API void              Split(ImDrawList* draw_list, int count);
    IMGUI_API void              Merge(ImDrawList* draw_list);
    IMGUI_API void              SetCurrentChannel(ImDrawList* draw_list, int channel_idx);
};

// ImDrawList 函数的标志
// （遗留：位 0 必须始终对应 ImDrawFlags_Closed，以便与使用布尔值的旧 API 向后兼容。位 1..3 必须未使用）
enum ImDrawFlags_
{
    ImDrawFlags_None = 0,
    ImDrawFlags_Closed = 1 << 0,                  // PathStroke(), AddPolyline(): 指定形状应关闭（重要：由于兼容性原因，这始终为 == 1）
    ImDrawFlags_RoundCornersTopLeft = 1 << 4,     // AddRect(), AddRectFilled(), PathRect(): 仅启用左上角圆角（当圆角 > 0.0f 时，默认为所有角）。曾为 0x01。
    ImDrawFlags_RoundCornersTopRight = 1 << 5,    // AddRect(), AddRectFilled(), PathRect(): 仅启用右上角圆角（当圆角 > 0.0f 时，默认为所有角）。曾为 0x02。
    ImDrawFlags_RoundCornersBottomLeft = 1 << 6,  // AddRect(), AddRectFilled(), PathRect(): 仅启用左下角圆角（当圆角 > 0.0f 时，默认为所有角）。曾为 0x04。
    ImDrawFlags_RoundCornersBottomRight = 1 << 7, // AddRect(), AddRectFilled(), PathRect(): 仅启用右下角圆角（当圆角 > 0.0f 时，默认为所有角）。曾为 0x08。
    ImDrawFlags_RoundCornersNone = 1 << 8,        // AddRect(), AddRectFilled(), PathRect(): 禁用所有角的圆角（当圆角 > 0.0f 时）。这不是零，不是隐式标志！
    ImDrawFlags_RoundCornersTop = ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersTopRight,
    ImDrawFlags_RoundCornersBottom = ImDrawFlags_RoundCornersBottomLeft | ImDrawFlags_RoundCornersBottomRight,
    ImDrawFlags_RoundCornersLeft = ImDrawFlags_RoundCornersBottomLeft | ImDrawFlags_RoundCornersTopLeft,
    ImDrawFlags_RoundCornersRight = ImDrawFlags_RoundCornersBottomRight | ImDrawFlags_RoundCornersTopRight,
    ImDrawFlags_RoundCornersAll = ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersTopRight | ImDrawFlags_RoundCornersBottomLeft | ImDrawFlags_RoundCornersBottomRight,
    ImDrawFlags_RoundCornersDefault_ = ImDrawFlags_RoundCornersAll, // 如果未指定任何 _RoundCornersXX 标志，则默认为所有角。
    ImDrawFlags_RoundCornersMask_ = ImDrawFlags_RoundCornersAll | ImDrawFlags_RoundCornersNone,
};

// ImDrawList 实例的标志。这些标志由 ImGui:: 函数根据 ImGuiIO 设置自动设置，通常不会直接操作。
// 然而，在调用 ImDrawList:: 函数之间，仍然可以临时修改标志。
enum ImDrawListFlags_
{
    ImDrawListFlags_None = 0,
    ImDrawListFlags_AntiAliasedLines = 1 << 0,       // 启用抗锯齿线条/边框（对于宽度为 1.0f 的线条或足够细以使用纹理绘制的线条，三角形数量翻倍，否则是三倍的三角形数量）
    ImDrawListFlags_AntiAliasedLinesUseTex = 1 << 1, // 在可能的情况下使用纹理启用抗锯齿线条/边框。要求后端使用双线性过滤（不是点/最近邻过滤）。
    ImDrawListFlags_AntiAliasedFill = 1 << 2,        // 启用填充形状（圆角矩形、圆形）周围的抗锯齿边缘。
    ImDrawListFlags_AllowVtxOffset = 1 << 3,         // 可以发出 'VtxOffset > 0' 以允许较大的网格。当启用 'ImGuiBackendFlags_RendererHasVtxOffset' 时设置。
};

// 绘制命令列表
// 这是 ImGui:: 函数填充的低级多边形列表。在每帧结束时，
// 所有命令列表会传递给你的 ImGuiIO::RenderDrawListFn 函数进行渲染。
// 每个 Dear ImGui 窗口都有自己的 ImDrawList。你可以使用 ImGui::GetWindowDrawList()
// 来访问当前窗口的绘制列表并绘制自定义图元。
// 你可以交替使用普通的 ImGui:: 调用和将图元添加到当前的绘制列表中。
// 在单视口模式下，左上角 == GetMainViewport()->Pos（通常是 0,0），右下角 == GetMainViewport()->Pos + Size（通常是 io.DisplaySize）。
// 你完全可以将任何变换矩阵应用于数据（根据变换的使用情况，你可能也需要将其应用于 ClipRect！）
// 重要：图元总是添加到列表中，而不会被剔除（剔除是在更高级别由 ImGui:: 函数完成的），如果你大量使用这个 API，考虑对绘制的对象进行粗略的剔除。
struct ImDrawList
{
    // 这是你需要渲染的内容
    ImVector<ImDrawCmd> CmdBuffer;  // 绘制命令。通常 1 个命令 = 1 个 GPU 绘制调用，除非命令是回调。
    ImVector<ImDrawIdx> IdxBuffer;  // 索引缓冲区。每个命令消耗 ImDrawCmd::ElemCount 个索引
    ImVector<ImDrawVert> VtxBuffer; // 顶点缓冲区。
    ImDrawListFlags Flags;          // 标志，你可以修改这些标志以调整每个图元的抗锯齿设置。

    // [内部，构建列表时使用]
    unsigned int _VtxCurrentIdx;           // [内部] 通常 == VtxBuffer.Size，除非我们超过了 64K 顶点，在这种情况下它会被重置为 0。
    ImDrawListSharedData *_Data;           // 指向共享绘制数据的指针（你可以使用 ImGui::GetDrawListSharedData() 获取当前 ImGui 上下文中的数据）
    ImDrawVert *_VtxWritePtr;              // [内部] 每个添加命令后在 VtxBuffer.Data 中的指针（为了避免过多使用 ImVector<> 操作符）
    ImDrawIdx *_IdxWritePtr;               // [内部] 每个添加命令后在 IdxBuffer.Data 中的指针（为了避免过多使用 ImVector<> 操作符）
    ImVector<ImVec2> _Path;                // [内部] 当前路径构建
    ImDrawCmdHeader _CmdHeader;            // [内部] 活动命令的模板。字段应与 CmdBuffer.back() 的字段匹配。
    ImDrawListSplitter _Splitter;          // [内部] 用于通道 API（注意：最好使用你自己持久化的 ImDrawListSplitter 实例！）
    ImVector<ImVec4> _ClipRectStack;       // [内部]
    ImVector<ImTextureID> _TextureIdStack; // [内部]
    ImVector<ImU8> _CallbacksDataBuf;      // [内部]
    float _FringeScale;                    // [内部] 抗锯齿边缘通过此值进行缩放，这有助于在缩放顶点缓冲区内容时保持清晰
    const char *_OwnerName;                // 指向拥有窗口名称的指针，用于调试

    // 如果你想创建 ImDrawList 实例，可以传递它们 ImGui::GetDrawListSharedData()。
    // （高级：你可以创建并使用自己的 ImDrawListSharedData，这样你就可以在没有 ImGui 的情况下使用 ImDrawList，但这更复杂）
    IMGUI_API ImDrawList(ImDrawListSharedData* shared_data);
    IMGUI_API ~ImDrawList();

    IMGUI_API void PushClipRect(const ImVec2 &clip_rect_min, const ImVec2 &clip_rect_max, bool intersect_with_current_clip_rect = false); // 渲染级别的剪裁。这个会传递到你的渲染函数中，但不会用于 CPU 端的粗剪裁。更推荐使用更高级的 ImGui::PushClipRect() 来影响逻辑（命中测试和小部件剔除）
    IMGUI_API void  PushClipRectFullScreen();
    IMGUI_API void  PopClipRect();
    IMGUI_API void  PushTextureID(ImTextureID texture_id);
    IMGUI_API void  PopTextureID();
    inline ImVec2   GetClipRectMin() const { const ImVec4& cr = _ClipRectStack.back(); return ImVec2(cr.x, cr.y); }
    inline ImVec2   GetClipRectMax() const { const ImVec4& cr = _ClipRectStack.back(); return ImVec2(cr.z, cr.w); }

    // 原始图形
    // - 填充形状必须始终使用顺时针方向的绕线。抗锯齿边缘依赖于此。逆时针形状将会有“向内”的抗锯齿效果。
    // - 对于矩形原始图形，“p_min”和“p_max”表示左上角和右下角。
    // - 对于圆形原始图形，使用 "num_segments == 0" 来自动计算细分（推荐）。
    //   在旧版本（直到 Dear ImGui 1.77）中，AddCircle 函数默认的 num_segments 为 12。
    //   在未来的版本中，我们将使用纹理来提供更便宜和更高质量的圆形。
    //   如果你需要保证特定的边数，请使用 AddNgon() 和 AddNgonFilled() 函数。

    IMGUI_API void AddLine(const ImVec2 &p1, const ImVec2 &p2, ImU32 col, float thickness = 1.0f);                                                                                                              // 添加线段
    IMGUI_API void AddRect(const ImVec2 &p_min, const ImVec2 &p_max, ImU32 col, float rounding = 0.0f, ImDrawFlags flags = 0, float thickness = 1.0f);                                                          // 添加矩形：a: 左上角，b: 右下角（== 左上角 + 大小）
    IMGUI_API void AddRectFilled(const ImVec2 &p_min, const ImVec2 &p_max, ImU32 col, float rounding = 0.0f, ImDrawFlags flags = 0);                                                                            // 填充矩形：a: 左上角，b: 右下角（== 左上角 + 大小）
    IMGUI_API void AddRectFilledMultiColor(const ImVec2 &p_min, const ImVec2 &p_max, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left);                                         // 填充矩形，多色
    IMGUI_API void AddQuad(const ImVec2 &p1, const ImVec2 &p2, const ImVec2 &p3, const ImVec2 &p4, ImU32 col, float thickness = 1.0f);                                                                          // 添加四边形
    IMGUI_API void AddQuadFilled(const ImVec2 &p1, const ImVec2 &p2, const ImVec2 &p3, const ImVec2 &p4, ImU32 col);                                                                                            // 填充四边形
    IMGUI_API void AddTriangle(const ImVec2 &p1, const ImVec2 &p2, const ImVec2 &p3, ImU32 col, float thickness = 1.0f);                                                                                        // 添加三角形
    IMGUI_API void AddTriangleFilled(const ImVec2 &p1, const ImVec2 &p2, const ImVec2 &p3, ImU32 col);                                                                                                          // 填充三角形
    IMGUI_API void AddCircle(const ImVec2 &center, float radius, ImU32 col, int num_segments = 0, float thickness = 1.0f);                                                                                      // 添加圆形
    IMGUI_API void AddCircleFilled(const ImVec2 &center, float radius, ImU32 col, int num_segments = 0);                                                                                                        // 填充圆形
    IMGUI_API void AddNgon(const ImVec2 &center, float radius, ImU32 col, int num_segments, float thickness = 1.0f);                                                                                            // 添加正多边形
    IMGUI_API void AddNgonFilled(const ImVec2 &center, float radius, ImU32 col, int num_segments);                                                                                                              // 填充正多边形
    IMGUI_API void AddEllipse(const ImVec2 &center, const ImVec2 &radius, ImU32 col, float rot = 0.0f, int num_segments = 0, float thickness = 1.0f);                                                           // 添加椭圆
    IMGUI_API void AddEllipseFilled(const ImVec2 &center, const ImVec2 &radius, ImU32 col, float rot = 0.0f, int num_segments = 0);                                                                             // 填充椭圆
    IMGUI_API void AddText(const ImVec2 &pos, ImU32 col, const char *text_begin, const char *text_end = NULL);                                                                                                  // 添加文本
    IMGUI_API void AddText(ImFont *font, float font_size, const ImVec2 &pos, ImU32 col, const char *text_begin, const char *text_end = NULL, float wrap_width = 0.0f, const ImVec4 *cpu_fine_clip_rect = NULL); // 使用字体和字号添加文本
    IMGUI_API void AddBezierCubic(const ImVec2 &p1, const ImVec2 &p2, const ImVec2 &p3, const ImVec2 &p4, ImU32 col, float thickness, int num_segments = 0);                                                    // 添加三次贝塞尔曲线（4个控制点）
    IMGUI_API void AddBezierQuadratic(const ImVec2 &p1, const ImVec2 &p2, const ImVec2 &p3, ImU32 col, float thickness, int num_segments = 0);                                                                  // 添加二次贝塞尔曲线（3个控制点）

    // 一般多边形
    // - 填充函数仅支持简单多边形（无自交，无孔洞）。
    // - 凹多边形的填充比凸多边形更昂贵：其复杂度为 O(N^2)。为用户提供，主要库不使用。
    IMGUI_API void AddPolyline(const ImVec2 *points, int num_points, ImU32 col, ImDrawFlags flags, float thickness); // 添加多边形线条
    IMGUI_API void AddConvexPolyFilled(const ImVec2 *points, int num_points, ImU32 col);                             // 填充凸多边形
    IMGUI_API void AddConcavePolyFilled(const ImVec2 *points, int num_points, ImU32 col);                            // 填充凹多边形

    // 图像原始图形
    // - 阅读FAQ以了解ImTextureID是什么。
    // - "p_min" 和 "p_max" 表示矩形的左上角和右下角。
    // - "uv_min" 和 "uv_max" 表示用于这些角落的归一化纹理坐标。使用 (0,0)->(1,1) 的纹理坐标通常会显示整个纹理。
    IMGUI_API void AddImage(ImTextureID user_texture_id, const ImVec2 &p_min, const ImVec2 &p_max, const ImVec2 &uv_min = ImVec2(0, 0), const ImVec2 &uv_max = ImVec2(1, 1), ImU32 col = IM_COL32_WHITE);                                                                                                 // 添加图像
    IMGUI_API void AddImageQuad(ImTextureID user_texture_id, const ImVec2 &p1, const ImVec2 &p2, const ImVec2 &p3, const ImVec2 &p4, const ImVec2 &uv1 = ImVec2(0, 0), const ImVec2 &uv2 = ImVec2(1, 0), const ImVec2 &uv3 = ImVec2(1, 1), const ImVec2 &uv4 = ImVec2(0, 1), ImU32 col = IM_COL32_WHITE); // 添加四边形图像
    IMGUI_API void AddImageRounded(ImTextureID user_texture_id, const ImVec2 &p_min, const ImVec2 &p_max, const ImVec2 &uv_min, const ImVec2 &uv_max, ImU32 col, float rounding, ImDrawFlags flags = 0);                                                                                                  // 添加圆角图像

    // 状态路径API，添加点然后使用 PathFillConvex() 或 PathStroke() 完成
    // - 重要：填充形状必须始终使用顺时针方向！抗锯齿的边缘依赖于此。逆时针形状会有“向内”的抗锯齿。
    //   例如，'PathArcTo(center, radius, PI * -0.5f, PI)' 是正确的，而 'PathArcTo(center, radius, PI, PI * -0.5f)' 在紧跟 PathFillConvex() 后将不会有正确的抗锯齿效果。
    inline void PathClear()
    {
        _Path.Size = 0;
    } // 清空路径
    inline void PathLineTo(const ImVec2 &pos)
    {
        _Path.push_back(pos);
    } // 向路径添加一条线
    inline void PathLineToMergeDuplicate(const ImVec2 &pos)
    {
        if (_Path.Size == 0 || memcmp(&_Path.Data[_Path.Size - 1], &pos, 8) != 0)
            _Path.push_back(pos);
    } // 向路径添加一条线，避免重复点
    inline void PathFillConvex(ImU32 col)
    {
        AddConvexPolyFilled(_Path.Data, _Path.Size, col);
        _Path.Size = 0;
    } // 填充凸多边形并清空路径
    inline void PathFillConcave(ImU32 col)
    {
        AddConcavePolyFilled(_Path.Data, _Path.Size, col);
        _Path.Size = 0;
    } // 填充凹多边形并清空路径
    inline void PathStroke(ImU32 col, ImDrawFlags flags = 0, float thickness = 1.0f)
    {
        AddPolyline(_Path.Data, _Path.Size, col, flags, thickness);
        _Path.Size = 0;
    }                                                                                                                                          // 画出路径的轮廓并清空路径
    IMGUI_API void PathArcTo(const ImVec2 &center, float radius, float a_min, float a_max, int num_segments = 0);                              // 添加圆弧路径
    IMGUI_API void PathArcToFast(const ImVec2 &center, float radius, int a_min_of_12, int a_max_of_12);                                        // 使用预计算的角度画12步的圆
    IMGUI_API void PathEllipticalArcTo(const ImVec2 &center, const ImVec2 &radius, float rot, float a_min, float a_max, int num_segments = 0); // 椭圆路径
    IMGUI_API void PathBezierCubicCurveTo(const ImVec2 &p2, const ImVec2 &p3, const ImVec2 &p4, int num_segments = 0);                         // 三次贝塞尔曲线（4个控制点）
    IMGUI_API void PathBezierQuadraticCurveTo(const ImVec2 &p2, const ImVec2 &p3, int num_segments = 0);                                       // 二次贝塞尔曲线（3个控制点）
    IMGUI_API void PathRect(const ImVec2 &rect_min, const ImVec2 &rect_max, float rounding = 0.0f, ImDrawFlags flags = 0);                     // 添加矩形路径

    // 高级：绘制回调
    // - 可用于更改渲染状态（更改采样器、混合、当前着色器）。可用于发出自定义渲染命令（虽然难以正确实现，但可能）。
    // - 使用特殊的 ImDrawCallback_ResetRenderState 回调，指示后端将其渲染状态重置为默认值。
    // - 渲染循环必须检查 ImDrawCmd 中的 'UserCallback' 并调用该函数，而不是渲染三角形。所有标准后端都遵循这一点。
    // - 对于某些后端，回调可能会访问后端在 ImGui_ImplXXXX_RenderState 结构体中公开的选定渲染状态，该结构体由 platform_io.Renderer_RenderState 指向。
    // - 重要：请注意使用 size==0（复制参数）和使用 size>0（将指向的数据复制到缓冲区）之间不同的间接级别。
    //   - 如果 userdata_size == 0：我们按原样复制/存储 'userdata' 参数。它将在渲染期间以未修改的形式可用，存储在 ImDrawCmd::UserCallbackData 中。
    //   - 如果 userdata_size > 0，我们将复制/存储由 'userdata' 指向的 'userdata_size' 字节。我们将它们存储在一个缓冲区内，该缓冲区存储在绘制列表中。ImDrawCmd::UserCallbackData 将指向该缓冲区，因此你需要从中检索数据。如果你期望动态大小的数据，回调可能需要使用 ImDrawCmd::UserCallbackDataSize。
    //   - 对 userdata_size > 0 的支持是在 v1.91.4（2024年10月）中添加的。因此，早期代码仅允许复制/存储简单的 void*。
    IMGUI_API void  AddCallback(ImDrawCallback callback, void* userdata, size_t userdata_size = 0);

    // 高级：其他
    IMGUI_API void AddDrawCmd();               // 如果需要强制创建新的绘制调用（以允许依赖渲染/混合），这非常有用。否则，原始图形将尽可能合并到同一个绘制调用中。
    IMGUI_API ImDrawList *CloneOutput() const; // 创建 CmdBuffer/IdxBuffer/VtxBuffer 的克隆

    // 高级：通道
    // - 用于将渲染分割成不同的层。通过切换通道，可以实现无序渲染（例如，先提交前景图形，再提交背景图形）。
    // - 用于减少绘制调用（例如，如果在多个裁剪矩形之间来回切换，最好将其追加到不同的通道中，然后在最后合并）。
    // - 这个 API 本不应该出现在 ImDrawList 中！
    //   更推荐使用您自己持久化的 ImDrawListSplitter 实例，因为您可以堆叠它们。
    //   使用 ImDrawList::ChannelsXXXX 时，您不能将一个拆分堆叠到另一个上面。
    inline void ChannelsSplit(int count)
    {
        _Splitter.Split(this, count);
    } // 将渲染分割成多个通道
    inline void ChannelsMerge()
    {
        _Splitter.Merge(this);
    } // 合并多个通道的渲染
    inline void ChannelsSetCurrent(int n)
    {
        _Splitter.SetCurrentChannel(this, n);
    } // 设置当前通道

    // 高级：图形分配
    // - 我们渲染三角形（三个顶点）
    // - 所有图形在使用前需要通过 PrimReserve() 进行预留。
    IMGUI_API void PrimReserve(int idx_count, int vtx_count);                                                                                                                                 // 预留图形的索引和顶点数量
    IMGUI_API void PrimUnreserve(int idx_count, int vtx_count);                                                                                                                               // 取消预留的图形索引和顶点数量
    IMGUI_API void PrimRect(const ImVec2 &a, const ImVec2 &b, ImU32 col);                                                                                                                     // 绘制轴对齐矩形（由两个三角形组成）
    IMGUI_API void PrimRectUV(const ImVec2 &a, const ImVec2 &b, const ImVec2 &uv_a, const ImVec2 &uv_b, ImU32 col);                                                                           // 带有 UV 坐标的矩形
    IMGUI_API void PrimQuadUV(const ImVec2 &a, const ImVec2 &b, const ImVec2 &c, const ImVec2 &d, const ImVec2 &uv_a, const ImVec2 &uv_b, const ImVec2 &uv_c, const ImVec2 &uv_d, ImU32 col); // 带有 UV 坐标的四边形
    inline void PrimWriteVtx(const ImVec2 &pos, const ImVec2 &uv, ImU32 col)
    {
        _VtxWritePtr->pos = pos;
        _VtxWritePtr->uv = uv;
        _VtxWritePtr->col = col;
        _VtxWritePtr++;
        _VtxCurrentIdx++;
    } // 写入顶点数据
    inline void PrimWriteIdx(ImDrawIdx idx)
    {
        *_IdxWritePtr = idx;
        _IdxWritePtr++;
    } // 写入索引数据
    inline void PrimVtx(const ImVec2 &pos, const ImVec2 &uv, ImU32 col)
    {
        PrimWriteIdx((ImDrawIdx)_VtxCurrentIdx);
        PrimWriteVtx(pos, uv, col);
    } // 使用唯一索引写入顶点

    // 废弃的名称
    // inline  void  AddEllipse(const ImVec2& center, float radius_x, float radius_y, ImU32 col, float rot = 0.0f, int num_segments = 0, float thickness = 1.0f) { AddEllipse(center, ImVec2(radius_x, radius_y), col, rot, num_segments, thickness); } // 在 1.90.5 版本中废弃（2024年3月）
    // inline  void  AddEllipseFilled(const ImVec2& center, float radius_x, float radius_y, ImU32 col, float rot = 0.0f, int num_segments = 0) { AddEllipseFilled(center, ImVec2(radius_x, radius_y), col, rot, num_segments); } // 在 1.90.5 版本中废弃（2024年3月）
    // inline  void  PathEllipticalArcTo(const ImVec2& center, float radius_x, float radius_y, float rot, float a_min, float a_max, int num_segments = 0) { PathEllipticalArcTo(center, ImVec2(radius_x, radius_y), rot, a_min, a_max, num_segments); } // 在 1.90.5 版本中废弃（2024年3月）
    // inline  void  AddBezierCurve(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness, int num_segments = 0) { AddBezierCubic(p1, p2, p3, p4, col, thickness, num_segments); } // 在 1.80 版本中废弃（2021年1月）
    // inline  void  PathBezierCurveTo(const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, int num_segments = 0) { PathBezierCubicCurveTo(p2, p3, p4, num_segments); } // 在 1.80 版本中废弃（2021年1月）

    // [内部辅助函数]
    IMGUI_API void _ResetForNewFrame();                                                                                  // 重置为新的一帧
    IMGUI_API void _ClearFreeMemory();                                                                                   // 清理未使用的内存
    IMGUI_API void _PopUnusedDrawCmd();                                                                                  // 弹出未使用的绘制命令
    IMGUI_API void _TryMergeDrawCmds();                                                                                  // 尝试合并绘制命令
    IMGUI_API void _OnChangedClipRect();                                                                                 // 处理剪裁矩形变化
    IMGUI_API void _OnChangedTextureID();                                                                                // 处理纹理ID变化
    IMGUI_API void _OnChangedVtxOffset();                                                                                // 处理顶点偏移变化
    IMGUI_API void _SetTextureID(ImTextureID texture_id);                                                                // 设置纹理ID
    IMGUI_API int _CalcCircleAutoSegmentCount(float radius) const;                                                       // 计算圆形自动分段数
    IMGUI_API void _PathArcToFastEx(const ImVec2 &center, float radius, int a_min_sample, int a_max_sample, int a_step); // 快速路径圆弧（扩展）
    IMGUI_API void _PathArcToN(const ImVec2 &center, float radius, float a_min, float a_max, int num_segments);          // 路径圆弧，指定分段数
};

// 渲染 Dear ImGui 帧的所有绘制数据
// （注意：样式和命名约定在这里有些不一致，我们目前保留它们是为了向后兼容，
// 因为这是库中最早公开的结构之一！基本上，ImDrawList == CmdList）
struct ImDrawData
{
    bool Valid;                      // 仅在调用 Render() 后并且在下一次 NewFrame() 调用之前有效。
    int CmdListsCount;               // 要渲染的 ImDrawList* 数量
    int TotalIdxCount;               // 方便起见，所有 ImDrawList 的 IdxBuffer.Size 之和
    int TotalVtxCount;               // 方便起见，所有 ImDrawList 的 VtxBuffer.Size 之和
    ImVector<ImDrawList *> CmdLists; // 要渲染的 ImDrawList* 数组。ImDrawLists 由 ImGuiContext 拥有，仅通过指针在这里引用。
    ImVec2 DisplayPos;               // 渲染视口的左上角位置（== 使用的正交投影矩阵的左上角）（== 对于主视口，等同于 GetMainViewport()->Pos，在大多数单视口应用中为 (0.0)）
    ImVec2 DisplaySize;              // 渲染视口的大小（== 对于主视口，等同于 GetMainViewport()->Size，在大多数单视口应用中为 io.DisplaySize）
    ImVec2 FramebufferScale;         // 每个 DisplaySize 单位的像素数量。基于 io.DisplayFramebufferScale。通常在普通显示器上为 (1,1)，在 Retina 显示器上为 OSX 上的 (2,2)。
    ImGuiViewport *OwnerViewport;    // 拥有此 ImDrawData 实例的视口，可能对渲染器有用（通常没有）。

    // 函数
    ImDrawData()
    {
        Clear();
    }                                                      // 构造函数，初始化时调用 Clear()
    IMGUI_API void Clear();                                // 清除 ImDrawData 内容
    IMGUI_API void AddDrawList(ImDrawList *draw_list);     // 辅助函数：将外部绘制列表添加到现有的 ImDrawData 中
    IMGUI_API void DeIndexAllBuffers();                    // 辅助函数：将所有缓冲区从索引模式转换为非索引模式，防止无法进行索引渲染时使用。注意：这很慢，并且很可能浪费资源。始终优先选择索引渲染！
    IMGUI_API void ScaleClipRects(const ImVec2 &fb_scale); // 辅助函数：缩放每个 ImDrawCmd 的 ClipRect 字段。如果你的最终输出缓冲区与 Dear ImGui 预期的缩放不同，或者窗口分辨率与帧缓冲区分辨率不同时使用。
};

//-----------------------------------------------------------------------------
// [SECTION] Font API (ImFontConfig, ImFontGlyph, ImFontAtlasFlags, ImFontAtlas, ImFontGlyphRangesBuilder, ImFont)
//-----------------------------------------------------------------------------

struct ImFontConfig
{
    void*           FontData;               //          // TTF/OTF data
    int             FontDataSize;           //          // TTF/OTF data size
    bool            FontDataOwnedByAtlas;   // true     // TTF/OTF data ownership taken by the container ImFontAtlas (will delete memory itself).
    int             FontNo;                 // 0        // Index of font within TTF/OTF file
    float           SizePixels;             //          // Size in pixels for rasterizer (more or less maps to the resulting font height).
    int             OversampleH;            // 2        // Rasterize at higher quality for sub-pixel positioning. Note the difference between 2 and 3 is minimal. You can reduce this to 1 for large glyphs save memory. Read https://github.com/nothings/stb/blob/master/tests/oversample/README.md for details.
    int             OversampleV;            // 1        // Rasterize at higher quality for sub-pixel positioning. This is not really useful as we don't use sub-pixel positions on the Y axis.
    bool            PixelSnapH;             // false    // Align every glyph AdvanceX to pixel boundaries. Useful e.g. if you are merging a non-pixel aligned font with the default font. If enabled, you can set OversampleH/V to 1.
    ImVec2          GlyphExtraSpacing;      // 0, 0     // Extra spacing (in pixels) between glyphs when rendered: essentially add to glyph->AdvanceX. Only X axis is supported for now.
    ImVec2          GlyphOffset;            // 0, 0     // Offset all glyphs from this font input.
    const ImWchar*  GlyphRanges;            // NULL     // THE ARRAY DATA NEEDS TO PERSIST AS LONG AS THE FONT IS ALIVE. Pointer to a user-provided list of Unicode range (2 value per range, values are inclusive, zero-terminated list).
    float           GlyphMinAdvanceX;       // 0        // Minimum AdvanceX for glyphs, set Min to align font icons, set both Min/Max to enforce mono-space font
    float           GlyphMaxAdvanceX;       // FLT_MAX  // Maximum AdvanceX for glyphs
    bool            MergeMode;              // false    // Merge into previous ImFont, so you can combine multiple inputs font into one ImFont (e.g. ASCII font + icons + Japanese glyphs). You may want to use GlyphOffset.y when merge font of different heights.
    unsigned int    FontBuilderFlags;       // 0        // Settings for custom font builder. THIS IS BUILDER IMPLEMENTATION DEPENDENT. Leave as zero if unsure.
    float           RasterizerMultiply;     // 1.0f     // Linearly brighten (>1.0f) or darken (<1.0f) font output. Brightening small fonts may be a good workaround to make them more readable. This is a silly thing we may remove in the future.
    float           RasterizerDensity;      // 1.0f     // DPI scale for rasterization, not altering other font metrics: make it easy to swap between e.g. a 100% and a 400% fonts for a zooming display. IMPORTANT: If you increase this it is expected that you increase font scale accordingly, otherwise quality may look lowered.
    ImWchar         EllipsisChar;           // 0        // Explicitly specify unicode codepoint of ellipsis character. When fonts are being merged first specified ellipsis will be used.

    // [Internal]
    char            Name[40];               // Name (strictly to ease debugging)
    ImFont*         DstFont;

    IMGUI_API ImFontConfig();
};

// Hold rendering data for one glyph.
// (Note: some language parsers may fail to convert the 31+1 bitfield members, in this case maybe drop store a single u32 or we can rework this)
struct ImFontGlyph
{
    unsigned int    Colored : 1;        // Flag to indicate glyph is colored and should generally ignore tinting (make it usable with no shift on little-endian as this is used in loops)
    unsigned int    Visible : 1;        // Flag to indicate glyph has no visible pixels (e.g. space). Allow early out when rendering.
    unsigned int    Codepoint : 30;     // 0x0000..0x10FFFF
    float           AdvanceX;           // Distance to next character (= data from font + ImFontConfig::GlyphExtraSpacing.x baked in)
    float           X0, Y0, X1, Y1;     // Glyph corners
    float           U0, V0, U1, V1;     // Texture coordinates
};

// Helper to build glyph ranges from text/string data. Feed your application strings/characters to it then call BuildRanges().
// This is essentially a tightly packed of vector of 64k booleans = 8KB storage.
struct ImFontGlyphRangesBuilder
{
    ImVector<ImU32> UsedChars;            // Store 1-bit per Unicode code point (0=unused, 1=used)

    ImFontGlyphRangesBuilder()              { Clear(); }
    inline void     Clear()                 { int size_in_bytes = (IM_UNICODE_CODEPOINT_MAX + 1) / 8; UsedChars.resize(size_in_bytes / (int)sizeof(ImU32)); memset(UsedChars.Data, 0, (size_t)size_in_bytes); }
    inline bool     GetBit(size_t n) const  { int off = (int)(n >> 5); ImU32 mask = 1u << (n & 31); return (UsedChars[off] & mask) != 0; }  // Get bit n in the array
    inline void     SetBit(size_t n)        { int off = (int)(n >> 5); ImU32 mask = 1u << (n & 31); UsedChars[off] |= mask; }               // Set bit n in the array
    inline void     AddChar(ImWchar c)      { SetBit(c); }                      // Add character
    IMGUI_API void  AddText(const char* text, const char* text_end = NULL);     // Add string (each character of the UTF-8 string are added)
    IMGUI_API void  AddRanges(const ImWchar* ranges);                           // Add ranges, e.g. builder.AddRanges(ImFontAtlas::GetGlyphRangesDefault()) to force add all of ASCII/Latin+Ext
    IMGUI_API void  BuildRanges(ImVector<ImWchar>* out_ranges);                 // Output new ranges
};

// See ImFontAtlas::AddCustomRectXXX functions.
struct ImFontAtlasCustomRect
{
    unsigned short  X, Y;           // Output   // Packed position in Atlas

    // [Internal]
    unsigned short  Width, Height;  // Input    // Desired rectangle dimension
    unsigned int    GlyphID : 31;   // Input    // For custom font glyphs only (ID < 0x110000)
    unsigned int    GlyphColored : 1; // Input  // For custom font glyphs only: glyph is colored, removed tinting.
    float           GlyphAdvanceX;  // Input    // For custom font glyphs only: glyph xadvance
    ImVec2          GlyphOffset;    // Input    // For custom font glyphs only: glyph display offset
    ImFont*         Font;           // Input    // For custom font glyphs only: target font
    ImFontAtlasCustomRect()         { X = Y = 0xFFFF; Width = Height = 0; GlyphID = 0; GlyphColored = 0; GlyphAdvanceX = 0.0f; GlyphOffset = ImVec2(0, 0); Font = NULL; }
    bool IsPacked() const           { return X != 0xFFFF; }
};

// Flags for ImFontAtlas build
enum ImFontAtlasFlags_
{
    ImFontAtlasFlags_None               = 0,
    ImFontAtlasFlags_NoPowerOfTwoHeight = 1 << 0,   // Don't round the height to next power of two
    ImFontAtlasFlags_NoMouseCursors     = 1 << 1,   // Don't build software mouse cursors into the atlas (save a little texture memory)
    ImFontAtlasFlags_NoBakedLines       = 1 << 2,   // Don't build thick line textures into the atlas (save a little texture memory, allow support for point/nearest filtering). The AntiAliasedLinesUseTex features uses them, otherwise they will be rendered using polygons (more expensive for CPU/GPU).
};

// Load and rasterize multiple TTF/OTF fonts into a same texture. The font atlas will build a single texture holding:
//  - One or more fonts.
//  - Custom graphics data needed to render the shapes needed by Dear ImGui.
//  - Mouse cursor shapes for software cursor rendering (unless setting 'Flags |= ImFontAtlasFlags_NoMouseCursors' in the font atlas).
// It is the user-code responsibility to setup/build the atlas, then upload the pixel data into a texture accessible by your graphics api.
//  - Optionally, call any of the AddFont*** functions. If you don't call any, the default font embedded in the code will be loaded for you.
//  - Call GetTexDataAsAlpha8() or GetTexDataAsRGBA32() to build and retrieve pixels data.
//  - Upload the pixels data into a texture within your graphics system (see imgui_impl_xxxx.cpp examples)
//  - Call SetTexID(my_tex_id); and pass the pointer/identifier to your texture in a format natural to your graphics API.
//    This value will be passed back to you during rendering to identify the texture. Read FAQ entry about ImTextureID for more details.
// Common pitfalls:
// - If you pass a 'glyph_ranges' array to AddFont*** functions, you need to make sure that your array persist up until the
//   atlas is build (when calling GetTexData*** or Build()). We only copy the pointer, not the data.
// - Important: By default, AddFontFromMemoryTTF() takes ownership of the data. Even though we are not writing to it, we will free the pointer on destruction.
//   You can set font_cfg->FontDataOwnedByAtlas=false to keep ownership of your data and it won't be freed,
// - Even though many functions are suffixed with "TTF", OTF data is supported just as well.
// - This is an old API and it is currently awkward for those and various other reasons! We will address them in the future!
struct ImFontAtlas
{
    IMGUI_API ImFontAtlas();
    IMGUI_API ~ImFontAtlas();
    IMGUI_API ImFont*           AddFont(const ImFontConfig* font_cfg);
    IMGUI_API ImFont*           AddFontDefault(const ImFontConfig* font_cfg = NULL);
    IMGUI_API ImFont*           AddFontFromFileTTF(const char* filename, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL);
    IMGUI_API ImFont*           AddFontFromMemoryTTF(void* font_data, int font_data_size, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL); // Note: Transfer ownership of 'ttf_data' to ImFontAtlas! Will be deleted after destruction of the atlas. Set font_cfg->FontDataOwnedByAtlas=false to keep ownership of your data and it won't be freed.
    IMGUI_API ImFont*           AddFontFromMemoryCompressedTTF(const void* compressed_font_data, int compressed_font_data_size, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL); // 'compressed_font_data' still owned by caller. Compress with binary_to_compressed_c.cpp.
    IMGUI_API ImFont*           AddFontFromMemoryCompressedBase85TTF(const char* compressed_font_data_base85, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL);              // 'compressed_font_data_base85' still owned by caller. Compress with binary_to_compressed_c.cpp with -base85 parameter.
    IMGUI_API void              ClearInputData();           // Clear input data (all ImFontConfig structures including sizes, TTF data, glyph ranges, etc.) = all the data used to build the texture and fonts.
    IMGUI_API void              ClearTexData();             // Clear output texture data (CPU side). Saves RAM once the texture has been copied to graphics memory.
    IMGUI_API void              ClearFonts();               // Clear output font data (glyphs storage, UV coordinates).
    IMGUI_API void              Clear();                    // Clear all input and output.

    // Build atlas, retrieve pixel data.
    // User is in charge of copying the pixels into graphics memory (e.g. create a texture with your engine). Then store your texture handle with SetTexID().
    // The pitch is always = Width * BytesPerPixels (1 or 4)
    // Building in RGBA32 format is provided for convenience and compatibility, but note that unless you manually manipulate or copy color data into
    // the texture (e.g. when using the AddCustomRect*** api), then the RGB pixels emitted will always be white (~75% of memory/bandwidth waste.
    IMGUI_API bool              Build();                    // Build pixels data. This is called automatically for you by the GetTexData*** functions.
    IMGUI_API void              GetTexDataAsAlpha8(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel = NULL);  // 1 byte per-pixel
    IMGUI_API void              GetTexDataAsRGBA32(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel = NULL);  // 4 bytes-per-pixel
    bool                        IsBuilt() const             { return Fonts.Size > 0 && TexReady; } // Bit ambiguous: used to detect when user didn't build texture but effectively we should check TexID != 0 except that would be backend dependent...
    void                        SetTexID(ImTextureID id)    { TexID = id; }

    //-------------------------------------------
    // Glyph Ranges
    //-------------------------------------------

    // 获取常见 Unicode 范围的辅助函数（每个范围包含两个值，值是闭区间，零终止的列表）
    // 注意：确保你的字符串是 UTF-8 编码，而不是本地代码页。
    // 阅读 https://github.com/ocornut/imgui/blob/master/docs/FONTS.md/#about-utf-8-encoding 以获取详细信息。
    // 注意：可以使用 ImFontGlyphRangesBuilder 来从文本数据构建字形范围。
    IMGUI_API const ImWchar *GetGlyphRangesDefault();                 // 基本拉丁字母、扩展拉丁字母
    IMGUI_API const ImWchar *GetGlyphRangesGreek();                   // 默认 + 希腊字母和科普特字母
    IMGUI_API const ImWchar *GetGlyphRangesKorean();                  // 默认 + 韩文字符
    IMGUI_API const ImWchar *GetGlyphRangesJapanese();                // 默认 + 平假名、片假名、半角字符、2999个汉字的选集
    IMGUI_API const ImWchar *GetGlyphRangesChineseFull();             // 默认 + 半角字符 + 日文平假名/片假名 + 完整的约21000个汉字统一表意文字
    IMGUI_API const ImWchar *GetGlyphRangesChineseSimplifiedCommon(); // 默认 + 半角字符 + 日文平假名/片假名 + 2500个常用简体中文汉字的统一表意文字集
    IMGUI_API const ImWchar *GetGlyphRangesCyrillic();                // 默认 + 大约400个西里尔字母
    IMGUI_API const ImWchar *GetGlyphRangesThai();                    // 默认 + 泰语字符
    IMGUI_API const ImWchar *GetGlyphRangesVietnamese();              // 默认 + 越南语字符

    //-------------------------------------------
    // [BETA] Custom Rectangles/Glyphs API
    //-------------------------------------------

    // You can request arbitrary rectangles to be packed into the atlas, for your own purposes.
    // - After calling Build(), you can query the rectangle position and render your pixels.
    // - If you render colored output, set 'atlas->TexPixelsUseColors = true' as this may help some backends decide of preferred texture format.
    // - You can also request your rectangles to be mapped as font glyph (given a font + Unicode point),
    //   so you can render e.g. custom colorful icons and use them as regular glyphs.
    // - Read docs/FONTS.md for more details about using colorful icons.
    // - Note: this API may be redesigned later in order to support multi-monitor varying DPI settings.
    IMGUI_API int               AddCustomRectRegular(int width, int height);
    IMGUI_API int               AddCustomRectFontGlyph(ImFont* font, ImWchar id, int width, int height, float advance_x, const ImVec2& offset = ImVec2(0, 0));
    ImFontAtlasCustomRect*      GetCustomRectByIndex(int index) { IM_ASSERT(index >= 0); return &CustomRects[index]; }

    // [Internal]
    IMGUI_API void              CalcCustomRectUV(const ImFontAtlasCustomRect* rect, ImVec2* out_uv_min, ImVec2* out_uv_max) const;
    IMGUI_API bool              GetMouseCursorTexData(ImGuiMouseCursor cursor, ImVec2* out_offset, ImVec2* out_size, ImVec2 out_uv_border[2], ImVec2 out_uv_fill[2]);

    //-------------------------------------------
    // Members
    //-------------------------------------------

    ImFontAtlasFlags            Flags;              // Build flags (see ImFontAtlasFlags_)
    ImTextureID                 TexID;              // User data to refer to the texture once it has been uploaded to user's graphic systems. It is passed back to you during rendering via the ImDrawCmd structure.
    int                         TexDesiredWidth;    // Texture width desired by user before Build(). Must be a power-of-two. If have many glyphs your graphics API have texture size restrictions you may want to increase texture width to decrease height.
    int                         TexGlyphPadding;    // FIXME: Should be called "TexPackPadding". Padding between glyphs within texture in pixels. Defaults to 1. If your rendering method doesn't rely on bilinear filtering you may set this to 0 (will also need to set AntiAliasedLinesUseTex = false).
    bool                        Locked;             // Marked as Locked by ImGui::NewFrame() so attempt to modify the atlas will assert.
    void*                       UserData;           // Store your own atlas related user-data (if e.g. you have multiple font atlas).

    // [Internal]
    // NB: Access texture data via GetTexData*() calls! Which will setup a default font for you.
    bool                        TexReady;           // Set when texture was built matching current font input
    bool                        TexPixelsUseColors; // Tell whether our texture data is known to use colors (rather than just alpha channel), in order to help backend select a format.
    unsigned char*              TexPixelsAlpha8;    // 1 component per pixel, each component is unsigned 8-bit. Total size = TexWidth * TexHeight
    unsigned int*               TexPixelsRGBA32;    // 4 component per pixel, each component is unsigned 8-bit. Total size = TexWidth * TexHeight * 4
    int                         TexWidth;           // Texture width calculated during Build().
    int                         TexHeight;          // Texture height calculated during Build().
    ImVec2                      TexUvScale;         // = (1.0f/TexWidth, 1.0f/TexHeight)
    ImVec2                      TexUvWhitePixel;    // Texture coordinates to a white pixel
    ImVector<ImFont*>           Fonts;              // Hold all the fonts returned by AddFont*. Fonts[0] is the default font upon calling ImGui::NewFrame(), use ImGui::PushFont()/PopFont() to change the current font.
    ImVector<ImFontAtlasCustomRect> CustomRects;    // Rectangles for packing custom texture data into the atlas.
    ImVector<ImFontConfig>      ConfigData;         // Configuration data
    ImVec4                      TexUvLines[IM_DRAWLIST_TEX_LINES_WIDTH_MAX + 1];  // UVs for baked anti-aliased lines

    // [Internal] Font builder
    const ImFontBuilderIO*      FontBuilderIO;      // Opaque interface to a font builder (default to stb_truetype, can be changed to use FreeType by defining IMGUI_ENABLE_FREETYPE).
    unsigned int                FontBuilderFlags;   // Shared flags (for all fonts) for custom font builder. THIS IS BUILD IMPLEMENTATION DEPENDENT. Per-font override is also available in ImFontConfig.

    // [Internal] Packing data
    int                         PackIdMouseCursors; // Custom texture rectangle ID for white pixel and mouse cursors
    int                         PackIdLines;        // Custom texture rectangle ID for baked anti-aliased lines

    // [Obsolete]
    //typedef ImFontAtlasCustomRect    CustomRect;         // OBSOLETED in 1.72+
    //typedef ImFontGlyphRangesBuilder GlyphRangesBuilder; // OBSOLETED in 1.67+
};

// Font runtime data and rendering
// ImFontAtlas automatically loads a default embedded font for you when you call GetTexDataAsAlpha8() or GetTexDataAsRGBA32().
struct ImFont
{
    // [Internal] Members: Hot ~20/24 bytes (for CalcTextSize)
    ImVector<float>             IndexAdvanceX;      // 12-16 // out //            // Sparse. Glyphs->AdvanceX in a directly indexable way (cache-friendly for CalcTextSize functions which only this info, and are often bottleneck in large UI).
    float                       FallbackAdvanceX;   // 4     // out // = FallbackGlyph->AdvanceX
    float                       FontSize;           // 4     // in  //            // Height of characters/line, set during loading (don't change after loading)

    // [Internal] Members: Hot ~28/40 bytes (for RenderText loop)
    ImVector<ImWchar>           IndexLookup;        // 12-16 // out //            // Sparse. Index glyphs by Unicode code-point.
    ImVector<ImFontGlyph>       Glyphs;             // 12-16 // out //            // All glyphs.
    const ImFontGlyph*          FallbackGlyph;      // 4-8   // out // = FindGlyph(FontFallbackChar)

    // [Internal] Members: Cold ~32/40 bytes
    // Conceptually ConfigData[] is the list of font sources merged to create this font.
    ImFontAtlas*                ContainerAtlas;     // 4-8   // out //            // What we has been loaded into
    const ImFontConfig*         ConfigData;         // 4-8   // in  //            // Pointer within ContainerAtlas->ConfigData to ConfigDataCount instances
    short                       ConfigDataCount;    // 2     // in  // ~ 1        // Number of ImFontConfig involved in creating this font. Bigger than 1 when merging multiple font sources into one ImFont.
    short                       EllipsisCharCount;  // 1     // out // 1 or 3
    ImWchar                     EllipsisChar;       // 2-4   // out // = '...'/'.'// Character used for ellipsis rendering.
    ImWchar                     FallbackChar;       // 2-4   // out // = FFFD/'?' // Character used if a glyph isn't found.
    float                       EllipsisWidth;      // 4     // out               // Width
    float                       EllipsisCharStep;   // 4     // out               // Step between characters when EllipsisCount > 0
    bool                        DirtyLookupTables;  // 1     // out //
    float                       Scale;              // 4     // in  // = 1.f      // Base font scale, multiplied by the per-window font scale which you can adjust with SetWindowFontScale()
    float                       Ascent, Descent;    // 4+4   // out //            // Ascent: distance from top to bottom of e.g. 'A' [0..FontSize] (unscaled)
    int                         MetricsTotalSurface;// 4     // out //            // Total surface in pixels to get an idea of the font rasterization/texture cost (not exact, we approximate the cost of padding between glyphs)
    ImU8                        Used4kPagesMap[(IM_UNICODE_CODEPOINT_MAX+1)/4096/8]; // 2 bytes if ImWchar=ImWchar16, 34 bytes if ImWchar==ImWchar32. Store 1-bit for each block of 4K codepoints that has one active glyph. This is mainly used to facilitate iterations across all used codepoints.

    // Methods
    IMGUI_API ImFont();
    IMGUI_API ~ImFont();
    IMGUI_API const ImFontGlyph*FindGlyph(ImWchar c);
    IMGUI_API const ImFontGlyph*FindGlyphNoFallback(ImWchar c);
    float                       GetCharAdvance(ImWchar c)           { return ((int)c < IndexAdvanceX.Size) ? IndexAdvanceX[(int)c] : FallbackAdvanceX; }
    bool                        IsLoaded() const                    { return ContainerAtlas != NULL; }
    const char*                 GetDebugName() const                { return ConfigData ? ConfigData->Name : "<unknown>"; }

    // 'max_width' stops rendering after a certain width (could be turned into a 2d size). FLT_MAX to disable.
    // 'wrap_width' enable automatic word-wrapping across multiple lines to fit into given width. 0.0f to disable.
    IMGUI_API ImVec2            CalcTextSizeA(float size, float max_width, float wrap_width, const char* text_begin, const char* text_end = NULL, const char** remaining = NULL); // utf8
    IMGUI_API const char*       CalcWordWrapPositionA(float scale, const char* text, const char* text_end, float wrap_width);
    IMGUI_API void              RenderChar(ImDrawList* draw_list, float size, const ImVec2& pos, ImU32 col, ImWchar c);
    IMGUI_API void              RenderText(ImDrawList* draw_list, float size, const ImVec2& pos, ImU32 col, const ImVec4& clip_rect, const char* text_begin, const char* text_end, float wrap_width = 0.0f, bool cpu_fine_clip = false);

    // [Internal] Don't use!
    IMGUI_API void              BuildLookupTable();
    IMGUI_API void              ClearOutputData();
    IMGUI_API void              GrowIndex(int new_size);
    IMGUI_API void              AddGlyph(const ImFontConfig* src_cfg, ImWchar c, float x0, float y0, float x1, float y1, float u0, float v0, float u1, float v1, float advance_x);
    IMGUI_API void              AddRemapChar(ImWchar dst, ImWchar src, bool overwrite_dst = true); // Makes 'dst' character/glyph points to 'src' character/glyph. Currently needs to be called AFTER fonts have been built.
    IMGUI_API void              SetGlyphVisible(ImWchar c, bool visible);
    IMGUI_API bool              IsGlyphRangeUnused(unsigned int c_begin, unsigned int c_last);
};

//-----------------------------------------------------------------------------
// [SECTION] Viewports
//-----------------------------------------------------------------------------

// Flags stored in ImGuiViewport::Flags, giving indications to the platform backends.
enum ImGuiViewportFlags_
{
    ImGuiViewportFlags_None                     = 0,
    ImGuiViewportFlags_IsPlatformWindow         = 1 << 0,   // Represent a Platform Window
    ImGuiViewportFlags_IsPlatformMonitor        = 1 << 1,   // Represent a Platform Monitor (unused yet)
    ImGuiViewportFlags_OwnedByApp               = 1 << 2,   // Platform Window: Is created/managed by the user application? (rather than our backend)
    ImGuiViewportFlags_NoDecoration             = 1 << 3,   // Platform Window: Disable platform decorations: title bar, borders, etc. (generally set all windows, but if ImGuiConfigFlags_ViewportsDecoration is set we only set this on popups/tooltips)
    ImGuiViewportFlags_NoTaskBarIcon            = 1 << 4,   // Platform Window: Disable platform task bar icon (generally set on popups/tooltips, or all windows if ImGuiConfigFlags_ViewportsNoTaskBarIcon is set)
    ImGuiViewportFlags_NoFocusOnAppearing       = 1 << 5,   // Platform Window: Don't take focus when created.
    ImGuiViewportFlags_NoFocusOnClick           = 1 << 6,   // Platform Window: Don't take focus when clicked on.
    ImGuiViewportFlags_NoInputs                 = 1 << 7,   // Platform Window: Make mouse pass through so we can drag this window while peaking behind it.
    ImGuiViewportFlags_NoRendererClear          = 1 << 8,   // Platform Window: Renderer doesn't need to clear the framebuffer ahead (because we will fill it entirely).
    ImGuiViewportFlags_NoAutoMerge              = 1 << 9,   // Platform Window: Avoid merging this window into another host window. This can only be set via ImGuiWindowClass viewport flags override (because we need to now ahead if we are going to create a viewport in the first place!).
    ImGuiViewportFlags_TopMost                  = 1 << 10,  // Platform Window: Display on top (for tooltips only).
    ImGuiViewportFlags_CanHostOtherWindows      = 1 << 11,  // Viewport can host multiple imgui windows (secondary viewports are associated to a single window). // FIXME: In practice there's still probably code making the assumption that this is always and only on the MainViewport. Will fix once we add support for "no main viewport".

    // Output status flags (from Platform)
    ImGuiViewportFlags_IsMinimized              = 1 << 12,  // Platform Window: Window is minimized, can skip render. When minimized we tend to avoid using the viewport pos/size for clipping window or testing if they are contained in the viewport.
    ImGuiViewportFlags_IsFocused                = 1 << 13,  // Platform Window: Window is focused (last call to Platform_GetWindowFocus() returned true)
};

// - Currently represents the Platform Window created by the application which is hosting our Dear ImGui windows.
// - With multi-viewport enabled, we extend this concept to have multiple active viewports.
// - In the future we will extend this concept further to also represent Platform Monitor and support a "no main platform window" operation mode.
// - About Main Area vs Work Area:
//   - Main Area = entire viewport.
//   - Work Area = entire viewport minus sections used by main menu bars (for platform windows), or by task bar (for platform monitor).
//   - Windows are generally trying to stay within the Work Area of their host viewport.
struct ImGuiViewport
{
    ImGuiID             ID;                     // Unique identifier for the viewport
    ImGuiViewportFlags  Flags;                  // See ImGuiViewportFlags_
    ImVec2              Pos;                    // Main Area: Position of the viewport (Dear ImGui coordinates are the same as OS desktop/native coordinates)
    ImVec2              Size;                   // Main Area: Size of the viewport.
    ImVec2              WorkPos;                // Work Area: Position of the viewport minus task bars, menus bars, status bars (>= Pos)
    ImVec2              WorkSize;               // Work Area: Size of the viewport minus task bars, menu bars, status bars (<= Size)
    float               DpiScale;               // 1.0f = 96 DPI = No extra scale.
    ImGuiID             ParentViewportId;       // (Advanced) 0: no parent. Instruct the platform backend to setup a parent/child relationship between platform windows.
    ImDrawData*         DrawData;               // The ImDrawData corresponding to this viewport. Valid after Render() and until the next call to NewFrame().

    // Platform/Backend Dependent Data
    // Our design separate the Renderer and Platform backends to facilitate combining default backends with each others.
    // When our create your own backend for a custom engine, it is possible that both Renderer and Platform will be handled
    // by the same system and you may not need to use all the UserData/Handle fields.
    // The library never uses those fields, they are merely storage to facilitate backend implementation.
    void*               RendererUserData;       // void* to hold custom data structure for the renderer (e.g. swap chain, framebuffers etc.). generally set by your Renderer_CreateWindow function.
    void*               PlatformUserData;       // void* to hold custom data structure for the OS / platform (e.g. windowing info, render context). generally set by your Platform_CreateWindow function.
    void*               PlatformHandle;         // void* to hold higher-level, platform window handle (e.g. HWND, GLFWWindow*, SDL_Window*), for FindViewportByPlatformHandle().
    void*               PlatformHandleRaw;      // void* to hold lower-level, platform-native window handle (under Win32 this is expected to be a HWND, unused for other platforms), when using an abstraction layer like GLFW or SDL (where PlatformHandle would be a SDL_Window*)
    bool                PlatformWindowCreated;  // Platform window has been created (Platform_CreateWindow() has been called). This is false during the first frame where a viewport is being created.
    bool                PlatformRequestMove;    // Platform window requested move (e.g. window was moved by the OS / host window manager, authoritative position will be OS window position)
    bool                PlatformRequestResize;  // Platform window requested resize (e.g. window was resized by the OS / host window manager, authoritative size will be OS window size)
    bool                PlatformRequestClose;   // Platform window requested closure (e.g. window was moved by the OS / host window manager, e.g. pressing ALT-F4)

    ImGuiViewport()     { memset(this, 0, sizeof(*this)); }
    ~ImGuiViewport()    { IM_ASSERT(PlatformUserData == NULL && RendererUserData == NULL); }

    // Helpers
    ImVec2              GetCenter() const       { return ImVec2(Pos.x + Size.x * 0.5f, Pos.y + Size.y * 0.5f); }
    ImVec2              GetWorkCenter() const   { return ImVec2(WorkPos.x + WorkSize.x * 0.5f, WorkPos.y + WorkSize.y * 0.5f); }
};

//-----------------------------------------------------------------------------
// [SECTION] ImGuiPlatformIO + other Platform Dependent Interfaces (ImGuiPlatformMonitor, ImGuiPlatformImeData)
//-----------------------------------------------------------------------------

// [BETA] (Optional) Multi-Viewport Support!
// If you are new to Dear ImGui and trying to integrate it into your engine, you can probably ignore this for now.
//
// This feature allows you to seamlessly drag Dear ImGui windows outside of your application viewport.
// This is achieved by creating new Platform/OS windows on the fly, and rendering into them.
// Dear ImGui manages the viewport structures, and the backend create and maintain one Platform/OS window for each of those viewports.
//
// See Recap:   https://github.com/ocornut/imgui/wiki/Multi-Viewports
// See Glossary https://github.com/ocornut/imgui/wiki/Glossary for details about some of the terminology.
//
// About the coordinates system:
// - When multi-viewports are enabled, all Dear ImGui coordinates become absolute coordinates (same as OS coordinates!)
// - So e.g. ImGui::SetNextWindowPos(ImVec2(0,0)) will position a window relative to your primary monitor!
// - If you want to position windows relative to your main application viewport, use ImGui::GetMainViewport()->Pos as a base position.
//
// Steps to use multi-viewports in your application, when using a default backend from the examples/ folder:
// - Application:  Enable feature with 'io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable'.
// - Backend:      The backend initialization will setup all necessary ImGuiPlatformIO's functions and update monitors info every frame.
// - Application:  In your main loop, call ImGui::UpdatePlatformWindows(), ImGui::RenderPlatformWindowsDefault() after EndFrame() or Render().
// - Application:  Fix absolute coordinates used in ImGui::SetWindowPos() or ImGui::SetNextWindowPos() calls.
//
// Steps to use multi-viewports in your application, when using a custom backend:
// - Important:    THIS IS NOT EASY TO DO and comes with many subtleties not described here!
//                 It's also an experimental feature, so some of the requirements may evolve.
//                 Consider using default backends if you can. Either way, carefully follow and refer to examples/ backends for details.
// - Application:  Enable feature with 'io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable'.
// - Backend:      Hook ImGuiPlatformIO's Platform_* and Renderer_* callbacks (see below).
//                 Set 'io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports' and 'io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports'.
//                 Update ImGuiPlatformIO's Monitors list every frame.
//                 Update MousePos every frame, in absolute coordinates.
// - Application:  In your main loop, call ImGui::UpdatePlatformWindows(), ImGui::RenderPlatformWindowsDefault() after EndFrame() or Render().
//                 You may skip calling RenderPlatformWindowsDefault() if its API is not convenient for your needs. Read comments below.
// - Application:  Fix absolute coordinates used in ImGui::SetWindowPos() or ImGui::SetNextWindowPos() calls.
//
// About ImGui::RenderPlatformWindowsDefault():
// - This function is a mostly a _helper_ for the common-most cases, and to facilitate using default backends.
// - You can check its simple source code to understand what it does.
//   It basically iterates secondary viewports and call 4 functions that are setup in ImGuiPlatformIO, if available:
//     Platform_RenderWindow(), Renderer_RenderWindow(), Platform_SwapBuffers(), Renderer_SwapBuffers()
//   Those functions pointers exists only for the benefit of RenderPlatformWindowsDefault().
// - If you have very specific rendering needs (e.g. flipping multiple swap-chain simultaneously, unusual sync/threading issues, etc.),
//   you may be tempted to ignore RenderPlatformWindowsDefault() and write customized code to perform your renderingg.
//   You may decide to setup the platform_io's *RenderWindow and *SwapBuffers pointers and call your functions through those pointers,
//   or you may decide to never setup those pointers and call your code directly. They are a convenience, not an obligatory interface.
//-----------------------------------------------------------------------------

// Access via ImGui::GetPlatformIO()
struct ImGuiPlatformIO
{
    IMGUI_API ImGuiPlatformIO();

    //------------------------------------------------------------------
    // Interface with OS and Platform backend (basic)
    //------------------------------------------------------------------

    // Optional: Access OS clipboard
    // (default to use native Win32 clipboard on Windows, otherwise uses a private clipboard. Override to access OS clipboard on other architectures)
    const char* (*Platform_GetClipboardTextFn)(ImGuiContext* ctx);
    void        (*Platform_SetClipboardTextFn)(ImGuiContext* ctx, const char* text);
    void*       Platform_ClipboardUserData;

    // Optional: Open link/folder/file in OS Shell
    // (default to use ShellExecuteA() on Windows, system() on Linux/Mac)
    bool        (*Platform_OpenInShellFn)(ImGuiContext* ctx, const char* path);
    void*       Platform_OpenInShellUserData;

    // Optional: Notify OS Input Method Editor of the screen position of your cursor for text input position (e.g. when using Japanese/Chinese IME on Windows)
    // (default to use native imm32 api on Windows)
    void        (*Platform_SetImeDataFn)(ImGuiContext* ctx, ImGuiViewport* viewport, ImGuiPlatformImeData* data);
    void*       Platform_ImeUserData;
    //void      (*SetPlatformImeDataFn)(ImGuiViewport* viewport, ImGuiPlatformImeData* data); // [Renamed to platform_io.PlatformSetImeDataFn in 1.91.1]

    // Optional: Platform locale
    // [Experimental] Configure decimal point e.g. '.' or ',' useful for some languages (e.g. German), generally pulled from *localeconv()->decimal_point
    ImWchar     Platform_LocaleDecimalPoint;     // '.'

    //------------------------------------------------------------------
    // Interface with Renderer Backend
    //------------------------------------------------------------------

    // Written by some backends during ImGui_ImplXXXX_RenderDrawData() call to point backend_specific ImGui_ImplXXXX_RenderState* structure.
    void*       Renderer_RenderState;

    //------------------------------------------------------------------
    // Input - Interface with OS/backends (Multi-Viewport support!)
    //------------------------------------------------------------------

    // For reference, the second column shows which function are generally calling the Platform Functions:
    //   N = ImGui::NewFrame()                        ~ beginning of the dear imgui frame: read info from platform/OS windows (latest size/position)
    //   F = ImGui::Begin(), ImGui::EndFrame()        ~ during the dear imgui frame
    //   U = ImGui::UpdatePlatformWindows()           ~ after the dear imgui frame: create and update all platform/OS windows
    //   R = ImGui::RenderPlatformWindowsDefault()    ~ render
    //   D = ImGui::DestroyPlatformWindows()          ~ shutdown
    // The general idea is that NewFrame() we will read the current Platform/OS state, and UpdatePlatformWindows() will write to it.

    // The handlers are designed so we can mix and match two imgui_impl_xxxx files, one Platform backend and one Renderer backend.
    // Custom engine backends will often provide both Platform and Renderer interfaces together and so may not need to use all functions.
    // Platform functions are typically called _before_ their Renderer counterpart, apart from Destroy which are called the other way.

    // Platform Backend functions (e.g. Win32, GLFW, SDL) ------------------- Called by -----
    void    (*Platform_CreateWindow)(ImGuiViewport* vp);                    // . . U . .  // Create a new platform window for the given viewport
    void    (*Platform_DestroyWindow)(ImGuiViewport* vp);                   // N . U . D  //
    void    (*Platform_ShowWindow)(ImGuiViewport* vp);                      // . . U . .  // Newly created windows are initially hidden so SetWindowPos/Size/Title can be called on them before showing the window
    void    (*Platform_SetWindowPos)(ImGuiViewport* vp, ImVec2 pos);        // . . U . .  // Set platform window position (given the upper-left corner of client area)
    ImVec2  (*Platform_GetWindowPos)(ImGuiViewport* vp);                    // N . . . .  //
    void    (*Platform_SetWindowSize)(ImGuiViewport* vp, ImVec2 size);      // . . U . .  // Set platform window client area size (ignoring OS decorations such as OS title bar etc.)
    ImVec2  (*Platform_GetWindowSize)(ImGuiViewport* vp);                   // N . . . .  // Get platform window client area size
    void    (*Platform_SetWindowFocus)(ImGuiViewport* vp);                  // N . . . .  // Move window to front and set input focus
    bool    (*Platform_GetWindowFocus)(ImGuiViewport* vp);                  // . . U . .  //
    bool    (*Platform_GetWindowMinimized)(ImGuiViewport* vp);              // N . . . .  // Get platform window minimized state. When minimized, we generally won't attempt to get/set size and contents will be culled more easily
    void    (*Platform_SetWindowTitle)(ImGuiViewport* vp, const char* str); // . . U . .  // Set platform window title (given an UTF-8 string)
    void    (*Platform_SetWindowAlpha)(ImGuiViewport* vp, float alpha);     // . . U . .  // (Optional) Setup global transparency (not per-pixel transparency)
    void    (*Platform_UpdateWindow)(ImGuiViewport* vp);                    // . . U . .  // (Optional) Called by UpdatePlatformWindows(). Optional hook to allow the platform backend from doing general book-keeping every frame.
    void    (*Platform_RenderWindow)(ImGuiViewport* vp, void* render_arg);  // . . . R .  // (Optional) Main rendering (platform side! This is often unused, or just setting a "current" context for OpenGL bindings). 'render_arg' is the value passed to RenderPlatformWindowsDefault().
    void    (*Platform_SwapBuffers)(ImGuiViewport* vp, void* render_arg);   // . . . R .  // (Optional) Call Present/SwapBuffers (platform side! This is often unused!). 'render_arg' is the value passed to RenderPlatformWindowsDefault().
    float   (*Platform_GetWindowDpiScale)(ImGuiViewport* vp);               // N . . . .  // (Optional) [BETA] FIXME-DPI: DPI handling: Return DPI scale for this viewport. 1.0f = 96 DPI.
    void    (*Platform_OnChangedViewport)(ImGuiViewport* vp);               // . F . . .  // (Optional) [BETA] FIXME-DPI: DPI handling: Called during Begin() every time the viewport we are outputting into changes, so backend has a chance to swap fonts to adjust style.
    ImVec4  (*Platform_GetWindowWorkAreaInsets)(ImGuiViewport* vp);         // N . . . .  // (Optional) [BETA] Get initial work area inset for the viewport (won't be covered by main menu bar, dockspace over viewport etc.). Default to (0,0),(0,0). 'safeAreaInsets' in iOS land, 'DisplayCutout' in Android land.
    int     (*Platform_CreateVkSurface)(ImGuiViewport* vp, ImU64 vk_inst, const void* vk_allocators, ImU64* out_vk_surface); // (Optional) For a Vulkan Renderer to call into Platform code (since the surface creation needs to tie them both).

    // Renderer Backend functions (e.g. DirectX, OpenGL, Vulkan) ------------ Called by -----
    void    (*Renderer_CreateWindow)(ImGuiViewport* vp);                    // . . U . .  // Create swap chain, frame buffers etc. (called after Platform_CreateWindow)
    void    (*Renderer_DestroyWindow)(ImGuiViewport* vp);                   // N . U . D  // Destroy swap chain, frame buffers etc. (called before Platform_DestroyWindow)
    void    (*Renderer_SetWindowSize)(ImGuiViewport* vp, ImVec2 size);      // . . U . .  // Resize swap chain, frame buffers etc. (called after Platform_SetWindowSize)
    void    (*Renderer_RenderWindow)(ImGuiViewport* vp, void* render_arg);  // . . . R .  // (Optional) Clear framebuffer, setup render target, then render the viewport->DrawData. 'render_arg' is the value passed to RenderPlatformWindowsDefault().
    void    (*Renderer_SwapBuffers)(ImGuiViewport* vp, void* render_arg);   // . . . R .  // (Optional) Call Present/SwapBuffers. 'render_arg' is the value passed to RenderPlatformWindowsDefault().

    // (Optional) Monitor list
    // - Updated by: app/backend. Update every frame to dynamically support changing monitor or DPI configuration.
    // - Used by: dear imgui to query DPI info, clamp popups/tooltips within same monitor and not have them straddle monitors.
    ImVector<ImGuiPlatformMonitor>  Monitors;

    //------------------------------------------------------------------
    // Output - List of viewports to render into platform windows
    //------------------------------------------------------------------

    // Viewports list (the list is updated by calling ImGui::EndFrame or ImGui::Render)
    // (in the future we will attempt to organize this feature to remove the need for a "main viewport")
    ImVector<ImGuiViewport*>        Viewports;                              // Main viewports, followed by all secondary viewports.
};

// (Optional) This is required when enabling multi-viewport. Represent the bounds of each connected monitor/display and their DPI.
// We use this information for multiple DPI support + clamping the position of popups and tooltips so they don't straddle multiple monitors.
struct ImGuiPlatformMonitor
{
    ImVec2  MainPos, MainSize;      // Coordinates of the area displayed on this monitor (Min = upper left, Max = bottom right)
    ImVec2  WorkPos, WorkSize;      // Coordinates without task bars / side bars / menu bars. Used to avoid positioning popups/tooltips inside this region. If you don't have this info, please copy the value for MainPos/MainSize.
    float   DpiScale;               // 1.0f = 96 DPI
    void*   PlatformHandle;         // Backend dependant data (e.g. HMONITOR, GLFWmonitor*, SDL Display Index, NSScreen*)
    ImGuiPlatformMonitor()          { MainPos = MainSize = WorkPos = WorkSize = ImVec2(0, 0); DpiScale = 1.0f; PlatformHandle = NULL; }
};

// (Optional) Support for IME (Input Method Editor) via the platform_io.Platform_SetImeDataFn() function.
struct ImGuiPlatformImeData
{
    bool    WantVisible;        // A widget wants the IME to be visible
    ImVec2  InputPos;           // Position of the input cursor
    float   InputLineHeight;    // Line height

    ImGuiPlatformImeData() { memset(this, 0, sizeof(*this)); }
};

//-----------------------------------------------------------------------------
// [SECTION] Obsolete functions and types
// (Will be removed! Read 'API BREAKING CHANGES' section in imgui.cpp for details)
// Please keep your copy of dear imgui up to date! Occasionally set '#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS' in imconfig.h to stay ahead.
//-----------------------------------------------------------------------------

#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
namespace ImGui
{
    // OBSOLETED in 1.91.0 (from July 2024)
    static inline void  PushButtonRepeat(bool repeat)                           { PushItemFlag(ImGuiItemFlags_ButtonRepeat, repeat); }
    static inline void  PopButtonRepeat()                                       { PopItemFlag(); }
    static inline void  PushTabStop(bool tab_stop)                              { PushItemFlag(ImGuiItemFlags_NoTabStop, !tab_stop); }
    static inline void  PopTabStop()                                            { PopItemFlag(); }
    IMGUI_API ImVec2    GetContentRegionMax();                                  // Content boundaries max (e.g. window boundaries including scrolling, or current column boundaries). You should never need this. Always use GetCursorScreenPos() and GetContentRegionAvail()!
    IMGUI_API ImVec2    GetWindowContentRegionMin();                            // Content boundaries min for the window (roughly (0,0)-Scroll), in window-local coordinates. You should never need this. Always use GetCursorScreenPos() and GetContentRegionAvail()!
    IMGUI_API ImVec2    GetWindowContentRegionMax();                            // Content boundaries max for the window (roughly (0,0)+Size-Scroll), in window-local coordinates. You should never need this. Always use GetCursorScreenPos() and GetContentRegionAvail()!
    // OBSOLETED in 1.90.0 (from September 2023)
    static inline bool  BeginChildFrame(ImGuiID id, const ImVec2& size, ImGuiWindowFlags window_flags = 0)  { return BeginChild(id, size, ImGuiChildFlags_FrameStyle, window_flags); }
    static inline void  EndChildFrame()                                                                     { EndChild(); }
    //static inline bool BeginChild(const char* str_id, const ImVec2& size_arg, bool borders, ImGuiWindowFlags window_flags){ return BeginChild(str_id, size_arg, borders ? ImGuiChildFlags_Borders : ImGuiChildFlags_None, window_flags); } // Unnecessary as true == ImGuiChildFlags_Borders
    //static inline bool BeginChild(ImGuiID id, const ImVec2& size_arg, bool borders, ImGuiWindowFlags window_flags)        { return BeginChild(id, size_arg, borders ? ImGuiChildFlags_Borders : ImGuiChildFlags_None, window_flags);     } // Unnecessary as true == ImGuiChildFlags_Borders
    static inline void  ShowStackToolWindow(bool* p_open = NULL)                { ShowIDStackToolWindow(p_open); }
    IMGUI_API bool      Combo(const char* label, int* current_item, bool (*old_callback)(void* user_data, int idx, const char** out_text), void* user_data, int items_count, int popup_max_height_in_items = -1);
    IMGUI_API bool      ListBox(const char* label, int* current_item, bool (*old_callback)(void* user_data, int idx, const char** out_text), void* user_data, int items_count, int height_in_items = -1);
    // OBSOLETED in 1.89.7 (from June 2023)
    IMGUI_API void      SetItemAllowOverlap();                                  // Use SetNextItemAllowOverlap() before item.
    // OBSOLETED in 1.89.4 (from March 2023)
    static inline void  PushAllowKeyboardFocus(bool tab_stop)                   { PushItemFlag(ImGuiItemFlags_NoTabStop, !tab_stop); }
    static inline void  PopAllowKeyboardFocus()                                 { PopItemFlag(); }

    // Some of the older obsolete names along with their replacement (commented out so they are not reported in IDE)
    //-- OBSOLETED in 1.89 (from August 2022)
    //IMGUI_API bool      ImageButton(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1)); // --> Use new ImageButton() signature (explicit item id, regular FramePadding). Refer to code in 1.91 if you want to grab a copy of this version.
    //-- OBSOLETED in 1.88 (from May 2022)
    //static inline void  CaptureKeyboardFromApp(bool want_capture_keyboard = true)                   { SetNextFrameWantCaptureKeyboard(want_capture_keyboard); } // Renamed as name was misleading + removed default value.
    //static inline void  CaptureMouseFromApp(bool want_capture_mouse = true)                         { SetNextFrameWantCaptureMouse(want_capture_mouse); }       // Renamed as name was misleading + removed default value.
    //-- OBSOLETED in 1.87 (from February 2022, more formally obsoleted April 2024)
    //IMGUI_API ImGuiKey  GetKeyIndex(ImGuiKey key);                                                  { IM_ASSERT(key >= ImGuiKey_NamedKey_BEGIN && key < ImGuiKey_NamedKey_END); const ImGuiKeyData* key_data = GetKeyData(key); return (ImGuiKey)(key_data - g.IO.KeysData); } // Map ImGuiKey_* values into legacy native key index. == io.KeyMap[key]. When using a 1.87+ backend using io.AddKeyEvent(), calling GetKeyIndex() with ANY ImGuiKey_XXXX values will return the same value!
    //static inline ImGuiKey GetKeyIndex(ImGuiKey key)                                                { IM_ASSERT(key >= ImGuiKey_NamedKey_BEGIN && key < ImGuiKey_NamedKey_END); return key; }
    //-- OBSOLETED in 1.86 (from November 2021)
    //IMGUI_API void      CalcListClipping(int items_count, float items_height, int* out_items_display_start, int* out_items_display_end); // Code removed, see 1.90 for last version of the code. Calculate range of visible items for large list of evenly sized items. Prefer using ImGuiListClipper.
    //-- OBSOLETED in 1.85 (from August 2021)
    //static inline float GetWindowContentRegionWidth()                                               { return GetWindowContentRegionMax().x - GetWindowContentRegionMin().x; }
    //-- OBSOLETED in 1.81 (from February 2021)
    //static inline bool  ListBoxHeader(const char* label, const ImVec2& size = ImVec2(0, 0))         { return BeginListBox(label, size); }
    //static inline bool  ListBoxHeader(const char* label, int items_count, int height_in_items = -1) { float height = GetTextLineHeightWithSpacing() * ((height_in_items < 0 ? ImMin(items_count, 7) : height_in_items) + 0.25f) + GetStyle().FramePadding.y * 2.0f; return BeginListBox(label, ImVec2(0.0f, height)); } // Helper to calculate size from items_count and height_in_items
    //static inline void  ListBoxFooter()                                                             { EndListBox(); }
    //-- OBSOLETED in 1.79 (from August 2020)
    //static inline void  OpenPopupContextItem(const char* str_id = NULL, ImGuiMouseButton mb = 1)    { OpenPopupOnItemClick(str_id, mb); } // Bool return value removed. Use IsWindowAppearing() in BeginPopup() instead. Renamed in 1.77, renamed back in 1.79. Sorry!
    //-- OBSOLETED in 1.78 (from June 2020): Old drag/sliders functions that took a 'float power > 1.0f' argument instead of ImGuiSliderFlags_Logarithmic. See github.com/ocornut/imgui/issues/3361 for details.
    //IMGUI_API bool      DragScalar(const char* label, ImGuiDataType data_type, void* p_data, float v_speed, const void* p_min, const void* p_max, const char* format, float power = 1.0f)                                                            // OBSOLETED in 1.78 (from June 2020)
    //IMGUI_API bool      DragScalarN(const char* label, ImGuiDataType data_type, void* p_data, int components, float v_speed, const void* p_min, const void* p_max, const char* format, float power = 1.0f);                                          // OBSOLETED in 1.78 (from June 2020)
    //IMGUI_API bool      SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, float power = 1.0f);                                                                        // OBSOLETED in 1.78 (from June 2020)
    //IMGUI_API bool      SliderScalarN(const char* label, ImGuiDataType data_type, void* p_data, int components, const void* p_min, const void* p_max, const char* format, float power = 1.0f);                                                       // OBSOLETED in 1.78 (from June 2020)
    //static inline bool  DragFloat(const char* label, float* v, float v_speed, float v_min, float v_max, const char* format, float power = 1.0f)    { return DragScalar(label, ImGuiDataType_Float, v, v_speed, &v_min, &v_max, format, power); }     // OBSOLETED in 1.78 (from June 2020)
    //static inline bool  DragFloat2(const char* label, float v[2], float v_speed, float v_min, float v_max, const char* format, float power = 1.0f) { return DragScalarN(label, ImGuiDataType_Float, v, 2, v_speed, &v_min, &v_max, format, power); } // OBSOLETED in 1.78 (from June 2020)
    //static inline bool  DragFloat3(const char* label, float v[3], float v_speed, float v_min, float v_max, const char* format, float power = 1.0f) { return DragScalarN(label, ImGuiDataType_Float, v, 3, v_speed, &v_min, &v_max, format, power); } // OBSOLETED in 1.78 (from June 2020)
    //static inline bool  DragFloat4(const char* label, float v[4], float v_speed, float v_min, float v_max, const char* format, float power = 1.0f) { return DragScalarN(label, ImGuiDataType_Float, v, 4, v_speed, &v_min, &v_max, format, power); } // OBSOLETED in 1.78 (from June 2020)
    //static inline bool  SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, float power = 1.0f)                 { return SliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format, power); }            // OBSOLETED in 1.78 (from June 2020)
    //static inline bool  SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* format, float power = 1.0f)              { return SliderScalarN(label, ImGuiDataType_Float, v, 2, &v_min, &v_max, format, power); }        // OBSOLETED in 1.78 (from June 2020)
    //static inline bool  SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* format, float power = 1.0f)              { return SliderScalarN(label, ImGuiDataType_Float, v, 3, &v_min, &v_max, format, power); }        // OBSOLETED in 1.78 (from June 2020)
    //static inline bool  SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* format, float power = 1.0f)              { return SliderScalarN(label, ImGuiDataType_Float, v, 4, &v_min, &v_max, format, power); }        // OBSOLETED in 1.78 (from June 2020)
    //-- OBSOLETED in 1.77 and before
    //static inline bool  BeginPopupContextWindow(const char* str_id, ImGuiMouseButton mb, bool over_items) { return BeginPopupContextWindow(str_id, mb | (over_items ? 0 : ImGuiPopupFlags_NoOpenOverItems)); } // OBSOLETED in 1.77 (from June 2020)
    //static inline void  TreeAdvanceToLabelPos()               { SetCursorPosX(GetCursorPosX() + GetTreeNodeToLabelSpacing()); }   // OBSOLETED in 1.72 (from July 2019)
    //static inline void  SetNextTreeNodeOpen(bool open, ImGuiCond cond = 0) { SetNextItemOpen(open, cond); }                       // OBSOLETED in 1.71 (from June 2019)
    //static inline float GetContentRegionAvailWidth()          { return GetContentRegionAvail().x; }                               // OBSOLETED in 1.70 (from May 2019)
    //static inline ImDrawList* GetOverlayDrawList()            { return GetForegroundDrawList(); }                                 // OBSOLETED in 1.69 (from Mar 2019)
    //static inline void  SetScrollHere(float ratio = 0.5f)     { SetScrollHereY(ratio); }                                          // OBSOLETED in 1.66 (from Nov 2018)
    //static inline bool  IsItemDeactivatedAfterChange()        { return IsItemDeactivatedAfterEdit(); }                            // OBSOLETED in 1.63 (from Aug 2018)
    //-- OBSOLETED in 1.60 and before
    //static inline bool  IsAnyWindowFocused()                  { return IsWindowFocused(ImGuiFocusedFlags_AnyWindow); }            // OBSOLETED in 1.60 (from Apr 2018)
    //static inline bool  IsAnyWindowHovered()                  { return IsWindowHovered(ImGuiHoveredFlags_AnyWindow); }            // OBSOLETED in 1.60 (between Dec 2017 and Apr 2018)
    //static inline void  ShowTestWindow()                      { return ShowDemoWindow(); }                                        // OBSOLETED in 1.53 (between Oct 2017 and Dec 2017)
    //static inline bool  IsRootWindowFocused()                 { return IsWindowFocused(ImGuiFocusedFlags_RootWindow); }           // OBSOLETED in 1.53 (between Oct 2017 and Dec 2017)
    //static inline bool  IsRootWindowOrAnyChildFocused()       { return IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows); }  // OBSOLETED in 1.53 (between Oct 2017 and Dec 2017)
    //static inline void  SetNextWindowContentWidth(float w)    { SetNextWindowContentSize(ImVec2(w, 0.0f)); }                      // OBSOLETED in 1.53 (between Oct 2017 and Dec 2017)
    //static inline float GetItemsLineHeightWithSpacing()       { return GetFrameHeightWithSpacing(); }                             // OBSOLETED in 1.53 (between Oct 2017 and Dec 2017)
    //IMGUI_API bool      Begin(char* name, bool* p_open, ImVec2 size_first_use, float bg_alpha = -1.0f, ImGuiWindowFlags flags=0); // OBSOLETED in 1.52 (between Aug 2017 and Oct 2017): Equivalent of using SetNextWindowSize(size, ImGuiCond_FirstUseEver) and SetNextWindowBgAlpha().
    //static inline bool  IsRootWindowOrAnyChildHovered()       { return IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows); }  // OBSOLETED in 1.52 (between Aug 2017 and Oct 2017)
    //static inline void  AlignFirstTextHeightToWidgets()       { AlignTextToFramePadding(); }                                      // OBSOLETED in 1.52 (between Aug 2017 and Oct 2017)
    //static inline void  SetNextWindowPosCenter(ImGuiCond c=0) { SetNextWindowPos(GetMainViewport()->GetCenter(), c, ImVec2(0.5f,0.5f)); } // OBSOLETED in 1.52 (between Aug 2017 and Oct 2017)
    //static inline bool  IsItemHoveredRect()                   { return IsItemHovered(ImGuiHoveredFlags_RectOnly); }               // OBSOLETED in 1.51 (between Jun 2017 and Aug 2017)
    //static inline bool  IsPosHoveringAnyWindow(const ImVec2&) { IM_ASSERT(0); return false; }                                     // OBSOLETED in 1.51 (between Jun 2017 and Aug 2017): This was misleading and partly broken. You probably want to use the io.WantCaptureMouse flag instead.
    //static inline bool  IsMouseHoveringAnyWindow()            { return IsWindowHovered(ImGuiHoveredFlags_AnyWindow); }            // OBSOLETED in 1.51 (between Jun 2017 and Aug 2017)
    //static inline bool  IsMouseHoveringWindow()               { return IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem); }       // OBSOLETED in 1.51 (between Jun 2017 and Aug 2017)
    //-- OBSOLETED in 1.50 and before
    //static inline bool  CollapsingHeader(char* label, const char* str_id, bool framed = true, bool default_open = false) { return CollapsingHeader(label, (default_open ? (1 << 5) : 0)); } // OBSOLETED in 1.49
    //static inline ImFont*GetWindowFont()                      { return GetFont(); }                                               // OBSOLETED in 1.48
    //static inline float GetWindowFontSize()                   { return GetFontSize(); }                                           // OBSOLETED in 1.48
    //static inline void  SetScrollPosHere()                    { SetScrollHere(); }                                                // OBSOLETED in 1.42
}

//-- OBSOLETED in 1.82 (from Mars 2021): flags for AddRect(), AddRectFilled(), AddImageRounded(), PathRect()
//typedef ImDrawFlags ImDrawCornerFlags;
//enum ImDrawCornerFlags_
//{
//    ImDrawCornerFlags_None      = ImDrawFlags_RoundCornersNone,         // Was == 0 prior to 1.82, this is now == ImDrawFlags_RoundCornersNone which is != 0 and not implicit
//    ImDrawCornerFlags_TopLeft   = ImDrawFlags_RoundCornersTopLeft,      // Was == 0x01 (1 << 0) prior to 1.82. Order matches ImDrawFlags_NoRoundCorner* flag (we exploit this internally).
//    ImDrawCornerFlags_TopRight  = ImDrawFlags_RoundCornersTopRight,     // Was == 0x02 (1 << 1) prior to 1.82.
//    ImDrawCornerFlags_BotLeft   = ImDrawFlags_RoundCornersBottomLeft,   // Was == 0x04 (1 << 2) prior to 1.82.
//    ImDrawCornerFlags_BotRight  = ImDrawFlags_RoundCornersBottomRight,  // Was == 0x08 (1 << 3) prior to 1.82.
//    ImDrawCornerFlags_All       = ImDrawFlags_RoundCornersAll,          // Was == 0x0F prior to 1.82
//    ImDrawCornerFlags_Top       = ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_TopRight,
//    ImDrawCornerFlags_Bot       = ImDrawCornerFlags_BotLeft | ImDrawCornerFlags_BotRight,
//    ImDrawCornerFlags_Left      = ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_BotLeft,
//    ImDrawCornerFlags_Right     = ImDrawCornerFlags_TopRight | ImDrawCornerFlags_BotRight,
//};

// RENAMED and MERGED both ImGuiKey_ModXXX and ImGuiModFlags_XXX into ImGuiMod_XXX (from September 2022)
// RENAMED ImGuiKeyModFlags -> ImGuiModFlags in 1.88 (from April 2022). Exceptionally commented out ahead of obscolescence schedule to reduce confusion and because they were not meant to be used in the first place.
//typedef ImGuiKeyChord ImGuiModFlags;      // == int. We generally use ImGuiKeyChord to mean "a ImGuiKey or-ed with any number of ImGuiMod_XXX value", so you may store mods in there.
//enum ImGuiModFlags_ { ImGuiModFlags_None = 0, ImGuiModFlags_Ctrl = ImGuiMod_Ctrl, ImGuiModFlags_Shift = ImGuiMod_Shift, ImGuiModFlags_Alt = ImGuiMod_Alt, ImGuiModFlags_Super = ImGuiMod_Super };
//typedef ImGuiKeyChord ImGuiKeyModFlags; // == int
//enum ImGuiKeyModFlags_ { ImGuiKeyModFlags_None = 0, ImGuiKeyModFlags_Ctrl = ImGuiMod_Ctrl, ImGuiKeyModFlags_Shift = ImGuiMod_Shift, ImGuiKeyModFlags_Alt = ImGuiMod_Alt, ImGuiKeyModFlags_Super = ImGuiMod_Super };

#define IM_OFFSETOF(_TYPE,_MEMBER)  offsetof(_TYPE, _MEMBER)    // OBSOLETED IN 1.90 (now using C++11 standard version)

#endif // #ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS

// RENAMED IMGUI_DISABLE_METRICS_WINDOW > IMGUI_DISABLE_DEBUG_TOOLS in 1.88 (from June 2022)
#ifdef IMGUI_DISABLE_METRICS_WINDOW
#error IMGUI_DISABLE_METRICS_WINDOW was renamed to IMGUI_DISABLE_DEBUG_TOOLS, please use new name.
#endif

//-----------------------------------------------------------------------------

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning (pop)
#endif

// Include imgui_user.h at the end of imgui.h
// May be convenient for some users to only explicitly include vanilla imgui.h and have extra stuff included.
#ifdef IMGUI_INCLUDE_IMGUI_USER_H
#ifdef IMGUI_USER_H_FILENAME
#include IMGUI_USER_H_FILENAME
#else
#include "imgui_user.h"
#endif
#endif

#endif // #ifndef IMGUI_DISABLE
