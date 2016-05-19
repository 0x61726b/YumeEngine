var Cef3DTools = { }

Cef3DTools.GetId = function(element)
{
  return $(element.target).attr("id");
}

Cef3DTools.SetValue = function(element,value) {
  $("#" + element).html(value);
}
Cef3DTools.SetElementValue = function(element,value,type) {
  var type = "slider";
  $("#" + type + element).prop("value",value);
}
Cef3DTools.SetCheckbox = function(element,value) {
  $("#" + "switch" + element).prop("checked",value);
}


var Cef3D = Cef3D || {};
Cef3D.Constants = new Map()
Cef3D.Constants.set("lpvFluxValue",3.5);
Cef3D.Constants.set("exposureSpeedValue",1.5);
Cef3D.Constants.set("lpvScaleValue",1.5);
Cef3D.Constants.set("switchBloom",true);
Cef3D.RegisterComponent = function(f,f1)
{

}
Cef3D.OnDomReady = function()
{

}

Cef3D.SendDomEvent = function(f1,f2,f3,f4)
{

}
module.exports = {
  Cef3D: Cef3D,
  Cef3DTools: Cef3DTools
}
