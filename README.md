## Using a JavaScript Charting Library in a C++ wxWidgets Application

### Background

To the best of my knowledge, there isn't a single wxWidgets charting library that meets all the following criteria: (1) free with a permissive license, (2) a large user and knowledge base, (3) feature-rich and interactive, and (4) actively maintained. This is not surprising, as it appears that creating and maintaining such library is quite challenging if not outright impossible.

On the other hand, there are JavaScript charting libraries that meet all four criteria, among else Plotly.js, Chart.js, or Apache ECharts. These libraries are also generally quite easy to use. wxWidgets provides a `wxWebView` control, which wraps a full web browser core, allowing these charting libraries to be used in a wxWidgets application.

While using `wxWebView` and a JavaScript library in a native C++ application might seem crazy to some, and certainly has some disadvantages, it may still be a viable approach for others. The wxECharts project presents a simple example of how to do this using [Apache ECharts](https://echarts.apache.org). wxECharts does not provide a ready-to-use code for applications, it is not a JavaScript library wrapper for wxWidgets. Rather, it demonstrates how certain tasks can be approached. The actual implementation will depend on the charting library used and the application's requirements. wxECharts also shows how to access the native `wxWebView` backend, to customize it and display very handy DevTools.

![wxECharts Screenshot](wxecharts-screenshot.gif?raw=true)

### wxECharts

The project consists of several C++ header and source files, with `charthelper.cpp` being the most important one. It handles almost all direct communication between the chart and the C++ code. The project also includes chart assets (see the `chart-assets` folder), which consist of an HTML file for `wxWebView` (`chart.html`), a JavaScript file for two-way communication between the C++ and JavaScript code (`chartscripts.js`), and the charting library itself (`echarts.min.js`).

#### Main Objectives

1. **Communicate with the chart from C++ code** (e.g., to change data or visuals).

2. **Communicate with C++ code from the chart** (e.g., when a chart data point is double-clicked).

3. **Export the chart** (so it can be saved or printed).

#### Communicating with the Chart from C++ Code

This is accomplished using `wxWebView::RunScript()`, where the C++ code asks JavaScript code to either (a) create or modify the chart or (b) query the chart for certain information.

#### Communicating with C++ Code from the Chart

The C++ code registers a message handler with `wxWebView::AddScriptMessageHandler("wxmsg")` and then processes `wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED` events sent by JavaScript with `window.wxmsg.postMessage()`.

#### Exporting the Chart

JavaScript charting libraries usually also provide the chart rendered as PNG and SVG. Since there is little that can be done with a non-trivial SVG in wxWidgets, PNG generally seems the better choice. While vector format would be preferable, a bitmap saved at a sufficiently high resolution should be adequate for most scenarios.

#### Platforms

Tested on Windows (10, 11) and (briefly) on Linux (Mint 22 Cinnamon). See BUILD.md for build instructions.

### Downsides of Using `wxWebView` for Charting

Using `wxWebView` adds a large dependency to the application, increasing the application’s startup time and memory footprint. However, on modern systems, neither may be noticeable, let alone the deal-breaker. Another potential downside is sometimes not-so-seamless integration of `wxWebView` into the application; for an example, see the "Known Issues" section.

### Known issues

When focused, `wxWebViewEdge` consumes keyboard shortcuts from the parent frame, see [wxWidgets Issue #24786](https://github.com/wxWidgets/wxWidgets/issues/24786).