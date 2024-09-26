///////////////////////////////////////////////////////////////////////////////
// Project:     wxECharts
// Home:        https://github.com/PBfordev/wxecharts
// File Name:   mainframe.cpp
// Purpose:     Implementation of application's main frame
// Author:      PB
// Created:     2024-08-22
// Copyright:   (c) 2024 PB
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/grid.h>
#include <wx/numdlg.h>
#include <wx/splitter.h>
#include <wx/statline.h>
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>
#include <wx/webview.h>

#ifdef __WXMSW__
    #include <wx/msw/private/comptr.h>
#endif // #ifdef __WXMSW__

#include "chartdlgs.h"
#include "mainframe.h"

#if USING_WEBVIEW_EDGE
    #include <WebView2.h>
#endif // #if USING_WEBVIEW_EDGE

#ifdef __WXGTK__
    #include <webkit2/webkit2.h>
#endif // #ifdef __WXGTK__

#include <json.hpp>

using namespace std;

using json = nlohmann::ordered_json;

wxEChartsMainFrame::wxEChartsMainFrame(wxWindow* parent, const wxString& chartAssetsFolder)
    : wxFrame(parent, wxID_ANY, wxTheApp->GetAppDisplayName())
{
    SetMinClientSize(FromDIP(wxSize(600, 400)));

    wxMenu* menu = new wxMenu;

    menu->Append(ID_CHART_COLORS, _("Change Chart &Colors...\tCtrl+C"));
    menu->Append(ID_CHART_SIZING_OPTIONS,  _("Change Chart Sizing &Options...\tCtrl+O"));
    menu->AppendSeparator();
    menu->Append(wxID_SAVE, _("&Save chart as PNG...\tCtrl+S"));
    menu->AppendSeparator();
    menu->Append(ID_SHOW_DEVTOOLS,  _("Show &DevTools\tCtrl+D"));


    SetMenuBar(new wxMenuBar());
    GetMenuBar()->Append(menu, _("&Chart"));

    Bind(wxEVT_MENU, &wxEChartsMainFrame::OnChartColors, this, ID_CHART_COLORS);
    Bind(wxEVT_MENU, &wxEChartsMainFrame::OnChartSizingOptions, this, ID_CHART_SIZING_OPTIONS);    
    Bind(wxEVT_MENU, &wxEChartsMainFrame::OnChartSave, this, wxID_SAVE);
    Bind(wxEVT_MENU, &wxEChartsMainFrame::OnShowDevTools, this, ID_SHOW_DEVTOOLS);

    InitChartData();

    wxSplitterWindow* mainSplitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
                                            wxSP_BORDER | wxSP_LIVE_UPDATE);
    wxSplitterWindow* topSplitter = new wxSplitterWindow(mainSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                            wxSP_BORDER | wxSP_LIVE_UPDATE);

    wxPanel* gridAndHelpPanel = new wxPanel(topSplitter);
    wxBoxSizer* gridAndHelpPanelSizer = new wxBoxSizer(wxVERTICAL);

    CreateGrid(gridAndHelpPanel);
    gridAndHelpPanelSizer->Add(m_grid, wxSizerFlags().Expand().Border());

    const auto helpText = "To interact with the chart:\n"
                          "* use the Chart menu,\n"
                          "* change a value in the grid,\n"
                          "* doubleclick a data point or the y axis,\n"
                          "* right click the chart window.\n";

    wxTextCtrl* helpCtrl = new wxTextCtrl(gridAndHelpPanel, wxID_ANY, helpText, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
    helpCtrl->SetBackgroundColour(gridAndHelpPanel->GetBackgroundColour());
    gridAndHelpPanelSizer->Add(helpCtrl, wxSizerFlags().Expand().Border());

    gridAndHelpPanel->SetSizer(gridAndHelpPanelSizer);

    CreateWebView(topSplitter, chartAssetsFolder);

    wxTextCtrl* logCtrl = new wxTextCtrl(mainSplitter, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
    wxLog::SetActiveTarget(new wxLogTextCtrl(logCtrl));
    wxLog::DisableTimestamp();

    topSplitter->SetSashGravity(0);
    topSplitter->SetMinimumPaneSize(FromDIP(20));
    topSplitter->SplitVertically(gridAndHelpPanel, m_webView, gridAndHelpPanel->GetBestWidth(-1));
    topSplitter->Bind(wxEVT_SPLITTER_DOUBLECLICKED, [topSplitter, gridAndHelpPanel](wxSplitterEvent&)
        { topSplitter->SetSashPosition(gridAndHelpPanel->GetBestWidth(-1)); });

    mainSplitter->SetSashGravity(1);
    mainSplitter->SetMinimumPaneSize(FromDIP(100));

    mainSplitter->SplitHorizontally(topSplitter, logCtrl, -FromDIP(60));

    wxLogMessage("Using chart assets folder '%s'.", chartAssetsFolder);
    wxLogMessage("Using wxWebView backend '%s'.", wxWebView::GetBackendVersionInfo(m_webViewBackend).ToString());
}

void wxEChartsMainFrame::InitChartData()
{
    m_chartHelper.AddVariableNames({"Variable 1", "Variable 2", "Variable 3"});

    ChartHelper::ValueSeries s;

    s.name = "Group A";
    s.type = ChartHelper::Bar;
    s.data = {10, 20, 30};
    m_chartHelper.AddSeries(s);

    s.name = "Group B";
    s.type = ChartHelper::Bar;
    s.data = {15, 25, 35};
    m_chartHelper.AddSeries(s);
}

void wxEChartsMainFrame::CreateGrid(wxWindow* parent)
{
    const auto variableNames = m_chartHelper.GetVariableNames();
    const auto seriesNames = m_chartHelper.GetSeriesNames();

    m_grid = new wxGrid(parent, wxID_ANY);
    m_grid->SetDefaultRenderer(new wxGridCellFloatRenderer(-1, 1));
    m_grid->SetDefaultEditor(new wxGridCellFloatEditor(-1, 1));
    m_grid->EnableDragRowSize(false);
    m_grid->CreateGrid(variableNames.size(), seriesNames.size());

    for ( size_t row = 0; row < variableNames.size(); ++row )
        m_grid->SetRowLabelValue(row, variableNames[row]);

    for ( size_t col = 0; col < seriesNames.size(); ++col )
    {
        vector<double> data;

        m_grid->SetColLabelValue(col, seriesNames[col]);
        if ( m_chartHelper.GetSeriesData(col, data) )
        {
            for ( size_t row = 0; row < variableNames.size(); ++row )
                m_grid->SetCellValue(row, col, wxString::FromDouble(data[row]));
        }
    }

    m_grid->Bind(wxEVT_GRID_CELL_CHANGING, &wxEChartsMainFrame::OnGridCellChanging, this);
    m_grid->Bind(wxEVT_GRID_CELL_CHANGED, &wxEChartsMainFrame::OnGridCellChanged, this);
}

void wxEChartsMainFrame::CreateWebView(wxWindow* parent, const wxString& assetsFolder)
{
    const wxString url = wxString::Format("file://%s", wxFileName(assetsFolder, "wxecharts.html").GetFullPath());

    m_webViewBackend = wxWebViewBackendDefault;
#if USING_WEBVIEW_EDGE
    m_webViewBackend = wxWebViewBackendEdge;
#endif

    m_webView = wxWebView::New(parent, wxID_ANY, url, wxDefaultPosition, wxDefaultSize, m_webViewBackend);
    m_webView->EnableContextMenu(false);
    m_webView->EnableHistory(false);

    if ( m_webView->AddScriptMessageHandler("wxmsg") )
        m_webView->Bind(wxEVT_WEBVIEW_SCRIPT_MESSAGE_RECEIVED, &wxEChartsMainFrame::OnWebViewMessageReceived, this);
    else
        wxLogError(_("Could not install the webview message handler, the application will be unusable."));

#if wxCHECK_VERSION(3, 3, 0)
    m_webView->Bind(wxEVT_WEBVIEW_CREATED, [this](wxWebViewEvent&){ ConfigureWebView(); });
#endif // #if wxCHECK_VERSION(3, 3, 0)
    m_webView->Bind(wxEVT_WEBVIEW_LOADED, &wxEChartsMainFrame::OnWebViewPageLoaded, this);
    m_webView->Bind(wxEVT_WEBVIEW_ERROR, &wxEChartsMainFrame::OnWebViewError, this);
}

void wxEChartsMainFrame::ConfigureWebView()
{
    if ( m_webViewConfigured )
        return;

    void* nativeBackend = m_webView->GetNativeBackend();

    if ( !nativeBackend )
        return;

    m_webViewConfigured = true;

#if USING_WEBVIEW_EDGE
    ICoreWebView2* webView2 = static_cast<ICoreWebView2*>(nativeBackend);
    HRESULT hr;
    wxCOMPtr<ICoreWebView2Settings> settings;

    hr = webView2->get_Settings(&settings);
    if ( FAILED(hr) )
    {
        wxLogError(_("Could not obtain WebView2Settings (error code 0x%08lx)."), (long)hr);
        return;
    }
    settings->put_IsBuiltInErrorPageEnabled(FALSE);
    settings->put_IsZoomControlEnabled(FALSE);

    wxCOMPtr<ICoreWebView2Settings3> settings3;

    hr = settings->QueryInterface(wxIID_PPV_ARGS(ICoreWebView2Settings3, &settings3));
    if ( FAILED(hr) )
    {
        wxLogError(_("Could not obtain WebView2Settings3 (error code 0x%08lx)."), (long)hr);
        return;
    }
    settings3->put_AreBrowserAcceleratorKeysEnabled(FALSE);
#endif // #if USING_WEBVIEW_EDGE
}

void wxEChartsMainFrame::OnGridCellChanging(wxGridEvent& e)
{
    static constexpr double minAllowedValue = -100;
    static constexpr double maxAllowedValue = 100;

    double d;

    if ( !e.GetString().ToDouble(&d)
          || d < minAllowedValue || d > maxAllowedValue )
    {
        e.Veto();
        wxLogError(_("Invalid series value '%s' (col %d, row %d)."), e.GetString(), e.GetCol(), e.GetRow());
    }
}

void wxEChartsMainFrame::OnGridCellChanged(wxGridEvent& e)
{
    const int row = e.GetRow();
    const int col = e.GetCol();
    const wxString sVal = m_grid->GetCellValue(row, col);

    vector<double> data;
    double dVal;

    if ( m_chartHelper.GetSeriesData(col, data) )
    {
        sVal.ToDouble(&dVal);
        data[row] = dVal;
        m_chartHelper.SetSeriesData(col, data);
        m_chartHelper.ChartUpdateSeries();
    }
}

void wxEChartsMainFrame::OnChartColors(wxCommandEvent&)
{
    vector<wxColour> colors;

    if ( !m_chartHelper.ChartGetColors(colors) )
        return;

    ChartColorsDlg dlg(this, colors);

    if ( dlg.ShowModal() == wxID_OK )
        m_chartHelper.ChartSetColors(dlg.GetColors());
}


void wxEChartsMainFrame::OnChartSizingOptions(wxCommandEvent&)
{
    double widthToHeightRatio;
    int minWidth, minHeight;

    if ( !m_chartHelper.ChartGetSizingOptions(widthToHeightRatio, minWidth, minHeight) )
        return;

    ChartSizingOptionsDlg dlg(this, widthToHeightRatio, minWidth, minHeight);

    if ( dlg.ShowModal() == wxID_OK )
    {
        dlg.GetSizingOptions(widthToHeightRatio, minWidth, minHeight);
        m_chartHelper.ChartSetSizingOptions(widthToHeightRatio, minWidth, minHeight);
    }
}

void wxEChartsMainFrame::OnChartSave(wxCommandEvent&)
{
    const int chartWidth = wxGetNumberFromUser(_("Enter chart width (height is computed with the width/height ratio"),
                             _("Width"), _("Save chart"),
                             1000, 400, 4000, this);

    if ( chartWidth == -1 )
        return;

    const wxString fileName = wxFileSelector(_("Select file name for chart image"), "", "chart", "",
                                _("PNG files (*.png)|*.png"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

    if ( !fileName.empty() )
        m_chartHelper.ChartSaveAsImage(chartWidth, fileName);
}

void wxEChartsMainFrame::OnShowDevTools(wxCommandEvent&)
{
    void* nativeBackend = m_webView->GetNativeBackend();

    if ( !nativeBackend )
        return;

#if defined(__WXMSW__)
    if ( m_webViewBackend == wxWebViewBackendEdge )
    {
#if USING_WEBVIEW_EDGE
        ICoreWebView2* webView2 = static_cast<ICoreWebView2*>(nativeBackend);
        const HRESULT hr = webView2->OpenDevToolsWindow();

        if ( FAILED(hr) )
            wxLogError(_("Could not open DevTools window (error code 0x%08lx)."), (long)hr);
        return;
#endif //#if USING_WEBVIEW_EDGE
    }
#elif defined(__WXGTK__)
   if ( m_webViewBackend == wxWebViewBackendWebKit )
   {
       WebKitWebView* wkv = static_cast<WebKitWebView*>(nativeBackend);

       if ( wkv )
       {
           WebKitSettings* settings = webkit_web_view_get_settings(WEBKIT_WEB_VIEW(wkv));

           if ( !settings )
           {
               wxLogError(_("Could not open DevTools (failed to obtain WebKitSettings)."));
               return;
           }
           g_object_set(settings, "enable-developer-extras", TRUE, NULL);

           WebKitWebInspector *inspector = webkit_web_view_get_inspector(WEBKIT_WEB_VIEW(wkv));
           if ( !inspector )
           {
               wxLogError(_("Could not open DevTools (failed to obtain WebKitInspector)."));
               return;
           }
           webkit_web_inspector_show(inspector);

           return;
        }
    }
#endif // #elif defined(__WXGTK__)
    wxLogMessage(_("Showing DevTools with this backend is not supported."));
}

void wxEChartsMainFrame::OnWebViewPageLoaded(wxWebViewEvent&)
{
    ConfigureWebView();

    m_chartHelper.SetWebView(m_webView);

    wxString version;

    if ( m_chartHelper.GetEChartsVersion(version) )
        wxLogMessage(_("Using Apache ECharts v%s."), version);
    else
        wxLogError(_("Could not determine Apache ECharts version."));

    if ( m_chartHelper.ChartCreate() )
    {
        m_chartHelper.ChartUpdateVariableNames();
        m_chartHelper.ChartUpdateSeries();
    }
}

void wxEChartsMainFrame::OnWebViewError(wxWebViewEvent&)
{
    wxLogError(_("Could not initialize the chart."));
    m_webView->SetPage(R"(<!DOCTYPE html><html><head><meta charset="utf-8"/></head><body>)", "");
}

void wxEChartsMainFrame::OnWebViewMessageReceived(wxWebViewEvent& evt)
{
    wxString msg;

    // all the chart messages start with "wxECharts::" and after that, the
    // indivdual message fields are separated by \t, the first one being
    // the message type and the rest message-type specific
    if ( evt.GetString().StartsWith("wxECharts::", &msg) )
    {
        constexpr char msgFieldDelimiter = '\t';
        const wxArrayString msgFields = wxStringTokenize(msg, msgFieldDelimiter);

        if ( msgFields.size() < 2 )
        {
            wxLogError(_("Invalid wxECharts message: '%s'"), msg);
            return;
        }

        wxArrayString params(msgFields);

        params.erase(params.begin()); // remove the "wxECharts::" item

        if ( msgFields[0] == "error" )
        {
            OnScriptChartError(params, msg);
        }
        else if ( msgFields[0] == "dblclick" )
        {
            OnScriptChartDoubleClick(params, msg);
        }
        else if ( msgFields[0] == "contextmenu" )
        {
            OnScriptChartContextMenu();
        }
        else
        {
            wxLogMessage(_("Unexpected wxECharts message: '%s'"), msg);
        }
    }
}

void wxEChartsMainFrame::OnScriptChartError(const wxArrayString& params, const wxString& msg)
{
    constexpr size_t validMinParamsCount = 3;

    if ( params.size() < validMinParamsCount )
    {
        wxLogError(_("Malformed wxECharts error message: '%s'"), msg);
        return;
    }

    wxLogError(_("JavaScript error %s in %s: %s."), params[0], params[1], params[2]);
}

void wxEChartsMainFrame::OnScriptChartDoubleClick(const wxArrayString& params, const wxString& msg)
{
    constexpr size_t validMinParamsCount = 2;

    if ( params.size() < validMinParamsCount )
    {
        wxLogError(_("Malformed wxECharts dblclick message: '%s'"), msg);
        return;
    }

    try
    {
        const json j = json::parse(string(params[1].utf8_string()));

        if ( params[0] == "yAxis" )
        {
           wxLogMessage("yAxis doubleclicked: axis index = %d, target = %s",
                         j.at("yAxisIndex").get<int>(), 
                         wxString::FromUTF8((j.at("targetType").get<string>())));
        }
        else if ( params[0] == "series" )
        {
            const size_t variableIdx = j.at("dataIndex").get<size_t>();
            const size_t seriesIdx = j.at("seriesIndex").get<size_t>();
            const wxString value = wxString::Format("%g", j.at("value").get<double>());
            const wxColor color = wxColor(wxString::FromUTF8(j.at("color").get<string>()));
            ChartHelper::ValueSeries series;
            wxString variableName;
            wxString seriesName;
            ChartHelper::SeriesType seriesType;
            int seriesTypeInt;

            if ( m_chartHelper.GetVariableName(variableIdx, variableName)
                 && m_chartHelper.GetSeriesName(seriesIdx, seriesName)
                 && m_chartHelper.GetSeriesType(seriesIdx, seriesType) )
            {
                seriesTypeInt = static_cast<int>(seriesType);

                ChartDataPropertiesDlg dlg(this, variableName, seriesName, seriesTypeInt, value, color);

                if (dlg.ShowModal() != wxID_OK )
                    return;

                if ( m_chartHelper.SetVariableName(variableIdx, variableName) )
                {
                    m_chartHelper.ChartUpdateVariableNames();
                    m_grid->SetRowLabelValue(variableIdx, variableName);
                }
                if ( m_chartHelper.SetSeriesName(seriesIdx, seriesName) )
                    m_grid->SetColLabelValue(seriesIdx, seriesName);
                 m_chartHelper.SetSeriesType(seriesIdx, static_cast<ChartHelper::SeriesType>(seriesTypeInt));
                 m_chartHelper.ChartUpdateSeries();
            }
        }
        else
        {
            wxLogMessage(_("Unexpected wxECharts dblclick message: '%s'"), msg);
        }
    }
    catch (const json::exception& e)
    {
        wxLogError(_("JSON parsing error in %s (%s)."), __FUNCTION__, e.what());
    }
}

void wxEChartsMainFrame::OnScriptChartContextMenu()
{
    wxLogMessage(_("wxECharts 'contextmenu' message received."));
}