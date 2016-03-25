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
var Overlay = React.createClass( {
  fps:0,
  ms:0,
  sampleName:"Sample Name",
  totalMemoryUsage:0,
  render: function() {
    return (
      <div id="topLeftInfo">
        <div id="sampleName">
          {this.sampleName}
        </div>
        <div id="fps">
          FPS: {this.fps} ({this.ms} ms)
        </div>
        <div id="totalMemory">
          Memory Usage: {this.totalMemoryUsage}
        </div>
      </div>
    );
  }
});

ReactDOM.render(
  React.createElement(Overlay),
  document.getElementById('main')
);
