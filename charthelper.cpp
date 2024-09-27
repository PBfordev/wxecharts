///////////////////////////////////////////////////////////////////////////////
// Project:     wxECharts
// Home:        https://github.com/PBfordev/wxecharts
// File Name:   charthelper.cpp
// Purpose:     Implementation of helper class using Apache ECharts
// Author:      PB
// Created:     2024-08-22
// Copyright:   (c) 2024 PB
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////


#include <wx/wx.h>
#include <wx/webview.h>

#include <utility>

#include <json.hpp>

#include "charthelper.h"

using namespace std;

using json = nlohmann::ordered_json;

ChartHelper::ChartHelper()
{}

void ChartHelper::SetWebView(wxWebView* webView)
{
    wxASSERT(webView);
    m_webView = webView;
}

size_t ChartHelper::GetVariableNamesCount() const
{
    return m_variableNames.size();
}

bool ChartHelper::GetVariableName(const size_t nameIdx, wxString& name) const
{
    wxCHECK(nameIdx < m_variableNames.size(), false);
    name = m_variableNames[nameIdx];
    return true;
}

vector<wxString> ChartHelper::GetVariableNames() const
{
    return m_variableNames;
}

bool ChartHelper::AddVariableName(const wxString& name)
{
    wxCHECK_MSG(!m_series.empty(), false, "Adding variable name after adding a series");

    for ( const auto& n : m_variableNames)
        wxCHECK_MSG(!n.IsSameAs(name, true), false, "Variable name already used");

    m_variableNames.push_back(name);
    return true;
}

bool  ChartHelper::AddVariableNames(const std::vector<wxString>& names)
{
    wxCHECK(!names.empty(), false);
    wxCHECK_MSG(m_series.empty(), false, "Adding variable name after adding a series");

    for ( const auto& vn : m_variableNames)
    {
        for ( const auto & n : names)
            wxCHECK_MSG(!n.IsSameAs(vn, true), false, "Variable name already used");
    }

    m_variableNames.insert(m_variableNames.end(), names.begin(), names.end());
    return true;
}

bool ChartHelper::SetVariableName(const size_t nameIdx, const wxString& name)
{
    wxCHECK(!name.empty(), false);
    wxCHECK(nameIdx < m_variableNames.size(), false);

    for ( size_t i = 0; i < m_variableNames.size(); ++i )
    {
        if ( i == nameIdx )
            continue;
        wxCHECK_MSG(!m_variableNames[i].IsSameAs(name, true), false, "Variable name already used");
    }

    m_variableNames[nameIdx] = name;
    return true;
}

size_t ChartHelper::GetSeriesCount() const
{
    return m_series.size();
}

bool ChartHelper::AddSeries(const ValueSeries& series)
{
    wxCHECK_MSG(!m_variableNames.empty(), false, "Adding series before adding variable names");
    wxCHECK(!series.name.empty(), false);
    wxCHECK(series.data.size() == m_variableNames.size(), false);

    for ( const auto& s : m_series )
        wxCHECK_MSG(!s.name.IsSameAs(series.name, true), false, "Series name already used");

    m_series.push_back(series);
    return true;
}

bool ChartHelper::GetSeriesName(const size_t seriesIdx, wxString& name) const
{
    wxCHECK(seriesIdx < m_series.size(), false);
    name = m_series[seriesIdx].name;
    return true;
}

vector<wxString> ChartHelper::GetSeriesNames() const
{
    vector<wxString> names;

    names.reserve(m_series.size());
    for ( const auto& s : m_series )
        names.push_back(s.name);

    return names;
}

bool ChartHelper::SetSeriesName(const size_t seriesIdx, const wxString& name)
{
    wxCHECK(!name.empty(), false);
    wxCHECK(seriesIdx < m_series.size(), false);

    for ( size_t i = 0; i < m_series.size(); ++i)
    {
        if ( i == seriesIdx )
            continue;
        wxCHECK_MSG(!m_series[i].name.IsSameAs(name, true), false, "Series name already used");
    }

    m_series[seriesIdx].name = name;
    return true;
}

bool ChartHelper::GetSeriesType(const size_t seriesIdx, SeriesType& type) const
{
    wxCHECK(seriesIdx < m_series.size(), false);
    type = m_series[seriesIdx].type;
    return true;
}

bool ChartHelper::SetSeriesType(const size_t seriesIdx, const SeriesType& type)
{
    wxCHECK(seriesIdx < m_series.size(), false);
    m_series[seriesIdx].type = type;
    return true;
}

bool ChartHelper::GetSeriesData(const size_t seriesIdx, std::vector<double>& data) const
{
    wxCHECK(seriesIdx < m_series.size(),false);
    data = m_series[seriesIdx].data;
    return true;
}

bool ChartHelper::SetSeriesData(const size_t seriesIdx, const std::vector<double>& data)
{
    wxCHECK(seriesIdx < m_series.size(), false);
    wxCHECK(data.size() == m_variableNames.size(), false);
    m_series[seriesIdx].data = data;
    return true;
}

void ChartHelper::RunChartCreate()
{
    wxCHECK_RET(m_webView, "m_webView is null");
    m_webView->RunScriptAsync("wxEChartsCreateChart('chart');", (void*)CreateChart);
}

void ChartHelper::RunChartUpdateSeries()
{
    wxCHECK_RET(m_webView, "m_webView is null");
    wxCHECK_RET(!m_series.empty(), "m_series is empty");

    wxString script;

    try
    {
        vector<json> allSeriesJSON;

        for ( const auto& s : m_series )
        {
            json oneSeriesJSON;

            oneSeriesJSON["name"] = s.name.utf8_string();
            if ( s.type == Bar )
                oneSeriesJSON["type"] = "bar";
            else
                oneSeriesJSON["type"] = "line";
            oneSeriesJSON["data"] = s.data;
            allSeriesJSON.push_back(oneSeriesJSON);
        }

        json j;

        j["series"] = allSeriesJSON;
        script.Printf("wxEChartsUpdateSeries('%s');", wxString::FromUTF8(j.dump()));
    }
    catch (const json::exception& e)
    {
        wxLogError(_("JSON error in %s (%s)."), __FUNCTION__, e.what());
        return;
    }

    m_webView->RunScriptAsync(script, (void*)UpdateSeries);
}

void ChartHelper::RunChartUpdateVariableNames()
{
    wxCHECK_RET(m_webView, "m_webView is null");
    wxCHECK_RET(!m_variableNames.empty(), "m_variableNames is empty");

    wxString script;

    try
    {
        json j;

        for ( const auto& n : m_variableNames )
            j.push_back(n.utf8_string());
        script.Printf("wxEChartsUpdateVariableNames('%s');", wxString::FromUTF8(j.dump()));
    }
    catch (const json::exception& e)
    {
        wxLogError(_("JSON error in %s (%s)."), __FUNCTION__, e.what());
        return;
    }

    m_webView->RunScriptAsync(script, (void*)UpdateVariableNames);
}

void ChartHelper::RunChartGetColors()
{
    wxCHECK_RET(m_webView, "m_webView is null");    
    m_webView->RunScriptAsync("wxEChartsGetChartColors();", (void*)GetColors);
}

void ChartHelper::RunChartSetColors(const std::vector<wxColour>& colors)
{
    wxCHECK_RET(m_webView, "m_webView is null");

    wxString script;

    try
    {
        json j = json::array();

        for ( const auto& c : colors )
        {
            j.push_back(c.GetAsString(wxC2S_HTML_SYNTAX).utf8_string());
        }
        script.Printf("wxEChartsSetChartColors('%s')", j.dump());
    }
    catch (const json::exception& e)
    {
        wxLogError(_("JSON error in %s (%s)."), __FUNCTION__, e.what());
        return;
    }

    m_webView->RunScriptAsync(script, (void*)SetColors);
}

void ChartHelper::RunChartGetSizingOptions()
{
    wxCHECK_RET(m_webView, "m_webView is null");
    wxString script;

    m_webView->RunScriptAsync("wxEChartsGetChartSizingOptions();", (void*)GetSizingOptions);
}


void ChartHelper::RunChartSetSizingOptions(const double widthToHeightRatio, const int minWidth, const int minHeight)
{
    wxCHECK_RET(m_webView, "m_webView is null");

    wxString script;

    try
    {
        json j;

        j["widthToHeightRatio"] = widthToHeightRatio;
        j["minWidth"] = minWidth;
        j["minHeight"] = minHeight;
        script.Printf("wxEChartsSetChartSizingOptions('%s');", j.dump());
    }
    catch (const json::exception& e)
    {
        wxLogError(_("JSON error in %s (%s)."), __FUNCTION__, e.what());
        return;
    }

    m_webView->RunScriptAsync(script, (void*)SetSizingOptions);
}


void ChartHelper::RunChartGetPNG(const int imageWidth)
{
    wxCHECK_RET(m_webView, "m_webView is null");

    wxString script;

    script.Printf("wxEChartsSaveChartAsImage(%d);", imageWidth);
    m_webView->RunScriptAsync(script, (void*)GetPNG);
}


void ChartHelper::RunChartGetEChartsVersion()
{
    wxCHECK_RET(m_webView, "m_webView is null");

    m_webView->RunScriptAsync("wxEChartsGetEChartsVersion();", (void*)GetEChartsVersion);
}

bool ChartHelper::JSONToColors(const wxString& JSONStr, std::vector<wxColour>& colors)
{
    try
    {
        const json j = json::parse(string(JSONStr.utf8_string()));

        if  ( !j.is_array() )
        {
            wxLogError(_("Invalid JSON colors string in %s."), __FUNCTION__);
            return false;
        }

        vector<wxColor> tmpColors;

        for ( const auto& e : j )
        {
            wxColour color(wxString::FromUTF8(e.get<string>()));

            if ( !color.IsOk() )
            {
                wxLogError(_("Invalid JSON color in %s."), __FUNCTION__);
                return false;
            }
            tmpColors.emplace_back(move(color));
        }
        colors = move(tmpColors);
    }
    catch (const json::exception& e)
    {
        wxLogError(_("JSON parsing error in %s (%s)."), __FUNCTION__, e.what());
        return false;
    }

    return true;
}

bool ChartHelper::JSONToSizingOptions(const wxString& JSONStr, double& widthToHeightRatio,
                                      int& minWidth, int& minHeight)
{
    try
    {
        const json j = json::parse(string(JSONStr.utf8_string()));

        widthToHeightRatio = j.at("widthToHeightRatio").get<double>();
        minWidth = j.at("minWidth").get<int>();
        minHeight = j.at("minHeight").get<int>();
    }
    catch (const json::exception& e)
    {
        wxLogError(_("JSON parsing error in %s (%s)."), __FUNCTION__, e.what());
        return false;
    }
    return true;
}