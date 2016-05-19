//###############################################################################
//Yume Engine MIT License (MIT)

// Copyright (c) 2015 arkenthera
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// File : CMakeLists.txt
// Date : 8.27.2015
// Comments :
//###############################################################################

var Tools = require('./Tools');

var YumeCef;
if(typeof Cef3D === "undefined") {
  YumeCef = Tools.Cef3D;
  console.log("Creating Mock");
}
else {
  YumeCef = Cef3D;
}

var Cef3DTools = Tools.Cef3DTools;

const GIOptionsPanel = React.createClass({
  componentDidUpdate: function() {
    componentHandler.upgradeDom();
  },
  onSwitchChange: function(slider) {
    //var id = Cef3DTools.GetId(slider) + "Value";
    //Cef3DTools.SetValue(id,slider.target.value);
    //YumeCef.SendDomEvent(this.onSendDomEvent,Cef3DTools.GetId(slider),"InputValueChanged","" + slider.target.value + "");
    var id = Cef3DTools.GetId(slider);
    var vName = id.substr(6,id.length - 1);

    Cef3DTools.SetValue(vName,slider.target.value);
    YumeCef.SendDomEvent(this.onSendDomEvent,Cef3DTools.GetId(slider),"InputChecked","" + $(slider.target).is(':checked') + "");
    YumeCef.Constants.set(vName,slider.target.value);
  },
  onSliderChange: function(slider) {
    //var id = Cef3DTools.GetId(slider) + "Value";
    //Cef3DTools.SetValue(id,slider.target.value);
    //YumeCef.SendDomEvent(this.onSendDomEvent,Cef3DTools.GetId(slider),"InputValueChanged","" + slider.target.value + "");
    var id = Cef3DTools.GetId(slider);
    var vName = id.substr(6,id.length - 1);

    Cef3DTools.SetValue(vName,slider.target.value);
    YumeCef.SendDomEvent(this.onSendDomEvent,Cef3DTools.GetId(slider),"InputValueChanged","" + slider.target.value + "");
    YumeCef.Constants.set(vName,slider.target.value);
  },
  componentDidMount: function() {
    YumeCef.RegisterComponent("GIPanel",this);

    for(var [key,value] of YumeCef.Constants) {
      Cef3DTools.SetElementValue(key,value);
    }
  },
  refresh: function() {
    this.forceUpdate();
  },
  onSendDomEvent: function() {

  },
  render: function() {
    return(
      <div>
        <div>
        <div className="optionsInner">GI Options</div>
        <div className="optionsGrid">
        <div className="col-xs-12">
          <label className="mdl-switch mdl-js-switch mdl-js-ripple-effect" htmlFor="switchDebugGI">
            <span className="mdl-switch__label">Show Indirect ontribution only</span>
            <input type="checkbox" id="switchDebugGI" className="mdl-switch__input" onChange={this.onSwitchChange} />
          </label>
        </div>
        <div className="row">
            <div className="col-xs-3">
              <div className="box optionCaption">GI Scale</div>
            </div>
            <div className="col-xs-7">
              <div className="box">
                <input className="mdl-slider mdl-js-slider" type="range" id="slidergiScaleValue" min="0" max="250" step="1" onChange={this.onSliderChange} />
              </div>
            </div>

            <div className="col-xs-2">
              <div id="giScaleValue">{YumeCef.Constants.get("giScaleValue")}</div>
            </div>
          </div>
        </div>
          <div className="optionsInner">Light Propagation Volumes</div>
          <div className="optionsGrid">
            <div className="row">
              <div className="col-xs-3">
                <div className="box optionCaption">Flux</div>
              </div>
              <div className="col-xs-7">
                <div className="box">
                  <input className="mdl-slider mdl-js-slider" type="range" id="sliderlpvFluxValue" min="0" max="4" step="0.2" onChange={this.onSliderChange} />
                </div>
              </div>

              <div className="col-xs-2">
                <div id="lpvFluxValue"> {YumeCef.Constants.get("lpvFluxValue") }</div>
              </div>

            </div>
          </div>
          <div className="optionsInner">VCT</div>
          <div className="optionsGrid">
            <div className="row">
              <div className="col-xs-5">
                <div className="box optionCaption">Floor Roughness</div>
              </div>
              <div className="col-xs-5">
                <div className="box">
                  <input className="mdl-slider mdl-js-slider" type="range" id="sliderVCTRoughnessValue" min="0" max="1" step="0.05" onChange={this.onSliderChange} />
                </div>
              </div>
              <div className="col-xs-2">
                <div id="VCTRoughnessValue">{YumeCef.Constants.get("VCTRoughnessValue")}</div>
              </div>
            </div>
          </div>
        </div>
      </div>
    )
  }
});

module.exports = GIOptionsPanel;
