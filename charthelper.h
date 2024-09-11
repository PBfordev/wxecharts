///////////////////////////////////////////////////////////////////////////////
// Project:     wxECharts
// Home:        https://github.com/PBfordev/wxecharts
// File Name:   charthelper.h
// Purpose:     Implementation of helper class using Apache ECharts
// Author:      PB
// Created:     2024-08-22
// Copyright:   (c) 2024 PB
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

#include <wx/string.h>

class wxColour;
class wxImage;
class wxMemoryBuffer;
class wxWebView;

/*****************************************************************

ChartHelper
---------------
helper class for communication with Apache ECharts

To create the chart:
1. Add variable names.
2. Add series.
4. Call SetWebView().
5. Call ChartCreate().
6. Call ChartUpdateSeries().

After a variable name or series properties are modified, the
appropriate ChartUpdate<X>() method must be called to reflect
the changes in the chart itself.

******************************************************************/

class ChartHelper final
{
public:
    enum SeriesType
    {
        Bar,
        Line,
    };

    struct ValueSeries
    {
        wxString name;
        SeriesType type{Bar};
        std::vector<double> data;
    };

    ChartHelper();
    void SetWebView(wxWebView* webView);

    size_t GetVariableNamesCount() const;

    bool GetVariableName(const size_t nameIdx, wxString& name) const;
    std::vector<wxString> GetVariableNames() const;
    bool AddVariableName(const wxString& name);
    bool AddVariableNames(const std::vector<wxString>& names);
    bool SetVariableName(const size_t nameIdx, const wxString& name);

    size_t GetSeriesCount() const;

    bool AddSeries(const ValueSeries& series);

    bool GetSeriesName(const size_t seriesIdx, wxString& name) const;
    std::vector<wxString> GetSeriesNames() const;
    bool SetSeriesName(const size_t seriesIdx, const wxString& name);

    bool GetSeriesType(const size_t seriesIdx, SeriesType& type) const;
    bool SetSeriesType(const size_t seriesIdx, const SeriesType& type);

    bool GetSeriesData(const size_t seriesIdx, std::vector<double>& data) const;
    bool SetSeriesData(const size_t seriesIdx, const std::vector<double>& data);

    bool ChartCreate();

    bool ChartUpdateSeries();
    bool ChartUpdateVariableNames();

    bool ChartGetColors(std::vector<wxColour>& colors);
    bool ChartSetColors(const std::vector<wxColour>& colors);

    bool ChartGetSizingOptions(double& widthToHeightRatio, int& minWidth, int& minHeight);
    bool ChartSetSizingOptions(const double widthToHeightRatio, const int minWidth, const int minHeight);

    // Obtains the chart rendered into a wxImage
    bool ChartGetAsImage(const int imageWidth, wxImage& image);
    // Obtains the chart rendered into a PNG
    bool ChartGetAsImage(const int imageWidth, wxMemoryBuffer& data);
    // Obtains the chart rendered into a PNG as a Base64-encoded string
    bool ChartGetAsImage(const int imageWidth, wxString& base64str);
    // Saves the chart as PNG
    bool ChartSaveAsImage(const int imageWidth, const wxString& fileName);

    bool GetEChartsVersion(wxString& version);
private:
    wxWebView* m_webView{nullptr};
    std::vector<wxString> m_variableNames;
    std::vector<ValueSeries> m_series;
};