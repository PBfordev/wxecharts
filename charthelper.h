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
    enum ScriptResult
    {
        CreateChart,

        UpdateSeries,
        UpdateVariableNames,

        GetColors,
        SetColors,

        GetSizingOptions,
        SetSizingOptions,

        GetPNG,

        GetEChartsVersion,
    };

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

    void RunChartCreate();

    void RunChartUpdateSeries();
    void RunChartUpdateVariableNames();

    void RunChartGetColors();
    void RunChartSetColors(const std::vector<wxColour>& colors);

    void RunChartGetSizingOptions();
    void RunChartSetSizingOptions(const double widthToHeightRatio, const int minWidth, const int minHeight);

    void RunChartGetPNG(const int imageWidth);

    void RunChartGetEChartsVersion();

    static bool JSONToColors(const wxString& JSONStr, std::vector<wxColour>& colors);
    static bool JSONToSizingOptions(const wxString& JSONStr, double& widthToHeightRatio,
                                    int& minWidth, int& minHeight);
private:
    wxWebView* m_webView{nullptr};
    std::vector<wxString> m_variableNames;
    std::vector<ValueSeries> m_series;
};