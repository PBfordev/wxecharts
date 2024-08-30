///////////////////////////////////////////////////////////////////////////////
// Project:     wxECharts
// Home:        https://github.com/PBfordev/wxecharts
// File Name:   wxECharts.h
// Purpose:     Declaration of the application class
// Author:      PB
// Created:     2024-08-22
// Copyright:   (c) 2024 PB
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/app.h>

class wxEChartsApp : public wxApp
{
private:
    bool OnInit() override;
    int OnExit() override;

    wxString GetChartAssetsFolder();
};

wxDECLARE_APP(wxEChartsApp);
