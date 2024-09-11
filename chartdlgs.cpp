///////////////////////////////////////////////////////////////////////////////
// Project:     wxECharts
// Home:        https://github.com/PBfordev/wxecharts
// File Name:   chartdlgs.cpp
// Purpose:     Implementation of dialogs for changing chart properties
// Author:      PB
// Created:     2024-08-22
// Copyright:   (c) 2024 PB
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include <wx/wx.h>
#include <wx/clrpicker.h>
#include <wx/intl.h>
#include <wx/radiobox.h>
#include <wx/scrolwin.h>
#include <wx/spinctrl.h>
#include <wx/valtext.h>
#include <wx/valgen.h>

#include "chartdlgs.h"

/*****************************************************************

ChartColorsDlg

******************************************************************/

ChartColorsDlg::ChartColorsDlg(wxWindow* parent, const std::vector<wxColor>& colors)
     : wxDialog(parent, wxID_ANY, _("Chart Colors"))
{
    wxCHECK_RET(!colors.empty(), "Colors are empty");

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxScrolledWindow* colorsPanel = new wxScrolledWindow(this);
    wxFlexGridSizer* colorsSizer = new wxFlexGridSizer(2);

    for ( size_t i = 0; i < colors.size(); ++i )
    {
        wxASSERT(colors[i].IsOk());

        colorsSizer->Add(new wxStaticText(colorsPanel, wxID_ANY, wxString::Format(_("Color %zu"), i)), 
                         wxSizerFlags().Border().CenterVertical());
        wxColourPickerCtrl* cp = new wxColourPickerCtrl(colorsPanel, wxID_ANY, colors[i],
                                                   wxDefaultPosition, wxDefaultSize,
                                                   wxCLRP_USE_TEXTCTRL | wxCLRP_SHOW_LABEL);
        colorsSizer->Add(cp, wxSizerFlags().Border());
        m_colorPickers.push_back(cp);
    }
    colorsPanel->SetSizer(colorsSizer);
    colorsPanel->SetScrollRate(wxButton::GetDefaultSize().x,wxButton::GetDefaultSize().y);
    mainSizer->Add(colorsPanel, wxSizerFlags().Proportion(2).Expand());

    mainSizer->AddSpacer(FromDIP(8));
    mainSizer->Add(CreateStdDialogButtonSizer(wxOK | wxCANCEL), wxSizerFlags().Expand().Border());
    SetSizer(mainSizer);
 }

std::vector<wxColor> ChartColorsDlg::GetColors() const
{
    std::vector<wxColor> colors;

    colors.reserve(m_colorPickers.size());
    for ( const auto cp : m_colorPickers )
        colors.push_back(cp->GetColour());

    return colors;
}


/*****************************************************************

 ChartSizingOptionsDlg

******************************************************************/

ChartSizingOptionsDlg::ChartSizingOptionsDlg(wxWindow* parent, const double widthToHeightRatio,
                                             const int minWidth, const int minHeight)
    : wxDialog(parent, wxID_ANY, _("Chart Sizing Options"))
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    mainSizer->Add(new wxStaticText(this, wxID_ANY, _("Width to Height Ratio")),
                   wxSizerFlags().Border(wxALL & ~wxBOTTOM));
    m_ratioCtrl = new wxSpinCtrlDouble(this, wxID_ANY, "",
                        wxDefaultPosition, wxDefaultSize,
                        wxSP_ARROW_KEYS | wxALIGN_RIGHT,
                        0.3, 3.0, widthToHeightRatio, 0.1);
    m_ratioCtrl->SetDigits(1);
    mainSizer->Add(m_ratioCtrl, wxSizerFlags().Expand().Border());

    mainSizer->Add(new wxStaticText(this, wxID_ANY, _("Minimal Width")),
                   wxSizerFlags().Border(wxALL & ~wxBOTTOM));
    m_widthCtrl = new wxSpinCtrl(this, wxID_ANY, "",
                        wxDefaultPosition, wxDefaultSize,
                        wxSP_ARROW_KEYS | wxALIGN_RIGHT,
                        25, 250, minWidth);
    m_widthCtrl->SetIncrement(10);
    mainSizer->Add(m_widthCtrl, wxSizerFlags().Expand().Border());

    mainSizer->Add(new wxStaticText(this, wxID_ANY, _("Minimal Height")),
                   wxSizerFlags().Border(wxALL & ~wxBOTTOM));
    m_heightCtrl = new wxSpinCtrl(this, wxID_ANY, "",
                        wxDefaultPosition, wxDefaultSize,
                        wxSP_ARROW_KEYS | wxALIGN_RIGHT,
                        25, 250, minHeight);
    m_heightCtrl->SetIncrement(10);
    mainSizer->Add(m_heightCtrl, wxSizerFlags().Expand().Border());

    mainSizer->AddSpacer(FromDIP(8));
    mainSizer->Add(CreateStdDialogButtonSizer(wxOK | wxCANCEL), wxSizerFlags().Expand().Border());
    SetSizerAndFit(mainSizer);
}

void ChartSizingOptionsDlg::GetSizingOptions(double& widthToHeightRatio,
                                            int& minWidth, int& minHeight) const
{
    widthToHeightRatio = m_ratioCtrl->GetValue();
    minWidth = m_widthCtrl->GetValue();
    minHeight = m_heightCtrl->GetValue();
}


/*****************************************************************

ChartDataPropertiesDlg

******************************************************************/

ChartDataPropertiesDlg::ChartDataPropertiesDlg(wxWindow* parent,
                               wxString& variableName, wxString& seriesName, int& seriesType,
                               const wxString& value, const wxColour& color)
    : wxDialog(parent, wxID_ANY, _("Chart Data Properties"))
{
   wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    mainSizer->Add(new wxStaticText(this, wxID_ANY, _("Variable Name")),
                   wxSizerFlags().Border(wxALL & ~wxBOTTOM));
    wxTextCtrl* variableNameCtrl = new wxTextCtrl(this, wxID_ANY);
    variableNameCtrl->SetValidator(wxTextValidator(wxFILTER_EMPTY, &variableName));
    mainSizer->Add(variableNameCtrl, wxSizerFlags().Expand().Border());

    mainSizer->Add(new wxStaticText(this, wxID_ANY, _("Series Name")),
                   wxSizerFlags().Border(wxALL & ~wxBOTTOM));
    wxTextCtrl* seriesNameCtrl = new wxTextCtrl(this, wxID_ANY);
    seriesNameCtrl->SetValidator(wxTextValidator(wxFILTER_EMPTY, &seriesName));
    mainSizer->Add(seriesNameCtrl, wxSizerFlags().Expand().Border());

    wxArrayString seriesTypes{_("bar"), _("line")};
    wxRadioBox* seriesTypeCtrl = new wxRadioBox(this, wxID_ANY, _("Series Type"),
                                                wxDefaultPosition, wxDefaultSize,
                                                seriesTypes);
    seriesTypeCtrl->SetValidator(wxGenericValidator(&seriesType));
    mainSizer->Add(seriesTypeCtrl, wxSizerFlags().Expand().Border());

    mainSizer->Add(new wxStaticText(this, wxID_ANY, _("Value (read-only)")),
                   wxSizerFlags().Border(wxALL & ~wxBOTTOM));
    wxStaticText* valueCtrl = new wxStaticText(this, wxID_ANY, value);
    valueCtrl->SetBackgroundColour(*wxWHITE);
    valueCtrl->SetForegroundColour(color);
    valueCtrl->SetFont(valueCtrl->GetFont().Bold());
    mainSizer->Add(valueCtrl, wxSizerFlags().Expand().Border());

    mainSizer->AddSpacer(FromDIP(8));
    mainSizer->Add(CreateStdDialogButtonSizer(wxOK | wxCANCEL), wxSizerFlags().Expand().Border());
    SetSizerAndFit(mainSizer);
}
