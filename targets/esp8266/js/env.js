var mainPage = "espruino from linkgo.io, good night";
var wifi = require("wifi")
var esp = require("ESP8266")
var http = require("http")

function onPage(req, res) {
  var rurl = url.parse(req.url,true);
  if (rurl.pathname=="/") {
    res.writeHead(200, {'Content-Type': 'text/html'});
    res.end(mainPage);
  } else if (rurl.pathname=="/cmd") {
    res.writeHead(200, {'Content-Type': 'text/plain'});
    var value = "";
    console.log(rurl);
    if (rurl.query && rurl.query.eval)
      value = eval(rurl.query.eval);
    res.end(value);
  } else {
    res.writeHead(404, {'Content-Type': 'text/plain'});
    res.end("Not Found.");
  }
}
