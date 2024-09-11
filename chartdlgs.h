///////////////////////////////////////////////////////////////////////////////
// Project:     wxECharts
// Home:        https://github.com/PBfordev/wxecharts
// File Name:   chartdlgs.h
// Purpose:     Declaration of dialogs for changing chart properties
// Author:      PB
// Created:     2024-08-22
// Copyright:   (c) 2024 PB
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/dialog.h>

#include <vector>

class wxColour;
class wxColourPickerCtrl;
class wxSpinCtrl;
class wxSpinCtrlDouble;

/*****************************************************************

ChartColorsDlg
---------------
show/change the chart color scheme

******************************************************************/
class ChartColorsDlg : public wxDialog
{
public:
    ChartColorsDlg(wxWindow* parent, const std::vector<wxColor>& colors);

    std::vector<wxColor> GetColors() const;
private:
    std::vector<wxColourPickerCtrl*> m_colorPickers;
};

/*****************************************************************

ChartSizingOptionsDlg
---------------------
show/change the chart minimum size and width/height ratio

******************************************************************/
class ChartSizingOptionsDlg : public wxDialog
{
public:
    ChartSizingOptionsDlg(wxWindow* parent, const double widthToHeightRatio,
                          const int minWidth, const int minHeight);

    void GetSizingOptions(double& widthToHeightRatio,
                          int& minWidth, int& minHeight) const;
private:
    wxSpinCtrlDouble* m_ratioCtrl{nullptr};
    wxSpinCtrl* m_widthCtrl{nullptr};
    wxSpinCtrl* m_heightCtrl{nullptr};
};

/*****************************************************************

ChartDataPropertiesDlg
----------------------
show/change the series and variable names and series type
show the color and value of a doubleclicked data point

******************************************************************/
class ChartDataPropertiesDlg : public wxDialog
{
public:
    ChartDataPropertiesDlg(wxWindow* parent,
                   wxString& variableName, wxString& seriesName, int& seriesType,
                   const wxString& value, const wxColour& color);
};