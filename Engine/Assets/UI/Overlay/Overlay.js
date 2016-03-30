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
    return (
      <div className="memoryUsagePanel">Yume Engine Alpha</div>
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
    return { fps:0,ms:0,sampleName: "Sample Name",
    totalMemory:"12 mb",materialQuality:"High",shadowInfo:"test",textureQuality:"High",
    softwareOcc:0,batchCount:0,lightCount:0,occluders:0,primitiveCount:0,shadowMaps:0
   };
  },
  componentWillMount: function() {
    OverlayGlobal.setSampleInfo = (data) => {
      this.setState( { sampleName: data.SampleName });
    };
    OverlayGlobal.setFrameInfo = (data) => {
      this.setState( { ms: data.ElapsedTime,
        fps: data.FrameRate,
        shadowInfo: data.ShadowInfo,
        materialQuality:data.MaterialQuality,
        textureQuality:data.TextureQuality,
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
          FPS: <p>{this.state.fps}</p> <span>({this.state.ms} ms)</span>
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
        <div id="lightCount">
          Light Count: <p>{this.state.lightCount}</p>
        </div>
        <div id="shadows">
          Shadows: <p>{this.state.shadowInfo}</p>
        </div>
        <div id="shadowMapCount">
          ShadowMaps: <p>{this.state.shadowMaps}</p>
        </div>
        <div id="occluders">
          Occluders: <p>{this.state.occluders}</p>
        </div>
      </div>
    );
  }
});

var PostFxPanel = React.createClass({
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
                <span>Option 1</span><input className="mdl-slider mdl-js-slider" type="range" id="s1" min="0" max="10" value="8" onChange={this.onSliderChange} />
                <span>Option 2</span><input className="mdl-slider mdl-js-slider" type="range" id="s2" min="0" max="10" value="8" onChange={this.onSliderChange} />
            </div>
          </div>

          <div id="bloomHDR">
            <label className="mdl-switch mdl-js-switch mdl-js-ripple-effect" htmlFor="bloomHDRSwitch">
              <span className="mdl-switch__label">Bloom HDR</span>
              <input type="checkbox" id="bloomHDRSwitch" className="mdl-switch__input" onChange={this.onSwitchChange} />
            </label>
            <div className="bloomHDRSwitchOptions">
              <p style={{width:"300px"}}>
                <span>Threshold</span><input className="mdl-slider mdl-js-slider" type="range" id="bloomHDRThreshold" min="0" max="10" value="8" onChange={this.onSliderChange} />
              </p>
            </div>
          </div>

          <div id="fxaa">
            <label className="mdl-switch mdl-js-switch mdl-js-ripple-effect" htmlFor="fxaaSwitch">
              <span className="mdl-switch__label">FXAA</span>
              <input type="checkbox" id="fxaaSwitch" className="mdl-switch__input" onChange={this.onSwitchChange} />
            </label>
          </div>

          <div id="blur">
            <label className="mdl-switch mdl-js-switch mdl-js-ripple-effect" htmlFor="blurSwitch">
              <span className="mdl-switch__label">Blur</span>
              <input type="checkbox" id="blurSwitch" className="mdl-switch__input" onChange={this.onSwitchChange} />
            </label>
            <div className="blurSwitchOptions">
              <p style={{width:"300px"}}>
                <span>Option 1</span><input className="mdl-slider mdl-js-slider" type="range" id="s1" min="0" max="10" value="4" step="2" onChange={this.onSliderChange} />
                <span>Option 2</span><input className="mdl-slider mdl-js-slider" type="range" id="s1" min="0" max="10" value="4" step="2" onChange={this.onSliderChange} />
              </p>
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
