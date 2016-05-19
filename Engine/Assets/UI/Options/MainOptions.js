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

const OptionsPanel = React.createClass({
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
    YumeCef.RegisterComponent("MainOptions",this);
    Cef3DTools.SetCheckbox("BloomEnabled",true);
    Cef3DTools.SetCheckbox("AE",true);

    for(var [key,value] of YumeCef.Constants) {
      Cef3DTools.SetElementValue(key,value);
    }

    var inst = this;
    $("#lightColorPicker").ColorPicker({
      color: '#0000ff',
      onShow: function (colpkr) {
        $(colpkr).fadeIn(500);
        return false;
      },
      onHide: function (colpkr) {
        $(colpkr).fadeOut(500);
        return false;
      },
      onChange: function (hsb, hex, rgb) {
        $('#lightColorPicker').css('backgroundColor', '#' + hex);
        YumeCef.SendDomEvent(inst.onSendDomEvent,"lightColor","InputValueChanged","" + rgb.r + " " + rgb.g + " " + rgb.b + "");
        console.log(rgb);
      }
    })
  },
  refresh: function() {
    this.forceUpdate();
  },
  onSendDomEvent: function() {

  },
  render: function() {
    return(
      <div>
      <div className="optionsInner">General Options</div>
      <div className="optionsGrid">
      <div className="row">
          <div className="col-xs-5">
            <div className="box optionCaption">Light Flux</div>
          </div>
          <div className="col-xs-5">
            <div className="box">
              <input className="mdl-slider mdl-js-slider" type="range" id="sliderlightFluxValue" min="0" max="100" step="1" onChange={this.onSliderChange} />
            </div>
          </div>

          <div className="col-xs-2">
            <div id="lightFluxValue">{YumeCef.Constants.get("lightFluxValue")}</div>
          </div>

          <div className="col-xs-5">
            <div className="box optionCaption">Warmth</div>
          </div>
          <div className="col-xs-5">
            <div className="box">
              <input className="mdl-slider mdl-js-slider" type="range" id="sliderlightWarmthValue" min="0" max="100" step="1" onChange={this.onSliderChange} />
            </div>
          </div>

          <div className="col-xs-2">
            <div id="lightWarmthValue">{YumeCef.Constants.get("lightWarmthValue")}</div>
          </div>

          <div className="col-xs-5">
            <div className="box optionCaption">Light Color</div>
          </div>
          <div className="col-xs-5">
            <div id="lightColorPicker">Click to pick a color</div>
          </div>
        </div>
      </div>
      <div className="optionsInner">Post Processing</div>
      <div className="optionsGrid">
      <div className="column">
        <div className="col-xs-12">
          <label className="mdl-switch mdl-js-switch mdl-js-ripple-effect" htmlFor="switchBloomEnabled">
            <span className="mdl-switch__label">Bloom</span>
            <input type="checkbox" id="switchBloomEnabled" className="mdl-switch__input" onChange={this.onSwitchChange} />
          </label>
        </div>
        <div className="col-xs-12">
          <label className="mdl-switch mdl-js-switch mdl-js-ripple-effect" htmlFor="switchAE">
            <span className="mdl-switch__label">Auto Exposure</span>
            <input type="checkbox" id="switchAE" className="mdl-switch__input" onChange={this.onSwitchChange} />
          </label>
        </div>
        <div className="col-xs-12">
          <label className="mdl-switch mdl-js-switch mdl-js-ripple-effect" htmlFor="switchHBAO">
            <span className="mdl-switch__label">HBAO</span>
            <input type="checkbox" id="switchHBAO" className="mdl-switch__input" onChange={this.onSwitchChange} />
          </label>
        </div>
        <div className="col-xs-12">
          <label className="mdl-switch mdl-js-switch mdl-js-ripple-effect" htmlFor="switchDOF">
            <span className="mdl-switch__label">Depth Of Field</span>
            <input type="checkbox" id="switchDOF" className="mdl-switch__input" onChange={this.onSwitchChange} />
          </label>
        </div>
        <div className="col-xs-12">
          <label className="mdl-switch mdl-js-switch mdl-js-ripple-effect" htmlFor="switchGodrays">
            <span className="mdl-switch__label">God Rays</span>
            <input type="checkbox" id="switchGodrays" className="mdl-switch__input" onChange={this.onSwitchChange} />
          </label>
        </div>
        <div className="col-xs-12">
          <label className="mdl-switch mdl-js-switch mdl-js-ripple-effect" htmlFor="switchFXAA">
            <span className="mdl-switch__label">FXAA</span>
            <input type="checkbox" id="switchFXAA" className="mdl-switch__input" onChange={this.onSwitchChange} />
          </label>
        </div>
        <div className="col-xs-12">
          <label className="mdl-switch mdl-js-switch mdl-js-ripple-effect" htmlFor="switchLensDistortion">
            <span className="mdl-switch__label">Lens Distortion</span>
            <input type="checkbox" id="switchLensDistortion" className="mdl-switch__input" onChange={this.onSwitchChange} />
          </label>
        </div>
      </div>
    </div>
  </div>
    )
  }
});

module.exports = OptionsPanel;
