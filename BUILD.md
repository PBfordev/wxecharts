## Building wxECharts

### Build requirements

* wxWidgets 3.2.0 or newer built with support for `wxWebView`, on Windows with `wxWebViewEdge` [enabled](https://docs.wxwidgets.org/stable/classwx_web_view.html#wxWEBVIEW_BACKEND_EDGE).
* CMake 3.24 or newer.
* Apache ECharts and [JSON for Modern C++](https://json.nlohmann.me/), both are bundled with the project.

### How to Build

Build as any other wxWidgets application with CMake, CMake's `find_package()` will be used to determine wxWidgets location.

#### Windows
For simplicity sake, only x64 build is supported. webview2 package folder must be made available to CMake using variable `WEBVIEW2_FOLDER`, for example
```
cmake -G "Visual Studio 17 2022" -DWEBVIEW2_FOLDER=D:\Dev\Desktop\Lib\webview2 <...more parameters...>
```
The package folder is usually `<WXWIDGETS_SRC_FOLDER>/3rdparty/webview2`, or if it was downloaded with CMake, it is in the CMake build folder,  `<BUILD_FOLDER>libs/webview/packages/Microsoft.Web.WebView2.<version>`.

#### Linux
CMake's `pkg_check_modules()` will be used to add `gio-2.0` and `webkit2gtk-4.0` or `webkit2gtk-4.1` packages.