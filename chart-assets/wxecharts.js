///////////////////////////////////////////////////////////////////////////////
// Project:     wxECharts
// Home:        https://github.com/PBfordev/wxecharts
// File Name:   wxecharts.js
// Purpose:     JavaScript for communicating between Apache ECharts and C++ code
// Author:      PB
// Created:     2024-08-22
// Copyright:   (c) 2024 PB
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// the one and only Apache ECharts instance
var wxEChartstheChart;

var wxEChartsSizingOptions =
{
  widthToHeightRatio: 1,
  minWidth: 150,
  minHeight: 150,
};

function wxEChartsSendMessage(prefix, params) {
  let message = 'wxECharts::'.concat(prefix, '\t', JSON.stringify(params)); 
  window.wxmsg.postMessage(message);
}

function wxEChartsSendErrorMessage(error, where)
{
  wxEChartsSendMessage('error\t'.concat(error.name, '\t', where, '\t', error.message));
}

function wxEChartsCreateChart(divId) {
  try {
    wxEChartstheChart = echarts.init(document.getElementById(divId));

    let option = {
      legend: { selectedMode: false },
      tooltip: {},
      animation: false,
      grid: { left: '10%', top: '10%', bottom: '10%', right: '10%' },
      textStyle: { fontFamily: "Calibri, Tahoma, Arial, sans-serif", 
                   fontSize: '1rem', color: 'black' },
      xAxis: {
        type: 'category',
        axisTick: { show: true },
        axisLine: { show: true, lineStyle: { color: 'black' } }
      },
      yAxis: {
        name: 'Value',
        nameTextStyle: { fontWeight: 'bold' },
        axisTick: { show: true },
        axisLine: { show: true, lineStyle: { color: 'black' } },
        triggerEvent: true
      }
    };
    wxEChartstheChart.setOption(option);
  } catch (e) {
    wxEChartsSendErrorMessage(e, arguments.callee.name);
  }

  wxEChartsResizeChart();
  window.onresize = function () { wxEChartsResizeChart(); };

  wxEChartstheChart.on('dblclick', 'yAxis', function (params) {
    let p = {};
    
    p.yAxisIndex = params.yAxisIndex;
    p.targetType = params.targetType;
    
    wxEChartsSendMessage('dblclick\tyAxis', p);
  });

  wxEChartstheChart.on('dblclick', 'series', function (params) {
    let p = {};

    p.name = params.name;
    p.componentIndex = params.componentIndex;
    p.componentSubType = params.componentSubType;
    p.dataIndex = params.dataIndex;
    p.seriesIndex = params.seriesIndex;
    p.seriesName = params.seriesName;
    p.seriesType = params.seriesType;
    p.color = params.color;
    p.value = params.value;
    wxEChartsSendMessage('dblclick\tseries', p);
  });

  wxEChartstheChart.on('contextmenu', function (params) {
    let p = {};
    
    p.componentType = params.componentType;
    p.clientX = params.event.event.clientX;
    p.clientY = params.event.event.clientY;
    wxEChartsSendMessage('contextmenu\tchart', p);
    params.event.stop();
  });  

  window.oncontextmenu = function (event) 
    { 
      let p = {};

      p.clientX = event.clientX;
      p.clientY = event.clientY;
      wxEChartsSendMessage('contextmenu\tnochart', p);
      event.preventDefault();
    }
}

function wxEChartsResizeChart() {
  try {
    const dom = wxEChartstheChart.getDom();
    const divWidth = dom.clientWidth;
    const divHeight = dom.clientHeight;
    let chartWidth = divWidth;
    let chartHeight = chartWidth / wxEChartsSizingOptions.widthToHeightRatio;

    if (chartHeight >= divHeight) {
      chartHeight = divHeight;
      chartWidth = divHeight * wxEChartsSizingOptions.widthToHeightRatio;
    }

    if (chartWidth >= wxEChartsSizingOptions.minWidth && chartHeight >= wxEChartsSizingOptions.minHeight)
      wxEChartstheChart.resize({ width: chartWidth, height: chartHeight });
  
  } catch (e) {
    wxEChartsSendErrorMessage(e, arguments.callee.name);
  }
}

function wxEChartsUpdateSeries(seriesJSON) {
  try {
    wxEChartstheChart.setOption(JSON.parse(seriesJSON));
  } catch (e) {
    wxEChartsSendErrorMessage(e, arguments.callee.name);
  }
}

function wxEChartsUpdateVariableNames(variableNamesJSON) {
  try {
    wxEChartstheChart.setOption({ xAxis : { data: JSON.parse(variableNamesJSON) } });
  } catch (e) {
    wxEChartsSendErrorMessage(e, arguments.callee.name);
  }
}

function wxEChartsSaveChartAsImage(width) {
  try {
    return wxEChartstheChart.getDataURL({ type: 'png', pixelRatio: width / wxEChartstheChart.getWidth(), backgroundColor: 'white' });
  } catch (e) {
    wxEChartsSendErrorMessage(e, arguments.callee.name);
  }
}

function wxEChartsGetChartColors() {
  try {
    return JSON.stringify(wxEChartstheChart.getOption().color);
  } catch (e) {
    wxEChartsSendErrorMessage(e, arguments.callee.name);
  }
}

function wxEChartsSetChartColors(colorsJSON) {  
  try {
    wxEChartstheChart.setOption({color: JSON.parse(colorsJSON)});
  } catch (e) {
    wxEChartsSendErrorMessage(e, arguments.callee.name);
  }
}

function wxEChartsGetChartSizingOptions() {
  try {
    return JSON.stringify(wxEChartsSizingOptions);
  } catch (e) {
    wxEChartsSendErrorMessage(e, arguments.callee.name);
  }
}

function wxEChartsSetChartSizingOptions(optionsJSON) {
  try {
    let o = JSON.parse(optionsJSON);
    
    wxEChartsSizingOptions.widthToHeightRatio = o.widthToHeightRatio;
    wxEChartsSizingOptions.minWidth = o.minWidth;
    wxEChartsSizingOptions.minHeight = o.minHeight;

    wxEChartsResizeChart();
  } catch (e) {
    wxEChartsSendErrorMessage(e, arguments.callee.name);
  }
}

function wxEChartsGetEChartsVersion() {
  try {
    return echarts.version;
  } catch (e) {
    wxEChartsSendErrorMessage(e, arguments.callee.name);
  }
}
