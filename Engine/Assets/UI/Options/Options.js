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


var Router = window.ReactRouter;
var BrowserHistory = Router.BrowserHistory;
var Route = Router.Route;

var Options = React.createClass({
  render: function() {
    return(<div className="container">
              <div className="leftPanel">
                <div>Yume Engine Options Panel</div>
                <div className="navigation">
                  <ul>
                    <li className="active"><ReactRouter.Link to="GIOptions">Global Illumination</ReactRouter.Link></li>
                    <li><ReactRouter.Link to="PostProcessing">Post Processing</ReactRouter.Link></li>
                    <li><a href="#">Global Illumination</a></li>
                  </ul>
                </div>
              </div>
          </div>)
  }
});

const YumeRouter = React.createClass({
  render: function() {
    return(
    <div><Options /><div>{this.props.children}</div></div>
  )
  }
});

const GIOptions = React.createClass({
  render: function() {
    return(
      <div> GI Options </div>
    )
  }
})

const PostProcessingOptions = React.createClass({
  render: function() {
    return(
      <div> Post Processing Options </div>
    )
  }
})

var Root = React.createClass({
  render: function() {
    return(<div>
      <ReactRouter.Router>
      <ReactRouter.Route path="/" component={YumeRouter}>
        <ReactRouter.Route name="GIOptions" path="GIOptions" component={GIOptions}/>
        <ReactRouter.Route name="PostProcessing" path="PostProcessing" component={PostProcessingOptions}/>
      </ReactRouter.Route>
    </ReactRouter.Router></div>
    );
  }
});

ReactDOM.render(
  React.createElement(Root),
  document.getElementById('main')
);
