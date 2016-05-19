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

var GIOptionsPanel = require('./GIOptions');
var PostProcessingOptions = require('./PostProcessing');
var OptionsPanel = require('./MainOptions');

var Router = window.ReactRouter;
var BrowserHistory = Router.BrowserHistory;
var Route = Router.Route;
var History = Router.History;
var Tools = require('./Tools');

var YumeCef;
if(typeof Cef3D === "undefined") {
  YumeCef = Tools.Cef3D;
  console.log("Creating Mock");
}
else {
  YumeCef = Cef3D;
}


var Options = React.createClass({
  componentDidMount: function() {
    $(".navigation ul li").click(function() {
      $("div.navigation ul li").removeClass("active");
      $(this).toggleClass("active");
    });
  },
  render: function() {
    return(<div className="leftPanel">
                <div className="navigation">
                  <ul>
                    <li className="active" id="testo"><ReactRouter.Link to="Home">General</ReactRouter.Link></li>
                    <li><ReactRouter.Link to="GIOptions">Global Illumination</ReactRouter.Link></li>
                    <li><ReactRouter.Link to="PostProcessing">Post Processing</ReactRouter.Link></li>
                  </ul>
                </div>
              </div>)
  }
});

//<div className="titleBar">
//  Yume Engine Options Panel
//</div>
//<div>
//  <Options />
//</div>
const SliderMenu = React.createClass({
  currentTab:"General",
  onLeftClick: function(e) {
    console.log("Left");
  },
  onRightClick: function(e) {
    console.log("Right");
  },
  render: function() {
    return(
      <div>
        <div className="sliderMenuText">
          <ReactRouter.Link to="GIOptions"><i className="fa fa-arrow-left" id="sliderMenuLeftArrow" aria-hidden="true"></i></ReactRouter.Link>
          <ReactRouter.Link to="Home">General Options</ReactRouter.Link>
          <ReactRouter.Link to="PostProcessing"><i className="fa fa-arrow-right" id="sliderMenuRightArrow" aria-hidden="true"></i></ReactRouter.Link>
          </div>
      </div>)
  }
});
const YumeRouter = React.createClass({
  componentDidMount: function() {

  },
  render: function() {
    return(<div>
    <div className="main">
      <SliderMenu />
      <div className="testcontainer">
        <div className="menuContent">
          {this.props.children}
        </div>
      </div>
    </div>
  </div>
  )
  }
});

var Root = React.createClass({
  render: function() {
    return(
      <ReactRouter.Router>
      <ReactRouter.Route component={YumeRouter}>
        <ReactRouter.Route name="Home" path="Home" component={OptionsPanel}/>
        <ReactRouter.Route name="GIOptions" path="GIOptions" component={GIOptionsPanel}/>
        <ReactRouter.Route name="PostProcessing" path="PostProcessing" component={PostProcessingOptions}/>
      </ReactRouter.Route>
      <ReactRouter.Redirect from="/" to="Home" />
    </ReactRouter.Router>
    );
  }
});

document.addEventListener("DOMContentLoaded", function() {

  YumeCef.OnDomReady();
}, false);

ReactDOM.render(
  React.createElement(Root),
  document.getElementById('main')
);
