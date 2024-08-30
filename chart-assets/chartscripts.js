///////////////////////////////////////////////////////////////////////////////
// Project:     wxECharts
// Home:        https://github.com/PBfordev/wxecharts
// File Name:   chartscripts.js
// Purpose:     JavaScript for communicating with wxECharts
// Author:      PB
// Created:     2024-08-22
// Copyright:   (c) 2024 PB
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// the one and only Apache Charts instance
var WXCharttheChart;

var WXChartSizingOptions =
{
  widthToHeightRatio: 1,
  minWidth: 150,
  minHeight: 150,
};

function WXChartSendMessage(prefix, params) {
  let message = 'WXChart::'.concat(prefix, '\t', JSON.stringify(params)); 
  window.wxmsg.postMessage(message);
}

function WXChartSendErrorMessage(error, params)
{
  WXChartSendMessage('error\t'.concat(error), params);
}

function WXChartSendErrorMessageJSON(errorName, errorMessage, functionName)
{
  let p = {};
  p.name = errorName;
  p.message = errorMessage;
  p.function = functionName;
  WXChartSendErrorMessage('JSON.parse()', p);
}

function WXChartCreateChart(divId, sizingOptions) {
  WXCharttheChart = echarts.init(document.getElementById(divId));

  var option = {
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
  WXCharttheChart.setOption(option);
  
  WXChartResizeChart();
  window.onresize = function () { WXChartResizeChart(); };

  WXCharttheChart.on('dblclick', 'yAxis', function (params) {
    let p = {};
    
    p.yAxisIndex = params.yAxisIndex;
    p.targetType = params.targetType;
    
    WXChartSendMessage('dblclick\tyAxis', p);
  });

  WXCharttheChart.on('dblclick', 'series', function (params) {
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
    WXChartSendMessage('dblclick\tseries', p);
  });

  WXCharttheChart.on('contextmenu', function (params) {
    let p = {};
    
    p.componentType = params.componentType;
    p.clientX = params.event.event.clientX;
    p.clientY = params.event.event.clientY;
    WXChartSendMessage('contextmenu\tchart', p);
    params.event.stop();
  });  

  window.oncontextmenu = function (event) 
    { 
      let p = {};

      p.clientX = event.clientX;
      p.clientY = event.clientY;
      WXChartSendMessage('contextmenu\tnochart', p);
      event.preventDefault();
    }
}

function WXChartResizeChart() {
  const dom = WXCharttheChart.getDom();
  const divWidth = dom.clientWidth;
  const divHeight = dom.clientHeight;
  let chartWidth = divWidth;
  let chartHeight = chartWidth / WXChartSizingOptions.widthToHeightRatio;

  if (chartHeight >= divHeight) {
    chartHeight = divHeight;
    chartWidth = divHeight * WXChartSizingOptions.widthToHeightRatio;
  }

  if (chartWidth >= WXChartSizingOptions.minWidth && chartHeight >= WXChartSizingOptions.minHeight)
    WXCharttheChart.resize({ width: chartWidth, height: chartHeight });
}

function WXChartUpdateSeries(seriesJSON) {
  try {
    WXCharttheChart.setOption(JSON.parse(seriesJSON));
  } catch (e) {
    WXChartSendErrorMessageJSON(e.name, e.message, arguments.callee.name);
  }
}

function WXChartUpdateVariableNames(variableNamesJSON) {
  try {
    WXCharttheChart.setOption({xAxis : { data: JSON.parse(variableNamesJSON) } });
  } catch (e) {
    WXChartSendErrorMessageJSON(e.name, e.message, arguments.callee.name);
  }
}

function WXChartSaveChartAsImage(width) {
  return WXCharttheChart.getDataURL({ type: 'png', pixelRatio: width / WXCharttheChart.getWidth(), backgroundColor: 'white' });
}

function WXChartGetChartColors() {
  return JSON.stringify(WXCharttheChart.getOption().color);
}

function WXChartSetChartColors(colorsJSON) {  
  try {
    WXCharttheChart.setOption({color: JSON.parse(colorsJSON)});
  } catch (e) {
    WXChartSendErrorMessageJSON(e.name, e.message, arguments.callee.name);
  }
}

function WXChartGetChartSizingOptions() {
  return JSON.stringify(WXChartSizingOptions);
}

function WXChartSetChartSizingOptions(optionsJSON) {
  try {
    var o = JSON.parse(optionsJSON);
  } catch (e) {
    WXChartSendErrorMessageJSON(e.name, e.message, arguments.callee.name);
    return;
  }

  WXChartSizingOptions.widthToHeightRatio = o.widthToHeightRatio;
  WXChartSizingOptions.minWidth = o.minWidth;
  WXChartSizingOptions.minHeight = o.minHeight;

  WXChartResizeChart();
}