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
function testo(name,params){
  params = JSON.parse(params);
  var event = new CustomEvent(name, {"detail": params});
  try {
    window.dispatchEvent(event);
  } catch(e) {
    console.error('Cef3DAPI.DispatchCustomEvent failed:' + name + ', handler exception: ' + e.message);
  }
}

function setCppProperty(name,property) {
  Cef3D.Constants.set(name,property);
}

var Cef3D = Cef3D || {};

Cef3D.ComponentMap = new Map();
Cef3D.Constants = new Map();

Cef3D.OnDomReady = function() {
  native function OnDomReady();

  OnDomReady();



};

Cef3D.SendDomEvent = function(callback,s,type,v) {
  native function SendDomEvent();

  SendDomEvent(callback,s,type,v);
};

Cef3D.RegisterComponent = function(name,component) {
  Cef3D.ComponentMap.set(name,component);
}
