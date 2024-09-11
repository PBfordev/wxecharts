///////////////////////////////////////////////////////////////////////////////
// Project:     wxECharts
// Home:        https://github.com/PBfordev/wxecharts
// File Name:   wxecharts.cpp
// Purpose:     Implementation of the application class
// Author:      PB
// Created:     2024-08-22
// Copyright:   (c) 2024 PB
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include <wx/wx.h>
#include <wx/config.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/webview.h>

#include "wxecharts.h"
#include "mainframe.h"

bool wxEChartsApp::OnInit()
{
    if ( !wxApp::OnInit() )
	    return false;

#if USING_WEBVIEW_EDGE
    if ( !wxWebView::IsBackendAvailable(wxWebViewBackendEdge) )
    {
        wxLogError(_("Cannot use wxWebViewEdge backend: Is 'WebView2Loader.dll' in the same folder as the executable?"));
        return false;
    }
#endif // #if USING_WEBVIEW_EDGE

    SetAppName("wxECharts");
    SetVendorName("PB");

#ifdef __WXMSW__
    wxStandardPaths& standardPaths = wxStandardPaths::Get();

    standardPaths.IgnoreAppSubDir("Debug DLL");
    standardPaths.IgnoreAppSubDir("Release DLL");
    standardPaths.IgnoreAppSubDir("x64");
#endif

    delete wxConfigBase::Set(new wxConfig(GetAppName(), GetVendorName()));

    const wxString assetsFolder = GetChartAssetsFolder();

    if ( assetsFolder.empty() )
    {
        wxLogError(_("Could not establish the chart assets folder: The application will terminate."));
        return false;
    }

    wxInitAllImageHandlers();

    wxEChartsMainFrame* mainFrame = new wxEChartsMainFrame(nullptr, assetsFolder);
    mainFrame->Show();

    return true;
}

int wxEChartsApp::OnExit()
{
    delete wxConfigBase::Set(nullptr);
    return wxApp::OnExit();
}

// for demonstration, be flexible when it comes to data assets folder location
wxString wxEChartsApp::GetChartAssetsFolder()
{
    static constexpr const char* chartAssets[] =
        {"wxecharts.html", "wxecharts.js", "echarts.min.js", };
    static constexpr auto assetsFolderName = "chart-assets";
    static constexpr auto configKeyName = "ChartAssetsFolder";

    const wxStandardPaths& standardPaths = wxStandardPaths::Get();
    wxConfigBase* config = wxConfigBase::Get();
    const wxConfigPathChanger changer(config, "/");
    wxString folder;
    wxFileName fn;

    auto HasChartAssets = [](const wxString& folder)
    {
        for ( const auto name : chartAssets )
        {
            if ( !wxFileName(folder, name).FileExists() )
                return false;
        }
        return true;
    };

    // first try the path stored in the config, if any
    if ( config->Read(configKeyName, &folder) )
    {
        if ( HasChartAssets(folder) )
            return folder;
    }

    // the standard data dir
    fn.AssignDir(standardPaths.GetDataDir());
    fn.AppendDir(assetsFolderName);
    folder = fn.GetPath();
    if ( HasChartAssets(folder) )
        return folder;

    // the folder with the executable
    fn.Assign(standardPaths.GetExecutablePath());
    fn.AppendDir(assetsFolderName);
    folder = fn.GetPath();
    if ( HasChartAssets(folder) )
        return folder;

    // try the folder above the one with the executable
    if ( fn.GetDirCount() > 2 )
    {
        fn.RemoveDir(fn.GetDirCount() - 2);
        folder = fn.GetPath();
        if ( HasChartAssets(folder) )
            return folder;
    }

    // could not find, ask the user
    for ( ;; )
    {
        wxFileDialog dlg(nullptr, _("Select the folder with chart assets"), "", chartAssets[0],
                         _("HTML Files (*.html)|*.html"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);

        if ( dlg.ShowModal() != wxID_OK )
            break;

        fn.Assign(dlg.GetPath());
        folder = fn.GetPath();
        if ( HasChartAssets(folder) )
        {
            config->Write(configKeyName, folder);
            return folder;
        }
    }

    // assets folder undetermined
    return wxEmptyString;
}

wxIMPLEMENT_APP(wxEChartsApp);