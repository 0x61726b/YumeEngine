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

OverlayGlobal = { };

var MemoryUsagePanel = React.createClass({
  getInitialState: function() {
    return { memoryString: "huehue" };
  },
  componentWillMount: function() {
    OverlayGlobal.setMemoryInfo = (data) => {
      this.setState( { memoryString: data.MemoryInfo });
    };
  },
  render: function() {
    return (<div className="memoryUsagePanel">Yume Engine Alpha</div>
    );
  }
});

var CameraPanel = React.createClass({
  getInitialState: function() {
    return { cameraPos: "",cameraRot:"",fov:"" };
  },
  componentWillMount: function() {
    OverlayGlobal.setCameraInfo = (data) => {
      this.setState( { cameraPos: data.CameraPos,cameraRot: data.CameraRot,fov: data.CameraFov });
    };
  },
  render: function() {
    return (
      <div className="cameraPanel">Camera Position: <div>{ this.state.cameraPos }</div> Rotation: <div>{this.state.cameraRot}</div> Fov: <div>{this.state.fov}</div></div>
    );
  }
});

var Overlay = React.createClass( {
  getInitialState: function() {
    return { fps:"199.23492482",ms:"",sampleName: "Sample Name",
    totalMemory:"12 mb",materialQuality:"High",shadowInfo:"test",textureQuality:"High",
    softwareOcc:0,batchCount:0,lightCount:0,occluders:0,primitiveCount:0,shadowMaps:0
   };
  },
  componentWillMount: function() {
    OverlayGlobal.setSampleInfo = (data) => {
      this.setState( { sampleName: data.SampleName });
    };
    OverlayGlobal.setFrameInfo = (data) => {
      var cutFps = data.FrameRate;
      cutFps = cutFps.substring(0,6);

      var cutMs = data.ElapsedTime;
      cutMs = cutMs.substring(0,3);
      this.setState( { ms: cutMs,
        fps: cutFps,
        shadowInfo: data.ShadowInfo,
        materialQuality:"Ultra",
        textureQuality:"Ultra",
        primitiveCount:data.PrimitiveCount,
        batchCount:data.BatchCount,
        lightCount:data.LightCount,
        shadowMaps:data.ShadowMaps,
        occluders:data.Occluders,
        totalMemory:data.TotalMemory
       });
    };
    OverlayGlobal.setTotalMemory = (data) => {
      var memory = data.TotalMemory;
      memory = memory / 1024;
      memory = memory / 1024;
      memory = memory.substring(0,6);
      this.setState( { totalMemory: memory });
    };
  },
  onFpsClick: function() {

  },
  render: function() {
    return (
      <div id="topLeftInfo">
        <div id="sampleName">
          Application Name: <p>{this.state.sampleName}</p>
        </div>
        <div id="fps" onClick={this.onFpsClick}>
          FPS: <p>{this.state.fps}</p> <div className="innerMs">({this.state.ms} ms)</div>
        </div>
        <div id="totalMemory">
          Memory Usage: <p>{this.state.totalMemory}</p> MBs
        </div>
        <div id="textureQuality">
          Texture Quality: <p>{this.state.textureQuality}</p>
        </div>
        <div id="materialQuality">
          Material Quality: <p>{this.state.materialQuality}</p>
        </div>
        <div id="primitiveCount">
          Primitive Count: <p>{this.state.primitiveCount}</p>
        </div>
        <div id="batchCount">
          Total Batches: <p>{this.state.batchCount}</p>
        </div>
      </div>
    );
  }
});

var PostFxPanel = React.createClass({
  componentDidMount: function() {
    $("#iGiScale").prop("value","2");
    $("#iGiLPVFlux").prop("value","4");
    $("#iLpvX").prop("value","0");
    $("#iLpvY").prop("value","0");
    $("#iLpvZ").prop("value","0");
    $("#iGiPropagations").prop("value","64");
    $("#iGiLightFlux").prop("value","1");

    //Post FX
    this.setDefaultValue("bloomThreshold","0.5");
    this.setDefaultValue("bloomSigma","0.5");

    this.setDefaultValue("ssaoScale","0.1");

    this.setDefaultValue("aeExposureKey","0.05");
    this.setDefaultValue("aeAdaptSpeed","0.4");

    this.setDefaultValue("dofCocScale","0.8");
    this.setDefaultValue("dofFocalPlane","2000");

    this.setDefaultValue("gTau","0.0005");

  },
  setDefaultValue: function(element,value) {
    $("#" + element).prop("value",value);
  },
  getInitialState: function() {
    return { bloomOn:false,bloomHDROn:false,fxaaOn:false,blurOn:false,autoExposureOn:false };
  },
  openOptionsPanel: function(element) {
    $("." + $(element.target).attr("id") + "Options").slideToggle("fast");
  },
  onSendDomEvent: function() {

  },
  onSwitchChange: function(element) {
    this.openOptionsPanel(element);
    Cef3D.SendDomEvent(this.onSendDomEvent,$(element.target).attr("id"),"InputChecked","" + $(element.target).is(':checked') + "");
  },
  onSliderChange: function(element) {
    Cef3D.SendDomEvent(this.onSendDomEvent,$(element.target).attr("id"),"InputValueChanged","" + element.target.value + "");
  },
  onCheckboxChange: function(element) {
    //
    // if($(element.target).attr("id") == "ssaoInput")
    // {
    //   this.enableInput($("#ssaoRadius"));
    //   this.enableInput($("#blurPass"));
    // }

    Cef3D.SendDomEvent(this.onSendDomEvent,$(element.target).attr("id"),"InputChecked","" + $(element.target).is(':checked') + "");
  },
  enableInput: function(element) {
    element.prop("disabled",!element.attr("disabled"));
  },
  render: function() {
    return(
      <div>
        <div className="postFxPanel">
          <div>Post Processing Options</div>
          <div id="bloom">
            <label className="mdl-switch mdl-js-switch mdl-js-ripple-effect" htmlFor="bloomSwitch">
              <span className="mdl-switch__label">Bloom</span>
              <input type="checkbox" id="bloomSwitch" className="mdl-switch__input" onChange={this.onSwitchChange} />
            </label>
            <div className="bloomSwitchOptions">
                <span>Threshold</span><input className="mdl-slider mdl-js-slider" type="range" id="bloomThreshold" min="0" max="1" step="0.1" onChange={this.onSliderChange} />
                <span>Sigma</span><input className="mdl-slider mdl-js-slider" type="range" id="bloomSigma" min="0" max="1" step="0.1" onChange={this.onSliderChange} />
            </div>
          </div>

          <div id="fxaa">
            <label className="mdl-switch mdl-js-switch mdl-js-ripple-effect" htmlFor="fxaaSwitch">
              <span className="mdl-switch__label">FXAA</span>
              <input type="checkbox" id="fxaaSwitch" className="mdl-switch__input" onChange={this.onSwitchChange} />
            </label>
          </div>

          <div id="ssao">
            <label className="mdl-switch mdl-js-switch mdl-js-ripple-effect" htmlFor="ssaoSwitch">
              <span className="mdl-switch__label">SSAO</span>
              <input type="checkbox" id="ssaoSwitch" className="mdl-switch__input" onChange={this.onSwitchChange} />
            </label>
            <div className="ssaoSwitchOptions">
              <p style={{width:"300px"}}>
                <span>Scale</span><input className="mdl-slider mdl-js-slider" type="range" id="ssaoScale" min="0" max="1" step="0.1" onChange={this.onSliderChange} />
              </p>
            </div>
          </div>

          <div id="autoexposure">
            <label className="mdl-switch mdl-js-switch mdl-js-ripple-effect" htmlFor="autoexposureSwitch">
              <span className="mdl-switch__label">Auto Exposure</span>
              <input type="checkbox" id="autoexposureSwitch" className="mdl-switch__input" onChange={this.onSwitchChange} />
            </label>
            <div className="autoexposureSwitchOptions">
              <p style={{width:"300px"}}>
                <span>Exposure Key</span><input className="mdl-slider mdl-js-slider" type="range" id="aeExposureKey" min="0" max="0.2" step="0.01" onChange={this.onSliderChange} />
                <span>Adapt Speed</span><input className="mdl-slider mdl-js-slider" type="range" id="aeAdaptSpeed" min="0" max="1" step="0.1" onChange={this.onSliderChange} />
              </p>
            </div>
          </div>

          <div id="depthOfField">
            <label className="mdl-switch mdl-js-switch mdl-js-ripple-effect" htmlFor="dofSwitch">
              <span className="mdl-switch__label">Bokeh DoF</span>
              <input type="checkbox" id="dofSwitch" className="mdl-switch__input" onChange={this.onSwitchChange} />
            </label>
            <div className="dofSwitchOptions">
              <p style={{width:"300px"}}>
                <span>CoC Scale</span><input className="mdl-slider mdl-js-slider" type="range" id="dofCocScale" min="0" max="2" step="0.1" onChange={this.onSliderChange} />
                <span>Focal Plane</span><input className="mdl-slider mdl-js-slider" type="range" id="dofFocalPlane" min="0" max="3000" step="100" onChange={this.onSliderChange} />
              </p>
            </div>
          </div>

          <div id="godrays">
            <label className="mdl-switch mdl-js-switch mdl-js-ripple-effect" htmlFor="godraysSwitch">
              <span className="mdl-switch__label">God Rays</span>
              <input type="checkbox" id="godraysSwitch" className="mdl-switch__input" onChange={this.onSwitchChange} />
            </label>
            <div className="godraysSwitchOptions">
              <p style={{width:"300px"}}>
                <span>TAU</span><input className="mdl-slider mdl-js-slider" type="range" id="gTau" min="0" max="0.01" step="0.0005" onChange={this.onSliderChange} />
              </p>
            </div>
          </div>
        </div>
        <div className="advancedOptsPanel">
          <div>LPV Options</div>
          <div className="ssaoOptions">
            <label className="mdl-checkbox mdl-js-checkbox mdl-js-ripple-effect" htmlFor="debugIndirect">
              <input type="checkbox" id="debugIndirect" className="mdl-checkbox__input" onChange={this.onCheckboxChange} />
              <span className="mdl-checkbox__label">Show Indirect Term</span>
            </label>
            <span>GI Scale</span><input className="mdl-slider mdl-js-slider" type="range" id="iGiScale" min="0" max="10" step="0.2" onChange={this.onSliderChange} />
            <span>LPV Flux Amp</span><input className="mdl-slider mdl-js-slider" type="range" id="iGiLPVFlux" min="0" max="5" step="0.25" onChange={this.onSliderChange} />
            <span>Number of Propagations</span><input className="mdl-slider mdl-js-slider" type="range" id="iGiPropagations" min="0" max="64" step="1" onChange={this.onSliderChange} />
            <span>Light Flux</span><input className="mdl-slider mdl-js-slider" type="range" id="iGiLightFlux" min="1" max="5" step="0.25" onChange={this.onSliderChange} />
            <div className="ssaoAdditional">
              <span>LPV Pos X</span><input className="mdl-slider mdl-js-slider" type="range" id="iLpvX" min="-1000" max="1000" step="10" onChange={this.onSliderChange} />
              <span>LPV Pos Y</span><input className="mdl-slider mdl-js-slider" type="range" id="iLpvY" min="-1000" max="1000" step="10" onChange={this.onSliderChange} />
              <span>LPV Pos Z</span><input className="mdl-slider mdl-js-slider" type="range" id="iLpvZ" min="-1000" max="1000" step="10" onChange={this.onSliderChange} />
            </div>
          </div>
        </div>
      </div>
    );
  }
});

 window.addEventListener('setFrameInfo',handleOverlay);
 window.addEventListener('setMemoryInfo',handleMemoryInfo);
 window.addEventListener('setSampleName',handleSampleInfo);
 window.addEventListener('setCameraInfo',handleCameraInfo);

function handleOverlay (e) {
   OverlayGlobal.setFrameInfo(e.detail);
 }
 function handleSampleInfo (e) {
   OverlayGlobal.setSampleInfo(e.detail);
 }
 function handleMemoryInfo (e) {
   OverlayGlobal.setMemoryInfo(e.detail);
 }
function handleCameraInfo (e) {
  OverlayGlobal.setCameraInfo(e.detail);
}


 document.addEventListener("DOMContentLoaded", function() {
   Cef3D.OnDomReady();
 }, false);


var Root = React.createClass({
  onChange: function(test) {
    return test;
  },
  render: function() {
    return(
      <div>
      <Overlay />
      <CameraPanel />
      <MemoryUsagePanel />
      <PostFxPanel />
      </div>
    );
  }
});

ReactDOM.render(
  React.createElement(Root),
  document.getElementById('main')
);
