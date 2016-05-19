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
var globalPp;

const PPOptionsPanel = React.createClass({
  componentDidUpdate: function() {
    componentHandler.upgradeDom();
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
    YumeCef.RegisterComponent("PPPanel",this);

    for(var [key,value] of YumeCef.Constants) {
      Cef3DTools.SetElementValue(key,value);
    }

    var inst = this;
    $("#fogColorPicker").ColorPicker({
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
    		$('#fogColorPicker').css('backgroundColor', '#' + hex);
        YumeCef.SendDomEvent(inst.onSendDomEvent,"godraysFogColor","InputValueChanged","" + rgb.r + " " + rgb.g + " " + rgb.b + "");
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
        <div>
          <div className="optionsInner">HBAO</div>
          <div className="optionsGrid">
            <div className="row">
              <div className="col-xs-5">
                <div className="box optionCaption">Angle Bias</div>
              </div>
              <div className="col-xs-5">
                <div className="box">
                  <input className="mdl-slider mdl-js-slider" type="range" id="sliderHBAOAngleBiasValue" min="0" max="60" step="1" onChange={this.onSliderChange} />
                </div>
              </div>
              <div className="col-xs-2">
                <div id="HBAOAngleBiasValue">{YumeCef.Constants.get("HBAOAngleBiasValue")}</div>
              </div>

              <div className="col-xs-5">
                <div className="box optionCaption">Num Directions</div>
              </div>
              <div className="col-xs-5">
                <div className="box">
                  <input className="mdl-slider mdl-js-slider" type="range" id="sliderHBAONumDirsValue" min="0" max="32" step="1" onChange={this.onSliderChange} />
                </div>
              </div>
              <div className="col-xs-2">
                <div id="HBAONumDirsValue">{YumeCef.Constants.get("HBAONumDirsValue")}</div>
              </div>

              <div className="col-xs-5">
                <div className="box optionCaption">Number of Steps</div>
              </div>
              <div className="col-xs-5">
                <div className="box">
                  <input className="mdl-slider mdl-js-slider" type="range" id="sliderHBAONumStepsValue" min="0" max="16" step="1" onChange={this.onSliderChange} />
                </div>
              </div>
              <div className="col-xs-2">
                <div id="HBAONumStepsValue">{YumeCef.Constants.get("HBAONumStepsValue")}</div>
              </div>

              <div className="col-xs-5">
                <div className="box optionCaption">Contrast</div>
              </div>
              <div className="col-xs-5">
                <div className="box">
                  <input className="mdl-slider mdl-js-slider" type="range" id="sliderHBAOContrastValue" min="0" max="5" step="0.125" onChange={this.onSliderChange} />
                </div>
              </div>
              <div className="col-xs-2">
                <div id="HBAOContrastValue">{YumeCef.Constants.get("HBAOContrastValue")}</div>
              </div>

              <div className="col-xs-5">
                <div className="box optionCaption">Attenuation</div>
              </div>
              <div className="col-xs-5">
                <div className="box">
                  <input className="mdl-slider mdl-js-slider" type="range" id="sliderHBAOAttenValue" min="0" max="1" step="0.1" onChange={this.onSliderChange} />
                </div>
              </div>
              <div className="col-xs-2">
                <div id="HBAOAttenValue">{YumeCef.Constants.get("HBAOAttenValue")}</div>
              </div>

              <div className="col-xs-5">
                <div className="box optionCaption">Radius</div>
              </div>
              <div className="col-xs-5">
                <div className="box">
                  <input className="mdl-slider mdl-js-slider" type="range" id="sliderHBAORadiusValue" min="0" max="50" step="1" onChange={this.onSliderChange} />
                </div>
              </div>
              <div className="col-xs-2">
                <div id="HBAORadiusValue">{YumeCef.Constants.get("HBAORadiusValue")}</div>
              </div>

              <div className="col-xs-5">
                <div className="box optionCaption">Blur Radius</div>
              </div>
              <div className="col-xs-5">
                <div className="box">
                  <input className="mdl-slider mdl-js-slider" type="range" id="sliderHBAOBlurRadiusValue" min="0" max="15" step="0.5" onChange={this.onSliderChange} />
                </div>
              </div>
              <div className="col-xs-2">
                <div id="HBAOBlurRadiusValue">{YumeCef.Constants.get("HBAOBlurRadiusValue")}</div>
              </div>

              <div className="col-xs-5">
                <div className="box optionCaption">Blur Sharpness</div>
              </div>
              <div className="col-xs-5">
                <div className="box">
                  <input className="mdl-slider mdl-js-slider" type="range" id="sliderHBAOBlurSharpness" min="0" max="32" step="1" onChange={this.onSliderChange} />
                </div>
              </div>
              <div className="col-xs-2">
                <div id="HBAOBlurSharpnessValue">{YumeCef.Constants.get("HBAOBlurSharpnessValue")}</div>
              </div>

              <div className="col-xs-5">
                <div className="box optionCaption">Edge Threshold</div>
              </div>
              <div className="col-xs-5">
                <div className="box">
                  <input className="mdl-slider mdl-js-slider" type="range" id="sliderHBAOBlurEdgeThresholdValue" min="0" max="1" step="0.1" onChange={this.onSliderChange} />
                </div>
              </div>
              <div className="col-xs-2">
                <div id="HBAOBlurEdgeThresholdValue">{YumeCef.Constants.get("HBAOBlurEdgeThresholdValue")}</div>
              </div>

            </div>
          </div>
          <div className="optionsInner">Bloom</div>
          <div className="optionsGrid">
            <div className="row">

              <div className="col-xs-5">
                <div className="box optionCaption">Threshold</div>
              </div>
              <div className="col-xs-5">
                <div className="box">
                  <input className="mdl-slider mdl-js-slider" type="range" id="sliderbloomTresholdValue" min="0" max="1" step="0.1" onChange={this.onSliderChange} />
                </div>
              </div>
              <div className="col-xs-2">
                <div id="bloomTresholdValue">{YumeCef.Constants.get("bloomTresholdValue")}</div>
              </div>

              <div className="col-xs-5">
                <div className="box optionCaption">Sigma</div>
              </div>
              <div className="col-xs-5">
                <div className="box">
                  <input className="mdl-slider mdl-js-slider" type="range" id="sliderbloomSigmaValue" min="0" max="1" step="0.1" onChange={this.onSliderChange} />
                </div>
              </div>
              <div className="col-xs-2">
                <div id="bloomSigmaValue">{YumeCef.Constants.get("bloomSigmaValue")}</div>
              </div>


            </div>
            <div className="optionsInner">Auto Exposure</div>
            <div className="optionsGrid">
                <div className="row">
                  <div className="col-xs-5">
                    <div className="box optionCaption">Exposure Speed</div>
                  </div>
                  <div className="col-xs-5">
                    <div className="box">
                      <input className="mdl-slider mdl-js-slider" type="range" id="sliderexposureSpeedValue" min="0" max="1" step="0.05" onChange={this.onSliderChange} />
                    </div>
                  </div>

                  <div className="col-xs-2">
                    <div id="exposureSpeedValue">{YumeCef.Constants.get("exposureSpeedValue")}</div>
                  </div>

                  <div className="col-xs-5">
                    <div className="box optionCaption">Exposure Key</div>
                  </div>
                  <div className="col-xs-5">
                    <div className="box">
                      <input className="mdl-slider mdl-js-slider" type="range" id="sliderexposureKeyValue" min="0" max="1" step="0.05" onChange={this.onSliderChange} />
                    </div>
                  </div>
                  <div className="col-xs-2">
                    <div id="exposureKeyValue">{YumeCef.Constants.get("exposureKeyValue")}</div>
                  </div>

                </div>
              </div>

              <div className="optionsInner">Godrays</div>
              <div className="optionsGrid">
                  <div className="row">
                    <div className="col-xs-5">
                      <div className="box optionCaption">TAU</div>
                    </div>
                    <div className="col-xs-5">
                      <div className="box">
                        <input className="mdl-slider mdl-js-slider" type="range" id="sliderGodraysTauValue" min="0" max="0.001" step="0.0005" onChange={this.onSliderChange} />
                      </div>
                    </div>
                    <div className="col-xs-2">
                      <div id="GodraysTauValue">{YumeCef.Constants.get("GodraysTauValue")}</div>
                    </div>


                    <div className="col-xs-5">
                      <div className="box optionCaption">Mist Speed</div>
                    </div>
                    <div className="col-xs-5">
                      <div className="box">
                        <input className="mdl-slider mdl-js-slider" type="range" id="sliderGodraysMistSpeedValue" min="0" max="50" step="1" onChange={this.onSliderChange} />
                      </div>
                    </div>
                    <div className="col-xs-2">
                      <div id="GodraysMistSpeedValue">{YumeCef.Constants.get("GodraysMistSpeedValue")}</div>
                    </div>

                    <div className="col-xs-5">
                      <div className="box optionCaption">Num Samples</div>
                    </div>
                    <div className="col-xs-5">
                      <div className="box">
                        <input className="mdl-slider mdl-js-slider" type="range" id="sliderGodraysNumSamplesValue" min="0" max="100" step="1" onChange={this.onSliderChange} />
                      </div>
                    </div>
                    <div className="col-xs-2">
                      <div id="GodraysNumSamplesValue">{YumeCef.Constants.get("GodraysNumSamplesValue")}</div>
                    </div>

                    <div className="col-xs-5">
                      <div className="box optionCaption">Dithering</div>
                    </div>
                    <div className="col-xs-5">
                      <div className="box">
                        <input className="mdl-slider mdl-js-slider" type="range" id="sliderGodraysDitheringValue" min="0" max="100" step="1" onChange={this.onSliderChange} />
                      </div>
                    </div>
                    <div className="col-xs-2">
                      <div id="GodraysDitheringValue">{YumeCef.Constants.get("GodraysDitheringValue")}</div>
                    </div>

                    <div className="col-xs-5">
                      <div className="box optionCaption">Fog Color</div>
                    </div>
                    <div className="col-xs-7">
                      <div className="box">
                        <div id="fogColorPicker">Click to pick a color</div>
                      </div>
                    </div>

                  </div>
                </div>

              <div className="optionsInner">Depth of Field</div>
              <div className="optionsGrid">
                  <div className="row">
                    <div className="col-xs-5">
                      <div className="box optionCaption">CoC Scale</div>
                    </div>
                    <div className="col-xs-5">
                      <div className="box">
                        <input className="mdl-slider mdl-js-slider" type="range" id="sliderDOFScale" min="0" max="0.01" step="0.0005" onChange={this.onSliderChange} />
                      </div>
                    </div>
                    <div className="col-xs-2">
                      <div id="value">Value</div>
                    </div>

                    <div className="col-xs-5">
                      <div className="box optionCaption">Focal Plane</div>
                    </div>
                    <div className="col-xs-5">
                      <div className="box">
                        <input className="mdl-slider mdl-js-slider" type="range" id="sliderDOFFocalPlane" min="0" max="1" step="0.1" onChange={this.onSliderChange} />
                      </div>
                    </div>
                    <div className="col-xs-2">
                      <div id="value">Value</div>
                    </div>
                  </div>
                </div>
          </div>
        </div>
      </div>
    )
  }
});

module.exports = PPOptionsPanel;
