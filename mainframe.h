///////////////////////////////////////////////////////////////////////////////
// Project:     wxECharts
// Home:        https://github.com/PBfordev/wxecharts
// File Name:   mainframe.h
// Purpose:     Declaration of application's main frame
// Author:      PB
// Created:     2024-08-22
// Copyright:   (c) 2024 PB
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/frame.h>

#include "charthelper.h"

#if !wxUSE_WEBVIEW
  #error "wxWidgets must be built with a support for wxWebView"
#endif

#ifdef __WXMSW__
    #if !wxUSE_WEBVIEW_EDGE
        #error "On Windows, wxUSE_WEBVIEW_EDGE must be set to 1 (or ON) when building wxWidgets"
    #endif// !wxUSE_WEBVIEW_EDGE
    #define USING_WEBVIEW_EDGE 1
#else // #ifdef __WXMSW__
    #define USING_WEBVIEW_EDGE 0
#endif

class wxArrayString;
class wxGrid;
class wxGridEvent;
class wxWebView;
class wxWebViewEvent;

class wxEChartsMainFrame : public wxFrame
{
public:
    wxEChartsMainFrame(wxWindow* parent, const wxString& chartAssetsFolder);
private:
    enum
    {
        ID_CHART_COLORS = wxID_HIGHEST + 10,
        ID_CHART_SIZING_OPTIONS,
        ID_SHOW_DEVTOOLS,
    };

    ChartHelper m_chartHelper;
    wxGrid* m_grid{nullptr};
    wxWebView* m_webView{nullptr};
    bool m_webViewConfigured{false};
    wxString m_webViewBackend;

    void InitChartData();

    void CreateGrid(wxWindow* parent);
    void CreateWebView(wxWindow* parent, const wxString& assetsFolder);
    void ConfigureWebView();

    void OnGridCellChanging(wxGridEvent& e);
    void OnGridCellChanged(wxGridEvent& e);

    void OnChartColors(wxCommandEvent&);
    void OnChartSizingOptions(wxCommandEvent&);
    void OnChartSave(wxCommandEvent&);
    void OnShowDevTools(wxCommandEvent&);

    void OnWebViewPageLoaded(wxWebViewEvent&);
    void OnWebViewError(wxWebViewEvent&);
    void OnWebViewScriptResult(wxWebViewEvent&);
    void OnWebViewMessageReceived(wxWebViewEvent& evt);

    void ChartChangeColors(const wxString& colorsJSONStr);
    void ChartChangeSizingOptions(const wxString& sizingOptionsJSONStr);
    void ChartSavePNG(const wxString& PNGAsBase64Str);
    void ChartShowVersion(const wxString& version);

    void OnMessageChartError(const wxArrayString& params, const wxString& msg);
    void OnMessageChartDoubleClick(const wxArrayString& params, const wxString& msg);
    void OnMessageChartContextMenu();
};