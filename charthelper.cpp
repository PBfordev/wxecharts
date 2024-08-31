///////////////////////////////////////////////////////////////////////////////
// Project:     wxECharts
// Home:        https://github.com/PBfordev/wxecharts
// File Name:   charthelper.cpp
// Purpose:     Implementation of helper class using Apache Echarts
// Author:      PB
// Created:     2024-08-22
// Copyright:   (c) 2024 PB
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////


#include <wx/wx.h>
#include <wx/base64.h>
#include <wx/ffile.h>
#include <wx/mstream.h>
#include <wx/webview.h>

#include <utility>

#include "nlohmann/json.hpp"
using json = nlohmann::ordered_json;

#include "charthelper.h"

using namespace std;

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

bool ChartHelper::ChartCreate()
{
    wxCHECK(m_webView, false);
    return m_webView->RunScript("WXChartCreateChart('chart');");
}

bool ChartHelper::ChartUpdateSeries()
{
    wxCHECK(m_webView, false);
    wxCHECK(!m_series.empty(), false);

    wxString script;

    try
    {
        std::vector<json> allSeriesJSON;

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
        script.Printf("WXChartUpdateSeries('%s');", j.dump());
    }
    catch (const json::exception& e)
    {
        wxLogError(_("JSON parsing error in %s (%s)."), __FUNCTION__, e.what());
        return false;
    }

    return m_webView->RunScript(script);
}

bool ChartHelper::ChartUpdateVariableNames()
{
    wxCHECK(m_webView, false);
    wxCHECK(!m_variableNames.empty(), false);

    wxString script;

    try
    {
        json j;

        for ( const auto& n : m_variableNames )
            j.push_back(n.utf8_string());
        script.Printf("WXChartUpdateVariableNames('%s');", j.dump());
    }
    catch (const json::exception& e)
    {
        wxLogError(_("JSON parsing error in %s (%s)."), __FUNCTION__, e.what());
        return false;
    }

    return m_webView->RunScript(script);
}

bool ChartHelper::ChartGetColors(std::vector<wxColour>& colors)
{
    wxCHECK(m_webView, false);

    wxString script, result;

    if( m_webView->RunScript("WXChartGetChartColors();", &result) )
    {
        try
        {
            const json j = json::parse(string(result.utf8_string()));

            wxCHECK(j.is_array(), false);

            std::vector<wxColor> tmpColors;

            for ( const auto& e : j )
            {
                wxColour color(wxString::FromUTF8(e.get<std::string>()));

                wxCHECK(color.IsOk(), false);
                tmpColors.push_back(color);
            }

            colors = move(tmpColors);
            return true;
        }
        catch (const json::exception& e)
        {
            wxLogError(_("JSON parsing error in %s (%s)."), __FUNCTION__, e.what());
        }
    }

    return false;
}

bool ChartHelper::ChartSetColors(const std::vector<wxColour>& colors)
{
    wxCHECK(m_webView, false);

    wxString script;

    try
    {
        json j = json::array();

        for ( const auto& c : colors )
        {
            wxCHECK(c.IsOk(), false);
            j.push_back(c.GetAsString(wxC2S_HTML_SYNTAX).utf8_string());
        }
        script.Printf("WXChartSetChartColors('%s')", j.dump());
    }
    catch (const json::exception& e)
    {
        wxLogError(_("JSON parsing error in %s (%s)."), __FUNCTION__, e.what());
        return false;
    }

    return m_webView->RunScript(script);
}

bool ChartHelper::ChartGetSizingOptions(double& widthToHeightRatio, int& minWidth, int& minHeight)
{
    wxCHECK(m_webView, false);
    wxString script, result;

    if ( m_webView->RunScript("WXChartGetChartSizingOptions();", &result) )
    {
        try
        {
            const json j = json::parse(string(result.utf8_string()));

            widthToHeightRatio = j["widthToHeightRatio"].get<double>();
            minWidth = j["minWidth"].get<int>();
            minHeight = j["minHeight"].get<int>();
            return true;
        }
        catch (const json::exception& e)
        {
            wxLogError(_("JSON parsing error in %s (%s)."), __FUNCTION__, e.what());
        }
    }

    return false;
}


bool ChartHelper::ChartSetSizingOptions(const double* widthToHeightRatio, const int* minWidth, const int* minHeight)
{
    wxCHECK(m_webView, false);
    wxCHECK(widthToHeightRatio || minWidth || minHeight, false);

    wxString script;

    try
    {
        json j;

        if ( widthToHeightRatio )
            j["widthToHeightRatio"] = *widthToHeightRatio;
        if ( minWidth )
            j["minWidth"] = *minWidth;
        if ( minHeight )
            j["minHeight"] = *minHeight;
        script.Printf("WXChartSetChartSizingOptions('%s');", j.dump());
    }
    catch (const json::exception& e)
    {
        wxLogError(_("JSON parsing error in %s (%s)."), __FUNCTION__, e.what());
        return false;
    }

    return m_webView->RunScript(script);
}

bool ChartHelper::ChartGetAsImage(const int imageWidth, wxImage& image)
{
    wxMemoryBuffer buf;

    if ( !ChartGetAsImage(imageWidth, buf) )
        return false;

    wxMemoryInputStream stream(buf.GetData(), buf.GetDataLen());

    return image.LoadFile(stream);
}

bool ChartHelper::ChartGetAsImage(const int imageWidth, wxMemoryBuffer& data)
{
    wxString base64str;

    if ( !ChartGetAsImage(imageWidth, base64str) )
        return false;

    data = wxBase64Decode(base64str);
    if ( data.IsEmpty() )
    {
        wxLogError(_("Could not decode the chart data URL."));
        return false;
    }

    return true;
}

bool ChartHelper::ChartGetAsImage(const int imageWidth, wxString& base64str)
{
    wxCHECK(m_webView, false);

    wxString script;
    wxString result;

    script.Printf("WXChartSaveChartAsImage(%d);", imageWidth);

    if ( !m_webView->RunScript(script, &result) )
        return false;

    wxString str;

    if ( !result.StartsWith("data:image/png;base64,", &str) )
    {
        wxLogError(_("Invalid chart data URL."));
        return false;
    }

    base64str = move(str);
    return true;
}

bool ChartHelper::ChartSaveAsImage(const int imageWidth, const wxString& fileName)
{
    wxMemoryBuffer buf;

    if ( !ChartGetAsImage(imageWidth, buf) )
        return false;

    wxFFile file(fileName, "wb");

    if ( !file.IsOpened() || file.Write(buf.GetData(), buf.GetDataLen()) != buf.GetDataLen() )
        return false;

    return true;
}